#include "MIRActorParallelizationOptimizer.h"
#include "../ILGenerator/MIRSchedule.h"
#include "../ILGenerator/MIRScheduleAnalyzer.h"
#include "../ILGenerator/MIRScheduleGraphInport.h"
#include "../ILGenerator/MIRScheduleGraphOutport.h"

using namespace std;

std::vector<std::string> MIRActorParallelizationOptimizer::supportedActorList = {
    "Sum",
    "Product",
    "ArithShift",
};

int MIRActorParallelizationOptimizer::optimize(MIRFunctionDataflow* function, MIRModel* model)
{
    int res;
    this->function = function;
    this->model = model;

    res = optimize();
    if(res < 0) {
        return res;
    }

    return 1;
}

int MIRActorParallelizationOptimizer::optimize()
{
    int res;
    MIRScheduleAnalyzer scheduleAnalyzer;
    res = scheduleAnalyzer.scheduleAnalyze(model, &schedule);
    if(res < 0) {
		return res;
	}

    for(int i = 0; i < schedule.scheduleGraphs.size(); i++)
    {
        if(schedule.scheduleGraphs[i]->type != MIRScheduleGraph::TypeDataflow)
            continue;
        graphDataflow = static_cast<MIRScheduleGraphDataflow*>(schedule.scheduleGraphs[i]);
        if(graphDataflow->nodes.empty())
            continue;
        
        mIRFunction = static_cast<MIRFunction*>(graphDataflow->nodes[0]->actor->parent);

        res = collectGraphNodeSeed();
        if(res < 0)
            return res;
    }

    return 1;
}



int MIRActorParallelizationOptimizer::collectGraphNodeSeed()
{
    map<string, vector<Graph>> graphMap;
    for(int j = 0; j < graphDataflow->nodes.size(); j++)
    {
        MIRScheduleGraphDataflowNode* node = graphDataflow->nodes[j];
        if(findIndexInVector(supportedActorList, node->actor->type) == -1)
        {
            continue;
        }
        Graph tempGraph;
        tempGraph.nodeList.push_back(node);
        graphMap[node->actor->type].push_back(tempGraph);
    }

    map<string, vector<Graph>>::iterator iter = graphMap.begin();
    for(;iter != graphMap.end(); iter++)
    {
        if(iter->second.size() < 4)
            continue;
        ExtensibleSameGraph extensibleSameGraph;
        extensibleSameGraph.graphList = iter->second;
        sameGraphGrouphList.extensibleSameGraphList.push(extensibleSameGraph);
    }

    return 1;
}

int MIRActorParallelizationOptimizer::tryToExtendExtensibleSameGraph()
{
    int res;
    while(!sameGraphGrouphList.extensibleSameGraphList.empty())
    {
        res = tryToExtendExtensibleSameGraphOne();
        if(res < 0)
            return res;
    }
    return 1;
}

int MIRActorParallelizationOptimizer::tryToExtendExtensibleSameGraphOne()
{
    ExtensibleSameGraph sameGraph = sameGraphGrouphList.extensibleSameGraphList.front();
    sameGraphGrouphList.extensibleSameGraphList.pop();

    vector<vector<MIRScheduleGraphDataflowNode*>> neighborNodesList;
    neighborNodesList.resize(sameGraph.graphList.size());
    for(int i = 0; i < sameGraph.graphList.size(); i++)
    {
        neighborNodesList[i] = getGraphNeighborNodes(sameGraph.graphList[i]);
    }

    // 可扩展的节点组合 的列表,
    // first 代表可扩展的个数， second 代表扩展节点(长度为子图个数，不能扩展的子图对应位置为null)
    vector<pair<int, vector<MIRScheduleGraphDataflowNode*>>> extensibleNodesList;

    // neighborNodesList.size() === sameGraph.graphList.size()
    for(int i = 0; i < neighborNodesList.size(); i++)
    {
        vector<MIRScheduleGraphDataflowNode*>& neighborNodes = neighborNodesList[i];
        // 对每个子图都搜索一遍邻居节点，保证每个可扩展的节点组合都能被找到
        for(int j = 0; j < neighborNodes.size(); j++)
        {
            MIRScheduleGraphDataflowNode* node = neighborNodes[j];
            int x, y;
            vector<pair<int, int>> pos = getExtendPositionListInGraphByNode(sameGraph.graphList[i], node);

            pair<int, vector<MIRScheduleGraphDataflowNode*>> extensibleNodes = getExtensibleNodesByNodeAndPos(sameGraph, i, node, pos);

            insertToExtensibleNodesList(extensibleNodesList, extensibleNodes);

            // To do
            // 扩展子图，存在全部子图都可扩展的情况，则直接全扩展，否则按不同组合情况分别扩展
            // 扩展后的子图，插入到扩展队列中



        }
    }

    return 1;
}

