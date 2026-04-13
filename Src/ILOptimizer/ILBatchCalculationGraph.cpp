#include "ILBatchCalculationGraph.h"

#include <set>
#include <algorithm>

#include "../Common/Utility.h"
#include "../ILBasic/ILParser.h"
using namespace std;


namespace
{
    const int CONST_NUM_2  = 2;
    const int CONST_NUM_3  = 3;
}

ILBatchCalculationGraphNodeInput::ILBatchCalculationGraphNodeInput()
{
}

ILBatchCalculationGraphNodeInput::ILBatchCalculationGraphNodeInput(std::string expression, std::string type, const std::vector<int>& arraySize)
{
    this->expression = expression;
    this->type = type;
    this->arraySize = arraySize;
}

ILBatchCalculationGraphNodeInput::~ILBatchCalculationGraphNodeInput()
{
}

ILBatchCalculationGraphNodeParameter::ILBatchCalculationGraphNodeParameter()
{
}

ILBatchCalculationGraphNodeParameter::ILBatchCalculationGraphNodeParameter(std::string expression, std::string type)
{
    this->expression = expression;
    this->type = type;
}

ILBatchCalculationGraphNodeParameter::~ILBatchCalculationGraphNodeParameter()
{
}

ILBatchCalculationGraphNodeOutput::ILBatchCalculationGraphNodeOutput()
{
}

ILBatchCalculationGraphNodeOutput::ILBatchCalculationGraphNodeOutput(std::string expression, std::string type, const std::vector<int>& arraySize)
{
    this->expression = expression;
    this->type = type;
    this->arraySize = arraySize;
}

ILBatchCalculationGraphNodeOutput::~ILBatchCalculationGraphNodeOutput()
{
}

bool ILBatchCalculationGraphNode::isCalculateSizeMatch(const ILBatchCalculationGraphNode* node) const
{
    if(this->calculateSize.size() != node->calculateSize.size())
        return false;

    for(int i = 0; i < this->calculateSize.size(); i++)
    {
        if(this->calculateSize[i] != node->calculateSize[i])
            return false;
    }
    return true;
}

std::vector<ILBatchCalculationGraphNode*> ILBatchCalculationGraphNode::getPredecessorDirect() const
{
    return predecessor;
}

std::vector<ILBatchCalculationGraphNode*> ILBatchCalculationGraphNode::getPredecessorAll() const
{
    
    vector<ILBatchCalculationGraphNode*> ret = getPredecessorDirect();
    
    int index = 0;
    while(index < ret.size())
    {
        vector<ILBatchCalculationGraphNode*> tempSourceNodes = ret[index]->getPredecessorDirect();
        for(int i = 0; i < tempSourceNodes.size(); i++)
        {
            if(find(ret.begin(), ret.end(), tempSourceNodes[i]) == ret.end())
            {
                ret.push_back(tempSourceNodes[i]);
            }
        }
        index++;
    }
    return ret;
}

const ILBatchCalculationGraph* ILBatchCalculationGraph::getRootGraph() const
{
    return rootGraph;
}

void ILBatchCalculationGraph::setNodeIONameMap(std::string name, std::string mapName)
{
    ILBatchCalculationGraph* curGraph = this;
    if(rootGraph)
        curGraph = const_cast<ILBatchCalculationGraph*>(rootGraph);
    curGraph->variableNameMap[name] = mapName;
}

std::string ILBatchCalculationGraph::getNodeIONameMap(std::string name) const
{
    const ILBatchCalculationGraph* curGraph = const_cast<ILBatchCalculationGraph*>(this);
    if(rootGraph)
        curGraph = rootGraph;
    if(curGraph->variableNameMap.find(name) != curGraph->variableNameMap.end())
        return curGraph->variableNameMap.at(name);
    return "";
}

bool ILBatchCalculationGraph::empty() const
{
    return graph.empty();
}

std::vector<ILBatchCalculationGraphNode*>& ILBatchCalculationGraph::operator[](int index)
{
    return graph[index];
}

const vector<ILBatchCalculationGraphNode*>& ILBatchCalculationGraph::at(int index) const
{
    return this->graph.at(index);
}

int ILBatchCalculationGraph::size() const
{
    return graph.size();
}

bool ILBatchCalculationGraph::isContainNode(const ILBatchCalculationGraphNode* node) const
{
    for(int i = 0; i < this->size(); i++)
    {
        for(int j = 0; !this->isLayerEmpty(i) && j < this->at(i).size(); j++)
        {
            if(this->at(i)[j] == node)
                return true;
        }
    }
    return false;
}

