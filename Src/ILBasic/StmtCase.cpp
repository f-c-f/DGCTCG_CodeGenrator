#include "StmtCase.h"
#include "Expression.h"

StmtCase::StmtCase()
{
    this->type = Statement::Case;
}

StmtCase::StmtCase(const Statement* parent)
{
    this->type = Statement::Case;
    this->parentStatement = const_cast<Statement*>(parent);
}

StmtCase::~StmtCase()
{
}


Statement* StmtCase::clone(Statement* parent)
{
	StmtCase* ret = new StmtCase;
	ret->parentStatement = parent;
	ret->type = this->type;
	ret->depth = parent->depth + 1;
	for (int i = 0; i < this->childStatements.size(); i++)
	{
		Statement* child = this->childStatements[i]->clone(ret);
		ret->childStatements.push_back(child);
	}
	for (int i = 0; i < this->innerExpressions.size(); i++)
	{
		Expression* exp = this->innerExpressions[i]->clone();
		ret->innerExpressions.push_back(exp);
	}

	if (this->value)
		ret->value = this->value->clone();

	return ret;
}
