#include "ILTransStatement.h"

#include "ILTransFile.h"

#include "ILTranslator.h"
#include "ILTranslatorForC.h"
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

#include "../ILBasic/ILPreprocessor.h"
#include <iostream>

#include "ILTransStatementForCBMC.h"
#include "ILTransStatementForTCGStateSearch.h"
#include "ILTransStatementForCoverage.h"

#include "ILTransInstrumentExpressionIterator.h"
#include "ILTransStatementForLibFuzzer.h"
#include "ILTransStatementForTCGHybrid.h"


#include "../ILBasic/ILAnalyzeSchedule.h"
#include "../ILBasic/ILAnalyzeScheduleNode.h"


#include "ILTranslatorForTCGStateSearch.h"
#include "ILTranslatorForTCGHybrid.h"
#include "ILTranslator.h"



using namespace std;


namespace
{
    const int CONST_NUM_2 = 2;
}

int ILTransStatement::translate(const Statement* statement, ILTransFile* file, const ILAnalyzeSchedule* iLTranslateSchedule)
{
    iLTransFile = file;

    this->iLTranslateSchedule = iLTranslateSchedule;

	statementStr = "";
	int res = translateStatement(statement);
	if(res < 0)
		return res;
    
	//file->transCodeFunction += statementStr;
    


	return 1;
}

int ILTransStatement::translateStatement(const Statement* stmt)
{
	int ret = 0;
    currentTransStatement = stmt;

    /*if (ILTranslatorForTCGStateSearch::configBranchDistanceGuidance && 
        ILTranslator::translateForTCGStateSearch && 
        ILTranslatorForTCGStateSearch::branchDistanceStmtObjectList.find(stmt) != ILTranslatorForTCGStateSearch::branchDistanceStmtObjectList.end())
    */
    if (ILTranslatorForTCGStateSearch::configBranchDistanceGuidance && 
        ILTranslator::translateForTCGStateSearch && 
        ILTranslatorForTCGStateSearch::branchDistanceStmtObjectList.find(stmt) != ILTranslatorForTCGStateSearch::branchDistanceStmtObjectList.end())
 
    {
        for (auto item : ILTranslatorForTCGStateSearch::branchDistanceStmtObjectList[stmt])
        {
            statementStr += "StateSearch::updateBranchDistanceValue(\"" + item + "\", " + item + ");\n";
        }
    }
    else if(ILTranslator::translateForTCGHybrid &&
        ILTranslatorForTCGHybrid::branchDistanceStmtObjectList.find(stmt) != ILTranslatorForTCGHybrid::branchDistanceStmtObjectList.end())

    {
        for (auto item : ILTranslatorForTCGHybrid::branchDistanceStmtObjectList[stmt])
        {
            statementStr += "TCGHybrid::updateBranchDistanceValue(\"" + item + "\", " + item + ");\n";
        }
    }
	if (stmt->type == Statement::If && (ret = translateStmtIf(stmt)) != 0) {
	}
	else if (stmt->type == Statement::Else && (ret = translateStmtElse(stmt)) != 0) {
	}
	else if (stmt->type == Statement::ElseIf && (ret = translateStmtElseIf(stmt)) != 0) {
	}
	else if (stmt->type == Statement::While && (ret = translateStmtWhile(stmt)) != 0) {
	}
	else if (stmt->type == Statement::DoWhile && (ret = translateStmtDoWhile(stmt)) != 0) {
	}
	else if (stmt->type == Statement::For && (ret = translateStmtFor(stmt)) != 0) {
	}
	else if (stmt->type == Statement::Continue && (ret = translateStmtContinue(stmt)) != 0) {
	}
	else if (stmt->type == Statement::Switch && (ret = translateStmtSwitch(stmt)) != 0) {
	}
	else if (stmt->type == Statement::Case && (ret = translateStmtCase(stmt)) != 0) {
	}
	else if (stmt->type == Statement::Default && (ret = translateStmtDefault(stmt)) != 0) {
	}
	else if (stmt->type == Statement::Break && (ret = translateStmtBreak(stmt)) != 0) {
	}
	else if (stmt->type == Statement::Return && (ret = translateStmtReturn(stmt)) != 0) {
	}
	else if (stmt->type == Statement::BraceStatement && (ret = translateStmtBraceStatement(stmt)) != 0) {
	}
	else if (stmt->type == Statement::LocalVariable && (ret = translateStmtLocalVariable(stmt)) != 0) {
	}
	else if (stmt->type == Statement::ExpressionWithSemicolon && (ret = translateStmtExpressionWithSemicolon(stmt)) != 0) {
	}
	else if (stmt->type == Statement::BatchCalculation && (ret = translateStmtBatchCalculation(stmt)) != 0) {
        
	}
	else {
		
	}
	if (ret < 0) return ret;
	return 1;
	
}


int ILTransStatement::translateStmtIf(const Statement* stmt)
{
	const StmtIf* stmtTemp = static_cast<const StmtIf*>(stmt);
	statementStr += "if (";
	int res = translateExpression(stmtTemp->condition);
	if(res < 0)
		return res;
	statementStr += ") {\n";

	//statementStr += "{\n";

	for (int i = 0; i < stmt->childStatements.size(); i++)
	{
		res = translateStatement(stmt->childStatements[i]);
		if(res < 0)
			return res;
	}
	statementStr += "}\n";
	return 1;
}

int ILTransStatement::translateStmtElse(const Statement* stmt)
{
	const StmtElse* stmtTemp = static_cast<const StmtElse*>(stmt);
	statementStr += "else {\n";

	//statementStr += "{\n";
	for (int i = 0; i < stmt->childStatements.size(); i++)
	{
		int res = translateStatement(stmt->childStatements[i]);
		if(res < 0)
			return res;
	}
	statementStr += "}\n";
	return 1;
}

int ILTransStatement::translateStmtElseIf(const Statement* stmt)
{
	const StmtElseIf* stmtTemp = static_cast<const StmtElseIf*>(stmt);
	statementStr += "else if (";
	int res = translateExpression(stmtTemp->condition);
	if(res < 0)
		return res;
	statementStr += ") {\n";

	//statementStr += "{\n";
	for (int i = 0; i < stmt->childStatements.size(); i++)
	{
		res = translateStatement(stmt->childStatements[i]);
		if(res < 0)
			return res;
	}
	statementStr += "}\n";
	return 1;
}

int ILTransStatement::translateStmtWhile(const Statement* stmt)
{
	const StmtWhile* stmtTemp = static_cast<const StmtWhile*>(stmt);
	statementStr += "while (";
	int res = translateExpression(stmtTemp->condition);
	if(res < 0)
		return res;
	statementStr += ") {\n";

	//statementStr += "{\n";
	for (int i = 0; i < stmt->childStatements.size(); i++)
	{
		res = translateStatement(stmt->childStatements[i]);
		if(res < 0)
			return res;
	}
	statementStr += "}\n";
	return 1;
}

int ILTransStatement::translateStmtDoWhile(const Statement* stmt)
{
	const StmtDoWhile* stmtTemp = static_cast<const StmtDoWhile*>(stmt);
	int res;
	
	statementStr += "do {\n";
	//statementStr += "{\n";
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

	return 1;
}

int ILTransStatement::translateStmtFor(const Statement* stmt)
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

	//statementStr += "{\n";
	for (int i = 0; i < stmtTemp->statements.size(); i++)
	{
		res = translateStatement(stmtTemp->statements[i]);
		if(res < 0)
			return res;
	}
	statementStr += "}\n";
	return 1;
}

int ILTransStatement::translateStmtContinue(const Statement* stmt)
{
	statementStr += "continue;\n";
	return 1;
}

int ILTransStatement::translateStmtSwitch(const Statement* stmt)
{
	const StmtSwitch* stmtTemp = static_cast<const StmtSwitch*>(stmt);
	
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
        statementStr += "break;\n";
	}

	statementStr += "}\n";
	return 1;
}

int ILTransStatement::translateStmtCase(const Statement* stmt)
{
	const StmtCase* stmtTemp = static_cast<const StmtCase*>(stmt);
	statementStr += "case ";
	int res = translateExpression(stmtTemp->value);
	if(res < 0)
		return res;
	statementStr += ":\n";

	return 1;
}

int ILTransStatement::translateStmtDefault(const Statement* stmt)
{
	const StmtDefault* stmtTemp = static_cast<const StmtDefault*>(stmt);
	statementStr += "default:\n";
	return 1;
}



int ILTransStatement::translateStmtBreak(const Statement* stmt)
{
	const StmtBreak* stmtTemp = static_cast<const StmtBreak*>(stmt);
	statementStr += "break;\n";
	return 1;
}

int ILTransStatement::translateStmtReturn(const Statement* stmt)
{
	const StmtReturn* stmtTemp = static_cast<const StmtReturn*>(stmt);

	statementStr += "return";
	if(stmtTemp->expression)
	{
        statementStr += " ";
		int res = translateExpression(stmtTemp->expression);
		if(res < 0)
			return res;
	}
	statementStr += ";\n";

	return 1;
}

int ILTransStatement::translateStmtBraceStatement(const Statement* stmt) const
{
	return -ILTranslator::ErrorTranslateStmtBraceStatementShouldNotExist;
}

int ILTransStatement::translateStmtLocalVariable(const Statement* stmt)
{
	const StmtLocalVariable* stmtTemp = static_cast<const StmtLocalVariable*>(stmt);
    
    if(stmtTemp->align > 0)
    {
        statementStr += "__declspec(aligned(" + to_string(stmtTemp->align) + ")) ";
    }

    if(stmtTemp->isConst)
    {
        statementStr += "const ";
    }

    statementStr += ILParser::convertDataType(stmtTemp->dataType);

	if(stmtTemp->isAddress > 0)
		statementStr += " " + stringRepeat("*", stmtTemp->isAddress);
	statementStr += " " + stmtTemp->name;
	if(!stmtTemp->arraySize.empty())
	{
		for(int i = 0;i < stmtTemp->arraySize.size(); i++)
		{
            int arrayLen = stmtTemp->arraySize[i];
            if(ILTranslator::translateForCBMC && arrayLen > ILTranslatorForCBMC::configCutOffArray)
                arrayLen = ILTranslatorForCBMC::configCutOffArray;
			statementStr += "[" + to_string(arrayLen) + "]";
		}
	}
	if(stmtTemp->defaultValue)
	{
		statementStr += " = ";
        if(ILTranslator::translateForCBMC && stmtTemp->defaultValue->type == Token::Compound) {
            statementStr += ILTranslatorForCBMC::getCutOffArrayDefaultValueExpStr(stmtTemp->name, stmtTemp->dataType, stmtTemp->arraySize, stmtTemp->defaultValue);
        }
	    else {
		    int res = translateExpression(stmtTemp->defaultValue);
		    if(res < 0)
			    return res;
        }
	}
    else
    {
        statementStr += " = ";
        if(ILParser::isBasicType(stmtTemp->dataType) && stmtTemp->arraySize.empty())
        {
            statementStr += "0";
        }
        else
        {
            statementStr += "{0}";
        }
    }
	statementStr += ";\n";

    // ����STCG������������������������ľ�����ر���ֵͳ�ƴ���
    if (ILTranslatorForTCGStateSearch::configBranchDistanceGuidance && 
        ILTranslator::translateForTCGStateSearch && stmtTemp->defaultValue)
    {
        std::vector<ILTranslatorForTCGStateSearch::BranchDistanceData>* branchDistanceDataList = &ILTranslatorForTCGStateSearch::branchDistanceDataList;
        for (int i = 0; i < branchDistanceDataList->size(); i++)
        {
            if (branchDistanceDataList->at(i).varName == stmtTemp->name)
            {
                // StateSearch::updateDistanceValue("ComplexParaTest2_DW->A", ComplexParaTest2_DW->A);
                statementStr += "StateSearch::updateBranchDistanceValue(\"" + stmtTemp->name + "\", " + stmtTemp->name + ");\n";
                break;
            }
        }
    }
    else if (ILTranslator::translateForTCGHybrid && stmtTemp->defaultValue)
    {
        std::vector<ILTranslatorForTCGHybrid::BranchDistanceData>* branchDistanceDataList = &ILTranslatorForTCGHybrid::branchDistanceDataList;
        for (int i = 0; i < branchDistanceDataList->size(); i++)
        {
            if (branchDistanceDataList->at(i).varName == stmtTemp->name)
            {
                // Hybrid::updateDistanceValue("ComplexParaTest2_DW->A", ComplexParaTest2_DW->A);
                statementStr += "TCGHybrid::updateBranchDistanceValue(\"" + stmtTemp->name + "\", " + stmtTemp->name + ");\n";
                break;
            }
        }
    }



	return 1;
}

