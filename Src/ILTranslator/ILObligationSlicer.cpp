#include "ILObligationSlicer.h"
#include "ILObligationSlice.h"
#include "ILTransInstrumentBitmapHelper.h"

#include "../ILBasic/ILParser.h"
#include "../ILBasic/ILFile.h"
#include "../ILBasic/ILFunction.h"
#include "../ILBasic/ILSchedule.h"
#include "../ILBasic/ILBranch.h"
#include "../ILBasic/ILCalculate.h"
#include "../ILBasic/ILObject.h"
#include "../ILBasic/Statement.h"
#include "../ILBasic/StmtIf.h"
#include "../ILBasic/StmtElseIf.h"
#include "../ILBasic/StmtWhile.h"
#include "../ILBasic/StmtFor.h"
#include "../ILBasic/StmtSwitch.h"
#include "../ILBasic/StmtCase.h"
#include "../ILBasic/StmtDefault.h"

#include "../Common/Utility.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>

using namespace std;

static string trimWs(const string& s)
{
	size_t a = 0, b = s.size();
	while (a < b && isspace(static_cast<unsigned char>(s[a])))
		++a;
	while (b > a && isspace(static_cast<unsigned char>(s[b - 1])))
		--b;
	return s.substr(a, b - a);
}

static string extractFunctionFromCbmcLabel(const string& label)
{
	const string key = ".coverage.";
	size_t p = label.find(key);
	if (p != string::npos && p > 0)
		return label.substr(0, p);
	return label;
}

static string resolveFileDisplayName(ILFunction* fn)
{
	if (!fn)
		return "";
	ILFile* f = fn->getContainerILFile();
	if (!f)
		return "";
	return f->name + (f->isHeadFile ? string(".h") : string(".cpp"));
}

static void collectInputsForFunction(ILFunction* fn, vector<string>& out)
{
	out.clear();
	if (!fn)
		return;
	for (ILInput* in : fn->inputs) {
		if (in)
			out.push_back(in->name);
	}
	sort(out.begin(), out.end());
	out.erase(unique(out.begin(), out.end()), out.end());
}

static string inferObligationType(const string& desc)
{
	if (desc.rfind("positive branch:", 0) == 0)
		return "branch_true_path";
	if (desc.rfind("switch branch:", 0) == 0)
		return "switch_case";
	if (desc.rfind("negative", 0) == 0)
		return "branch_false_or_post";
	return "unknown";
}

static void walkStatementsForKey(const Statement* st, const void* key, const ILBranch** outBr,
	const Statement** outSt);

static void walkScheduleNodes(const vector<ILScheduleNode*>& nodes, const void* key, const ILBranch** outBr,
	const Statement** outSt)
{
	for (ILScheduleNode* n : nodes) {
		if (!n)
			continue;
		if (n->objType == ILObject::TypeBranch) {
			ILBranch* b = static_cast<ILBranch*>(n);
			if (static_cast<const void*>(b) == key)
				*outBr = b;
			walkScheduleNodes(b->scheduleNodes, key, outBr, outSt);
		} else if (n->objType == ILObject::TypeCalculate) {
			ILCalculate* c = static_cast<ILCalculate*>(n);
			walkStatementsForKey(&c->statements, key, outBr, outSt);
		}
	}
}

static void walkStatementsForKey(const Statement* st, const void* key, const ILBranch** outBr,
	const Statement** outSt)
{
	if (!st)
		return;
	if (static_cast<const void*>(st) == key)
		*outSt = st;
	for (Statement* ch : st->childStatements)
		walkStatementsForKey(ch, key, outBr, outSt);
}

static void resolveBranchMapKeyInParser(const ILParser* parser, const void* key, const ILBranch** outBr,
	const Statement** outSt)
{
	*outBr = nullptr;
	*outSt = nullptr;
	if (!parser || !key)
		return;
	for (ILFunction* fn : parser->allFunctions) {
		if (fn && fn->schedule)
			walkScheduleNodes(fn->schedule->scheduleNodes, key, outBr, outSt);
		if (*outBr || *outSt)
			return;
	}
}

