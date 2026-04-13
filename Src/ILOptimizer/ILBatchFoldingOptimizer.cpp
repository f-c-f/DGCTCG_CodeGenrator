#include "ILBatchFoldingOptimizer.h"

#include <algorithm>
#include <unordered_map>


#include "ILOptimizer.h"

#include "../ILBasic/ILAnalyzeScheduleNode.h"

#include "../ILBasic/ILSchedule.h"
#include "../ILBasic/ILParser.h"
#include "../ILBasic/ILExpressionParser.h"
#include "../ILBasic/Expression.h"
#include "../ILBasic/StmtBatchCalculation.h"
#include "../ILBasic/StmtLocalVariable.h"
#include "../ILBasic/StmtFor.h"
#include "../ILBasic/StmtExpression.h"
#include "../Common/Utility.h"
#include "../ILBasic/ILFile.h"
#include "../ILBasic/ILHeadFile.h"
#include "../ILBasic/ILLocalVariable.h"
#include "../ILBasic/ILGlobalVariable.h"

using namespace std;


int ILBatchFoldingOptimizer::optimize(ILSchedule* schedule, ILParser* parser)
{
    this->schedule = schedule;
    this->iLParser = parser;

    int res = ops.collectSchedule(schedule, parser);
    if(res < 0)
        return res;

    //print();


    res = optimize();
    if(res < 0)
        return res;
    
    
	return 1;
}



int ILBatchFoldingOptimizer::optimize()
{
    int optimizedCount = 0;

    bool printProcess = false;

    if(printProcess)
        ops.print();

    
    int currentFindIndex = 0;
    int res = 1;
    while(res > 0)
    {
        res = optimizeOnce(currentFindIndex);
        
        if(printProcess)
            ops.print();
    }
    if(res < 0)
        return res;

	return 1;
}

int ILBatchFoldingOptimizer::optimizeOnce(int& currentFindIndex)
{
    int res;
    const vector<ILAnalyzeScheduleNode*>* nodeList = ops.getScheduleNodeList();

    int i = currentFindIndex;

    bool canOptimize = false;

    for (; i < nodeList->size(); i++)
    {
        currentFindIndex = i;

        const ILAnalyzeScheduleNode* node = (*nodeList)[i];

        if (node->nodeType != ILAnalyzeScheduleNode::TypeStatement)
        {
            continue;
        }

        if (node->statement->type != Statement::BatchCalculation)
        {
            continue;
        }

        StmtBatchCalculation* batchCalculation = static_cast<StmtBatchCalculation*>(node->statement);

        if (batchCalculation->operationType != StmtBatchCalculation::TypeReadData)
        {
            continue;
        }

        StmtBatchCalculation* retNode = nullptr;
        int retNodeInputIndex = -1;
        optimizeOnceFindNextReadBeforeWrite(i + 1, batchCalculation, retNode, retNodeInputIndex);

        if (retNode == nullptr)
        {
            continue;
        }

        // 找到了可以合并的节点

        retNode->inputs[retNodeInputIndex].name = batchCalculation->inputs[0].name;

        // // 删除当前节点
        // batchCalculation->parentStatement->removeChildStatement(batchCalculation);

        canOptimize = true;

        //currentFindIndex = i + 1;
        break;
    }

    //if(!canOptimize)
    //    currentFindIndex = i + 1;

    // 重新计算调度节点
    ops.release();
    res = ops.collectSchedule(schedule, iLParser);
    if (res <= 0)
        return res;

    if (canOptimize)
        return 1;
    else
        return 0;
}

int ILBatchFoldingOptimizer::optimizeOnceFindNextReadBeforeWrite(int currentFindIndex, const StmtBatchCalculation* readDataNode, StmtBatchCalculation*& retNode, int& retNodeInputIndex)
{
    int res;
    const vector<ILAnalyzeScheduleNode*>* nodeList = ops.getScheduleNodeList();

    int i = currentFindIndex;

    retNode = nullptr;
    retNodeInputIndex = -1;


    for (; i < nodeList->size(); i++)
    {
        const ILAnalyzeScheduleNode* node = (*nodeList)[i];
        if (node->nodeType != ILAnalyzeScheduleNode::TypeStatement)
        {
            continue;
        }

        if (node->statement->type != Statement::BatchCalculation)
        {
            continue;
        }

        StmtBatchCalculation* batchCalculation = static_cast<StmtBatchCalculation*>(node->statement);

        if (batchCalculation->operationType == StmtBatchCalculation::TypeAssign)
        {
            if (batchCalculation->outputs[0].name == readDataNode->inputs[0].name)
            {
                return 0;
            }
        }


        for (int j = 0; j < batchCalculation->inputs.size(); j++)
        {
            if (batchCalculation->inputs[j].name == readDataNode->outputs[0].name)
            {
                retNode = batchCalculation;
                retNodeInputIndex = j;

                return 1;
            }
        }

    }


    return 0;
}
