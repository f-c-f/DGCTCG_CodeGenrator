#pragma once
#include "Statement.h"
class StmtExpression:
	public Statement
{
public:
    StmtExpression();
    explicit StmtExpression(const Statement* parent);
    ~StmtExpression() override;

	Expression* expression = nullptr;

    virtual Statement* clone(Statement* parent);

    void setExpression(std::string expStr);
};