int ILTransStatement::translateStmtExpressionWithSemicolon(const Statement* stmt)
{
	const StmtExpression* stmtTemp = static_cast<const StmtExpression*>(stmt);
	int res = translateExpression(stmtTemp->expression);
	statementStr += ";\n";
	if(res < 0)
		return res;

    // STCG 分支距离引导：为赋值语句插桩，更新分支距离统计信息
    if (ILTranslatorForTCGStateSearch::configBranchDistanceGuidance && 
        ILTranslator::translateForTCGStateSearch && stmtTemp->expression)
    {
        Expression* exp = stmtTemp->expression;
        string assignedVarName = "";
        if (exp->type >= Token::Assign && exp->type <= Token::OrAssign || 
            exp->type == Token::SelfAdd ||
            exp->type == Token::SelfSub || 
            exp->type == Token::SelfAddLeft || 
            exp->type == Token::SelfSubLeft)
        {
            assignedVarName = exp->childExpressions[0]->expressionStr;
            //assignedVarName = getVariableExpRootVarName(assignedVarName);
        }

        if (!assignedVarName.empty())
        {
            std::vector<ILTranslatorForTCGStateSearch::BranchDistanceData>* branchDistanceDataList = &ILTranslatorForTCGStateSearch::branchDistanceDataList;
            for (int i = 0; i < branchDistanceDataList->size(); i++)
            {
                if (branchDistanceDataList->at(i).varName == assignedVarName)
                {
                    // StateSearch::updateDistanceValue("ComplexParaTest2_DW->A", ComplexParaTest2_DW->A);
                    statementStr += "StateSearch::updateBranchDistanceValue(\"" + assignedVarName + "\", " + assignedVarName + ");\n";
                    break;
                }
            }
        }
    }
    // Hybrid 分支距离引导：为赋值语句插桩，更新分支距离统计信息
    else if (ILTranslator::translateForTCGHybrid && stmtTemp->expression)
    {
        Expression* exp = stmtTemp->expression;
        string assignedVarName = "";
        if (exp->type >= Token::Assign && exp->type <= Token::OrAssign || 
            exp->type == Token::SelfAdd ||
            exp->type == Token::SelfSub || 
            exp->type == Token::SelfAddLeft || 
            exp->type == Token::SelfSubLeft)
        {
            assignedVarName = exp->childExpressions[0]->expressionStr;
            //assignedVarName = getVariableExpRootVarName(assignedVarName);
        }
        if (!assignedVarName.empty())
        {
            std::vector<ILTranslatorForTCGHybrid::BranchDistanceData>* branchDistanceDataList = &ILTranslatorForTCGHybrid::branchDistanceDataList;
            for (int i = 0; i < branchDistanceDataList->size(); i++)
            {
                if (branchDistanceDataList->at(i).varName == assignedVarName)
                {
                    // Hybrid::updateDistanceValue("ComplexParaTest2_DW->A", ComplexParaTest2_DW->A);
                    statementStr += "TCGHybrid::updateBranchDistanceValue(\"" + assignedVarName + "\", " + assignedVarName + ");\n";
                    break;
                }
            }
        }
    }



	return 1;
}



std::map<StmtBatchCalculation::OperationType, std::string> ILTransStatement::stmtBatchCalculationOperationToStrMap = {
    pair<StmtBatchCalculation::OperationType, string>(StmtBatchCalculation::TypeAdd, "+"),
    pair<StmtBatchCalculation::OperationType, string>(StmtBatchCalculation::TypeSub, "-"),
    pair<StmtBatchCalculation::OperationType, string>(StmtBatchCalculation::TypeMul, "*"),
    pair<StmtBatchCalculation::OperationType, string>(StmtBatchCalculation::TypeDiv, "/"),
    pair<StmtBatchCalculation::OperationType, string>(StmtBatchCalculation::TypeLShr, ">>"),
    pair<StmtBatchCalculation::OperationType, string>(StmtBatchCalculation::TypeAShr, ">>"),
    pair<StmtBatchCalculation::OperationType, string>(StmtBatchCalculation::TypeShl, "<<"),
    pair<StmtBatchCalculation::OperationType, string>(StmtBatchCalculation::TypeAnd, "&"),
    pair<StmtBatchCalculation::OperationType, string>(StmtBatchCalculation::TypeOr, "|"),
    pair<StmtBatchCalculation::OperationType, string>(StmtBatchCalculation::TypeXor, "^"),
    pair<StmtBatchCalculation::OperationType, string>(StmtBatchCalculation::TypeLogicAnd, "&&"),
    pair<StmtBatchCalculation::OperationType, string>(StmtBatchCalculation::TypeLogicOr, "||"),
    pair<StmtBatchCalculation::OperationType, string>(StmtBatchCalculation::TypeLogicNot, "!"),
    pair<StmtBatchCalculation::OperationType, string>(StmtBatchCalculation::TypeLess, "<"),
    pair<StmtBatchCalculation::OperationType, string>(StmtBatchCalculation::TypeLessEqual, "<="),
    pair<StmtBatchCalculation::OperationType, string>(StmtBatchCalculation::TypeGreater, ">"),
    pair<StmtBatchCalculation::OperationType, string>(StmtBatchCalculation::TypeGreaterEqual, ">="),
    pair<StmtBatchCalculation::OperationType, string>(StmtBatchCalculation::TypeEqual, "=="),
    pair<StmtBatchCalculation::OperationType, string>(StmtBatchCalculation::TypeNotEqual, "!="),
    pair<StmtBatchCalculation::OperationType, string>(StmtBatchCalculation::TypeMod, "%"),
};
std::map<StmtBatchCalculation::OperationType, Token::Type> ILTransStatement::stmtBatchCalculationOperationToTokenTypeMap = {
    pair<StmtBatchCalculation::OperationType, Token::Type>(StmtBatchCalculation::TypeAdd, Token::Add),
    pair<StmtBatchCalculation::OperationType, Token::Type>(StmtBatchCalculation::TypeSub, Token::Sub),
    pair<StmtBatchCalculation::OperationType, Token::Type>(StmtBatchCalculation::TypeMul, Token::Mul),
    pair<StmtBatchCalculation::OperationType, Token::Type>(StmtBatchCalculation::TypeDiv, Token::Div),
    pair<StmtBatchCalculation::OperationType, Token::Type>(StmtBatchCalculation::TypeLShr, Token::RightShift),
    pair<StmtBatchCalculation::OperationType, Token::Type>(StmtBatchCalculation::TypeAShr, Token::RightShift),
    pair<StmtBatchCalculation::OperationType, Token::Type>(StmtBatchCalculation::TypeShl, Token::LeftShift),
    pair<StmtBatchCalculation::OperationType, Token::Type>(StmtBatchCalculation::TypeAnd, Token::BitAnd),
    pair<StmtBatchCalculation::OperationType, Token::Type>(StmtBatchCalculation::TypeOr, Token::BitOr),
    pair<StmtBatchCalculation::OperationType, Token::Type>(StmtBatchCalculation::TypeXor, Token::BitXor),
    pair<StmtBatchCalculation::OperationType, Token::Type>(StmtBatchCalculation::TypeLogicAnd, Token::And),
    pair<StmtBatchCalculation::OperationType, Token::Type>(StmtBatchCalculation::TypeLogicOr, Token::Or),
    pair<StmtBatchCalculation::OperationType, Token::Type>(StmtBatchCalculation::TypeLogicNot, Token::Not),
    pair<StmtBatchCalculation::OperationType, Token::Type>(StmtBatchCalculation::TypeLess, Token::Less),
    pair<StmtBatchCalculation::OperationType, Token::Type>(StmtBatchCalculation::TypeLessEqual, Token::LessEqual),
    pair<StmtBatchCalculation::OperationType, Token::Type>(StmtBatchCalculation::TypeGreater, Token::Greater),
    pair<StmtBatchCalculation::OperationType, Token::Type>(StmtBatchCalculation::TypeGreaterEqual, Token::GreaterEqual),
    pair<StmtBatchCalculation::OperationType, Token::Type>(StmtBatchCalculation::TypeEqual, Token::Equal),
    pair<StmtBatchCalculation::OperationType, Token::Type>(StmtBatchCalculation::TypeNotEqual, Token::NotEqual),
    pair<StmtBatchCalculation::OperationType, Token::Type>(StmtBatchCalculation::TypeMod, Token::Mod),
};

namespace {
    vector<string> indexVarStrListLast;
    string indexStrLast;
}

int ILTransStatement::translateStmtBatchCalculationGenBatchForBegin(const StmtBatchCalculation* stmt, vector<int>& arraySize, vector<string>& indexVarStrList, string& indexStr)
{

    arraySize = stmt->inputs[0].arraySize;
    if (stmt->inputs.size() > 1 && stmt->inputs[1].arraySize.size() > arraySize.size())
    {
        arraySize = stmt->inputs[1].arraySize;
    }


    if (ILTranslator::configMergeBatchCalculation)
    {
        Statement* prevStmt = getPreviousTranslateStatement(stmt);
        if (prevStmt && prevStmt->type == Statement::BatchCalculation)
        {
            StmtBatchCalculation* prevStmtTemp = static_cast<StmtBatchCalculation*>(prevStmt);

            vector<int> lastArraySize = prevStmtTemp->inputs[0].arraySize;
            if (prevStmtTemp->inputs.size() > 1 && prevStmtTemp->inputs[1].arraySize.size() > lastArraySize.size())
            {
                lastArraySize = prevStmtTemp->inputs[1].arraySize;
            }

            if (lastArraySize == arraySize)
            {
                indexStr = indexStrLast;
                indexVarStrList = indexVarStrListLast;

                return 0;
            }

        }
    }


    
    indexVarStrList.clear();
    for(int i = 0; i < arraySize.size(); i++) {
        string indexVarStr = ILParser::getAvailableVariableName("batchIndex", stmt->getDomain());
        indexVarStrList.push_back(indexVarStr);
        ILParser::registerVariableName(indexVarStr, stmt->getDomain());
        statementStr += "int " + indexVarStr + ";\n";
        // ��ʱĬ��outputֻ��һ��
        if (stmt->outputs[0].range.start != -1)
        {
            statementStr += "for (" + indexVarStr + " = " + to_string(stmt->outputs[0].range.start) + "; ";
            statementStr += indexVarStr + " < " + to_string(stmt->outputs[0].range.start + stmt->outputs[0].range.length) + "; " + indexVarStr + "++) {\n";
        }
        else
        {
            statementStr += "for (" + indexVarStr + " = 0; ";
            statementStr += indexVarStr + " < " + to_string(arraySize[i]) + "; " + indexVarStr + "++) {\n";
        }
    }
    indexStr = "";
    for(int i = 0; i < arraySize.size(); i++) {
        indexStr += "[" + indexVarStrList[i] + "]";
    }

    indexStrLast = indexStr;
    indexVarStrListLast = indexVarStrList;

    return 1;
}


int ILTransStatement::translateStmtBatchCalculationGenBatchForBegin(const StmtBatchCalculation* stmt, std::vector<int>& arraySize, std::vector<std::string>& indexVarStrList, std::string& indexStr, std::string interval)
{
    arraySize = stmt->inputs[0].arraySize;
    if (stmt->inputs.size() > 1 && stmt->inputs[1].arraySize.size() > arraySize.size())
    {
        arraySize = stmt->inputs[1].arraySize;
    }
    indexVarStrList.clear();
    for (int i = 0; i < arraySize.size(); i++) {
        string indexVarStr = ILParser::getAvailableVariableName("batchIndex", stmt->getDomain());
        indexVarStrList.push_back(indexVarStr);
        ILParser::registerVariableName(indexVarStr, stmt->getDomain());
        statementStr += "int " + indexVarStr + ";\n";
        // ��ʱĬ��outputֻ��һ��
        if (stmt->outputs[0].range.start != -1)
        {
            statementStr += "for (" + indexVarStr + " = " + to_string(stmt->outputs[0].range.start) + "; ";
            statementStr += indexVarStr + " < " + to_string(stmt->outputs[0].range.start + stmt->outputs[0].range.length) + "; " + indexVarStr + " += " + interval + ") {\n";
        }
        else
        {
            statementStr += "for (" + indexVarStr + " = 0; ";
            statementStr += indexVarStr + " < " + to_string(arraySize[i]) + "; " + indexVarStr + " += " + interval + ") {\n";
        }
    }
    indexStr = "";
    for (int i = 0; i < arraySize.size(); i++) {
        indexStr += "[" + indexVarStrList[i] + "]";
    }
    return 1;
}


int ILTransStatement::translateStmtBatchCalculationGenBatchForEnd(const StmtBatchCalculation* stmt, const std::vector<int>& arraySize)
{
    if(ILTranslator::configMergeBatchCalculation)
    {
        Statement* nextStmt = getNextTranslateStatement(stmt);
        if (nextStmt && nextStmt->type == Statement::BatchCalculation)
        {
            StmtBatchCalculation* nextStmtTemp = static_cast<StmtBatchCalculation*>(nextStmt);

            vector<int> nextArraySize = nextStmtTemp->inputs[0].arraySize;
            if (nextStmtTemp->inputs.size() > 1 && nextStmtTemp->inputs[1].arraySize.size() > nextArraySize.size())
            {
                nextArraySize = nextStmtTemp->inputs[1].arraySize;
            }

            if (nextArraySize == arraySize)
            {
                return 0;
            }
        }

    }

    for(int i = 0; i < arraySize.size(); i++) {
        statementStr += "}\n";
    }
    return 1;
}

