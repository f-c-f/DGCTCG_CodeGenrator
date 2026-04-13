#pragma once
#include "Statement.h"
class StmtLocalVariable:
	public Statement
{
public:
    StmtLocalVariable();
    explicit StmtLocalVariable(const Statement* parent);
    ~StmtLocalVariable() override;

	std::string name;
	std::string dataType;
	int isAddress = 0;
    int align = 0;
    bool isVolatile = false;
    bool isConst = false;
	std::vector<int> arraySize;
	Expression* defaultValue = nullptr;

    virtual Statement* clone(Statement* parent);

    void set(std::string name, std::string dataType, Expression* defaultValue);
    void set(std::string name, std::string dataType, int isAddress, std::vector<int> arraySize, Expression* defaultValue);
    void set(std::string name, std::string dataType, int isAddress, std::vector<int> arraySize, int align, bool isVolatile, bool isConst, Expression* defaultValue);
};
