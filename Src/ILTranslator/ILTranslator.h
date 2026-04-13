#pragma once
#include <string>
#include <map>
#include "ILTranslatorForC.h"
#include "ILTranslatorForCBMC.h"
#include "ILTranslatorForSimulator.h"

class ArgumentParser;

class ILTranslator
{
public:
	static ILParser* iLParser;

    static std::string configLanguage;
    static bool configGenerateComment;

    static bool configInstrumentCondition;
    static bool configInstrumentDecision;
    static bool configInstrumentMCDC;
    static bool configCompleteElse;

    static bool configMergeBatchCalculation;

    static bool translateForC;
    static bool translateForCBMC;
    static bool translateForTCGStateSearch;
    static bool translateForLibFuzzer;
    static bool translateForTCGHybrid;
	static bool translateForCoverage;
	static bool translateForSimulator;


    static std::string transCodeHeadFile;
	static std::string transCodeStructDefine;
	static std::string transCodeEnumDefine;
	static std::string transCodeMacro;
	static std::string transCodeTypeDefine;
	static std::string transCodeGlobalVariable;
	static std::string transCodeFunctionDefine;
	static std::string transCodeUtilityFunction;
	static std::string transCodeFunction;
    
	static std::string transCodeUtilityFunctionImplement;


    static void setConfig(ArgumentParser& argumentParser);

    static std::string getRefComment(const std::vector<ILRef*>* refList);

    enum {
		ErrorHeadFileTemplateCanNotFind = 170001,
		ErrorUtilityFunctionTemplateCanNotFind,
		ErrorILTransLocalVariableShouldNotInILSchedule,
		ErrorBranchType,
		ErrorTranslateStmtBraceStatementShouldNotExist,
		ErrorCanNotFindMainInitFunction,
		ErrorCanNotFindMainStateParameterOfInitFunction,
		ErrorCanNotFindMainExecFunction,
		ErrorInputOutputCountOfStmtBatchCalculation,
		ErrorInvalidBatchCalculationOperationType,
		ErrorResourceFileNotExist,
		ErrorCanNotFindRefInSubsystemCalculate,
		ErrorWrongSubsystemExpression,
		ErrorNotProvideTestCase,
    };

    static std::vector<std::string> errorStrList;
    
	static std::string getErrorStr();
	static void setCurrentError(int errorCode, std::string msg);

    static std::map<std::string, std::string> cbmcTypeMap;
        

private:
	static int errorCode;
	static std::string errorStr;
};
