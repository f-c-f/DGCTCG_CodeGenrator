#pragma once

#include <unordered_map>

#include "../ILBasic/ILAnalyzeSchedule.h"

class StmtLocalVariable;
class StmtFor;
class ILRef;
class Statement;
class ILBranch;
class ILCalculate;
class Expression;
class ILAnalyzeScheduleNode;
class ILParser;
class ILSchedule;
class ILAnalyzeScheduleNodeVariableRef;
class StmtBatchCalculation;

class ILBatchFoldingOptimizer
{
public:
	int optimize(ILSchedule* schedule, ILParser* parser);

private:
	ILSchedule* schedule = nullptr;
	ILParser* iLParser = nullptr;

    ILAnalyzeSchedule ops;

	int optimize(); // 一次一次优化，直到无法再次优化

    int optimizeOnce(int &currentFindIndex);

    int optimizeOnceFindNextReadBeforeWrite(int currentFindIndex, const StmtBatchCalculation* readDataNode, StmtBatchCalculation*& retNode, int &retNodeInputIndex);
};
