#include "ILScheduleOptimizer.h"

#include "ILBranchOptimizer.h"
#include "ILCalculateOptimizer.h"
#include "ILOptimizer.h"
#include "../ILBasic/ILAnalyzeScheduleNode.h"
#include "../ILBasic/ILAnalyzeScheduleVariable.h"
#include "../ILBasic/Expression.h"
#include "../ILBasic/ILSchedule.h"
#include "../ILBasic/ILLocalVariable.h"
#include "../ILBasic/ILInput.h"
#include "../ILBasic/ILOutput.h"
#include "../ILBasic/ILCalculate.h"
#include "../ILBasic/ILBranch.h"
#include "../ILBasic/ILMember.h"
#include "../ILBasic/StmtLocalVariable.h"
#include "../ILBasic/Statement.h"
#include "../ILBasic/ILParser.h"
#include "../ILBasic/ILRef.h"

#include "../ILBasic/BasicTypeCalculator.h"

using namespace std;


int ILScheduleOptimizer::optimize(ILSchedule* schedule, ILParser* parser)
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
    

    res = optimizeSimplyILNode();
    if(res < 0)
        return res;

    
	return 1;
}



int ILScheduleOptimizer::optimize()
{
    int optimizedCount = 0;

    bool printProcess = false;

    if(printProcess)
        ops.print();

    int res = 1;
    
    while(res > 0)
    {
        optimizedCount++;
        if(printProcess)
        {
            cout << "\033[33m" << "Optimized Count : " << optimizedCount << "\033[0m" << endl;
            ops.print();
        }

        if((res = optimizeOnceFold()) < 0)
            return res;
        if(!ILOptimizer::configExpressionSimplification)
            continue;
        if(res == 0 && (res = optimizeOnceRedundancy()) < 0)
            return res;
        if(res == 0 && (res = optimizeOnceOnlyRead()) < 0)
            return res;
    }
    
	return 1;
}

int ILScheduleOptimizer::optimizeOnceFold()
{
    //ILOptimizeScheduleNode* node = optimizeFindSingleAssignedNodeAndVariable(optimizeCurrentFindIndex);
    ILAnalyzeScheduleNodeVariableRef* variableStore = nullptr;
    ILAnalyzeScheduleNodeVariableRef* variableRead = nullptr;

    int findIndex = 0;

    while(true)
    {
        // 先寻找被单次赋值的变量
        int res = ops.optimizeFindSingleAssignedReadVariable(findIndex, variableStore, variableRead);
        if(res == 0 || !variableStore || !variableRead)
            return 0;

        if(variableStore->variable->callName != variableRead->variable->callName ||
            variableStore->expression != variableStore->expressionFull ||
            variableRead->expression != variableRead->expressionFull) {
            findIndex++;
            continue;
        } else if(variableRead->node->nodeType == ILAnalyzeScheduleNode::TypeStatement &&
            variableRead->node->statement->type == Statement::For) {
            findIndex++;
            continue;
        } else if(ops.optimizeAreVariableSourceBeAssigned(variableStore->node->id, variableRead->node->id, variableStore)) {
            // 判断在找到的当前节点 到 变量单次读取节点之间 ，被赋值变量的数据源的所有变量是否被赋过值
            findIndex++;
            continue;
        } else if(!ops.isDomainContain(variableRead->node->domain, variableStore->node->domain)) {
            // 最后判断读变量的作用域是否包含或等于写变量的作用域
            // 如果写变量的作用域在if等语句块里面，而读变量在if等语句块下(外)面，则不能优化
            findIndex++;
            continue;
        } else if(ops.optimizeAreNodeVariablesConflictInDomain(variableStore->node, variableRead->node->domain)) {
            // 保证所有折叠来的变量在新的作用域里没有冲突变量(重名)
            findIndex++;
            continue;
        } else if(ops.optimizeIsVariableRefAddress(variableStore)) {
            // 忽略地址变量、数组变量的优化
            findIndex++;
            continue;
        } else if(ops.optimizeIsScheduleNodeContainFunction(variableStore->node)) {
            // 写节点包含函数调用则不优化，否则影响美观
            findIndex++;
            continue;
        } else if(variableStore->node->nodeType == ILAnalyzeScheduleNode::TypeStatement &&
            variableStore->node->statement->type == Statement::BatchCalculation) {
            //BatchCalculation不参与折叠
            findIndex++;
            continue;
        } else if(variableRead->node->nodeType == ILAnalyzeScheduleNode::TypeStatement &&
            variableRead->node->statement->type == Statement::BatchCalculation) {
            //BatchCalculation不参与折叠
            findIndex++;
            continue;
        }
        break;
    }

    int res = optimizeOnceFoldVariable(variableStore, variableRead);
    if(res < 0)
        return res;
    //findIndex++;

    return 1;
}

