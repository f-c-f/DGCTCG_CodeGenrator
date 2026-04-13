#include "MIRScheduleAnalyzerDataflow.h"

#include <algorithm>
#include <functional>
#include <queue>
#include <stack>

#include "MIRScheduleAnalyzer.h"

#include "ILGenerator.h"
#include "MIRPreprocessor.h"
#include "MIRScheduleGraphActionPort.h"
#include "MIRScheduleGraphDataflowNode.h"
#include "MIRScheduleGraphInport.h"
#include "MIRScheduleGraphOutport.h"
#include "MIRScheduleGraphStateflow.h"
#include "../Common/Utility.h"
#include "../MIRBasic/MIRFunctionDataflow.h"
#include "../MIRBasic/MIRFunctionStateflow.h"

#include "CGTActorExeTranslator.h"


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

using namespace std;



MIRScheduleAnalyzerDataflow::MIRScheduleAnalyzerDataflow(MIRScheduleAnalyzer* bassAnalyzer)
{
    this->bassAnalyzer = bassAnalyzer;
}

MIRScheduleAnalyzerDataflow::~MIRScheduleAnalyzerDataflow()
{
}

int MIRScheduleAnalyzerDataflow::collectScheduleFunction(MIRSchedule* schedule, MIRFunction* function)
{
    if(function->type == MIRFunction::TypeDataflow)
    {
        return collectScheduleFunctionDataflow(schedule, reinterpret_cast<MIRFunctionDataflow*>(function));
    }
    return -MIRScheduleAnalyzer::ErrorUnknownMIRFunctionType;
}

int MIRScheduleAnalyzerDataflow::collectScheduleFunctionDataflow(MIRSchedule* schedule, MIRFunctionDataflow* function)
{
    MIRScheduleGraphDataflow* graph = new MIRScheduleGraphDataflow();
    schedule->scheduleGraphs.push_back(graph);
    graph->name = function->name;

    for(int i = 0; i < function->actors.size(); i++)
    {
        int res = collectScheduleFunctionDataflowNodes(graph, function->actors[i]);
        if(res < 0)
            return res;
    }
    // 输入输出端口相对外部是正常的输入输出端口，
    // 但相对内部，输入端口则对应outport，输出端口则对应inport
    for(int i = 0; i < function->outports.size(); i++)
    {
        int res = collectScheduleFunctionOutports(graph, function->outports[i]);
        if(res < 0)
            return res;
    }
    for(int i = 0; i < function->inports.size(); i++)
    {
        int res = collectScheduleFunctionInports(graph, function->inports[i]);
        if(res < 0)
            return res;
    }
    for(int i = 0; i < function->relations.size(); i++)
    {
        int res = collectScheduleFunctionDataflowRelation(graph, function, function->relations[i]);
        if(res < 0)
            return res;
    }
    for(int i = 0; i < graph->nodes.size(); i++)
    {
        int res = collectScheduleFunctionFunctionCallOrder(graph, graph->nodes[i]);
        if(res < 0)
            return res;
    }

    return 1;
}

int MIRScheduleAnalyzerDataflow::collectScheduleFunctionDataflowNodes(MIRScheduleGraphDataflow* scheduleGraphDataflow,
    MIRActor* actor)
{
    MIRScheduleGraphDataflowNode* node = new MIRScheduleGraphDataflowNode();
    scheduleGraphDataflow->actorToNodeMap[actor] = node;
    scheduleGraphDataflow->nodes.push_back(node);
    node->actor = actor;
    node->graph = scheduleGraphDataflow;

    for(int i = 0; i < actor->inports.size(); i++)
    {
        MIRScheduleGraphInport* inport = new MIRScheduleGraphInport();
        node->inports.push_back(inport);
        inport->containerNode = node;
        inport->inportFromActor = actor->inports[i];
    }

    for(int i = 0; i < actor->outports.size(); i++)
    {
        MIRScheduleGraphOutport* outport = new MIRScheduleGraphOutport();
        node->outports.push_back(outport);
        outport->containerNode = node;
        outport->outportFromActor = actor->outports[i];
    }

    for(int i = 0; i < actor->actionPorts.size(); i++)
    {
        MIRScheduleGraphActionPort* actionPort = new MIRScheduleGraphActionPort();
        node->actionPorts.push_back(actionPort);
        actionPort->containerNode = node;
        actionPort->actionPort = actor->actionPorts[i];
    }
    
    return 1;
}

int MIRScheduleAnalyzerDataflow::collectScheduleFunctionDataflowRelation(MIRScheduleGraphDataflow* scheduleGraphDataflow, MIRFunctionDataflow* function,
    MIRRelation* relation)
{
    for(int i = 0; i < relation->srcObjs.size(); i++)
    {
        MIRScheduleGraphOutport* srcOutport = findScheduleGraphOutport(scheduleGraphDataflow, relation->srcObjs[i]);
        if(!srcOutport)
            return -MIRScheduleAnalyzer::ErrorInnerOutportNotFound;

        for(int j = 0; j < relation->dstObjs.size(); j++)
        {
            MIRScheduleGraphInport* srcInport = nullptr;
            MIRScheduleGraphActionPort* srcActionPort = nullptr;
            if(relation->dstObjs[j]->objType == MIRObject::TypeInport || relation->dstObjs[j]->objType == MIRObject::TypeOutport) {
                srcInport = findScheduleGraphInport(scheduleGraphDataflow, static_cast<MIRInport*>(relation->dstObjs[j]));
            }else if (relation->dstObjs[j]->objType == MIRObject::TypeActionPort) {
                srcActionPort = findScheduleGraphActionPort(scheduleGraphDataflow, static_cast<MIRActionPort*>(relation->dstObjs[j]));
            }
            if(srcInport)
            {
                srcInport->srcOutports.push_back(srcOutport);
                srcOutport->dstInports.push_back(srcInport);
            }
            else if(srcActionPort)
            {
                srcActionPort->srcOutports.push_back(srcOutport);
                srcOutport->dstActionPorts.push_back(srcActionPort);
            }
            else
            {
                return -MIRScheduleAnalyzer::ErrorInnerInportOrActionPortNotFound;
            }
        }
    }
    return 1;
}

int MIRScheduleAnalyzerDataflow::collectScheduleFunctionFunctionCallOrder(MIRScheduleGraphDataflow* scheduleGraphDataflow, 
    MIRScheduleGraphDataflowNode* node)
{
    if(node->actor->type != "Function")
        return 0;

    vector<MIRScheduleGraphDataflowNode*> pred = getNodePredecessors(node);
    MIRScheduleGraphDataflowNode* demuxNode = nullptr;
    for(int i = 0; i < pred.size(); i++) {
        if(pred[i]->actor->type == "Demux") {
            demuxNode = pred[i];
            break;
        }
    }
    if(!demuxNode)
        return 0;

    vector<MIRScheduleGraphDataflowNode*> succ = getNodeSuccessors(demuxNode);
    for(int i = 0; i < succ.size(); i++) {
        if(succ[i] == node)
            break;
        if(succ[i]->actor->type != "Function")
            continue;
        
        vector<MIRScheduleGraphDataflowNode*> predAll = getNodeAllPredecessors(succ[i]);
        if(find(predAll.begin(), predAll.end(), node) != predAll.end()) {
            //scheduleGraphDataflow->necessaryPredecessorNodeMap[succ[i]].push_back(node);  // 没必要的前驱设置，因为已经存在数据连接了
        } else {
            scheduleGraphDataflow->necessaryPredecessorNodeMap[node].push_back(succ[i]);
        }
    }
    return 1;
}

