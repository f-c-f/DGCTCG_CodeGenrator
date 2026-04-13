#include "ILPreprocessor.h"

#include "Expression.h"
#include "ILParser.h"
#include "ILFile.h"
#include "ILFunction.h"
#include "ILSchedule.h"
#include "ILCalculate.h"
#include "ILBranch.h"
#include "ILRef.h"
#include "Statement.h"
#include "Expression.h"
#include "StmtElse.h"
#include "StmtIf.h"
#include "StmtExpression.h"
#include "StmtLocalVariable.h"


#include <algorithm>
#include "StmtSwitch.h"
#include "StmtCase.h"
#include "ILExpressionParser.h"
#include "../Common/Utility.h"

using namespace std;

vector<Statement*> ILPreprocessor::extraStatementInstrumented;

int ILPreprocessor::errorCode = 0;
std::string ILPreprocessor::errorStr;

bool ILPreprocessor::configInstrumentDecision = false;
bool ILPreprocessor::configInstrumentCondition = false;
bool ILPreprocessor::configAddSwitchMCDC = false;
bool ILPreprocessor::configInstrumentMCDC = false;
bool ILPreprocessor::configCompeleteElse = false;

std::vector<std::string> ILPreprocessor::errorStrList = {
	"ErrorILPreprocessorInnerError,				  ",
};

void ILPreprocessor::setConfigInstrumentDecision(bool value)
{
    configInstrumentDecision = value;
}

void ILPreprocessor::setConfigInstrumentCondition(bool value)
{
    configInstrumentCondition = value;
}

void ILPreprocessor::setConfigAddSwitchMCDC(bool value)
{
    configAddSwitchMCDC = value;
}

void ILPreprocessor::setConfigInstrumentMCDC(bool value)
{
    configInstrumentMCDC = value;
}

void ILPreprocessor::setConfigCompleteElse(bool value)
{
    configCompeleteElse = value;
}

string ILPreprocessor::getErrorStr()
{
	string ret = "ILPreprocessor\nError Code: " + to_string(errorCode) + "\n";
	
	ret += "\n";

	int errorCodeIndex = -errorCode - ILPreprocessor::ErrorILPreprocessorInnerError;

	if(errorCodeIndex >= 0 && errorCodeIndex < errorStrList.size())
		ret += errorStrList[errorCodeIndex];
	else
		ret += "Unknow error code.";

	ret += "\n";

	ret += "Message: " + errorStr;

	return ret;
}

void ILPreprocessor::setCurrentError(int errorCode, string msg)
{
	ILPreprocessor::errorCode = errorCode;
	ILPreprocessor::errorStr = msg;

    cout << getErrorStr() << endl;
}


int ILPreprocessor::preprocess(ILParser* iLParser)
{
    int res;

    // addedIfExpression.clear();

    res = preprocessILParser(iLParser);
    if(res < 0)
        return res;
    res = preprocessILParserReverseOrder(iLParser);
    if(res < 0)
        return res;
    return 1;
}

int ILPreprocessor::preprocessILParser(ILParser* iLParser)
{
    int res;
    for(int i = 0; i < iLParser->files.size(); i++)
    {
        ILFile* file = iLParser->files[i];
        res = preprocessILFile(file);
        if(res < 0)
            return res;
    }
    return 1;
}

int ILPreprocessor::preprocessILParserReverseOrder(ILParser* iLParser)
{
    int res;
    for(int i = 0; i < iLParser->files.size(); i++)
    {
        ILFile* file = iLParser->files[iLParser->files.size() - i - 1];
        res = preprocessILFileReverseOrder(file);
        if(res < 0)
            return res;
    }
    return 1;
}

int ILPreprocessor::preprocessILFile(ILFile* iLFile)
{
    int res;
    for(int i = 0; i < iLFile->functions.size(); i++)
    {
        ILFunction* function = iLFile->functions[i];
        res = preprocessILFunction(function);
        if(res < 0)
            return res;
    }
    return 1;
}