static string buildStatementParentChain(const Statement* st)
{
	string parts;
	const Statement* p = st ? st->parentStatement : nullptr;
	while (p) {
		if (!parts.empty())
			parts = " / " + parts;
		parts = Statement::statementTypeName[p->type] + parts;
		p = p->parentStatement;
	}
	return parts.empty() ? string("(no parent chain)") : parts;
}

static string ilBranchContextLine(const ILBranch* br)
{
	if (!br || !br->condition)
		return "ILBranch (no condition)";
	return string("ILBranch type=") + to_string(static_cast<int>(br->type)) + " cond=" + br->condition->expressionStr;
}

static string statementAnchorLine(const Statement* st)
{
	if (!st)
		return "Statement?";
	if (st->type == Statement::If) {
		const StmtIf* s = static_cast<const StmtIf*>(st);
		if (s->condition)
			return string("StmtIf cond=") + s->condition->expressionStr;
	}
	if (st->type == Statement::ElseIf) {
		const StmtElseIf* s = static_cast<const StmtElseIf*>(st);
		if (s->condition)
			return string("StmtElseIf cond=") + s->condition->expressionStr;
	}
	if (st->type == Statement::While) {
		const StmtWhile* s = static_cast<const StmtWhile*>(st);
		if (s->condition)
			return string("StmtWhile cond=") + s->condition->expressionStr;
	}
	if (st->type == Statement::For) {
		const StmtFor* s = static_cast<const StmtFor*>(st);
		if (s->condition)
			return string("StmtFor cond=") + s->condition->expressionStr;
	}
	return string("Statement type=") + Statement::statementTypeName[st->type];
}

bool ILObligationSlicer::parseHybridSliceRequestFile(const string& path, vector<int>& outIndices)
{
	outIndices.clear();
	if (!isFileExist(path))
		return false;
	const string j = readFile(path);
	if (j.empty())
		return false;
	const string key = "\"uncovered_branch_bitmap_indices\":[";
	size_t p = j.find(key);
	if (p == string::npos)
		return false;
	p += key.size();
	while (p < j.size() && isspace(static_cast<unsigned char>(j[p])))
		++p;
	while (p < j.size() && j[p] != ']') {
		while (p < j.size() && (isspace(static_cast<unsigned char>(j[p])) || j[p] == ','))
			++p;
		if (p < j.size() && j[p] == ']')
			break;
		size_t q = p;
		if (p < j.size() && j[p] == '-')
			++q;
		while (q < j.size() && isdigit(static_cast<unsigned char>(j[q])))
			++q;
		if (q == p)
			return false;
		outIndices.push_back(stringToInt(j.substr(p, q - p)));
		p = q;
	}
	sort(outIndices.begin(), outIndices.end());
	outIndices.erase(unique(outIndices.begin(), outIndices.end()), outIndices.end());
	return !outIndices.empty();
}

bool ILObligationSlicer::parseBitmapIndexListArg(const string& arg, vector<int>& outIndices)
{
	outIndices.clear();
	if (trimWs(arg).empty())
		return false;
	if (arg.find(',') != string::npos) {
		vector<string> parts = stringSplit(arg, ",");
		for (string& p : parts) {
			p = trimWs(p);
			if (p.empty())
				continue;
			outIndices.push_back(stringToInt(p));
		}
	} else if (isFileExist(arg)) {
		ifstream f(arg.c_str());
		string line;
		while (getline(f, line)) {
			line = trimWs(line);
			if (line.empty() || line[0] == '#')
				continue;
			outIndices.push_back(stringToInt(line));
		}
	} else {
		outIndices.push_back(stringToInt(trimWs(arg)));
	}
	sort(outIndices.begin(), outIndices.end());
	outIndices.erase(unique(outIndices.begin(), outIndices.end()), outIndices.end());
	return !outIndices.empty();
}


