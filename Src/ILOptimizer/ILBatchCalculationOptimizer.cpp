#include "ILBatchCalculationOptimizer.h"

#include <algorithm>
#include <unordered_map>


#include "ILOptimizer.h"

#include "../ILBasic/ILAnalyzeScheduleNode.h"
#include "ILBatchInstructionProvider.h"
#include "ILBatchCalculationGraph.h"

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

namespace
{
    const int CONST_NUM_2 = 2;
    const int CONST_NUM_3 = 3;
    const int CONST_NUM_16 = 16;
}

std::map<std::string, std::string> ILBatchCalculationOptimizer::currentNodeTypeConvertMap;

int ILBatchCalculationOptimizer::optimize(ILSchedule* schedule, ILParser* parser)
{
    this->schedule = schedule;
    this->iLParser = parser;

    int res = ops.collectSchedule(schedule, parser);
    if(res < 0)
        return res;

    //print();

    ILBatchInstructionProvider::loadInstructionConfig();

    res = optimize();
    if(res < 0)
        return res;
    
    
	return 1;
}



int ILBatchCalculationOptimizer::optimize()
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

void ILBatchCalculationOptimizer::clearCurrentOptimizedObject()
{
    currentOptimizedAlignedGlobalVariableList.clear();
    currentOptimizedAlignedLocalVariableList.clear();
    currentOptimizedAlignedStatementVariableList.clear();
    currentOptimizedStmtForIterVariable = nullptr;
    currentOptimizedStmtFor = nullptr;
}

void ILBatchCalculationOptimizer::releaseCurrentOptimizedObject()
{
    
    for(auto iter = currentOptimizedAlignedGlobalVariableList.begin(); iter != currentOptimizedAlignedGlobalVariableList.end(); iter++)
    {
        iter->first->align = iter->second;
    }
    for(auto iter = currentOptimizedAlignedLocalVariableList.begin(); iter != currentOptimizedAlignedLocalVariableList.end(); iter++)
    {
        iter->first->align = iter->second;
    }
    for(auto iter = currentOptimizedAlignedStatementVariableList.begin(); iter != currentOptimizedAlignedStatementVariableList.end(); iter++)
    {
        iter->first->align = iter->second;
    }
    if(currentOptimizedStmtForIterVariable)
        iLParser->releaseStatement(currentOptimizedStmtForIterVariable);
    if(currentOptimizedStmtFor)
        iLParser->releaseStatement(currentOptimizedStmtFor);
    clearCurrentOptimizedObject();
}

int ILBatchCalculationOptimizer::optimizeOnce(int &currentFindIndex)
{
    
    ILBatchCalculationGraph graph;

    clearCurrentOptimizedObject();

    // 收集一组批量计算，这些批量计算可以放到同一个大循环里进行计算
    int res = collectBatchCalculation(currentFindIndex, graph);
    if(res <= 0)
        return res;

    if(graph.empty())
        return 0;

    ILBatchCalculationGraphNode* firstGraphNode = graph.getFirstNode();
    if(!firstGraphNode)
        return 0;

    currentFindIndex = firstGraphNode->scheduleNode->id + 1;
    
    // 将收集到的一组批量计算图转化为一个循环下的SIMD指令，不能转换成SIMD指令的转换为等价的若干句普通指令
    res = translateBatchCalculationGraph(graph);
    if(res < 0)
        return res;

    graph.release();



    // 重新计算调度节点
    ops.release();
    res = ops.collectSchedule(schedule, iLParser) & res;
    if(res <= 0)
        return res;


	return 1;
}

int ILBatchCalculationOptimizer::collectBatchCalculation(int startFindIndex, ILBatchCalculationGraph& graph)
{
    const vector<ILAnalyzeScheduleNode*>* scheduleNodes = ops.getScheduleNodeList();
    map<const ILAnalyzeScheduleVariable*, int> nodeVarReadWriteLimit; // 1表示不能写，2表示不能读写。用来分割批量计算图
    // Batch_B = Batch_A;
    // B[i] = c;
    // Batch_C = Batch_B; // 这种情况两个Batch操作就要分开。
    // 在遍历Node过程中，记录图中间被插入的非Batch节点，然后在后面对批量节点添加合并限制

    vector<ILAnalyzeScheduleNode*> ret;
    for(int i = startFindIndex; i < scheduleNodes->size(); i++) {
        ILAnalyzeScheduleNode* scheduleNode = (*scheduleNodes)[i];

        bool canNotJoinGraph = false;
        if(scheduleNode->nodeType != ILAnalyzeScheduleNode::TypeStatement ||
            scheduleNode->statement->type != Statement::BatchCalculation) {
            canNotJoinGraph = true;
        } else if(((StmtBatchCalculation*)scheduleNode->statement)->operationType >= StmtBatchCalculation::TypeDotMul) {
            canNotJoinGraph = true;
        } else if(graph.empty()) {
            graph[0].push_back(createBatchCalculationGraphNodeByScheduleNode(scheduleNode));
            continue;
        }

        if(canNotJoinGraph == false)
        {
            ILBatchCalculationGraphNode* firstGraphNode = graph.getFirstNode();
            if(!firstGraphNode && canNotJoinGraph == false)
                return 0;
            ILBatchCalculationGraphNode* tempGraphNode = createBatchCalculationGraphNodeByScheduleNode(scheduleNode);
            if(!tempGraphNode->isCalculateSizeMatch(firstGraphNode)) {
                delete tempGraphNode;
                canNotJoinGraph = true;
            } else if(!ops.isSameDomain(firstGraphNode->scheduleNode->domain, scheduleNode->domain)) {
                delete tempGraphNode;
                canNotJoinGraph = true;
            } else if(collectBatchCalculationCanPassVarReadWrite(scheduleNode, nodeVarReadWriteLimit) == false) {
                delete tempGraphNode;
                canNotJoinGraph = true;
            } else if(tryToAddGraphNode(graph, tempGraphNode) == 0) {
                delete tempGraphNode;
                canNotJoinGraph = true;
            }
        }

        // 处理不能加入当前图的Node，收集它们的读写数据，对之后批量Node的添加进行限制
        if(canNotJoinGraph && !graph.empty())
        {
            for(auto v : scheduleNode->variableRead)
            {
                nodeVarReadWriteLimit[v->variable] = nodeVarReadWriteLimit[v->variable] == 2 ? 2 : 1;
            }
            for(auto v : scheduleNode->variableStored)
            {
                nodeVarReadWriteLimit[v->variable] = 2;
            }
        }
    }
    if(graph.empty())
        return 0;
    return 1;
}

bool ILBatchCalculationOptimizer::collectBatchCalculationCanPassVarReadWrite(
    const ILAnalyzeScheduleNode* scheduleNode, 
    const map<const ILAnalyzeScheduleVariable*, int>& nodeVarReadWriteLimit)
{
    for(auto v : scheduleNode->variableRead)
    {
        if(nodeVarReadWriteLimit.find(v->variable) == nodeVarReadWriteLimit.end())
            continue;
        if(nodeVarReadWriteLimit.at(v->variable) == 2)
            return false;
    }
    for(auto v : scheduleNode->variableStored)
    {
        if(nodeVarReadWriteLimit.find(v->variable) == nodeVarReadWriteLimit.end())
            continue;
        if(nodeVarReadWriteLimit.at(v->variable) >= 1)
            return false;
    }
    return true;
}

