#include "StmtAssemble.h"
#include "Expression.h"

StmtAssemble::StmtAssemble()
{
    this->type = Statement::Assemble;
}

StmtAssemble::StmtAssemble(const Statement* parent)
{
    this->type = Statement::Assemble;
    this->parentStatement = const_cast<Statement*>(parent);
}

StmtAssemble::~StmtAssemble()
{
}

Statement* StmtAssemble::clone(Statement* parent)
{
	StmtAssemble* ret = new StmtAssemble;
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

	ret->inputs = this->inputs;
	ret->outputs = this->outputs;
	ret->expressions = this->expressions;
	
	return ret;
}


