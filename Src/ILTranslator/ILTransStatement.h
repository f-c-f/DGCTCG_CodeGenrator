#pragma once
#include <map>
#include <string>

#include "../ILBasic/StmtBatchCalculation.h"
#include "../ILBasic/Token.h"
class StmtFor;
class ILRef;
class ILFile;
class Statement;
class Expression;
class ILTransFile;
class ILAnalyzeSchedule;

class ILTransStatement
{
public:
    int translate(const Statement* statement, ILTransFile* file, const ILAnalyzeSchedule* iLTranslateSchedule);
    int translateStatement(const Statement* statement);
    
    const Statement* currentTransStatement = nullptr;
	std::string statementStr;

    static int translateStmtBatchCalculationLoopUnrolling(std::string& stmtCode);
    
private:
    const ILAnalyzeSchedule* iLTranslateSchedule = nullptr;
    Statement* getPreviousTranslateStatement(const Statement* statement) const;
    Statement* getNextTranslateStatement(const Statement* statement) const;

protected:

    ILTransFile* iLTransFile = nullptr;
    
	//用于转化各种语句
	virtual int translateStmtIf(const Statement* stmt);
	virtual int translateStmtElse(const Statement* stmt);
	virtual int translateStmtElseIf(const Statement* stmt);
	virtual int translateStmtWhile(const Statement* stmt);
	virtual int translateStmtDoWhile(const Statement* stmt);
	virtual int translateStmtFor(const Statement* stmt);
	virtual int translateStmtContinue(const Statement* stmt);
	virtual int translateStmtSwitch(const Statement* stmt);
	virtual int translateStmtCase(const Statement* stmt);
	virtual int translateStmtDefault(const Statement* stmt);
	int translateStmtBreak(const Statement* stmt);
	int translateStmtReturn(const Statement* stmt);
	int translateStmtBraceStatement(const Statement* stmt) const;
	int translateStmtLocalVariable(const Statement* stmt);
	int translateStmtExpressionWithSemicolon(const Statement* stmt);

    static std::map<StmtBatchCalculation::OperationType, std::string> stmtBatchCalculationOperationToStrMap;
    static std::map<StmtBatchCalculation::OperationType, Token::Type> stmtBatchCalculationOperationToTokenTypeMap;
    int translateStmtBatchCalculationGenBatchForBegin(const StmtBatchCalculation* stmt, std::vector<int>& arraySize, std::vector<std::string>& indexVarStrList, std::string& indexStr);
    int translateStmtBatchCalculationGenBatchForBegin(const StmtBatchCalculation* stmt, std::vector<int>& arraySize, std::vector<std::string>& indexVarStrList, std::string& indexStr, std::string interval);
    int translateStmtBatchCalculationGenBatchForEnd(const StmtBatchCalculation* stmt, const std::vector<int>& arraySize);

    int translateStmtBatchCalculation(const Statement* stmt);
    static int translateStmtBatchCalculationLoopUnrollingCanUnrolling(const Statement& root, std::string& loopVar, int& loopCount, StmtFor** forStmt);
    static int translateStmtBatchCalculationLoopUnrollingGetLoopBeforeAndAfterCode(const Statement& root, const StmtFor* forStmt, std::string& beforeCode, std::string& afterCode);
    static int translateStmtBatchCalculationLoopUnrollingGetLoopStmtStr(const StmtFor* forStmt, std::string& retStmtStr);
    static int translateStmtBatchCalculationLoopUnrollingGetUnrollingStmtStr(const std::string loopStmtStr, const std::string& loopVar, int loopCount, std::string& retStmtStr);
    
    int translateStmtBatchCalculationAssign(const StmtBatchCalculation* stmt);
    int translateStmtBatchCalculationReadData(const StmtBatchCalculation* stmt);
    int translateStmtBatchCalculationLogicNot(const StmtBatchCalculation* stmt);
    int translateStmtBatchCalculationLogicNAnd(const StmtBatchCalculation* stmt);
    int translateStmtBatchCalculationLogicNOr(const StmtBatchCalculation* stmt);
    int translateStmtBatchCalculationLogicXor(const StmtBatchCalculation* stmt);
    int translateStmtBatchCalculationLogicNXor(const StmtBatchCalculation* stmt);
    int translateStmtBatchCalculationSquare(const StmtBatchCalculation* stmt);
    int translateStmtBatchCalculationSqrt(const StmtBatchCalculation* stmt);
    int translateStmtBatchCalculationRSqrt(const StmtBatchCalculation* stmt);
    int translateStmtBatchCalculationExp(const StmtBatchCalculation* stmt);
    int translateStmtBatchCalculationAbs(const StmtBatchCalculation* stmt);
    int translateStmtBatchCalculationDAbs(const StmtBatchCalculation* stmt);
    int translateStmtBatchCalculationLog(const StmtBatchCalculation* stmt);
    int translateStmtBatchCalculationLog10(const StmtBatchCalculation* stmt);
    int translateStmtBatchCalculationSAdd(const StmtBatchCalculation* stmt);
    int translateStmtBatchCalculationSSub(const StmtBatchCalculation* stmt);
    int translateStmtBatchCalculationSMul(const StmtBatchCalculation* stmt);
    int translateStmtBatchCalculationSDMul(const StmtBatchCalculation* stmt);

    int translateStmtBatchCalculationComplex(const StmtBatchCalculation* stmt);
    int translateStmtBatchCalculationComplexDotMul(const StmtBatchCalculation* stmt);
    int translateStmtBatchCalculationComplexFFT(const StmtBatchCalculation* stmt);
    int translateStmtBatchCalculationComplexDCT(const StmtBatchCalculation* stmt);
    int translateStmtBatchCalculationComplexConv(const StmtBatchCalculation* stmt);
    int translateStmtBatchCalculationComplexCorrelation(const StmtBatchCalculation* stmt);

    int translateStmtBatchCalculationComplexAddHeadFile(std::string name, std::vector<ILRef*>* refList);
    int translateStmtBatchCalculationComplexAddUtilityFunction(std::string name, std::vector<ILRef*>* refList);
    int translateStmtBatchCalculationComplexAddExternFile(std::string name);

    int updateSubsystemExpression(Expression* expression) const;
    virtual int translateExpression(Expression* exp);
};




