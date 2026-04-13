#include "ILGeneratorDataflow.h"

#include <stack>
#include <string>

#include "../ILBasic/ILExpressionParser.h"

#include "../ILBasic/ILParser.h"
#include "../ILBasic/ILSaver.h"
#include "../ILBasic/ILRef.h"
#include "../ILBasic/Statement.h"
#include "../ILBasic/ILLocalVariable.h"
#include "../ILBasic/ILSchedule.h"
#include "../ILBasic/ILHeadFile.h"
#include "../ILBasic/ILMacro.h"
#include "../ILBasic/ILTypeDefine.h"
#include "../ILBasic/ILEnumDefine.h"
#include "../ILBasic/ILStructDefine.h"
#include "../ILBasic/ILUtilityFunction.h"
#include "../ILBasic/ILGlobalVariable.h"
#include "../ILBasic/ILCalculate.h"
#include "../ILBasic/ILBranch.h"
#include "../ILBasic/ILFunction.h"
#include "../ILBasic/ILInput.h"
#include "../ILBasic/ILOutput.h"
#include "../ILBasic/StmtExpression.h"

#include "MIRSchedule.h"
#include "MIRScheduleGraph.h"
#include "ILGeneratorDataflow.h"

#include "CGTActorExeTranslator.h"
#include "ILGenerator.h"
#include "ILGeneratorStateflow.h"
#include "MIRPreprocessor.h"
#include "MIRScheduleGraphDataflow.h"
#include "MIRScheduleGraphDataflowNode.h"
#include "MIRScheduleGraphOutport.h"
#include "MIRScheduleGraphInport.h"

using namespace std;

int ILGeneratorDataflow::generateIL(MIRScheduleGraphDataflow* scheduleGraph, MIRModel* model, ILParser* retILParser)
{
    int res;
    this->scheduleGraph = scheduleGraph;
    this->model = model;
    this->iLParser = retILParser;
    mapMIRActorGenerateCount.clear();

    ILFile* iLFileForMIRScheduleGraph = nullptr;
    res = generateIL(scheduleGraph, scheduleGraph->CGT, &iLFileForMIRScheduleGraph);
    if(res < 0)
        return res;

    retILParser->files.push_back(iLFileForMIRScheduleGraph);

    return 1;
}


int ILGeneratorDataflow::generateIL(const MIRScheduleGraphDataflow* schedule, const CodeGenerationTable& CGT, ILFile** retILFile)
{
    int res = 1;

    // 先获取所有组件的中间代码信息
    for(int i = 0; i < CGT.elements.size(); i++)
    {
        CGTElement* element = CGT.elements[i];
        if(element->type == CGTElement::TypeBranch)
        {
            res = generateILInformationForActorExeTraverseBranch(static_cast<CGTBranch*>(element));
        }
        else if(element->type == CGTElement::TypeActorExe)
        {
            res = generateILInformationForActorExe(static_cast<CGTActorExe*>(element));
        }
        if(res < 0)
            return res;
    }

    res = generateILUpdateForDelayActor(CGT);
    if(res < 0)
        return res;
    
    // 之后进行中间代码的拼装
    *retILFile = new ILFile();
    (*retILFile)->name = schedule->name;

    ILFunction* functionInit = nullptr;
    res = generateILFunctionInit(schedule, *retILFile, &functionInit);
    if(res < 0)
        return res;

    ILFunction* functionExec = nullptr;
    res = generateILFunctionExec(schedule, *retILFile, &functionExec);
    if(res < 0)
        return res;

    res = generateILGraphOutportAssignForFunctionInport(schedule, functionExec->schedule);
    if(res < 0)
        return res;

    ILObject* parentSchedule = functionExec->schedule;
    res = generateILLoopForForIteratorActor(schedule, functionExec->schedule);
    if(res < 0)
        return res;
    if(res == 1)
        parentSchedule = functionExec->schedule->scheduleNodes[0];

    for(int i = 0; i < CGT.elements.size(); i++)
    {
        CGTElement* element = CGT.elements[i];
        if(element->type == CGTElement::TypeBranch)
        {
            res = generateILBranch(static_cast<CGTBranch*>(element), parentSchedule, functionInit, functionExec, *retILFile);
        }
        else if(element->type == CGTElement::TypeActorExe)
        {
            res = generateILActorExe(static_cast<CGTActorExe*>(element), parentSchedule, functionInit, functionExec, *retILFile);
        }
        if(res < 0)
            return res;
    }

    for(int i = 0; i < schedule->outports.size(); i++)
    {
        const MIRScheduleGraphInport* outport = schedule->outports[i];
        if((res = generateILOutportAssign(outport, functionExec->schedule)) < 0)
            return res;
    }
    

    if((res = generateILFunctionExecInput(schedule, *retILFile, &functionExec))<0)
        return res;
    if((res = generateILFunctionExecOutput(schedule, *retILFile, &functionExec))<0)
        return res;

    CGTActorExeTranslator::releaseActorLibraries();

    return res;
}