int ILTransStatement::translateStmtBatchCalculation(const Statement* stmt)
{
    int res;
    const StmtBatchCalculation* stmtTemp = static_cast<const StmtBatchCalculation*>(stmt);
    if(stmtTemp->operationType >= StmtBatchCalculation::TypeDotMul &&
        stmtTemp->operationType < StmtBatchCalculation::TypeUnknown) {
        res = translateStmtBatchCalculationComplex(stmtTemp);
        return res;
    } else if(stmtTemp->operationType == StmtBatchCalculation::TypeAssign) {
        res = translateStmtBatchCalculationAssign(stmtTemp);
        return res;
    } else if (stmtTemp->operationType == StmtBatchCalculation::TypeSAdd) {
        res = translateStmtBatchCalculationSAdd(stmtTemp);
        return res;
    } else if (stmtTemp->operationType == StmtBatchCalculation::TypeSSub) {
        res = translateStmtBatchCalculationSSub(stmtTemp);
        return res;
    } else if (stmtTemp->operationType == StmtBatchCalculation::TypeSMul) {
        res = translateStmtBatchCalculationSMul(stmtTemp);
        return res;
    } else if(stmtTemp->operationType == StmtBatchCalculation::TypeReadData) {
        res = translateStmtBatchCalculationReadData(stmtTemp);
        return res;
    } else if(stmtTemp->operationType == StmtBatchCalculation::TypeLogicNot) {
        res = translateStmtBatchCalculationLogicNot(stmtTemp);
        return res;
    } else if(stmtTemp->operationType == StmtBatchCalculation::TypeLogicNAnd) {
        res = translateStmtBatchCalculationLogicNAnd(stmtTemp);
        return res;
    } else if(stmtTemp->operationType == StmtBatchCalculation::TypeLogicNOr) {
        res = translateStmtBatchCalculationLogicNOr(stmtTemp);
        return res;
    } else if(stmtTemp->operationType == StmtBatchCalculation::TypeLogicXor) {
        res = translateStmtBatchCalculationLogicXor(stmtTemp);
        return res;
    } else if(stmtTemp->operationType == StmtBatchCalculation::TypeLogicNXor) {
        res = translateStmtBatchCalculationLogicNXor(stmtTemp);
        return res;
    } else if(stmtTemp->operationType == StmtBatchCalculation::TypeSquare) {
        res = translateStmtBatchCalculationSquare(stmtTemp);
        return res;
    } else if(stmtTemp->operationType == StmtBatchCalculation::TypeSqrt) {
        res = translateStmtBatchCalculationSqrt(stmtTemp);
        return res;
    } else if (stmtTemp->operationType == StmtBatchCalculation::TypeRSqrt) {
        res = translateStmtBatchCalculationRSqrt(stmtTemp);
        return res;
    } else if(stmtTemp->operationType == StmtBatchCalculation::TypeExp) {
        res = translateStmtBatchCalculationExp(stmtTemp);
        return res;
    } else if(stmtTemp->operationType == StmtBatchCalculation::TypeAbs) {
        res = translateStmtBatchCalculationAbs(stmtTemp);
        return res;
    } else if (stmtTemp->operationType == StmtBatchCalculation::TypeDAbs) {
        res = translateStmtBatchCalculationDAbs(stmtTemp);
        return res;
    } else if (stmtTemp->operationType == StmtBatchCalculation::TypeSDMul) {
        res = translateStmtBatchCalculationSDMul(stmtTemp);
        return res;
    }
     
    // 先处理通用的二元运算符，然后生成对应的代码    
    string opDataType = ILParser::convertDataType(stmtTemp->dataType);
    if (stmtBatchCalculationOperationToStrMap.find(stmtTemp->operationType) != stmtBatchCalculationOperationToStrMap.end())
    {
        if(stmtTemp->inputs.size() != CONST_NUM_2 || stmtTemp->outputs.size() != 1) {
            return -ILTranslator::ErrorInputOutputCountOfStmtBatchCalculation;
        }
        string input1Type = ILParser::convertDataType(stmtTemp->inputs[0].type);
        string input2Type = ILParser::convertDataType(stmtTemp->inputs[1].type);
        string outputType = ILParser::convertDataType(stmtTemp->outputs[0].type);

        vector<string> indexVarStrList;
        vector<int> arraySize;
        string indexStr;

        res = translateStmtBatchCalculationGenBatchForBegin(stmtTemp, arraySize, indexVarStrList, indexStr);
        if(res < 0)
            return res;

        string outputCalType = BasicTypeCalculator::getExpResultType(input1Type, input2Type, stmtBatchCalculationOperationToTokenTypeMap[stmtTemp->operationType]);
        outputCalType = ILParser::convertDataType(outputCalType);

        statementStr += stmtTemp->outputs[0].name + indexStr + " = ";
        statementStr += outputCalType != outputType ? "(" + outputType + ")(" : "";
        statementStr += input1Type != opDataType ? "(" + opDataType + ")" : "";
        statementStr += stmtTemp->inputs[0].name + (stmtTemp->inputs[0].arraySize.empty() ? "" : indexStr);
        statementStr += " " + stmtBatchCalculationOperationToStrMap[stmtTemp->operationType] + " ";
        statementStr += input2Type != opDataType ? "(" + opDataType + ")" : "";
        if(!stmtTemp->inputs[1].defaultValue)
            statementStr += stmtTemp->inputs[1].name + (stmtTemp->inputs[1].arraySize.empty() ? "" : indexStr);
        else
            statementStr += stmtTemp->inputs[1].defaultValue->expressionStr;
        statementStr += outputCalType != outputType ? ")" : "";
        statementStr += ";\n";
        
        res = translateStmtBatchCalculationGenBatchForEnd(stmtTemp, arraySize);
        if(res < 0)
            return res;
        
    }
    else
    {
        return -ILTranslator::ErrorInvalidBatchCalculationOperationType;
    }
    return 1;
}

int ILTransStatement::translateStmtBatchCalculationLoopUnrolling(std::string& stmtCode)
{
    if(stmtCode.empty())
        return 0;
    int res;
    string stmtStr = stmtCode;
    Statement root;
    ILCCodeParser codeParser;
    res = codeParser.parseCCode(stmtStr, &root);
    if(res < 0)
        return res;
    
    string loopVar;
    int loopCount = 0;
    StmtFor* forStmt = nullptr;

    res = translateStmtBatchCalculationLoopUnrollingCanUnrolling(root, loopVar, loopCount, &forStmt);
    if(res < 0)
        return res;
    if(loopVar.empty() || loopCount == 0)
        return 0;

    
    string loopStmtStr;
    res = translateStmtBatchCalculationLoopUnrollingGetLoopStmtStr(forStmt, loopStmtStr);
    if(res < 0)
        return res;

    string unrollingLoopStmtStr;
    res = translateStmtBatchCalculationLoopUnrollingGetUnrollingStmtStr(loopStmtStr, loopVar, loopCount, unrollingLoopStmtStr);
    if(res < 0)
        return res;

    string beforeCodeStr;
    string afterCodeStr;
    res = translateStmtBatchCalculationLoopUnrollingGetLoopBeforeAndAfterCode(root, forStmt, beforeCodeStr, afterCodeStr);
    if(res < 0)
        return res;

    stmtCode = beforeCodeStr + unrollingLoopStmtStr + afterCodeStr;

    return 1;
}

Statement* ILTransStatement::getPreviousTranslateStatement(const Statement* statement) const
{
    if (iLTranslateSchedule == nullptr)
        return nullptr;

    const vector<ILAnalyzeScheduleNode*>* nodeList = iLTranslateSchedule->getScheduleNodeList();

    //Statement* last = nullptr;
    for (int i = 0; i < nodeList->size(); i++)
    {
        ILAnalyzeScheduleNode* node = (*nodeList)[i];

        if (node->statement == statement && node->prevNode && node->prevNode->statement)
            return node->prevNode->statement;
    }

    return nullptr;
}

Statement* ILTransStatement::getNextTranslateStatement(const Statement* statement) const
{
    if (iLTranslateSchedule == nullptr)
        return nullptr;

    const vector<ILAnalyzeScheduleNode*>* nodeList = iLTranslateSchedule->getScheduleNodeList();

    //Statement* last = nullptr;
    for (int i = 0; i < nodeList->size(); i++)
    {
        ILAnalyzeScheduleNode* node = (*nodeList)[i];

        if (node->statement == statement && node->nextNode && node->nextNode->statement)
            return node->nextNode->statement;
    }

    return nullptr;
}

int ILTransStatement::translateStmtBatchCalculationLoopUnrollingCanUnrolling(const Statement& root,
    std::string& loopVar, int& loopCount, StmtFor** forStmt)
{
    *forStmt = nullptr;
    for(int i = 0; i < root.childStatements.size() - 1; i++)
    {
        if(root.childStatements[i]->type == Statement::LocalVariable ||
            root.childStatements[i + 1]->type == Statement::For){
            *forStmt = (StmtFor*)root.childStatements[i + 1];
        }
    }
    if(!(*forStmt))
        return 0;
    
    
    if((*forStmt)->iterator.size() != 1 || 
        (*forStmt)->iterator[0]->type != Statement::ExpressionWithSemicolon ||
        ((StmtExpression*)((*forStmt)->iterator[0]))->expression == nullptr ||
        !stringEndsWith(((StmtExpression*)(*forStmt)->iterator[0])->expression->expressionStr,"++")) {
        return 0;
    }

    if((*forStmt)->initial.size() != 1 || 
        (*forStmt)->initial[0]->type != Statement::ExpressionWithSemicolon ||
        ((StmtExpression*)(*forStmt)->initial[0])->expression == nullptr ||
        !stringEndsWith(((StmtExpression*)(*forStmt)->initial[0])->expression->expressionStr," = 0")) {
        return 0;
    }
    
    Expression* iteratorExp = ((StmtExpression*)(*forStmt)->iterator[0])->expression;
    Expression* initialExp = ((StmtExpression*)(*forStmt)->initial[0])->expression;
    Expression* condition = (*forStmt)->condition;

    loopVar = iteratorExp->expressionStr.substr(0, iteratorExp->expressionStr.length() - 2);

    if(condition->type != Token::Less || condition->childExpressions.size() != 2 ||
        condition->childExpressions[0]->expressionStr != loopVar ||
        condition->childExpressions[1]->type != Token::Number ||
        stringToInt(condition->childExpressions[1]->expressionStr) > 4) {
        return 0;
    }
    
    loopCount = stringToInt(condition->childExpressions[1]->expressionStr);

    return 1;
}

int ILTransStatement::translateStmtBatchCalculationLoopUnrollingGetLoopBeforeAndAfterCode(const Statement& root,
    const StmtFor* forStmt, std::string& beforeCode, std::string& afterCode)
{
    beforeCode = "";
    afterCode = "";
    bool after = false;
    
    for(int j = 0; j < root.childStatements.size(); j++){

        if(root.childStatements[j] == forStmt || 
            j + 1 < root.childStatements.size() && root.childStatements[j + 1] == forStmt){
            after = true;
            continue;
        }

        string statementStr;
        if(ILTranslator::translateForCBMC)
        {
            ILTransStatementForCBMC iLTransStatement;
            int res = iLTransStatement.translateStatement(root.childStatements[j]);
            if(res < 0)
                return res;
            statementStr = iLTransStatement.statementStr;
        }
        else if(ILTranslator::translateForTCGStateSearch)
        {
            ILTransStatementForTCGStateSearch iLTransStatement;
            int res = iLTransStatement.translateStatement(root.childStatements[j]);
            if(res < 0)
                return res;
            statementStr = iLTransStatement.statementStr;
        }
        else if (ILTranslator::translateForLibFuzzer)
        {
            ILTransStatementForLibFuzzer iLTransStatement;
            int res = iLTransStatement.translateStatement(root.childStatements[j]);
            if (res < 0)
                return res;
            statementStr = iLTransStatement.statementStr;
        }
        else if(ILTranslator::translateForTCGHybrid)
        {
            ILTransStatementForTCGHybrid iLTransStatement;
            int res = iLTransStatement.translateStatement(root.childStatements[j]);
            if(res < 0)
                return res;
            statementStr = iLTransStatement.statementStr;
        }
        else if (ILTranslator::translateForCoverage)
        {
            ILTransStatementForCoverage iLTransStatement;
            int res = iLTransStatement.translateStatement(root.childStatements[j]);
            if (res < 0)
                return res;
            statementStr = iLTransStatement.statementStr;
        }
        else
        {
            ILTransStatement iLTransStatement;
            int res = iLTransStatement.translateStatement(root.childStatements[j]);
            if(res < 0)
                return res;
            statementStr = iLTransStatement.statementStr;
        }

        if(!after)
            beforeCode += statementStr;
        else
            afterCode += statementStr;
    }
    
    return 1;
}

int ILTransStatement::translateStmtBatchCalculationLoopUnrollingGetLoopStmtStr(const StmtFor* forStmt, std::string& retStmtStr)
{
    retStmtStr = "";

    for(int j = 0; j < forStmt->statements.size(); j++){

        string statementStr;
        if(ILTranslator::translateForCBMC)
        {
            ILTransStatementForCBMC iLTransStatement;
            int res = iLTransStatement.translateStatement(forStmt->statements[j]);
            if(res < 0)
                return res;
            statementStr = iLTransStatement.statementStr;
        }
        else if(ILTranslator::translateForTCGStateSearch)
        {
            ILTransStatementForTCGStateSearch iLTransStatement;
            int res = iLTransStatement.translateStatement(forStmt->statements[j]);
            if(res < 0)
                return res;
            statementStr = iLTransStatement.statementStr;
        }
        else if (ILTranslator::translateForLibFuzzer)
        {
            ILTransStatementForLibFuzzer iLTransStatement;
            int res = iLTransStatement.translateStatement(forStmt->statements[j]);
            if (res < 0)
                return res;
            statementStr = iLTransStatement.statementStr;
        }
        else if(ILTranslator::translateForTCGHybrid)
        {
            ILTransStatementForTCGHybrid iLTransStatement;
            int res = iLTransStatement.translateStatement(forStmt->statements[j]);
            if(res < 0)
                return res;
            statementStr = iLTransStatement.statementStr;
        }
        else if (ILTranslator::translateForCoverage)
        {
            ILTransStatementForCoverage iLTransStatement;
            int res = iLTransStatement.translateStatement(forStmt->statements[j]);
            if (res < 0)
                return res;
            statementStr = iLTransStatement.statementStr;
        }
        else
        {
            ILTransStatement iLTransStatement;
            int res = iLTransStatement.translateStatement(forStmt->statements[j]);
            if(res < 0)
                return res;
            statementStr = iLTransStatement.statementStr;
        }

        retStmtStr += statementStr;
    }
    
    
    return 1;
}