int ILScheduleOptimizer::optimizeOnceFoldVariable(
    ILAnalyzeScheduleNodeVariableRef* variableStore,
    ILAnalyzeScheduleNodeVariableRef* variableRead)
{
    if(variableStore->node->nodeType == ILAnalyzeScheduleNode::TypeLocalVariable) {
        return optimizeOnceFoldLocalVariable(variableStore, variableRead);
    } else if(variableStore->node->nodeType == ILAnalyzeScheduleNode::TypeStatement &&
        variableStore->node->statement->type == Statement::LocalVariable) {
        return optimizeOnceFoldStatementLocalVariable(variableStore, variableRead);
    } else if(variableStore->node->nodeType == ILAnalyzeScheduleNode::TypeStatement &&
        variableStore->node->statement->type == Statement::ExpressionWithSemicolon) {
        return optimizeOnceFoldExpressionAssignedVariable(variableStore, variableRead);
    } else {
        return -ILOptimizer::ErrorInternal;
    }
}

int ILScheduleOptimizer::optimizeOnceFoldLocalVariable(
    ILAnalyzeScheduleNodeVariableRef* variableStore,
    ILAnalyzeScheduleNodeVariableRef* variableRead)
{
    if(!ILOptimizer::configVariableFolding)
        return 0;

    ILAnalyzeScheduleNode* nodeStore = const_cast<ILAnalyzeScheduleNode*>(variableStore->node);
    ILAnalyzeScheduleNode* nodeRead = const_cast<ILAnalyzeScheduleNode*>(variableRead->node);
    ILLocalVariable* iLLocalVariableStore = variableStore->node->iLLocalVariable;
    Expression* expressionRead = const_cast<Expression*>(variableRead->expressionFull);

    Expression* typeConvertExp = ops.addTypeConvertToExpression(iLLocalVariableStore->defaultValue, iLLocalVariableStore->type, nodeRead->domain);
    typeConvertExp->parentExpression = expressionRead->parentExpression;

    int res = optimizeOnceFoldMergeExpression(typeConvertExp, expressionRead);
    if(res < 0)
        return res;
    
    typeConvertExp->release();
    delete typeConvertExp;

    res = ops.recollectScheduleNodeExpressionVariable(nodeRead);
    if(res < 0)
        return res;


    res = optimizeOnceFoldMergeRef(nodeStore, nodeRead);
    if(res < 0)
        return res;

    res = optimizeOnceFoldMergeInput(nodeStore, nodeRead);
    if(res < 0)
        return res;

    res = optimizeOnceFoldMergeOutput(nodeStore, nodeRead);
    if(res < 0)
        return res;

    //iLLocalVariableStore->defaultValue = nullptr;
    iLParser->releaseILObject(iLLocalVariableStore);

    //res = optimizeOnceFoldSimplyILNode(nodeStore, nodeRead);
    //if(res < 0)
    //    return res;
    
    //res = optimizeOnceFoldMergeNodeVariable(variableStore, variableRead);
    //if(res < 0)
    //    return res;

    res = ops.releaseScheduleOptimizeNodeAndVariable(variableStore);
    if(res < 0)
        return res;

    return 1;
}

