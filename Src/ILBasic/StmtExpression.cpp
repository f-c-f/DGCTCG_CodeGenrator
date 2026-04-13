#include "StmtExpression.h"
#include "Expression.h"
#include "ILExpressionParser.h"

StmtExpression::StmtExpression()
{
    this->type = Statement::ExpressionWithSemicolon;
}

StmtExpression::StmtExpression(const Statement* parent)
{
    this->type = Statement::ExpressionWithSemicolon;
    this->parentStatement = const_cast<Statement*>(parent);
}

StmtExpression::~StmtExpression()
{
}


Statement* StmtExpression::clone(Statement* parent)
{
	StmtExpression* ret = new StmtExpression;
	ret->parentStatement = parent;
	ret->type = this->type;
	ret->depth = parent->depth + 1;
	for (int i = 0; i < this->childStatements.size(); i++)
	{
		Statement* child = this->childStatements[i]->clone(ret);
		ret->childStatements.push_back(child);
	}

	if (this->expression)
	{
	    ret->expression = this->expression->clone();
		ret->innerExpressions.push_back(ret->expression);
	}

	return ret;
}

void StmtExpression::setExpression(std::string expStr)
{
    if(this->expression) {
        delete this->expression;
        this->expression = nullptr;
        this->innerExpressions.clear();
    }
    
    this->expression = ILExpressionParser::parseExpression(expStr);
    this->innerExpressions.push_back(this->expression);
}