int ILTransStatement::translateStmtBatchCalculationLoopUnrollingGetUnrollingStmtStr(
    const std::string loopStmtStr, const std::string& loopVar, int loopCount, std::string& retStmtStr)
{
    retStmtStr = "";
    for(int i = 0; i < loopCount; i++)
    {
        string tempIndex = to_string(i);
        string temp = loopStmtStr;
        size_t start;
        start = temp.find(loopVar);
        while(start != string::npos)
        {
            char beforeChar = ' ';
            char endChar = ' ';
            if(start > 0)
                beforeChar = temp[start - 1];
            if(start + loopVar.length() < loopStmtStr.length() - 1)
                endChar = temp[start + loopVar.length()];
            
            if(beforeChar == '_' || (beforeChar >= '0' && beforeChar <= '9') || 
                (beforeChar >= 'a' && beforeChar <= 'z') || (beforeChar >= 'A' && beforeChar <= 'Z')) {
                start = temp.find(loopVar, start + 1);
                continue;
            }
            if(endChar == '_' || (endChar >= '0' && endChar <= '9') || 
                (endChar >= 'a' && endChar <= 'z') || (endChar >= 'A' && endChar <= 'Z')) {
                start = temp.find(loopVar, start + 1);
                continue;
            }

            temp = temp.replace(start, loopVar.length(), tempIndex);

            start = temp.find(loopVar, start + 1);
        }
        retStmtStr += temp;
    }
    return 1;
}

int ILTransStatement::translateStmtBatchCalculationAssign(const StmtBatchCalculation* stmt)
{

    string opDataType = ILParser::convertDataType(stmt->dataType);

    if(stmt->inputs.size() != 1 || stmt->outputs.size() != 1) {
        return -ILTranslator::ErrorInputOutputCountOfStmtBatchCalculation;
    }
    string inputType = ILParser::convertDataType(stmt->inputs[0].type);
    string outputType = ILParser::convertDataType(stmt->outputs[0].type);
    vector<string> indexindexVarStrList;
    vector<int> arraySize = stmt->outputs[0].arraySize;
    string indexStr;

    int res = translateStmtBatchCalculationGenBatchForBegin(stmt, arraySize, indexindexVarStrList, indexStr);
    if (res < 0)
        return res;

    // for(int i = 0; i < arraySize.size(); i++) {
    //     string indexVarStr = ILParser::getAvailableVariableName("batchIndex", stmt->getDomain());
    //     indexindexVarStrList.push_back(indexVarStr);
    //     ILParser::registerVariableName(indexVarStr, stmt->getDomain());
    //     statementStr += "int " + indexVarStr + ";\n";
    //     statementStr += "for (" + indexVarStr + " = 0; ";
    //     statementStr += indexVarStr + " < " + to_string(arraySize[i]) + "; " + indexVarStr + "++) {\n";
    // }
    // string indexStr;
    // for(int i = 0; i < arraySize.size(); i++) {
    //     indexStr += "[" + indexindexVarStrList[i] + "]";
    // }

    statementStr += stmt->outputs[0].name + indexStr + " = ";
    statementStr += inputType != opDataType ? "(" + opDataType + ")" : "";
    statementStr += stmt->inputs[0].name + (stmt->inputs[0].arraySize.empty() ? "" : indexStr);
    statementStr += ";\n";
    
    res = translateStmtBatchCalculationGenBatchForEnd(stmt, arraySize);
    if (res < 0)
        return res;

    // for(int i = 0; i < arraySize.size(); i++) {
    //     statementStr += "}\n";
    // }

    return 1;
}

int ILTransStatement::translateStmtBatchCalculationReadData(const StmtBatchCalculation* stmt)
{
    string opDataType = ILParser::convertDataType(stmt->dataType);

    if(stmt->inputs.size() != 1 || stmt->outputs.size() != 1) {
        return -ILTranslator::ErrorInputOutputCountOfStmtBatchCalculation;
    }
    string inputType = ILParser::convertDataType(stmt->inputs[0].type);
    string outputType = ILParser::convertDataType(stmt->outputs[0].type);
    vector<string> indexindexVarStrList;
    vector<int> arraySize = stmt->outputs[0].arraySize;
    string indexStr;

    int res = translateStmtBatchCalculationGenBatchForBegin(stmt, arraySize, indexindexVarStrList, indexStr);
    if (res < 0)
        return res;

    // for(int i = 0; i < arraySize.size(); i++) {
    //     string indexVarStr = ILParser::getAvailableVariableName("batchIndex", stmt->getDomain());
    //     indexindexVarStrList.push_back(indexVarStr);
    //     ILParser::registerVariableName(indexVarStr, stmt->getDomain());
    //     statementStr += "int " + indexVarStr + ";\n";
    //     statementStr += "for (" + indexVarStr + " = 0; ";
    //     statementStr += indexVarStr + " < " + to_string(arraySize[i]) + "; " + indexVarStr + "++) {\n";
    // }
    // 
    // for(int i = 0; i < arraySize.size(); i++) {
    //     indexStr += "[" + indexindexVarStrList[i] + "]";
    // }

    statementStr += stmt->outputs[0].name + indexStr + " = ";
    statementStr += inputType != opDataType ? "(" + opDataType + ")" : "";
    statementStr += stmt->inputs[0].name + (stmt->inputs[0].arraySize.empty() ? "" : indexStr);
    statementStr += ";\n";
    
    res = translateStmtBatchCalculationGenBatchForEnd(stmt, arraySize);
    if (res < 0)
        return res;

    // for(int i = 0; i < arraySize.size(); i++) {
    //     statementStr += "}\n";
    // }
    return 1;
}

int ILTransStatement::translateStmtBatchCalculationLogicNot(const StmtBatchCalculation* stmt)
{
    int res;
    string opDataType = ILParser::convertDataType(stmt->dataType);

    if(stmt->inputs.size() != 1 || stmt->outputs.size() != 1) {
        return -ILTranslator::ErrorInputOutputCountOfStmtBatchCalculation;
    }
    string inputType = ILParser::convertDataType(stmt->inputs[0].type);
    string outputType = ILParser::convertDataType(stmt->outputs[0].type);

    vector<string> indexVarStrList;
    vector<int> arraySize;
    string indexStr;

    res = translateStmtBatchCalculationGenBatchForBegin(stmt, arraySize, indexVarStrList, indexStr);
    if(res < 0)
        return res;

    string outputCalType = "bool";
    outputCalType = ILParser::convertDataType(outputCalType);

    statementStr += stmt->outputs[0].name + indexStr + " = ";
    statementStr += outputCalType != outputType ? "(" + outputType + ")(" : "";
    statementStr += "!";
    statementStr += inputType != opDataType ? "(" + opDataType + ")" : "";
    statementStr += stmt->inputs[0].name + (stmt->inputs[0].arraySize.empty() ? "" : indexStr);
    
    statementStr += outputCalType != outputType ? ")" : "";
    statementStr += ";\n";
    
    res = translateStmtBatchCalculationGenBatchForEnd(stmt, arraySize);
    if(res < 0)
        return res;
        
    return 1;
}

int ILTransStatement::translateStmtBatchCalculationLogicNAnd(const StmtBatchCalculation* stmt)
{
    int res;
    string opDataType = ILParser::convertDataType(stmt->dataType);

    if(stmt->inputs.size() != 2 || stmt->outputs.size() != 1) {
        return -ILTranslator::ErrorInputOutputCountOfStmtBatchCalculation;
    }
    string input1Type = ILParser::convertDataType(stmt->inputs[0].type);
    string input2Type = ILParser::convertDataType(stmt->inputs[1].type);
    string outputType = ILParser::convertDataType(stmt->outputs[0].type);

    vector<string> indexVarStrList;
    vector<int> arraySize;
    string indexStr;

    res = translateStmtBatchCalculationGenBatchForBegin(stmt, arraySize, indexVarStrList, indexStr);
    if(res < 0)
        return res;

    string outputCalType = "bool";
    outputCalType = ILParser::convertDataType(outputCalType);

    statementStr += stmt->outputs[0].name + indexStr + " = ";
    statementStr += outputCalType != outputType ? "(" + outputType + ")(" : "";
    statementStr += "!(";
    statementStr += input1Type != opDataType ? "(" + opDataType + ")" : "";
    statementStr += stmt->inputs[0].name + (stmt->inputs[0].arraySize.empty() ? "" : indexStr);
    statementStr += " && ";
    statementStr += input2Type != opDataType ? "(" + opDataType + ")" : "";
    statementStr += stmt->inputs[1].name + (stmt->inputs[1].arraySize.empty() ? "" : indexStr);
    statementStr += ")";
    statementStr += outputCalType != outputType ? ")" : "";
    statementStr += ";\n";
    
    res = translateStmtBatchCalculationGenBatchForEnd(stmt, arraySize);
    if(res < 0)
        return res;
        
    return 1;
}

int ILTransStatement::translateStmtBatchCalculationLogicNOr(const StmtBatchCalculation* stmt)
{
    int res;
    string opDataType = ILParser::convertDataType(stmt->dataType);

    if(stmt->inputs.size() != 2 || stmt->outputs.size() != 1) {
        return -ILTranslator::ErrorInputOutputCountOfStmtBatchCalculation;
    }
    string input1Type = ILParser::convertDataType(stmt->inputs[0].type);
    string input2Type = ILParser::convertDataType(stmt->inputs[1].type);
    string outputType = ILParser::convertDataType(stmt->outputs[0].type);

    vector<string> indexVarStrList;
    vector<int> arraySize;
    string indexStr;

    res = translateStmtBatchCalculationGenBatchForBegin(stmt, arraySize, indexVarStrList, indexStr);
    if(res < 0)
        return res;

    string outputCalType = "bool";
    outputCalType = ILParser::convertDataType(outputCalType);

    statementStr += stmt->outputs[0].name + indexStr + " = ";
    statementStr += outputCalType != outputType ? "(" + outputType + ")(" : "";
    statementStr += "!(";
    statementStr += input1Type != opDataType ? "(" + opDataType + ")" : "";
    statementStr += stmt->inputs[0].name + (stmt->inputs[0].arraySize.empty() ? "" : indexStr);
    statementStr += " || ";
    statementStr += input2Type != opDataType ? "(" + opDataType + ")" : "";
    statementStr += stmt->inputs[1].name + (stmt->inputs[1].arraySize.empty() ? "" : indexStr);
    statementStr += ")";
    statementStr += outputCalType != outputType ? ")" : "";
    statementStr += ";\n";
    
    res = translateStmtBatchCalculationGenBatchForEnd(stmt, arraySize);
    if(res < 0)
        return res;
        
    return 1;
}

int ILTransStatement::translateStmtBatchCalculationLogicXor(const StmtBatchCalculation* stmt)
{
    int res;
    string opDataType = ILParser::convertDataType(stmt->dataType);

    if(stmt->inputs.size() != 2 || stmt->outputs.size() != 1) {
        return -ILTranslator::ErrorInputOutputCountOfStmtBatchCalculation;
    }
    string input1Type = ILParser::convertDataType(stmt->inputs[0].type);
    string input2Type = ILParser::convertDataType(stmt->inputs[1].type);
    string outputType = ILParser::convertDataType(stmt->outputs[0].type);

    vector<string> indexVarStrList;
    vector<int> arraySize;
    string indexStr;

    res = translateStmtBatchCalculationGenBatchForBegin(stmt, arraySize, indexVarStrList, indexStr);
    if(res < 0)
        return res;

    string outputCalType = "bool";
    outputCalType = ILParser::convertDataType(outputCalType);

    statementStr += stmt->outputs[0].name + indexStr + " = ";
    statementStr += outputCalType != outputType ? "(" + outputType + ")(" : "";
    statementStr += "!((";
    statementStr += input1Type != opDataType ? "(" + opDataType + ")" : "";
    statementStr += stmt->inputs[0].name + (stmt->inputs[0].arraySize.empty() ? "" : indexStr);
    statementStr += " == 0)";
    statementStr += " == ";
    statementStr += "(";
    statementStr += input2Type != opDataType ? "(" + opDataType + ")" : "";
    statementStr += stmt->inputs[1].name + (stmt->inputs[1].arraySize.empty() ? "" : indexStr);
    statementStr += " == 0)";
    statementStr += ")";
    statementStr += outputCalType != outputType ? ")" : "";
    statementStr += ";\n";
    
    res = translateStmtBatchCalculationGenBatchForEnd(stmt, arraySize);
    if(res < 0)
        return res;
        
    return 1;
}

int ILTransStatement::translateStmtBatchCalculationLogicNXor(const StmtBatchCalculation* stmt)
{
    int res;
    string opDataType = ILParser::convertDataType(stmt->dataType);

    if(stmt->inputs.size() != 2 || stmt->outputs.size() != 1) {
        return -ILTranslator::ErrorInputOutputCountOfStmtBatchCalculation;
    }
    string input1Type = ILParser::convertDataType(stmt->inputs[0].type);
    string input2Type = ILParser::convertDataType(stmt->inputs[1].type);
    string outputType = ILParser::convertDataType(stmt->outputs[0].type);

    vector<string> indexVarStrList;
    vector<int> arraySize;
    string indexStr;

    res = translateStmtBatchCalculationGenBatchForBegin(stmt, arraySize, indexVarStrList, indexStr);
    if(res < 0)
        return res;

    string outputCalType = "bool";
    outputCalType = ILParser::convertDataType(outputCalType);

    statementStr += stmt->outputs[0].name + indexStr + " = ";
    statementStr += outputCalType != outputType ? "(" + outputType + ")(" : "";
    statementStr += "(";
    statementStr += input1Type != opDataType ? "(" + opDataType + ")" : "";
    statementStr += stmt->inputs[0].name + (stmt->inputs[0].arraySize.empty() ? "" : indexStr);
    statementStr += " == 0)";
    statementStr += " == ";
    statementStr += "(";
    statementStr += input2Type != opDataType ? "(" + opDataType + ")" : "";
    statementStr += stmt->inputs[1].name + (stmt->inputs[1].arraySize.empty() ? "" : indexStr);
    statementStr += " == 0)";
    statementStr += outputCalType != outputType ? ")" : "";
    statementStr += ";\n";
    
    res = translateStmtBatchCalculationGenBatchForEnd(stmt, arraySize);
    if(res < 0)
        return res;
        
    return 1;
}