int ILScheduleOptimizer::optimizeOnceFoldStatementLocalVariable(
    ILAnalyzeScheduleNodeVariableRef* variableStore,
    ILAnalyzeScheduleNodeVariableRef* variableRead)
{
    ILAnalyzeScheduleNode* nodeStore = const_cast<ILAnalyzeScheduleNode*>(variableStore->node);
    ILAnalyzeScheduleNode* nodeRead = const_cast<ILAnalyzeScheduleNode*>(variableRead->node);
    StmtLocalVariable* stmtLocalVariable = static_cast<StmtLocalVariable*>(variableStore->node->statement);
    Expression* expressionRead = const_cast<Expression*>(variableRead->expressionFull);

    Expression* typeConvertExp = ops.addTypeConvertToExpression(stmtLocalVariable->defaultValue, stmtLocalVariable->dataType, nodeRead->domain);
    typeConvertExp->parentExpression = expressionRead->parentExpression;
    int res = optimizeOnceFoldMergeExpression(typeConvertExp, expressionRead);
    if(res < 0)
        return res;
    
    typeConvertExp->release();
    delete typeConvertExp;
    
    res = ops.recollectScheduleNodeExpressionVariable(nodeRead);
    if(res < 0)
        return res;

    res = optimizeOnceFoldMergeRef(nodeStore, nodeRead);
    if(res < 0)
        return res;

    res = optimizeOnceFoldMergeInput(nodeStore, nodeRead);
    if(res < 0)
        return res;

    res = optimizeOnceFoldMergeOutput(nodeStore, nodeRead);
    if(res < 0)
        return res;

    //stmtLocalVariable->defaultValue = nullptr;
    //stmtLocalVariable->innerExpressions.clear();
    iLParser->releaseStatement(stmtLocalVariable);

    //res = optimizeOnceFoldSimplyILNode(nodeStore, nodeRead);
    //if(res < 0)
    //    return res;
    
    //res = optimizeOnceFoldMergeNodeVariable(variableStore, variableRead);
    //if(res < 0)
    //    return res;
    
    res = ops.releaseScheduleOptimizeNodeAndVariable(variableStore);
    if(res < 0)
        return res;

    return 1;
}

int ILScheduleOptimizer::optimizeOnceFoldExpressionAssignedVariable(
    ILAnalyzeScheduleNodeVariableRef* variableStore,
    ILAnalyzeScheduleNodeVariableRef* variableRead)
{
    ILAnalyzeScheduleNode* nodeStore = const_cast<ILAnalyzeScheduleNode*>(variableStore->node);
    ILAnalyzeScheduleNode* nodeRead = const_cast<ILAnalyzeScheduleNode*>(variableRead->node);
    Statement* stmtAssign = variableStore->node->statement;
    Expression* expressionRead = const_cast<Expression*>(variableRead->expressionFull);

    Expression* lastAssignExp = nullptr;
    Expression* p = variableStore->expressionFull->parentExpression;
    while(p) {
        if(p->type == Token::Assign) {
            lastAssignExp = p;
        }
        p = p->parentExpression;
    }
    if(!lastAssignExp)
        return -ILOptimizer::ErrorInternal;

    Expression* expressionStoreSrc = lastAssignExp->childExpressions[1];
    Expression* readExpParent = expressionRead->parentExpression;
    bool isAddressExp = false;
    if(readExpParent && (readExpParent->type == Token::Add || readExpParent->type == Token::AddAssign))
    {
        string readExpType = ops.getExpressionBasicType(readExpParent, nodeRead->domain, isAddressExp);
    }
    string targetType;
    if(!isAddressExp)
    {
        ILAnalyzeScheduleVariableType assignedVariableType = ops.getVariableExpressionType(variableStore->expressionFull, variableStore->node->domain);
        targetType = assignedVariableType.type;
    }
    else
    {
        targetType = "i32";
    }
    Expression* typeConvertExp = ops.addTypeConvertToExpression(expressionStoreSrc, targetType, nodeStore->domain);
    typeConvertExp->parentExpression = readExpParent;
    
    int res = optimizeOnceFoldMergeExpression(typeConvertExp, expressionRead);
    if(res < 0)
        return res;

    typeConvertExp->release();
    delete typeConvertExp;
    
    res = ops.recollectScheduleNodeExpressionVariable(nodeRead);
    if(res < 0)
        return res;

    res = optimizeOnceFoldMergeRef(nodeStore, nodeRead);
    if(res < 0)
        return res;

    res = optimizeOnceFoldMergeInput(nodeStore, nodeRead);
    if(res < 0)
        return res;

    res = optimizeOnceFoldMergeOutput(nodeStore, nodeRead);
    if(res < 0)
        return res;
    
    iLParser->releaseStatement(stmtAssign);
    
    //res = optimizeOnceFoldSimplyILNode(nodeStore, nodeRead);
    //if(res < 0)
    //    return res;

    //res = optimizeOnceFoldMergeNodeVariable(variableStore, variableRead);
    //if(res < 0)
    //    return res;
    
    res = ops.releaseScheduleOptimizeNodeAndVariable(variableStore);
    if(res < 0)
        return res;

    return 1;
}