MIRScheduleGraphOutport* MIRScheduleAnalyzerDataflow::findScheduleGraphOutport(MIRScheduleGraphDataflow* scheduleGraphDataflow,
    MIRObject* outport)
{
    for(int i = 0; i < scheduleGraphDataflow->inports.size(); i++)
    {
        if(scheduleGraphDataflow->inports[i]->inportFromFunction == outport)
            return scheduleGraphDataflow->inports[i];
    }
    for(int i = 0; i < scheduleGraphDataflow->nodes.size(); i++)
    {
        MIRScheduleGraphDataflowNode* node = scheduleGraphDataflow->nodes[i];
        for(int i = 0; i < node->outports.size(); i++)
        {
            if(node->outports[i]->outportFromActor == outport)
                return node->outports[i];
        }
    }
    return nullptr;
}

MIRScheduleGraphInport* MIRScheduleAnalyzerDataflow::findScheduleGraphInport(MIRScheduleGraphDataflow* scheduleGraphDataflow,
    MIRObject* inport)
{
    for(int i = 0; i < scheduleGraphDataflow->outports.size(); i++)
    {
        if(scheduleGraphDataflow->outports[i]->outportFromFunction == inport)
            return scheduleGraphDataflow->outports[i];
    }
    for(int i = 0; i < scheduleGraphDataflow->nodes.size(); i++)
    {
        MIRScheduleGraphDataflowNode* node = scheduleGraphDataflow->nodes[i];
        for(int i = 0; i < node->inports.size(); i++)
        {
            if(node->inports[i]->inportFromActor == inport)
                return node->inports[i];
        }
    }
    return nullptr;
}


MIRScheduleGraphActionPort* MIRScheduleAnalyzerDataflow::findScheduleGraphActionPort(
    MIRScheduleGraphDataflow* scheduleGraphDataflow, MIRActionPort* actionPort)
{
    for(int i = 0; i < scheduleGraphDataflow->nodes.size(); i++)
    {
        MIRScheduleGraphDataflowNode* node = scheduleGraphDataflow->nodes[i];
        for(int i = 0; i < node->actionPorts.size(); i++)
        {
            if(node->actionPorts[i]->actionPort == actionPort)
                return node->actionPorts[i];
        }
    }
    return nullptr;
}


int MIRScheduleAnalyzerDataflow::collectScheduleFunctionInports(MIRScheduleGraph* scheduleGraph,
    MIRInport* inport)
{
    MIRScheduleGraphOutport* graphInport = new MIRScheduleGraphOutport();
    scheduleGraph->inports.push_back(graphInport);
    graphInport->containerGraph = scheduleGraph;
    graphInport->inportFromFunction = inport;
    return 1;
}

int MIRScheduleAnalyzerDataflow::collectScheduleFunctionOutports(MIRScheduleGraph* scheduleGraph,
    MIROutport* outport)
{
    MIRScheduleGraphInport* graphOutport = new MIRScheduleGraphInport();
    scheduleGraph->outports.push_back(graphOutport);
    graphOutport->containerGraph = scheduleGraph;
    graphOutport->outportFromFunction = outport;
    return 1;
}



int MIRScheduleAnalyzerDataflow::calculateScheduleGraph(MIRScheduleGraph* schedule)
{
    if(schedule->type == MIRScheduleGraph::TypeDataflow)
    {
        return calculateScheduleGraphDataflow(reinterpret_cast<MIRScheduleGraphDataflow*>(schedule));
    }
    return -MIRScheduleAnalyzer::ErrorUnknownMIRFunctionType;
}

int MIRScheduleAnalyzerDataflow::calculateScheduleGraphDataflow(MIRScheduleGraphDataflow* schedule)
{
    int res;

    res = topologicalSort(schedule);
    if(res < 0)
    {
        return res;
    }

    map<MIRScheduleGraphDataflowNode*, BranchInfo> branchInfos;
    res = branchMarking(schedule, branchInfos);
    if(res < 0)
    {
        return res;
    }
    
    res = buildCodeGenerationTable(schedule, branchInfos, schedule->CGT);
    if(res < 0)
    {
        return res;
    }

    //CGT.print();

    //ILFile* iLFileForMIRScheduleGraph = nullptr;
    //res = generateIL(schedule, CGT, &iLFileForMIRScheduleGraph);
    //if(res < 0)
    //    return res;
    //
    //schedule->iLFile = iLFileForMIRScheduleGraph;
    
    //CGT.release();

    //tinyxml2::XMLDocument doc;
	//tinyxml2::XMLElement* root = doc.NewElement("Root");
    //doc.InsertEndChild(root);
    //
    //ILSaver iLSaver;
    //iLSaver.saveFile(iLFileForMIRScheduleGraph, root, &doc);
    //
    //tinyxml2::XMLPrinter printer;
	//doc.Print(&printer);
    //
	//printf("%s\n", printer.CStr());

    return 1;
}

int MIRScheduleAnalyzerDataflow::topologicalSort(MIRScheduleGraphDataflow* schedule)
{
    int res;
    std::vector<MIRScheduleGraphDataflowNode*> nodes = schedule->nodes;

    std::vector<MIRScheduleGraphDataflowNode*> curLayer;

    int currentLayerId = 0;

    while(!nodes.empty())
    {
        for(int i = 0; i < nodes.size(); i++)
        {
            if(topologicalSortIsNodeNoPred(nodes[i], nodes))
            {
                curLayer.push_back(nodes[i]);
            }
        }
        res = topologicalSortSwapDelayActor(curLayer);
        if(res < 0) {
            return res;
        }

        schedule->layers[currentLayerId] = curLayer;
        for(int i = 0; i < curLayer.size(); i++)
        {
            auto iter = find(nodes.begin(), nodes.end(), curLayer[i]);
            if(iter == nodes.end())
            {
                return -MIRScheduleAnalyzer::ErrorInnerTopologicalSortNodeNotFound;
            }
            nodes.erase(iter);
        }
        
        curLayer.clear();
        currentLayerId++;
    }

    return 1;
}

int MIRScheduleAnalyzerDataflow::topologicalSortSwapDelayActor(
    std::vector<MIRScheduleGraphDataflowNode*>& scheduleLayer)
{
    map<string, vector<int>> delayActorGroupList;
    for(int i = 0; i < scheduleLayer.size(); i++)
    {
        MIRScheduleGraphDataflowNode* node = scheduleLayer[i];
        string name = node->actor->name;
        if(node->actor->type == "DelayRead") {
            name = name.substr(0, name.length() - 5);
        }
        if(node->actor->type == "DelayStore") {
            name = name.substr(0, name.length() - 6);
        }
        delayActorGroupList[name].push_back(i);
    }
    map<string, vector<int>>::iterator iter = delayActorGroupList.begin();
    for(; iter != delayActorGroupList.end(); iter++)
    {
        if(iter->second.size() != 2)
            continue;

        int id1 = iter->second[0];
        int id2 = iter->second[1];
        MIRScheduleGraphDataflowNode* node1 = scheduleLayer[id1];
        MIRScheduleGraphDataflowNode* node2 = scheduleLayer[id2];

        if(!(node1->actor->type == "DelayStore" && node2->actor->type == "DelayRead")) {
            continue;
        }

        scheduleLayer[id1] = node2;
        scheduleLayer[id2] = node1;
    }
    return 1;
}

bool MIRScheduleAnalyzerDataflow::topologicalSortIsNodeNoPred(
    MIRScheduleGraphDataflowNode* node,
    std::vector<MIRScheduleGraphDataflowNode*> lastNodes)
{
    std::vector<MIRScheduleGraphDataflowNode*> preds = getNodePredecessors(node);
    if(node->graph->necessaryPredecessorNodeMap.find(node) != node->graph->necessaryPredecessorNodeMap.end())
    {
        vector<MIRScheduleGraphDataflowNode*>* necessaryPreds = &(node->graph->necessaryPredecessorNodeMap[node]);
        preds.insert(preds.end(), necessaryPreds->begin(), necessaryPreds->end());
    }

    for(int i = 0; i < preds.size(); i++)
    {
        auto iter = find(lastNodes.begin(), lastNodes.end(), preds[i]);
        
        if(iter != lastNodes.end())
        {
            if(*iter == node)
            {
                continue;
            }
            return false;
        }
    }
    return true;
}

