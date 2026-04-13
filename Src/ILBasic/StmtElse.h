#pragma once
#include "Statement.h"
class StmtIf;
class StmtElse:
	public Statement
{
public:
    StmtElse();
    explicit StmtElse(const Statement* parent);
    ~StmtElse() override;

	std::vector<Statement*> statements;

    StmtIf* getFirstIfStatement();

    virtual Statement* clone(Statement* parent);
};