int ILScheduleOptimizer::optimizeOnceFoldMergeExpression(const Expression* src, Expression* dst) const
{
    ////为了避免在语句释放时把用于替换的表达式释放掉，就要在原表达式的子表达式列表中去掉用于替换的表达式节点
	//if(src->parentExpression){
    //    vector<Expression*>* childExpList = &(src->parentExpression->childExpressions);
    //    auto iter = std::find(childExpList->begin(), childExpList->end(), src);
	//	src->parentExpression->childExpressions.erase(iter);
	//}

    dst->replace(src);
	
    return 1;
}

int ILScheduleOptimizer::optimizeOnceFoldMergeRef(ILAnalyzeScheduleNode* src, ILAnalyzeScheduleNode* dst) const
{
    vector<ILRef*>* refListSrc = nullptr;
    vector<ILRef*>* refListDst = nullptr;
    ILObject* dstParent = nullptr;

    if(src->nodeType == ILAnalyzeScheduleNode::TypeLocalVariable) {
        refListSrc = &(src->iLLocalVariable->refs);
    } else if(src->nodeType == ILAnalyzeScheduleNode::TypeStatement) {
        refListSrc = &(src->iLCalculate->refs);
    }

    if(dst->nodeType == ILAnalyzeScheduleNode::TypeLocalVariable) {
        refListDst = &(dst->iLLocalVariable->refs);
        dstParent = dst->iLLocalVariable;
    } else if(dst->nodeType == ILAnalyzeScheduleNode::TypeStatement) {
        refListDst = &(dst->iLCalculate->refs);
        dstParent = dst->iLCalculate;
    } else if(dst->nodeType == ILAnalyzeScheduleNode::TypeBranch) {
        refListDst = &(dst->iLBranch->refs);
        dstParent = dst->iLBranch;
    }

    if(!refListSrc || !refListDst || refListSrc == refListDst)
        return 0;

    for(int i = 0; i < refListSrc->size(); i++)
    {
        refListDst->push_back(((*refListSrc)[i])->clone(dstParent));
    }

    return 1;
}

int ILScheduleOptimizer::optimizeOnceFoldMergeInput(ILAnalyzeScheduleNode* src, ILAnalyzeScheduleNode* dst) const
{
    vector<ILInput*>* inputListSrc = nullptr;
    vector<ILInput*>* inputListDst = nullptr;
    ILObject* dstParent = nullptr;

    if(src->nodeType == ILAnalyzeScheduleNode::TypeStatement) {
        inputListSrc = &(src->iLCalculate->inputs);
    }

    if(dst->nodeType == ILAnalyzeScheduleNode::TypeStatement) {
        inputListDst = &(dst->iLCalculate->inputs);
        dstParent = dst->iLCalculate;
    }

    if(!inputListSrc || !inputListDst || inputListSrc == inputListDst)
        return 0;
    
    for(int i = 0; i < inputListSrc->size(); i++)
    {
        inputListDst->push_back(((*inputListSrc)[i])->clone(dstParent));
    }

    return 1;
}