int ILBatchCalculationOptimizer::translateBatchCalculationGraph(ILBatchCalculationGraph& graph)
{
    int res;
    StmtFor* stmtFor = nullptr;
    unordered_map<ILBatchCalculationGraphNode*, bool> nodeMask;
    int simdCount = ILBatchInstructionProvider::calculateInstructionBatchCount(graph);
    if(simdCount < CONST_NUM_2)
        return 0;
    else if((res = translateBatchCalculationGraphFor(graph, simdCount, &stmtFor)) <= 0)
        return releaseStmtForAndReturn(res);
    else if((res = translateBatchCalculationGraphInputVariable(graph, simdCount, stmtFor)) <= 0)
        return releaseStmtForAndReturn(res);

    // 尝试尽可能地翻译节点，直到所有节点被翻译完
    while(nodeMask.size() < graph.getNodeCount()) {
        if((res = translateBatchCalculationGraphOne(graph, nodeMask, simdCount, stmtFor))<=0)
            return releaseStmtForAndReturn(res);
    }
    if((res = translateBatchCalculationGraphOutputVariable(graph, nodeMask, simdCount, stmtFor)) <= 0)
        return releaseStmtForAndReturn(res);
    // 移除批量运算的中间代码
    for(int i = 0; i < graph.size(); i++) {
        for(int j = 0; !graph.isLayerEmpty(i) && j < graph.at(i).size(); j++) {
            ILBatchCalculationGraphNode* tempNode = graph.at(i)[j];
            iLParser->releaseStatement(tempNode->statement);
        }
    }
    return 1;
}

int ILBatchCalculationOptimizer::translateBatchCalculationGraphOne(ILBatchCalculationGraph& graph, unordered_map<ILBatchCalculationGraphNode*, bool>& nodeMask, int simdCount, StmtFor* stmtFor)
{
    int res;
    ILBatchCalculationGraphNode* firstNode = graph.getFirstNode(nodeMask);
    if(!firstNode)
        return releaseStmtForAndReturn(ILOptimizer::ErrorInternal);

    
    if(firstNode->operationType == StmtBatchCalculation::TypeAssign)
    {
        res = translateBatchCalculationGraphOneBatchAssign(firstNode, graph, nodeMask, simdCount, stmtFor);
        return 1;
    }
    if(firstNode->operationType == StmtBatchCalculation::TypeReadData)
    {
        res = translateBatchCalculationGraphOneBatchReadData(firstNode, graph, nodeMask, simdCount, stmtFor);
        return 1;
    }

    // 获取基于第一个节点的全部满足约束的子图
    vector<ILBatchCalculationGraph> subGraphList = graph.getAllSubGraphByGraphNode(nodeMask, firstNode, CONST_NUM_2, CONST_NUM_2);
    ILBatchCalculationGraph& translatedGraph = subGraphList[0];
    ILBatchInstruction instruction;
    // 按子图的从大到小进行指令获取的尝试
    for(int i = subGraphList.size() - 1; i >= 0; i--) {
        if(ILBatchInstructionProvider::getInstruction(subGraphList[i], simdCount, instruction) > 0) {
            if(isInstructionMissGraphOutput(graph, subGraphList[i], instruction) ||
                isInstructionNeedOtherOutputButNodeGen(graph, instruction)) {
                continue;
            }
            translatedGraph = subGraphList[i];
            if((res = translateBatchCalculationAddHeadFile()) <= 0)
                return releaseStmtForAndReturn(res);
            break;
        }
    }
    currentNodeTypeConvertMap.clear();
    //若无法进行SIMD运算则翻译为普通指令
    if(instruction.format.empty() && ILBatchInstructionProvider::getNormalInstruction(subGraphList[0], simdCount, instruction) <= 0) {
        return releaseStmtForAndReturn(0); //ILOptimizer::ErrorInternal
    } else if(instruction.format.empty()) {
        return releaseStmtForAndReturn(0);
    } else if((res = translateBatchCalculationGraphSubOutput(graph, simdCount, stmtFor, instruction.format)) <= 0) {
        // 根据获得的指令的格式字符串中的输出，生成输出变量
        return releaseStmtForAndReturn(res);
    }else if((res = translateBatchCalculationGraphInputConvert(graph, simdCount, stmtFor, instruction.format)) <= 0) {
        // 根据获得的指令的格式字符串中的输入，生成必要的数据类型转换
        return releaseStmtForAndReturn(res);
    }else if((res = translateBatchCalculationGraphOutputConvertVariable(graph, simdCount, stmtFor, instruction.format)) <= 0) {
        // 根据获得的指令的格式字符串中的输出，生成必要的数据类型转换
        return releaseStmtForAndReturn(res);
    }
    // 处理计算图中的变量名，将其作为指令的参数填进去
    string retCode = instruction.getFormattedCode(graph, currentNodeTypeConvertMap);
    // 插入到ILParser中
    if((res = translateBatchCalculationGraphCalculation(graph, stmtFor, retCode)) <= 0) {
        return releaseStmtForAndReturn(res);
    } else if((res = translateBatchCalculationGraphOutputConvert(graph, simdCount, stmtFor, instruction.format)) <= 0) {
        // 根据获得的指令的格式字符串中的输出，生成必要的数据类型转换
        return releaseStmtForAndReturn(res);
    }
    translateBatchCalculationGraphMask(translatedGraph, nodeMask);
    return 1;
}

int ILBatchCalculationOptimizer::translateBatchCalculationGraphOneBatchAssign(ILBatchCalculationGraphNode* assignNode, ILBatchCalculationGraph& graph,
    std::unordered_map<ILBatchCalculationGraphNode*, bool>& nodeMask, int simdCount, StmtFor* stmtFor)
{
    int res;

    ILBatchCalculationGraph startGraph = graph.getSubGraphByOneGraphNode(assignNode);

    ILBatchInstruction instruction;
    if(ILBatchInstructionProvider::getBatchStoreInstruction(assignNode->dataType, simdCount, instruction) > 0) {
        if(isInstructionMissGraphOutput(graph, startGraph, instruction)/* ||
            isInstructionNeedOtherOutputButNodeGen(graph, instruction)*/) {
            res = -ILOptimizer::ErrorBatchAssignCanNotFindDataSource;
            return releaseStmtForAndReturn(res);
        }
        if((res = translateBatchCalculationAddHeadFile()) <= 0)
            return releaseStmtForAndReturn(res);
    } else {
        res = -ILOptimizer::ErrorBatchAssignDoNotHaveSIMDInstructionSupport;
        return releaseStmtForAndReturn(res);
    }
    currentNodeTypeConvertMap.clear();
    
    if(instruction.format.empty()) {
        res = -ILOptimizer::ErrorBatchAssignDoNotHaveSIMDInstructionSupport;
        return releaseStmtForAndReturn(res);
    }
    
    
    // 处理计算图中的变量名，将其作为指令的参数填进去
    string inputDataName = assignNode->inputs[0].expression;

    if(assignNode->inputs[0].type != assignNode->dataType)
    {
        if((res = translateBatchCalculationGraphInputConvertOne(graph, simdCount, stmtFor, inputDataName, assignNode->inputs[0].type, assignNode->dataType)) <= 0) {
            // 根据获得的指令的格式字符串中的输入，生成必要的数据类型转换
            return releaseStmtForAndReturn(res);
        }
    }
    string tempVarName;
    if(currentNodeTypeConvertMap.find(inputDataName) != currentNodeTypeConvertMap.end())
        tempVarName = currentNodeTypeConvertMap.at(inputDataName);
    else
        tempVarName = graph.getNodeIONameMap(inputDataName);
    if(tempVarName.empty())
    {
        vector<void*> domain = stmtFor->getDomain();
        //domain.pop_back();
        tempVarName = ILParser::getAvailableVariableName(inputDataName + "_Batch", domain);
        ILParser::registerVariableName(tempVarName, domain);
        graph.setNodeIONameMap(inputDataName, tempVarName);
        StmtLocalVariable* stmtLocalVar = new StmtLocalVariable(stmtFor);
    
        stmtFor->statements.push_back(stmtLocalVar);
        stmtFor->childStatements.push_back(stmtLocalVar);

        stmtLocalVar->name = tempVarName;
        stmtLocalVar->dataType = ILBatchInstructionProvider::getBatchVariableType(assignNode->dataType, simdCount);
    }
    
    


    string outputDataName = assignNode->outputs[0].expression;
    string retCode = instruction.getFormattedDataStoreCode(tempVarName, outputDataName + "+" + stmtFor->condition->childExpressions[0]->expressionStr);
    // 插入到ILParser中
    if((res = translateBatchCalculationGraphCalculation(graph, stmtFor, retCode)) <= 0) {
        return releaseStmtForAndReturn(res);
    }
    translateBatchCalculationGraphMask(startGraph, nodeMask);
    return 1;
}