int ILGeneratorDataflow::generateILForInnerFunction(const CGTActorExe* actorExeFunction)
{
    int res;
    MIRActor* functionActor = actorExeFunction->node->actor;
    string functionName = functionActor->getParameterByName("FunctionRef")->value;
    MIRFunction* function = ILGenerator::model->getFunctionByName(functionName);
    if(!function)
        return -MIRScheduleAnalyzer::ErrorUnknownFunctionName;

    for(int i = 0; i < actorExeFunction->node->inports.size(); i++)
    {
        MIRScheduleGraphInport* graphInport = actorExeFunction->node->inports[i];
        vector<MIRScheduleGraphOutport*> srcGraphOutport = graphInport->getRealSrcOutport(actorExeFunction);
        if(srcGraphOutport.empty())
            continue;

        MIRInport* functionInport = function->getInportByName(graphInport->inportFromActor->name);
        if(!functionInport)
            continue;

        MIRInport* dst = graphInport->inportFromActor;

        if(srcGraphOutport[0]->inportFromFunction)
        {
            MIRInport* src = srcGraphOutport[0]->inportFromFunction;
            
            functionInport->type = src->type;
            functionInport->arraySize = src->arraySize;
            functionInport->isAddress = src->isAddress;
            
            dst->type = src->type;
            dst->arraySize = src->arraySize;
            dst->isAddress = src->isAddress;
        }
        else
        {
            MIROutport* src = srcGraphOutport[0]->outportFromActor;

            functionInport->type = src->type;
            functionInport->arraySize = src->arraySize;
            functionInport->isAddress = src->isAddress;
            
            dst->type = src->type;
            dst->arraySize = src->arraySize;
            dst->isAddress = src->isAddress;
        }

    }

    // 因为delay组件的类型推导可能会导致某个复合组件重新进行代码生成，所以要清除掉该复合组件原来生成的ILFile
    for(int i = 0; i < ILGenerator::retILParser->files.size(); i++)
    {
        ILFile* iLFile = ILGenerator::retILParser->files[i];
        if(iLFile->name == functionName)
        {
            iLFile->release();
            ILGenerator::retILParser->files.erase(ILGenerator::retILParser->files.begin() + i);
            break;
        }
    }
    res = ILGenerator::generateInnerFunction(functionName);
    if(res < 0)
        return res;

    // 根据复合组件代码类型推导结果，赋值该actor的输出
    for(int i = 0; i < functionActor->outports.size(); i++)
    {
        MIROutport* actorOutport = functionActor->outports[i];
        MIROutport* functionOutport = function->getOutportByName(actorOutport->name);
        if(!functionOutport)
            continue;

        actorOutport->type = functionOutport->type;
        actorOutport->arraySize = functionOutport->arraySize;
        actorOutport->isAddress = functionOutport->isAddress;
    }
    return 1;
}

int ILGeneratorDataflow::generateILInformationForActorExe(CGTActorExe* actorExe)
{
    int res;

    if(actorExe->node->actor->type == "Function")
    {
        res = generateILForInnerFunction(actorExe);
        if(res < 0)
            return res;
    }

    //保证生成代码的过程不会出现变量冲突类型重定义等错误
    ILParser::clearVariableAndDataTypeRecord();

    CGTActorExeTranslator actorExeTranslator;
    string retILInformation;
    res = actorExeTranslator.transCGTActorExe(actorExe, this->model, retILInformation);
    if(res < 0)
        return res;

    res = transILTypeInference(retILInformation, actorExe);
    if(res < 0)
        return res;

    CGTActorExe* key = actorExe;

    //if(mapCGTActorExeToILInformation[key] == retILInformation)
    //    return 0;
    mapCGTActorExeToILInformation[key] = retILInformation;

    return 1;
}

int ILGeneratorDataflow::generateILInformationForActorExeTraverseBranch(const CGTBranch* branch)
{
    int res = 0;
    for(int i = 0; i < branch->elements.size(); i++)
    {
        CGTElement* element = branch->elements[i];
        if(element->type == CGTElement::TypeBranch)
        {
            res = generateILInformationForActorExeTraverseBranch(static_cast<CGTBranch*>(element));
        }
        else if(element->type == CGTElement::TypeActorExe)
        {
            res = generateILInformationForActorExe(static_cast<CGTActorExe*>(element));
        }
        if(res < 0)
            return res;
    }
    return 1;
}

int ILGeneratorDataflow::generateILFunctionInit(const MIRScheduleGraphDataflow* schedule, ILFile* iLFile,
                                                        ILFunction** retFunction)
{
    int res;
    *retFunction = new ILFunction(iLFile);
    iLFile->functions.push_back(*retFunction);
    (*retFunction)->name = schedule->name + "_Init";
    (*retFunction)->returnType = "void";
    (*retFunction)->type = ILFunction::FunctionInit;

    ILRef* tempILRef = new ILRef();
    tempILRef->actor = "CompositeActor";
    tempILRef->path = schedule->name;
    (*retFunction)->refs.push_back(tempILRef);

    ILSchedule* iLSchedule = new ILSchedule(*retFunction);
    (*retFunction)->schedule = iLSchedule;

    ILInput* iLInputState = new ILInput(*retFunction);
    (*retFunction)->inputs.push_back(iLInputState);
    iLInputState->name = "self";
    iLInputState->type = schedule->name + "_state";
    iLInputState->isAddress = 1;

    return 1;
}

int ILGeneratorDataflow::generateILFunctionExec(const MIRScheduleGraphDataflow* schedule, ILFile* iLFile, ILFunction** retFunction)
{
    int res;
    *retFunction = new ILFunction(iLFile);
    iLFile->functions.push_back(*retFunction);
    (*retFunction)->name = schedule->name + "_Update";
    (*retFunction)->returnType = "void";
    (*retFunction)->type = ILFunction::FunctionExec;

    
    ILRef* tempILRef = new ILRef();
    tempILRef->actor = "CompositeActor";
    tempILRef->path = schedule->name;
    (*retFunction)->refs.push_back(tempILRef);
    
    ILSchedule* iLSchedule = new ILSchedule(*retFunction);
    (*retFunction)->schedule = iLSchedule;
    
    // 创建该函数状态的全局变量结构体类型，仅仅是为了适配华为
    ILStructDefine* iLStructDefine = new ILStructDefine(iLFile);
    iLFile->structDefines.push_back(iLStructDefine);
    iLStructDefine->name = schedule->name + "_state";

    //ILGlobalVariable* iLGlobalVariable = new ILGlobalVariable(iLFile);
    //iLFile->globalVariables.push_back(iLGlobalVariable);
    //iLGlobalVariable->name = schedule->name + "_state";


    // 创建函数对应的状态接口，仅仅是为了适配华为
    ILInput* iLInputState = new ILInput(*retFunction);
    (*retFunction)->inputs.push_back(iLInputState);
    iLInputState->name = "self";
    iLInputState->type = schedule->name + "_state";
    iLInputState->isAddress = 1;


    return 1;
}

int ILGeneratorDataflow::generateILFunctionExecInput(const MIRScheduleGraphDataflow* schedule, ILFile* iLFile,
    ILFunction** retFunction)
{
    int res;
    for(int i = 0; i < schedule->inports.size(); i++)
    {
        ILInput* iLInput = new ILInput(*retFunction);
        (*retFunction)->inputs.push_back(iLInput);
        MIRScheduleGraphOutport* inport = schedule->inports[i];
        MIRInport* mIRInport = inport->inportFromFunction;
        iLInput->name = mIRInport->name;
        iLInput->type = mIRInport->type == "#Default" ? MIRPreprocessor::defaultType : mIRInport->type;
        iLInput->isAddress = mIRInport->isAddress;
        iLInput->arraySize = mIRInport->arraySize;
        ILExpressionParser expressionParser;
	    Expression* expRet = nullptr;
        if(!mIRInport->defaultValue.empty())
        {
            res = expressionParser.parseExpression(mIRInport->defaultValue, &expRet);
            if(res < 0)
                return res;
        }
        iLInput->defaultValue = expRet;
    }
    return 1;
}