int ILScheduleOptimizer::optimizeOnceFoldMergeOutput(ILAnalyzeScheduleNode* src, ILAnalyzeScheduleNode* dst) const
{
    vector<ILOutput*>* outputListSrc = nullptr;
    vector<ILOutput*>* outputListDst = nullptr;
    ILObject* dstParent = nullptr;

    if(src->nodeType == ILAnalyzeScheduleNode::TypeStatement) {
        outputListSrc = &(src->iLCalculate->outputs);
    }

    if(dst->nodeType == ILAnalyzeScheduleNode::TypeStatement) {
        outputListDst = &(dst->iLCalculate->outputs);
        dstParent = dst->iLCalculate;
    }

    if(!outputListSrc || !outputListDst || outputListSrc == outputListDst)
        return 0;
    
    for(int i = 0; i < outputListSrc->size(); i++)
    {
        outputListDst->push_back(((*outputListSrc)[i])->clone(dstParent));
    }
    return 1;
}

/*int ILScheduleOptimizer::optimizeOnceFoldSimplyILNode(ILOptimizeScheduleNode* src, ILOptimizeScheduleNode* dst)
{
    int res;
    if(src->nodeType == ILOptimizeScheduleNode::TypeStatement) {
        ILCalculateOptimizer iLCalculateOptimizer;
        res = iLCalculateOptimizer.optimize(src->iLCalculate, iLParser);
        if(res < 0)
            return res;
        src->iLCalculate->isOptimized = true;
    }

    if(dst->nodeType == ILOptimizeScheduleNode::TypeStatement) {
        ILCalculateOptimizer iLCalculateOptimizer;
        res = iLCalculateOptimizer.optimize(dst->iLCalculate, iLParser);
        if(res < 0)
            return res;
        dst->iLCalculate->isOptimized = true;
    } else if(dst->nodeType == ILOptimizeScheduleNode::TypeBranch) {
        ILBranchOptimizer iLBranchOptimizer;
        res = iLBranchOptimizer.optimize(dst->iLBranch, iLParser);
        if(res < 0)
            return res;
    }

    return 0;
}*/

/*int ILScheduleOptimizer::optimizeOnceFoldMergeNodeVariable(ILOptimizeScheduleNodeVariableRef* variableStore,
    ILOptimizeScheduleNodeVariableRef* variableRead)
{
    ILOptimizeScheduleNode* nodeStore = variableStore->node;
    ILOptimizeScheduleNode* nodeRead = variableRead->node;

    for(int i = 0; i < nodeStore->variableStored.size(); i++)
    {
        if(nodeStore->variableStored[i] == variableStore)
            continue;
        ILOptimizeScheduleNodeVariableRef* varRef = nodeStore->variableStored[i]->clone();
        varRef->node = nodeRead;
        nodeRead->variableStored.push_back(varRef);

    }

    for(int i = 0; i < nodeStore->variableRead.size(); i++)
    {
        ILOptimizeScheduleNodeVariableRef* varRef = nodeStore->variableRead[i]->clone();
        varRef->node = nodeRead;
        nodeRead->variableRead.push_back(varRef);
    }

    return 1;
}*/

