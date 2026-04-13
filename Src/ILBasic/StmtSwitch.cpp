#include "StmtSwitch.h"
#include "Expression.h"

StmtSwitch::StmtSwitch()
{
    this->type = Statement::Switch;
}

StmtSwitch::StmtSwitch(const Statement* parent)
{
    this->type = Statement::Switch;
    this->parentStatement = const_cast<Statement*>(parent);
}

StmtSwitch::~StmtSwitch()
{
}

Statement* StmtSwitch::clone(Statement* parent)
{
	StmtSwitch* ret = new StmtSwitch;
	ret->parentStatement = parent;
	ret->type = this->type;
	ret->depth = parent->depth + 1;

	if(this->condition)
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
