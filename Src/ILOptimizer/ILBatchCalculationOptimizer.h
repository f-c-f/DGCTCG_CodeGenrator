#pragma once

#include <unordered_map>

#include "../ILBasic/ILAnalyzeSchedule.h"

class ILBatchCalculationGraphNodeInput;
class ILBatchCalculationGraphNodeParameter;
class StmtLocalVariable;
class ILBatchInstruction;
class StmtFor;
class ILBatchCalculationGraphNode;
class ILBatchCalculationGraph;
class ILRef;
class Statement;
class ILBranch;
class ILCalculate;
class Expression;
class ILAnalyzeScheduleNode;
class ILParser;
class ILSchedule;
class ILAnalyzeScheduleNodeVariableRef;


class ILBatchCalculationOptimizer
{
public:
	int optimize(ILSchedule* schedule, ILParser* parser);

    

private:
	ILSchedule* schedule = nullptr;
	ILParser* iLParser = nullptr;

    ILAnalyzeSchedule ops;

    // 尝试尽可能将多个StmtBatchCalculation组合到一起分析
    // 然后将其视为展开的for循环，以4/8/16的批计算为一组
    // 通过构建计算图，进行指令选择
	int optimize(); // 一次一次优化，直到无法再次优化

    std::map<ILGlobalVariable*, int> currentOptimizedAlignedGlobalVariableList;
    std::map<ILLocalVariable*, int> currentOptimizedAlignedLocalVariableList;
    std::map<StmtLocalVariable*, int> currentOptimizedAlignedStatementVariableList;
    std::vector<StmtLocalVariable*> currentOptimizedStatementVariableList;
    StmtLocalVariable* currentOptimizedStmtForIterVariable = nullptr;
    StmtFor* currentOptimizedStmtFor = nullptr;
    void clearCurrentOptimizedObject();
    void releaseCurrentOptimizedObject();

    int optimizeOnce(int &currentFindIndex);

    // 收集一组批量计算，这些批量计算可以放到同一个大循环里进行计算
    int collectBatchCalculation(int startFindIndex, ILBatchCalculationGraph& graph);
    bool collectBatchCalculationCanPassVarReadWrite(const ILAnalyzeScheduleNode* scheduleNode, const std::map<const ILAnalyzeScheduleVariable*, int>& nodeVarReadWriteLimit);

    // 将收集到的一组批量计算图转化为一个循环下的SIMD指令，不能转换成SIMD指令的转换为等价的若干句普通指令
    int translateBatchCalculationGraph(ILBatchCalculationGraph& graph);
    int translateBatchCalculationGraphOne(ILBatchCalculationGraph& graph, std::unordered_map<ILBatchCalculationGraphNode*, bool>& nodeMask, int simdCount, StmtFor* stmtFor);
    int translateBatchCalculationGraphOneBatchAssign(ILBatchCalculationGraphNode* assignNode, ILBatchCalculationGraph& graph, std::unordered_map<ILBatchCalculationGraphNode*, bool>& nodeMask, int simdCount, StmtFor* stmtFor);
    int translateBatchCalculationGraphOneBatchReadData(ILBatchCalculationGraphNode* readDataNode, ILBatchCalculationGraph& graph, std::unordered_map<ILBatchCalculationGraphNode*, bool>& nodeMask, int simdCount, StmtFor* stmtFor);
    void translateBatchCalculationGraphMask(const ILBatchCalculationGraph& translatedGraph, std::unordered_map<ILBatchCalculationGraphNode*, bool>& nodeMask) const;
    // 创建批量计算的for循环，如果计算图批计算次数较小，则无需创建for，stmtForRet为空
    int translateBatchCalculationGraphFor(const ILBatchCalculationGraph& graph, int simdCount, StmtFor** stmtForRet);
    int translateBatchCalculationGraphForIndex(std::vector<std::string>& variableForIndexStrList, ILBatchCalculationGraphNode* lastNode, std::vector<Statement*>* stmtList, std::vector<Statement*>* stmtList2, int& insertPos);
    int translateBatchCalculationGraphForStmt(int simdCount, StmtFor** stmtForRet, const std::vector<std::string>& variableForIndexStrList, ILBatchCalculationGraphNode* lastNode, int& insertPos);
    // 在for循环中创建批量计算图所需的初始变量，如果stmtFor为空，则在对应的图的最后一个节点对应的ILParser节点处插入局部变量
    int translateBatchCalculationGraphInputVariable(ILBatchCalculationGraph& graph, int simdCount, StmtFor* stmtFor);
    int translateBatchCalculationGraphInputVariableAddToInputList(ILBatchCalculationGraph& graph, int i, int j, std::vector<ILBatchCalculationGraphNodeInput*> &externalInputList);
    int translateBatchCalculationGraphInputVariableList(ILBatchCalculationGraph& graph, int simdCount, StmtFor* stmtFor, const std::vector<ILBatchCalculationGraphNodeInput*>& externalInputList, std::vector<Statement*>* stmtList, std::vector<Statement*>* stmtList2, int& insertPos);
    // 在for循环中，根据获得的指令的格式字符串中的输出生成输出变量
    int translateBatchCalculationGraphSubOutput(ILBatchCalculationGraph& graph, int simdCount, StmtFor* stmtFor, std::string format) const;

