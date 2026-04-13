#include "ILTransStatementForTCGStateSearch.h"

#include "ILTransFile.h"

#include "ILTranslator.h"
#include "ILTransUtilityFunction.h"
#include "../Common/Utility.h"

#include "../ILBasic/StmtBreak.h"
#include "../ILBasic/StmtCase.h"
#include "../ILBasic/StmtDefault.h"
#include "../ILBasic/StmtDoWhile.h"
#include "../ILBasic/StmtElse.h"
#include "../ILBasic/StmtElseIf.h"
#include "../ILBasic/StmtExpression.h"
#include "../ILBasic/StmtFor.h"
#include "../ILBasic/StmtIf.h"
#include "../ILBasic/StmtLocalVariable.h"
#include "../ILBasic/StmtReturn.h"
#include "../ILBasic/StmtSwitch.h"
#include "../ILBasic/StmtWhile.h"
#include "../ILBasic/StmtBatchCalculation.h"

#include "../ILBasic/ILParser.h"
#include "../ILBasic/Expression.h"
#include "../ILBasic/ILUtilityFunction.h"
#include "../ILBasic/ILHeadFile.h"

#include "../ILBasic/BasicTypeCalculator.h"
#include "../ILBasic/ILCalculate.h"
#include "../ILBasic/ILFile.h"
#include "../ILBasic/ILRef.h"
#include "../ILBasic/ILExternFile.h"
#include "ILTransInstrumentBranchCoverage.h"
#include "ILTransInstrumentMCDCCoverage.h"
#include "ILTransInstrumentBitmapHelper.h"
#include "ILTransInstrumentExpressionIterator.h"

using namespace std;


namespace
{
    const int CONST_NUM_2 = 2;
}


int ILTransStatementForTCGStateSearch::translateStmtIf(const Statement* stmt)
{
	const StmtIf* stmtTemp = static_cast<const StmtIf*>(stmt);
	statementStr += "if (";
	int res = translateExpression(stmtTemp->condition);
	if(res < 0)
		return res;
	statementStr += ") {\n";
	
	Instrument_BC::InsertStatementInBlockBranchCoverageCollection(stmt, "positive_" + stmtTemp->condition->expressionStr, statementStr);

	for (int i = 0; i < stmt->childStatements.size(); i++)
	{
		res = translateStatement(stmt->childStatements[i]);
		if(res < 0)
			return res;
	}
	statementStr += "}\n";

	Instrument_BC::InsertStatementPostBlockBranchCoverageCollection(stmt, statementStr);

	return 1;
}

int ILTransStatementForTCGStateSearch::translateStmtElse(const Statement* stmt)
{
	const StmtElse* stmtTemp = static_cast<const StmtElse*>(stmt);
	statementStr += "else {\n";
	
	std::string condDesc = "";
	const Statement* prevStmt = stmt;
	do
	{
		prevStmt = prevStmt->getPreviousStatement();
		const StmtIf* prevStmtTemp = static_cast<const StmtIf*>(prevStmt);
		condDesc += "_" + prevStmtTemp->condition->expressionStr;
	} while (prevStmt->type != Statement::Type::If);

	
	Instrument_BC::InsertStatementInBlockBranchCoverageCollection(stmt, "negative_" + condDesc, statementStr);
	
	for (int i = 0; i < stmt->childStatements.size(); i++)
	{
		int res = translateStatement(stmt->childStatements[i]);
		if(res < 0)
			return res;
	}
	statementStr += "}\n";
	return 1;
}

int ILTransStatementForTCGStateSearch::translateStmtElseIf(const Statement* stmt)
{
	const StmtElseIf* stmtTemp = static_cast<const StmtElseIf*>(stmt);
	statementStr += "else if (";
	int res = translateExpression(stmtTemp->condition);
	if(res < 0)
		return res;
	statementStr += ") {\n";
	
	Instrument_BC::InsertStatementInBlockBranchCoverageCollection(stmt, "positive_" + stmtTemp->condition->expressionStr, statementStr);

	for (int i = 0; i < stmt->childStatements.size(); i++)
	{
		res = translateStatement(stmt->childStatements[i]);
		if(res < 0)
			return res;
	}
	statementStr += "}\n";

	Instrument_BC::InsertStatementPostBlockBranchCoverageCollection(stmt, statementStr);

	return 1;
}

int ILTransStatementForTCGStateSearch::translateStmtWhile(const Statement* stmt)
{
	const StmtWhile* stmtTemp = static_cast<const StmtWhile*>(stmt);
	statementStr += "while (";
	int res = translateExpression(stmtTemp->condition);
	if(res < 0)
		return res;
	statementStr += ") {\n";
	
	Instrument_BC::InsertStatementInBlockBranchCoverageCollection(stmt, "positive_" + stmtTemp->condition->expressionStr, statementStr);

	for (int i = 0; i < stmt->childStatements.size(); i++)
	{
		res = translateStatement(stmt->childStatements[i]);
		if(res < 0)
			return res;
	}
	statementStr += "}\n";

	Instrument_BC::InsertStatementPostBlockBranchCoverageCollection(stmt, statementStr);

	return 1;
}

int ILTransStatementForTCGStateSearch::translateStmtDoWhile(const Statement* stmt)
{
	const StmtDoWhile* stmtTemp = static_cast<const StmtDoWhile*>(stmt);
	int res;
	
	statementStr += "do {\n";
	
	for (int i = 0; i < stmt->childStatements.size(); i++)
	{
		res = translateStatement(stmt->childStatements[i]);
		if(res < 0)
			return res;
	}
	statementStr += "} ";
	statementStr += "while (";
	res = translateExpression(stmtTemp->condition);
	if(res < 0)
		return res;
	statementStr += ");\n";

	Instrument_BC::InsertStatementPostBlockBranchCoverageCollection(stmt, statementStr);

	return 1;
}