int ILBatchCalculationOptimizer::translateBatchCalculationGraphOneBatchReadData(
    ILBatchCalculationGraphNode* readDataNode, ILBatchCalculationGraph& graph,
    std::unordered_map<ILBatchCalculationGraphNode*, bool>& nodeMask, int simdCount, StmtFor* stmtFor)
{
    int res;

    ILBatchCalculationGraph startGraph = graph.getSubGraphByOneGraphNode(readDataNode);

    ILBatchInstruction instruction;
    if(ILBatchInstructionProvider::getBatchLoadInstruction(readDataNode->dataType, simdCount, simdCount, instruction) > 0) {
        if(isInstructionMissGraphOutput(graph, startGraph, instruction)/* ||
            isInstructionNeedOtherOutputButNodeGen(graph, instruction)*/) {
            res = -ILOptimizer::ErrorBatchReadDataCanNotFindDataSource;
            return releaseStmtForAndReturn(res);
        }
        if((res = translateBatchCalculationAddHeadFile()) <= 0)
            return releaseStmtForAndReturn(res);
    } else {
        res = -ILOptimizer::ErrorBatchReadDataDoNotHaveSIMDInstructionSupport;
        return releaseStmtForAndReturn(res);
    }
    currentNodeTypeConvertMap.clear();
    
    if(instruction.format.empty()) {
        res = -ILOptimizer::ErrorBatchReadDataDoNotHaveSIMDInstructionSupport;
        return releaseStmtForAndReturn(res);
    }
    
    
    ILBatchCalculationGraphNodeOutput* output = &(readDataNode->outputs[0]);
    string varStr = output->expression;
    string tempVarName = graph.getNodeIONameMap(varStr);
    if(tempVarName.empty())
    {
        vector<void*> domain = stmtFor->getDomain();
        //domain.pop_back();
        tempVarName = ILParser::getAvailableVariableName(output->expression + "_Batch", domain);
        ILParser::registerVariableName(tempVarName, domain);
        graph.setNodeIONameMap(output->expression, tempVarName);
    }
    
    StmtLocalVariable* stmtLocalVar = new StmtLocalVariable(stmtFor);
    
    stmtFor->statements.push_back(stmtLocalVar);
    stmtFor->childStatements.push_back(stmtLocalVar);

    stmtLocalVar->name = tempVarName;
    stmtLocalVar->dataType = ILBatchInstructionProvider::getBatchVariableType(output->type, simdCount);
    if(stmtLocalVar->dataType.empty())
        return 0;

    
    string inputDataName = readDataNode->inputs[0].expression;
    string retCode = instruction.getFormattedDataLoadCode(inputDataName + "+" + stmtFor->condition->childExpressions[0]->expressionStr);//, outputDataName + "+" + stmtFor->condition->childExpressions[0]->expressionStr

    ILExpressionParser expParser;
    stmtLocalVar->defaultValue = expParser.parseExpression(retCode);
    
    translateBatchCalculationGraphMask(startGraph, nodeMask);
    return 1;
}

void ILBatchCalculationOptimizer::translateBatchCalculationGraphMask(
    const ILBatchCalculationGraph& translatedGraph,
    std::unordered_map<ILBatchCalculationGraphNode*, bool>& nodeMask) const
{
    // mark图的遮罩，继续转换图中剩下的节点
    for(int i = 0; i < translatedGraph.size(); i++) {
        for(int j = 0; !translatedGraph.isLayerEmpty(i) && j < translatedGraph.at(i).size(); j++) {
            ILBatchCalculationGraphNode* tempNode = translatedGraph.at(i)[j];
            nodeMask[tempNode] = true;
        }
    }
}

int ILBatchCalculationOptimizer::translateBatchCalculationGraphFor(
    const ILBatchCalculationGraph& graph, int simdCount, StmtFor** stmtForRet)
{
    ILBatchCalculationGraphNode* firstNode = graph.getFirstNode();
    if(!firstNode)
        return -ILOptimizer::ErrorInternal;
    vector<Statement*>* stmtList = firstNode->statement->parentStatement->getStatementList();
    vector<Statement*>* stmtList2 = &firstNode->statement->parentStatement->childStatements;
    if(!stmtList || !stmtList2)
        return -ILOptimizer::ErrorInternal;

    int insertPos = 0;
    for(int i = 0; i < stmtList->size(); i++) {
        if((*stmtList)[i] == firstNode->statement) {
            insertPos = i;
            break;
        }
    }

    vector<string> variableForIndexStrList;
    int res = translateBatchCalculationGraphForIndex(variableForIndexStrList, firstNode, stmtList, stmtList2, insertPos);
    if(res < 0)
        return res;

    res = translateBatchCalculationGraphForStmt(simdCount, stmtForRet, variableForIndexStrList, firstNode, insertPos);
    if(res < 0)
        return res;

    return 1;
}

int ILBatchCalculationOptimizer::translateBatchCalculationGraphForIndex(
    vector<string>& variableForIndexStrList,
    ILBatchCalculationGraphNode* lastNode, vector<Statement*>* stmtList,
    vector<Statement*>* stmtList2, int& insertPos)
{
    for(int i = 0; i < lastNode->calculateSize.size(); i++)
    {
        string variableForIndexStr = ILParser::getAvailableVariableName("batchIndex" ,lastNode->statement->getDomain());
        if(ILParser::registerVariableName(variableForIndexStr, lastNode->statement->getDomain()) < 0)
            return -ILOptimizer::ErrorInternal;

        StmtLocalVariable* stmtIndexVar = new StmtLocalVariable(lastNode->statement->parentStatement);
        stmtIndexVar->name = variableForIndexStr;
        stmtIndexVar->dataType = "i32";
        stmtList->insert(stmtList->begin() + insertPos, stmtIndexVar);
        if(stmtList != stmtList2)
            stmtList2->push_back(stmtIndexVar);
        insertPos++;

        currentOptimizedStmtForIterVariable = stmtIndexVar;

        variableForIndexStrList.push_back(variableForIndexStr);
    }
    return 1;
}

