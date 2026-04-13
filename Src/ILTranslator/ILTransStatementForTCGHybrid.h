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
class ILTransStatementForTCGHybrid:
	public ILTransStatement
{
private:
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
	
};