int MIRScheduleAnalyzerDataflow::branchMarking(MIRScheduleGraphDataflow* schedule, map<MIRScheduleGraphDataflowNode*, BranchInfo> &retBranchInfos)
{
    int res;
    for(int i = 0; i < schedule->layers.size(); i++)
    {
        for(int j = 0; j < schedule->layers[i].size(); j++)
        {
            res = branchMarkingOneNode(schedule->layers[i][j], retBranchInfos);
            if(res < 0)
            {
                return res;
            }
        }
    }

    //branchInfoPrint(schedule, retBranchInfos);

    return 1;
}

int MIRScheduleAnalyzerDataflow::branchMarkingOneNode(MIRScheduleGraphDataflowNode* node, map<MIRScheduleGraphDataflowNode*, BranchInfo>& retBranchInfos)
{
    std::vector<MIRScheduleGraphDataflowNode*> preds = getNodePredecessors(node);
    std::vector<MIRScheduleGraphOutport*> predsOutports = getNodePredecessorsOutports(node);
    if(preds.empty() && predsOutports.empty())
    {
        // 构建一个空的分支信息
        BranchInfo tempBranchInfo;
        BranchPath tempBranchPath;
        BranchPathDataSrc tempBranchPathDataSrc;
        Branch tempBranch;
        tempBranch.branchId = 0;
        tempBranchPath.branchs.push_back(tempBranch);
        tempBranchInfo.branchPathWithSrc.push_back(pair<BranchPath, BranchPathDataSrc>(tempBranchPath, tempBranchPathDataSrc));

        retBranchInfos[node] = tempBranchInfo;

        return 1;
    }
    
    int res;
    BranchInfo branchInfo;
    for(int i = 0; i < predsOutports.size(); i++)
    {
        MIRScheduleGraphOutport* predOutport = predsOutports[i];
        MIRScheduleGraphDataflowNode* pred = predOutport->containerNode;

        BranchInfo* branchInfoOfPred = NULL;
        if(pred)
            branchInfoOfPred = &retBranchInfos[predOutport->containerNode];

        if(!branchInfoOfPred)
        {
            res = branchMarkingOneNodePredIsInport(predOutport, branchInfoOfPred, branchInfo);
        }
        else if(isDataflowNodeBranchNode(pred))
        {
            res = branchMarkingOneNodePredIsBranch(predOutport, branchInfoOfPred, branchInfo);
        }
        else
        {
            res = branchMarkingOneNodePredIsNotBranch(predOutport, branchInfoOfPred, branchInfo);
        }
        if(res < 0)
        {
            return res;
        }
    }

    res = branchInfoSimplify(node, branchInfo, retBranchInfos);
    if(res < 0)
    {
        return res;
    }

    retBranchInfos[node] = branchInfo;
    return 1;
}

int MIRScheduleAnalyzerDataflow::branchMarkingOneNodePredIsInport(MIRScheduleGraphOutport* predOutport,
    BranchInfo* branchInfoOfPred, BranchInfo& branchInfo)
{
    // 构建一个空的分支信息
    BranchInfo tempBranchInfo;
    BranchPath tempBranchPath;
    Branch tempBranch;
    tempBranch.branchId = 0;
    tempBranchPath.branchs.push_back(tempBranch);
    BranchPathDataSrc tempBranchPathDataSrc;
    tempBranchPathDataSrc.srcs.push_back(predOutport);
    tempBranchInfo.branchPathWithSrc.push_back(pair<BranchPath, BranchPathDataSrc>(tempBranchPath, tempBranchPathDataSrc));
    
    
    int res = branchMarkingOneNodePredAddBranchPathWithSrc(tempBranchInfo, branchInfo);
    if(res < 0)
        return res;

    return 1;
}

int MIRScheduleAnalyzerDataflow::branchMarkingOneNodePredIsBranch(MIRScheduleGraphOutport* predOutport,
    BranchInfo* branchInfoOfPred, BranchInfo &branchInfo)
{
    MIRScheduleGraphDataflowNode* pred = predOutport->containerNode;

    BranchInfo tempBranchInfo;
    for(int i = 0; i < branchInfoOfPred->branchPathWithSrc.size(); i++)
    {
        BranchPath bp = branchInfoOfPred->branchPathWithSrc[i].first;
        Branch branch;
        branch.branchNode = pred;
        branch.branchId = getDataflowNodeBranchId(pred, predOutport);
        bp.branchs.push_back(branch);

        BranchPathDataSrc bpDataSrc;
        bpDataSrc.srcs.push_back(predOutport);
        
        tempBranchInfo.branchPathWithSrc.push_back(pair<BranchPath, BranchPathDataSrc>(bp, bpDataSrc));
    }

    int res = branchMarkingOneNodePredAddBranchPathWithSrc(tempBranchInfo, branchInfo);
    if(res < 0)
        return res;

    return 1;
}

int MIRScheduleAnalyzerDataflow::branchMarkingOneNodePredIsNotBranch(MIRScheduleGraphOutport* predOutport,
    BranchInfo* branchInfoOfPred, BranchInfo &branchInfo)
{
    MIRScheduleGraphDataflowNode* pred = predOutport->containerNode;

    BranchInfo tempBranchInfo;
    for(int i = 0; i < branchInfoOfPred->branchPathWithSrc.size(); i++)
    {
        BranchPath bp = branchInfoOfPred->branchPathWithSrc[i].first;

        BranchPathDataSrc bpDataSrc;
        bpDataSrc.srcs.push_back(predOutport);
        
        tempBranchInfo.branchPathWithSrc.push_back(pair<BranchPath, BranchPathDataSrc>(bp, bpDataSrc));
    }

    int res = branchMarkingOneNodePredAddBranchPathWithSrc(tempBranchInfo, branchInfo);
    if(res < 0)
        return res;

    return 1;
}

int MIRScheduleAnalyzerDataflow::branchMarkingOneNodePredAddBranchPathWithSrc(const BranchInfo& srcBranchInfo,
    BranchInfo& dstBranchInfo)
{
    for(int i = 0; i < srcBranchInfo.branchPathWithSrc.size(); i++)
    {
        const BranchPath &srcBp = srcBranchInfo.branchPathWithSrc[i].first;

        bool found = false;
        for(int j = 0; j < dstBranchInfo.branchPathWithSrc.size(); j++)
        {
            BranchPath &dstBp = dstBranchInfo.branchPathWithSrc[j].first;

            if(!areTwoBranchPathSame(srcBp, dstBp))
            {
                continue;
            }

            const BranchPathDataSrc &srcBpDataSrc = srcBranchInfo.branchPathWithSrc[i].second;
            BranchPathDataSrc &dstBpDataSrc = dstBranchInfo.branchPathWithSrc[j].second;

            mergeTwoBranchPathDataSrc(srcBpDataSrc, dstBpDataSrc);
            found = true;
            break;
        }

        if(!found)
        {
            dstBranchInfo.branchPathWithSrc.push_back(srcBranchInfo.branchPathWithSrc[i]);
        }
    }

    return 1;
}

