#include "StmtDefault.h"
#include "Expression.h"

StmtDefault::StmtDefault()
{
    this->type = Statement::Default;
}

StmtDefault::StmtDefault(const Statement* parent)
{
    this->type = Statement::Default;
    this->parentStatement = const_cast<Statement*>(parent);
}

StmtDefault::~StmtDefault()
{
}


Statement* StmtDefault::clone(Statement* parent)
{
	StmtDefault* ret = new StmtDefault;
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

	return ret;
}
