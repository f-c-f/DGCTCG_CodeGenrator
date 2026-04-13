#pragma once
#include <string>
#include <vector>

#include "ILFunction.h"


class ILObject;
class Expression;

class Statement
{
public:


	enum Type{
	    If, Else, ElseIf, While, 
		DoWhile, For, Continue, Switch, Case, 
		Default, Break, Return, BraceStatement, 
		LocalVariable, ExpressionWithSemicolon,
        BatchCalculation, Assemble,
        ExpressionList,
        Unknown,
	};

    
    Statement();
    Statement(Type type, const Statement* parent);
    Statement(Type type, int depth, const Statement* parent);
    explicit Statement(Type type);
    explicit Statement(const Statement* parent);
    virtual ~Statement();

    static std::vector<std::string> statementTypeName;
	Type type = Unknown;
    std::vector<Statement*> childStatements; //各个子类都有的子语句列表
	std::vector<Expression*> innerExpressions;
	int depth = -1;
	Statement* parentStatement = nullptr;

    ILObject* parentILObject = nullptr;


	Statement* getNextStatement() const;

	Statement* getPreviousStatement() const;

    //获得派生类Statement的表达式，比如if的条件表达式
    Expression* getValueExpression() const;
    
    std::vector<Statement*>* getStatementList() const;
    Statement* getFirstChildStatement() const;
    void addChildStatement(Statement* stmt);

	void print() const;
	virtual void release();


    std::vector<void*> getDomain() const;
    ILFunction* getContainerILFunction() const;


    virtual Statement* clone(Statement* parent);

    void insertStatementBefore(Statement* stmt, Statement* before);
    void insertStatementAfter(Statement* stmt, Statement* after);
    void insertStatementAtBegin(Statement* stmt);

    void removeChildStatement(Statement* stmt);
    void removeChildStatementWithoutDelete(Statement* stmt);
};