bool ILBatchCalculationGraph::isContainScheduleNode(const ILAnalyzeScheduleNode* node) const
{
    for(int i = 0; i < this->size(); i++)
    {
        for(int j = 0; !this->isLayerEmpty(i) && j < this->at(i).size(); j++)
        {
            if(this->at(i)[j]->scheduleNode == node)
                return true;
        }
    }
    return false;
}

int ILBatchCalculationGraph::getNodeCount() const
{
    int ret = 0;
    for(int i = 0; i < this->size(); i++)
    {
        for(int j = 0; !this->isLayerEmpty(i) && j < this->at(i).size(); j++)
        {
            ret++;
        }
    }
    return ret;
}

int ILBatchCalculationGraph::getRealLayerCount() const
{
    int ret = 0;
    for(int i = 0; i < this->size(); i++)
    {
        if(!this->isLayerEmpty(i))
            ret++;
    }
    return ret;
}


ILBatchCalculationGraphNode* ILBatchCalculationGraph::getFirstNode() const
{
    for(int i = 0; i < this->size(); i++)
    {
        for(int j = 0; !this->isLayerEmpty(i) && j < this->at(i).size(); j++)
        {
            return this->at(i)[j];
        }
    }
    return nullptr;
}

ILBatchCalculationGraphNode* ILBatchCalculationGraph::getFirstNode(
    const std::unordered_map<ILBatchCalculationGraphNode*, bool>& mask) const
{
    for(int i = 0; i < this->size(); i++)
    {
        for(int j = 0; !this->isLayerEmpty(i) && j < this->at(i).size(); j++)
        {
            ILBatchCalculationGraphNode* tempNode = this->at(i)[j];
            if(mask.find(tempNode) == mask.end() || mask.at(tempNode) == false) {
                return tempNode;
            }
            
        }
    }
    return nullptr;
}

ILBatchCalculationGraphNode* ILBatchCalculationGraph::getLastNode() const
{
    for(int i = this->size() - 1; i >= 0; i--)
    {
        if(this->isLayerEmpty(i))
            continue;
        for(int j = this->at(i).size() - 1; j >= 0; j--)
        {
            ILBatchCalculationGraphNode* tempNode = this->at(i)[j];
            return tempNode;
        }
    }
    return nullptr;
}

ILBatchCalculationGraph ILBatchCalculationGraph::getSubGraphByOneGraphNode(
    const ILBatchCalculationGraphNode* nodeTop) const
{
    int layerId;
    int arrayId;
    ILBatchCalculationGraph startGraph;
    if(!findNode(nodeTop, layerId, arrayId))
        return startGraph;
    startGraph.rootGraph = const_cast<ILBatchCalculationGraph*>(this);
    for(int i = 0; i < this->size(); i++) {
        startGraph[i];
    }
    startGraph[layerId].push_back(const_cast<ILBatchCalculationGraphNode*>(nodeTop));
    return startGraph;
}

std::vector<ILBatchCalculationGraph> ILBatchCalculationGraph::getAllSubGraphByGraphNode(
    const std::unordered_map<ILBatchCalculationGraphNode*, bool>& mask,
    const ILBatchCalculationGraphNode* nodeTop,
    int depthConstrain, int countConstrain) const
{
    vector<ILBatchCalculationGraph> ret;
    int layerId;
    int arrayId;
    if(!findNode(nodeTop, layerId, arrayId))
        return ret;
    
    ILBatchCalculationGraph startGraph;
    startGraph.rootGraph = const_cast<ILBatchCalculationGraph*>(this);
    for(int i = 0; i < this->size(); i++) {
        startGraph[i];
    }
    startGraph[layerId].push_back(const_cast<ILBatchCalculationGraphNode*>(nodeTop));
    ret.push_back(startGraph);
    //if(nodeTop->operationType == StmtBatchCalculation::TypeAssign)
    //    return ret;

    int index = 0;
    while(index < ret.size())
    {
        vector<ILBatchCalculationGraph> tempSourceNodes = ret[index].getExtendOneNodeGraph(*this, mask, depthConstrain, countConstrain);
        
        for(int j = 0; j < tempSourceNodes.size(); j++)
        {
            bool hasSameGraph = false;
            for(int i = 0; i < ret.size(); i++)
            {
                if(tempSourceNodes[j].isSameAsAbsolutely(ret[i]))
                {
                    hasSameGraph = true;
                    break;
                }
            }
            if(hasSameGraph)
                continue;
            ret.push_back(tempSourceNodes[j]);
        }

        index++;
    }
    
    return ret;
}