int ILBatchCalculationOptimizer::translateBatchCalculationGraphForStmt(
    int simdCount, StmtFor** stmtForRet, const vector<string>& variableForIndexStrList,
    ILBatchCalculationGraphNode* lastNode, int& insertPos)
{
    StmtFor* stmtForLast = nullptr;
    for(int i = 0; i < lastNode->calculateSize.size(); i++)
    {
        Statement* parent = !stmtForLast ? lastNode->statement->parentStatement : stmtForLast;

        vector<Statement*>* stmtList = parent->getStatementList();
        vector<Statement*>* stmtList2 = &parent->childStatements;

        StmtFor* stmtFor = new StmtFor(parent);

        StmtExpression* stmtInit = new StmtExpression(stmtFor);
        stmtFor->initial.push_back(stmtInit);
        stmtFor->childStatements.push_back(stmtInit);
        stmtInit->expression = ILExpressionParser::parseExpression(variableForIndexStrList[i] + "=0");
        stmtInit->innerExpressions.push_back(stmtInit->expression);
        string condStr = variableForIndexStrList[i] + "<" + to_string(lastNode->calculateSize[i]);
        stmtFor->condition = ILExpressionParser::parseExpression(condStr);
        if(!stmtFor->condition)
            return -ILOptimizer::ErrorInternal;
        
        stmtFor->innerExpressions.push_back(stmtFor->condition);

        StmtExpression* stmtIter = new StmtExpression(stmtFor);
        stmtFor->iterator.push_back(stmtIter);
        stmtFor->childStatements.push_back(stmtIter);
        int iterCount = i != lastNode->calculateSize.size() - 1 ? 1 : simdCount;
        string iterStr = variableForIndexStrList[i] + "+=" + to_string(iterCount);
        stmtIter->expression = ILExpressionParser::parseExpression(iterStr);
        if(!stmtIter->expression)
            return -ILOptimizer::ErrorInternal;

        stmtIter->innerExpressions.push_back(stmtIter->expression);


        if(stmtForLast) {
            insertPos = 0;
        } else {
            currentOptimizedStmtFor = stmtFor;
        }
        stmtList->insert(stmtList->begin() + insertPos, stmtFor);
        if(stmtList != stmtList2)
            stmtList2->push_back(stmtFor);

        stmtForLast = stmtFor;
    }
    *stmtForRet = stmtForLast;

    return 1;
}

int ILBatchCalculationOptimizer::translateBatchCalculationGraphInputVariable(
    ILBatchCalculationGraph& graph, int simdCount, StmtFor* stmtFor)
{
    int res;
    ILBatchCalculationGraphNode* lastNode = graph.getLastNode();
    if(!lastNode)
        return -ILOptimizer::ErrorInternal;

    vector<Statement*>* stmtList = lastNode->statement->parentStatement->getStatementList();
    vector<Statement*>* stmtList2 = &lastNode->statement->parentStatement->childStatements;

    if(!stmtList || !stmtList2)
        return -ILOptimizer::ErrorInternal;

    int insertPos = 0;
    for(int i = 0; i < stmtList->size(); i++) {
        if((*stmtList)[i] == stmtFor) {
            insertPos = i;
            break;
        }
    }

    vector<ILBatchCalculationGraphNodeInput*> externalInputList;
    for(int i = 0; i < graph.size(); i++) {
        for(int j = 0; !graph.isLayerEmpty(i) && j < graph.at(i).size(); j++) {
            res = translateBatchCalculationGraphInputVariableAddToInputList(graph, i, j, externalInputList);
            if(res <= 0)
                return res;
        }
    }

    res = translateBatchCalculationGraphInputVariableList(graph, simdCount, stmtFor, externalInputList, stmtList, stmtList2, insertPos);
    if(res <= 0)
        return res;

    return 1;
}

int ILBatchCalculationOptimizer::translateBatchCalculationGraphInputVariableAddToInputList(ILBatchCalculationGraph& graph, int i, int j, vector<ILBatchCalculationGraphNodeInput*> &externalInputList)
{
    ILBatchCalculationGraphNode* tempNode = graph.at(i)[j];
    for(int k = 0; k < tempNode->inputs.size(); k++)
    {
        if(graph.isNodeInputSourceInner(&(tempNode->inputs[k])))
        {
            continue;
        }
        bool found = false;
        for(int m = 0; m < externalInputList.size(); m++)
        {
            if(externalInputList[m]->expression == tempNode->inputs[k].expression)
            {
                found = true;
                break;
            }
        }
        if(found)
        {
            continue;
        }

        externalInputList.push_back(&(tempNode->inputs[k]));
    }
    return 1;
}

int ILBatchCalculationOptimizer::translateBatchCalculationGraphInputVariableList(ILBatchCalculationGraph& graph,
                                                                                 int simdCount, StmtFor* stmtFor, const vector<ILBatchCalculationGraphNodeInput*>& externalInputList,
                                                                                 vector<Statement*>* stmtList, vector<Statement*>* stmtList2, int& insertPos)
{
    int res;
    for(int i = 0; i < externalInputList.size(); i++)
    {
        ILBatchCalculationGraphNodeInput* input = externalInputList[i];
        int loadCount = input->arraySize.empty() ? 1 : simdCount;
        if(loadCount != 1 && (res = setVariableAlign(input->expression, stmtFor->getDomain())) < 0) {
            return res;
        }
        StmtLocalVariable* stmtLocalVar = new StmtLocalVariable(stmtFor);
        
        vector<void*> domain;
        if(loadCount != 1) {
            stmtFor->statements.push_back(stmtLocalVar);
            stmtFor->childStatements.push_back(stmtLocalVar);
            domain = stmtFor->getDomain();
        } else {
            stmtLocalVar->parentStatement = stmtFor->parentStatement;
            domain = stmtFor->parentStatement->getDomain();

            stmtList = stmtLocalVar->parentStatement->getStatementList();
            stmtList2 = &stmtLocalVar->parentStatement->childStatements;
            stmtList->insert(stmtList->begin() + insertPos, stmtLocalVar);
            if(stmtList != stmtList2)
                stmtList2->push_back(stmtLocalVar);
            insertPos++;
        }
        string tempVarName = ILParser::getAvailableVariableName(input->expression + "_Batch", domain);
        ILParser::registerVariableName(tempVarName, domain);
        graph.setNodeIONameMap(input->expression, tempVarName);
        stmtLocalVar->name = tempVarName;
        stmtLocalVar->dataType = ILBatchInstructionProvider::getBatchVariableType(input->type, simdCount);
        if(stmtLocalVar->dataType.empty())
            return 0;

        ILBatchInstruction instruction;
        
        int res = ILBatchInstructionProvider::getBatchLoadInstruction(input->type, simdCount, loadCount, instruction);
        if(res <= 0)
            return res;
        string inputDataStr = input->expression;
        inputDataStr += (loadCount == 1) ? "" : "+" + stmtFor->condition->childExpressions[0]->expressionStr;
        string defaultValueStr = instruction.getFormattedDataLoadCode(inputDataStr);
        stmtLocalVar->defaultValue = ILExpressionParser::parseExpression(defaultValueStr);
        if(!stmtLocalVar->defaultValue)
            return 0;
        stmtLocalVar->innerExpressions.push_back(stmtLocalVar->defaultValue);
        
    }
    return 1;
}

