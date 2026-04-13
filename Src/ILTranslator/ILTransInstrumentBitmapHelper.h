#pragma once
#include <map>
#include <string>
#include <vector>
#include "../ILBasic/ILObject.h"

class ILFunction;

class ILTransInstrumentBitmapHelper {
	public:
		static const std::string AlwaysTrueCondDesc;

		static std::map<std::string, int> munitIndexMap;
		static int munitBitmapCount;
		static std::map<int, std::string> munitDesc;

		static std::map<std::string, int> branchIndexMap;
		static int branchBitmapCount;
		static std::map<int, std::string> branchDesc;
        static std::map<std::string, int> branchCoverFunctionCoverCount;
		static std::map<int, std::string> branchCoverNameForCBMC;
        static std::map<int, int> branchCoverCallDepth;
		static std::map<const void*, int> branchMapId; //???ILBranch??Statement???????Id
        static std::map<int, std::vector<void*>> branchCoverDomain;
        static std::map<int, int> branchCoverParent;
        static std::map<int, int> branchCoverBrother;

		static std::map<std::string, int> condIndexMap;
		static int condBitmapCount;
		static std::map<int, std::string> condDesc;

		static std::map<std::string, int> decIndexMap;
		static int decBitmapCount;
		static std::map<int, std::string> decDesc;

		static std::map<std::string, int> mcdcIndexMap;
		static int mcdcBitmapCount;
		static std::map<int, int> mcdcConditionNumInDecision;
		static std::map<int, std::string> mcdcConditionDescInDecision;
		// ????????? MCDC decision ???????????????????????????????????
		static std::map<int, std::string> mcdcDecisionExpressionStr;
		static ILFunction* mcdcMetadataContextFn;
		static std::map<int, std::string> mcdcDecisionFunctionName;
		static std::map<int, std::string> mcdcDecisionFileName;
		static void recordMcdcDecisionSite(int bid);

		static void PrintDescToJson(std::string jsonFilePath);
		static void clearMetadataAuxiliaryMaps();

	private:
		static void PrintDescToJson(std::map<int, std::string>& desc, std::string& descStr);

};