int ILPreprocessor::preprocessILFileReverseOrder(ILFile* iLFile)
{
    int res;
    for(int i = 0; i < iLFile->functions.size(); i++)
    {
        ILFunction* function = iLFile->functions[iLFile->functions.size() - i - 1];
        res = preprocessILFunctionReverseOrder(function);
        if(res < 0)
            return res;
    }
    return 1;
}

int ILPreprocessor::preprocessILFunction(ILFunction* iLFunction)
{
    int res;
    ILSchedule* schedule = iLFunction->schedule;
    res = preprocessILSchedule(schedule);
    if(res < 0)
        return res;
    return 1;
}

int ILPreprocessor::preprocessILFunctionReverseOrder(ILFunction* iLFunction)
{
    int res;
    ILSchedule* schedule = iLFunction->schedule;
    res = preprocessILScheduleReverseOrder(schedule);
    if(res < 0)
        return res;
    return 1;
}

int ILPreprocessor::preprocessILSchedule(ILSchedule* iLSchedule)
{
    int res;
    for(int i = 0; i < iLSchedule->scheduleNodes.size(); i++)
    {
        ILScheduleNode* scheduleNode = iLSchedule->scheduleNodes[i];
        if(scheduleNode->objType == ILObject::TypeBranch)
        {
            ILBranch* branch = static_cast<ILBranch*>(scheduleNode);
            res = preprocessILBranch(branch);
            if(res < 0)
                return res;
        }
        else if(scheduleNode->objType == ILObject::TypeCalculate)
        {
            ILCalculate* calculate = static_cast<ILCalculate*>(scheduleNode);
            res = preprocessILCalculate(calculate);
            if(res < 0)
                return res;
        }
        
    }
    return 1;
}

int ILPreprocessor::preprocessILScheduleReverseOrder(ILSchedule* iLSchedule)
{
    int res;
    for(int i = 0; i < iLSchedule->scheduleNodes.size(); i++)
    {
        ILScheduleNode* scheduleNode = iLSchedule->scheduleNodes[iLSchedule->scheduleNodes.size() - i - 1];
        if(scheduleNode->objType == ILObject::TypeBranch)
        {
            ILBranch* branch = static_cast<ILBranch*>(scheduleNode);
            res = preprocessILBranchReverseOrder(branch);
            if(res < 0)
                return res;
        }
        else if(scheduleNode->objType == ILObject::TypeCalculate)
        {
            ILCalculate* calculate = static_cast<ILCalculate*>(scheduleNode);
            res = preprocessILCalculateReverseOrder(calculate);
            if(res < 0)
                return res;
        }
        
    }
    return 1;
}

int ILPreprocessor::preprocessILBranch(ILBranch* iLBranch)
{
    int res;
    for(int i = 0; i < iLBranch->scheduleNodes.size(); i++)
    {
        ILScheduleNode* scheduleNode = iLBranch->scheduleNodes[i];
        if(scheduleNode->objType == ILObject::TypeBranch)
        {
            ILBranch* branch = static_cast<ILBranch*>(scheduleNode);
            res = preprocessILBranchReverseOrder(branch);
            if(res < 0)
                return res;
        }
        else if(scheduleNode->objType == ILObject::TypeCalculate)
        {
            ILCalculate* calculate = static_cast<ILCalculate*>(scheduleNode);
            res = preprocessILCalculateReverseOrder(calculate);
            if(res < 0)
                return res;
        }
        
    }

    if(configCompeleteElse)
    {
        if(iLBranch->type == ILBranch::TypeIf || iLBranch->type == ILBranch::TypeElseIf)
        {
            ILObject* nextObj = iLBranch->getNextSameILObject();
            if(!nextObj || 
                (static_cast<ILBranch*>(nextObj)->type != ILBranch::TypeElse && 
                    static_cast<ILBranch*>(nextObj)->type != ILBranch::TypeElseIf))
            {
                addElseBranchBehindIf(iLBranch);
            }
        }
    }
    return 1;
}