int ILGeneratorDataflow::generateILFunctionExecOutput(const MIRScheduleGraphDataflow* schedule, ILFile* iLFile,
    ILFunction** retFunction)
{
    int res;
    for(int i = 0; i < schedule->outports.size(); i++)
    {
        ILOutput* iLOutput = new ILOutput(*retFunction);
        (*retFunction)->outputs.push_back(iLOutput);
        MIRScheduleGraphInport* outport = schedule->outports[i];
        MIROutport* mIROutport = outport->outportFromFunction;
        

        iLOutput->name = mIROutport->name;
        iLOutput->type = mIROutport->type == "#Default" ? MIRPreprocessor::defaultType : mIROutport->type;
        iLOutput->isAddress = mIROutport->isAddress;
        iLOutput->arraySize = mIROutport->arraySize;
        ILExpressionParser expressionParser;
	    Expression* expRet = nullptr;
        if(!mIROutport->defaultValue.empty())
        {
	        res = expressionParser.parseExpression(mIROutport->defaultValue, &expRet);
	        if(res < 0)
		        return res;
        }
        iLOutput->defaultValue = expRet;
    }
    return 1;
}

int ILGeneratorDataflow::generateILLoopForForIteratorActor(const MIRScheduleGraphDataflow* schedule, ILSchedule* parent)
{
    bool needLoop = false;
    int loopCount = 0;
    MIRActor* loopActor = nullptr;
    for(int i = 0; i < schedule->nodes.size(); i++)
    {
        MIRScheduleGraphDataflowNode* node = schedule->nodes[i];
        if(node->actor && node->actor->type == "ForIterator")
        {
            needLoop = true;
            loopActor = node->actor;
            MIRParameter* para = node->actor->getParameterByName("IterationLimit");
            loopCount = para ? stringToInt(para->value) : 1;
            break;
        }
    }
    if(!needLoop)
        return 0;
    ILBranch* iLLoop = new ILBranch();
    iLLoop->type = ILBranch::TypeFor;
    ILExpressionParser expParser;
    iLLoop->condition = expParser.parseExpression(to_string(loopCount));
    iLLoop->parent = parent;
    parent->scheduleNodes.insert(parent->scheduleNodes.begin(), iLLoop);
    ILRef* iLRef = new ILRef();
    iLRef->actor = loopActor->type;
    iLRef->path = schedule->name + "." + parent->getContainerILFunction()->name + "." + loopActor->name;
    iLRef->parent = iLLoop;
    iLLoop->refs.push_back(iLRef);
    return 1;
}

int ILGeneratorDataflow::generateILBranch(const CGTBranch* branch, ILObject* parent,
                                          ILFunction* functionInit, ILFunction* functionExec, ILFile* iLFile)
{
    int res = 0;

    vector<ILScheduleNode*>* scheduleNodes = nullptr;
    if(parent->objType == ILObject::TypeSchedule)
        scheduleNodes = &static_cast<ILSchedule*>(parent)->scheduleNodes;
    else if(parent->objType == ILObject::TypeBranch)
        scheduleNodes = &static_cast<ILBranch*>(parent)->scheduleNodes;
    
    ILBranch* iLBranch = new ILBranch(parent);
    scheduleNodes->push_back(iLBranch);

    if(branch->branchType != CGTBranch::TypeElse && branch->branchType != CGTBranch::TypeCombineElse &&
        branch->branchType != CGTBranch::TypeUnknown)
    {
        const vector<BranchPath>* realBranchPaths = &branch->realBranchPaths;
        const vector<BranchPath>* realBranchPathsOfParent = nullptr;
        if(branch->parent)
            realBranchPathsOfParent = &static_cast<CGTBranch*>(branch->parent)->realBranchPaths;
        vector<Branch> branchConditionList = generateILBranchConditionBranch(realBranchPaths, realBranchPathsOfParent);
        string condition = "";
        for(int i = 0; i < branchConditionList.size(); i++)
        {
            Branch& branchCondTemp = branchConditionList[i];
            if( i != 0)
                condition += " && ";
            condition += getDataflowNodeBranchConditionByBranchId(branchCondTemp.branchNode, branchCondTemp.branchId);
        }

        ILExpressionParser expressionParser;
	    Expression* expRet = nullptr;
	    int res = expressionParser.parseExpression(condition, &expRet);
	    if(res < 0)
		    return res;
	    iLBranch->condition = expRet;
    }

    if (branch->branchType == CGTBranch::TypeIf || branch->branchType == CGTBranch::TypeCombineIf) {
        iLBranch->type = ILBranch::TypeIf;
    } else if (branch->branchType == CGTBranch::TypeElseIf || branch->branchType == CGTBranch::TypeCombineElseIf) {
        iLBranch->type = ILBranch::TypeElseIf;
    } else if (branch->branchType == CGTBranch::TypeElse || branch->branchType == CGTBranch::TypeCombineElse) {
        iLBranch->type = ILBranch::TypeElse;
    }

    for(int i = 0; i < branch->elements.size(); i++)
    {
        CGTElement* element = branch->elements[i];
        if(element->type == CGTElement::TypeBranch)
        {
            res = generateILBranch(static_cast<CGTBranch*>(element), iLBranch, functionInit, functionExec, iLFile);
        }
        else if(element->type == CGTElement::TypeActorExe)
        {
            res = generateILActorExe(static_cast<CGTActorExe*>(element), iLBranch, functionInit, functionExec, iLFile);
        }
        if(res < 0)
            return res;
    }

    return 1;
}