int ILBatchCalculationOptimizer::translateBatchCalculationGraphSubOutput(
    ILBatchCalculationGraph& graph,
    int simdCount, StmtFor* stmtFor, std::string format) const
{
    unordered_map<string, bool> generatedVar;
    size_t posStart = format.find("%o_");
    size_t posEnd;
    while(posStart != string::npos) {
        posEnd = posStart + CONST_NUM_3;
        while(format[posEnd] >= '0' && format[posEnd] <= '9' || format[posEnd] == '_') {
            posEnd++;
        }
        string tempVarStr = format.substr(posStart, posEnd - posStart);
        if(generatedVar.find(tempVarStr) != generatedVar.end()) {
            posStart = format.find("%o_", posEnd);
            continue;
        }
        generatedVar[tempVarStr] = true;
        vector<string> tempVarStrSp = stringSplit(tempVarStr, "_");
        int layerId = stringToInt(tempVarStrSp[1]);
        int nodeId = stringToInt(tempVarStrSp[CONST_NUM_2]);
        int varId = stringToInt(tempVarStrSp[CONST_NUM_3]);
        ILBatchCalculationGraphNode* node = graph.at(layerId)[nodeId];
        
        ILBatchCalculationGraphNodeOutput* output = &(node->outputs[varId]);
        string varStr = output->expression;
        string tempVarName = graph.getNodeIONameMap(varStr);
        if(tempVarName.empty())
        {
            vector<void*> domain = stmtFor->getDomain();
            //domain.pop_back();
            tempVarName = ILParser::getAvailableVariableName(output->expression + "_Batch", domain);
            ILParser::registerVariableName(tempVarName, domain);
            graph.setNodeIONameMap(output->expression, tempVarName);
        }
        
        StmtLocalVariable* stmtLocalVar = new StmtLocalVariable(stmtFor);
        
        stmtFor->statements.push_back(stmtLocalVar);
        stmtFor->childStatements.push_back(stmtLocalVar);

        stmtLocalVar->name = tempVarName;
        stmtLocalVar->dataType = ILBatchInstructionProvider::getBatchVariableType(output->type, simdCount);
        if(stmtLocalVar->dataType.empty())
            return 0;

        //string defaultValueStr = ILBatchInstructionProvider::getBatchLoadInstruction(output->type, simdCount);
        //defaultValueStr += "(";
        //defaultValueStr += output->expression + "+" + stmtFor->condition->childExpressions[0]->expressionStr;
        //defaultValueStr += ")";
        //stmtLocalVar->defaultValue = ILExpressionParser::parseExpression(defaultValueStr);
        //stmtLocalVar->innerExpressions.push_back(stmtLocalVar->defaultValue);


        posStart = format.find("%o_", posEnd);
    }
    return 1;
}

int ILBatchCalculationOptimizer::translateBatchCalculationGraphInputConvert(ILBatchCalculationGraph& graph,
    int simdCount, StmtFor* stmtFor, std::string format) const
{
    size_t posStart = format.find("%i_");
    size_t posEnd;
    while(posStart != string::npos) {
        posEnd = posStart + CONST_NUM_3;
        while(format[posEnd] >= '0' && format[posEnd] <= '9' || format[posEnd] == '_') {
            posEnd++;
        }
        string tempVarStr = format.substr(posStart, posEnd - posStart);
        vector<string> tempVarStrSp = stringSplit(tempVarStr, "_");
        int layerId = stringToInt(tempVarStrSp[1]);
        int nodeId = stringToInt(tempVarStrSp[CONST_NUM_2]);
        int varId = stringToInt(tempVarStrSp[CONST_NUM_3]);
        ILBatchCalculationGraphNode* node = graph.at(layerId)[nodeId];
        
        ILBatchCalculationGraphNodeInput* input = &(node->inputs[varId]);

        string typeSimpleSrc = ILParser::getBasicDataTypeSimple(input->type);
        string typeSimpleDst = ILParser::getBasicDataTypeSimple(node->dataType);

        if(typeSimpleSrc == typeSimpleDst)
        {
            posStart = format.find("%i_", posEnd);
            continue;
        }

        string variableName = input->expression;
        
        if(currentNodeTypeConvertMap.find(variableName) != currentNodeTypeConvertMap.end())
        {
            posStart = format.find("%i_", posEnd);
            continue;
        }

        int res = translateBatchCalculationGraphInputConvertOne(graph, simdCount, stmtFor, variableName, typeSimpleSrc, typeSimpleDst);
        if(res <= 0)
            return res;
        
        
        posStart = format.find("%i_", posEnd);
    }
    return 1;
}

int ILBatchCalculationOptimizer::translateBatchCalculationGraphInputConvertOne(const ILBatchCalculationGraph& graph,
    int simdCount, StmtFor* stmtFor, string variableName, string typeSimpleSrc, string typeSimpleDst) const
{
    int res;
    string oriVariableName = graph.getNodeIONameMap(variableName);
    vector<void*> domain = stmtFor->getDomain();
    //domain.pop_back();
    string tempVarName = ILParser::getAvailableVariableName(variableName + "_BatchConvert", domain);
    ILParser::registerVariableName(tempVarName, domain);
    currentNodeTypeConvertMap[variableName] = tempVarName;

    StmtLocalVariable* stmtLocalVar = new StmtLocalVariable(stmtFor);
        
    stmtFor->statements.push_back(stmtLocalVar);
    stmtFor->childStatements.push_back(stmtLocalVar);

    stmtLocalVar->name = tempVarName;
    stmtLocalVar->dataType = ILBatchInstructionProvider::getBatchVariableType(typeSimpleDst, simdCount);
    if(stmtLocalVar->dataType.empty())
        return 0;
    
    ILBatchInstruction instruction;
    if((res = ILBatchInstructionProvider::getBatchDataConvertInstruction(typeSimpleSrc, typeSimpleDst, simdCount, instruction)) < 0)
        return res;

    string expStr = instruction.getFormattedDataTypeConvertCode(oriVariableName, tempVarName);

    int expCount = 0;
    vector<string> expStrSp = stringSplit(expStr, ";");
    for(int k = 0; k < expStrSp.size(); k++)
    {
        expStrSp[k] = stringTrim(expStrSp[k]);
        expCount += expStrSp[k].empty() ? 0 : 1;
    }
    
    for(int k = 0; expCount > 1 && k < expStrSp.size(); k++)
    {
        if(expStrSp[k].empty())
            continue;

        StmtExpression* stmtExp = new StmtExpression(stmtFor);
        stmtFor->statements.push_back(stmtExp);
        stmtFor->childStatements.push_back(stmtExp);
        stmtExp->expression = ILExpressionParser::parseExpression(expStrSp[k]);
        if(!stmtExp->expression)
            return -ILOptimizer::ErrorInternal;
        stmtExp->innerExpressions.push_back(stmtExp->expression);
    }
    if(expCount == 1)
    {
        size_t pos = expStr.find("=") + 1;
        string defaultValueStr = expStr.substr(pos, expStr.length() - pos);
        stmtLocalVar->defaultValue = ILExpressionParser::parseExpression(defaultValueStr);
        if(!stmtLocalVar->defaultValue)
            return -ILOptimizer::ErrorInternal;
        stmtLocalVar->innerExpressions.push_back(stmtLocalVar->defaultValue);
    }
    return 1;
}