int ILPreprocessor::preprocessILBranchReverseOrder(ILBranch* iLBranch)
{
    int res;
    for(int i = 0; i < iLBranch->scheduleNodes.size(); i++)
    {
        ILScheduleNode* scheduleNode = iLBranch->scheduleNodes[iLBranch->scheduleNodes.size() - i - 1];
        if(scheduleNode->objType == ILObject::TypeBranch)
        {
            ILBranch* branch = static_cast<ILBranch*>(scheduleNode);
            res = preprocessILBranchReverseOrder(branch);
            if(res < 0)
                return res;
        }
        else if(scheduleNode->objType == ILObject::TypeCalculate)
        {
            ILCalculate* calculate = static_cast<ILCalculate*>(scheduleNode);
            res = preprocessILCalculateReverseOrder(calculate);
            if(res < 0)
                return res;
        }
        
    }
    return 1;
}

int ILPreprocessor::preprocessILCalculate(ILCalculate* iLCalculate)
{
    int res;
    for(int i = 0; i < iLCalculate->statements.childStatements.size(); i++)
    {
        Statement* statement = iLCalculate->statements.childStatements[i];
        res = preprocessILStatement(statement);
        if(res < 0)
            return res;
    }
    return 1;
}

int ILPreprocessor::preprocessILCalculateReverseOrder(ILCalculate* iLCalculate)
{
    int res;
    for(int i = 0; i < iLCalculate->statements.childStatements.size(); i++)
    {
        Statement* statement = iLCalculate->statements.childStatements[iLCalculate->statements.childStatements.size() - i - 1];
        res = preprocessILStatementReverseOrder(statement);
        if(res < 0)
            return res;
    }
    return 1;
}

int ILPreprocessor::preprocessILStatement(Statement* statement)
{
    if(findIndexInVector(extraStatementInstrumented, statement) != -1)
        return 0;
    int res;
    int size = statement->childStatements.size();
    for(int i = 0; i < statement->childStatements.size(); i++)
    {
        Statement* stmt = statement->childStatements[i];
        res = preprocessILStatement(stmt);
        if(res < 0)
            return res;
    }
    if(configCompeleteElse)
    {
        if(statement->type == Statement::If || statement->type == Statement::ElseIf)
        {
            Statement* nextStmt = statement->getNextStatement();
            if(!nextStmt || 
                (nextStmt->type != Statement::Else && nextStmt->type != Statement::ElseIf))
            {
                addElseStmtBehindIf(statement);
            }
        }
    }

    return 1;
}

int ILPreprocessor::preprocessILStatementReverseOrder(Statement* statement)
{
    if(findIndexInVector(extraStatementInstrumented, statement) != -1)
        return 0;
    int res;
    int size = statement->childStatements.size();
    for(int i = 0; i < size; i++)
    {
        Statement* stmt = statement->childStatements[size - i - 1];
        res = preprocessILStatementReverseOrder(stmt);
        if(res < 0)
            return res;
    }

    /*
    if(configInstrumentCondition)
    {
        vector<Expression*> boolExpList;
        for(int i = 0; i < statement->innerExpressions.size(); i++)
        {
            collectBooleanExpression(statement->innerExpressions[i], boolExpList);
        }
        for(int i = 0; i < boolExpList.size(); i++)
        {
            addBooleanExpressionIfElseBeforeStatement(statement, boolExpList[i]);
        }
    }
    */

    if(configAddSwitchMCDC && statement->type == Statement::Switch)
    {
        addSwitchMCDCStatement(statement);
    }

    if(configInstrumentMCDC || configInstrumentDecision || configInstrumentCondition)
    {
        addMCDCBranchBeforeStatement(statement);
    }

    return 1;
}

void ILPreprocessor::addElseBranchBehindIf(ILBranch* iLBranch)
{
    ILObject* parent = iLBranch->parent;
    vector<ILScheduleNode*>* list = nullptr;
    if(parent->objType == ILObject::TypeSchedule)
    {
        list = &(static_cast<ILSchedule*>(parent)->scheduleNodes);
    }
    else if(parent->objType == ILObject::TypeBranch)
    {
        list = &(static_cast<ILBranch*>(parent)->scheduleNodes);
    }
    else
    {
        return;
    }

    for(int i = 0; i < list->size(); i++)
    {
        if((*list)[i] == iLBranch)
        {
            ILBranch* branchElse = new ILBranch(parent);
            branchElse->type = ILBranch::TypeElse;
            for(int j = 0; j < iLBranch->refs.size(); j++)
            {
                 branchElse->refs.push_back(iLBranch->refs[j]->clone(branchElse));
            }
            (*list).insert((*list).begin() + i + 1, branchElse);
            return;
        }
    }
}

