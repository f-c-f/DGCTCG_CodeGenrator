#include "StmtElseIf.h"
#include "StmtIf.h"
#include "Expression.h"

StmtElseIf::StmtElseIf()
{
    this->type = Statement::ElseIf;
}

StmtElseIf::StmtElseIf(const Statement* parent)
{
    this->type = Statement::ElseIf;
    this->parentStatement = const_cast<Statement*>(parent);
}

StmtElseIf::~StmtElseIf()
{
}


StmtIf* StmtElseIf::getFirstIfStatement()
{
    Statement* stmt = this->getPreviousStatement();
    while (stmt->type != Statement::If)
    {
        stmt = stmt->getPreviousStatement();
    }
    return (StmtIf*)stmt;
}

Statement* StmtElseIf::clone(Statement* parent)
{
	StmtElseIf* ret = new StmtElseIf;
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
