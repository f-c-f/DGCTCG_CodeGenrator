#pragma once
#include <string>

class Expression;

class ILTransExpression
{
public:
    int translate(const Expression* expression, std::string* statementStr) const;
};
