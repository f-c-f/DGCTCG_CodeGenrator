#include "StmtContinue.h"
#include "Expression.h"

StmtContinue::StmtContinue()
{
    this->type = Statement::Continue;
}

StmtContinue::StmtContinue(const Statement* parent)
{
    this->type = Statement::Continue;
    this->parentStatement = const_cast<Statement*>(parent);
}

StmtContinue::~StmtContinue()
{
}

Statement* StmtContinue::clone(Statement* parent)
{
	StmtContinue* ret = new StmtContinue;
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
