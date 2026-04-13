#pragma once

#include <string>
#include <vector>
#include <map>
#include <set>

#include "../ILBasic/Token.h"

class ILSchedule;
class ILRef;
class ILParser;
class TCGConfig;
class ILAnalyzeSchedule;
class ILAnalyzeScheduleNode;
class ILGlobalVariable;
class Expression;

class ILTranslatorForTCGHybrid
{
public:
	
	static ILParser* iLParser;
    static TCGConfig* tcgConfig;

    ILTranslatorForTCGHybrid(ILParser* iLParser, TCGConfig* tcgConfig);

	int translate(std::string outputDir);

    static void setConfigGenerateComment(bool value);
    static bool configGenerateComment;
    
    static void setConfigCutOffArray(int value);
	static int configCutOffArray;
    
	std::map<std::string, std::string> fileCodes;
    
	static std::string transCodeBranchRegister;
	static std::string transCodeGlobalVariableRegister;
    static std::string transCodeInputRegister;
    static std::string transCodeParameterRegister;
    static std::string transCodeBranchDistanceRegister;

    static std::map<const void*, std::set<std::string>> branchDistanceStmtObjectList;
    void release();
    class BranchDistanceData
    {
    public:
        int branchId = 0; // ��������?��branchId
        int distanceType = 0; // 1: value distance  2: branch distance
        std::string varName;
        std::string distanceOpType; // only support '>' and '<'
        std::string targetValue; // ��?����?��?��?�� �������?��?�����?��?
        void* branchObject = nullptr;

        BranchDistanceData() {}
        BranchDistanceData(int branchId, int distanceType, std::string varName, std::string distanceOpType, std::string targetValue)
            : branchId(branchId), distanceType(distanceType), varName(varName), distanceOpType(distanceOpType), targetValue(targetValue) {}

    };
    static std::vector<BranchDistanceData> branchDistanceDataList;
    static std::map<std::string, std::pair<std::string, std::string>> arrayDistanceBranchObjectList;
    static std::map<const void*, int> branchDistanceBranchObjectList; //key: branchObject, value: branchId
    static std::map<ILSchedule* ,ILAnalyzeSchedule*> iLAnalyzeSchedule;
private:

    std::string getConfigTestRedefineOutputCode() const;
        class VarReadTreeNode {
    public:
        VarReadTreeNode() {}
        std::string varName;
        Token::Type opType;
        Expression* exp = nullptr;
        bool isGlobalRelated = false;
        bool isConst = false;
        std::string referedConstValue;
        //const ILAnalyzeScheduleVariable* variable;
        //bool isCompareConst;
        std::vector<VarReadTreeNode*> children;
        VarReadTreeNode* parent = nullptr;

        void* branchObject = nullptr;

        void release()
        {
            for (size_t i = 0; i < children.size(); i++)
            {
                children[i]->release();
                delete children[i];
            }
        }
    };

    int translateForTCGHybridCalculateBranchDistanceRegister();
    int translateForTCGHybridCalculateBranchDistanceCollectOneSchedule(ILSchedule* iLSchedule, const ILParser* parser, std::vector<std::pair<void*, std::string>>& listBranchReadGlobalVar);
    void translateForTCGHybridCalculateBranchDistanceCollectOneScheduleBranchVarReadTree(const std::vector<ILAnalyzeScheduleNode*>& scheduleNodeDataFlowList, ILAnalyzeScheduleNode* node, ILAnalyzeSchedule* analyzeSchedule, VarReadTreeNode* varReadTreeRoot);
    void translateForTCGHybridCalculateBranchDistanceCollectOneScheduleBranchVarReadTreeTraverseExp(const Expression* exp, VarReadTreeNode* varReadTreeNode, ILAnalyzeSchedule* analyzeSchedule, const std::vector<ILAnalyzeScheduleNode*>& scheduleNodeDataFlowList, int nodeIndex, ILAnalyzeScheduleNode* curNode, ILAnalyzeScheduleNode* curBranchNode);

    // std::string repalceBranchDistancePlaceholder(std::string branchDistanceRegisterCode);

    int translateForTCGHybridCalculateArrayDistanceRegister(const ILParser* parser);
    void translateForTCGHybridCalculateArrayDistanceRegisterOne(ILGlobalVariable* iLGlobalVariable);
    void translateForTCGHybridCalculateArrayDistanceRegisterOne(std::string varName, std::string type, std::vector<int> arraySize);

    int copyResourceFiles(std::string outputDir);



    
};

