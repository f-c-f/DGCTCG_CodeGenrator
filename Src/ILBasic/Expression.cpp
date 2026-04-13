#include "Expression.h"
#include "ILExpressionParser.h"
#include "ILCCodeParser.h"
using namespace std;

namespace
{
    const int CONST_NUM_2  = 2;
}

Expression::Expression(): isExpression(false), type(), parentExpression(nullptr)
{
}

Expression::Expression(bool isExpression, Token::Type type, std::string tokenStr, const Expression* parent)
    : isExpression(isExpression), type(type), tokenStr(tokenStr), parentExpression(const_cast<Expression*>(parent))
{
}

Expression::~Expression()
{
}

void Expression::replace(const Expression* srcExpression)
{
    //优先级比父表达式的操作符低，就需要加括号
	if (srcExpression->childExpressions.size() > 0 &&
        this->parentExpression &&
        ILCCodeParser::getOperatorPrecedence(srcExpression->type) >
        ILCCodeParser::getOperatorPrecedence(this->parentExpression->type) &&
        this->parentExpression->type != Token::Bracket &&
        this->parentExpression->type != Token::Subscript)
	{
		this->type = Token::Bracket;
		this->tokenStr = "(";
		this->expressionStr = "(" + srcExpression->expressionStr + ")";
		this->isExpression = srcExpression->isExpression;
		for(int i = 0;i < this->childExpressions.size();i++)
		{
			this->childExpressions[i]->release();
            delete this->childExpressions[i];
		}
		this->childExpressions.clear();
		this->childExpressions.push_back(srcExpression->clone());
        this->childExpressions[0]->parentExpression = this;
	}
	else
	{
		this->type = srcExpression->type;
		this->tokenStr = srcExpression->tokenStr;
		this->expressionStr = srcExpression->expressionStr;
		this->isExpression = srcExpression->isExpression;
		for(int i = 0; i < this->childExpressions.size(); i++)
		{
			this->childExpressions[i]->release();
            delete this->childExpressions[i];
		}
		this->childExpressions.clear();
		
        for(int i = 0; i < srcExpression->childExpressions.size(); i++)
		{
			this->childExpressions.push_back(srcExpression->childExpressions[i]->clone());
            this->childExpressions[i]->parentExpression = this;
		}
	}
	Expression* p = this;
	while(p->parentExpression)
	{
		p = p->parentExpression;
	}
	p->updateExpressionStr();
}

vector<string> Expression::operatorRightStrList = {"++", "--"};
vector<string> Expression::operatorMemberStrList = {".", "->"};
vector<string> Expression::operatorLeftStrList = {"++", "--", "-", "+", "!", "~"};
vector<string> Expression::operatorAddressStrList = {"*", "&"};
vector<string> Expression::operatorBinocularStrList = {
    "*", "/", "%", "+", "-", "<<", ">>", "<", "<=",
    ">", ">=", "==", "!=", "&", "^", "|", "&&", "||"};
vector<string> Expression::operatorAssignStrList = {
    "=", "+=", "-=", "*=", "/=", "%=", "<<=", ">>=", "&=", "^=", "|="};
void Expression::updateExpressionStr()
{
	for(int i = 0; i < childExpressions.size();i++){
		childExpressions[i]->updateExpressionStr();
	}
	// Leaf types: no child access
	if(this->type == Token::Number || this->type == Token::Variable || this->type == Token::Word){
		this->expressionStr = this->tokenStr;
		return;
	}
	if(this->type == Token::Bracket || this->type == Token::Compound){
		this->expressionStr = this->type == Token::Bracket ? "(" : "{";
		for(int i = 0; i < this->childExpressions.size();i++){
			if(i != 0)
				this->expressionStr += ", ";
			this->expressionStr += this->childExpressions[i]->expressionStr;
		}
		this->expressionStr += this->type == Token::Bracket ? ")" : "}";
		return;
	}
	if(childExpressions.size() < 1)
		return;
	if(this->type >= Token::SelfAdd && this->type <= Token::SelfSub){
		this->expressionStr = childExpressions[0]->expressionStr + operatorRightStrList[this->type - Token::SelfAdd];
	}else if(this->type >= Token::Point && this->type <= Token::Arrow){
		if(childExpressions.size() < 2) return;
		this->expressionStr = childExpressions[0]->expressionStr + operatorMemberStrList[this->type - Token::Point] + childExpressions[1]->expressionStr;
	}else if(this->type == Token::Function){
		this->expressionStr = this->tokenStr;
		this->expressionStr += this->childExpressions[0]->expressionStr;
	}else if(this->type >= Token::SelfAddLeft && this->type <= Token::BitNot){
		this->expressionStr = operatorLeftStrList[this->type - Token::SelfAddLeft] + childExpressions[0]->expressionStr;
	}else if(this->type == Token::TypeConvert && childExpressions.size() == 1){
		this->expressionStr = "(" + childExpressions[0]->expressionStr + ")";
	}else if(this->type == Token::TypeConvert && childExpressions.size() == CONST_NUM_2){
		this->expressionStr = "(" + childExpressions[0]->expressionStr + ")"  + childExpressions[1]->expressionStr;
	}else if(this->type >= Token::Indirection && this->type <= Token::AddressOf){
		this->expressionStr = operatorAddressStrList[this->type - Token::Indirection] + childExpressions[0]->expressionStr;
	}else if(this->type == Token::SizeOf){
		this->expressionStr = "sizeof" + childExpressions[0]->expressionStr;
	}else if(this->type >= Token::Mul && this->type <= Token::Or){
		if(childExpressions.size() < 2) return;
		this->expressionStr = childExpressions[0]->expressionStr + " " + operatorBinocularStrList[this->type - Token::Mul] + " " + childExpressions[1]->expressionStr;
	}else if(this->type == Token::TernaryConditional){
		if(childExpressions.size() < 2) return;
		this->expressionStr = childExpressions[0]->expressionStr + " ? " + childExpressions[1]->expressionStr;
	}else if(this->type == Token::Select){
		if(childExpressions.size() < 2) return;
		this->expressionStr = childExpressions[0]->expressionStr + " : " + childExpressions[1]->expressionStr;
	}else if(this->type >= Token::Assign && this->type <= Token::OrAssign){
		if(childExpressions.size() < 2) return;
		this->expressionStr = childExpressions[0]->expressionStr + " " + operatorAssignStrList[this->type - Token::Assign] + " " + childExpressions[1]->expressionStr;
	}else if(this->type == Token::Subscript){
		this->expressionStr = "[" + childExpressions[0]->expressionStr + "]";
	}else if(this->type == Token::ArrayIndex){
		if(childExpressions.size() < 2) return;
		this->expressionStr = childExpressions[0]->expressionStr + childExpressions[1]->expressionStr;
	}
}

