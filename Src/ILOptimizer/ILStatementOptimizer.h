#pragma once

class Expression;
class ILParser;
class Statement;

class ILStatementOptimizer
{
public:
	int optimize(Statement* statement, ILParser* parser);
	int optimizeInner(Statement* statement, ILParser* parser);
	Statement* statement = nullptr;
    
private:
	ILParser* iLParser = nullptr;

	int traverseOptimizeStatement(Statement* statement) const;
	int optimizeExpression(Expression* expression) const;
	int optimizeStatement(Statement* statement) const;

    bool isStatementContainAssign(const Statement* statement) const;

	int release() const;
};

