#include "ILTransExpression.h"
using namespace std;

int ILTransExpression::translate(const Expression* expression, string* statementStr) const
{
	/*int res;
	if(expression->childExpressions.empty())
	{
		*statementStr +=  "\n" +expression->expressionStr;
		return 1;
	}
	for(int i = 0; i< expression->childExpressions.size();i++)
	{
		ILTransExpression iLTransExpression;
		res = iLTransExpression.translate(expression->childExpressions[i], &(*statementStr));
		if(res < 0)
			return res;
	}*/
	return 1;
}
