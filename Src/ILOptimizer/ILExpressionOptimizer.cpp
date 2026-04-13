#include "ILExpressionOptimizer.h"
using namespace std;

int ILExpressionOptimizer::optimize(Expression* expression, ILParser* parser)
{
    this->iLParser = parser;
	this->expression = expression;


	return 1;
}

int ILExpressionOptimizer::release() const
{
	return 1;
}