int MIRScheduleAnalyzerDataflow::branchInfoSimplify(MIRScheduleGraphDataflowNode* node, BranchInfo& branchInfo, const map<MIRScheduleGraphDataflowNode*, BranchInfo>& retBranchInfos)
{
    std::sort(branchInfo.branchPathWithSrc.begin(), branchInfo.branchPathWithSrc.end(), sortBranchInfoByBranchPathLength);
    
    int res = branchInfoSimplifyMergeSource(branchInfo);
    if(res < 0)
    {
        return res;
    }

    if(node->actor && node->actor->type == "BooleanSwitch")
    {
        res = branchInfoSimplifyRemoveBooleanSwitchCtrlBranchInfo(node, branchInfo, retBranchInfos);
        if(res < 0)
        {
            return res;
        }
    }
    
    res = branchInfoSimplifyMergeBranch(branchInfo);
    if(res < 0)
    {
        return res;
    }
    return 1;
}

int MIRScheduleAnalyzerDataflow::branchInfoSimplifyRemoveBooleanSwitchCtrlBranchInfo(MIRScheduleGraphDataflowNode* node, BranchInfo& branchInfo, const map<MIRScheduleGraphDataflowNode*, BranchInfo>& retBranchInfos)
{
    vector<bool> reserved;
    for(int i = 0; i < branchInfo.branchPathWithSrc.size(); i++)
        reserved.push_back(false);
    MIRScheduleGraphInport* inport = node->inports[0];
    for(int k = 0; k < branchInfo.branchPathWithSrc.size(); k++)
    {
        const BranchPath& branchPath = branchInfo.branchPathWithSrc[k].first;
        bool find = false;
        for(int j = 0; !find && j < inport->srcOutports.size(); j++)
        {
            MIRScheduleGraphOutport* src = inport->srcOutports[j];
            const BranchInfo& branchInfoOfPred = retBranchInfos.at(src->containerNode);
            for(int i = 0; !find && i < branchInfoOfPred.branchPathWithSrc.size(); i++)
            {
                BranchPath predBranchPath = branchInfoOfPred.branchPathWithSrc[i].first;

                if(isDataflowNodeBranchNode(src->containerNode))
                {
                    Branch branch;
                    branch.branchNode = src->containerNode;
                    branch.branchId = getDataflowNodeBranchId(src->containerNode, src);
                    predBranchPath.branchs.push_back(branch);
                }
                
                if(areTwoBranchPathSame(branchPath, predBranchPath))
                {
                    find = true;
                    reserved[k] = true;
                }
            }
        }
    }
    for(int i = reserved.size() - 1; i >= 0; i--)
    {
        if(!reserved[i])
            branchInfo.branchPathWithSrc.erase(branchInfo.branchPathWithSrc.begin() + i);
    }
    return 1;
}

int MIRScheduleAnalyzerDataflow::branchInfoSimplifyMergeSource(BranchInfo& branchInfo)
{
    
    //a. 将父级分支路径上的数据源合并到子级分支路径上
	//O,A1  v   =>  O,A1  v,w
	//O     w   =>  O     w
    for(int i = 0; i < branchInfo.branchPathWithSrc.size(); i++)
    {
        for(int j = i + 1; j < branchInfo.branchPathWithSrc.size(); j++)
        {
            const BranchPath &srcBp = branchInfo.branchPathWithSrc[i].first;
            const BranchPath &dstBp = branchInfo.branchPathWithSrc[j].first;
            if(!isBranchPathContainBranchPath(srcBp, dstBp))
            {
                continue;
            }
            const BranchPathDataSrc &srcBpDataSrc = branchInfo.branchPathWithSrc[i].second;
            BranchPathDataSrc &dstBpDataSrc = branchInfo.branchPathWithSrc[j].second;
            mergeTwoBranchPathDataSrc(srcBpDataSrc, dstBpDataSrc);
        }
    }

    //b. 如果父级分支路径的数据源和子级分支路径的数据源一致，则删除子级分支路径
	//O,A1  v   =>  
	//O     v   =>  O     v
    set<int> removedId;
    for(int i = 0; i < branchInfo.branchPathWithSrc.size(); i++)
    {
        for(int j = i + 1; j < branchInfo.branchPathWithSrc.size(); j++)
        {
            const BranchPath &srcBp = branchInfo.branchPathWithSrc[i].first;
            const BranchPath &dstBp = branchInfo.branchPathWithSrc[j].first;
            if(!isBranchPathContainBranchPath(srcBp, dstBp))
            {
                continue;
            }
            const BranchPathDataSrc &srcBpDataSrc = branchInfo.branchPathWithSrc[i].second;
            const BranchPathDataSrc &dstBpDataSrc = branchInfo.branchPathWithSrc[j].second;
            if(!areTwoBranchPathDataSrcSame(srcBpDataSrc, dstBpDataSrc))
            {
                continue;
            }
            removedId.insert(j);
        }
    }
    auto iter = removedId.rbegin();
    for(; iter != removedId.rend(); iter++)
    {
        int id = *iter;
        branchInfo.branchPathWithSrc.erase(branchInfo.branchPathWithSrc.begin() + id);
    }

    return 1;
}

int MIRScheduleAnalyzerDataflow::branchInfoSimplifyMergeBranch(BranchInfo& branchInfo)
{
    // 由于branchInfo已经按照分支路径从短到长排序，这里按照从长到段的逆序进行处理
    int currentHandleCount = 0;
    while(currentHandleCount < branchInfo.branchPathWithSrc.size())
    {
        int currentHandleId = branchInfo.branchPathWithSrc.size() - currentHandleCount - 1;

        const BranchPath &srcBp = branchInfo.branchPathWithSrc[currentHandleId].first;
        
        // 将合并后的分支信息 和 与之分支相同的分支路径 合并数据源
        // O    v   =>   O    v,w
	    // O    v,w =>
        bool mergeSameBranch = false;
        for(int i = currentHandleId - 1; i >= 0; i--)
        {
            const BranchPath &dstBp = branchInfo.branchPathWithSrc[i].first;
            if(!areTwoBranchPathSame(srcBp, dstBp))
            {
                continue;
            }
            const BranchPathDataSrc &srcBpDataSrc = branchInfo.branchPathWithSrc[currentHandleId].second;
            BranchPathDataSrc &dstBpDataSrc = branchInfo.branchPathWithSrc[i].second;
            for(int j = 0; j < srcBpDataSrc.srcs.size(); j++)
            {
                if(find(dstBpDataSrc.srcs.begin(), dstBpDataSrc.srcs.end(), srcBpDataSrc.srcs[j]) == dstBpDataSrc.srcs.end())
                {
                    dstBpDataSrc.srcs.push_back(srcBpDataSrc.srcs[j]);
                }
            }
            branchInfo.branchPathWithSrc.erase(branchInfo.branchPathWithSrc.begin() + currentHandleId);
            mergeSameBranch = true;
        }
        if(mergeSameBranch)
        {
            continue;
        }

        if(srcBp.branchs.size() == 1)
        {
            currentHandleCount++;
            continue;
        }
        
        BranchPath subBranchPath = srcBp;
        subBranchPath.branchs.erase(subBranchPath.branchs.begin() + subBranchPath.branchs.size() - 1);
        
        const BranchPathDataSrc &srcBpDataSrc = branchInfo.branchPathWithSrc[currentHandleId].second;

        // 分支路径的最后一个分支
        const Branch &lastBranch = srcBp.branchs[srcBp.branchs.size() - 1];

        // 匹配剩余未处理的分支路径
        set<int> branchIdList; // 记录找出的分支id，用来统计是否达到完全分支
        branchIdList.insert(lastBranch.branchId);
        vector<int> branchPathIdList; // 记录要合并的那些分支的id，已经是从大到小排序
        branchPathIdList.push_back(currentHandleId);


        // 找出所有与当前分支路径重合(除去最后一个分支)的且数据源相同的组件
        for(int i = currentHandleId - 1; i >= 0; i--)
        {
            const BranchPath &dstBp = branchInfo.branchPathWithSrc[i].first;
            const BranchPathDataSrc &dstBpDataSrc = branchInfo.branchPathWithSrc[i].second;
            if(dstBp.branchs.size() != srcBp.branchs.size())
            {
                break;
            }
            if(dstBp.branchs[dstBp.branchs.size() - 1].branchNode != srcBp.branchs[srcBp.branchs.size() - 1].branchNode ||
                !isBranchPathContainBranchPath(subBranchPath, dstBp) ||
                !areTwoBranchPathDataSrcSame(srcBpDataSrc, dstBpDataSrc))
            {
                continue;
            }

            branchIdList.insert(dstBp.branchs[dstBp.branchs.size() - 1].branchId);
            branchPathIdList.push_back(i);
        }

        // 判断是否占有全部分支
        if(getDataflowNodeBranchCount(lastBranch.branchNode) != branchIdList.size())
        {
            currentHandleCount++;
            continue;
        }

        // 占有全部分支则合并
        BranchPathDataSrc bpDataSrcTemp = srcBpDataSrc;
        for(int i = 0; i < branchPathIdList.size(); i++)
        {
            branchInfo.branchPathWithSrc.erase(branchInfo.branchPathWithSrc.begin() + branchPathIdList[i]);
        }

        int insertPos = currentHandleId - 1;
        if(branchInfo.branchPathWithSrc.size() < insertPos)
        {
            insertPos = branchInfo.branchPathWithSrc.size() - 1;
        }

        for(; insertPos >= 0 && insertPos < branchInfo.branchPathWithSrc.size(); insertPos--)
        {
            if(branchInfo.branchPathWithSrc[insertPos].first.branchs.size() <= subBranchPath.branchs.size())
            {
                insertPos++;
                break;
            }
        }
        insertPos = branchInfo.branchPathWithSrc.empty() ? 0 : insertPos;
        branchInfo.branchPathWithSrc.insert(branchInfo.branchPathWithSrc.begin() + insertPos, pair<BranchPath, BranchPathDataSrc>(subBranchPath, bpDataSrcTemp));




    }

    return 1;
}


