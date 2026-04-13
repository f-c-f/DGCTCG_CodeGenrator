#pragma once
#include <map>
#include <string>

#include "../ILBasic/StmtBatchCalculation.h"
#include "../ILBasic/Token.h"
#include "ILTransStatement.h"

class ILRef;
class ILFile;
class Statement;
class Expression;
class ILTransFile;

class ILTransStatementForSimulator:
	public ILTransStatement
{
public:
	const ILCalculate* calculator;
	std::string calculatorType;
public:
	virtual int translate(const ILCalculate* calculate, ILTransFile* file, int count);
private:
	virtual int translateExpression(Expression* exp);

	const int isFunctionExec(const ILCalculate* calculate);
	const int stmtNeedRecord();
	const int stmtNeedDiagnose();
	const void instrumentRecord();
	const void instrumentDiagnose();
	const void instrumentSPDiagnose(int count, Expression* parent);
	const void creatDiagLogic(std::string calculatorPath, std::string ope, std::string outType, std::string inType1, std::string inType2 = "");

	const std::string getOperator(std::string statementStr, std::string calculatorType);

    const bool needIntegerOverflow();
    const bool needIntegerSaturation();
    const bool needParameterDowncast();
    const bool needParameterOverflow();
    const bool needParameterPrecisionLoss();
    const bool needInt32ToFloatConv();
    const bool needArrayOutOfBound();
    const bool needDevideByZero();
};