int ILScheduleOptimizer::optimizeOnceRedundancy()
{
    ILAnalyzeScheduleNodeVariableRef* variableStore = nullptr;
    int findIndex = 0;
    while(true)
    {
        // 先寻找被单次赋值并之后未读写的变量
        int res = ops.optimizeFindSingleAssignedNoReadStoredVariable(findIndex, variableStore);
        if(res == 0 || !variableStore)
            return 0;
        
        // 然后判断该语句是否存在其它值的写入操作，没有才可以被优化掉
        if (variableStore->node->variableStored.size() == 1)
             break;
        findIndex++;
    }

    int res = optimizeOnceRedundancyVariable(variableStore);
    if(res < 0)
        return res;

    return 1;
}

int ILScheduleOptimizer::optimizeOnceRedundancyVariable(
    ILAnalyzeScheduleNodeVariableRef* variableStore)
{
    if(variableStore->node->nodeType == ILAnalyzeScheduleNode::TypeLocalVariable) {
        return optimizeOnceRedundancyLocalVariable(variableStore);
    } else if(variableStore->node->nodeType == ILAnalyzeScheduleNode::TypeStatement &&
        variableStore->node->statement->type == Statement::LocalVariable) {
        return optimizeOnceRedundancyStatementLocalVariable(variableStore);
    } else if(variableStore->node->nodeType == ILAnalyzeScheduleNode::TypeStatement &&
        variableStore->node->statement->type == Statement::ExpressionWithSemicolon) {
        return optimizeOnceRedundancyExpressionAssignedVariable(variableStore);
    } else if(variableStore->node->nodeType == ILAnalyzeScheduleNode::TypeStatement &&
        variableStore->node->statement->type == Statement::BatchCalculation) {
        return optimizeOnceRedundancyExpressionAssignedVariable(variableStore);
    } else {
        return -ILOptimizer::ErrorInternal;
    }
}

int ILScheduleOptimizer::optimizeOnceRedundancyLocalVariable(
    ILAnalyzeScheduleNodeVariableRef* variableStore)
{
    ILLocalVariable* iLLocalVariableStore = variableStore->node->iLLocalVariable;
    
    iLLocalVariableStore->defaultValue = nullptr;
    iLParser->releaseILObject(iLLocalVariableStore);
    
    int res = ops.releaseScheduleOptimizeNodeAndVariable(variableStore);
    if(res < 0)
        return res;

    return 1;
}

int ILScheduleOptimizer::optimizeOnceRedundancyStatementLocalVariable(
    ILAnalyzeScheduleNodeVariableRef* variableStore)
{
    StmtLocalVariable* stmtLocalVariable = static_cast<StmtLocalVariable*>(variableStore->node->statement);
    
    stmtLocalVariable->defaultValue = nullptr;
    stmtLocalVariable->innerExpressions.clear();
    iLParser->releaseStatement(stmtLocalVariable);
    
    int res = ops.releaseScheduleOptimizeNodeAndVariable(variableStore);
    if(res < 0)
        return res;

    return 1;
}

int ILScheduleOptimizer::optimizeOnceRedundancyExpressionAssignedVariable(
    ILAnalyzeScheduleNodeVariableRef* variableStore)
{
    Statement* stmtAssign = variableStore->node->statement;

    iLParser->releaseStatement(stmtAssign);
    
    int res = ops.releaseScheduleOptimizeNodeAndVariable(variableStore);
    if(res < 0)
        return res;

    return 1;
}

int ILScheduleOptimizer::optimizeOnceRedundancyStatementBatchCalculation(
    ILAnalyzeScheduleNodeVariableRef* variableStore)
{
    Statement* stmtAssign = variableStore->node->statement;

    iLParser->releaseStatement(stmtAssign);
    
    int res = ops.releaseScheduleOptimizeNodeAndVariable(variableStore);
    if(res < 0)
        return res;

    return 1;
}

int ILScheduleOptimizer::optimizeOnceOnlyRead()
{
    ILAnalyzeScheduleNode* node = nullptr;
    //寻找只读的语句，这里包括单纯的读语句，也包括空的可被优化的分支语句
    int res = optimizeOnceOnlyReadFindNode(node);
    if(res <= 0)
        return res;
    
    res = optimizeOnceOnlyReadRemove(node);
    if(res < 0)
        return res;

    return 1;
}

