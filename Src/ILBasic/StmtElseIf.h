#pragma once
#include "Statement.h"
class StmtIf;
class StmtElseIf:
	public Statement
{
public:
    StmtElseIf();
    explicit StmtElseIf(const Statement* parent);
    ~StmtElseIf() override;

	Expression* condition = nullptr;
	std::vector<Statement*> statements;

    StmtIf* getFirstIfStatement();

    virtual Statement* clone(Statement* parent);
};