void MIRScheduleAnalyzerDataflow::branchInfoPrint(
    const MIRScheduleGraphDataflow* schedule,
    const std::map<MIRScheduleGraphDataflowNode*,
    BranchInfo>& retBranchInfos) const
{
    for(int i = 0; i < schedule->layers.size(); i++)
    {
        cout << "\033[1;32m";
        cout << "Layer " << i << ":\n";
        for(int j = 0; j < schedule->layers.at(i).size(); j++)
        {
            MIRScheduleGraphDataflowNode* node = schedule->layers.at(i)[j];
            cout << "\033[0m";
            cout << node->actor->name << "\n";

            for(int k = 0; k < node->actionPorts.size(); k++)
            {
                cout << "  act: " <<node->actionPorts[k]->actionPort->name << "\n";
            }
            for(int k = 0; k < node->inports.size(); k++)
            {
                cout << "  in: " <<node->inports[k]->inportFromActor->name << "\n";
            }
            for(int k = 0; k < node->outports.size(); k++)
            {
                cout << "  out: " <<node->outports[k]->outportFromActor->name << "\n";
            }
            
            

            const BranchInfo& branchInfo = retBranchInfos.at(node);
            for(int k = 0; k < branchInfo.branchPathWithSrc.size(); k++)
            {
                cout << "\033[1;33m";
                cout << "  ";
                for(int m = 0; m < branchInfo.branchPathWithSrc[k].first.branchs.size(); m++)
                {
                    const Branch &branch = branchInfo.branchPathWithSrc[k].first.branchs[m];
                    if(m!=0)
                    {
                        cout << " -> ";
                    }
                    if(branch.branchNode)
                    {
                        cout << branch.branchNode->actor->name << ".";
                    }
                    cout << branch.branchId;
                }

                cout << "\033[1;36m";
                cout << " : ";
                for(int m = 0; m < branchInfo.branchPathWithSrc[k].second.srcs.size(); m++)
                {
                    if(m!=0)
                    {
                        cout << ", ";
                    }
                    MIRScheduleGraphOutport* src = branchInfo.branchPathWithSrc[k].second.srcs[m];
                    if(src->containerNode)
                    {
                        cout << src->containerNode->actor->name << ".";
                    }
                    if(src->outportFromActor)
                    {
                        cout << src->outportFromActor->name;
                    }
                    else if(src->inportFromFunction)
                    {
                        cout << src->inportFromFunction->name;
                    }
                }
                cout << "\n";
            }

            
            cout << "\n";
        }
    }
    cout << "\033[0m";
}


int MIRScheduleAnalyzerDataflow::buildCodeGenerationTable(
    const MIRScheduleGraphDataflow* schedule,
    const std::map<MIRScheduleGraphDataflowNode*, BranchInfo>& branchInfos,
    CodeGenerationTable& retCGT)
{
    int res;
    res = buildCGTEmpty(retCGT);
    if(res < 0)
    {
        return res;
    }

    for(int i = 0; i < schedule->layers.size(); i++)
    {
        for(int j = 0; j < schedule->layers.at(i).size(); j++)
        {
            const MIRScheduleGraphDataflowNode* node = schedule->layers.at(i)[j];
            
            res = buildCGTOneNode(node, branchInfos, retCGT);
            if(res < 0)
            {
                return res;
            }
        }

        res = buildCGTUpdateLayer(retCGT);
        if(res < 0)
        {
            return res;
        }
    }

    return 1;
}

int MIRScheduleAnalyzerDataflow::buildCGTEmpty(CodeGenerationTable& retCGT)
{
    CGTInserter* mainInserter = new CGTInserter();
    retCGT.elements.push_back(mainInserter);
    return 1;
}