int MIRActorParallelizationOptimizer::tryToExtendExtensibleSameGraphOneDo()
{

    return 1;
}

pair<int, vector<MIRScheduleGraphDataflowNode*>> MIRActorParallelizationOptimizer::getExtensibleNodesByNodeAndPos(
    ExtensibleSameGraph& sameGraph, int curSearchIndex, MIRScheduleGraphDataflowNode* node,
    vector<pair<int, int>>& pos)
{
    int count = 1;
    vector<MIRScheduleGraphDataflowNode*> ret;
    ret.resize(sameGraph.graphList.size());
    for(int k = 0; k < sameGraph.graphList.size(); k++)
    {
        if(k == curSearchIndex)
        {
            ret[k] = node;
            continue;
        }

        vector<MIRScheduleGraphDataflowNode*> extensibleNodes = getExtensibleNodesInGraphByPosition(sameGraph.graphList[k], pos);

        bool found = false;
        for(int i = 0; i < extensibleNodes.size(); i++)
        {
            if(extensibleNodes[i]->actor->type == node->actor->type)
            {
                ret[k] = extensibleNodes[i];
                found = true;
                count++;
                break;
            }
        }
        if(!found)
            ret[k] = nullptr;

    }
    return pair<int, vector<MIRScheduleGraphDataflowNode*>>(count, ret);
}



vector<std::pair<int, int>> MIRActorParallelizationOptimizer::getExtendPositionListInGraphByNode(Graph& graph,
                                                                                                 MIRScheduleGraphDataflowNode* node)
{
    std::vector<std::pair<int, int>> ret;
    for(int i = 0; i < graph.nodeList.size(); i++)
    {
        MIRScheduleGraphDataflowNode* nodeTemp = graph.nodeList[i];
        for(int j = 0; j < nodeTemp->inports.size(); j++)
        {
            MIRScheduleGraphInport* inport = nodeTemp->inports[j];
            for(int k = 0; k < inport->srcOutports.size(); k++)
            {
                if(inport->srcOutports[i]->containerNode == node)
                {
                    ret.push_back(pair<int, int>(i, j));
                }
            }
        }
        for(int j = 0; j < nodeTemp->outports.size(); j++)
        {
            MIRScheduleGraphOutport* outport = nodeTemp->outports[j];
            for(int k = 0; k < outport->dstInports.size(); k++)
            {
                if(outport->dstInports[i]->containerNode)
                {
                    ret.push_back(pair<int, int>(i, -j));
                }
            }
        }
    }
    return ret;
}