int ILTransStatement::translateStmtBatchCalculationSquare(const StmtBatchCalculation* stmt)
{
    int res;
    string opDataType = ILParser::convertDataType(stmt->dataType);

    if(stmt->inputs.size() != 1 || stmt->outputs.size() != 1) {
        return -ILTranslator::ErrorInputOutputCountOfStmtBatchCalculation;
    }
    string input1Type = ILParser::convertDataType(stmt->inputs[0].type);
    string outputType = ILParser::convertDataType(stmt->outputs[0].type);

    vector<string> indexVarStrList;
    vector<int> arraySize;
    string indexStr;

    res = translateStmtBatchCalculationGenBatchForBegin(stmt, arraySize, indexVarStrList, indexStr);
    if(res < 0)
        return res;

    string outputCalType = BasicTypeCalculator::getExpResultType(input1Type, input1Type, Token::Mul);
    outputCalType = ILParser::convertDataType(outputCalType);

    statementStr += stmt->outputs[0].name + indexStr + " = ";
    statementStr += outputCalType != outputType ? "(" + outputType + ")(" : "";
    statementStr += input1Type != opDataType ? "(" + opDataType + ")" : "";
    statementStr += stmt->inputs[0].name + (stmt->inputs[0].arraySize.empty() ? "" : indexStr);
    statementStr += " * ";
    statementStr += input1Type != opDataType ? "(" + opDataType + ")" : "";
    statementStr += stmt->inputs[0].name + (stmt->inputs[0].arraySize.empty() ? "" : indexStr);
    
    statementStr += outputCalType != outputType ? ")" : "";
    statementStr += ";\n";
    
    res = translateStmtBatchCalculationGenBatchForEnd(stmt, arraySize);
    if(res < 0)
        return res;
        
    return 1;
}

int ILTransStatement::translateStmtBatchCalculationSqrt(const StmtBatchCalculation* stmt)
{
    int res;
    if (stmt->inputs.size() != 1 || stmt->outputs.size() != 1) {
        return -ILTranslator::ErrorInputOutputCountOfStmtBatchCalculation;
    }
    string opDataType = ILParser::convertDataType(stmt->dataType);
    string input1Type = ILParser::convertDataType(stmt->inputs[0].type);
    string outputType = ILParser::convertDataType(stmt->outputs[0].type);

    vector<string> indexVarStrList;
    vector<int> arraySize;
    string indexStr;

    // Ӳ��ָ���Ż����?    if (ILTranslator::translateForC)
    {
        if (ILTranslatorForC::configTargetDevice == "Tricore")
        {
            res = translateStmtBatchCalculationGenBatchForBegin(stmt, arraySize, indexVarStrList, indexStr);
            if (res < 0)
                return res;

            string outputCalType = BasicTypeCalculator::getExpResultType(input1Type, input1Type, Token::Mul);
            outputCalType = ILParser::convertDataType(outputCalType);

            statementStr += "rqseed_f (&" + stmt->inputs[0].name + (stmt->inputs[0].arraySize.empty() ? "" : indexStr)
				+ ", &" + stmt->outputs[0].name + indexStr + ");\n";
            res = translateStmtBatchCalculationGenBatchForEnd(stmt, arraySize);
            return res;
        }
        else if (ILTranslatorForC::configTargetDevice == "ARMv8")
        {
            if (stmt->outputs[0].type == "f32")
            {
                // Ĭ��float32x4_t
                res = translateStmtBatchCalculationGenBatchForBegin(stmt, arraySize, indexVarStrList, indexStr, "4");
                if (res < 0)
                    return res;
                statementStr += "float32x4_t " + stmt->inputs[0].name + "_vec = vld1q_f32(&" + stmt->inputs[0].name
                    + indexStr + ");\n";
                statementStr += "float32x4_t " + stmt->outputs[0].name + "_estimate = vrsqrteq_f32(" + stmt->inputs[0].name
                    + "_vec);\n";
                statementStr += stmt->outputs[0].name + "_estimate = vmulq_f32(" + stmt->outputs[0].name
                    + "_estimate, vrsqrtsq_f32(" + stmt->inputs[0].name + "_vec, " + "vmulq_f32(" + stmt->outputs[0].name + "_estimate, " + stmt->outputs[0].name
                    + "_estimate)));\n";
                statementStr += stmt->outputs[0].name + "_estimate = vmulq_f32(" + stmt->outputs[0].name
                    + "_estimate, vrsqrtsq_f32(" + stmt->inputs[0].name + "_vec, " + "vmulq_f32(" + stmt->outputs[0].name + "_estimate, " + stmt->outputs[0].name
                    + "_estimate)));\n";
                statementStr += "vst1q_f32(&" + stmt->outputs[0].name + indexStr
                    + ", vmulq_f32(" + stmt->outputs[0].name + "_estimate, " + stmt->inputs[0].name + "_vec));\n";
                res = translateStmtBatchCalculationGenBatchForEnd(stmt, arraySize);
                return res;
            }
        }
        else if (ILTranslatorForC::configTargetDevice == "RISC-V")
        {
            res = translateStmtBatchCalculationGenBatchForBegin(stmt, arraySize, indexVarStrList, indexStr, "4");
            if (res < 0)
                return res;
            if (stmt->outputs[0].type == "f32")
            {
                statementStr += "vfsqrt(&" + stmt->inputs[0].name + indexStr + ", &"  + stmt->outputs[0].name + indexStr + ");\n";
            }
            res = translateStmtBatchCalculationGenBatchForEnd(stmt, arraySize);
            return res;
        }
    }

    // �������?    res = translateStmtBatchCalculationGenBatchForBegin(stmt, arraySize, indexVarStrList, indexStr);
    if(res < 0)
        return res;

    string outputCalType = BasicTypeCalculator::getExpResultType(input1Type, input1Type, Token::Mul);
    outputCalType = ILParser::convertDataType(outputCalType);

    statementStr += stmt->outputs[0].name + indexStr + " = ";
    statementStr += outputCalType != outputType ? "(" + outputType + ")(" : "";
    statementStr += "sqrt(";
    statementStr += input1Type != opDataType ? "(" + opDataType + ")" : "";
    statementStr += stmt->inputs[0].name + (stmt->inputs[0].arraySize.empty() ? "" : indexStr);
    statementStr += ")";
    statementStr += outputCalType != outputType ? ")" : "";
    statementStr += ";\n";
    
    res = translateStmtBatchCalculationGenBatchForEnd(stmt, arraySize);
    if(res < 0)
        return res;
        
    return 1;
}

int ILTransStatement::translateStmtBatchCalculationRSqrt(const StmtBatchCalculation* stmt)
{
    int res;
    string opDataType = ILParser::convertDataType(stmt->dataType);

    if (stmt->inputs.size() != 1 || stmt->outputs.size() != 1) {
        return -ILTranslator::ErrorInputOutputCountOfStmtBatchCalculation;
    }
    string input1Type = ILParser::convertDataType(stmt->inputs[0].type);
    string outputType = ILParser::convertDataType(stmt->outputs[0].type);

    vector<string> indexVarStrList;
    vector<int> arraySize;
    string indexStr;

    if (ILTranslator::translateForC)
    {
        if (ILTranslatorForC::configTargetDevice == "Tricore")
        {
            res = translateStmtBatchCalculationGenBatchForBegin(stmt, arraySize, indexVarStrList, indexStr);
            if (res < 0)
                return res;

            string outputCalType = BasicTypeCalculator::getExpResultType(input1Type, input1Type, Token::Mul);
            outputCalType = ILParser::convertDataType(outputCalType);

            statementStr += "qseed_f (&" + stmt->inputs[0].name + (stmt->inputs[0].arraySize.empty() ? "" : indexStr)
                + ", &" + stmt->outputs[0].name + indexStr + ");\n";
            res = translateStmtBatchCalculationGenBatchForEnd(stmt, arraySize);
            return res;
        }
        else if (ILTranslatorForC::configTargetDevice == "ARMv8")
        {
            if (stmt->outputs[0].type == "f32")
            {
				// Ĭ��float32x4_t
                res = translateStmtBatchCalculationGenBatchForBegin(stmt, arraySize, indexVarStrList, indexStr, "4");
                if (res < 0)
                    return res;
                statementStr += "float32x4_t " + stmt->inputs[0].name + "_vec = vld1q_f32(&" + stmt->outputs[0].name
                    + indexStr + ");\n";
                statementStr += "float32x4_t " + stmt->outputs[0].name + "_estimate = vrsqrteq_f32(" + stmt->inputs[0].name
                    + "_vec);\n";
                statementStr += stmt->outputs[0].name + "_estimate = vmulq_f32(" + stmt->outputs[0].name
                    + "_estimate, vrsqrtsq_f32(" + stmt->inputs[0].name + "_vec, " + "vmulq_f32(" + stmt->outputs[0].name + "_estimate, " + stmt->outputs[0].name
                    + "_estimate)));\n"; 
                statementStr += stmt->outputs[0].name + "_estimate = vmulq_f32(" + stmt->outputs[0].name
                    + "_estimate, vrsqrtsq_f32(" + stmt->inputs[0].name + "_vec, " + "vmulq_f32(" + stmt->outputs[0].name + "_estimate, " + stmt->outputs[0].name
                    + "_estimate)));\n";
                statementStr += "vst1q_f32(&" + stmt->outputs[0].name + indexStr
                    + ", " + stmt->outputs[0].name + "_estimate);\n";
                res = translateStmtBatchCalculationGenBatchForEnd(stmt, arraySize);
                return res;
            }
        }
        else if (ILTranslatorForC::configTargetDevice == "RISC-V")
        {
            res = translateStmtBatchCalculationGenBatchForBegin(stmt, arraySize, indexVarStrList, indexStr, "4");
            if (res < 0)
                return res;
            if (stmt->outputs[0].type == "f32")
            {
                statementStr += "vfrsqrt(&" + stmt->inputs[0].name + indexStr + ", &" + stmt->outputs[0].name + indexStr + ");\n";
            }
            res = translateStmtBatchCalculationGenBatchForEnd(stmt, arraySize);
            return res;
        }
    }


    res = translateStmtBatchCalculationGenBatchForBegin(stmt, arraySize, indexVarStrList, indexStr);
    if (res < 0)
        return res;

    string outputCalType = BasicTypeCalculator::getExpResultType(input1Type, input1Type, Token::Mul);
    outputCalType = ILParser::convertDataType(outputCalType);

    statementStr += stmt->outputs[0].name + indexStr + " = ";
    statementStr += outputCalType != outputType ? "(" + outputType + ")(" : "";
    statementStr += "1.0 / sqrt(";
    statementStr += input1Type != opDataType ? "(" + opDataType + ")" : "";
    statementStr += stmt->inputs[0].name + (stmt->inputs[0].arraySize.empty() ? "" : indexStr);
    statementStr += ")";
    statementStr += outputCalType != outputType ? ")" : "";
    statementStr += ";\n";

    res = translateStmtBatchCalculationGenBatchForEnd(stmt, arraySize);
    if (res < 0)
        return res;

    return 1;
}

int ILTransStatement::translateStmtBatchCalculationExp(const StmtBatchCalculation* stmt)
{
    int res;
    string opDataType = ILParser::convertDataType(stmt->dataType);

    if(stmt->inputs.size() != 1 || stmt->outputs.size() != 1) {
        return -ILTranslator::ErrorInputOutputCountOfStmtBatchCalculation;
    }
    string input1Type = ILParser::convertDataType(stmt->inputs[0].type);
    string outputType = ILParser::convertDataType(stmt->outputs[0].type);

    vector<string> indexVarStrList;
    vector<int> arraySize;
    string indexStr;

    res = translateStmtBatchCalculationGenBatchForBegin(stmt, arraySize, indexVarStrList, indexStr);
    if(res < 0)
        return res;

    string outputCalType = BasicTypeCalculator::getExpResultType(input1Type, input1Type, Token::Mul);
    outputCalType = ILParser::convertDataType(outputCalType);

    statementStr += stmt->outputs[0].name + indexStr + " = ";
    statementStr += outputCalType != outputType ? "(" + outputType + ")(" : "";
    statementStr += "exp(";
    statementStr += input1Type != opDataType ? "(" + opDataType + ")" : "";
    statementStr += stmt->inputs[0].name + (stmt->inputs[0].arraySize.empty() ? "" : indexStr);
    statementStr += ")";
    statementStr += outputCalType != outputType ? ")" : "";
    statementStr += ";\n";
    
    res = translateStmtBatchCalculationGenBatchForEnd(stmt, arraySize);
    if(res < 0)
        return res;
        
    return 1;
}

