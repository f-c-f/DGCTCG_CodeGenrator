#include "StmtElse.h"
#include "StmtIf.h"
#include "Expression.h"

StmtElse::StmtElse()
{
    this->type = Statement::Else;
}

StmtElse::StmtElse(const Statement* parent)
{
    this->type = Statement::Else;
    this->parentStatement = const_cast<Statement*>(parent);
}

StmtElse::~StmtElse()
{
}

StmtIf* StmtElse::getFirstIfStatement()
{
    Statement* stmt = this->getPreviousStatement();
    while (stmt->type != Statement::If)
    {
        stmt = stmt->getPreviousStatement();
    }
    return (StmtIf*)stmt;
}

Statement* StmtElse::clone(Statement* parent)
{
	StmtElse* ret = new StmtElse;
	ret->parentStatement = parent;
	ret->type = this->type;
	ret->depth = parent->depth + 1;
	for (int i = 0; i < this->innerExpressions.size(); i++)
	{
		Expression* exp = this->innerExpressions[i]->clone();
		ret->innerExpressions.push_back(exp);
	}
	
	for (int i = 0; i < this->statements.size(); i++)
	{
		Statement* child = this->statements[i]->clone(ret);
		ret->statements.push_back(child);
		ret->childStatements.push_back(child);
	}

	return ret;
}
