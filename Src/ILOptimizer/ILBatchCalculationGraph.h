#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "../ILBasic/StmtBatchCalculation.h"


class ILAnalyzeScheduleNode;

class ILBatchCalculationGraphNodeInput
{
public:
    ILBatchCalculationGraphNodeInput();
    ILBatchCalculationGraphNodeInput(std::string expression, std::string type, const std::vector<int>& arraySize);
    ~ILBatchCalculationGraphNodeInput();
    std::string expression;
	std::string type;
    std::vector<int> arraySize;
};

class ILBatchCalculationGraphNodeParameter
{
public:
    ILBatchCalculationGraphNodeParameter();
    ILBatchCalculationGraphNodeParameter(std::string expression, std::string type);
    ~ILBatchCalculationGraphNodeParameter();
    std::string expression;
	std::string type;
};

class ILBatchCalculationGraphNodeOutput
{
public:
    ILBatchCalculationGraphNodeOutput();
    ILBatchCalculationGraphNodeOutput(std::string expression, std::string type, const std::vector<int>& arraySize);
    ~ILBatchCalculationGraphNodeOutput();
	std::string expression;
	std::string type;
    std::vector<int> arraySize;
};

// 同时支持批量计算 和 单个节点计算构成的同种运算的批量计算
class ILBatchCalculationGraphNode
{
public:
    ILAnalyzeScheduleNode* scheduleNode = nullptr;
    StmtBatchCalculation* statement = nullptr; // 当处理批量计算的时候才使用

    StmtBatchCalculation::OperationType operationType = StmtBatchCalculation::TypeUnknown;
    std::string dataType;
    StmtBatchCalculation::ModifierType modifierType = StmtBatchCalculation::TypeNone;
    StmtBatchCalculation::ShapeType shapeType = StmtBatchCalculation::TypeNormal;

    std::vector<int> calculateSize;

    std::vector<ILBatchCalculationGraphNodeInput> inputs;
    std::vector<ILBatchCalculationGraphNodeParameter> parameters;
    std::vector<ILBatchCalculationGraphNodeOutput> outputs;

    std::vector<ILBatchCalculationGraphNode*> predecessor;
    std::vector<ILBatchCalculationGraphNode*> successor;

    bool isCalculateSizeMatch(const ILBatchCalculationGraphNode* node) const;

    std::vector<ILBatchCalculationGraphNode*> getPredecessorDirect() const;
    std::vector<ILBatchCalculationGraphNode*> getPredecessorAll() const;

};

// 批量计算数据流图
// 该图可能存在某些层为空层，但该层也必须存在一个空的数组
// 可能仅仅是某大图中的完全对应的一小部分
class ILBatchCalculationGraph
{
public:

    std::map<int, std::vector<ILBatchCalculationGraphNode*>> graph;

    const ILBatchCalculationGraph* getRootGraph() const;

    void setNodeIONameMap(std::string name, std::string mapName);
    std::string getNodeIONameMap(std::string name) const;

    bool empty() const;
    std::vector<ILBatchCalculationGraphNode*>& operator[](int index);
    const std::vector<ILBatchCalculationGraphNode*>& at(int index) const;
    int size() const;

    bool isContainNode(const ILBatchCalculationGraphNode* node) const;
    bool isContainScheduleNode(const ILAnalyzeScheduleNode* node) const;

    int getNodeCount() const;
    int getRealLayerCount() const;
    
    ILBatchCalculationGraphNode* getFirstNode() const;
    ILBatchCalculationGraphNode* getFirstNode(const std::unordered_map<ILBatchCalculationGraphNode*, bool>& mask) const;
    
    ILBatchCalculationGraphNode* getLastNode() const;

    // 以一个节点为最顶端节点，返回计算图中所有符合约束条件的子图
    ILBatchCalculationGraph getSubGraphByOneGraphNode(const ILBatchCalculationGraphNode* nodeTop) const;
    std::vector<ILBatchCalculationGraph> getAllSubGraphByGraphNode(const std::unordered_map<ILBatchCalculationGraphNode*, bool>& mask, const ILBatchCalculationGraphNode* nodeTop, int depthConstrain, int countConstrain) const;
    std::vector<ILBatchCalculationGraph> getExtendOneNodeGraph(const ILBatchCalculationGraph& graphComplete, const std::unordered_map<ILBatchCalculationGraphNode*, bool>& mask, int depthConstrain, int countConstrain) const;
    void getExtendOneNodeGraphTryNearNodes(const std::map<ILBatchCalculationGraphNode*, int> &allNearNodeDirect, int depthConstrain, int countConstrain, std::vector<ILBatchCalculationGraph> &ret) const;

    bool isLayerEmpty(int layerId) const;

    bool isSameAsAbsolutely(const ILBatchCalculationGraph& graph) const;

    bool isConvexGraph() const;

    bool findNode(const ILBatchCalculationGraphNode* node, int &layerId, int &arrayId) const;

    bool isNodeInputSourceInner(const ILBatchCalculationGraphNodeInput* input) const;

    void release();

    
private:

    const ILBatchCalculationGraph* rootGraph = nullptr;

    std::map<std::string, std::string> variableNameMap;
    
    bool isNodeCauseGraphNotConvex(const ILBatchCalculationGraphNode* node) const;

    ILBatchCalculationGraph getSupplementaryGraph(const ILBatchCalculationGraph& graphComplete) const;
};
