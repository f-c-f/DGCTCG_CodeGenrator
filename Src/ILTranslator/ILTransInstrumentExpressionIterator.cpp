#include "ILTransInstrumentExpressionIterator.h"
#include "ILTransInstrumentMCDCCoverage.h"
#include "ILTransInstrumentBitmapHelper.h"
#include "../Common/Utility.h"
#include <assert.h>

std::map<int, std::string> MCDCCoverageFunc;
/*
bool ILTransInstrumentExpressionIterator::IsBoolExpression(const Expression* expression)
{
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
*/

/*
Expression * ILTransInstrumentExpressionIterator::SkipBracketToFindRealParent(Expression * par) {
	if (par == nullptr) {
		return par;
	}
	if (par->type == Token::Bracket) {
		Expression * parpar = par->parentExpression;
		return SkipBracketToFindRealParent(parpar);
	}
	return par;
}
*/
void ILTransInstrumentExpressionIterator::RecursiveInstrumentRootBoolExpressions(const Expression * root, std::map<std::string, std::string>& exprStrMap)
{
	int pos = 0;
	std::string modified = root->expressionStr;
	for (Expression* exp : root->childExpressions)
	{
		RecursiveInstrumentRootBoolExpressions(exp, exprStrMap);
	}
	bool isBoolExp = root->isBoolExpression();
    Expression * p = root->parentExpression;
	Expression * rp = p ? p->SkipBracketToFindRealParent() : nullptr;
	if (isBoolExp && (rp == nullptr || !rp->isBoolExpression())) {
		modified = ILTransInstrumentMCDCCoverage::GenerateMCDCToRootExpression(root, exprStrMap);
	}
	else {
		for (Expression* exp : root->childExpressions)
		{
			std::string oriExpStr = exp->expressionStr;
			std::string expAddr = std::to_string((long long)exp);
			std::string expStr = exprStrMap[expAddr];
			int srclen = oriExpStr.size(), dstlen = expStr.size();
			pos = modified.find(oriExpStr, pos);
			assert(pos != std::string::npos);
			modified.replace(pos, srclen, expStr);
			pos += dstlen;
		}
	}
	std::string bbStr = std::to_string((long long)root);
	exprStrMap[bbStr] = modified;
}


std::string ILTransInstrumentExpressionIterator::InstrumentRootBoolExpressions(const Expression* root, ILFunction* mcdcContextFunction)
{
	if (root == NULL)
	{
		return "";
	}
	ILFunction* prevCtx = ILTransInstrumentBitmapHelper::mcdcMetadataContextFn;
	ILTransInstrumentBitmapHelper::mcdcMetadataContextFn = mcdcContextFunction;
	std::map<std::string, std::string> exprStrMap;
//	if (stringStartsWith(stringTrim(root->expressionStr), "*euler321_I2B_12B_Y_outR1")) {
//		printf("OK! Here!");
//	}
	RecursiveInstrumentRootBoolExpressions(root, exprStrMap);
	ILTransInstrumentBitmapHelper::mcdcMetadataContextFn = prevCtx;
	std::string bbStr = std::to_string((long long)root);
	std::string res = exprStrMap[bbStr];
	return res;
}