static bool findJsonIntFieldKeyPosition(const string& j, const char* key, int desiredValue, size_t& outPos)
{
	const string prefix = string("\"") + key + "\":";
	size_t pos = 0;
	while ((pos = j.find(prefix, pos)) != string::npos) {
		size_t numStart = pos + prefix.size();
		while (numStart < j.size() && isspace(static_cast<unsigned char>(j[numStart])))
			++numStart;
		size_t numEnd = numStart;
		if (numStart < j.size() && j[numStart] == '-')
			++numEnd;
		while (numEnd < j.size() && isdigit(static_cast<unsigned char>(j[numEnd])))
			++numEnd;
		if (numEnd == numStart) {
			++pos;
			continue;
		}
		const int v = stringToInt(j.substr(numStart, numEnd - numStart));
		if (v != desiredValue) {
			++pos;
			continue;
		}
		outPos = pos;
		return true;
	}
	return false;
}

static bool braceExtractJsonObjectContaining(const string& j, size_t innerPos, string& outObj)
{
	outObj.clear();
	int depth = 0;
	size_t start = string::npos;
	for (size_t i = innerPos; i-- > 0;) {
		if (j[i] == '}') {
			depth++;
		} else if (j[i] == '{') {
			if (depth == 0) {
				start = i;
				break;
			}
			depth--;
		}
	}
	if (start == string::npos)
		return false;
	depth = 0;
	size_t end = string::npos;
	for (size_t i = start; i < j.size(); ++i) {
		if (j[i] == '{')
			depth++;
		else if (j[i] == '}') {
			depth--;
			if (depth == 0) {
				end = i + 1;
				break;
			}
		}
	}
	if (end == string::npos || end <= start)
		return false;
	outObj = j.substr(start, end - start);
	return true;
}

static void parseJsonStringArrayField(const string& blob, const string& field, vector<string>& out)
{
	out.clear();
	const string pat = "\"" + field + "\":[";
	size_t p = blob.find(pat);
	if (p == string::npos)
		return;
	p += pat.size();
	while (p < blob.size() && isspace(static_cast<unsigned char>(blob[p])))
		++p;
	while (p < blob.size() && blob[p] != ']') {
		while (p < blob.size() && (blob[p] == ',' || isspace(static_cast<unsigned char>(blob[p]))))
			++p;
		if (p < blob.size() && blob[p] == ']')
			break;
		if (p >= blob.size() || blob[p] != '"')
			break;
		++p;
		const size_t q = blob.find('"', p);
		if (q == string::npos)
			break;
		out.push_back(blob.substr(p, q - p));
		p = q + 1;
	}
}

bool ILObligationSlicer::tryExtractBranchRelationSnippet(const string& branchRelationJson, int branchId,
	string& outSnippet)
{
	if (branchRelationJson.empty())
		return false;
	size_t pos = 0;
	if (!findJsonIntFieldKeyPosition(branchRelationJson, "branch_id", branchId, pos))
		return false;
	return braceExtractJsonObjectContaining(branchRelationJson, pos, outSnippet);
}

static bool extractObligationSnippet(const string& oblJson, int bitmapIndex, string& outSnippet)
{
	if (oblJson.empty())
		return false;
	size_t pos = 0;
	if (!findJsonIntFieldKeyPosition(oblJson, "bitmap_index", bitmapIndex, pos))
		return false;
	return braceExtractJsonObjectContaining(oblJson, pos, outSnippet);
}

static void buildSliceText(const ILObligationSlice& s, ostringstream& out)
{
	out << "=== Obligation slice (bitmap_index=" << s.bitmap_index << ") ===\n";
	out << "obligation_type: " << s.obligation_type << "\n";
	out << "function_name: " << s.function_name << "\n";
	out << "file_name: " << s.file_name << "\n";
	out << "cbmc_property_label: " << s.cbmc_property_label << "\n";
	out << "condition_text (instrumentation desc): " << s.condition_text << "\n";
	out << "IL anchor: " << s.il_anchor_kind << "\n";
	out << "control_context_il: " << s.control_context_il << "\n";
	out << "related_inputs: ";
	for (size_t i = 0; i < s.related_inputs.size(); ++i) {
		if (i) out << ", ";
		out << s.related_inputs[i];
	}
	out << "\nread_globals: ";
	for (size_t i = 0; i < s.read_globals.size(); ++i) {
		if (i) out << ", ";
		out << s.read_globals[i];
	}
	out << "\nwrite_globals: ";
	for (size_t i = 0; i < s.write_globals.size(); ++i) {
		if (i) out << ", ";
		out << s.write_globals[i];
	}
	out << "\n--- obligation_map.json (snippet) ---\n";
	out << s.obligation_map_json_snippet << "\n";
	out << "--- branch_relation.json (snippet) ---\n";
	out << s.branch_relation_json_snippet << "\n";
}

