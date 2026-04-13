#pragma once

class ILParser;
class Expression;

class ILExpressionOptimizer
{
public:
	int optimize(Expression* expression, ILParser* parser);
	Expression* expression = nullptr;

private:
	ILParser* iLParser = nullptr;

	int release() const;
};

