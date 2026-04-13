#pragma once
#include <set>
#include <string>

class Expression;
class Statement;
class ILParser;
class ILCalculate;
class ILCalculateOptimizer
{
public:
	int optimize(ILCalculate* calculate, ILParser* parser);
	ILCalculate* calculate = nullptr;

private:
	ILParser* iLParser = nullptr;

	std::set<std::string> variableList;
	int colletExpressionVariable();
	int traverseCollectStatement(Statement* statement);
	//用来遍历表达式中的变量将它们加入到variableList中
	int traverseCollectExpression(Expression* expression);

	int optimizeInputsAndOutputs();

	int release() const;
};
