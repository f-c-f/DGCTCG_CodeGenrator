#pragma once
#include <map>
#include <string>
#include <vector>

#include "../ILBasic/ILAnalyzeSchedule.h"

class ILRef;
class Statement;
class ILBranch;
class ILCalculate;
class Expression;
class ILAnalyzeScheduleNode;
class ILParser;
class ILSchedule;
class ILAnalyzeScheduleNodeVariableRef;


class ILScheduleOptimizer
{
public:
	int optimize(ILSchedule* schedule, ILParser* parser);

    

private:
	ILSchedule* schedule = nullptr;
	ILParser* iLParser = nullptr;

    ILAnalyzeSchedule ops;

	int optimize(); // 一次一次优化，直到无法再次优化

    int optimizeOnceFold(); // 尝试一次表达式折叠优化,由于可能存在去掉后面的冗余赋值导致前面又有可折叠的计算，所以该函数每次都从头寻找
    int optimizeOnceFoldVariable(ILAnalyzeScheduleNodeVariableRef* variableStore, ILAnalyzeScheduleNodeVariableRef* variableRead);
    int optimizeOnceFoldLocalVariable(ILAnalyzeScheduleNodeVariableRef* variableStore, ILAnalyzeScheduleNodeVariableRef* variableRead);
    int optimizeOnceFoldStatementLocalVariable(ILAnalyzeScheduleNodeVariableRef* variableStore, ILAnalyzeScheduleNodeVariableRef* variableRead);
    int optimizeOnceFoldExpressionAssignedVariable(ILAnalyzeScheduleNodeVariableRef* variableStore, ILAnalyzeScheduleNodeVariableRef* variableRead);
    int optimizeOnceFoldMergeExpression(const Expression* src, Expression* dst) const;
    int optimizeOnceFoldMergeRef(ILAnalyzeScheduleNode* src, ILAnalyzeScheduleNode* dst) const;
    int optimizeOnceFoldMergeInput(ILAnalyzeScheduleNode* src, ILAnalyzeScheduleNode* dst) const;
    int optimizeOnceFoldMergeOutput(ILAnalyzeScheduleNode* src, ILAnalyzeScheduleNode* dst) const;
    // comment = true;
    // int optimizeOnceFoldMergeNodeVariable(ILOptimizeScheduleNodeVariableRef* variableStore, ILOptimizeScheduleNodeVariableRef* variableRead);

    int optimizeOnceRedundancy(); // 尝试一次冗余变量去除优化，由于可能存在去掉后面的冗余赋值导致前面又有冗余计算，所以该函数每次都从头寻找
    int optimizeOnceRedundancyVariable(ILAnalyzeScheduleNodeVariableRef* variableStore);
    int optimizeOnceRedundancyLocalVariable(ILAnalyzeScheduleNodeVariableRef* variableStore);
    int optimizeOnceRedundancyStatementLocalVariable(ILAnalyzeScheduleNodeVariableRef* variableStore);
    int optimizeOnceRedundancyExpressionAssignedVariable(ILAnalyzeScheduleNodeVariableRef* variableStore);
    int optimizeOnceRedundancyStatementBatchCalculation(ILAnalyzeScheduleNodeVariableRef* variableStore);

    int optimizeOnceOnlyRead(); // 尝试一次只读语句去除优化，由于可能存在去掉后面的冗余赋值导致前面又有冗余计算，所以该函数每次都从头寻找
    int optimizeOnceOnlyReadFindNode(ILAnalyzeScheduleNode* &retNode);
    int optimizeOnceOnlyReadRemove(ILAnalyzeScheduleNode* node);
    bool optimizeOnceOnlyReadFindNodeOne(ILAnalyzeScheduleNode* node) const;

    int optimizeSimplyILNode() const;
    int optimizeSimplyILNodeRefList(std::vector<ILRef*>* refList) const;
    
};