int ILTransStatement::translateStmtBatchCalculationSAdd(const StmtBatchCalculation* stmt)
{
    int res;
    string opDataType = ILParser::convertDataType(stmt->dataType);

    if (stmt->inputs.size() != 2 || stmt->outputs.size() != 1) {
        return -ILTranslator::ErrorInputOutputCountOfStmtBatchCalculation;
    }
    string input1Type = ILParser::convertDataType(stmt->inputs[0].type);
    string input2Type = ILParser::convertDataType(stmt->inputs[1].type);
    string outputType = ILParser::convertDataType(stmt->outputs[0].type);

    vector<string> indexVarStrList;
    vector<int> arraySize;
    string indexStr;

    if (ILTranslator::translateForC)
    {
        string input1TempVar;
        string input2TempVar;
        string outputTempVar;
        if (ILTranslatorForC::configPackageParameter)
        {
            input1TempVar = stringStartsWith(stmt->inputs[0].name, "self->") ? stmt->inputs[0].name.substr(6) + "_vec" : stmt->inputs[0].name + "_vec";
			input2TempVar = stringStartsWith(stmt->inputs[1].name, "self->") ? stmt->inputs[1].name.substr(6) + "_vec" : stmt->inputs[1].name + "_vec";
			outputTempVar = stringStartsWith(stmt->outputs[0].name, "self->") ? stmt->outputs[0].name.substr(6) + "_vec" : stmt->outputs[0].name + "_vec";
        }
        
        if (ILTranslatorForC::configTargetDevice == "Tricore")
        {
            res = translateStmtBatchCalculationGenBatchForBegin(stmt, arraySize, indexVarStrList, indexStr);
            if (res < 0)
                return res;
            if (stmt->outputs[0].type == "i32")
            {
                statementStr += "adds_i (&(" + stmt->inputs[0].name + (stmt->inputs[0].arraySize.empty() ? "" : indexStr) + "), "
                    + "&(" + stmt->inputs[1].name + (stmt->inputs[1].arraySize.empty() ? "" : indexStr) + "), "
					+ "&(" + stmt->outputs[0].name + indexStr + "));\n";
            }
            else {
                statementStr += "adds_u (&(" + stmt->inputs[0].name + (stmt->inputs[0].arraySize.empty() ? "" : indexStr) + "), "
                    + "&(" + stmt->inputs[1].name + (stmt->inputs[1].arraySize.empty() ? "" : indexStr) + "), "
                    + "&(" + stmt->outputs[0].name + indexStr + "));\n";
            }
            res = translateStmtBatchCalculationGenBatchForEnd(stmt, arraySize);
            return res;
        }
        else if (ILTranslatorForC::configTargetDevice == "RISC-V")
        {
            res = translateStmtBatchCalculationGenBatchForBegin(stmt, arraySize, indexVarStrList, indexStr, "4");
            if (res < 0)
                return res;
            if (stmt->outputs[0].type == "i32")
            {
				statementStr += "vsadd_vv(&(" + stmt->inputs[0].name + indexStr + "), &(" + stmt->inputs[1].name  + indexStr + "), &(" + stmt->outputs[0].name + indexStr + "));\n";
			}
            res = translateStmtBatchCalculationGenBatchForEnd(stmt, arraySize);
            return res;
        }
        else if (ILTranslatorForC::configTargetDevice == "ARMv8")
        {
            // Ĭ����4λ�������Ĵ���
            // ��Ĭ�ϴ���i32��u32
            res = translateStmtBatchCalculationGenBatchForBegin(stmt, arraySize, indexVarStrList, indexStr, "4");
            if (res < 0)
                return res;
            if (stmt->outputs[0].type == "i32")
            {
                    statementStr += "int32x4_t " + input1TempVar + " = vld1q_s32(&(" + stmt->inputs[0].name
                        + indexStr + "));\n";
                    statementStr += "int32x4_t " + input2TempVar + " = vld1q_s32(&(" + stmt->inputs[1].name
                        + indexStr + "));\n";
                    statementStr += "int32x4_t " + outputTempVar + " = vqaddq_s32(" + input1TempVar
                        + ", " + input2TempVar + ");\n";
                    statementStr += "vst1q_s32(&(" + stmt->outputs[0].name + indexStr
                        + "), " + outputTempVar + ");\n";
            }
            else if (stmt->outputs[0].type == "u32") {
                statementStr += "uint32x4_t " + input1TempVar + " = vld1q_u32(&(" + stmt->inputs[0].name
                    + indexStr + "));\n";
                statementStr += "uint32x4_t " + input2TempVar + " = vld1q_u32(&(" + stmt->inputs[1].name
                    + indexStr + "));\n";
                statementStr += "uint32x4_t " + outputTempVar + " = vqaddq_u32(" + input1TempVar
                    + ", " + input2TempVar + ");\n";
                statementStr += "vst1q_u32(" + stmt->outputs[0].name + indexStr
                    + ", " + outputTempVar + ");\n";
            }
            res = translateStmtBatchCalculationGenBatchForEnd(stmt, arraySize);
            return res;
        }

    }

    // ��������ķ���?    res = translateStmtBatchCalculationGenBatchForBegin(stmt, arraySize, indexVarStrList, indexStr);
    if (res < 0)
        return res;

    string outputCalType = BasicTypeCalculator::getExpResultType(input1Type, input1Type, Token::Mul);
    outputCalType = ILParser::convertDataType(outputCalType);

    // ���SADD����Ĭ���� int ���� unsigned int
    string tempVar = stmt->outputs[0].name + "_temp";
    statementStr += stmt->outputs[0].type == "i32" ? "long long " : "unsigned long long ";
    statementStr += tempVar + ";\n";
    statementStr += tempVar + " = (" + (stmt->outputs[0].type == "i32" ? "long long)" : "unsigned long long)") +
         stmt->inputs[0].name + (stmt->inputs[0].arraySize.empty() ? "" : indexStr) + " + " +
         stmt->inputs[1].name + (stmt->inputs[1].arraySize.empty() ? "" : indexStr) + ";\n";

    if (stmt->outputs[0].type == "i32")
    {
		statementStr += "if ( " + tempVar + " > 2147483647LL) { \n";
		statementStr += tempVar + " = 2147483647LL;\n";
		statementStr += "} else if (" + tempVar + " < -2147483648LL) { \n";
		statementStr += tempVar + " = -2147483648LL;}\n";
		statementStr += stmt->outputs[0].name + indexStr + " = " + tempVar + ";\n";
    }
    else
    {
        statementStr += "if ( " + tempVar + " > 4294967295ULL) { \n";
        statementStr += tempVar + " = 4294967295ULL;}\n";
        statementStr += stmt->outputs[0].name + indexStr + " = " + tempVar + ";\n";
    }

    res = translateStmtBatchCalculationGenBatchForEnd(stmt, arraySize);
    if (res < 0)
        return res;

    return 1;
}

int ILTransStatement::translateStmtBatchCalculationSSub(const StmtBatchCalculation* stmt)
{
    int res;
    string opDataType = ILParser::convertDataType(stmt->dataType);

    if (stmt->inputs.size() != 2 || stmt->outputs.size() != 1) {
        return -ILTranslator::ErrorInputOutputCountOfStmtBatchCalculation;
    }
    string input1Type = ILParser::convertDataType(stmt->inputs[0].type);
    string input2Type = ILParser::convertDataType(stmt->inputs[1].type);
    string outputType = ILParser::convertDataType(stmt->outputs[0].type);

    vector<string> indexVarStrList;
    vector<int> arraySize;
    string indexStr;

    if (ILTranslator::translateForC)
    {
        string input1TempVar;
        string input2TempVar;
        string outputTempVar;
        if (ILTranslatorForC::configPackageParameter)
        {
            input1TempVar = stringStartsWith(stmt->inputs[0].name, "self->") ? stmt->inputs[0].name.substr(6) + "_vec" : stmt->inputs[0].name + "_vec";
            input2TempVar = stringStartsWith(stmt->inputs[1].name, "self->") ? stmt->inputs[1].name.substr(6) + "_vec" : stmt->inputs[1].name + "_vec";
            outputTempVar = stringStartsWith(stmt->outputs[0].name, "self->") ? stmt->outputs[0].name.substr(6) + "_vec" : stmt->outputs[0].name + "_vec";
        }
        if (ILTranslatorForC::configTargetDevice == "Tricore")
        {
            res = translateStmtBatchCalculationGenBatchForBegin(stmt, arraySize, indexVarStrList, indexStr);
            if (res < 0)
                return res;
            if (stmt->outputs[0].type == "i32")
            {
                statementStr += "subs_i (&(" + stmt->inputs[0].name + (stmt->inputs[0].arraySize.empty() ? "" : indexStr) + "), "
                    + "&(" + stmt->inputs[1].name + (stmt->inputs[1].arraySize.empty() ? "" : indexStr) + "), "
                    + "&(" + stmt->outputs[0].name + indexStr + "));\n";
            }
            else {
                statementStr += "subs_u (&(" + stmt->inputs[0].name + (stmt->inputs[0].arraySize.empty() ? "" : indexStr) + "), "
                    + "&(" + stmt->inputs[1].name + (stmt->inputs[1].arraySize.empty() ? "" : indexStr) + "), "
                    + "&(" + stmt->outputs[0].name + indexStr + "));\n";
            }
            res = translateStmtBatchCalculationGenBatchForEnd(stmt, arraySize);
            return res;
        }
        else if (ILTranslatorForC::configTargetDevice == "RISC-V")
        {
            res = translateStmtBatchCalculationGenBatchForBegin(stmt, arraySize, indexVarStrList, indexStr, "4");
            if (res < 0)
                return res;
            if (stmt->outputs[0].type == "i32")
            {
                statementStr += "vssub_vv(&(" + stmt->inputs[0].name + indexStr + "), &(" + stmt->inputs[1].name + indexStr + "), &(" + stmt->outputs[0].name + indexStr + "));\n";
            }
            res = translateStmtBatchCalculationGenBatchForEnd(stmt, arraySize);
            return res;
        }
        else if (ILTranslatorForC::configTargetDevice == "ARMv8")
        {
            // Ĭ����4λ�������Ĵ���
            // ��Ĭ�ϴ���i32��u32
            res = translateStmtBatchCalculationGenBatchForBegin(stmt, arraySize, indexVarStrList, indexStr, "4");
            if (res < 0)
                return res;
            if (stmt->outputs[0].type == "i32")
            {
                statementStr += "int32x4_t " + input1TempVar + " = vld1q_s32(&(" + stmt->inputs[0].name
                    + indexStr + "));\n";
                statementStr += "int32x4_t " + input2TempVar + " = vld1q_s32(&(" + stmt->inputs[1].name
                    + indexStr + "));\n";
                statementStr += "int32x4_t " + outputTempVar + " = vqsubq_s32(" + input1TempVar
                    + ", " + input2TempVar + "" + ");\n";
                statementStr += "vst1q_s32(&(" + stmt->outputs[0].name + indexStr
                    + "), " + outputTempVar + ");\n";
            }
            else if (stmt->outputs[0].type == "u32") {
                statementStr += "uint32x4_t " + input1TempVar + " = vld1q_u32(&(" + stmt->inputs[0].name
                    + indexStr + "));\n";
                statementStr += "uint32x4_t " + input2TempVar + " = vld1q_u32(&(" + stmt->inputs[1].name
                    + indexStr + "));\n";
                statementStr += "uint32x4_t " + outputTempVar + " = vqsubq_u32(" + input1TempVar
                    + ", " + input2TempVar + ");\n";
                statementStr += "vst1q_u32(" + stmt->outputs[0].name + indexStr
                    + ", " + outputTempVar + ");\n";
            }
            res = translateStmtBatchCalculationGenBatchForEnd(stmt, arraySize);
            return res;
        }
    }

    // ��������ķ���?    res = translateStmtBatchCalculationGenBatchForBegin(stmt, arraySize, indexVarStrList, indexStr);
    if (res < 0)
        return res;

    string outputCalType = BasicTypeCalculator::getExpResultType(input1Type, input1Type, Token::Mul);
    outputCalType = ILParser::convertDataType(outputCalType);

    // ���SADD����Ĭ���� int ���� unsigned int
    string tempVar = stmt->outputs[0].name + "_temp";
    statementStr += stmt->outputs[0].type == "i32" ? "long long " : "unsigned long long ";
    statementStr += tempVar + ";\n";

    statementStr += tempVar + " = (" + (stmt->outputs[0].type == "i32" ? "long long)" : "unsigned long long)")
        + stmt->inputs[0].name + (stmt->inputs[0].arraySize.empty() ? "" : indexStr) + " - "
        + stmt->inputs[1].name + (stmt->inputs[1].arraySize.empty() ? "" : indexStr) + ";\n";

    if (stmt->outputs[0].type == "i32")
    {
        statementStr += "if ( " + tempVar + " > 2147483647LL) { \n";
        statementStr += tempVar + " = 2147483647LL;\n";
        statementStr += "} else if (" + tempVar + " < -2147483648LL) { \n";
        statementStr += tempVar + " = -2147483648LL;}\n";
        statementStr += stmt->outputs[0].name + indexStr + " = " + tempVar + ";\n";
    }
    else
    {
        statementStr += "if ( " + tempVar + " > 4294967295ULL) { \n";
        statementStr += tempVar + " = 4294967295ULL;}\n";
        statementStr += stmt->outputs[0].name + indexStr + " = " + tempVar + ";\n";
    }

    res = translateStmtBatchCalculationGenBatchForEnd(stmt, arraySize);
    if (res < 0)
        return res;

    return 1;
}