int MIRScheduleAnalyzerDataflow::buildCGTOneNode(
    const MIRScheduleGraphDataflowNode* node,
    const std::map<MIRScheduleGraphDataflowNode*, BranchInfo>& branchInfos,
    CodeGenerationTable& CGT)
{
    if(CGT.handledNodes.find(node) != CGT.handledNodes.end())
        return 0;


    const BranchInfo& branchInfo = branchInfos.at(const_cast<MIRScheduleGraphDataflowNode*>(node));

    // 旧思路，会出现二分支后面的组件先把公共分支前的Inserter都清除的问题
    // 1. 分支存在冲突
    //   1.1. 公共分支前存在需要被删除的Inserter
    //      ** 创建TypeCFRemoveInserterAndCreateBranchAndInsert的CGTOperation
    //
    //   1.2. 公共分支前不存在需要被删除的Inserter
    //      ** 创建TypeCFCreateBranchAndInsert的CGTOperation
    //
    // 2. 分支不存在冲突
    //   2.1. 公共分支前存在需要被删除的Inserter
    //     2.1.1. 找不到(不完全找到)对应自身分支信息的Inserter
    //              ** 创建TypeNCRemoveInserterAndCreateBranchAndInsert的CGTOperation
    //
    //     2.1.2. 可以完全找到对应自身分支信息的Inserter
    //              ** 创建TypeNCRemoveInserterAndInsert的CGTOperation
    //
    //   2.2. 公共分支前不存在需要被删除的Inserter
    //     2.2.1. 找不到(不完全找到)对应自身分支信息的Inserter
    //              ** 创建TypeNCCreateBranchAndInsert的CGTOperation
    //
    //     2.2.2. 可以完全找到对应自身分支信息的Inserter
    //              ** 创建TypeNCInsert的CGTOperation


    // 新思路
    // 1. 分支存在冲突
    //   1.1. 公共分支前存在需要被删除的Inserter
    //      ** 创建TypeCFRemoveInserterAndCreateBranchAndInsert的CGTOperation
    //
    //   1.2. 公共分支前不存在需要被删除的Inserter
    //      ** 创建TypeCFCreateBranchAndInsert的CGTOperation
    //
    // 2. 分支不存在冲突
    //   2.1. 可以完全找到对应自身分支信息的Inserter
    //      ** 创建TypeNCInsert的CGTOperation
    //   2.2. 找不到(不完全找到)对应自身分支信息的Inserter
    //     2.1.1. 公共分支前存在需要被删除的Inserter
    //              ** 创建TypeNCRemoveInserterAndCreateBranchAndInsert的CGTOperation
    //
    //     2.1.2. 公共分支前不存在需要被删除的Inserter
    //              ** 创建TypeNCCreateBranchAndInsert的CGTOperation
    //

    bool isBranchInfoConflict = buildCGTIsBranchInfoConflict(branchInfo);
    bool isCommonBranchNeedRemovedInsert = buildCGTIsCommonBranchNeedRemovedInserter(branchInfo, CGT);

    CGTOperation* operation = new CGTOperation();
    if(isBranchInfoConflict) {
        if(isCommonBranchNeedRemovedInsert) {
            operation->type = CGTOperation::TypeCFRemoveInserterAndCreateBranchAndInsert;
        } else {
            operation->type = CGTOperation::TypeCFCreateBranchAndInsert;
        }
    } else {
        bool isThereSuitableInserter = buildCGTIsThereSuitableInserter(branchInfo, CGT);
        if(isThereSuitableInserter) {
            operation->type = CGTOperation::TypeNCInsert;
        } else {
            //if(isCommonBranchNeedRemovedInsert) {
            //    operation->type = CGTOperation::TypeNCRemoveInserterAndCreateBranchAndInsert;
            //} else {
                operation->type = CGTOperation::TypeNCCreateBranchAndInsert;
            //}
        }
    }

    operation->setBranchInfo(const_cast<BranchInfo*>(&branchInfo));
    operation->setCGT(&CGT);

    
    //operation->addDataflowNode(const_cast<BranchInfo*>(&branchInfo), const_cast<MIRScheduleGraphDataflowNode*>(node));
    
    // 3. 将该节点直接连接的全部分支信息(路径)一致的节点一起进行CGT构建
    //    保证同一“串”组件可以顺序地生成代码，并在CodeGenerationTable的handledNodes标记已生成

    

    
    int res = buildCGTOneNodeSameBranchInfo(node, branchInfos, CGT, branchInfo, operation);
    if(res < 0)
        return res;
    
    
    CGT.operationList.addOperation(operation);
    
    //CGT.handledNodes[node] = true;

    return 1;
}

int MIRScheduleAnalyzerDataflow::buildCGTOneNodeSameBranchInfo(const MIRScheduleGraphDataflowNode* node,
    const std::map<MIRScheduleGraphDataflowNode*, BranchInfo>& branchInfos, CodeGenerationTable& CGT,
    const BranchInfo& branchInfo, CGTOperation* operation)
{
    CGT.handledNodes[node] = true;

    vector<MIRScheduleGraphDataflowNode*> predNodes; // 实际的当前计算节点的后继，表现为后继节点的全部前驱
    predNodes.push_back(const_cast<MIRScheduleGraphDataflowNode*>(node));
    int currentNodeId = 0;
    while(currentNodeId < predNodes.size() && !ILGenerator::configTopologyBasedOrder)
    {
        // 按顺序遍历队列中的每个节点
        const MIRScheduleGraphDataflowNode* tempNode = predNodes[currentNodeId];

        vector<MIRScheduleGraphDataflowNode*> succNodes = getNodeSuccessors(tempNode);
        for(int i = 0; i < succNodes.size(); i++)
        {
            MIRScheduleGraphDataflowNode* succNode = succNodes[i];

            // 忽略已经被CGT处理的节点
            if(CGT.handledNodes.find(succNode) != CGT.handledNodes.end())
            {
                continue;
            }

            // 该后继节点的分支信息是否和当前节点一致
            const BranchInfo& succNodeBranchInfo = branchInfos.at(succNode);
            if(!areTwoBranchInfoPathSame(branchInfo, succNodeBranchInfo))
            {
                continue;
            }

            // // 该后继节点的所有前驱节点是否全都在队列中
            // bool isAllInPredNodes = true;
            // vector<MIRScheduleGraphDataflowNode*> predNodesOfSuccNode = getNodePredecessors(succNode);
            // for(int j = 0; j < predNodesOfSuccNode.size(); j++)
            // {
            //     if(find(predNodes.begin(), predNodes.end(), predNodesOfSuccNode[j]) == predNodes.end())
            //     {
            //         isAllInPredNodes = false;
            //         break;
            //     }
            // }
            // if(!isAllInPredNodes)
            // {
            //     continue;
            // }

            // 该后继节点的所有前驱节点是否已经被处理(加入到CGT中)过
            // 这样可以保证 同层的分支相同的节点 的同分支信息节点也被加进来
            bool isAllHandled = true;
            vector<MIRScheduleGraphDataflowNode*> predNodesOfSuccNode = getNodePredecessors(succNode);
            for(int j = 0; j < predNodesOfSuccNode.size(); j++)
            {
                if(CGT.handledNodes.find(predNodesOfSuccNode[j]) == CGT.handledNodes.end())
                {
                    isAllHandled = false;
                    break;
                }
            }
            if(!isAllHandled)
            {
                continue;
            }


            // 已经找到的同分支信息的节点就不再加入队列中了
            if(find(predNodes.begin(), predNodes.end(), succNode) == predNodes.end())
            {
                predNodes.push_back(succNode);
                
                CGT.handledNodes[succNodes[i]] = true;
            }
        }
        currentNodeId++;
    }

    // 将基于该节点找到的全部分支相同且无其它(除了该节点本身)数据依赖的节点也一起加入CGT中
    vector<MIRScheduleGraphDataflowNode*> &succNodes = predNodes;
    for(int i = 0; i < succNodes.size(); i++)
    {
        const BranchInfo& succNodeBranchInfo = branchInfos.at(succNodes[i]);

        operation->addDataflowNode(const_cast<BranchInfo*>(&succNodeBranchInfo), succNodes[i]);

    }
    return 1;
}

int MIRScheduleAnalyzerDataflow::buildCGTUpdateLayer(CodeGenerationTable& retCGT)
{
    retCGT.operationList.execute();
    retCGT.operationList.release();
    return 1;
}

bool MIRScheduleAnalyzerDataflow::buildCGTIsBranchInfoConflict(const BranchInfo& branchInfo)
{
    for(int i = 0; i < branchInfo.branchPathWithSrc.size(); i++)
    {
        const BranchPath &bp1 = branchInfo.branchPathWithSrc[i].first;
        for(int j = 0; j < branchInfo.branchPathWithSrc.size(); j++)
        {
            if(i == j)
                continue;
            
            const BranchPath &bp2 = branchInfo.branchPathWithSrc[j].first;
            
            if(areTwoBranchPathConflict(bp1, bp2))
                return true;
        }
    }
    return false;
}


bool MIRScheduleAnalyzerDataflow::buildCGTIsCommonBranchNeedRemovedInserter(const BranchInfo& branchInfo,
    const CodeGenerationTable& CGT)
{
    
    BranchPath commonBranchPath = getBranchInfoCommonBranchPath(branchInfo);

    const CGTInserter* inserter = buildCGTGetBaseInserterOfCommonBranchPath(commonBranchPath, CGT);

    // 先寻找inserter的父节点，然后在该父节点到该inserter的范围内寻找有没有其他的inserter
    bool startSearch = inserter->parent ? false : true;
    CodeGenerationTable::Iterator iter(CGT);
    const CGTElement* element = iter.next();
    while(element)
    {
        if(element == inserter->parent)
        {
            startSearch = true;
            element = (const CGTElement*)iter.next();
            continue;
        }

        if(startSearch && element->type == CGTElement::TypeInserter)
        {
            if(element != inserter)
            {
                return true;
            }
            else
            {
                return false;
            }
        }

        element = (const CGTElement*)iter.next();
    }
    return false;
}