bool ILObligationSlicer::emitSlicesForBitmapIndices(const ILParser* parser, const vector<int>& indices,
	const string& obligationMapJsonPath, const string& branchRelationJsonPath, const string& outputDir)
{
	if (!parser || indices.empty())
		return false;
	const string oblJson = isFileExist(obligationMapJsonPath) ? readFile(obligationMapJsonPath) : string();
	const string brJson = isFileExist(branchRelationJsonPath) ? readFile(branchRelationJsonPath) : string();
	const string sliceDir = outputDir + "/Slices";
	createFolder(sliceDir);
	ostringstream manifest;
	manifest << "# obligation slices generated next to hybrid/state-search translate\n";
	for (int idx : indices) {
		if (idx < 0 || idx >= ILTransInstrumentBitmapHelper::branchBitmapCount) {
			manifest << "# skip invalid bitmap_index " << idx << "\n";
			continue;
		}
		const void* mapKey = nullptr;
		for (const auto& e : ILTransInstrumentBitmapHelper::branchMapId) {
			if (e.second == idx) {
				mapKey = e.first;
				break;
			}
		}
		ILObligationSlice s;
		s.bitmap_index = idx;
		auto dit = ILTransInstrumentBitmapHelper::branchDesc.find(idx);
		if (dit != ILTransInstrumentBitmapHelper::branchDesc.end())
			s.condition_text = dit->second;
		auto cit = ILTransInstrumentBitmapHelper::branchCoverNameForCBMC.find(idx);
		if (cit != ILTransInstrumentBitmapHelper::branchCoverNameForCBMC.end())
			s.cbmc_property_label = cit->second;
		s.obligation_type = inferObligationType(s.condition_text);
		s.function_name = extractFunctionFromCbmcLabel(s.cbmc_property_label);

		const ILBranch* br = nullptr;
		const Statement* st = nullptr;
		resolveBranchMapKeyInParser(parser, mapKey, &br, &st);
		ILFunction* fn = nullptr;
		if (br)
			fn = br->getContainerILFunction();
		else if (st)
			fn = st->getContainerILFunction();
		s.file_name = resolveFileDisplayName(fn);
		collectInputsForFunction(fn, s.related_inputs);

		if (br) {
			s.il_anchor_kind = "ILBranch";
			s.control_context_il = ilBranchContextLine(br);
			string refp;
			for (ILRef* r : br->refs) {
				if (r)
					refp += r->path + " ";
			}
			if (!refp.empty())
				s.control_context_il += " | refs: " + trimWs(refp);
		} else if (st) {
			s.il_anchor_kind = "Statement";
			s.control_context_il = statementAnchorLine(st) + " | parents: " + buildStatementParentChain(st);
		} else {
			s.il_anchor_kind = mapKey ? "unknown" : "no_map_key";
			s.control_context_il =
				"(Could not resolve branchMapId pointer in IL tree; metadata-only below.)";
		}

		extractObligationSnippet(oblJson, idx, s.obligation_map_json_snippet);
		tryExtractBranchRelationSnippet(brJson, idx, s.branch_relation_json_snippet);
		parseJsonStringArrayField(s.branch_relation_json_snippet, "read_globals", s.read_globals);
		parseJsonStringArrayField(s.branch_relation_json_snippet, "write_globals", s.write_globals);

		ostringstream body;
		buildSliceText(s, body);
		s.slice_text = body.str();

		const string outPath = sliceDir + "/slice_" + to_string(idx) + ".txt";
		writeFile(outPath, s.slice_text);
		manifest << idx << "\t" << outPath << "\n";
	}
	const string manPath = sliceDir + "/slice_manifest.txt";
	writeFile(manPath, manifest.str());
	appendFile(outputDir + "/log_slice.txt", string("[ILObligationSlicer] wrote ") + manPath + "\n");
	return true;
}