int ILBatchCalculationOptimizer::translateBatchCalculationGraphOutputConvertVariable(ILBatchCalculationGraph& graph,
                                                                                     int simdCount, StmtFor* stmtFor, std::string format) const
{
    size_t posStart = format.find("%o_");
    size_t posEnd;
    while(posStart != string::npos) {
        posEnd = posStart + CONST_NUM_3;
        while(format[posEnd] >= '0' && format[posEnd] <= '9' || format[posEnd] == '_') {
            posEnd++;
        }
        string tempVarStr = format.substr(posStart, posEnd - posStart);
        vector<string> tempVarStrSp = stringSplit(tempVarStr, "_");
        int layerId = stringToInt(tempVarStrSp[1]);
        int nodeId = stringToInt(tempVarStrSp[CONST_NUM_2]);
        int varId = stringToInt(tempVarStrSp[CONST_NUM_3]);
        ILBatchCalculationGraphNode* node = graph.at(layerId)[nodeId];
        
        ILBatchCalculationGraphNodeOutput* output = &(node->outputs[varId]);

        string typeSimpleSrc = ILParser::getBasicDataTypeSimple(node->dataType);
        string typeSimpleDst = ILParser::getBasicDataTypeSimple(output->type);

        if(typeSimpleSrc == typeSimpleDst)
        {
            posStart = format.find("%o_", posEnd);
            continue;
        }

        string variableName = output->expression;
        
        if(currentNodeTypeConvertMap.find(variableName) != currentNodeTypeConvertMap.end())
        {
            posStart = format.find("%o_", posEnd);
            continue;
        }

        string oriVariableName = graph.getNodeIONameMap(variableName);
        vector<void*> domain = stmtFor->getDomain();
        //domain.pop_back();
        string tempVarName = ILParser::getAvailableVariableName(variableName + "_BatchConvert", domain);
        ILParser::registerVariableName(tempVarName, domain);
        currentNodeTypeConvertMap[variableName] = tempVarName;
        
        
        StmtLocalVariable* stmtLocalVar = new StmtLocalVariable(stmtFor);
        
        stmtFor->statements.push_back(stmtLocalVar);
        stmtFor->childStatements.push_back(stmtLocalVar);

        stmtLocalVar->name = tempVarName;
        stmtLocalVar->dataType = ILBatchInstructionProvider::getBatchVariableType(typeSimpleSrc, simdCount);
        if(stmtLocalVar->dataType.empty())
            return 0;
        
        
        posStart = format.find("%o_", posEnd);
    }
    return 1;
}

int ILBatchCalculationOptimizer::translateBatchCalculationGraphOutputConvert(ILBatchCalculationGraph& graph,
    int simdCount, StmtFor* stmtFor, std::string format) const
{
    size_t posStart = format.find("%o_");
    size_t posEnd;
    while(posStart != string::npos) {
        posEnd = posStart + CONST_NUM_3;
        while(format[posEnd] >= '0' && format[posEnd] <= '9' || format[posEnd] == '_') {
            posEnd++;
        }
        string tempVarStr = format.substr(posStart, posEnd - posStart);
        vector<string> tempVarStrSp = stringSplit(tempVarStr, "_");
        int layerId = stringToInt(tempVarStrSp[1]);
        int nodeId = stringToInt(tempVarStrSp[CONST_NUM_2]);
        int varId = stringToInt(tempVarStrSp[CONST_NUM_3]);
        ILBatchCalculationGraphNode* node = graph.at(layerId)[nodeId];
        
        ILBatchCalculationGraphNodeOutput* output = &(node->outputs[varId]);

        string typeSimpleSrc = ILParser::getBasicDataTypeSimple(node->dataType);
        string typeSimpleDst = ILParser::getBasicDataTypeSimple(output->type);

        if(typeSimpleSrc == typeSimpleDst)
        {
            posStart = format.find("%o_", posEnd);
            continue;
        }

        string variableName = output->expression;
        
        string dstVariableName = graph.getNodeIONameMap(variableName);
        string tempVarName = currentNodeTypeConvertMap[variableName];
        
        ILBatchInstruction instruction;
        int res = ILBatchInstructionProvider::getBatchDataConvertInstruction(typeSimpleSrc, typeSimpleDst, simdCount, instruction);
        if(res <= 0)
            return res;

        string expStr = instruction.getFormattedDataTypeConvertCode(tempVarName, dstVariableName);
        
        vector<string> expStrSp = stringSplit(expStr, ";");
        
        for(int k = 0; k < expStrSp.size(); k++)
        {
            if(expStrSp[k].empty())
                continue;

            StmtExpression* stmtExp = new StmtExpression(stmtFor);
            stmtFor->statements.push_back(stmtExp);
            stmtFor->childStatements.push_back(stmtExp);
            stmtExp->expression = ILExpressionParser::parseExpression(expStrSp[k]);
            if(!stmtExp->expression)
                return -ILOptimizer::ErrorInternal;
            stmtExp->innerExpressions.push_back(stmtExp->expression);
        }
        
        posStart = format.find("%o_", posEnd);
    }
    return 1;
}

int ILBatchCalculationOptimizer::translateBatchCalculationGraphCalculation(ILBatchCalculationGraph& graph,
                                                                           StmtFor* stmtFor, std::string insertCode) const
{
    vector<string> insertCodeLine = stringSplit(insertCode, ";");
    for(int i = 0; i < insertCodeLine.size(); i++)
    {
        if(insertCodeLine[i].empty())
            continue;

        StmtExpression* tempStmt = new StmtExpression(stmtFor);
        stmtFor->statements.push_back(tempStmt);
        stmtFor->childStatements.push_back(tempStmt);
        tempStmt->expression = ILExpressionParser::parseExpression(insertCodeLine[i]);
        if(!tempStmt->expression)
            return -ILOptimizer::ErrorInternal;
        tempStmt->innerExpressions.push_back(tempStmt->expression);
    }

    return 1;
}

int ILBatchCalculationOptimizer::translateBatchCalculationGraphOutputVariable(ILBatchCalculationGraph& graph,
    unordered_map<ILBatchCalculationGraphNode*, bool>& nodeMask, 
    int simdCount, StmtFor* stmtFor)
{

    std::vector<ILBatchCalculationGraphNode*> exportNodeList;
    for(int i = 0; i < graph.size(); i++) {
        for(int j = 0; !graph.isLayerEmpty(i) && j < graph.at(i).size(); j++) {
            ILBatchCalculationGraphNode* tempNode = graph.at(i)[j];
            if(isGraphNodeOutputNeedExport(graph, tempNode))
            {
                exportNodeList.push_back(tempNode);
            }
        }
    }

    for(int i = 0; i < exportNodeList.size(); i++)
    {
        for(int j = 0; j < exportNodeList[i]->outputs.size(); j++)
        {
            ILBatchCalculationGraphNodeOutput* output = &(exportNodeList[i]->outputs[j]);
            
            setVariableAlign(output->expression, stmtFor->getDomain());

            ILBatchInstruction instruction;
            int res = ILBatchInstructionProvider::getBatchStoreInstruction(output->type, simdCount, instruction);
            if(res <= 0)
                return res;

            string expStr = instruction.getFormattedDataStoreCode(graph.getNodeIONameMap(output->expression), output->expression + "+" + stmtFor->condition->childExpressions[0]->expressionStr);

            vector<string> expStrSp = stringSplit(expStr, ";");
            for(int k = 0; k < expStrSp.size(); k++)
            {
                string expStrLine = stringTrim(expStrSp[k]);
                if(expStrLine.empty())
                    continue;
                StmtExpression* stmtExp = new StmtExpression(stmtFor);
                stmtFor->statements.push_back(stmtExp);
                stmtFor->childStatements.push_back(stmtExp);
                stmtExp->expression = ILExpressionParser::parseExpression(expStrLine);
                if(!stmtExp->expression)
                    return 0;
                stmtExp->innerExpressions.push_back(stmtExp->expression);
            }
            
        }
    }
    return 1;
}

