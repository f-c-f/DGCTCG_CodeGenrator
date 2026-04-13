#include "StmtWhile.h"
#include "Expression.h"

StmtWhile::StmtWhile()
{
    this->type = Statement::While;
}

StmtWhile::StmtWhile(const Statement* parent)
{
    this->type = Statement::While;
    this->parentStatement = const_cast<Statement*>(parent);
}

StmtWhile::~StmtWhile()
{
}

Statement* StmtWhile::clone(Statement* parent)
{
	StmtWhile* ret = new StmtWhile;
	ret->parentStatement = parent;
	ret->type = this->type;
	ret->depth = parent->depth + 1;

	if (this->condition)
	{
	    ret->condition = this->condition->clone();
		ret->innerExpressions.push_back(ret->condition);
	}
	for (int i = 0; i < this->statements.size(); i++)
	{
		Statement* child = this->statements[i]->clone(ret);
		ret->statements.push_back(child);
		ret->childStatements.push_back(child);
	}

	return ret;
}
