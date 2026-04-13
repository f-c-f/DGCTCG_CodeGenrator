#include "ILTransInstrumentBitmapHelper.h"
#include "../ILBasic/ILFunction.h"
#include "../ILBasic/ILFile.h"
#include "../Common/Utility.h"
#include <algorithm> 

const std::string ILTransInstrumentBitmapHelper::AlwaysTrueCondDesc = "@etrue";

std::map<std::string, int> ILTransInstrumentBitmapHelper::munitIndexMap;
int ILTransInstrumentBitmapHelper::munitBitmapCount = 0;
std::map<int, std::string> ILTransInstrumentBitmapHelper::munitDesc;

std::map<std::string, int> ILTransInstrumentBitmapHelper::branchIndexMap;
int ILTransInstrumentBitmapHelper::branchBitmapCount = 0;
std::map<int, std::string> ILTransInstrumentBitmapHelper::branchDesc;
std::map<std::string, int> ILTransInstrumentBitmapHelper::branchCoverFunctionCoverCount;
std::map<int, std::string> ILTransInstrumentBitmapHelper::branchCoverNameForCBMC;
std::map<int, int> ILTransInstrumentBitmapHelper::branchCoverCallDepth;
std::map<const void*, int> ILTransInstrumentBitmapHelper::branchMapId;

std::map<int, std::vector<void*>> ILTransInstrumentBitmapHelper::branchCoverDomain;
std::map<int, int> ILTransInstrumentBitmapHelper::branchCoverParent;
std::map<int, int> ILTransInstrumentBitmapHelper::branchCoverBrother;

std::map<std::string, int> ILTransInstrumentBitmapHelper::condIndexMap;
int ILTransInstrumentBitmapHelper::condBitmapCount = 0;
std::map<int, std::string> ILTransInstrumentBitmapHelper::condDesc;

std::map<std::string, int> ILTransInstrumentBitmapHelper::decIndexMap;
int ILTransInstrumentBitmapHelper::decBitmapCount = 0;
std::map<int, std::string> ILTransInstrumentBitmapHelper::decDesc;

std::map<std::string, int> ILTransInstrumentBitmapHelper::mcdcIndexMap;
int ILTransInstrumentBitmapHelper::mcdcBitmapCount = 0;
std::map<int, int> ILTransInstrumentBitmapHelper::mcdcConditionNumInDecision;
std::map<int, std::string> ILTransInstrumentBitmapHelper::mcdcConditionDescInDecision;
std::map<int, std::string> ILTransInstrumentBitmapHelper::mcdcDecisionExpressionStr;
ILFunction* ILTransInstrumentBitmapHelper::mcdcMetadataContextFn = nullptr;
std::map<int, std::string> ILTransInstrumentBitmapHelper::mcdcDecisionFunctionName;
std::map<int, std::string> ILTransInstrumentBitmapHelper::mcdcDecisionFileName;

void ILTransInstrumentBitmapHelper::recordMcdcDecisionSite(int bid)
{
	ILFunction* fn = mcdcMetadataContextFn;
	if (!fn)
		return;
	mcdcDecisionFunctionName[bid] = fn->name;
	ILFile* f = fn->getContainerILFile();
	if (f)
		mcdcDecisionFileName[bid] = f->name + (f->isHeadFile ? std::string(".h") : std::string(".cpp"));
}

void ILTransInstrumentBitmapHelper::clearMetadataAuxiliaryMaps()
{
	mcdcDecisionExpressionStr.clear();
	mcdcMetadataContextFn = nullptr;
	mcdcDecisionFunctionName.clear();
	mcdcDecisionFileName.clear();
}

void ILTransInstrumentBitmapHelper::PrintDescToJson(std::map<int, std::string>& desc, std::string& descStr)
{
	descStr += "{\n";
	std::map<int, std::string>::iterator iter = desc.begin();
	std::map<int, std::string>::iterator iter_end = desc.end();
	while (iter != iter_end) {
		descStr += "\"" + std::to_string(iter->first) + "\"";
		descStr += ":";
		descStr += "\"" + iter->second + "\"";
		if (std::next(iter) != iter_end) {
			descStr += ",";
		}
		iter++;
	}
	descStr += "}\n";
}

void ILTransInstrumentBitmapHelper::PrintDescToJson(std::string jsonFilePath)
{
	std::string descStr = "";
	descStr += "{\n";

	descStr += "\"munit coverage description\":";
	PrintDescToJson(ILTransInstrumentBitmapHelper::munitDesc, descStr);
	descStr += ",\n";
	descStr += "\"branch coverage description\":";
	PrintDescToJson(ILTransInstrumentBitmapHelper::branchDesc, descStr);
	descStr += ",\n";
	descStr += "\"condition coverage description\":";
	PrintDescToJson(ILTransInstrumentBitmapHelper::condDesc, descStr);
	descStr += ",\n";
	descStr += "\"decision coverage description\":";
	PrintDescToJson(ILTransInstrumentBitmapHelper::decDesc, descStr);
	descStr += ",\n";
	descStr += "\"mcdc coverage description\":";
	PrintDescToJson(ILTransInstrumentBitmapHelper::mcdcConditionDescInDecision, descStr);
	
	descStr += "}\n";

	writeFile(jsonFilePath, descStr);
}


