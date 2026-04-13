#pragma once
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <iostream>
#include <stack>

#include "../Common/tinyxml2.h"

#include "../MIRBasic/MIRObject.h"
#include "../MIRBasic/MIRModel.h"
#include "../MIRBasic/MIRFunction.h"
#include "../MIRBasic/MIRActor.h"
#include "../MIRBasic/MIRInport.h"
#include "../MIRBasic/MIROutport.h"
#include "../MIRBasic/MIRActionPort.h "
#include "../MIRBasic/MIRRelation.h"
#include "../MIRBasic/MIRSaver.h"

#include "../Common/Utility.h"

#include "MIRSchedule.h"
#include "MIRScheduleAnalyzer.h"
#include "MIRScheduleGraphDataflow.h"

#include "BranchInfo.h"
#include "CodeGenerationTable.h"

class ILCalculate;
class MIRScheduleGraphActionPort;
class MIRScheduleGraphStateflow;
class MIRFunctionStateflow;
class MIRFunctionDataflow;

class MIRScheduleAnalyzerDataflow
{
public:
    MIRScheduleAnalyzerDataflow(MIRScheduleAnalyzer* bassAnalyzer);
    ~MIRScheduleAnalyzerDataflow();

    int collectScheduleFunction(MIRSchedule* schedule, MIRFunction* function);
    int calculateScheduleGraph(MIRScheduleGraph* schedule);


private:
    friend class CGTOperation;
    friend class CodeGenerationTable;

    MIRScheduleAnalyzer* bassAnalyzer = nullptr;
    
    int collectScheduleFunctionDataflow(MIRSchedule* schedule, MIRFunctionDataflow* function);
    int collectScheduleFunctionDataflowNodes(MIRScheduleGraphDataflow* scheduleGraphDataflow, MIRActor* actor);
    int collectScheduleFunctionDataflowRelation(MIRScheduleGraphDataflow* scheduleGraphDataflow, MIRFunctionDataflow* function, MIRRelation* relation);
    MIRScheduleGraphOutport* findScheduleGraphOutport(MIRScheduleGraphDataflow* scheduleGraphDataflow, MIRObject* outport);
    MIRScheduleGraphInport* findScheduleGraphInport(MIRScheduleGraphDataflow* scheduleGraphDataflow, MIRObject* inport);
    MIRScheduleGraphActionPort* findScheduleGraphActionPort(MIRScheduleGraphDataflow* scheduleGraphDataflow, MIRActionPort* actionPort);
    int collectScheduleFunctionFunctionCallOrder(MIRScheduleGraphDataflow* scheduleGraphDataflow, MIRScheduleGraphDataflowNode* node);

    int collectScheduleFunctionInports(MIRScheduleGraph* scheduleGraph, MIRInport* inport);
    int collectScheduleFunctionOutports(MIRScheduleGraph* scheduleGraph, MIROutport* outport);
   
    int calculateScheduleGraphDataflow(MIRScheduleGraphDataflow* schedule);

    // 模型数据流转控制流算法 ：
    //
    // 1. 拓扑排序算法
    int topologicalSort(MIRScheduleGraphDataflow* schedule);
    // 为保证delayRead一定在delayStroe生成的代码前，需要调整从delay组件分离而来的两个组件的顺序(仅限同层)
    int topologicalSortSwapDelayActor(std::vector<MIRScheduleGraphDataflowNode*>& scheduleLayer);
    bool topologicalSortIsNodeNoPred(MIRScheduleGraphDataflowNode* node, std::vector<MIRScheduleGraphDataflowNode*> lastNodes);

    // 2. 分支信息标记及化简
    
    // 2.1 分支标记
    int branchMarking(MIRScheduleGraphDataflow* schedule, std::map<MIRScheduleGraphDataflowNode*, BranchInfo> &retBranchInfos);
    int branchMarkingOneNode(MIRScheduleGraphDataflowNode* node, std::map<MIRScheduleGraphDataflowNode*, BranchInfo> &retBranchInfos);
    int branchMarkingOneNodePredIsInport(MIRScheduleGraphOutport* predOutport, BranchInfo* branchInfoOfPred, BranchInfo &branchInfo);
    int branchMarkingOneNodePredIsBranch(MIRScheduleGraphOutport* predOutport, BranchInfo* branchInfoOfPred, BranchInfo &branchInfo);
    int branchMarkingOneNodePredIsNotBranch(MIRScheduleGraphOutport* predOutport, BranchInfo* branchInfoOfPred, BranchInfo &branchInfo);
    int branchMarkingOneNodePredAddBranchPathWithSrc(const BranchInfo &srcBranchInfo, BranchInfo &dstBranchInfo);


    // 2.2 单个组件分支信息化简
    int branchInfoSimplify(MIRScheduleGraphDataflowNode* node, BranchInfo &branchInfo, const std::map<MIRScheduleGraphDataflowNode*, BranchInfo>& retBranchInfos);