void ILPreprocessor::addElseStmtBehindIf(Statement* statement)
{
    Statement* parent = statement->parentStatement;
    
    StmtElse* stmtElse = new StmtElse();
    stmtElse->depth = statement->depth;
    stmtElse->parentStatement = statement->parentStatement;
    stmtElse->parentILObject = statement->parentILObject;

    extraStatementInstrumented.push_back(stmtElse);

    vector<Statement*>* list = &parent->childStatements;
    for(int i = 0; i < list->size(); i++)
    {
        if((*list)[i] == statement)
        {
            (*list).insert((*list).begin() + i + 1, stmtElse);
            break;
        }
    }
    
    vector<Statement*>* list2 = parent->getStatementList();
    for(int i = 0; i < list2->size() && list != list2; i++)
    {
        if((*list2)[i] == statement)
        {
            (*list2).insert((*list2).begin() + i + 1, stmtElse);
            break;
        }
    }

}

/*
void ILPreprocessor::collectBooleanExpression(Expression* exp, std::vector<Expression*>& boolExpList)
{
    if(find(addedIfExpression.begin(), addedIfExpression.end(), exp) != addedIfExpression.end())
        return;

    if(exp->type == Token::Or || exp->type == Token::And)
    {
        boolExpList.push_back(exp->childExpressions[0]);
        boolExpList.push_back(exp->childExpressions[1]);
    }
    for(int i = 0; i < exp->childExpressions.size(); i++)
    {
        collectBooleanExpression(exp->childExpressions[i], boolExpList);
    }
}

void ILPreprocessor::addBooleanExpressionIfElseBeforeStatement(Statement* statement, Expression* boolExp)
{
    while(boolExp->type == Token::Bracket)
    {
        boolExp = boolExp->childExpressions[0];
    }
    if(boolExp->type == Token::Or || boolExp->type == Token::And)
    {
        return;
    }

    Statement* parent = statement->parentStatement;
    
    StmtIf* stmtIf = new StmtIf();
    stmtIf->depth = statement->depth;
    stmtIf->parentStatement = statement->parentStatement;
    stmtIf->parentILObject = statement->parentILObject;
    stmtIf->condition = boolExp->clone();
    stmtIf->innerExpressions.push_back(stmtIf->condition);
    addedIfExpression.push_back(stmtIf->condition);
    extraStatementInstrumented.push_back(stmtIf);

    StmtElse* stmtElse = new StmtElse();
    stmtElse->depth = statement->depth;
    stmtElse->parentStatement = statement->parentStatement;
    stmtElse->parentILObject = statement->parentILObject;
    extraStatementInstrumented.push_back(stmtElse);

    vector<Statement*>* list = &parent->childStatements;
    for(int i = 0; i < list->size(); i++)
    {
        if((*list)[i] == statement)
        {
            (*list).insert((*list).begin() + i, stmtIf);
            (*list).insert((*list).begin() + i + 1, stmtElse);
            break;
        }
    }

    vector<Statement*>* list2 = parent->getStatementList();
    for(int i = 0; i < list2->size() && list != list2; i++)
    {
        if((*list2)[i] == statement)
        {
            (*list2).insert((*list2).begin() + i, stmtIf);
            (*list2).insert((*list2).begin() + i + 1, stmtElse);
            break;
        }
    }
}
*/


namespace {
    int switchMCDCCount = 0;
}