bool MIRScheduleAnalyzerDataflow::buildCGTIsThereSuitableInserter(const BranchInfo& branchInfo,
    const CodeGenerationTable& CGT)
{
    for(int i = 0; i < branchInfo.branchPathWithSrc.size(); i++)
    {
        const BranchPath &bp = branchInfo.branchPathWithSrc[i].first;
        const CGTInserter* inserter = buildCGTFindInserterByBranchPath(bp, CGT);
        if(!inserter)
            return false;
    }

    return true;
}

const CGTInserter* MIRScheduleAnalyzerDataflow::buildCGTFindInserterByBranchPath(
    const BranchPath& branchPath, const CodeGenerationTable& CGT)
{
    CodeGenerationTable::Iterator iter(CGT);
    const CGTElement* element = iter.next();
    while(element)
    {
        if(element->type != CGTElement::TypeInserter)
        {
            element = (const CGTElement*)iter.next();
            continue;
        }

        const CGTInserter* inserter = static_cast<const CGTInserter*>(element);
        const CGTBranch* branch = (const CGTBranch*)inserter->parent;
        if(branch)
        {
            if(branch->realBranchPaths.size() != 1 || 
                !(branch->branchType >= CGTBranch::TypeIf && branch->branchType <= CGTBranch::TypeElse))
            {
                element = (const CGTElement*)iter.next();
                continue;
            }

            if(areTwoBranchPathSame(branchPath, branch->realBranchPaths[0]))
            {
                return inserter;
            }
        }
        else
        {
            if(branchPath.branchs.size() == 1)
            {
                return inserter;
            }
        }

        element = (const CGTElement*)iter.next();
    }
    return nullptr;
}

const CGTInserter* MIRScheduleAnalyzerDataflow::buildCGTGetBaseInserterOfCommonBranchPath(
    BranchPath &commonBranchPath, const CodeGenerationTable& CGT)
{
    const CGTInserter* inserter = buildCGTFindInserterByBranchPath(commonBranchPath, CGT);

    // 找不到公共分支路径则寻找公共分支路径的子分支路径(去掉最后一个分支)
    while(!inserter && !commonBranchPath.branchs.empty())
    {
        commonBranchPath.branchs.erase(commonBranchPath.branchs.begin() + commonBranchPath.branchs.size() - 1);
        inserter = buildCGTFindInserterByBranchPath(commonBranchPath, CGT);
    }
    return inserter;
}

std::vector<Branch> MIRScheduleAnalyzerDataflow::buildCGTGetBranchInfoConditionBranchList(
    const BranchInfo& branchInfo, BranchPath& commonBranchPath)
{
    vector<Branch> ret;
    for(int i = 0; i < branchInfo.branchPathWithSrc.size(); i++)
    {
        const BranchPath& branchPath = branchInfo.branchPathWithSrc[i].first;
        for(int j = commonBranchPath.branchs.size(); j < branchPath.branchs.size(); j++)
        {
            const Branch &branch = branchPath.branchs[j];
            bool found = false;
            for(int k = 0; k < ret.size(); k++)
            {
                if(branch.branchId == ret[k].branchId && branch.branchNode == ret[k].branchNode)
                {
                    found = true;
                    break;
                }
            }
            if(!found)
            {
                ret.push_back(branch);
            }
        }
    }
    return ret;
}

bool MIRScheduleAnalyzerDataflow::buildCGTIsConditionBranchListAlwaysFalse(std::vector<Branch>& conditionBranchList)
{
    for(int i = 0; i < conditionBranchList.size(); i++)
    {
        for(int j = 0; j < conditionBranchList.size(); j++)
        {
            if(i == j)
                continue;

            const Branch &b1 = conditionBranchList[i];
            const Branch &b2 = conditionBranchList[j];

            if(b1.branchNode == b2.branchNode && b1.branchId == b2.branchId)
            {
                return true;
            }
        }
    }
    return false;
}


std::vector<MIRScheduleGraphDataflowNode*> MIRScheduleAnalyzerDataflow::getNodePredecessors(
    const MIRScheduleGraphDataflowNode* node) const
{
    vector<MIRScheduleGraphDataflowNode*> preds;
    for(int i = 0; i < node->inports.size(); i++)
    {
        MIRScheduleGraphInport* inport = node->inports[i];
        for(int j = 0 ; j < inport->srcOutports.size(); j++)
        {
            MIRScheduleGraphOutport* src = inport->srcOutports[j];
            if(src->containerNode && find(preds.begin(), preds.end(), src->containerNode) == preds.end())
                preds.push_back(src->containerNode);
        }
    }

    for(int i = 0; i < node->actionPorts.size(); i++)
    {
        MIRScheduleGraphActionPort* actionPort = node->actionPorts[i];
        for(int j = 0; j < actionPort->srcOutports.size(); j++)
        {
            MIRScheduleGraphOutport* src = actionPort->srcOutports[j];
            if(src->containerNode && find(preds.begin(), preds.end(), src->containerNode) == preds.end())
                preds.push_back(src->containerNode);
        }
    }

    return preds;
}

std::vector<MIRScheduleGraphOutport*> MIRScheduleAnalyzerDataflow::getNodePredecessorsOutports(
    const MIRScheduleGraphDataflowNode* node) const
{
    
    vector<MIRScheduleGraphOutport*> predsOutports;
    for(int i = 0; i < node->inports.size(); i++)
    {
        MIRScheduleGraphInport* inport = node->inports[i];
        for(int j = 0; j < inport->srcOutports.size(); j++)
        {
            MIRScheduleGraphOutport* src = inport->srcOutports[j];
            if(find(predsOutports.begin(), predsOutports.end(), src) == predsOutports.end())
                predsOutports.push_back(src);
        }
    }

    for(int i = 0; i < node->actionPorts.size(); i++)
    {
        MIRScheduleGraphActionPort* actionPort = node->actionPorts[i];
        for(int j = 0; j < actionPort->srcOutports.size(); j++)
        {
            MIRScheduleGraphOutport* src = actionPort->srcOutports[j];
            if(find(predsOutports.begin(), predsOutports.end(), src) == predsOutports.end())
            predsOutports.push_back(src);
        }
    }

    return predsOutports;
}

std::vector<MIRScheduleGraphDataflowNode*> MIRScheduleAnalyzerDataflow::getNodeSuccessors(
    const MIRScheduleGraphDataflowNode* node) const
{
    
    vector<MIRScheduleGraphDataflowNode*> succs;
    for(int i = 0; i < node->outports.size(); i++)
    {
        MIRScheduleGraphOutport* outport = node->outports[i];
        for(int j = 0; j < outport->dstInports.size(); j++)
        {
            MIRScheduleGraphInport* dst = outport->dstInports[j];
            if(dst->containerNode && find(succs.begin(), succs.end(), dst->containerNode) == succs.end())
                succs.push_back(dst->containerNode);
        }
        for(int j = 0; j < outport->dstActionPorts.size(); j++)
        {
            MIRScheduleGraphActionPort* dst = outport->dstActionPorts[j];
            if(dst->containerNode && find(succs.begin(), succs.end(), dst->containerNode) == succs.end())
                succs.push_back(dst->containerNode);
        }
    }
    
    return succs;
}

