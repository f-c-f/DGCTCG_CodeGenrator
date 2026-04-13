#pragma once
#include <map>
#include <set>
#include <string>
#include <vector>

#include "../ILBasic/Expression.h"

class ILAnalyzeScheduleVariable;
class ILAnalyzeScheduleNode;
class ILAnalyzeSchedule;
class ILObject;
class ILSchedule;
class ILGlobalVariable;
class ILOutput;
class ILInput;
class ILConfig;
class ILFunction;
class ILParser;
class ILConfigParameter;

class ILTransMain
{
public:
    const ILParser* iLParser = nullptr;

    int translate(const ILParser* parser, std::string& code);
    int translateForC(const ILParser* parser, std::string& code);
    int translateForCBMC(const ILParser* parser, std::string& code);
    int translateForTCGStateSearch(const ILParser* parser, std::string& code);
    int translateForLibFuzzer(const ILParser* parser, std::string& code);
    int translateForTCGHybrid(const ILParser* parser, std::string& code);
    int translateForCoverage(const ILParser* parser, std::string& code);
    int translateForSimulator(const ILParser* parser, std::string& code);

	std::string transCodeHeadFile;
	std::string transCodeStructDefine;
	std::string transCodeMacro;
	std::string transCodeTypeDefine;
	std::string transCodeGlobalVariable;
	std::string transCodeUtilityFunction;
	std::string transCodeFunction;

private:

    
    std::string translateMainInitParameter(const ILFunction* mainInitFunction) const;
    std::string translateMainInitFuncionCall(const ILFunction* mainInitFunction) const;
    std::string translateMainExecParameter(const ILFunction* mainExecFunction, int step = 1) const;
    std::string translateMainExecParameterAssignment(const ILFunction* mainExecFunction, int step = 1) const;
    std::string translateMainExecFuncionCall(const ILFunction* mainExecFunction, int step = 1) const;
    std::string translateMainExecOutputPrint(const ILFunction* mainExecFunction);

    std::string translateMainInput(const ILInput* input, std::string perfix) const;
    std::string translateMainOutput(const ILOutput* output, std::string perfix) const;

    int translateMainInstanceGlobalVariable();

    // Only used by TranslatorForC
    int translateForCMainFunction(const ILParser* parser, std::string& code);
    
    // Only used by TranslatorForCBMC
    int translateForCBMCMainFunction(const ILParser* parser, std::string& code);
    int translateForCBMCGlobalVariableResetFunction(const ILParser* parser, std::string& code);
    std::string translateForCBMCGlobalVariableResetFunctionOne(ILGlobalVariable* iLGlobalVariable);
    void translateForCBMCGlobalVariableResetFunctionOne(std::string varName, std::string type, std::vector<int> arraySize, std::string& retCode);
    
    int translateMainFunctionOneEntry(const ILParser* parser, std::string& code) const;
    int translateMainFunctionMultiEntries(const ILParser* parser, std::string& code) const;
    std::string translateMainFunctionInputCover(ILInput* input, int step) const;
    std::string translateMainFunctionInputCoverTraverse(std::string name, std::string type, std::vector<int> arraySize, bool isAddress) const;
    std::string translateMainFunctionParameterCover(const ILConfigParameter* para) const;
    std::string translateMainFunctionParameterCoverTraverse(std::string name, std::string type, std::vector<int> arraySize, bool isAddress) const;
    std::string translateMainFunctionRangeConstraint(int step) const;


    // Only used by TranslatorForTCGStateSearch
    int translateForTCGSSMainFunction(const ILParser* parser, std::string& code);
    int translateForTCGSSHeadFile();
    std::string translateForTCGSSMainStateSearchInit();
    std::string translateForTCGSSMainStateSearchRegistBranch();
    std::string translateForTCGSSMainStateSearchBranchRelation();
    std::string translateForTCGSSMainStateSearchRegistGlobalVariables();
    std::string translateForTCGSSMainStateSearchRegistGlobalVariableOne(ILGlobalVariable* iLGlobalVariable);
    void translateForTCGSSMainStateSearchRegistGlobalVariableOne(std::string varName, std::string type, std::vector<int> arraySize, std::string& retCode);
    
    int translateForTCGSSMainStateSearchRegistBranchRelationsCollectOneSchedule(ILSchedule* iLSchedule, const ILParser* parser, std::vector<std::pair<void*, std::string>>& listBranchWriteGlobalVar, std::vector<std::pair<void*, std::string>>& listBranchReadGlobalVar);
    void translateForTCGSSMainStateSearchRegistBranchRelationsCollectOneScheduleCollectVarMap(ILSchedule* iLSchedule, std::map<std::string, std::string>& globalVariablePointerMap, const std::vector<ILAnalyzeScheduleNode*>* scheduleNodeList, ILAnalyzeSchedule* analyzeSchedule);
    void translateForTCGSSMainStateSearchRegistBranchRelationsCollectOneScheduleBranchVarRead(const std::vector<ILAnalyzeScheduleNode*>& scheduleNodeDataFlowList, std::vector<std::pair<void*, std::string>>& listBranchReadGlobalVar, ILAnalyzeScheduleNode* node, std::map<std::string, std::string>& globalVariablePointerMap);
    void translateForTCGSSMainStateSearchRegistBranchRelationsCollectOneScheduleBranchVarReadGetDefaultList(ILAnalyzeScheduleNode* node, std::vector<const ILAnalyzeScheduleVariable*>& variableReadList);
    void translateForTCGSSMainStateSearchRegistBranchRelationsCollectOneScheduleBranchVarReadFindSource(const std::vector<ILAnalyzeScheduleNode*>& scheduleNodeDataFlowList, int nodeIndex, const ILAnalyzeScheduleVariable* var, std::set<const ILAnalyzeScheduleVariable*>& variableReadSet);
    void translateForTCGSSMainStateSearchRegistBranchRelationsCollectOneScheduleBranchVarWrite(const std::vector<ILAnalyzeScheduleNode*>& scheduleNodeDataFlowList, std::vector<std::pair<void*, std::string>>& listBranchWriteGlobalVar, ILAnalyzeScheduleNode* node, std::map<std::string, std::string>& globalVariablePointerMap);
    