void ILPreprocessor::addSwitchMCDCStatement(Statement* statement)
{
    StmtSwitch* stmtSwitch = static_cast<StmtSwitch*>(statement);
    vector<string> caseValueList;
    for(int i = 0; i < stmtSwitch->childStatements.size(); i++)
    {
        Statement* stmt = stmtSwitch->childStatements[i];
        if(stmt->type != Statement::Case)
            continue;
        StmtCase* stmtCase = static_cast<StmtCase*>(stmt);
        caseValueList.push_back(stmtCase->value->expressionStr);
    }

    switchMCDCCount++;
    
    string expValueStr;
    for(int i = 0; i < caseValueList.size(); i++)
    {
        StmtLocalVariable* stmtLocalVar = new StmtLocalVariable(statement->parentStatement);
        string name = "Switch_MCDC_Value_" + to_string(switchMCDCCount) + "_" + to_string(i + 1);
        string type = "u8";
        string expStr = stmtSwitch->condition->expressionStr + " != "+ caseValueList[i] + ";";
        Expression* exp = ILExpressionParser::parseExpression(expStr);
        stmtLocalVar->set(name, type, exp);
        statement->parentStatement->insertStatementBefore(stmtLocalVar, stmtSwitch);

        expValueStr += "Switch_MCDC_Value_" + to_string(switchMCDCCount) + "_" + to_string(i + 1) + " && ";
    }
    expValueStr = expValueStr.substr(0, expValueStr.length() - 4);
    expValueStr += ";";

    StmtLocalVariable* stmtLocalVar = new StmtLocalVariable(statement->parentStatement);
    string expName = "Switch_MCDC_Value_" + to_string(switchMCDCCount);
    string type = "u8";
    Expression* exp = ILExpressionParser::parseExpression(expValueStr);
    stmtLocalVar->set(expName, type, exp);
    statement->parentStatement->insertStatementBefore(stmtLocalVar, stmtSwitch);
}

void ILPreprocessor::addMCDCBranchBeforeStatement(Statement* statement)
{
    Expression* exp = nullptr;

    // 把ElseIf要插桩的MCDC表达式也放在If前面，并加上前置左右if或else if条件为false的判断
    if(statement->type == Statement::If ||
        //statement->type == Statement::While || 
        //statement->type == Statement::DoWhile || 
        //statement->type == Statement::For || 
        statement->type == Statement::LocalVariable || 
        statement->type == Statement::ExpressionWithSemicolon)
    {
        exp = statement->getValueExpression();
    }
    
    if(exp)
        addMCDCBranchBeforeStatementRecursiveExp(exp, statement, statement);

    if(statement->type == Statement::If)
    {
        Statement* next = statement->getNextStatement();
        while(next && next->type == Statement::ElseIf)
        {
            exp = next->getValueExpression();
            if(exp)
                addMCDCBranchBeforeStatementRecursiveExp(exp, statement, next);
            next = next->getNextStatement();
        }
    }

}

void ILPreprocessor::addMCDCBranchBeforeStatementRecursiveExp(Expression* exp, Statement* statement, Statement* realStatement)
{
	for (Expression* e : exp->childExpressions)
	{
		addMCDCBranchBeforeStatementRecursiveExp(e, statement, realStatement);
	}
	bool isBoolExp = exp->isBoolExpression();
    Expression * p = exp->parentExpression;
	Expression * rp = p ? p->SkipBracketToFindRealParent() : nullptr;
	if (isBoolExp && (rp == nullptr || !rp->isBoolExpression())) {
		
        string instrumentStmtStr = GenerateMCDCToRootStatements(exp, realStatement);
        if(!instrumentStmtStr.empty())
        {
            ILCCodeParser codeParser;
            Statement* newStmt = new Statement;
		    int res = codeParser.parseCCode(instrumentStmtStr, newStmt);
            if(res < 0)
                setCurrentError(-ErrorILPreprocessorInnerError, "addMCDCBranchBeforeStatementRecursiveExp, Parse C Code Error.");

            for(int i = 0; i < newStmt->childStatements.size(); i++)
            {
                Statement* stmt = newStmt->childStatements[i];
                statement->parentStatement->insertStatementBefore(stmt, statement);
                extraStatementInstrumented.push_back(stmt);
            }
            newStmt->childStatements.clear();
            newStmt->release();
            delete newStmt;
        }
	}

}


std::string ILPreprocessor::MCDCCoverageFunc;
std::map<const Expression*, std::string> ILPreprocessor::MCDCCoverageSubExpMap;
int ILPreprocessor::MCDCSubExpId;
std::vector<std::pair<const Expression*, std::string>> ILPreprocessor::MCDCCoverageLeafExp;
std::string ILPreprocessor::MCDCCoverageFuncAll;
int ILPreprocessor::mcdcCount;


