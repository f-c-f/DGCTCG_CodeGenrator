#pragma once

#include <queue>

#include "../ILGenerator/MIRSchedule.h"
#include "../ILGenerator/MIRScheduleGraphDataflowNode.h"
#include "../ILBasic/StmtBatchCalculation.h"



class MIRModel;
class MIRFunctionDataflow;

class MIRActorParallelizationOptimizer
{
public:
	int optimize(MIRFunctionDataflow* function, MIRModel* model);

private:
    MIRModel* model = nullptr;
    MIRFunctionDataflow* function = nullptr;

    MIRSchedule schedule;

    static std::vector<std::string> supportedActorList;

    int optimize();

    class Graph
    {
    public:
        std::vector<MIRScheduleGraphDataflowNode*> nodeList;
        
    };

    // 存储同构子图中所有独立的图
    class SameGraph
    {
    public:
        std::vector<Graph> graphList;
    };

    // 从种子节点子图一点一点扩展来的用来表示可扩展的同构子图列表
    class ExtensibleSameGraph
    {
    public:
        std::vector<Graph> graphList;
    };

    class SameGraphGroup
    {
    public:
        

        // 基于种子节点，一点一点扩展出来的同构子图
        std::vector<SameGraph> sameGraphList;

        
        // 若能同时向外扩展一个节点，则直接在这些图中同时添加这一个节点
        // 比如同时扩展c
        // {a-b, a-b, a-b, a-b} -> {a-b-c, a-b-c, a-b-c, a-b-c}
        //
        // 若不能，
        // 先讲该组可扩展同构子图加入到同构子图列表，****
        // 之后则依据可扩展节点的包含关系，创建新的extensibleSameGraph
        // 比如1,2,3图能扩展c，2,3,4图能扩展d
        // {a-b, a-b, a-b, a-b} -> {a-b-c, a-b-c, a-b-c}
        //                      -> {       a-b-d, a-b-d, a-b-d}
        // 再比如1,2,3图能扩展c，1,2图能扩展d，由于能扩展c的图完全包含能扩展d的，
        // 所以这里不创建扩展d的extensibleSameGraph
        // 也就是，是否要创建某个extensibleSameGraph，
        // 看能扩展该节点的图集合是否被当前可同时扩展的其它图集合包含，
        // 包含则不创建
        // 对于一个extensibleSameGraph，
        // 需要找出该同构图集中所有可能的扩展节点(以扩展方位和节点类型确定唯一性)的
        // 可扩展图集合
        // {a-b, a-b, a-b, a-b} -> {a-b-c, a-b-c, a-b-c}
        std::queue<ExtensibleSameGraph> extensibleSameGraphList;
    };

    // *** 以下只针对同一组数据流图 *** \\


    // 同构子图组列表
    SameGraphGroup sameGraphGrouphList;
    MIRScheduleGraphDataflow* graphDataflow = nullptr;
    MIRFunction* mIRFunction = nullptr;

    // 收集同构子图种子子图
    int collectGraphNodeSeed();
    // 尝试扩展可扩展子图
    int tryToExtendExtensibleSameGraph();
    int tryToExtendExtensibleSameGraphOne();
    int tryToExtendExtensibleSameGraphOneDo();

    // *** 以上只针对同一组数据流图 *** //

    // 获取一个图的所有邻居节点
    std::vector<MIRScheduleGraphDataflowNode*> getGraphNeighborNodes(Graph& graph);

    //// 获取同构子图中可以扩展的端口位置列表，
    //// 比如一个加法节点时，{{1, 1}, {1, 2}, {1, -1}};
    //// 一组端口位置中第一个数表示图中的第几个节点，第二个数表示节点的第几个端口(正数为输入端口，负数为输出端口)
    //std::vector<std::pair<int, int>> getGraphExtendPositionList(Graph graph);

    // 根据一个节点，获取该节点在图中的位置关系
    std::vector<std::pair<int, int>> getExtendPositionListInGraphByNode(Graph& graph, MIRScheduleGraphDataflowNode* node);

    // 根据一组固定的位置关系，获取图中满足该位置的所有节点
    std::vector<MIRScheduleGraphDataflowNode*> getExtensibleNodesInGraphByPosition(Graph& graph, std::vector<std::pair<int, int>>& pos);

    // 根据一个节点和它在图中的位置，寻找其它(含自身)图中能扩展出来该节点的所有节点
    std::pair<int, std::vector<MIRScheduleGraphDataflowNode*>> getExtensibleNodesByNodeAndPos(ExtensibleSameGraph& sameGraph, int curSearchIndex, MIRScheduleGraphDataflowNode* node, std::vector<std::pair<int, int>>& pos);

    // 向可扩展节点组合列表中插入一个组合
    // 同时进行子集检查，移除掉该组合的子集
    // 且，如果该组合被包含，则不插入
    void insertToExtensibleNodesList(std::vector<std::pair<int, std::vector<MIRScheduleGraphDataflowNode*>>>& list, std::pair<int, std::vector<MIRScheduleGraphDataflowNode*>> extensibleNodes);
};

