#pragma once
#include <vector>
#include <string>
#include "../ILBasic/Expression.h"
#include <map>

class ILFunction;

class ILTransInstrumentExpressionIterator
{
public:    
    static std::string InstrumentRootBoolExpressions(const Expression* expression, ILFunction* mcdcContextFunction = nullptr);

private:
    //static bool IsBoolExpression(const Expression* expression);
    //static Expression * SkipBracketToFindRealParent(Expression * par);
    static void RecursiveInstrumentRootBoolExpressions(const Expression * root, std::map<std::string, std::string>& exprStrMap);

    //static void generateMCDCFunc();
    
    static std::map<int, std::string> MCDCCoverageFuncMap;

};

