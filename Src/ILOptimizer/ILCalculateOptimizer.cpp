#include "ILCalculateOptimizer.h"

#include "ILOptimizer.h"
#include "ILStatementOptimizer.h"

#include "../ILBasic/ILSchedule.h"
#include "../ILBasic/ILInput.h"
#include "../ILBasic/ILOutput.h"
#include "../ILBasic/ILCalculate.h"

#include "../ILBasic/Statement.h"
#include "../ILBasic/StmtFor.h"
#include "../ILBasic/Expression.h"
#include "../ILBasic/ILParser.h"
#include "../ILBasic/StmtBatchCalculation.h"

using namespace std;
int ILCalculateOptimizer::optimize(ILCalculate* calculate, ILParser* parser)
{
    this->iLParser = parser;
	this->calculate = calculate;
	int	res = colletExpressionVariable();
	if(res<0)
		return res;

	res = optimizeInputsAndOutputs();
	if(res<0)
		return res;

	for(int i = calculate->statements.childStatements.size() - 1;i >= 0; i--)
	{
		Statement* statement = calculate->statements.childStatements[i];
		ILStatementOptimizer iLStatementOptimizer;
		res = iLStatementOptimizer.optimize(statement, iLParser);
		if(res < 0)
			return res;
	}


	if(calculate->statements.childStatements.empty())
	{
		iLParser->releaseILObject(calculate);
	}

	return 1;
}

int ILCalculateOptimizer::colletExpressionVariable()
{
	for(int i = 0;i < calculate->statements.childStatements.size(); i++)
	{
		Statement* statement = calculate->statements.childStatements[i];
		int res = traverseCollectStatement(statement);
			if(res < 0)
				return res;
	}
	return 1;
}

int ILCalculateOptimizer::traverseCollectStatement(Statement* statement)
{
	int res = 0;
    Expression* exp = statement->getValueExpression();
    vector<Statement*>* stmtList = statement->getStatementList();
    if(statement->type == Statement::DoWhile) {
		for(int i = 0; stmtList && res >=0 && i < stmtList->size();i++){
			res = traverseCollectStatement((*stmtList)[i]);
		}
        if(res >= 0) {
		    res = traverseCollectExpression(exp);
        }
		return res < 0 ? res : 1;
	} else if(statement->type == Statement::For) {
		StmtFor* stmtTemp = static_cast<StmtFor*>(statement);
		for(int i = 0; res >=0 && i< stmtTemp->initial.size();i++) {
			res = traverseCollectStatement(stmtTemp->initial[i]);
		}
        if(res >= 0) {
		    res = traverseCollectExpression(stmtTemp->condition);
        }
		for(int i = 0; res >=0 && i < stmtTemp->statements.size();i++) {
			res = traverseCollectStatement(stmtTemp->statements[i]);
		}
		for(int i = 0; res >=0 && i< stmtTemp->iterator.size();i++) {
			res = traverseCollectStatement(stmtTemp->iterator[i]);
		}
		return res < 0 ? res : 1;
	} else if(statement->type == Statement::BatchCalculation) {
        StmtBatchCalculation* stmt = static_cast<StmtBatchCalculation*>(statement);
        
        for(int i = 0; i < stmt->inputs.size(); i++) {
            string varName = stmt->inputs[i].name;
            this->variableList.insert(varName);
        }
        for(int i = 0; i < stmt->outputs.size(); i++) {
            string varName = stmt->outputs[i].name;
            this->variableList.insert(varName);
        }
    }
    //else {
    //    return 0;
        //comment = true;
        //-ILOptimizer::ErrorInvalidStatementType;
    //}
    if(exp && (res = traverseCollectExpression(exp)) < 0) {
        return res;
    }
	for(int i = 0; stmtList && i < stmtList->size();i++) {
		res = traverseCollectStatement((*stmtList)[i]);
		if(res < 0) {
			return res;
		}
	}
	return 1;
}

int ILCalculateOptimizer::traverseCollectExpression(Expression* expression)
{
	if(!expression)
		return 0;
	if(expression->type == Token::Variable)
	{
		this->variableList.insert(expression->expressionStr);
	}

	for(int i = 0;i < expression->childExpressions.size();i++)
	{
		int res = traverseCollectExpression(expression->childExpressions[i]);
		if(res < 0)
			return res;
	}
    return 1;
}

int ILCalculateOptimizer::optimizeInputsAndOutputs()
{
    unordered_map<string, bool> inputMap;
    for(int i = calculate->inputs.size() - 1; i >= 0; i--)
	{
        ILInput* input = calculate->inputs[i];
        if(inputMap.find(input->name) != inputMap.end()) {
            iLParser->releaseILObject(input);
        } else {
            inputMap.insert(make_pair(input->name, true));
        }
    }
    unordered_map<string, bool> outputMap;
    for(int i = calculate->outputs.size() - 1; i >= 0; i--)
    {
        ILOutput* output = calculate->outputs[i];
        if(outputMap.find(output->name) != outputMap.end()) {
            iLParser->releaseILObject(output);
        } else {
            outputMap.insert(make_pair(output->name, true));
        }
    }

	for(int i = calculate->inputs.size() - 1; i >= 0; i--)
	{
		ILInput* input = calculate->inputs[i];
		if(variableList.find(input->sourceStr) == variableList.end() &&
			variableList.find(input->name) == variableList.end())
		{
			iLParser->releaseILObject(input);
		}
	}
	for(int i = calculate->outputs.size() - 1; i >= 0; i--)
	{
		ILOutput* output = calculate->outputs[i];
		if(!(variableList.find(calculate->name + "_" + output->name) != variableList.end() || 
            variableList.find(output->name) != variableList.end() ||
            variableList.find(output->sourceStr) != variableList.end()))
		{
			iLParser->releaseILObject(output);
		}
	}




	return 1;
}

int ILCalculateOptimizer::release() const
{
	return 1;
}