int ILTransStatement::translateStmtBatchCalculationSMul(const StmtBatchCalculation* stmt)
{
    int res;
    string opDataType = ILParser::convertDataType(stmt->dataType);

    if (stmt->inputs.size() != 2 || stmt->outputs.size() != 1) {
        return -ILTranslator::ErrorInputOutputCountOfStmtBatchCalculation;
    }
    string input1Type = ILParser::convertDataType(stmt->inputs[0].type);
    string input2Type = ILParser::convertDataType(stmt->inputs[1].type);
    string outputType = ILParser::convertDataType(stmt->outputs[0].type);

    vector<string> indexVarStrList;
    vector<int> arraySize;
    string indexStr;

    if (ILTranslator::translateForC)
    {
        if (ILTranslatorForC::configTargetDevice == "Tricore")
        {
            res = translateStmtBatchCalculationGenBatchForBegin(stmt, arraySize, indexVarStrList, indexStr);
            if (res < 0)
                return res;
            if (stmt->outputs[0].type == "i32")
            {
                statementStr += "muls_i (&(" + stmt->inputs[0].name + (stmt->inputs[0].arraySize.empty() ? "" : indexStr) + "), "
                    + "&(" + stmt->inputs[1].name + (stmt->inputs[1].arraySize.empty() ? "" : indexStr) + "), "
                    + "&(" + stmt->outputs[0].name + indexStr + "));\n";
            }
            else {
                statementStr += "muls_u (&(" + stmt->inputs[0].name + (stmt->inputs[0].arraySize.empty() ? "" : indexStr) + "), "
                    + "&(" + stmt->inputs[1].name + (stmt->inputs[1].arraySize.empty() ? "" : indexStr) + "), "
                    + "&(" + stmt->outputs[0].name + indexStr + "));\n";
            }
            res = translateStmtBatchCalculationGenBatchForEnd(stmt, arraySize);
            return res;
        }
        //else if (ILTranslatorForC::configTargetDevice == "RISC-V")
        //{
        //    res = translateStmtBatchCalculationGenBatchForBegin(stmt, arraySize, indexVarStrList, indexStr, "4");
        //    if (res < 0)
        //        return res;
        //    if (stmt->outputs[0].type == "i32")
        //    {
        //        statementStr += "vsmul_vv(&(" + stmt->inputs[0].name + indexStr + "), &(" + stmt->inputs[1].name + indexStr + "), &(" + stmt->outputs[0].name + indexStr + "));\n";
        //    }
        //    res = translateStmtBatchCalculationGenBatchForEnd(arraySize);
        //    return res;
        //}
    }

    // ��������ķ���?    res = translateStmtBatchCalculationGenBatchForBegin(stmt, arraySize, indexVarStrList, indexStr);
    if (res < 0)
        return res;

    string outputCalType = BasicTypeCalculator::getExpResultType(input1Type, input1Type, Token::Mul);
    outputCalType = ILParser::convertDataType(outputCalType);

    // ���SADD����Ĭ���� int ���� unsigned int
    string tempVar = stmt->outputs[0].name + "_temp";
    statementStr += stmt->outputs[0].type == "i32" ? "long long " : "unsigned long long ";
    statementStr += tempVar + ";\n";

    statementStr += tempVar + " = (" + (stmt->outputs[0].type == "i32" ? "long long)" : "unsigned long long)")
        + stmt->inputs[0].name + (stmt->inputs[0].arraySize.empty() ? "" : indexStr) + " * "
        + stmt->inputs[1].name + (stmt->inputs[1].arraySize.empty() ? "" : indexStr) + ";\n";

    if (stmt->outputs[0].type == "i32")
    {
        statementStr += "if ( " + tempVar + " > 2147483647LL) { \n";
        statementStr += tempVar + " = 2147483647LL;\n";
        statementStr += "} else if (" + tempVar + " < -2147483648LL) { \n";
        statementStr += tempVar + " = -2147483648LL;}\n";
        statementStr += stmt->outputs[0].name + indexStr + " = " + tempVar + ";\n";
    }
    else
    {
        statementStr += "if ( " + tempVar + " > 4294967295ULL) { \n";
        statementStr += tempVar + " = 4294967295ULL;}\n";
        statementStr += stmt->outputs[0].name + indexStr + " = " + tempVar + ";\n";
    }

    res = translateStmtBatchCalculationGenBatchForEnd(stmt, arraySize);
    if (res < 0)
        return res;

    return 1;
}

int ILTransStatement::translateStmtBatchCalculationAbs(const StmtBatchCalculation* stmt)
{
    int res;
    string opDataType = ILParser::convertDataType(stmt->dataType);

    if(stmt->inputs.size() != 1 || stmt->outputs.size() != 1) {
        return -ILTranslator::ErrorInputOutputCountOfStmtBatchCalculation;
    }
    string input1Type = ILParser::convertDataType(stmt->inputs[0].type);
    string outputType = ILParser::convertDataType(stmt->outputs[0].type);

    vector<string> indexVarStrList;
    vector<int> arraySize;
    string indexStr;

    res = translateStmtBatchCalculationGenBatchForBegin(stmt, arraySize, indexVarStrList, indexStr);
    if(res < 0)
        return res;

    string outputCalType = BasicTypeCalculator::getExpResultType(input1Type, input1Type, Token::Mul);
    outputCalType = ILParser::convertDataType(outputCalType);

    statementStr += stmt->outputs[0].name + indexStr + " = ";
    statementStr += outputCalType != outputType ? "(" + outputType + ")(" : "";
    if(opDataType == "f32" || opDataType == "f64")
        statementStr += "fabs(";
    else
        statementStr += "abs(";
    statementStr += input1Type != opDataType ? "(" + opDataType + ")" : "";
    statementStr += stmt->inputs[0].name + (stmt->inputs[0].arraySize.empty() ? "" : indexStr);
    statementStr += ")";
    statementStr += outputCalType != outputType ? ")" : "";
    statementStr += ";\n";
    
    res = translateStmtBatchCalculationGenBatchForEnd(stmt, arraySize);
    if(res < 0)
        return res;
        
    return 1;
}

int ILTransStatement::translateStmtBatchCalculationDAbs(const StmtBatchCalculation* stmt)
{
    int res;
    string opDataType = ILParser::convertDataType(stmt->dataType);

    if (stmt->inputs.size() != 2 || stmt->outputs.size() != 1) {
        return -ILTranslator::ErrorInputOutputCountOfStmtBatchCalculation;
    }
    string input1Type = ILParser::convertDataType(stmt->inputs[0].type);
    string outputType = ILParser::convertDataType(stmt->outputs[0].type);

    vector<string> indexVarStrList;
    vector<int> arraySize;
    string indexStr;

    if (ILTranslator::translateForC)
    {
        string input1TempVar;
        string input2TempVar;
        string outputTempVar;
        if (ILTranslatorForC::configPackageParameter)
        {
            input1TempVar = stringStartsWith(stmt->inputs[0].name, "self->") ? stmt->inputs[0].name.substr(6) + "_vec" : stmt->inputs[0].name + "_vec";
            input2TempVar = stringStartsWith(stmt->inputs[1].name, "self->") ? stmt->inputs[1].name.substr(6) + "_vec" : stmt->inputs[1].name + "_vec";
            outputTempVar = stringStartsWith(stmt->outputs[0].name, "self->") ? stmt->outputs[0].name.substr(6) + "_vec" : stmt->outputs[0].name + "_vec";
        }
        if (ILTranslatorForC::configTargetDevice == "ARMv8")
        {
            // Ĭ����4λ�������Ĵ���
            // ��Ĭ�ϴ���i32��u32
            res = translateStmtBatchCalculationGenBatchForBegin(stmt, arraySize, indexVarStrList, indexStr, "4");
            if (res < 0)
                return res;
            if (stmt->outputs[0].type == "i32")
            {
				statementStr += "int32x4_t " + input1TempVar + " = vld1q_s32(&(" + stmt->inputs[0].name
                    + indexStr + "));\n";
                statementStr += "int32x4_t " + input2TempVar + " = vld1q_s32(&(" + stmt->inputs[1].name
                    + indexStr + "));\n";
				statementStr += "int32x4_t " + outputTempVar + " = vabdq_s32(" + input1TempVar
                    + ", " + input2TempVar + ");\n";
                statementStr += "vst1q_s32(&(" + stmt->outputs[0].name + indexStr 
                    + "), " + outputTempVar + ");\n";
            }
            else if (stmt->outputs[0].type == "u32") {
                statementStr += "uint32x4_t " + input1TempVar + " = vld1q_u32(&(" + stmt->inputs[0].name
                    + indexStr + "));\n";
                statementStr += "uint32x4_t " + input2TempVar + " = vld1q_u32(&(" + stmt->inputs[1].name
                    + indexStr + "));\n";
                statementStr += "uint32x4_t " + outputTempVar + " = vabdq_u32(" + input1TempVar
                    + ", " + input2TempVar + ");\n";
                statementStr += "vst1q_u32(" + stmt->outputs[0].name + indexStr
                    + ", " + outputTempVar + ");\n";
            }
            res = translateStmtBatchCalculationGenBatchForEnd(stmt, arraySize);
            return res;
        }
        else if (ILTranslatorForC::configTargetDevice == "Tricore")
        {
            res = translateStmtBatchCalculationGenBatchForBegin(stmt, arraySize, indexVarStrList, indexStr);
            if (res < 0)
                return res;
            if (stmt->outputs[0].type == "i32")
            {
                statementStr += "absdif_i (&(" + stmt->inputs[0].name + (stmt->inputs[0].arraySize.empty() ? "" : indexStr) + "), "
                    + "&(" + stmt->inputs[1].name + (stmt->inputs[1].arraySize.empty() ? "" : indexStr) + "), "
                    + "&(" + stmt->outputs[0].name + indexStr + "));\n";
            }
            else {
                statementStr += "absdif_u (&(" + stmt->inputs[0].name + (stmt->inputs[0].arraySize.empty() ? "" : indexStr) + "), "
                    + "&(" + stmt->inputs[1].name + (stmt->inputs[1].arraySize.empty() ? "" : indexStr) + "), "
                    + "&(" + stmt->outputs[0].name + indexStr + "));\n";
            }
            res = translateStmtBatchCalculationGenBatchForEnd(stmt, arraySize);
            return res;
        }
        else if (ILTranslatorForC::configTargetDevice == "RISC-V")
        {
            res = translateStmtBatchCalculationGenBatchForBegin(stmt, arraySize, indexVarStrList, indexStr, "4");
            if (res < 0)
                return res;
            if (stmt->outputs[0].type == "i32")
            {
                statementStr += "vabsdif_vv(&(" + stmt->inputs[0].name + indexStr + "), &(" + stmt->inputs[1].name + indexStr + "), &(" + stmt->outputs[0].name + indexStr + "));\n";
            }
            res = translateStmtBatchCalculationGenBatchForEnd(stmt, arraySize);
            return res;
        }
    }

    return 1;
}

int ILTransStatement::translateStmtBatchCalculationLog(const StmtBatchCalculation* stmt)
{
    int res;
    string opDataType = ILParser::convertDataType(stmt->dataType);

    if(stmt->inputs.size() != 1 || stmt->outputs.size() != 1) {
        return -ILTranslator::ErrorInputOutputCountOfStmtBatchCalculation;
    }
    string input1Type = ILParser::convertDataType(stmt->inputs[0].type);
    string outputType = ILParser::convertDataType(stmt->outputs[0].type);

    vector<string> indexVarStrList;
    vector<int> arraySize;
    string indexStr;

    res = translateStmtBatchCalculationGenBatchForBegin(stmt, arraySize, indexVarStrList, indexStr);
    if(res < 0)
        return res;

    string outputCalType = BasicTypeCalculator::getExpResultType(input1Type, input1Type, Token::Mul);
    outputCalType = ILParser::convertDataType(outputCalType);

    statementStr += stmt->outputs[0].name + indexStr + " = ";
    statementStr += outputCalType != outputType ? "(" + outputType + ")(" : "";
    statementStr += "log(";
    statementStr += input1Type != opDataType ? "(" + opDataType + ")" : "";
    statementStr += stmt->inputs[0].name + (stmt->inputs[0].arraySize.empty() ? "" : indexStr);
    statementStr += ")";
    statementStr += outputCalType != outputType ? ")" : "";
    statementStr += ";\n";
    
    res = translateStmtBatchCalculationGenBatchForEnd(stmt, arraySize);
    if(res < 0)
        return res;
        
    return 1;
}

int ILTransStatement::translateStmtBatchCalculationLog10(const StmtBatchCalculation* stmt)
{
    int res;
    string opDataType = ILParser::convertDataType(stmt->dataType);

    if(stmt->inputs.size() != 1 || stmt->outputs.size() != 1) {
        return -ILTranslator::ErrorInputOutputCountOfStmtBatchCalculation;
    }
    string input1Type = ILParser::convertDataType(stmt->inputs[0].type);
    string outputType = ILParser::convertDataType(stmt->outputs[0].type);

    vector<string> indexVarStrList;
    vector<int> arraySize;
    string indexStr;

    res = translateStmtBatchCalculationGenBatchForBegin(stmt, arraySize, indexVarStrList, indexStr);
    if(res < 0)
        return res;

    string outputCalType = BasicTypeCalculator::getExpResultType(input1Type, input1Type, Token::Mul);
    outputCalType = ILParser::convertDataType(outputCalType);

    statementStr += stmt->outputs[0].name + indexStr + " = ";
    statementStr += outputCalType != outputType ? "(" + outputType + ")(" : "";
    statementStr += "log10(";
    statementStr += input1Type != opDataType ? "(" + opDataType + ")" : "";
    statementStr += stmt->inputs[0].name + (stmt->inputs[0].arraySize.empty() ? "" : indexStr);
    statementStr += ")";
    statementStr += outputCalType != outputType ? ")" : "";
    statementStr += ";\n";
    
    res = translateStmtBatchCalculationGenBatchForEnd(stmt, arraySize);
    if(res < 0)
        return res;
        
    return 1;
}
int ILTransStatement::translateStmtBatchCalculationComplex(const StmtBatchCalculation* stmt)
{
    int res;
    if(stmt->operationType == StmtBatchCalculation::TypeFFT) {
        res = translateStmtBatchCalculationComplexFFT(stmt);
        return res;
    } else if(stmt->operationType == StmtBatchCalculation::TypeDCT) {
        res = translateStmtBatchCalculationComplexDCT(stmt);
        return res;
    } else if(stmt->operationType == StmtBatchCalculation::TypeConv) {
        res = translateStmtBatchCalculationComplexConv(stmt);
        return res;
    } else if (stmt->operationType == StmtBatchCalculation::TypeCorrelation) {
        res = translateStmtBatchCalculationComplexCorrelation(stmt);
        return res;
    } else if(stmt->operationType == StmtBatchCalculation::TypeDotMul) {
        res = translateStmtBatchCalculationComplexDotMul(stmt);
        return res;
    } else if(stmt->operationType == StmtBatchCalculation::TypeSquare) {
        res = translateStmtBatchCalculationSquare(stmt);
        return res;
    } else if(stmt->operationType == StmtBatchCalculation::TypeSqrt) {
        res = translateStmtBatchCalculationSqrt(stmt);
        return res;
    } else if (stmt->operationType == StmtBatchCalculation::TypeRSqrt) {
        res = translateStmtBatchCalculationRSqrt(stmt);
        return res;
    }

    return 1;
}