int ILScheduleOptimizer::optimizeOnceOnlyReadFindNode(ILAnalyzeScheduleNode*& retNode)
{
    const vector<ILAnalyzeScheduleNode*>* scheduleNodes = ops.getScheduleNodeList();
    for(int i = scheduleNodes->size() - 1; i >= 0; i--) {
        ILAnalyzeScheduleNode* node = (*scheduleNodes)[i];
        if(!node->variableStored.empty())
            continue;

        if(optimizeOnceOnlyReadFindNodeOne(node))
        {
            retNode = node;
            return 1;
        }
    }
    retNode = nullptr;
    return 0;
}

bool ILScheduleOptimizer::optimizeOnceOnlyReadFindNodeOne(ILAnalyzeScheduleNode* node) const
{
    if (node->nodeType == ILAnalyzeScheduleNode::TypeBranch)
    {
        ILBranch* branch = node->iLBranch;
        if ((branch->type == ILBranch::TypeElse || branch->type == ILBranch::TypeFor || branch->type == ILBranch::TypeWhile) &&
            branch->isInnerEmpty()) {
            return true;
        }
        else if ((branch->type == ILBranch::TypeElseIf || branch->type == ILBranch::TypeIf) &&
            branch->isInnerEmpty()) {
            ILObject* next = branch->getNextSameILObject();
            if (!next || next->objType != ILObject::TypeBranch ||
                (static_cast<ILBranch*>(next)->type != ILBranch::TypeElseIf &&
                    static_cast<ILBranch*>(next)->type != ILBranch::TypeElse)) {
                return true;
            }
        }
    }
    else if (node->nodeType == ILAnalyzeScheduleNode::TypeStatement &&
        (node->statement->type >= Statement::If && node->statement->type <= Statement::While ||
            node->statement->type == Statement::Switch)) {
        Statement* stmt = node->statement;
        if ((stmt->type == Statement::While || stmt->type == Statement::Switch) &&
            stmt->childStatements.empty()) {
            return true;
        }
        else if ((stmt->type == Statement::ElseIf || stmt->type == Statement::If) &&
            stmt->childStatements.empty()) {
            Statement* next = stmt->getNextStatement();
            if (!next || (next->type != Statement::ElseIf && next->type != Statement::Else)) {
                return true;
            }
        }
        else if(stmt->type == Statement::Else && stmt->childStatements.empty()) {
            Statement* previous = stmt->getPreviousStatement();
            if(!previous && previous->type == Statement::If) {
                return true;
            }
        }
    }
    else if (node->nodeType == ILAnalyzeScheduleNode::TypeStatement &&
        node->statement->type == Statement::ExpressionWithSemicolon) {
        Statement* stmt = node->statement;
        if (stmt->getValueExpression() && !stmt->getValueExpression()->isContainAssign()) {
            return true;
        }
    }
    else if(node->variableRead.empty() &&
        node->nodeType == ILAnalyzeScheduleNode::TypeStatement &&
        node->statement->type == Statement::BatchCalculation) {
        return true;
    }
    return false;
}

int ILScheduleOptimizer::optimizeOnceOnlyReadRemove(ILAnalyzeScheduleNode* node)
{
    ILBranch* branch = nullptr;
    Statement* stmt = nullptr;

    if(node->nodeType == ILAnalyzeScheduleNode::TypeBranch) {
        branch = node->iLBranch;
    } else if(node->nodeType == ILAnalyzeScheduleNode::TypeStatement) {
        stmt = node->statement;
    }

    int res = ops.releaseScheduleOptimizeNode(node);
    if(res < 0)
        return res;

    if(node->nodeType == ILAnalyzeScheduleNode::TypeBranch) {
        iLParser->releaseILObject(branch);
    } else if(node->nodeType == ILAnalyzeScheduleNode::TypeStatement) {
        iLParser->releaseStatement(stmt);
    }

    return 1;
}

