#include "StmtBreak.h"
#include "Expression.h"

StmtBreak::StmtBreak()
{
    this->type = Statement::Break;
}

StmtBreak::StmtBreak(const Statement* parent)
{
    this->type = Statement::Break;
    this->parentStatement = const_cast<Statement*>(parent);
}

StmtBreak::~StmtBreak()
{
}


Statement* StmtBreak::clone(Statement* parent)
{
	StmtBreak* ret = new StmtBreak;
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
