#pragma once
#include <map>
#include <string>

#include "CodeGenerationTable.h"

#include "../Common/tinyxml2.h"

class MIRActor;
class ILSchedule;
class MIRModel;
class ILScheduleNode;
class ILCalculate;
class MIRScheduleGraphInport;
class ILObject;
class ILFunction;
class ILFile;
class MIRScheduleGraphDataflow;
class MIRSchedule;
class ILParser;

class ILGeneratorDataflow
{
public:
    int generateIL(MIRScheduleGraphDataflow* scheduleGraph, MIRModel* model, ILParser* retILParser);

private:

    MIRScheduleGraphDataflow* scheduleGraph = nullptr;
    ILParser* iLParser = nullptr;
    MIRModel* model = nullptr;
    
    // 4. 基于代码生成位置表构建中间代码IL

    //// 用于对Delay组件进行新的类型推导时，快速地找到需要刷新的组件的ILCalculate
    //std::map<CGTActorExe*, ILCalculate*>  mapInitCGTActorExeToILCalculate;
    //std::map<CGTActorExe*, ILCalculate*>  mapExecCGTActorExeToILCalculate;
    ////std::map<ILCalculate*, CGTActorExe*>  mapILCalculateToCGTActorExe;

    // 采用先基于组件模板获取各个CGTActorExe元素的IL数据，然后再整体拼装中间代码的方式
    // 进行中间代码生成
    // 这样可以方便后续刷新Delay组件的中间代码时的中间代码信息更新
    std::map<CGTActorExe*, std::string> mapCGTActorExeToILInformation;

    // 用于记录一个组件因为分支而生成多次。第二次生成时就不需要再生成全局变量、局部变量之类的内容了，只需要生成执行代码。
    std::map<MIRActor*, int> mapMIRActorGenerateCount;

    // 用于在为CGT中组件生成代码时记录组件后面对应的端口是否需要被生成在主干分支上
    // 不能直接在CGT中跟着组件生成的原因是ForIteration组件会导致重复的输出端口赋值，影响效率
    std::map<const MIRScheduleGraphInport*, const MIRScheduleGraphOutport*> mapGraphOutportGenerationSrc;


    int generateIL(const MIRScheduleGraphDataflow* schedule, const CodeGenerationTable &CGT, ILFile** retILFile);
    // 当需要类型推导复合组件时调用这个函数。该函数可能会由于Delay的求解而调用多次
    int generateILForInnerFunction(const CGTActorExe* actorExeFunction);

    int generateILInformationForActorExe(CGTActorExe* actorExe);
    int generateILInformationForActorExeTraverseBranch(const CGTBranch* branch);
    
    int generateILFunctionInit(const MIRScheduleGraphDataflow* schedule, ILFile* iLFile, ILFunction** retFunction);
    int generateILFunctionExec(const MIRScheduleGraphDataflow* schedule, ILFile* iLFile, ILFunction** retFunction);
    int generateILFunctionExecInput(const MIRScheduleGraphDataflow* schedule, ILFile* iLFile, ILFunction** retFunction);
    int generateILFunctionExecOutput(const MIRScheduleGraphDataflow* schedule, ILFile* iLFile, ILFunction** retFunction);
    int generateILLoopForForIteratorActor(const MIRScheduleGraphDataflow* schedule, ILSchedule* parent);
    int generateILBranch(const CGTBranch* branch, ILObject* parent, ILFunction* functionInit, ILFunction* functionExec, ILFile* iLFile);
    int generateILActorExe(const CGTActorExe* actorExe, ILObject* parent, ILFunction* functionInit, ILFunction* functionExec, ILFile* iLFile);
    int generateILOutportAssign(const MIRScheduleGraphInport* outport, ILObject* parent);
    std::vector<Branch> generateILBranchConditionBranch(const std::vector<BranchPath>* realBranchPaths, const std::vector<BranchPath>* realBranchPathsOfParent);
    int generateILGraphOutportAssign(const CGTActorExe* actorExe, ILObject* parent);
    int generateILGraphOutportAssignForFunctionInport(const MIRScheduleGraphDataflow* schedule, ILObject* parent);
    int generateILGraphOutportAssignInferenceType(MIRScheduleGraphInport* graphOutport, MIRScheduleGraphOutport* srcOutport);
    int generateILGraphOutportAssignCalculate(MIRScheduleGraphInport* graphOutport, MIRScheduleGraphOutport* srcOutport, ILObject* parent, std::vector<ILScheduleNode*>* scheduleNodes);
    int generateILGraphOutportAssignCalculateStatement(MIRScheduleGraphInport* graphOutport, MIRScheduleGraphOutport* srcOutport, ILCalculate* retILCalculate);
    // 由于DelayRead组件都处于数据流图的最前端，在一遍代码生成后并不能有效地进行类型推导，所以要更新这些组件的输入输出类型
    int generateILUpdateForDelayActor(const CodeGenerationTable &CGT);
    int generateILUpdateForDelayActorOnce(const CodeGenerationTable &CGT, CGTActorExe* actorExeDelayRead);
    CGTActorExe* generateILUpdateForDelayActorOnceFindDelayStore(const CodeGenerationTable &CGT, CGTActorExe* actorExeDelayRead);
    std::vector<CGTActorExe*> generateILUpdateForDelayActorOnceGetSuccessors(const CodeGenerationTable &CGT, CGTActorExe* actorExe);

    int transILInformationToIL(const std::string& iLXMLStr, ILFile* retILFile, ILFunction* functionInit, ILFunction* functionUpdate, ILCalculate* retILCalculate, bool justTransExec = false);
    int transILInformationToILHeadFile(tinyxml2::XMLElement* root, ILFile* retILFile);
    int transILInformationToILMacro(tinyxml2::XMLElement* root, ILFile* retILFile);
    int transILInformationToILTypeDefine(tinyxml2::XMLElement* root, ILFile* retILFile);
    int transILInformationToILEnumDefine(tinyxml2::XMLElement* root, ILFile* retILFile);
    int transILInformationToILStructDefine(tinyxml2::XMLElement* root, ILFile* retILFile);
    int transILInformationToILUtilityFunction(tinyxml2::XMLElement* root, ILFile* retILFile);
    int transILInformationToILGlobalVariable(tinyxml2::XMLElement* root, ILFile* retILFile);
    int transILInformationToILLocalVariable(tinyxml2::XMLElement* root, ILFunction* functionUpdate);
    int transILInformationToILInitCalculate(tinyxml2::XMLElement* root, ILFunction* functionInit);
    int transILInformationToILUpdateCalculate(tinyxml2::XMLElement* root, ILCalculate* retILCalculate);

    int transILTypeInference(const std::string& iLXMLStr, CGTActorExe* retActorExe);

    static std::string getDataflowNodeBranchConditionByBranchId(const MIRScheduleGraphDataflowNode* node, int branchId);

};


