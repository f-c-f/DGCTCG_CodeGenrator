#include "StmtReturn.h"
#include "Expression.h"

StmtReturn::StmtReturn()
{
    this->type = Statement::Return;
}

StmtReturn::StmtReturn(const Statement* parent)
{
    this->type = Statement::Return;
    this->parentStatement = const_cast<Statement*>(parent);
}

StmtReturn::~StmtReturn()
{
}

Statement* StmtReturn::clone(Statement* parent)
{
	StmtReturn* ret = new StmtReturn;
	ret->parentStatement = parent;
	ret->type = this->type;
	ret->depth = parent->depth + 1;
	for (int i = 0; i < this->childStatements.size(); i++)
	{
		Statement* child = this->childStatements[i]->clone(ret);
		ret->childStatements.push_back(child);
	}

	if(this->expression)
	{
	    ret->expression = this->expression->clone();
		ret->innerExpressions.push_back(ret->expression);
	}

	return ret;
}

