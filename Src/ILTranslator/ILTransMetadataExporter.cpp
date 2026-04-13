#include "ILTransMetadataExporter.h"

#include "ILTransInstrumentBitmapHelper.h"
#include "ILTranslator.h"
#include "ILTranslatorForTCGHybrid.h"
#include "ILTranslatorForTCGStateSearch.h"
#include "../Common/Utility.h"
#include <cstdio>
#include "../ILBasic/ILFile.h"
#include "../ILBasic/ILFunction.h"
#include "../ILBasic/ILInput.h"
#include "../ILBasic/ILParser.h"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <map>
#include <set>
#include <sstream>

using namespace std;

namespace
{

map<int, set<string>> g_branchReadGlobals;
map<int, set<string>> g_branchWriteGlobals;

string jsonEscape(const string& s)
{
	string o;
	o.reserve(s.size() + 8);
	for (char c : s) {
		switch (c) {
		case '\\': o += "\\\\"; break;
		case '"': o += "\\\""; break;
		case '\b': o += "\\b"; break;
		case '\f': o += "\\f"; break;
		case '\n': o += "\\n"; break;
		case '\r': o += "\\r"; break;
		case '\t': o += "\\t"; break;
		default:
			if (static_cast<unsigned char>(c) < 0x20) {
				char buf[8];
				snprintf(buf, sizeof(buf), "\\u%04x", static_cast<unsigned char>(c));
				o += buf;
			} else {
				o += c;
			}
		}
	}
	return o;
}

string csvEscapeField(const string& s)
{
	bool needQuote = s.find_first_of(",\"\r\n") != string::npos;
	string out;
	if (needQuote)
		out += '"';
	for (char c : s) {
		if (c == '"')
			out += "\"\"";
		else
			out += c;
	}
	if (needQuote)
		out += '"';
	return out;
}

void emitStringArray(ostringstream& out, const vector<string>& items)
{
	out << "[";
	for (size_t i = 0; i < items.size(); ++i) {
		if (i) out << ",";
		out << "\"" << jsonEscape(items[i]) << "\"";
	}
	out << "]";
}

void emitIntArray(ostringstream& out, const vector<int>& items)
{
	out << "[";
	for (size_t i = 0; i < items.size(); ++i) {
		if (i) out << ",";
		out << items[i];
	}
	out << "]";
}

string inferObligationType(const string& desc)
{
	if (desc.rfind("positive branch:", 0) == 0)
		return "branch_true_path";
	if (desc.rfind("switch branch:", 0) == 0)
		return "switch_case";
	if (desc.rfind("negative", 0) == 0)
		return "branch_false_or_post";
	return "unknown";
}

// 供 LLM 阅读：义务类型一句话说明（与 obligation_type 机器码并存）
string obligationTypeHintForLlm(const string& oblType)
{
	if (oblType == "branch_true_path")
		return "Instrumented as the taken then-path / case-true side of a branch.";
	if (oblType == "switch_case")
		return "Instrumented as a specific switch arm.";
	if (oblType == "branch_false_or_post")
		return "Else-side, negated-path, or post-block coverage point (paired with a primary branch).";
	if (oblType == "unknown")
		return "Branch obligation; see condition_text for wording.";
	return "Branch coverage obligation.";
}

// 分支义务 condition_text：去掉内部前缀，改成自然语言
string humanizeBranchConditionForLlm(const string& raw)
{
	static const char kPos[] = "positive branch:";
	static const char kSw[] = "switch branch:";
	if (raw.rfind(kPos, 0) == 0) {
		string inner = stringTrim(raw.substr(strlen(kPos)));
		return "Then / taken path: " + inner;
	}
	if (raw.rfind(kSw, 0) == 0) {
		string inner = stringTrim(raw.substr(strlen(kSw)));
		return "Switch arm: " + inner;
	}
	if (raw.rfind("negative", 0) == 0)
		return "Else / complement or post-point: " + raw;
	return raw;
}

// 条件/决策位图描述：把 "expr==true" 拼接改成可读的「谓词 + 记录的真假侧」
void splitPredicateAndOutcomeSuffix(const string& raw, string& predicate, string& outcome)
{
	predicate = raw;
	outcome.clear();
	if (stringEndsWith(raw, "==false")) {
		predicate = stringTrim(raw.substr(0, raw.size() - strlen("==false")));
		outcome = "false";
		return;
	}
	if (stringEndsWith(raw, "==true")) {
		predicate = stringTrim(raw.substr(0, raw.size() - strlen("==true")));
		outcome = "true";
	}
}

string humanizeCondDecDescriptionForLlm(const string& raw)
{
	string pred, outv;
	splitPredicateAndOutcomeSuffix(raw, pred, outv);
	if (!outv.empty()) {
		return "Predicate (" + pred + ") — this slot records when it evaluates to " + outv + ".";
	}
	return raw;
}

string extractFunctionFromCbmcLabel(const string& label)
{
	const string key = ".coverage.";
	size_t p = label.find(key);
	if (p != string::npos && p > 0)
		return label.substr(0, p);
	return label;
}

ILFunction* findILFunctionByName(const ILParser* parser, const string& name)
{
	if (!parser)
		return nullptr;
	for (ILFile* file : parser->files) {
		for (ILFunction* fn : file->functions) {
			if (fn->name == name)
				return fn;
		}
	}
	return nullptr;
}

string resolveFileDisplayName(ILFunction* fn)
{
	if (!fn)
		return "";
	ILFile* f = fn->getContainerILFile();
	if (!f)
		return "";
	return f->name + (f->isHeadFile ? string(".h") : string(".cpp"));
}

void collectRelatedInputs(const ILParser* parser, const string& functionName, vector<string>& outNames)
{
	outNames.clear();
	ILFunction* fn = findILFunctionByName(parser, functionName);
	if (!fn)
		return;
	for (ILInput* in : fn->inputs) {
		if (in)
			outNames.push_back(in->name);
	}
	sort(outNames.begin(), outNames.end());
	outNames.erase(unique(outNames.begin(), outNames.end()), outNames.end());
}

// 与义务表一致：按函数名聚合该函数下所有 branch 槽位上出现过的读/写全局（旁路近似）
void collectRelatedGlobalsForFunction(const string& functionName, vector<string>& outNames)
{
	outNames.clear();
	if (functionName.empty())
		return;
	set<string> acc;
	const int nb = ILTransInstrumentBitmapHelper::branchBitmapCount;
	for (int i = 0; i < nb; ++i) {
		auto lit = ILTransInstrumentBitmapHelper::branchCoverNameForCBMC.find(i);
		if (lit == ILTransInstrumentBitmapHelper::branchCoverNameForCBMC.end())
			continue;
		if (extractFunctionFromCbmcLabel(lit->second) != functionName)
			continue;
		if (g_branchReadGlobals.count(i))
			acc.insert(g_branchReadGlobals[i].begin(), g_branchReadGlobals[i].end());
		if (g_branchWriteGlobals.count(i))
			acc.insert(g_branchWriteGlobals[i].begin(), g_branchWriteGlobals[i].end());
	}
	outNames.assign(acc.begin(), acc.end());
	sort(outNames.begin(), outNames.end());
}

vector<string> splitMcdcAtomStrings(const string& blob)
{
	vector<string> parts = stringSplit(blob, "#");
	vector<string> out;
	for (string p : parts) {
		p = stringTrim(p);
		if (!p.empty())
			out.push_back(p);
	}
	return out;
}

void emitMcdcDecisionsJsonArray(ostringstream& out, const ILParser* parser)
{
	out << "[";
	const int m = ILTransInstrumentBitmapHelper::mcdcBitmapCount;
	for (int bid = 0; bid < m; ++bid) {
		if (bid)
			out << ",";
		string decisionExpr;
		auto deIt = ILTransInstrumentBitmapHelper::mcdcDecisionExpressionStr.find(bid);
		if (deIt != ILTransInstrumentBitmapHelper::mcdcDecisionExpressionStr.end())
			decisionExpr = deIt->second;
		string blob;
		auto blIt = ILTransInstrumentBitmapHelper::mcdcConditionDescInDecision.find(bid);
		if (blIt != ILTransInstrumentBitmapHelper::mcdcConditionDescInDecision.end())
			blob = blIt->second;
		vector<string> atoms = splitMcdcAtomStrings(blob);
		string funcName;
		auto fnIt = ILTransInstrumentBitmapHelper::mcdcDecisionFunctionName.find(bid);
		if (fnIt != ILTransInstrumentBitmapHelper::mcdcDecisionFunctionName.end())
			funcName = fnIt->second;
		string fileName;
		auto flIt = ILTransInstrumentBitmapHelper::mcdcDecisionFileName.find(bid);
		if (flIt != ILTransInstrumentBitmapHelper::mcdcDecisionFileName.end())
			fileName = flIt->second;
		vector<string> inputsForDecision;
		vector<string> globalsForDecision;
		collectRelatedInputs(parser, funcName, inputsForDecision);
		collectRelatedGlobalsForFunction(funcName, globalsForDecision);
		out << "{\"decision_id\":" << bid;
		out << ",\"function_name\":\"" << jsonEscape(funcName) << "\"";
		out << ",\"file_name\":\"" << jsonEscape(fileName) << "\"";
		out << ",\"decision_expr\":\"" << jsonEscape(decisionExpr) << "\"";
		out << ",\"decision_summary\":\"MCDC decision slot " << bid
			<< ": combined boolean expression is in decision_expr; atoms[] lists independent sub-conditions.\"";
		int atomCount = 0;
		auto cnIt = ILTransInstrumentBitmapHelper::mcdcConditionNumInDecision.find(bid);
		if (cnIt != ILTransInstrumentBitmapHelper::mcdcConditionNumInDecision.end())
			atomCount = cnIt->second;
		out << ",\"condition_atom_count\":" << atomCount;
		out << ",\"atoms\":[";
		for (size_t ai = 0; ai < atoms.size(); ++ai) {
			if (ai)
				out << ",";
			string condId = "mcdc_" + to_string(bid) + "_atom_" + to_string(ai);
			out << "{";
			out << "\"cond_id\":\"" << jsonEscape(condId) << "\"";
			out << ",\"atom_index\":" << ai;
			out << ",\"atom_expr\":\"" << jsonEscape(atoms[ai]) << "\"";
			out << ",\"atom_description\":\"Independent boolean atom #" << ai << " inside this decision: "
				<< jsonEscape(atoms[ai]) << "\"";
			out << ",\"function_name\":\"" << jsonEscape(funcName) << "\"";
			out << ",\"file_name\":\"" << jsonEscape(fileName) << "\"";
			out << ",\"related_inputs\":";
			emitStringArray(out, inputsForDecision);
			out << ",\"related_globals\":";
			emitStringArray(out, globalsForDecision);
			out << "}";
		}
		out << "]}";
	}
	out << "]";
}

void emitConditionPairsJsonArray(ostringstream& out)
{
	out << "[";
	const int c = ILTransInstrumentBitmapHelper::condBitmapCount;
	for (int i = 0; i < c; ++i) {
		if (i)
			out << ",";
		string desc;
		auto it = ILTransInstrumentBitmapHelper::condDesc.find(i);
		if (it != ILTransInstrumentBitmapHelper::condDesc.end())
			desc = it->second;
		string pred, outv;
		splitPredicateAndOutcomeSuffix(desc, pred, outv);
		string descLlm = humanizeCondDecDescriptionForLlm(desc);
		out << "{\"cond_index\":" << i;
		out << ",\"predicate\":\"" << jsonEscape(pred) << "\"";
		if (!outv.empty())
			out << ",\"outcome\":\"" << jsonEscape(outv) << "\"";
		out << ",\"description\":\"" << jsonEscape(descLlm) << "\"}";
	}
	out << "]";
}

void emitDecisionPairsJsonArray(ostringstream& out)
{
	out << "[";
	const int d = ILTransInstrumentBitmapHelper::decBitmapCount;
	for (int i = 0; i < d; ++i) {
		if (i)
			out << ",";
		string desc;
		auto it = ILTransInstrumentBitmapHelper::decDesc.find(i);
		if (it != ILTransInstrumentBitmapHelper::decDesc.end())
			desc = it->second;
		string pred, outv;
		splitPredicateAndOutcomeSuffix(desc, pred, outv);
		string descLlm = humanizeCondDecDescriptionForLlm(desc);
		out << "{\"dec_index\":" << i;
		out << ",\"predicate\":\"" << jsonEscape(pred) << "\"";
		if (!outv.empty())
			out << ",\"outcome\":\"" << jsonEscape(outv) << "\"";
		out << ",\"description\":\"" << jsonEscape(descLlm) << "\"}";
	}
	out << "]";
}

void emitBranchDistanceSection(ostringstream& out, const string& keyName, bool isFirstKey,
	const vector<ILTranslatorForTCGStateSearch::BranchDistanceData>& list,
	const map<const void*, int>& branchMap)
{
	if (!isFirstKey)
		out << ",";
	out << "\"" << keyName << "\":[";
	for (size_t i = 0; i < list.size(); ++i) {
		if (i)
			out << ",";
		int branchId = -1;
		auto it = branchMap.find(list[i].branchObject);
		if (it != branchMap.end())
			branchId = it->second;
		out << "{";
		out << "\"distance_id\":" << i;
		out << ",\"branch_id\":" << branchId;
		out << ",\"distance_type\":" << list[i].distanceType;
		out << ",\"var_name\":\"" << jsonEscape(list[i].varName) << "\"";
		out << ",\"compare_op\":\"" << jsonEscape(list[i].distanceOpType) << "\"";
		out << ",\"target_value\":\"" << jsonEscape(list[i].targetValue) << "\"";
		out << "}";
	}
	out << "]";
}

void emitBranchDistanceSectionHybrid(ostringstream& out, const string& keyName, bool isFirstKey,
	const vector<ILTranslatorForTCGHybrid::BranchDistanceData>& list,
	const map<const void*, int>& branchMap)
{
	if (!isFirstKey)
		out << ",";
	out << "\"" << keyName << "\":[";
	for (size_t i = 0; i < list.size(); ++i) {
		if (i)
			out << ",";
		int branchId = -1;
		auto it = branchMap.find(list[i].branchObject);
		if (it != branchMap.end())
			branchId = it->second;
		out << "{";
		out << "\"distance_id\":" << i;
		out << ",\"branch_id\":" << branchId;
		out << ",\"distance_type\":" << list[i].distanceType;
		out << ",\"var_name\":\"" << jsonEscape(list[i].varName) << "\"";
		out << ",\"compare_op\":\"" << jsonEscape(list[i].distanceOpType) << "\"";
		out << ",\"target_value\":\"" << jsonEscape(list[i].targetValue) << "\"";
		out << "}";
	}
	out << "]";
}

void emitArrayDistanceSection(ostringstream& out, const string& keyName, bool isFirstKey,
	const map<string, pair<string, string>>& arrMap)
{
	if (!isFirstKey)
		out << ",";
	out << "\"" << keyName << "\":[";
	size_t i = 0;
	for (const auto& item : arrMap) {
		if (i++)
			out << ",";
		string targetStride = item.second.first + " * sizeof(" + item.second.second + ")";
		out << "{";
		out << "\"distance_id\":" << (i - 1);
		out << ",\"branch_id\":null";
		out << ",\"distance_type\":\"array\"";
		out << ",\"var_name\":\"" << jsonEscape(item.first) << "\"";
		out << ",\"compare_op\":\"\"";
		out << ",\"target_value\":\"" << jsonEscape(targetStride) << "\"";
		out << "}";
	}
	out << "]";
}

void writeDistanceMapJson(const string& path)
{
	ostringstream out;
	out << "{";
	bool first = true;
	const auto& ssList = ILTranslatorForTCGStateSearch::branchDistanceDataList;
	const auto& ssMap = ILTranslatorForTCGStateSearch::branchDistanceBranchObjectList;
	const auto& ssArr = ILTranslatorForTCGStateSearch::arrayDistanceBranchObjectList;
	const auto& hyList = ILTranslatorForTCGHybrid::branchDistanceDataList;
	const auto& hyMap = ILTranslatorForTCGHybrid::branchDistanceBranchObjectList;
	const auto& hyArr = ILTranslatorForTCGHybrid::arrayDistanceBranchObjectList;

	if (!ssList.empty() || !ssArr.empty()) {
		if (!ssList.empty()) {
			emitBranchDistanceSection(out, "branch_value_distances", first, ssList, ssMap);
			first = false;
		}
		if (!ssArr.empty()) {
			emitArrayDistanceSection(out, "array_distances", first, ssArr);
			first = false;
		}
	} else if (!hyList.empty() || !hyArr.empty()) {
		if (!hyList.empty()) {
			emitBranchDistanceSectionHybrid(out, "branch_value_distances", first, hyList, hyMap);
			first = false;
		}
		if (!hyArr.empty()) {
			emitArrayDistanceSection(out, "array_distances", first, hyArr);
			first = false;
		}
	}
	if (first) {
		out << "\"branch_value_distances\":[],\"array_distances\":[]";
	}
	out << "}";
	ofstream f(path, ios::binary);
	if (f)
		f << out.str();
}

void writeObligationMapCsv(const string& path, const ILParser* parser, int n)
{
	ofstream f(path, ios::binary);
	if (!f)
		return;
	f << "bitmap_index,obligation_type,obligation_hint,function_name,file_name,condition_text,branch_depth,call_depth,related_inputs,related_globals,cbmc_property_label\n";
	for (int i = 0; i < n; ++i) {
		string cond = ILTransInstrumentBitmapHelper::branchDesc[i];
		string cbmcLabel = ILTransInstrumentBitmapHelper::branchCoverNameForCBMC[i];
		string funcName = extractFunctionFromCbmcLabel(cbmcLabel);
		ILFunction* fn = findILFunctionByName(parser, funcName);
		string fileName = resolveFileDisplayName(fn);
		string oblType = inferObligationType(cond);
		string oblHint = obligationTypeHintForLlm(oblType);
		int callDepth = 0;
		auto cdIt = ILTransInstrumentBitmapHelper::branchCoverCallDepth.find(i);
		if (cdIt != ILTransInstrumentBitmapHelper::branchCoverCallDepth.end())
			callDepth = cdIt->second;
		int branchDepth = 0;
		auto domIt = ILTransInstrumentBitmapHelper::branchCoverDomain.find(i);
		if (domIt != ILTransInstrumentBitmapHelper::branchCoverDomain.end())
			branchDepth = static_cast<int>(domIt->second.size());
		vector<string> relatedInputs;
		collectRelatedInputs(parser, funcName, relatedInputs);
		string inputsJoined;
		for (size_t k = 0; k < relatedInputs.size(); ++k) {
			if (k) inputsJoined += ";";
			inputsJoined += relatedInputs[k];
		}
		set<string> rg = g_branchReadGlobals[i];
		set<string> wg = g_branchWriteGlobals[i];
		set<string> mergedGlobals = rg;
		mergedGlobals.insert(wg.begin(), wg.end());
		string globalsJoined;
		size_t gi = 0;
		for (const string& g : mergedGlobals) {
			if (gi++) globalsJoined += ";";
			globalsJoined += g;
		}
		f << i << ",";
		f << csvEscapeField(oblType) << ",";
		f << csvEscapeField(oblHint) << ",";
		f << csvEscapeField(funcName) << ",";
		f << csvEscapeField(fileName) << ",";
		f << csvEscapeField(humanizeBranchConditionForLlm(cond)) << ",";
		f << branchDepth << ",";
		f << callDepth << ",";
		f << csvEscapeField(inputsJoined) << ",";
		f << csvEscapeField(globalsJoined) << ",";
		f << csvEscapeField(cbmcLabel) << "\n";
	}
}

} // namespace