std::vector<Branch> ILGeneratorDataflow::generateILBranchConditionBranch(const std::vector<BranchPath>* realBranchPaths,
    const std::vector<BranchPath>* realBranchPathsOfParent)
{
    vector<Branch> branchList;
    vector<Branch> branchListOfParent;

    
    // 收集父级分支的分支列表
    for(int i = 0;realBranchPathsOfParent && i < realBranchPathsOfParent->size(); i++)
    {
        for(int j = 0; j < (*realBranchPathsOfParent)[i].branchs.size(); j++)
        {
            bool found = false;
            for(int k = 0; k < branchListOfParent.size(); k++)
            {
                if(branchListOfParent[k].branchId == (*realBranchPathsOfParent)[i].branchs[j].branchId &&
                    branchListOfParent[k].branchNode == (*realBranchPathsOfParent)[i].branchs[j].branchNode)
                {
                    found = true;
                    break;
                }
            }
            if(!found)
            {
                branchListOfParent.push_back((*realBranchPathsOfParent)[i].branchs[j]);
            }
        }
    }

    // 收集当前分支的分支列表
    for(int i = 0;realBranchPaths && i < realBranchPaths->size(); i++)
    {
        for(int j = 0; j < (*realBranchPaths)[i].branchs.size(); j++)
        {
            bool found = false;
            for(int k = 0; k < branchList.size(); k++)
            {
                if(branchList[k].branchId == (*realBranchPaths)[i].branchs[j].branchId &&
                    branchList[k].branchNode == (*realBranchPaths)[i].branchs[j].branchNode)
                {
                    found = true;
                    break;
                }
            }
            if(found)
            {
                continue;
            }
            found = false;
            for(int k = 0; k < branchListOfParent.size(); k++)
            {
                if(branchListOfParent[k].branchId == (*realBranchPaths)[i].branchs[j].branchId &&
                    branchListOfParent[k].branchNode == (*realBranchPaths)[i].branchs[j].branchNode)
                {
                    found = true;
                    break;
                }
            }
            if(found)
            {
                continue;
            }
            if(!(*realBranchPaths)[i].branchs[j].branchNode)
            {
                continue;
            }

            branchList.push_back((*realBranchPaths)[i].branchs[j]);
        }
    }
    return branchList;
}

int ILGeneratorDataflow::generateILGraphOutportAssign(const CGTActorExe* actorExe, ILObject* parent)
{
    int res;

    vector<ILScheduleNode*>* scheduleNodes = nullptr;
    if(parent->objType == ILObject::TypeSchedule)
        scheduleNodes = &static_cast<ILSchedule*>(parent)->scheduleNodes;
    else if(parent->objType == ILObject::TypeBranch)
        scheduleNodes = &static_cast<ILBranch*>(parent)->scheduleNodes;

    unordered_map<MIRScheduleGraphOutport*, bool> assignedOutport;
    for(int i = 0; i < actorExe->node->graph->outports.size(); i++)
    {
        assignedOutport.clear();
        MIRScheduleGraphInport* graphOutport = actorExe->node->graph->outports[i];
        

        for(int j = 0; j < graphOutport->srcOutports.size(); j++)
        {
            MIRScheduleGraphOutport* srcOutport = graphOutport->srcOutports[j];
            if(srcOutport->containerNode != actorExe->node || 
                assignedOutport.find(srcOutport) != assignedOutport.end()) {
                continue;
            }
            assignedOutport[srcOutport] = true;

            // 先确定是该组件对该端口进行了赋值，才能进行有效地类型推断
            res = generateILGraphOutportAssignInferenceType(graphOutport, srcOutport);
            if(res < 0)
                return res;

            // 仅在该组件位于其它分支时生成输出端口的赋值语句
            // 这样做是为了防止批量赋值语句被生成在ForIteration子系统的For循环体中
            if(actorExe->parent) {
                res = generateILGraphOutportAssignCalculate(graphOutport, srcOutport, parent, scheduleNodes);
                if(res < 0)
                    return res;
            } else {
                mapGraphOutportGenerationSrc[graphOutport] = srcOutport;
            }
            
        }
    }
    return 1;
}

int ILGeneratorDataflow::generateILGraphOutportAssignForFunctionInport(const MIRScheduleGraphDataflow* schedule,
    ILObject* parent)
{
    int res;

    vector<ILScheduleNode*>* scheduleNodes = nullptr;
    if(parent->objType == ILObject::TypeSchedule)
        scheduleNodes = &static_cast<ILSchedule*>(parent)->scheduleNodes;
    else if(parent->objType == ILObject::TypeBranch)
        scheduleNodes = &static_cast<ILBranch*>(parent)->scheduleNodes;

    unordered_map<MIRScheduleGraphOutport*, bool> assignedOutport;
    for(int i = 0; i < schedule->outports.size(); i++)
    {
        assignedOutport.clear();
        MIRScheduleGraphInport* graphOutport = schedule->outports[i];
        

        for(int j = 0; j < graphOutport->srcOutports.size(); j++)
        {
            MIRScheduleGraphOutport* srcOutport = graphOutport->srcOutports[j];
            if(!srcOutport->inportFromFunction ||
                assignedOutport.find(srcOutport) != assignedOutport.end()) {
                continue;
            }
            assignedOutport[srcOutport] = true;

            // 先确定是该组件对该端口进行了赋值，才能进行有效地类型推断
            res = generateILGraphOutportAssignInferenceType(graphOutport, srcOutport);
            if(res < 0)
                return res;
            res = generateILGraphOutportAssignCalculate(graphOutport, srcOutport, parent, scheduleNodes);
            if(res < 0)
                return res;
        }
    }
    return 1;
}

int ILGeneratorDataflow::generateILGraphOutportAssignInferenceType(MIRScheduleGraphInport* graphOutport, MIRScheduleGraphOutport* srcOutport)
{
    MIROutport* mIROutport = graphOutport->outportFromFunction;
    if(mIROutport->type == "#Default")
    {
        if(srcOutport->inportFromFunction && srcOutport->inportFromFunction->type != "#Default") {
            mIROutport->type = srcOutport->inportFromFunction->type;
        }
        else if(srcOutport->outportFromActor && srcOutport->outportFromActor->type != "#Default") {
            mIROutport->type = srcOutport->outportFromActor->type;
        }
    }
    if(mIROutport->arraySize.empty())
        {
        if(srcOutport->inportFromFunction && !srcOutport->inportFromFunction->arraySize.empty()) {
            mIROutport->arraySize = srcOutport->inportFromFunction->arraySize;
        }
        else if(srcOutport->outportFromActor && !srcOutport->outportFromActor->arraySize.empty()) {
            mIROutport->arraySize = srcOutport->outportFromActor->arraySize;
        }
    }
    return 1;
}