void Expression::release()
{
	for(int i = 0;i < childExpressions.size(); i++)
	{
		childExpressions[i]->release();
        delete this->childExpressions[i];
	}
	childExpressions.clear();
    //comment = true;
	//delete this;
}

Expression* Expression::clone() const
{
	Expression* ret = new Expression();
	ret->expressionStr = this->expressionStr;
	ret->isExpression = this->isExpression;
	ret->parentExpression = nullptr;
	ret->tokenStr = this->tokenStr;
	ret->type = this->type;
	for(int i = 0;i < this->childExpressions.size(); i++)
	{
		Expression* child = this->childExpressions[i]->clone();
		child->parentExpression = ret;
		ret->childExpressions.push_back(child);
	}
	ret->updateExpressionStr();
    return ret;
}

bool Expression::isContainAssign() const
{
    if(this->childExpressions.empty())
        return false;
    if(this->type >= Token::Assign && this->type <= Token::OrAssign || 
        this->type == Token::SelfAdd || 
        this->type == Token::SelfSub || 
        this->type == Token::SelfAddLeft || 
        this->type == Token::SelfSubLeft || 
        this->type == Token::Function)
        return true;
    for(int i = 0; i < this->childExpressions.size(); i++)
    {
        if(this->childExpressions[i]->isContainAssign())
        {
            return true;
        }
    }
    return false;
}

bool Expression::isBoolExpression() const
{
    const Expression* expression = this;
	if (expression->parentExpression != nullptr && expression->parentExpression->type == Token::TernaryConditional && expression->parentExpression->childExpressions[0] == expression)
	{
		return true;
	}
	if (expression->type == Token::Not ||expression->type == Token::And || expression->type == Token::Or || 
		expression->type == Token::Equal ||
		expression->type == Token::NotEqual || expression->type == Token::Less || 
		expression->type == Token::LessEqual || 
		expression->type == Token::Greater || expression->type == Token::GreaterEqual)
	{
		return true;
	}
	return false;
}


bool Expression::isConstantExpression() const
{
    if (this->type == Token::Number)
        return true;
    if (this->type == Token::Negative || this->type == Token::Positive ||
        this->type == Token::Bracket)
        return this->childExpressions[0]->isConstantExpression();
    return false;
}

bool Expression::isContainVariable(string varName) const
{
    if (this->type == Token::Variable && this->tokenStr == varName)
        return true;
    for (int i = 0; i < this->childExpressions.size(); i++)
    {
        if (this->childExpressions[i]->isContainVariable(varName))
            return true;
    }
    return false;
}

std::string Expression::getAssignedVariableName() const
{
    if (this->childExpressions.empty())
        return "";

    if (!(this->type >= Token::Assign && this->type <= Token::OrAssign ||
        this->type == Token::SelfAdd ||
        this->type == Token::SelfSub ||
        this->type == Token::SelfAddLeft ||
        this->type == Token::SelfSubLeft))
        return "";

    Expression* varExp = this->childExpressions[0];
    if (varExp->type == Token::Indirection)
        varExp = varExp->childExpressions[0];


    return varExp->expressionStr;
}

std::vector<std::string> Expression::getAllVariableNames() const
{
    std::vector<std::string> ret;
    if (this->type == Token::Variable)
    {
        ret.push_back(this->tokenStr);
        return ret;
    }
    for (int i = 0; i < this->childExpressions.size(); i++)
    {
        std::vector<std::string> childVars = this->childExpressions[i]->getAllVariableNames();
        // 去重添加进ret中
        for (int j = 0; j < childVars.size(); j++)
        {
            bool isExist = false;
            for (int k = 0; k < ret.size(); k++)
            {
                if (ret[k] == childVars[j])
                {
                    isExist = true;
                    break;
                }
            }
            if (!isExist)
                ret.push_back(childVars[j]);
        }
    }

    return ret;
}

Expression * Expression::SkipBracketToFindRealParent()
{
    Expression* par = this;
	if (par == nullptr) {
		return par;
	}
	if (par->type == Token::Bracket) {
		Expression * parpar = par->parentExpression;
        if(!parpar)
            return nullptr;
		return parpar->SkipBracketToFindRealParent();
	}
	return par;
}