std::vector<ILBatchCalculationGraph> ILBatchCalculationGraph::getExtendOneNodeGraph(
    const ILBatchCalculationGraph& graphComplete,
    const std::unordered_map<ILBatchCalculationGraphNode*, bool>& mask,
    int depthConstrain, int countConstrain) const
{
    vector<ILBatchCalculationGraph> ret;

    //ILBatchCalculationGraphNode* 后继节点 int
    map<ILBatchCalculationGraphNode*, int> allNearNodeDirect;

    for(int i = 0; i < this->size(); i++) {
        for(int j = 0; !this->isLayerEmpty(i) && j < this->at(i).size(); j++) {
            for(int k = 0; k < this->at(i)[j]->successor.size(); k++) {
                ILBatchCalculationGraphNode* node = this->at(i)[j]->successor[k];
                if(this->isContainNode(node) || mask.find(node) != mask.end() || node->operationType == StmtBatchCalculation::TypeAssign || node->operationType == StmtBatchCalculation::TypeReadData)
                    continue;
                // 更新该后继节点的层数
                allNearNodeDirect[node] = i + 1;
            }
            for(int k = 0; k < this->at(i)[j]->predecessor.size(); k++) {
                ILBatchCalculationGraphNode* node = this->at(i)[j]->predecessor[k];
                if(this->isContainNode(node) || mask.find(node) != mask.end() || node->operationType == StmtBatchCalculation::TypeAssign || node->operationType == StmtBatchCalculation::TypeReadData)
                    continue;
                // 更新该后继节点的层数
                allNearNodeDirect[node] = i + 1;
            }
        }
    }
    for(int i = 0; i < graphComplete.size(); i++) {
        for(int j = 0; !graphComplete.isLayerEmpty(i) && j < graphComplete.at(i).size(); j++) {
            ILBatchCalculationGraphNode* node = graphComplete.at(i)[j];
            if(allNearNodeDirect.find(node) != allNearNodeDirect.end()) {
                allNearNodeDirect[node] = i;
            }
        }
    }

    getExtendOneNodeGraphTryNearNodes(allNearNodeDirect, depthConstrain, countConstrain, ret);

    return ret;
}

void ILBatchCalculationGraph::getExtendOneNodeGraphTryNearNodes(const map<ILBatchCalculationGraphNode*, int> &allNearNodeDirect, int depthConstrain, int countConstrain, vector<ILBatchCalculationGraph> &ret) const
{
    for(auto iter = allNearNodeDirect.begin(); iter != allNearNodeDirect.end(); iter++) {
        ILBatchCalculationGraph tempGraph = *this;

        tempGraph[iter->second].push_back(iter->first);

        //comment = true
        //ILBatchCalculationGraph supplementaryGraph = tempGraph.getSupplementaryGraph(graphComplete);
        bool isConvexGraph = tempGraph.isConvexGraph();
        //comment = true
        //bool isConvexGraphSup = supplementaryGraph.isConvexGraph();
        int nodeCount = tempGraph.getNodeCount();
        int realLayerCount = tempGraph.getRealLayerCount();

        if(!isConvexGraph || nodeCount > countConstrain || // !isConvexGraphSup || 
            realLayerCount > depthConstrain)
            continue;

        bool hasSameGraph = false;
        for(int i = 0; i < ret.size(); i++) {
            if(tempGraph.isSameAsAbsolutely(ret[i])) {
                hasSameGraph = true;
                break;
            }
        }
        if(hasSameGraph)
            continue;

        ret.push_back(tempGraph);
    }
}

bool ILBatchCalculationGraph::isLayerEmpty(int layerId) const
{
    if(this->graph.find(layerId) == this->graph.end())
        return true;
    return this->graph.at(layerId).empty();
}

bool ILBatchCalculationGraph::isSameAsAbsolutely(const ILBatchCalculationGraph& graph) const
{
    if(graph.size() != this->size())
    {
        return false;
    }
    for(int i = 0; i < graph.size(); i++)
    {
        if(this->isLayerEmpty(i) && !graph.isLayerEmpty(i) ||
            graph.isLayerEmpty(i) && !this->isLayerEmpty(i)) {
            return false;
        }
        else if(this->isLayerEmpty(i) && graph.isLayerEmpty(i)) {
            continue;
        }
        if(graph.at(i).size() != this->at(i).size())
        {
            return false;
        }
        for(int j = 0; j < graph.at(i).size(); j++)
        {
            if(graph.at(i)[j] != this->at(i)[j])
            {
                return false;
            }
        }
    }
    return true;
}