int ILGeneratorDataflow::generateILGraphOutportAssignCalculate(
    MIRScheduleGraphInport* graphOutport,
    MIRScheduleGraphOutport* srcOutport,
    ILObject* parent,
    vector<ILScheduleNode*>* scheduleNodes)
{
    int res;

    ILCalculate* iLCalculate = new ILCalculate();
    iLCalculate->parent = parent;
    scheduleNodes->push_back(iLCalculate);

    iLCalculate->name = graphOutport->outportFromFunction->name;

    ILRef* iLRef = new ILRef();
    iLRef->parent = iLCalculate;
    iLCalculate->refs.push_back(iLRef);
    iLRef->actor = "Outport";
    iLRef->path = graphOutport->containerGraph->name + "." + iLCalculate->name;

    ILInput* iLInput = new ILInput();
    iLInput->parent = iLCalculate;
    iLCalculate->inputs.push_back(iLInput);
    if(srcOutport->outportFromActor) {
        iLInput->name = srcOutport->outportFromActor->name;
        iLInput->type = srcOutport->outportFromActor->type;
        iLInput->arraySize = srcOutport->outportFromActor->arraySize;
        iLInput->isAddress = srcOutport->outportFromActor->isAddress;
        iLInput->sourceStr = srcOutport->containerNode->actor->name + "_" + srcOutport->outportFromActor->name;

    } else {
        iLInput->name = srcOutport->inportFromFunction->name;
        iLInput->type = srcOutport->inportFromFunction->type;
        iLInput->arraySize = srcOutport->inportFromFunction->arraySize;
        iLInput->isAddress = srcOutport->inportFromFunction->isAddress;
        iLInput->sourceStr = srcOutport->inportFromFunction->name;
    }

    ILOutput* iLOutnput = new ILOutput();
    iLOutnput->parent = iLCalculate;
    iLCalculate->outputs.push_back(iLOutnput);
    iLOutnput->name = graphOutport->outportFromFunction->name;
    iLOutnput->type = graphOutport->outportFromFunction->type;
    iLOutnput->arraySize = graphOutport->outportFromFunction->arraySize;
    iLOutnput->isAddress = graphOutport->outportFromFunction->isAddress;

    res = generateILGraphOutportAssignCalculateStatement(graphOutport, srcOutport, iLCalculate);
    if(res < 0)
        return res;
    return 1;
}

int ILGeneratorDataflow::generateILGraphOutportAssignCalculateStatement(MIRScheduleGraphInport* graphOutport,
    MIRScheduleGraphOutport* srcOutport, ILCalculate* retILCalculate)
{
    if(!graphOutport->outportFromFunction->arraySize.empty())
    {
        StmtBatchCalculation* stmt = new StmtBatchCalculation();
        stmt->operationType = StmtBatchCalculation::TypeAssign;
        stmt->dataType = graphOutport->outportFromFunction->type;


        StmtBatchCalculation::StmtBatchCalculationInput stmtInput;
        if(srcOutport->outportFromActor) {
            stmtInput.name = srcOutport->containerNode->actor->name + "_" + srcOutport->outportFromActor->name;
            stmtInput.type = srcOutport->outportFromActor->type;
            stmtInput.arraySize = srcOutport->outportFromActor->arraySize;

        } else {
            stmtInput.name = srcOutport->inportFromFunction->name;
            stmtInput.type = srcOutport->inportFromFunction->type;
            stmtInput.arraySize = srcOutport->inportFromFunction->arraySize;
        }
        stmt->inputs.push_back(stmtInput);

        StmtBatchCalculation::StmtBatchCalculationOutput stmtOutput;
        stmtOutput.name = graphOutport->outportFromFunction->name;
        stmtOutput.type = graphOutport->outportFromFunction->type;
        stmtOutput.arraySize = graphOutport->outportFromFunction->arraySize;
        stmt->outputs.push_back(stmtOutput);

        stmt->parentStatement = &retILCalculate->statements;
        retILCalculate->statements.childStatements.push_back(stmt);
    }
    else
    {
        StmtExpression* stmt = new StmtExpression();
        string expStr;
        string srcVarStr;
        StmtBatchCalculation::StmtBatchCalculationInput stmtInput;
        if(srcOutport->outportFromActor) {
            srcVarStr = srcOutport->containerNode->actor->name + "_" + srcOutport->outportFromActor->name;

        } else {
            srcVarStr = srcOutport->inportFromFunction->name;
        }
        string dstVarStr;
        dstVarStr = graphOutport->outportFromFunction->name;
        expStr = "*" + dstVarStr + " = " + srcVarStr + ";";
        ILExpressionParser expParser;
        stmt->expression = expParser.parseExpression(expStr);

        stmt->parentStatement = &retILCalculate->statements;
        retILCalculate->statements.childStatements.push_back(stmt);
    }
    
    return 1;
}

int ILGeneratorDataflow::generateILUpdateForDelayActor(
    const CodeGenerationTable& CGT)
{
    int res;

    CodeGenerationTable::Iterator iter(CGT);
    const CGTElement* element = iter.next();
    while(element)
    {
        if(element->type != CGTElement::TypeActorExe) {
            element = iter.next();
            continue;
        }
        CGTActorExe* actorExe = static_cast<CGTActorExe*>(const_cast<CGTElement*>(element));
        MIRActor* actor = actorExe->node->actor;
        if(actor->type != "DelayRead") {
            element = iter.next();
            continue;
        }
        CGTActorExe* delayReadActorExe = actorExe;
        CGTActorExe* delayStoreActorExe = generateILUpdateForDelayActorOnceFindDelayStore(CGT, delayReadActorExe);
        if(!delayStoreActorExe)
            return -MIRScheduleAnalyzer::ErrorInnerCanNotFindDelayStoreActorExeByDelayRead;

        MIRActor* delayReadActor = delayReadActorExe->node->actor;
        MIRActor* delayStoreActor = delayStoreActorExe->node->actor;

        bool isTypeSame = delayReadActor->outports[0]->type == delayStoreActor->inports[0]->type;
        bool isArraySizeSame = true;
        if(delayReadActor->outports[0]->arraySize.size() != delayStoreActor->inports[0]->arraySize.size()) {
            isArraySizeSame = false;
        }
        for(int j = 0; isArraySizeSame && j < delayReadActor->outports[0]->arraySize.size(); j++)
        {
            if(delayReadActor->outports[0]->arraySize[j] != delayStoreActor->inports[0]->arraySize[j]) {
                isArraySizeSame = false;
                break;
            }
        }
        
        if(isTypeSame && isArraySizeSame) {
            element = iter.next();
            continue;
        }

        delayReadActor->outports[0]->type = delayStoreActor->inports[0]->type;
        delayReadActor->outports[0]->arraySize = delayStoreActor->inports[0]->arraySize;

        res = generateILUpdateForDelayActorOnce(CGT, actorExe);
        if(res < 0)
            return res;

        //如果找到一个新的未进行类型推导的DelayReadActor，则从头寻找新的未进行类型推导的DelayReadActor
        
        iter.init(CGT);
        element = iter.next();
    }
    return 1;
}