    // 将BooleanSwitch分支组件中的Ctrl端口带来的分支信息去除，只保留In端口的，因为当In端口没数据时，该组件也没有任何意义
    int branchInfoSimplifyRemoveBooleanSwitchCtrlBranchInfo(MIRScheduleGraphDataflowNode* node, BranchInfo &branchInfo, const std::map<MIRScheduleGraphDataflowNode*, BranchInfo>& retBranchInfos);

    // 单个组件分支路径数据源合并
    // a. 将父级分支路径上的数据源合并到子级分支路径上
	// O,A1  v   =>  O,A1  v,w
	// O     w   =>  O     w
	// b. 如果父级分支路径的数据源和子级分支路径的数据源一致，则删除子级分支路径
	// O,A1  v   =>  
	// O     v   =>  O     v
    int branchInfoSimplifyMergeSource(BranchInfo &branchInfo);
    
    // 合并多分支同数据源的分支路径
    // O,B2   =>   O,B2
	// O,A2   =>  
	// O,A1   =>  
	// O      =>   O
    int branchInfoSimplifyMergeBranch(BranchInfo &branchInfo);
    
    void branchInfoPrint(const MIRScheduleGraphDataflow* schedule, const std::map<MIRScheduleGraphDataflowNode*, BranchInfo> &retBranchInfos) const;


    // 3. 构建代码生成位置表
    
    int buildCodeGenerationTable(const MIRScheduleGraphDataflow* schedule, const std::map<MIRScheduleGraphDataflowNode*, BranchInfo> &branchInfos, CodeGenerationTable &retCGT);
    int buildCGTEmpty(CodeGenerationTable &retCGT);
    int buildCGTOneNode(const MIRScheduleGraphDataflowNode* node, const std::map<MIRScheduleGraphDataflowNode*, BranchInfo>& branchInfos, CodeGenerationTable &CGT);
    int buildCGTOneNodeSameBranchInfo(const MIRScheduleGraphDataflowNode* node, const std::map<MIRScheduleGraphDataflowNode*, BranchInfo>& branchInfos, CodeGenerationTable &CGT, const BranchInfo& branchInfo, CGTOperation* operation);
    int buildCGTUpdateLayer(CodeGenerationTable &retCGT);

    static bool buildCGTIsBranchInfoConflict(const BranchInfo &branchInfo);
    static bool buildCGTIsCommonBranchNeedRemovedInserter(const BranchInfo &branchInfo, const CodeGenerationTable& CGT);
    static bool buildCGTIsThereSuitableInserter(const BranchInfo &branchInfo, const CodeGenerationTable& CGT);

    static const CGTInserter* buildCGTFindInserterByBranchPath(const BranchPath &branchPath, const CodeGenerationTable &CGT);
    static const CGTInserter* buildCGTGetBaseInserterOfCommonBranchPath(BranchPath &commonBranchPath, const CodeGenerationTable &CGT);
    
    static std::vector<Branch> buildCGTGetBranchInfoConditionBranchList(const BranchInfo &branchInfo, BranchPath &commonBranchPath);
    static bool buildCGTIsConditionBranchListAlwaysFalse(std::vector<Branch> &conditionBranchList);

    std::vector<MIRScheduleGraphDataflowNode*> getNodePredecessors(const MIRScheduleGraphDataflowNode* node) const;
    std::vector<MIRScheduleGraphOutport*> getNodePredecessorsOutports(const MIRScheduleGraphDataflowNode* node) const;
    std::vector<MIRScheduleGraphDataflowNode*> getNodeSuccessors(const MIRScheduleGraphDataflowNode* node) const;
    std::vector<MIRScheduleGraphDataflowNode*> getNodeAllPredecessors(const MIRScheduleGraphDataflowNode* node) const;

    static BranchPath getBranchInfoCommonBranchPath(const BranchInfo& branchInfo);
    static bool areTwoBranchPathConflict(const BranchPath &srcBp, const BranchPath &dstBp);
    static bool areTwoBranchPathSame(const BranchPath &srcBp, const BranchPath &dstBp);
    static bool areTwoBranchPathDataSrcSame(const BranchPathDataSrc &srcBpDataSrc, const BranchPathDataSrc &dstBpDataSrc);
    static void mergeTwoBranchPathDataSrc(const BranchPathDataSrc &srcBpDataSrc, BranchPathDataSrc &dstBpDataSrc);
    static bool isBranchPathContainBranchPath(const BranchPath &srcBp, const BranchPath &dstBp);
    static bool areTwoBranchInfoPathSame(const BranchInfo &srcBranchInfo, const BranchInfo &dstBranchInfo);
    

    static bool isDataflowNodeBranchNode(const MIRScheduleGraphDataflowNode* node);
    static int getDataflowNodeBranchCount(const MIRScheduleGraphDataflowNode* node);
    static int getDataflowNodeBranchId(const MIRScheduleGraphDataflowNode* node, const MIRScheduleGraphOutport* outport);
    
};