    std::string translateForTCGSSMainStateSearchBranchDistanceRegister();

    std::string translateForTCGSSMainInit();
    std::string translateForTCGSSMainLoop();
    int translateForTCGSSStepFunction(const ILParser* parser, std::string& code);
    std::string translateForTCGSSMainExecParameterCopy(const ILFunction* mainExecFunction) const;
    std::string translateForTCGSSMainExecParameterCopyTraverse(std::string name, std::string type, std::vector<int> arraySize, bool isAddress) const;
    int translateForTCGSSMainStateSearchRegistInputs(const ILParser* parser);
    std::string translateForTCGSSMainStateSearchRegistInputOne(std::string name, std::string type, std::vector<int> arraySize, bool isAddress) const;
    int translateForTCGSSMainStateSearchRegistParameters(const ILParser* parser);
    std::string translateForTCGSSMainStateSearchRegistParameterOne(std::string name, std::string type, std::vector<int> arraySize, bool isAddress) const;
    std::string translateForHybridMainBranchDistanceRegister();

    // Only used by TranslatorForLibFuzzer
    int translateForLibFuzzerMainFunction(const ILParser* parser, std::string& code);
    int translateForLibFuzzerHeadFile();
    std::string translateForLibFuzzerMainTargetParaReloadInputSequence() const;
    std::string translateForLibFuzzerMainExecInputCopy(const ILFunction* mainExecFunction) const;
    std::string translateForLibFuzzerMainExecInputCopyTraverse(std::string name, std::string type, std::vector<int> arraySize, bool isAddress) const;

    std::string translateForLibFuzzerMainParameterCopy(const ILConfig* config) const;
    std::string translateForLibFuzzerMainParameterCopyTraverse(std::string name, std::string type, std::vector<int> arraySize, bool isAddress) const;


    // Only used by TranslatorForTCGHybrid
    int translateForTCGHybridMainFunction(const ILParser* parser, std::string& code);
    int translateForTCGHybridHeadFile();
    std::string translateForTCGHybridMainHybridInit();
    std::string translateForTCGHybridMainHybridRegistBranch();
    std::string translateForTCGHybridMainHybridRegistGlobalVariables();
    std::string translateForTCGHybridMainHybridRegistGlobalVariableOne(ILGlobalVariable* iLGlobalVariable);
    void translateForTCGHybridMainHybridRegistGlobalVariableOne(std::string varName, std::string type, std::vector<int> arraySize, std::string& retCode);
    //std::string translateForTCGHybridMainStateSearchBranchDistanceRegister();

    std::string translateForTCGHybridMainInit();
    std::string translateForTCGHybridMainLoop();
    int translateForTCGHybridStepFunction(const ILParser* parser, std::string& code);
    std::string translateForTCGHybridMainExecInputCopy(const ILFunction* mainExecFunction) const;
    std::string translateForTCGHybridMainExecInputCopyTraverse(std::string name, std::string type, std::vector<int> arraySize, bool isAddress) const;
    int translateForTCGHybridMainHybridRegistInputs(const ILParser* parser);
    std::string translateForTCGHybridMainHybridRegistInputOne(std::string name, std::string type, std::vector<int> arraySize, bool isAddress) const;
    std::string translateForTCGHybridMainParameterCopy(const ILConfig* config) const;
    std::string translateForTCGHybridMainParameterCopyTraverse(std::string name, std::string type, std::vector<int> arraySize, bool isAddress) const;
    int translateForTCGHybridMainHybridRegistParameters(const ILParser* parser);
    std::string translateForTCGHybridMainHybridRegistParameterOne(std::string name, std::string type, std::vector<int> arraySize, bool isAddress) const;
    // Only used by TranslatorForCoverage
    int translateForCoverageHeadFile();
    int translateForCoverageMainFunction(const ILParser* parser, std::string& code);
    std::string translateForCoverageInfoInit();
    std::string translateForCoverageFilesName();
    std::string translateForCoverageInfoOutput();
    std::string translateForCoverageMainExecParameterCopy(const ILFunction* mainExecFunction) const;
    std::string translateForCoverageMainExecParameterCopyTraverse(std::string name, std::string type, std::vector<int> arraySize, bool isAddress) const;

    // Only used by TranslatorForSimulator
    int translateForSimulatorMainFunction(const ILParser* parser, std::string& code);
};