int ILGeneratorDataflow::generateILUpdateForDelayActorOnce(const CodeGenerationTable &CGT,
    CGTActorExe* actorExeDelayRead)
{
    int res;
    

    vector<CGTActorExe*> updateActorExeQueue;
    updateActorExeQueue.push_back(actorExeDelayRead);

    while(!updateActorExeQueue.empty())
    {
        CGTActorExe* updateActorExe = updateActorExeQueue[0];
        updateActorExeQueue.erase(updateActorExeQueue.begin());

        res = generateILInformationForActorExe(updateActorExe);
        if(res == 0)
            continue;
        if(res < 0)
            return res;

        vector<CGTActorExe*> succs = generateILUpdateForDelayActorOnceGetSuccessors(CGT, updateActorExe);

        for(int i = 0; i < succs.size(); i++)
        {
            bool found = false;
            for(int j = 0; j < updateActorExeQueue.size(); j++)
            {
                if(succs[i] == updateActorExeQueue[j])
                {
                    found = true;
                    break;
                }
            }
            if(!found)
            {
                updateActorExeQueue.push_back(succs[i]);
            }
        }

    }

    return 1;
}

CGTActorExe* ILGeneratorDataflow::generateILUpdateForDelayActorOnceFindDelayStore(
    const CodeGenerationTable& CGT, CGTActorExe* actorExeDelayRead)
{
    MIRActor* actorDelayRead = actorExeDelayRead->node->actor;
    string delayActorName = actorDelayRead->name.substr(0, actorDelayRead->name.length() - 5);
    CodeGenerationTable::Iterator iter(CGT);
    const CGTElement* element = iter.next();
    while(element)
    {
        if(element->type != CGTElement::TypeActorExe) {
            element = iter.next();
            continue;
        }
        CGTActorExe* actorExe = static_cast<CGTActorExe*>(const_cast<CGTElement*>(element));
        MIRActor* actor = actorExe->node->actor;
        if(actor->type != "DelayStore") {
            element = iter.next();
            continue;
        }
        string actorName = actor->name;
        if(!stringEndsWith(actorName, "_Store")) {
            element = iter.next();
            continue;
        }
        if(actorName.substr(0, actorName.length() - 6) == delayActorName)
        {
            return actorExe;
        }
        element = iter.next();
    }
    return nullptr;
}

std::vector<CGTActorExe*> ILGeneratorDataflow::generateILUpdateForDelayActorOnceGetSuccessors(
    const CodeGenerationTable& CGT, CGTActorExe* actorExe)
{
    vector<CGTActorExe*> ret;
    CodeGenerationTable::Iterator iter(CGT);
    const CGTElement* element = iter.next();
    while(element)
    {
        if(element->type != CGTElement::TypeActorExe) {
            element = iter.next();
            continue;
        }
        CGTActorExe* actorExeTemp = static_cast<CGTActorExe*>(const_cast<CGTElement*>(element));

        for(int i = 0; i < actorExeTemp->realBranchPathDataSrc.srcs.size(); i++)
        {
            MIRScheduleGraphOutport* srcOutput = actorExeTemp->realBranchPathDataSrc.srcs[i];
            if(srcOutput->containerNode && srcOutput->containerNode == actorExe->node)
            {
                ret.push_back(actorExeTemp);
            }
        }
        element = iter.next();
    }
    return ret;
}

int ILGeneratorDataflow::generateILActorExe(const CGTActorExe* actorExe,
    ILObject* parent, ILFunction* functionInit, ILFunction* functionExec, ILFile* iLFile)
{
    int res;

    MIRActor* mirActor = actorExe->node->actor;
    bool justTransExec = false;
    if(mirActor)
    {
        if(mapMIRActorGenerateCount.find(mirActor) == mapMIRActorGenerateCount.end())
        {
            mapMIRActorGenerateCount[mirActor] = 0;
        }
        else
        {
            justTransExec = true;
        }
        mapMIRActorGenerateCount[mirActor]++;
    }
    

    vector<ILScheduleNode*>* scheduleNodes = nullptr;
    if(parent->objType == ILObject::TypeSchedule)
        scheduleNodes = &static_cast<ILSchedule*>(parent)->scheduleNodes;
    else if(parent->objType == ILObject::TypeBranch)
        scheduleNodes = &static_cast<ILBranch*>(parent)->scheduleNodes;
    
    CGTActorExeTranslator actorExeTranslator;
    ILCalculate* iLCalculate = new ILCalculate();
    iLCalculate->parent = parent;
    scheduleNodes->push_back(iLCalculate);

    //res = actorExeTranslator.transCGTActorExe(actorExe, this->bassAnalyzer, functionInit, functionExec, iLCalculate, iLFile);
    //if(res < 0)
    //    return res;

    res = transILInformationToIL(mapCGTActorExeToILInformation.at(const_cast<CGTActorExe*>(actorExe)), iLFile, functionInit, functionExec, iLCalculate, justTransExec);
    if(res < 0)
        return res;
    res = generateILGraphOutportAssign(actorExe, parent);
    if(res < 0)
        return res;

    return 1;
}