int ILBatchCalculationOptimizer::translateBatchCalculationAddHeadFile() const
{
    vector<string> headFileStrList = ILBatchInstructionProvider::getHeadFileList();

    for(int i = 0; i < headFileStrList.size(); i++)
    {
        ILFile* file = reinterpret_cast<ILFile*>(schedule->parent->parent);
        vector<ILHeadFile*>* headFileList = file->getListOfILHeadFile();
        if(!headFileList)
            return -ILOptimizer::ErrorInternal;
        for(int j = 0; j < headFileList->size(); j++)
        {
            ILHeadFile* headFile = (*headFileList)[j];
            if(headFile->name == headFileStrList[i])
                break;
        }
        ILHeadFile* newHeadFile = new ILHeadFile(file);
        newHeadFile->name = headFileStrList[i];
        headFileList->push_back(newHeadFile);
    }
    return 1;
}

int ILBatchCalculationOptimizer::releaseStmtForAndReturn(int returnValue)
{
    releaseCurrentOptimizedObject();
    return returnValue;
}

int ILBatchCalculationOptimizer::tryToAddGraphNode(ILBatchCalculationGraph& graph,
                                                   ILBatchCalculationGraphNode* node)
{
    int maxPredecessorLayer = -1;
    std::vector<ILBatchCalculationGraphNode*> predecessor;

    for(int i = 0; i < graph.size(); i++) {
        for(int j = 0; !graph.isLayerEmpty(i) && j < graph.at(i).size(); j++) {
            ILBatchCalculationGraphNode* tempNode = graph.at(i)[j];
            for(int k = 0; k < node->inputs.size(); k++) {
                const ILBatchCalculationGraphNodeInput* input = &(node->inputs[k]);
                for(int m = 0; m < tempNode->outputs.size(); m++) {
                    const ILBatchCalculationGraphNodeOutput* output = &(tempNode->outputs[m]);
                    if(input->expression == output->expression) {
                        predecessor.push_back(tempNode);
                        maxPredecessorLayer = i > maxPredecessorLayer ? i : maxPredecessorLayer;
                    }
                }
            }
        }
    }

    // 判断是否能构成凸计算图
    if(!tryToAddGraphNodeIsNoDependent(graph, node)) {
        return 0;
    }

    // 判断图中其它节点的语句是否能够迁移至当前节点的语句位置
    if(!tryToAddGraphNodeCanGraphMove(graph, node)) {
        return 0;
    }
    

    graph[maxPredecessorLayer + 1].push_back(node);
    for(int i = 0; i < predecessor.size(); i++) {
        ILBatchCalculationGraphNode* tempNode = predecessor[i];
        tempNode->successor.push_back(node);
        node->predecessor.push_back(tempNode);
    }

    return 1;
}

bool ILBatchCalculationOptimizer::tryToAddGraphNodeIsNoDependent(const ILBatchCalculationGraph& graph,
    const ILBatchCalculationGraphNode* node) const
{
    // 首先找出所有非批量计算的直接前驱
    vector<ILAnalyzeScheduleNode*> allDirectPredecessorScheduleNode =
        ops.getVariableSourceNodesDirect(node->scheduleNode);
    for(int i = 0; i < graph.size(); i++) {
        for(int j = 0; !graph.isLayerEmpty(i) && j < graph.at(i).size(); j++) {
            ILBatchCalculationGraphNode* tempNode = graph.at(i)[j];
            auto iter = find(allDirectPredecessorScheduleNode.begin(), allDirectPredecessorScheduleNode.end(), tempNode->scheduleNode);
            if(iter != allDirectPredecessorScheduleNode.end()) {
                allDirectPredecessorScheduleNode.erase(iter);
            }
        }
    }
    // 然后统计所有非批量计算前驱的所有前驱
    vector<ILAnalyzeScheduleNode*> allPredecessorScheduleNode;
    for(int i = 0; i < allDirectPredecessorScheduleNode.size(); i++) {
        ILAnalyzeScheduleNode* tempNode = allDirectPredecessorScheduleNode[i];
        vector<ILAnalyzeScheduleNode*> tempSourceNodes = ops.getVariableSourceNodesAll(tempNode);
        for(int j = 0; j < tempSourceNodes.size(); j++) {
            if(find(allPredecessorScheduleNode.begin(), allPredecessorScheduleNode.end(), tempSourceNodes[j]) == allPredecessorScheduleNode.end()) {
                allPredecessorScheduleNode.push_back(tempSourceNodes[j]);
            }
        }
    }
    // 最后判断计算图中是否存在这些前驱节点，存在则无法将该节点加入计算图
    for(int k = 0; k < allPredecessorScheduleNode.size(); k++) {
        if(graph.isContainScheduleNode(allPredecessorScheduleNode[k])) {
            return false;
        }
    }
    return true;
}

bool ILBatchCalculationOptimizer::tryToAddGraphNodeCanGraphMove(const ILBatchCalculationGraph& graph,
    const ILBatchCalculationGraphNode* nodeDst)
{
    for(int i = 0; i < graph.size(); i++) {
        for(int j = 0; !graph.isLayerEmpty(i) && j < graph.at(i).size(); j++) {
            ILBatchCalculationGraphNode* tempNode = graph.at(i)[j];
            
            if(!tryToAddGraphNodeCanNodeMove(graph, tempNode, nodeDst))
                return false;
        }
    }
    return true;
}

bool ILBatchCalculationOptimizer::tryToAddGraphNodeCanNodeMove(const ILBatchCalculationGraph& graph, const ILBatchCalculationGraphNode* nodeSrc,
    const ILBatchCalculationGraphNode* nodeDst)
{

    const vector<ILAnalyzeScheduleNode*>* scheduleNodeList = ops.getScheduleNodeList();
    ILAnalyzeScheduleNode* scheduleNodeSrc = nodeSrc->scheduleNode;
    ILAnalyzeScheduleNode* scheduleNodeDst = nodeDst->scheduleNode;

    vector<ILAnalyzeScheduleVariable*> allReadVariable;
    vector<ILAnalyzeScheduleVariable*> allStoreVariable;
    for(int k = scheduleNodeSrc->id + 1; k < scheduleNodeDst->id; k++)
    {
        ILAnalyzeScheduleNode* scheduleNodeTemp = (*scheduleNodeList)[k];
        if(graph.isContainScheduleNode(scheduleNodeTemp))
            continue;

        for(int i = 0; i < scheduleNodeTemp->variableRead.size(); i++)
        {
            allReadVariable.push_back(const_cast<ILAnalyzeScheduleVariable*>(scheduleNodeTemp->variableRead[i]->variable));
        }
        for(int i = 0; i < scheduleNodeTemp->variableStored.size(); i++)
        {
            allStoreVariable.push_back(const_cast<ILAnalyzeScheduleVariable*>(scheduleNodeTemp->variableStored[i]->variable));
        }
    }
    
    for(int i = 0; i < scheduleNodeSrc->variableRead.size(); i++) {
        if(find(allStoreVariable.begin(), allStoreVariable.end(), scheduleNodeSrc->variableRead[i]->variable) != allStoreVariable.end())
            return false;
    }
    
    for(int i = 0; i < scheduleNodeSrc->variableStored.size(); i++) {
        if(find(allReadVariable.begin(), allReadVariable.end(), scheduleNodeSrc->variableStored[i]->variable) != allReadVariable.end())
            return false;
    }

    return true;
}