    static std::map<std::string, std::string> currentNodeTypeConvertMap;
    // 在for循环中，根据获得的指令的格式字符串中的输入数据的数据类型转换
    int translateBatchCalculationGraphInputConvert(ILBatchCalculationGraph& graph, int simdCount, StmtFor* stmtFor, std::string format) const;
    int translateBatchCalculationGraphInputConvertOne(const ILBatchCalculationGraph& graph, int simdCount, StmtFor* stmtFor, std::string variableName, std::string typeSimpleSrc, std::string typeSimpleDst) const;
    // 在for循环中，根据获得的指令的格式字符串中的输出数据的数据类型转换的临时变量
    int translateBatchCalculationGraphOutputConvertVariable(ILBatchCalculationGraph& graph, int simdCount, StmtFor* stmtFor, std::string format) const;
    // 在for循环中，根据获得的指令的格式字符串中的输出数据的数据类型转换
    int translateBatchCalculationGraphOutputConvert(ILBatchCalculationGraph& graph, int simdCount, StmtFor* stmtFor, std::string format) const;
    // 在for循环中添加批量计算子图对应的代码
    int translateBatchCalculationGraphCalculation(ILBatchCalculationGraph& graph, StmtFor* stmtFor, std::string insertCode) const;
    // 在for循环中的最后创建批量计算图所需的初始变量
    int translateBatchCalculationGraphOutputVariable(ILBatchCalculationGraph& graph, std::unordered_map<ILBatchCalculationGraphNode*, bool>& nodeMask, int simdCount, StmtFor* stmtFor);
    // 在ILParser中添加头文件
    int translateBatchCalculationAddHeadFile() const;

    int releaseStmtForAndReturn(int returnValue);

    // 尝试将一个批量计算合并到批量计算图中
    int tryToAddGraphNode(ILBatchCalculationGraph& graph, ILBatchCalculationGraphNode* node);

    // 判断尝试新加入的节点的所有非批量计算前驱的所有前驱不包括在现有计算图内，
    // 也就是判断是否存在下图中a节点(B为批量计算，a为非批量计算)
    // 存在这种情况则无法进行批量计算合并
    //    B       /
    //   / \      /
    //  B   a     /
    //   \ /      /
    //    B       /
    bool tryToAddGraphNodeIsNoDependent(const ILBatchCalculationGraph& graph, const ILBatchCalculationGraphNode* node) const;

    // 判断图中其它节点的语句是否能够迁移至当前节点的语句位置(因为要将这些节点合并到一个for循环里面)
    // 图中 所有其他节点依赖的读变量 在其节点位置到当前节点之间 没有被改写过
    // 并且 所有其他节点的写出变量 在其节点位置到当前节点之间 没有被读取过
    // 和能否构成凸计算图并不等价
    // 因为存在图中批量计算节点的数据源变量 在计算完之后 被未纳入图中的计算改写的情况
    // 或存在图中批量计算节点的输出变量 在计算完之后 被未纳入图中的计算读取的情况
    bool tryToAddGraphNodeCanGraphMove(const ILBatchCalculationGraph& graph, const ILBatchCalculationGraphNode* nodeDst);
    // 判断图中单个节点的语句是否能够迁移至当前节点的语句位置
    bool tryToAddGraphNodeCanNodeMove(const ILBatchCalculationGraph& graph, const ILBatchCalculationGraphNode* nodeSrc, const ILBatchCalculationGraphNode* nodeDst);
    
    // 通过ScheduleNode创建一个ILBatchCalculationGraphNode
    // 暂时设置输入输出的表达式为变量名，只用来初步匹配联通计算
    ILBatchCalculationGraphNode* createBatchCalculationGraphNodeByScheduleNode(
        const ILAnalyzeScheduleNode* node) const;

    // 判断一个节点是否会输出图外需要使用的变量
    bool isGraphNodeOutputNeedExport(const ILBatchCalculationGraph& graph, const ILBatchCalculationGraphNode* node) const;

    // 判断整个计算图的导出变量是否被指令消除了，比如乘加操作的乘在图外需要计算，但被单条乘加指令所隐藏了
    bool isInstructionMissGraphOutput(const ILBatchCalculationGraph& graph, const ILBatchCalculationGraph& subGraph, const ILBatchInstruction& instruction) const;

    bool isInstructionNeedOtherOutputButNodeGen(const ILBatchCalculationGraph& graph, const ILBatchInstruction& instruction) const;


    int setVariableAlign(std::string variableName, std::vector<void*> domain);
};