std::string ILPreprocessor::HandleConditionExpression(const int mcdcId, const Expression* condition, int& decCondId)
{
	std::string opPre = "";
	std::string opPost = "";
	bool condIsNonLeaf = false;
	bool trimOpPost = true;
	switch (condition->type) {
		case Token::Type::And: {
			opPost = "&&";
			condIsNonLeaf = true;
			break;
		}
		case Token::Type::Or: {
			opPost = "||";
			condIsNonLeaf = true;
			break;
		}
		case Token::Type::Not: {
			opPre = "!";
			condIsNonLeaf = true;
			break;
		}
		case Token::Type::Bracket: {
			opPre = "(";
			opPost = ")";
			trimOpPost = false;
			condIsNonLeaf = true;
			break;
		}
		default:
			break;
	}

	//std::string modifiedExpr = "";
	std::string modifiedExprSplit = "";
	if (condIsNonLeaf) {
		std::vector<Expression*> childs = condition->childExpressions;
		for (Expression* child : childs) {
			//modifiedExpr += (opPre + HandleConditionExpression(mcdcId, child, exprStrMap, decCondId) + opPost);
            HandleConditionExpression(mcdcId, child, decCondId);
            modifiedExprSplit += (opPre + MCDCCoverageSubExpMap.at(child) + opPost);
		}
		if (trimOpPost && opPost != "" && stringEndsWith(modifiedExprSplit, opPost)) {
			//modifiedExpr = modifiedExpr.substr(0, modifiedExpr.length() - opPost.length());
            modifiedExprSplit = modifiedExprSplit.substr(0, modifiedExprSplit.length() - opPost.length());
		}
        MCDCSubExpId++;
        std::string subExpStr = "unsigned char mcdc_" + std::to_string(mcdcId) + "_exp_" + std::to_string(MCDCSubExpId) + " = " + modifiedExprSplit;
        MCDCCoverageSubExpMap[condition] = "mcdc_" + std::to_string(mcdcId) + "_exp_" + std::to_string(MCDCSubExpId);

        MCDCCoverageFunc += subExpStr + ";\n";
	}
	else {

        MCDCSubExpId++;
        std::string subExpStr = "unsigned char mcdc_" + std::to_string(mcdcId) + "_exp_" + std::to_string(MCDCSubExpId) + " = mcdc_" + std::to_string(mcdcId) + "_c_" + std::to_string(decCondId);
        MCDCCoverageSubExpMap[condition] = "mcdc_" + std::to_string(mcdcId) + "_c_" + std::to_string(decCondId);

        MCDCCoverageFunc += subExpStr + ";\n";
        MCDCCoverageLeafExp.push_back(make_pair(condition, MCDCCoverageSubExpMap[condition]));

		//const void* condAddr = static_cast<const void*>(condition);
		//std::string cbbStr = std::to_string((long long)condAddr);
		//bool exist = exprStrMap.find(cbbStr) != exprStrMap.end();
		//std::string rstr = (exist ? exprStrMap[cbbStr] : condition->expressionStr);
        decCondId++;
		
	}

	return modifiedExprSplit;
}



// const void* block, 
std::string ILPreprocessor::GenerateMCDCToRootStatements(const Expression* condition, const Statement* statement)
{
	if (condition == nullptr) {
		return "";
	}
    MCDCCoverageLeafExp.clear();
    MCDCCoverageSubExpMap.clear();
    MCDCCoverageFunc = "";
    MCDCSubExpId = 0;

	int mcdcId = ILPreprocessor::mcdcCount;
	ILPreprocessor::mcdcCount++;

	int condId = 1;
	std::string modifiedExpr = HandleConditionExpression(mcdcId, condition, condId);
	int condNum = condId - 1;

    string func;
    if(MCDCCoverageLeafExp.size() >= 1)
    {
        bool needGenCondition = false;
        bool needGenDecision = false;
        bool needGenMCDC = false;

        generateBranchGateJudgement(statement, needGenCondition, needGenDecision, needGenMCDC);

        func += generateMCDCFuncExpSplit(needGenCondition, needGenDecision, needGenMCDC);
        func += generateMCDCFuncMaskingCond(mcdcId, needGenCondition, needGenDecision, needGenMCDC);
        func += generateMCDCFuncRes(mcdcId, statement, needGenCondition, needGenDecision, needGenMCDC);
        //func = codeIndentProcess(func);
    }
    
    return func;
}