ILBatchCalculationGraphNode* ILBatchCalculationOptimizer::createBatchCalculationGraphNodeByScheduleNode(
    const ILAnalyzeScheduleNode* node) const
{
    StmtBatchCalculation* stmt = static_cast<StmtBatchCalculation*>(node->statement);
    ILBatchCalculationGraphNode* ret = new ILBatchCalculationGraphNode();
    ret->operationType = stmt->operationType;
    ret->dataType = stmt->dataType;
    ret->modifierType = stmt->modifierType;
    ret->shapeType = stmt->shapeType;
    ret->scheduleNode = const_cast<ILAnalyzeScheduleNode*>(node);
    ret->statement = stmt;
    
    for(int i = 0; i < stmt->inputs.size(); i++)
    {
        if(!stmt->inputs[i].defaultValue)
        {
            ret->inputs.push_back(ILBatchCalculationGraphNodeInput(stmt->inputs[i].name, stmt->inputs[i].type, stmt->inputs[i].arraySize));
            if(stmt->inputs[i].arraySize.size() > ret->calculateSize.size())
            {
                ret->calculateSize = stmt->inputs[i].arraySize;
            }
        }
        else
        {
            ret->parameters.push_back(ILBatchCalculationGraphNodeParameter(stmt->inputs[i].defaultValue->expressionStr, stmt->inputs[i].type));
        }
    }
    for(int i = 0; i < stmt->outputs.size(); i++)
    {
        ret->outputs.push_back(ILBatchCalculationGraphNodeOutput(stmt->outputs[i].name, stmt->outputs[i].type, stmt->inputs[i].arraySize));
    }
    return ret;
}

bool ILBatchCalculationOptimizer::isGraphNodeOutputNeedExport(const ILBatchCalculationGraph& graph, const ILBatchCalculationGraphNode* node) const
{
    if(node->operationType == StmtBatchCalculation::TypeAssign ||
       node->operationType == StmtBatchCalculation::TypeReadData)
        return false;
    ILBatchCalculationGraphNode* lastGraphNode = graph.getLastNode();
    if(!lastGraphNode)
        return true;
    ILAnalyzeScheduleNode* scheduleNodeLast = lastGraphNode->scheduleNode;
    if(!scheduleNodeLast)
        return true;

    //将批量运算之后被使用到的输出变量进行SIMD写出
    const vector<ILAnalyzeScheduleNode*>* scheduleNodeList = ops.getScheduleNodeList();
    for(int i = 0; i < node->outputs.size(); i++)
    {
        string outputVarStr = node->outputs[i].expression;

        for(int j = scheduleNodeLast->id + 1; j < scheduleNodeList->size(); j++)
        {
            ILAnalyzeScheduleNode* tempNode = (*scheduleNodeList)[j];
            if(tempNode->isStoreVariable(outputVarStr))
                return true;
            if(tempNode->isReadVariable(outputVarStr))
                return true;
        }
    }
    return false;
}

bool ILBatchCalculationOptimizer::isInstructionMissGraphOutput(const ILBatchCalculationGraph& graph,
    const ILBatchCalculationGraph& subGraph, const ILBatchInstruction& instruction) const
{
    vector<string> outputVarFormatList;
    for(int i = 0; i < subGraph.size(); i++) {
        for(int j = 0; !subGraph.isLayerEmpty(i) && j < subGraph.at(i).size(); j++) {
            ILBatchCalculationGraphNode* tempNode = subGraph.at(i)[j];
            if(!isGraphNodeOutputNeedExport(graph, tempNode))
            {
                continue;
            }
            for(int k = 0; k < tempNode->outputs.size(); k++)
            {
                string outputVarFormat = "%o";
                outputVarFormat += "_" + to_string(i);
                outputVarFormat += "_" + to_string(j);
                outputVarFormat += "_" + to_string(k);
                outputVarFormatList.push_back(outputVarFormat);
            }
        }
    }
    for(int i = 0; i < outputVarFormatList.size(); i++)
    {
        if(instruction.format.find(outputVarFormatList[i]) == string::npos)
        {
            return true;
        }
    }
    return false;
}

bool ILBatchCalculationOptimizer::isInstructionNeedOtherOutputButNodeGen(
    const ILBatchCalculationGraph& graph,
    const ILBatchInstruction& instruction) const
{
    string format = instruction.format;
    size_t posStart = format.find("%o_");
    size_t posEnd;
    while(posStart != string::npos) {
        posEnd = posStart + CONST_NUM_3;
        while(format[posEnd] >= '0' && format[posEnd] <= '9' || format[posEnd] == '_') {
            posEnd++;
        }
        string tempVarStr = format.substr(posStart, posEnd - posStart);
        vector<string> tempVarStrSp = stringSplit(tempVarStr, "_");
        int layerId = stringToInt(tempVarStrSp[1]);
        int nodeId = stringToInt(tempVarStrSp[CONST_NUM_2]);
        int varId = stringToInt(tempVarStrSp[CONST_NUM_3]);
        ILBatchCalculationGraphNode* node = graph.at(layerId)[nodeId];
        
        string variableName  = node->inputs[varId].expression;

        string variableNameBatch = graph.getNodeIONameMap(variableName);
        if(variableNameBatch.empty())
            return true;
        
        posStart = format.find("%o_", posEnd);
    }
    return false;
}

int ILBatchCalculationOptimizer::setVariableAlign(std::string variableName, std::vector<void*> domain)
{
    // 考虑到很多场景一旦使用对齐数据，而中途取得对齐数据的中间部分的指针时候，对齐基本就失效了，除非按16对齐取指针
    // 比如selector组件，比如其它处理数组数据的外部函数
    // 所以暂时取消掉对Intel平台下的数组对齐处理，SIMD数据加载和写出指令都采用非对齐的函数实现
    return 1;
    if(!stringStartsWith(ILOptimizer::configTargetDevice, "Intel"))
        return 1;
    ILAnalyzeScheduleVariable* var = ops.getVariableInDomain(variableName, domain);
    if(!var)
        return -ILOptimizer::ErrorInternal;
    if(var->variableType == ILAnalyzeScheduleVariable::TypeLocalVariable) {
        if(var->iLLocalVariable->align != CONST_NUM_16) {
            currentOptimizedAlignedLocalVariableList[var->iLLocalVariable] = var->iLLocalVariable->align;
            var->iLLocalVariable->align = CONST_NUM_16;
        }

    }
    else if(var->variableType == ILAnalyzeScheduleVariable::TypeGlobalVariable) {
        if(var->iLGlobalVariable->align != CONST_NUM_16) {
            currentOptimizedAlignedGlobalVariableList[var->iLGlobalVariable] = var->iLGlobalVariable->align;
            var->iLGlobalVariable->align = CONST_NUM_16;
        }
    }
    else if(var->variableType == ILAnalyzeScheduleVariable::TypeStatementVariable) {
        StmtLocalVariable* stmt = static_cast<StmtLocalVariable*>(var->statement);
        if(stmt->align != CONST_NUM_16) {
            currentOptimizedAlignedStatementVariableList[stmt] = stmt->align;
            stmt->align = CONST_NUM_16;
        }
    }
    return 1;
}
