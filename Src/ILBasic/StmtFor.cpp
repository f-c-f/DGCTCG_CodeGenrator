#include "StmtFor.h"
#include "Expression.h"

StmtFor::StmtFor()
{
    this->type = Statement::For;
}

StmtFor::StmtFor(const Statement* parent)
{
    this->type = Statement::For;
    this->parentStatement = const_cast<Statement*>(parent);
}

StmtFor::~StmtFor()
{
}


Statement* StmtFor::clone(Statement* parent)
{
	StmtFor* ret = new StmtFor;
	ret->parentStatement = parent;
	ret->type = this->type;
	ret->depth = parent->depth + 1;

	if (this->condition)
	{
	    ret->condition = this->condition->clone();
		ret->innerExpressions.push_back(ret->condition);
	}
	for (int i = 0; i < this->initial.size(); i++)
	{
		Statement* child = this->initial[i]->clone(ret);
		ret->initial.push_back(child);
		ret->childStatements.push_back(child);
	}
	for (int i = 0; i < this->iterator.size(); i++)
	{
		Statement* child = this->iterator[i]->clone(ret);
		ret->iterator.push_back(child);
		ret->childStatements.push_back(child);
	}
	for (int i = 0; i < this->statements.size(); i++)
	{
		Statement* child = this->statements[i]->clone(ret);
		ret->statements.push_back(child);
		ret->childStatements.push_back(child);
	}

	return ret;
}
