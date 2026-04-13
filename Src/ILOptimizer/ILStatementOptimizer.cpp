#include "ILStatementOptimizer.h"
#include "ILExpressionOptimizer.h"
#include "ILOptimizer.h"


#include "../ILBasic/ILParser.h"
#include "../ILBasic/Expression.h"


#include "../ILBasic/Statement.h"
#include "../ILBasic/StmtExpression.h"
#include "../ILBasic/StmtElse.h"
#include "../ILBasic/StmtDoWhile.h"
#include "../ILBasic/StmtFor.h"

using namespace std;

//comment = true;
//unordered_map<Statement*, bool> ILStatementOptimizer::isStatementOptimized;
int ILStatementOptimizer::optimize(Statement* statement, ILParser* parser)
{
	//if(isStatementOptimized.find(statement) != isStatementOptimized.end())
	//	return 0;
	//isStatementOptimized[statement] = true;
	this->statement = statement;
	this->iLParser = parser;
	int res = traverseOptimizeStatement(statement);
	if(res < 0)
		return res;

    res = optimizeInner(statement, parser);
	if(res < 0)
		return res;

    return 1;
}

int ILStatementOptimizer::optimizeInner(Statement* statement, ILParser* parser)
{
	if(statement->type == Statement::If || statement->type == Statement::ElseIf) {
		Statement* nextStmt = statement->getNextStatement();
		if(!isStatementContainAssign(statement) && statement->getStatementList()->empty() &&
			(!nextStmt || nextStmt->type != Statement::Else && nextStmt->type != Statement::ElseIf)) {
			iLParser->releaseStatement(statement);
		}
	} else if(statement->type == Statement::While ||  statement->type == Statement::DoWhile ||
        statement->type == Statement::For || statement->type == Statement::Switch) {
		if(statement->getStatementList()->empty() && !isStatementContainAssign(statement)) {
			iLParser->releaseStatement(statement);
		}
	} else if(statement->type == Statement::Continue || statement->type == Statement::Return) {
		Statement* stmtNext = statement->getNextStatement();
		while(stmtNext) {
			iLParser->releaseStatement(stmtNext);
			stmtNext = statement->getNextStatement();
		}
	} else if(statement->type == Statement::Break) {
		Statement* stmtNext = statement->getNextStatement();
		while(stmtNext) {
            if(stmtNext->type == Statement::Case || stmtNext->type == Statement::Default)
                break;
			iLParser->releaseStatement(stmtNext);
			stmtNext = statement->getNextStatement();
		}
	} else if(statement->type == Statement::Case || statement->type == Statement::Default ||
            statement->type == Statement::LocalVariable || statement->type == Statement::BatchCalculation) {
	} else if(statement->type == Statement::ExpressionWithSemicolon) {
		StmtExpression* stmtTemp = static_cast<StmtExpression*>(statement);
		if(!stmtTemp->expression || stmtTemp->expression->expressionStr.empty()) {
			iLParser->releaseStatement(statement);
		}
	} else if(statement->type == Statement::Else) {
        Statement* previous = statement->getPreviousStatement();
        vector<Statement*>* stmtList = statement->getStatementList();
        if(stmtList && stmtList->empty()) {
            iLParser->releaseStatement(statement);
        }
    }
    else
	    return -ILOptimizer::ErrorInvalidStatementType;
    return 1;
}

//void ILStatementOptimizer::clear()
//{
//	isStatementOptimized.clear();
//}

int ILStatementOptimizer::traverseOptimizeStatement(Statement* statement) const
{
	int res = 0;
    Expression* exp = statement->getValueExpression();
    vector<Statement*>* stmtList = statement->getStatementList();
    if (statement->type == Statement::If || statement->type == Statement::Else ||
        statement->type == Statement::ElseIf || statement->type == Statement::While ||
        statement->type == Statement::Switch || statement->type == Statement::Case ||
        statement->type == Statement::Return || statement->type == Statement::LocalVariable ||
        statement->type == Statement::ExpressionWithSemicolon || statement->type == Statement::Continue || 
        statement->type == Statement::Default || statement->type == Statement::Break ||
        statement->type == Statement::BatchCalculation){
    } else if (statement->type == Statement::DoWhile){
		StmtDoWhile* stmtTemp = static_cast<StmtDoWhile*>(statement);
		for(int i = stmtTemp->statements.size() - 1; res >= 0 && i >= 0; i--){
			res = optimizeStatement(stmtTemp->statements[i]);
		}
		if(res < 0 || (res = optimizeExpression(stmtTemp->condition)) < 0)
			return res;

		return 1;
	} else if (statement->type == Statement::For) {
		StmtFor* stmtTemp = static_cast<StmtFor*>(statement);
		for(int i = stmtTemp->initial.size() - 1; res >= 0 && i >= 0; i--) {
			res = optimizeStatement(stmtTemp->initial[i]);
		}
        res = optimizeExpression(stmtTemp->condition);
		for(int i = stmtTemp->statements.size() - 1; res >= 0 && i >= 0; i--) {
			res = optimizeStatement(stmtTemp->statements[i]);
		}
		for(int i = stmtTemp->iterator.size() - 1; res >= 0 && i >= 0; i--) {
			res = optimizeStatement(stmtTemp->iterator[i]);
		}
        if(res < 0)
			return res;

		return 1;
	}
	else {
        return -ILOptimizer::ErrorInvalidStatementType;
	}
    if(exp && (res = optimizeExpression(exp)) < 0)
		return res;

    if (stmtList) {
        for(int i = stmtList->size() - 1; res >=0 && i >= 0; i--){
		    res = optimizeStatement((*stmtList)[i]);
	    }
        if(res < 0)
		    return res;

    }
	return 1;
}

int ILStatementOptimizer::optimizeExpression(Expression* expression) const
{
	if(!expression)
		return 0;
	ILExpressionOptimizer iLExpressionOptimizer;
	int res = iLExpressionOptimizer.optimize(expression, iLParser);
	if(res < 0)
		return res;
	return 1;
}

int ILStatementOptimizer::optimizeStatement(Statement* statement) const
{
	if(!statement)
		return 0;
	ILStatementOptimizer iLStatementOptimizer;
	int res = iLStatementOptimizer.optimize(statement, iLParser);
	if(res < 0)
		return res;
	return 1;
}

bool ILStatementOptimizer::isStatementContainAssign(const Statement* statement) const
{
    if(statement->type == Statement::If || statement->type == Statement::ElseIf ||
        statement->type == Statement::While || statement->type == Statement::ExpressionWithSemicolon ||
        statement->type == Statement::Switch) {
		return statement->getValueExpression()->isContainAssign();
	} else if(statement->type == Statement::Return) {
        if(statement->getValueExpression())
            return statement->getValueExpression()->isContainAssign();
		return false;
	} else if(statement->type == Statement::Break || statement->type == Statement::Continue ||
        statement->type == Statement::Case || statement->type == Statement::Default ||
        statement->type == Statement::Else) {
		return false;
	} else if(statement->type == Statement::LocalVariable || statement->type == Statement::DoWhile ||
        statement->type == Statement::For) {
		return true;
    }
    return true;
}

int ILStatementOptimizer::release() const
{
	return 1;
}