int ILGeneratorDataflow::generateILOutportAssign(const MIRScheduleGraphInport* outport, ILObject* parent)
{
    int res;
    if(mapGraphOutportGenerationSrc.find(outport) != mapGraphOutportGenerationSrc.end())
    {
        MIRScheduleGraphInport* out = const_cast<MIRScheduleGraphInport*>(outport);
        MIRScheduleGraphOutport* src = const_cast<MIRScheduleGraphOutport*>(mapGraphOutportGenerationSrc.at(outport));
        vector<ILScheduleNode*>* scheduleNodes = nullptr;
        if(parent->objType == ILObject::TypeSchedule)
            scheduleNodes = &static_cast<ILSchedule*>(parent)->scheduleNodes;
        else if(parent->objType == ILObject::TypeBranch)
            scheduleNodes = &static_cast<ILBranch*>(parent)->scheduleNodes;

        res = generateILGraphOutportAssignCalculate(out, src, parent, scheduleNodes);
        if(res < 0)
            return res;
    }
    return 1;
}


std::string ILGeneratorDataflow::getDataflowNodeBranchConditionByBranchId(const MIRScheduleGraphDataflowNode* node, int branchId)
{
    if(!node->actor)
        return "error";

    if(node->actor->type == "BooleanSwitch")
    {
        if(branchId == 0)
            return node->actor->name + "_Ctrl";
        else
            return "!" + node->actor->name + "_Ctrl";
    }

    return "true";
}



int ILGeneratorDataflow::transILInformationToIL(const std::string& iLXMLStr, ILFile* retILFile, ILFunction* functionInit, ILFunction* functionUpdate, ILCalculate* retILCalculate, bool justTransExec)
{
    int res;
    tinyxml2::XMLDocument doc;
	tinyxml2::XMLError ret = doc.LoadFile(iLXMLStr.c_str(), iLXMLStr.length());
	if (ret != 0) {
		return -MIRScheduleAnalyzer::ErrorLoadActorReturnILXMLFail;
	}
	tinyxml2::XMLElement* root = doc.RootElement();

    if(!justTransExec && (res = transILInformationToILHeadFile(root, retILFile)) < 0) {
        return res;
    }
    if(!justTransExec && (res = transILInformationToILMacro(root, retILFile)) < 0) {
        return res;
    }
    if(!justTransExec && (res = transILInformationToILTypeDefine(root, retILFile)) < 0) {
        return res;
    }
    if(!justTransExec && (res = transILInformationToILEnumDefine(root, retILFile)) < 0) {
        return res;
    }
    if(!justTransExec && (res = transILInformationToILStructDefine(root, retILFile)) < 0) {
        return res;
    }
    if(!justTransExec && (res = transILInformationToILUtilityFunction(root, retILFile)) < 0) {
        return res;
    }
    if(!justTransExec && (res = transILInformationToILGlobalVariable(root, retILFile)) < 0) {
        return res;
    }
    if(!justTransExec && (res = transILInformationToILLocalVariable(root, functionUpdate)) < 0) {
        return res;
    }
    if(!justTransExec && (res = transILInformationToILInitCalculate(root, functionInit)) < 0) {
        return res;
    }
    if((res = transILInformationToILUpdateCalculate(root, retILCalculate)) < 0) {
        return res;
    }

    return 1;
}

int ILGeneratorDataflow::transILInformationToILHeadFile(tinyxml2::XMLElement* root, ILFile* retILFile)
{
    int res;
    ILParser tempILParser;
    tinyxml2::XMLElement* xmlNode = root->FirstChildElement("HeadFile");
    while(xmlNode)
    {
        ILHeadFile* retILHeadFile = new ILHeadFile();
        retILHeadFile->parent = retILFile;
	    retILFile->headFiles.push_back(retILHeadFile);

        res = tempILParser.parseHeadFileIndependent(xmlNode, retILHeadFile);
        if(res < 0)
            return res;
        xmlNode = xmlNode->NextSiblingElement("HeadFile");
    }
    return 1;
}

int ILGeneratorDataflow::transILInformationToILMacro(tinyxml2::XMLElement* root, ILFile* retILFile)
{
    int res;
    ILParser tempILParser;
    tinyxml2::XMLElement* xmlNode = root->FirstChildElement("Macro");
    while(xmlNode)
    {
        ILMacro* retILMacro = new ILMacro();
        retILMacro->parent = retILFile;

        res = tempILParser.parseMacroIndependent(xmlNode, retILMacro);
        if(res < 0)
            return res;

        ILMacro* find = nullptr;
        for(int i = 0; i < retILFile->macros.size(); i++)
        {
            if(retILFile->macros[i]->name == retILMacro->name && 
                retILFile->macros[i]->value == retILMacro->value)
            {
                find = retILFile->macros[i];
                break;
            }
        }
        if(!find)
        {
            retILFile->macros.push_back(retILMacro);
        }
        else
        {
            find->refs.insert(find->refs.end(), retILMacro->refs.begin(), retILMacro->refs.end());
            retILMacro->refs.clear();
            delete retILMacro;
        }
        xmlNode = xmlNode->NextSiblingElement("Macro");
    }
    return 1;
}

int ILGeneratorDataflow::transILInformationToILTypeDefine(tinyxml2::XMLElement* root, ILFile* retILFile)
{
    int res;
    ILParser tempILParser;
    tinyxml2::XMLElement* xmlNode = root->FirstChildElement("TypeDefine");
    while(xmlNode)
    {
        ILTypeDefine* retILTypeDefine = new ILTypeDefine();
        retILTypeDefine->parent = retILFile;
	    retILFile->typeDefines.push_back(retILTypeDefine);

        res = tempILParser.parseTypeDefineIndependent(xmlNode, retILTypeDefine);
        if(res < 0)
            return res;
        xmlNode = xmlNode->NextSiblingElement("TypeDefine");
    }
    return 1;
}

int ILGeneratorDataflow::transILInformationToILEnumDefine(tinyxml2::XMLElement* root, ILFile* retILFile)
{
    int res;
    ILParser tempILParser;
    tinyxml2::XMLElement* xmlNode = root->FirstChildElement("EnumDefine");
    while(xmlNode)
    {
        ILEnumDefine* retILEnumDefine = new ILEnumDefine();
        retILEnumDefine->parent = retILFile;
	    retILFile->enumDefines.push_back(retILEnumDefine);

        res = tempILParser.parseEnumDefineIndependent(xmlNode, retILEnumDefine);
        if(res < 0)
            return res;
        xmlNode = xmlNode->NextSiblingElement("EnumDefine");
    }
    return 1;
}