void ILTransMetadataExporter::reset()
{
	ILTransInstrumentBitmapHelper::clearMetadataAuxiliaryMaps();
	g_branchReadGlobals.clear();
	g_branchWriteGlobals.clear();
}

void ILTransMetadataExporter::recordBranchRelationPairs(
	const vector<pair<void*, string>>& listBranchReadGlobalVar,
	const vector<pair<void*, string>>& listBranchWriteGlobalVar)
{
	for (const auto& e : listBranchReadGlobalVar) {
		auto it = ILTransInstrumentBitmapHelper::branchMapId.find(e.first);
		if (it == ILTransInstrumentBitmapHelper::branchMapId.end())
			continue;
		g_branchReadGlobals[it->second].insert(e.second);
	}
	for (const auto& e : listBranchWriteGlobalVar) {
		auto it = ILTransInstrumentBitmapHelper::branchMapId.find(e.first);
		if (it == ILTransInstrumentBitmapHelper::branchMapId.end())
			continue;
		g_branchWriteGlobals[it->second].insert(e.second);
	}
}

bool ILTransMetadataExporter::flush(const string& outputDir, const ILParser* parser)
{
	const int n = ILTransInstrumentBitmapHelper::branchBitmapCount;

	ostringstream obligationJson;
	obligationJson << "{\"obligations\":[";

	for (int i = 0; i < n; ++i) {
		if (i)
			obligationJson << ",";
		string cond = ILTransInstrumentBitmapHelper::branchDesc[i];
		string condLlm = humanizeBranchConditionForLlm(cond);
		string cbmcLabel = ILTransInstrumentBitmapHelper::branchCoverNameForCBMC[i];
		string funcName = extractFunctionFromCbmcLabel(cbmcLabel);
		ILFunction* fn = findILFunctionByName(parser, funcName);
		string fileName = resolveFileDisplayName(fn);
		string oblType = inferObligationType(cond);
		string oblHint = obligationTypeHintForLlm(oblType);
		int callDepth = 0;
		{
			auto it = ILTransInstrumentBitmapHelper::branchCoverCallDepth.find(i);
			if (it != ILTransInstrumentBitmapHelper::branchCoverCallDepth.end())
				callDepth = it->second;
		}
		int branchDepth = 0;
		{
			auto it = ILTransInstrumentBitmapHelper::branchCoverDomain.find(i);
			if (it != ILTransInstrumentBitmapHelper::branchCoverDomain.end())
				branchDepth = static_cast<int>(it->second.size());
		}
		vector<string> relatedInputs;
		collectRelatedInputs(parser, funcName, relatedInputs);

		set<string> rg = g_branchReadGlobals[i];
		set<string> wg = g_branchWriteGlobals[i];
		set<string> mergedGlobals = rg;
		mergedGlobals.insert(wg.begin(), wg.end());
		vector<string> relatedGlobals(mergedGlobals.begin(), mergedGlobals.end());

		obligationJson << "{";
		obligationJson << "\"bitmap_index\":" << i << ",";
		obligationJson << "\"obligation_type\":\"" << jsonEscape(oblType) << "\",";
		obligationJson << "\"obligation_hint\":\"" << jsonEscape(oblHint) << "\",";
		obligationJson << "\"function_name\":\"" << jsonEscape(funcName) << "\",";
		obligationJson << "\"file_name\":\"" << jsonEscape(fileName) << "\",";
		obligationJson << "\"condition_text\":\"" << jsonEscape(condLlm) << "\",";
		obligationJson << "\"branch_depth\":" << branchDepth << ",";
		obligationJson << "\"call_depth\":" << callDepth << ",";
		obligationJson << "\"related_inputs\":";
		emitStringArray(obligationJson, relatedInputs);
		obligationJson << ",";
		obligationJson << "\"related_globals\":";
		emitStringArray(obligationJson, relatedGlobals);
		obligationJson << ",";
		obligationJson << "\"cbmc_property_label\":\"" << jsonEscape(cbmcLabel) << "\"";
		obligationJson << "}";
	}
	obligationJson << "],\"mcdc_decisions\":";
	emitMcdcDecisionsJsonArray(obligationJson, parser);
	obligationJson << ",\"condition_pairs\":";
	emitConditionPairsJsonArray(obligationJson);
	obligationJson << ",\"decision_pairs\":";
	emitDecisionPairsJsonArray(obligationJson);
	obligationJson << "}";

	vector<vector<int>> childrenByParent(n);
	for (int j = 0; j < n; ++j) {
		auto it = ILTransInstrumentBitmapHelper::branchCoverParent.find(j);
		if (it == ILTransInstrumentBitmapHelper::branchCoverParent.end())
			continue;
		int p = it->second;
		if (p >= 0 && p < n)
			childrenByParent[p].push_back(j);
	}
	for (int i = 0; i < n; ++i)
		sort(childrenByParent[i].begin(), childrenByParent[i].end());

	ostringstream relationJson;
	relationJson << "{\"branches\":[";
	for (int i = 0; i < n; ++i) {
		if (i)
			relationJson << ",";
		int parent = -1;
		auto pit = ILTransInstrumentBitmapHelper::branchCoverParent.find(i);
		if (pit != ILTransInstrumentBitmapHelper::branchCoverParent.end())
			parent = pit->second;

		vector<string> reads;
		vector<string> writes;
		if (g_branchReadGlobals.count(i))
			reads.assign(g_branchReadGlobals[i].begin(), g_branchReadGlobals[i].end());
		if (g_branchWriteGlobals.count(i))
			writes.assign(g_branchWriteGlobals[i].begin(), g_branchWriteGlobals[i].end());
		sort(reads.begin(), reads.end());
		sort(writes.begin(), writes.end());

		relationJson << "{";
		relationJson << "\"branch_id\":" << i << ",";
		relationJson << "\"read_globals\":";
		emitStringArray(relationJson, reads);
		relationJson << ",";
		relationJson << "\"write_globals\":";
		emitStringArray(relationJson, writes);
		relationJson << ",";
		relationJson << "\"parent_branch\":" << parent << ",";
		relationJson << "\"children_branch\":";
		emitIntArray(relationJson, childrenByParent[i]);
		relationJson << "}";
	}
	relationJson << "]}";

	createFolder(outputDir);
	{
		ofstream fo(outputDir + "/obligation_map.json", ios::binary);
		if (!fo)
			return false;
		fo << obligationJson.str();
	}
	{
		ofstream fr(outputDir + "/branch_relation.json", ios::binary);
		if (!fr)
			return false;
		fr << relationJson.str();
	}
	writeObligationMapCsv(outputDir + "/obligation_map.csv", parser, n);
	writeDistanceMapJson(outputDir + "/distance_map.json");

	g_branchReadGlobals.clear();
	g_branchWriteGlobals.clear();
	return true;
}