vector<MIRScheduleGraphDataflowNode*> MIRActorParallelizationOptimizer::getExtensibleNodesInGraphByPosition(
    Graph& graph, vector<pair<int, int>>& pos)
{
    vector<MIRScheduleGraphDataflowNode*> ret;
    int x = pos[0].first;
    int y = pos[0].second;

    MIRScheduleGraphDataflowNode* nodeTemp = graph.nodeList[x];
    if(y > 0) {
        for(int i = 0; nodeTemp->inports[y]->srcOutports.size(); i++) {
            if(nodeTemp->inports[y]->srcOutports[i]->containerNode) {
                ret.push_back(nodeTemp->inports[y]->srcOutports[i]->containerNode);
            }
        }
    } else {
        for(int i = 0; nodeTemp->outports[y]->dstInports.size(); i++) {
            if(nodeTemp->outports[y]->dstInports[i]->containerNode) {
                ret.push_back(nodeTemp->outports[y]->dstInports[i]->containerNode);
            }
        }
    }

    for(int p = 1; p < pos.size(); p++) {

        vector<bool> existInRetList;
        for(int j = 0; j < ret.size(); j++) {
            existInRetList.push_back(false);
        }
        
        x = pos[p].first;
        y = pos[p].second;

        nodeTemp = graph.nodeList[x];
        if(y > 0) {
            for(int i = 0; nodeTemp->inports[y]->srcOutports.size(); i++) {
                if(nodeTemp->inports[y]->srcOutports[i]->containerNode) {
                    int index = findIndexInVector(ret, nodeTemp->inports[y]->srcOutports[i]->containerNode);
                    if(index != -1)
                        existInRetList[index] = true;
                }
            }
        } else {
            for(int i = 0; nodeTemp->outports[y]->dstInports.size(); i++) {
                if(nodeTemp->outports[y]->dstInports[i]->containerNode) {
                    int index = findIndexInVector(ret, nodeTemp->outports[y]->dstInports[i]->containerNode);
                    if(index != -1)
                        existInRetList[index] = true;
                }
            }
        }

        for(int i = existInRetList.size() - 1; i >= 0; i--) {
            if(!existInRetList[i]) {
                ret.erase(ret.begin() + i);
            }
        }
    }

    return ret;
}

//std::vector<std::pair<int, int>> MIRActorParallelizationOptimizer::getGraphExtendPositionList(Graph graph)
//{
//
//}

vector<MIRScheduleGraphDataflowNode*> MIRActorParallelizationOptimizer::getGraphNeighborNodes(Graph& graph)
{
    set<MIRScheduleGraphDataflowNode*> neighborList;
    for(int i = 0; i < graph.nodeList.size(); i++)
    {
        MIRScheduleGraphDataflowNode* node = graph.nodeList[i];
        for(int j = 0; j < node->inports.size(); j++)
        {
            MIRScheduleGraphInport* inport = node->inports[j];
            for(int k = 0; k < inport->srcOutports.size(); k++)
            {
                if(inport->srcOutports[i]->containerNode)
                {
                    neighborList.insert(inport->srcOutports[i]->containerNode);
                }
            }
        }
        for(int j = 0; j < node->outports.size(); j++)
        {
            MIRScheduleGraphOutport* outport = node->outports[j];
            for(int k = 0; k < outport->dstInports.size(); k++)
            {
                if(outport->dstInports[i]->containerNode)
                {
                    neighborList.insert(outport->dstInports[i]->containerNode);
                }
            }
        }
    }
    vector<MIRScheduleGraphDataflowNode*> ret;
    set<MIRScheduleGraphDataflowNode*>::iterator iter = neighborList.begin();
    for(;iter != neighborList.end(); )
    {
        if(findIndexInVector(graph.nodeList, *iter) != -1) {
            iter = neighborList.erase(iter);
        } else {
            ret.push_back(*iter);
            iter++;
        }
    }
    return ret;
}

void MIRActorParallelizationOptimizer::insertToExtensibleNodesList(
    std::vector<std::pair<int, std::vector<MIRScheduleGraphDataflowNode*>>>& list,
    std::pair<int, std::vector<MIRScheduleGraphDataflowNode*>> extensibleNodes)
{
    // 移除掉该组合的子集
    for(int i = list.size() - 1; i >= 0 ; i--)
    {
        bool fullyContain = true;
        for(int j = 0; j < list[i].second.size(); j++)
        {
            if(list[i].second[j] != extensibleNodes.second[j] && extensibleNodes.second[j] != nullptr)
            {
                fullyContain = false;
                break;
            }
        }
        if(fullyContain)
            list.erase(list.begin() + i);
    }
    // 如果该组合被包含，则不插入
    for(int i = list.size() - 1; i >= 0 ; i--)
    {
        bool fullyContained = true;
        for(int j = 0; j < list[i].second.size(); j++)
        {
            if(list[i].second[j] != extensibleNodes.second[j] && list[i].second[j] != nullptr)
            {
                fullyContained = false;
                break;
            }
        }
        if(fullyContained)
            return;
    }
    
    int count = extensibleNodes.first;
    int insertPos = 0;
    for(int i = 0; i < list.size(); i++)
    {
        if(list[i].first <= count)
            insertPos = i + 1;
    }

    list.insert(list.begin() + insertPos, extensibleNodes);
}
