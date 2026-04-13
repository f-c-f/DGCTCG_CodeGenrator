#include "StmtIf.h"
#include "Expression.h"

StmtIf::StmtIf()
{
    this->type = Statement::If;
}

StmtIf::StmtIf(const Statement* parent)
{
    this->type = Statement::If;
    this->parentStatement = const_cast<Statement*>(parent);
}

StmtIf::~StmtIf()
{
}


Statement* StmtIf::clone(Statement* parent)
{
	StmtIf* ret = new StmtIf;
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