int ILScheduleOptimizer::optimizeSimplyILNode() const
{
    int res;
    for(int i = schedule->scheduleNodes.size() - 1; i >= 0; i--)
    {
        if(schedule->scheduleNodes[i]->objType == ILObject::TypeBranch)
        {
            res = optimizeSimplyILNodeRefList(&static_cast<ILBranch*>(schedule->scheduleNodes[i])->refs);
            if(res < 0)
                return res;
            ILBranchOptimizer iLBranchOptimizer;
            res = iLBranchOptimizer.optimize(static_cast<ILBranch*>(schedule->scheduleNodes[i]), iLParser);
            if(res < 0)
                return res;
        }
        else if(schedule->scheduleNodes[i]->objType == ILObject::TypeCalculate)
        {
            res = optimizeSimplyILNodeRefList(&static_cast<ILCalculate*>(schedule->scheduleNodes[i])->refs);
            if(res < 0)
                return res;
            ILCalculateOptimizer iLCalculateOptimizer;
            res = iLCalculateOptimizer.optimize(static_cast<ILCalculate*>(schedule->scheduleNodes[i]), iLParser);
            if(res < 0)
                return res;
        }
    }
    return 1;
}

int ILScheduleOptimizer::optimizeSimplyILNodeRefList(std::vector<ILRef*>* refList) const
{
    for(int j = refList->size() - 1; j >= 0; j--)
    {
        for(int k = j - 1; k >= 0; k--)
        {
            if((*refList)[j]->actor == (*refList)[k]->actor && (*refList)[j]->path == (*refList)[k]->path)
            {
                iLParser->releaseILObject((*refList)[j]);
                break;
            }
        }
    }
    return 1;
}

/*int ILScheduleOptimizer::releaseScheduleOptimizeNodeAndVariable(
    ILOptimizeScheduleNodeVariableRef* variableStore,
    ILOptimizeScheduleNodeVariableRef* variableRead)
{
    ILOptimizeScheduleNode* nodeStore = variableStore->node;
    ILOptimizeScheduleNode* nodeRead = variableRead->node;
    ILOptimizeScheduleVariable* variable = variableStore->variable;

    //// 在读节点的读变量列表中移除该变量
    //for (int i = 0; i < nodeRead->variableRead.size(); i++)
    //{
    //    if (nodeRead->variableRead[i]->variable == variable)
    //    {
    //        delete nodeRead->variableRead[i];
    //        nodeRead->variableRead.erase(nodeRead->variableRead.begin() + i);
    //        break;
    //    }
    //}

    // 释放变量
    if(nodeStore->nodeType == ILOptimizeScheduleNode::TypeLocalVariable ||
        (nodeStore->nodeType == ILOptimizeScheduleNode::TypeStatement &&
        nodeStore->statement->type == Statement::LocalVariable)) {
        string varName = variable->name;
        std::vector<ILOptimizeScheduleVariable*>* variableList = &(scheduleVariables[varName]);
        auto iterVariableList = find(variableList->begin(), variableList->end(), variable);
        if(iterVariableList == variableList->end())
            return -ILOptimizer::ErrorInternal;
        variableList->erase(iterVariableList);
        variable->release();
        delete variable;
        variable = nullptr;

        if(variableList->empty())
        {
            auto iterScheduleVariables = scheduleVariables.find(varName);
            scheduleVariables.erase(iterScheduleVariables);
        }
    }


    // 释放调度节点，并重新计算id
    int offset = 0;
    for(int i = 0; i < scheduleNodes.size() + offset; i++)
    {
        if(scheduleNodes[i - offset] == nodeStore)
        {
            scheduleNodes[i - offset]->release();
            delete scheduleNodes[i - offset];
            scheduleNodes.erase(scheduleNodes.begin() + i - offset);

            offset = 1;
        }
        else if(offset)
        {
            scheduleNodes[i - offset]->id = i - offset;
        }
    }
    return 1;
}*/