std::vector<MIRScheduleGraphDataflowNode*> MIRScheduleAnalyzerDataflow::getNodeAllPredecessors(
    const MIRScheduleGraphDataflowNode* node) const
{
    vector<MIRScheduleGraphDataflowNode*> preds;
    unordered_map<const MIRScheduleGraphDataflowNode*, bool> foundNodes;
    queue<const MIRScheduleGraphDataflowNode*> nodeNeedFound;
    nodeNeedFound.push(node);
    while(!nodeNeedFound.empty())
    {
        const MIRScheduleGraphDataflowNode* curNode = nodeNeedFound.front();
        nodeNeedFound.pop();

        if(foundNodes.find(curNode) != foundNodes.end())
            continue;
        foundNodes[curNode] = true;

        for(int i = 0; i < curNode->inports.size(); i++)
        {
            MIRScheduleGraphInport* inport = curNode->inports[i];
            for(int j = 0 ; j < inport->srcOutports.size(); j++)
            {
                MIRScheduleGraphOutport* src = inport->srcOutports[j];
                if(src->containerNode)
                {
                    if(find(preds.begin(), preds.end(), src->containerNode) == preds.end())
                        preds.push_back(src->containerNode);
                    nodeNeedFound.push(src->containerNode);
                }
            }
        }

        for(int i = 0; i < curNode->actionPorts.size(); i++)
        {
            MIRScheduleGraphActionPort* actionPort = curNode->actionPorts[i];
            for(int j = 0 ; j < actionPort->srcOutports.size(); j++)
            {
                MIRScheduleGraphOutport* src = actionPort->srcOutports[j];
                if(src->containerNode)
                {
                    if(find(preds.begin(), preds.end(), src->containerNode) == preds.end())
                        preds.push_back(src->containerNode);
                    nodeNeedFound.push(src->containerNode);
                }
            }
        }
    }
    return preds;
}

BranchPath MIRScheduleAnalyzerDataflow::getBranchInfoCommonBranchPath(const BranchInfo& branchInfo)
{
    BranchPath commonBranchPath;
    const BranchPath &firstBranchPath = branchInfo.branchPathWithSrc[0].first;
    for(int i = 0; i < firstBranchPath.branchs.size(); i++)
    {
        const Branch &branch = firstBranchPath.branchs[i];
        for(int j = 1; j < branchInfo.branchPathWithSrc.size(); j++)
        {
            const BranchPath &otherBranchPath = branchInfo.branchPathWithSrc[j].first;
            if(i >= otherBranchPath.branchs.size())
            {
                return commonBranchPath;
            }
            const Branch &branchDst = otherBranchPath.branchs[i];
            if(branch.branchId != branchDst.branchId || branch.branchNode != branchDst.branchNode)
            {
                return commonBranchPath;
            }
        }
        commonBranchPath.branchs.push_back(branch);
    }
    return commonBranchPath;
}

bool MIRScheduleAnalyzerDataflow::areTwoBranchPathConflict(const BranchPath& srcBp, const BranchPath& dstBp)
{
    // 只要任意两个分支路径不符合“0->A.0->B.0, 0->A.1”、“0->A.0->B.0, 0->A.0->B.1”这种从一个分支组件分叉的
    // 都认为是冲突的
    
    for(int i = 0; i < srcBp.branchs.size() && i < dstBp.branchs.size(); i++)
    {
        if(srcBp.branchs[i].branchNode == dstBp.branchs[i].branchNode &&
            srcBp.branchs[i].branchId == dstBp.branchs[i].branchId)
        {
            continue;
        }

        if(srcBp.branchs[i].branchNode != dstBp.branchs[i].branchNode)
            return true;

        if(srcBp.branchs[i].branchId != dstBp.branchs[i].branchId)
            return false;
    }
    return true;
}

bool MIRScheduleAnalyzerDataflow::areTwoBranchPathSame(const BranchPath& srcBp, const BranchPath& dstBp)
{
    if(srcBp.branchs.size() != dstBp.branchs.size())
        return false;
    for(int i = 0; i < srcBp.branchs.size(); i++)
    {
        if(srcBp.branchs[i].branchNode != dstBp.branchs[i].branchNode ||
            srcBp.branchs[i].branchId != dstBp.branchs[i].branchId)
        {
            return false;
        }
    }
    return true;
}

bool MIRScheduleAnalyzerDataflow::areTwoBranchPathDataSrcSame(const BranchPathDataSrc& srcBpDataSrc,
    const BranchPathDataSrc& dstBpDataSrc)
{
    if(srcBpDataSrc.srcs.size() != dstBpDataSrc.srcs.size())
        return false;


    for(int i = 0; i < srcBpDataSrc.srcs.size(); i++)
    {
        bool found = false;
        for(int j = 0; j < dstBpDataSrc.srcs.size(); j++)
        {
            if(srcBpDataSrc.srcs[i] == dstBpDataSrc.srcs[j])
            {
                found = true;
                break;
            }
        }
        if(!found)
        {
            return false;
        }
    }
    return true;
}

void MIRScheduleAnalyzerDataflow::mergeTwoBranchPathDataSrc(const BranchPathDataSrc& srcBpDataSrc,
    BranchPathDataSrc& dstBpDataSrc)
{
    for(int i = 0; i < srcBpDataSrc.srcs.size(); i++)
    {
        MIRScheduleGraphOutport* p = srcBpDataSrc.srcs[i];
        bool found = false;
        for(int j = 0; j < dstBpDataSrc.srcs.size(); j++)
        {
            if(dstBpDataSrc.srcs[j] == p)
            {
                found = true;
                break;
            }
        }
        if(found)
            continue;
        dstBpDataSrc.srcs.push_back(p);
    }
}

bool MIRScheduleAnalyzerDataflow::isBranchPathContainBranchPath(const BranchPath& srcBp, const BranchPath& dstBp)
{
    if(srcBp.branchs.size() >= dstBp.branchs.size())
    {
        return false;
    }

    for(int i = 0; i < srcBp.branchs.size(); i++)
    {
        if(srcBp.branchs[i].branchNode != dstBp.branchs[i].branchNode ||
            srcBp.branchs[i].branchId != dstBp.branchs[i].branchId)
        {
            return false;
        }
    }
    return true;
}

bool MIRScheduleAnalyzerDataflow::areTwoBranchInfoPathSame(const BranchInfo& srcBranchInfo, const BranchInfo& dstBranchInfo)
{
    if(srcBranchInfo.branchPathWithSrc.size() != dstBranchInfo.branchPathWithSrc.size())
        return false;
    for(int i = 0; i < srcBranchInfo.branchPathWithSrc.size(); i++)
    {
        const BranchPath &srcBp = srcBranchInfo.branchPathWithSrc[i].first;
        const BranchPath &dstBp = dstBranchInfo.branchPathWithSrc[i].first;
        if(!areTwoBranchPathSame(srcBp, dstBp))
        {
            return false;
        }
    }
    return true;
}


bool MIRScheduleAnalyzerDataflow::isDataflowNodeBranchNode(const MIRScheduleGraphDataflowNode* node)
{
    if(!node->actor)
        return false;

    if(node->actor->type == "BooleanSwitch")
    {
        return true;
    }
    return false;
}


int MIRScheduleAnalyzerDataflow::getDataflowNodeBranchCount(const MIRScheduleGraphDataflowNode* node)
{
    if(!node->actor)
        return -1;

    if(node->actor->type == "BooleanSwitch")
    {
        return 2;
    }
    return 1;
}


int MIRScheduleAnalyzerDataflow::getDataflowNodeBranchId(const MIRScheduleGraphDataflowNode* node, const MIRScheduleGraphOutport* outport)
{
    if(!node->actor)
        return -1;

    for(int i = 0; i < node->actor->outports.size(); i++)
    {
        if(outport->outportFromActor == node->actor->outports[i])
        {
            return i;
        }
    }

    return -1;
}