bool ILBatchCalculationGraph::isConvexGraph() const
{
    for(int i = 0; i < this->size(); i++) {
        for(int j = 0; !this->isLayerEmpty(i) && j < this->at(i).size(); j++) {
            ILBatchCalculationGraphNode* tempNode = this->at(i)[j];
            if(isNodeCauseGraphNotConvex(tempNode)) {
                return false;
            }
        }
    }
    return true;
}

bool ILBatchCalculationGraph::findNode(const ILBatchCalculationGraphNode* node, int& layerId, int& arrayId) const
{
    for(int i = 0; i < this->size(); i++) {
        for(int j = 0; !this->isLayerEmpty(i) && j < this->at(i).size(); j++) {
            ILBatchCalculationGraphNode* tempNode = this->at(i)[j];
            if(tempNode == node) {
                layerId = i;
                arrayId = j;
                return true;
            }
        }
    }
    return false;
}

bool ILBatchCalculationGraph::isNodeInputSourceInner(const ILBatchCalculationGraphNodeInput* input) const
{
    for(int i = 0; i < this->size(); i++)
    {
        for(int j = 0; !this->isLayerEmpty(i) && j < this->at(i).size(); j++)
        {
            ILBatchCalculationGraphNode* tempNode = this->at(i)[j];
            for(int k = 0; k < tempNode->outputs.size(); k++)
            {
                if(tempNode->outputs[k].expression == input->expression)
                    return true;
            }
        }
    }
    return false;
}

void ILBatchCalculationGraph::release()
{
    for(int i = 0; i < this->size(); i++)
    {
        for(int j = 0; !this->isLayerEmpty(i) && j < this->at(i).size(); j++)
        {
            delete this->at(i)[j];
        }
    }
    this->graph.clear();
}

bool ILBatchCalculationGraph::isNodeCauseGraphNotConvex(const ILBatchCalculationGraphNode* node) const
{
    // 首先找出所有不在当前图的直接前驱
    vector<ILBatchCalculationGraphNode*> allDirectPredecessorDirect = node->getPredecessorDirect();
    for(int i = 0; i < this->size(); i++) {
        for(int j = 0; !this->isLayerEmpty(i) && j < this->at(i).size(); j++) {
            ILBatchCalculationGraphNode* tempNode = this->at(i)[j];
            auto iter = find(allDirectPredecessorDirect.begin(), allDirectPredecessorDirect.end(), tempNode);
            if(iter != allDirectPredecessorDirect.end()) {
                allDirectPredecessorDirect.erase(iter);
            }
        }
    }

    // 然后统计所有不在当前图的直接前驱节点的所有前驱
    vector<ILBatchCalculationGraphNode*> allPredecessorNode;
    for(int i = 0; i < allDirectPredecessorDirect.size(); i++) {
        ILBatchCalculationGraphNode* tempNode = allDirectPredecessorDirect[i];
        vector<ILBatchCalculationGraphNode*> tempSourceNodes = tempNode->getPredecessorAll();
        for(int j = 0; j < tempSourceNodes.size(); j++) {
            if(find(allPredecessorNode.begin(), allPredecessorNode.end(), tempSourceNodes[j]) == allPredecessorNode.end()) {
                allPredecessorNode.push_back(tempSourceNodes[j]);
            }
        }
    }
    // 最后判断计算图中是否存在这些前驱节点，存在则无法将该节点加入计算图
    for(int k = 0; k < allPredecessorNode.size(); k++) {
        if(this->isContainNode(allPredecessorNode[k])) {
            return true;
        }
    }

    return false;
}

ILBatchCalculationGraph ILBatchCalculationGraph::getSupplementaryGraph(
    const ILBatchCalculationGraph& graphComplete) const
{
    ILBatchCalculationGraph ret;
    for(int i = 0; i < graphComplete.size(); i++) {
        for(int j = 0; !graphComplete.isLayerEmpty(i) && j < graphComplete.at(i).size(); j++) {
            ILBatchCalculationGraphNode* tempNode = graphComplete.at(i)[j];
            
            if(!this->isContainNode(tempNode))
            {
                ret[i].push_back(tempNode);
            }
        }
    }
    return ret;
}
