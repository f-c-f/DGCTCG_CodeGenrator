#include "StmtDoWhile.h"
#include "Expression.h"

StmtDoWhile::StmtDoWhile()
{
    this->type = Statement::DoWhile;
}

StmtDoWhile::StmtDoWhile(const Statement* parent)
{
    this->type = Statement::DoWhile;
    this->parentStatement = const_cast<Statement*>(parent);
}

StmtDoWhile::~StmtDoWhile()
{
}


Statement* StmtDoWhile::clone(Statement* parent)
{
	StmtDoWhile* ret = new StmtDoWhile;
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
