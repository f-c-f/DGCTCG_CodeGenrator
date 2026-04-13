#include "StmtLocalVariable.h"
#include "Expression.h"

StmtLocalVariable::StmtLocalVariable()
{
    this->type = Statement::LocalVariable;
}

StmtLocalVariable::StmtLocalVariable(const Statement* parent)
{
    this->type = Statement::LocalVariable;
    this->parentStatement = const_cast<Statement*>(parent);
}

StmtLocalVariable::~StmtLocalVariable()
{
}


Statement* StmtLocalVariable::clone(Statement* parent)
{
	StmtLocalVariable* ret = new StmtLocalVariable;
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

	ret->name = this->name;
	ret->dataType = this->dataType;
	ret->isAddress = this->isAddress;
	ret->align = this->align;
	ret->isVolatile = this->isVolatile;
	ret->isConst = this->isConst;
	ret->arraySize = this->arraySize;
	if(this->defaultValue)
	    ret->defaultValue = this->defaultValue->clone();

	return ret;
}

void StmtLocalVariable::set(std::string name, std::string dataType, Expression* defaultValue)
{
    this->name = name;
    this->dataType = dataType;
    this->defaultValue = defaultValue;
}

void StmtLocalVariable::set(std::string name, std::string dataType, int isAddress, std::vector<int> arraySize, Expression* defaultValue)
{
    this->name = name;
    this->dataType = dataType;
    this->isAddress = isAddress;
    this->arraySize = arraySize;
    this->defaultValue = defaultValue;
}

void StmtLocalVariable::set(std::string name, std::string dataType, int isAddress, std::vector<int> arraySize, int align, bool isVolatile, bool isConst, Expression* defaultValue)
{
    this->name = name;
    this->dataType = dataType;
    this->isAddress = isAddress;
    this->arraySize = arraySize;
    this->align = align;
    this->isVolatile = isVolatile;
    this->isConst = isConst;
    this->defaultValue = defaultValue;
}