int ILTransStatementForTCGStateSearch::translateStmtFor(const Statement* stmt)
{
	const StmtFor* stmtTemp = static_cast<const StmtFor*>(stmt);
	int res;
	statementStr += "for (";
	for(int i = 0;i <stmtTemp->initial.size();i++)
	{
		if(i != 0)
			statementStr += ",";
		res = translateStatement(stmtTemp->initial[i]);
		if(res < 0)
			return res;
		statementStr = stringTrim(statementStr);
		if(stringEndsWith(statementStr, ";"))
		{
			statementStr = statementStr.substr(0,statementStr.length()-1);
		}
	}
	statementStr += ";";

	if(stmtTemp->condition)
    {
	    res = translateExpression(stmtTemp->condition);
	    if(res < 0)
		    return res;
    }

	statementStr += ";";

	for(int i = 0;i <stmtTemp->iterator.size();i++)
	{
		if(i != 0)
			statementStr += ",";
		res = translateStatement(stmtTemp->iterator[i]);
		if(res < 0)
			return res;
		statementStr = stringTrim(statementStr);
		if(stringEndsWith(statementStr, ";"))
		{
			statementStr = statementStr.substr(0,statementStr.length()-1);
		}
	}
	
	statementStr += ") {\n";

	std::string for_cond_str = ILTransInstrumentBitmapHelper::AlwaysTrueCondDesc;
	if (stmtTemp->condition != nullptr) {
		for_cond_str = stmtTemp->condition->expressionStr;
	}
	
	Instrument_BC::InsertStatementInBlockBranchCoverageCollection(stmt, "positive_" + for_cond_str, statementStr);

	for (int i = 0; i < stmtTemp->statements.size(); i++)
	{
		res = translateStatement(stmtTemp->statements[i]);
		if(res < 0)
			return res;
	}
	statementStr += "}\n";

	Instrument_BC::InsertStatementPostBlockBranchCoverageCollection(stmt, statementStr);
	
	return 1;
}

int ILTransStatementForTCGStateSearch::translateStmtContinue(const Statement* stmt)
{
	statementStr += "continue;\n";
	return 1;
}

int ILTransStatementForTCGStateSearch::translateStmtSwitch(const Statement* stmt)
{
	const StmtSwitch* stmtTemp = static_cast<const StmtSwitch*>(stmt);
	//ILTransInstrumentMCDCCoverage::InsertMCDCToSwitch(stmtTemp, statementStr);
	
	statementStr += "switch(";
	int res = translateExpression(stmtTemp->condition);
	if(res < 0)
		return res;
	statementStr += ")\n";

	statementStr += "{\n";
	bool lastIsDefault = false;
	int csize = stmt->childStatements.size();
	Statement* prevCaseStmt = nullptr;
	for (int i = 0; i < csize; i++)
	{
		Statement* oneStmt = stmt->childStatements[i];
		if (oneStmt->type == Statement::Type::Default) {
			lastIsDefault = true;
		}

		res = translateStatement(oneStmt);
		if(res < 0)
			return res;

		if (oneStmt->type == Statement::Type::Case || oneStmt->type == Statement::Type::Default) {
			statementStr += "{\n";
			prevCaseStmt = oneStmt;
		}

		bool insertRightBracket = false;
		int nexti = i + 1;
		if (nexti >= csize) {
			if (prevCaseStmt != nullptr) {
				insertRightBracket = true;
			}
		}
		else {
			Statement* nextStmt = stmt->childStatements[nexti];
			if (prevCaseStmt != nullptr && (nextStmt->type == Statement::Type::Case || nextStmt->type == Statement::Type::Default)) {
				insertRightBracket = true;
			}
		}

		if (insertRightBracket) {
			statementStr += "}\n";
		}
	}

	if (!lastIsDefault) {
		statementStr += "default:\n";
		Instrument_BC::InsertSwitchDefaultBranchCoverageCollection(stmt, "switch_default_" + stmtTemp->condition->expressionStr, statementStr);
	}

	statementStr += "}\n";
	return 1;
}

int ILTransStatementForTCGStateSearch::translateStmtCase(const Statement* stmt)
{
	const StmtCase* stmtTemp = static_cast<const StmtCase*>(stmt);
	statementStr += "case ";
	int res = translateExpression(stmtTemp->value);
	if(res < 0)
		return res;
	statementStr += ":\n";
	const StmtSwitch* parentStmtTemp = static_cast<const StmtSwitch*>(stmt->parentStatement);
	Instrument_BC::InsertStatementInBlockBranchCoverageCollection(stmt, "switch_" + parentStmtTemp->condition->expressionStr + "_case_" + stmtTemp->value->expressionStr, statementStr);

	return 1;
}

int ILTransStatementForTCGStateSearch::translateStmtDefault(const Statement* stmt)
{
	const StmtDefault* stmtTemp = static_cast<const StmtDefault*>(stmt);
	statementStr += "default:\n";
    const StmtSwitch* parentStmtTemp = static_cast<const StmtSwitch*>(stmt->parentStatement);
	Instrument_BC::InsertStatementInBlockBranchCoverageCollection(stmt, "switch_default_" + parentStmtTemp->condition->expressionStr, statementStr);

	return 1;
}