void ILPreprocessor::generateBranchGateJudgement(const Statement* statement, bool& needGenCondition, bool& needGenDecision, bool& needGenMCDC)
{
    if (configInstrumentCondition && 
        !((statement->type == Statement::If || statement->type == Statement::ElseIf) && MCDCCoverageLeafExp.size() == 1))
    {
        needGenCondition = true;
    }
    if (configInstrumentDecision && statement->type != Statement::If && MCDCCoverageLeafExp.size() != 1) 
    {
        needGenCondition = true;
        needGenDecision = true;
    }
    if (configInstrumentMCDC && MCDCCoverageLeafExp.size() != 1) 
    {
        needGenCondition = true;
        needGenDecision = true;
        needGenMCDC = true;
    }
}

std::string ILPreprocessor::generateMCDCFuncExpSplit(bool needGenCondition, bool needGenDecision, bool needGenMCDC)
{
    string func;
    for (int i = 0; needGenCondition && i < MCDCCoverageLeafExp.size(); i++) {
        func += "unsigned char " + MCDCCoverageLeafExp[i].second + " = " + MCDCCoverageLeafExp[i].first->expressionStr + ";\n";
        // + " = ((1 << " + to_string(i+1) + ") & condValue) != 0;\n";
    }
    if(needGenDecision)
        func += MCDCCoverageFunc;
    return func;
}


std::string ILPreprocessor::generateMCDCFuncMaskingCond(int mcdcId, bool needGenCondition, bool needGenDecision, bool needGenMCDC)
{
    string func;
    auto it = MCDCCoverageLeafExp.begin();
    for (; needGenMCDC && it != MCDCCoverageLeafExp.end(); it++) {
        func += "unsigned char " + it->second + "_masked = ";

        std::string cond = "";
        const Expression* p = it->first->parentExpression;
        const Expression* c = it->first;
        while(p)
        {
            if(p->type == Token::Type::And || p->type == Token::Type::Or)
            {
                const Expression* other = p->childExpressions[0] == c ? p->childExpressions[1] : p->childExpressions[0];
                cond += MCDCCoverageSubExpMap.at(other) + " == " + ((p->type == Token::Type::And) ? "0" : "1") + " || ";
            }
            c = p;
            p = p->parentExpression;
        }

        if(cond.empty())
            func += "0;\n";
        else
            func += cond.substr(0, cond.length() - 4) + ";\n";
    }
    return func;
}

std::string ILPreprocessor::generateMCDCFuncRes(int mcdcId, const Statement* statement, bool needGenCondition, bool needGenDecision, bool needGenMCDC)
{
    string func;
    for (int i = 0; needGenCondition && i < MCDCCoverageLeafExp.size(); i++) {
        func += "if(" + MCDCCoverageLeafExp[i].second + ")\n";
        func += "{\n";
        func += "}\n";
        func += "else\n";
        func += "{\n";
        func += "}\n";
    }
    if (needGenDecision) {
        func += "if(mcdc_" + std::to_string(mcdcId) + "_exp_" + std::to_string(MCDCSubExpId) + ")\n";
        func += "{\n";
        func += "}\n";
        func += "else\n";
        func += "{\n";
        func += "}\n";
    }
    for (int i = 0; needGenMCDC && i < MCDCCoverageLeafExp.size(); i++) {
        func += "if(" + MCDCCoverageLeafExp[i].second + " && !" + MCDCCoverageLeafExp[i].second + "_masked)\n";
        func += "{\n";
        func += "}\n";
        func += "else if(!" + MCDCCoverageLeafExp[i].second + " && !" + MCDCCoverageLeafExp[i].second + "_masked)\n";
        func += "{\n";
        func += "}\n";
        //func += "else\n";
        //func += "{\n";
        //func += "}\n";
    }
    return func;
}