int ILGeneratorDataflow::transILInformationToILStructDefine(tinyxml2::XMLElement* root, ILFile* retILFile)
{
    int res;
    ILParser tempILParser;
    tinyxml2::XMLElement* xmlNode = root->FirstChildElement("StructDefine");
    while(xmlNode)
    {
        ILStructDefine* retILStructDefine = new ILStructDefine();
        retILStructDefine->parent = retILFile;
	    retILFile->structDefines.push_back(retILStructDefine);

        res = tempILParser.parseStructDefineIndependent(xmlNode, retILStructDefine);
        if(res < 0)
            return res;
        xmlNode = xmlNode->NextSiblingElement("StructDefine");
    }
    return 1;
}

int ILGeneratorDataflow::transILInformationToILUtilityFunction(tinyxml2::XMLElement* root, ILFile* retILFile)
{
    int res;
    ILParser tempILParser;
    tinyxml2::XMLElement* xmlNode = root->FirstChildElement("UtilityFunction");
    while(xmlNode)
    {
        ILUtilityFunction* retILUtilityFunction = new ILUtilityFunction();
        retILUtilityFunction->parent = retILFile;
	    retILFile->utilityFunctions.push_back(retILUtilityFunction);

        res = tempILParser.parseUtilityFunctionIndependent(xmlNode, retILUtilityFunction);
        if(res < 0)
            return res;
        xmlNode = xmlNode->NextSiblingElement("UtilityFunction");
    }
    return 1;
}

int ILGeneratorDataflow::transILInformationToILGlobalVariable(tinyxml2::XMLElement* root, ILFile* retILFile)
{
    int res;
    ILParser tempILParser;
    tinyxml2::XMLElement* xmlNode = root->FirstChildElement("GlobalVariable");
    while(xmlNode)
    {
        ILGlobalVariable* retILGlobalVariable = new ILGlobalVariable();
        retILGlobalVariable->parent = retILFile;
	    retILFile->globalVariables.push_back(retILGlobalVariable);

        res = tempILParser.parseGlobalVariableIndependent(xmlNode, retILGlobalVariable);
        if(res < 0)
            return res;
        xmlNode = xmlNode->NextSiblingElement("GlobalVariable");
    }
    return 1;
}

int ILGeneratorDataflow::transILInformationToILLocalVariable(tinyxml2::XMLElement* root, ILFunction* functionUpdate)
{
    int res;
    ILParser tempILParser;
    tinyxml2::XMLElement* xmlNode = root->FirstChildElement("LocalVariable");
    while(xmlNode)
    {
        ILLocalVariable* retILLocalVariable = new ILLocalVariable();
        retILLocalVariable->parent = functionUpdate->schedule;
	    functionUpdate->schedule->localVariables.push_back(retILLocalVariable);

        res = tempILParser.parseLocalVariableIndependent(xmlNode, retILLocalVariable);
        if(res < 0)
            return res;
        xmlNode = xmlNode->NextSiblingElement("LocalVariable");
    }
    return 1;
}

int ILGeneratorDataflow::transILInformationToILInitCalculate(tinyxml2::XMLElement* root, ILFunction* functionInit)
{
    tinyxml2::XMLElement* xmlNodeCalculate = root->FirstChildElement("InitCalculate");
    if(!xmlNodeCalculate) {
        return 0;
    }
    xmlNodeCalculate = xmlNodeCalculate->FirstChildElement("Calculate");
    if(!xmlNodeCalculate) {
        return 0;
    }
    int res;
    ILParser tempILParser;
    ILCalculate* retILCalculate = new ILCalculate();
    retILCalculate->parent = functionInit->schedule;
	functionInit->schedule->scheduleNodes.push_back(retILCalculate);

    res = tempILParser.parseCalculateIndependent(xmlNodeCalculate, retILCalculate);
    if(res < 0)
        return res;
    return 1;
}

int ILGeneratorDataflow::transILInformationToILUpdateCalculate(tinyxml2::XMLElement* root, ILCalculate* retILCalculate)
{
    tinyxml2::XMLElement* xmlNodeCalculate = root->FirstChildElement("UpdateCalculate");
    if(!xmlNodeCalculate) {
        return 0;
    }
    xmlNodeCalculate = xmlNodeCalculate->FirstChildElement("Calculate");
    if(!xmlNodeCalculate) {
        return 0;
    }
    int res;
    ILParser tempILParser;
    res = tempILParser.parseCalculateIndependent(xmlNodeCalculate, retILCalculate);
    if(res < 0)
        return res;
    return 1;
}

int ILGeneratorDataflow::transILTypeInference(const std::string& iLXMLStr, CGTActorExe* retActorExe)
{
    int res;
    tinyxml2::XMLDocument doc;
	tinyxml2::XMLError ret = doc.LoadFile(iLXMLStr.c_str(), iLXMLStr.length());
	if (ret != 0) {
		return -MIRScheduleAnalyzer::ErrorLoadActorReturnILXMLFail;
	}
	tinyxml2::XMLElement* root = doc.RootElement();

    ILCalculate* iLCalculate = new ILCalculate();

    if((res = transILInformationToILUpdateCalculate(root, iLCalculate)) < 0) {
        //iLCalculate->release();
        //delete iLCalculate;
        return res;
    }
    
    for(int i = 0; i < iLCalculate->inputs.size(); i++)
    {
        ILInput* iLInput = iLCalculate->inputs[i];
        if(!retActorExe->node) {
            continue;
        }
        MIRScheduleGraphInport* sgInput = retActorExe->node->getInportByName(iLInput->name);
        if(!sgInput || !sgInput->inportFromActor) {
            continue;
        }
        sgInput->inportFromActor->arraySize = iLInput->arraySize;
        sgInput->inportFromActor->type = iLInput->type;
    }
    for(int i = 0; i < iLCalculate->outputs.size(); i++)
    {
        ILOutput* iLOutput = iLCalculate->outputs[i];
        if(!retActorExe->node) {
            continue;
        }
        MIRScheduleGraphOutport* sgOutput = retActorExe->node->getOutportByName(iLOutput->name);
        if(!sgOutput || !sgOutput->outportFromActor) {
            continue;
        }
        sgOutput->outportFromActor->arraySize = iLOutput->arraySize;
        sgOutput->outportFromActor->type = iLOutput->type;
    }
    
    iLCalculate->release();
    delete iLCalculate;
    return 1;
}