int ILTransStatement::translateStmtBatchCalculationComplexDotMul(const StmtBatchCalculation* stmt)
{
    int res;
    string opDataType = ILParser::convertDataType(stmt->dataType);

    if(stmt->inputs.size() != 2 || stmt->outputs.size() != 1) {
        return -ILTranslator::ErrorInputOutputCountOfStmtBatchCalculation;
    }
    string input1Type = ILParser::convertDataType(stmt->inputs[0].type);
    string input2Type = ILParser::convertDataType(stmt->inputs[1].type);
    string outputType = ILParser::convertDataType(stmt->outputs[0].type);

    vector<string> indexVarStrList;
    vector<int> arraySize;
    string indexStr;

    statementStr += stmt->outputs[0].name + " = 0;\n";

    res = translateStmtBatchCalculationGenBatchForBegin(stmt, arraySize, indexVarStrList, indexStr);
    if(res < 0)
        return res;

    string outputCalType = BasicTypeCalculator::getExpResultType(input1Type, input2Type, Token::Mul);
    outputCalType = ILParser::convertDataType(outputCalType);

    statementStr += stmt->outputs[0].name + " += ";
    statementStr += outputCalType != outputType ? "(" + outputType + ")(" : "";
    statementStr += input1Type != opDataType ? "(" + opDataType + ")" : "";
    statementStr += stmt->inputs[0].name + (stmt->inputs[0].arraySize.empty() ? "" : indexStr);
    statementStr += " * ";
    statementStr += input2Type != opDataType ? "(" + opDataType + ")" : "";
    statementStr += stmt->inputs[1].name + (stmt->inputs[1].arraySize.empty() ? "" : indexStr);
    statementStr += outputCalType != outputType ? ")" : "";
    statementStr += ";\n";
    
    res = translateStmtBatchCalculationGenBatchForEnd(stmt, arraySize);
    if(res < 0)
        return res;
        
    return 1;
}

int ILTransStatement::translateStmtBatchCalculationComplexFFT(const StmtBatchCalculation* stmt)
{
    int res;
    //statementStr += "memcpy(" + stmt->outputs[0].name + ", ";
    //statementStr += stmt->inputs[0].name + ", ";
    //statementStr += to_string(stmt->outputs[0].arraySize[0]) + ");\n";

    statementStr += "rfft_1d_f32_normal(";
    statementStr += to_string(stmt->outputs[0].arraySize[0]) + ", ";
    statementStr += stmt->inputs[0].name + ", ";
    statementStr += stmt->outputs[0].name + ");\n";

    ILCalculate* iLCalculate = (ILCalculate*)stmt->parentStatement->parentILObject;
    res = translateStmtBatchCalculationComplexAddHeadFile("rfft_1d_f32_normal.h", &iLCalculate->refs);
    if(res < 0)
        return res;

    res = translateStmtBatchCalculationComplexAddExternFile("rfft_1d_f32_normal.h");
    if(res < 0)
        return res;

    res = translateStmtBatchCalculationComplexAddExternFile("rfft_1d_f32_normal.c");
    if(res < 0)
        return res;

    return 1;
}

int ILTransStatement::translateStmtBatchCalculationComplexDCT(const StmtBatchCalculation* stmt)
{
    int res;
    //statementStr += "memcpy(" + stmt->outputs[0].name + ", ";
    //statementStr += stmt->inputs[0].name + ", ";
    //statementStr += to_string(stmt->outputs[0].arraySize[0]) + ");\n";

    statementStr += "dct_1d_f32_normal(";
    statementStr += to_string(stmt->outputs[0].arraySize[0]) + ", ";
    statementStr += stmt->inputs[0].name + ", ";
    statementStr += stmt->outputs[0].name + ");\n";

    ILCalculate* iLCalculate = (ILCalculate*)stmt->parentStatement->parentILObject;
    res = translateStmtBatchCalculationComplexAddHeadFile("dct_1d_f32_normal.h", &iLCalculate->refs);
    if(res < 0)
        return res;

    res = translateStmtBatchCalculationComplexAddExternFile("dct_1d_f32_normal.h");
    if(res < 0)
        return res;

    res = translateStmtBatchCalculationComplexAddExternFile("dct_1d_f32_normal.c");
    if(res < 0)
        return res;

    return 1;
}


int ILTransStatement::translateStmtBatchCalculationComplexConv(const StmtBatchCalculation* stmt)
{
    int res;
    //statementStr += "memcpy(" + stmt->outputs[0].name + ", ";
    //statementStr += stmt->inputs[0].name + ", ";
    //statementStr += to_string(stmt->outputs[0].arraySize[0]) + ");\n";

    statementStr += "conv_1d_f32_normal(";
    statementStr += to_string(stmt->inputs[0].arraySize[0]) + ", ";
    statementStr += stmt->inputs[0].name + ", ";
    statementStr += to_string(stmt->inputs[1].arraySize[0]) + ", ";
    statementStr += stmt->inputs[1].name + ", ";
    statementStr += stmt->outputs[0].name + ");\n";

    ILCalculate* iLCalculate = (ILCalculate*)stmt->parentStatement->parentILObject;
    res = translateStmtBatchCalculationComplexAddHeadFile("conv_1d_f32_normal.h", &iLCalculate->refs);
    if(res < 0)
        return res;

    res = translateStmtBatchCalculationComplexAddExternFile("conv_1d_f32_normal.h");
    if(res < 0)
        return res;

    res = translateStmtBatchCalculationComplexAddExternFile("conv_1d_f32_normal.c");
    if(res < 0)
        return res;

    return 1;
}

int ILTransStatement::translateStmtBatchCalculationComplexCorrelation(const StmtBatchCalculation* stmt)
{
    int res;
    //statementStr += "memcpy(" + stmt->outputs[0].name + ", ";
    //statementStr += stmt->inputs[0].name + ", ";
    //statementStr += to_string(stmt->outputs[0].arraySize[0]) + ");\n";

    statementStr += "corre_f32_normal(";
    statementStr += to_string(stmt->inputs[1].arraySize[0]) + ", ";
    statementStr += stmt->inputs[1].name + ", ";
    statementStr += to_string(stmt->inputs[0].arraySize[0]) + ", ";
    statementStr += stmt->inputs[0].name + ", ";
    statementStr += stmt->outputs[0].name + ");\n";

    ILCalculate* iLCalculate = (ILCalculate*)stmt->parentStatement->parentILObject;
    res = translateStmtBatchCalculationComplexAddHeadFile("corre_f32_normal.h", &iLCalculate->refs);
    if (res < 0)
        return res;

    res = translateStmtBatchCalculationComplexAddExternFile("corre_f32_normal.h");
    if (res < 0)
        return res;

    res = translateStmtBatchCalculationComplexAddExternFile("corre_f32_normal.c");
    if (res < 0)
        return res;

    return 1;
}

int ILTransStatement::translateStmtBatchCalculationComplexAddHeadFile(std::string name,
    std::vector<ILRef*>* refList)
{
    ILHeadFile* headFile = new ILHeadFile();
    headFile->name = name;
    for(int i = 0; i < refList->size(); i++)
    {
        headFile->refs.push_back((*refList)[i]->clone(headFile));
    }
    iLTransFile->file->headFiles.push_back(headFile);
    
    return 1;
}

int ILTransStatement::translateStmtBatchCalculationComplexAddUtilityFunction(std::string name,
    std::vector<ILRef*>* refList)
{
    ILUtilityFunction* utilityFunction = new ILUtilityFunction();
    utilityFunction->name = name;
    for(int i = 0; i < refList->size(); i++)
    {
        utilityFunction->refs.push_back((*refList)[i]->clone(utilityFunction));
    }
    iLTransFile->file->utilityFunctions.push_back(utilityFunction);

    return 1;
}

int ILTransStatement::translateStmtBatchCalculationComplexAddExternFile(std::string name)
{
    ILExternFile* externFile = new ILExternFile();
    externFile->name = name;
    ILTranslator::iLParser->externFiles.push_back(externFile);
    

    return 1;
}

int ILTransStatement::updateSubsystemExpression(Expression* expression) const
{
    if (expression->childExpressions.size() <= 0 || expression->childExpressions[0]->type != Token::Bracket)
    {
        // Expression������
        return -ILTranslator::ErrorWrongSubsystemExpression;
    }
    // �����ڲ�������
    Expression* content = expression->childExpressions[0];
    // ��һ�������������ж��ǲ���self�ṹ��
    Expression* self = content->childExpressions[0];
    if (self->type == Token::AddressOf && stringEndsWith(self->childExpressions[0]->tokenStr, "_instance"))
    {
        for (int i = content->childExpressions.size() - 1; i > 0; i--)
        {
            content->childExpressions[i]->release();
            content->childExpressions.pop_back();
        }
    }
    else {
        // Expression ������
        return -ILTranslator::ErrorWrongSubsystemExpression;
    }
    expression->updateExpressionStr();
    return 1;
}

int ILTransStatement::translateExpression(Expression* exp)
{
    if (ILTranslator::translateForC && ILTranslatorForC::configPackageParameter && exp->type == Token::Function)
    {
        updateSubsystemExpression(exp);
    }

    Statement* stmt = const_cast<Statement*>(currentTransStatement);
    if (findIndexInVector(ILPreprocessor::extraStatementInstrumented, stmt) == -1 &&
        (ILTranslator::translateForTCGStateSearch ||
            ILTranslator::translateForTCGHybrid ||
            ILTranslator::translateForCoverage))
        statementStr += ILTransInstrumentExpressionIterator::InstrumentRootBoolExpressions(
            exp, currentTransStatement ? currentTransStatement->getContainerILFunction() : nullptr);
	else
        statementStr += exp->expressionStr;

	/*ILTransExpression iLTransExpression;
	int res;
	res = iLTransExpression.translate(exp, &statementStr);
	if(res < 0)
		return res;*/

	return 1;
}

int ILTransStatement::translateStmtBatchCalculationSDMul(const StmtBatchCalculation* stmt)
{
    int res;
    string opDataType = ILParser::convertDataType(stmt->dataType);

    if (stmt->inputs.size() != 2 || stmt->outputs.size() != 1) {
        return -ILTranslator::ErrorInputOutputCountOfStmtBatchCalculation;
    }
    string input1Type = ILParser::convertDataType(stmt->inputs[0].type);
    string outputType = ILParser::convertDataType(stmt->outputs[0].type);

    vector<string> indexVarStrList;
    vector<int> arraySize;
    string indexStr;

    if (ILTranslator::translateForC)
    {
        string input1TempVar;
        string input2TempVar;
        string outputTempVar;
        if (ILTranslatorForC::configPackageParameter)
        {
            input1TempVar = stringStartsWith(stmt->inputs[0].name, "self->") ? stmt->inputs[0].name.substr(6) + "_vec" : stmt->inputs[0].name + "_vec";
            input2TempVar = stringStartsWith(stmt->inputs[1].name, "self->") ? stmt->inputs[1].name.substr(6) + "_vec" : stmt->inputs[1].name + "_vec";
            outputTempVar = stringStartsWith(stmt->outputs[0].name, "self->") ? stmt->outputs[0].name.substr(6) + "_vec" : stmt->outputs[0].name + "_vec";
        }
        if (ILTranslatorForC::configTargetDevice == "ARMv8")
        {
            // Ĭ����4λ�������Ĵ���
            // ��Ĭ�ϴ���i32��u32
            res = translateStmtBatchCalculationGenBatchForBegin(stmt, arraySize, indexVarStrList, indexStr, "4");
            if (res < 0)
                return res;
            if (stmt->outputs[0].type == "i32")
            {
                statementStr += "int16x4_t " + input1TempVar + " = vld1_s16(&(" + stmt->inputs[0].name
                    + indexStr + "));\n";
                statementStr += "int16x4_t " + input2TempVar + " = vld1_s16(&(" + stmt->inputs[1].name
                    + indexStr + "));\n";
                statementStr += "int32x4_t " + outputTempVar + " = vqdmull_s16(" + input1TempVar
                    + ", " + input2TempVar + ");\n";
                statementStr += "vst1q_s32(&(" + stmt->outputs[0].name + indexStr
                    + "), " + outputTempVar + ");\n";
            }
            res = translateStmtBatchCalculationGenBatchForEnd(stmt, arraySize);
            return res;
        }
    }

    return 1;
}
