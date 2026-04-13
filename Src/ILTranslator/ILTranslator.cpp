#include "ILTranslator.h"
#include "../Common/ArgumentParser.h"
#include "../Common/Utility.h"

#include "ILTransFile.h"
#include "ILTransMain.h"

#include "../ILBasic/ILParser.h"
#include "../ILBasic/ILFile.h"
#include "../ILBasic/ILRef.h"
#include "../ILBasic/ILExternFile.h"
#include "ILTransExternFile.h"

using namespace std;

ILParser* ILTranslator::iLParser;

string ILTranslator::configLanguage = "c";
bool ILTranslator::configGenerateComment = true;

bool ILTranslator::configInstrumentDecision = false;
bool ILTranslator::configInstrumentCondition = false;
bool ILTranslator::configInstrumentMCDC = false;
bool ILTranslator::configCompleteElse = false;

bool ILTranslator::configMergeBatchCalculation = false;

bool ILTranslator::translateForC = false;
bool ILTranslator::translateForCBMC = false;
bool ILTranslator::translateForTCGStateSearch = false;
bool ILTranslator::translateForLibFuzzer = false;
bool ILTranslator::translateForTCGHybrid = false;
bool ILTranslator::translateForCoverage = false;
bool ILTranslator::translateForSimulator = false;


string ILTranslator::transCodeHeadFile;
string ILTranslator::transCodeStructDefine;
string ILTranslator::transCodeEnumDefine;
string ILTranslator::transCodeMacro;
string ILTranslator::transCodeTypeDefine;
string ILTranslator::transCodeGlobalVariable;
string ILTranslator::transCodeUtilityFunction;
string ILTranslator::transCodeFunctionDefine;
string ILTranslator::transCodeFunction;

string ILTranslator::transCodeUtilityFunctionImplement;


int ILTranslator::errorCode = 0;
std::string ILTranslator::errorStr;

std::vector<std::string> ILTranslator::errorStrList = {
	"ErrorHeadFileTemplateCanNotFind,				  ",
	"ErrorUtilityFunctionTemplateCanNotFind,		  ",
	"ErrorILTransLocalVariableShouldNotInILSchedule,  ",
	"ErrorBranchType,								  ",
	"ErrorTranslateStmtBraceStatementShouldNotExist,  ",
	"ErrorCanNotFindMainInitFunction,                 ",
	"ErrorCanNotFindMainStateParameterOfInitFunction  ",
	"ErrorCanNotFindMainExecFunction,                 ",
	"ErrorInputOutputCountOfStmtBatchCalculation,     ",
	"ErrorInvalidBatchCalculationOperationType,       ",
	"ErrorResourceFileNotExist                        ",
	"ErrorCanNotFindRefInSubsystemCalculate           ",
	"ErrorWrongSubsystemExpression                    ",
    "ErrorNotProvideTestCase                          ",
};

//bool, char, int, float, double, loff_t, long, pchar, pthread_t, sector_t, short, size_t, u32, uchar, uint, ulong, unsigned, ushort
map<string, string> ILTranslator::cbmcTypeMap = {
    pair<string, string>("i8",  "__VERIFIER_nondet_char()"),
    pair<string, string>("i16", "__VERIFIER_nondet_short()"),
    pair<string, string>("i32", "__VERIFIER_nondet_int()"),
    pair<string, string>("i64", "__VERIFIER_nondet_long()"),
    pair<string, string>("u8",  "__VERIFIER_nondet_char()"),
    pair<string, string>("u16", "__VERIFIER_nondet_ushort()"),
    pair<string, string>("u32", "__VERIFIER_nondet_u32()"),
    pair<string, string>("u64", "__VERIFIER_nondet_ulong()"),
    pair<string, string>("f32", "__VERIFIER_nondet_float()"),
    pair<string, string>("f64", "__VERIFIER_nondet_double()"),
};

namespace 
{
    const int CONST_NUM_2 = 2;
}

void ILTranslator::setConfig(ArgumentParser& argumentParser)
{
    configLanguage = argumentParser.getFlagValue("-l");
    if (configLanguage == "c")
        translateForC = true;
    else if (configLanguage == "cbmc")
        translateForCBMC = true;
    else if (configLanguage == "tcg_state_search")
        translateForTCGStateSearch = true;
    else if (configLanguage == "libfuzzer")
        translateForLibFuzzer = true;
    else if (configLanguage == "tcg_hybrid")
        translateForTCGHybrid = true;
    else if (configLanguage == "coverage")
        translateForCoverage = true;
    else if (configLanguage == "simulator")
        translateForSimulator = true;

    configGenerateComment =	stringToLower(argumentParser.getFlagValue("-c")) == "true";

    configInstrumentCondition = stringToLower(argumentParser.getFlagValue("-ic", boolToString(configInstrumentCondition))) == "true";
    configInstrumentDecision = stringToLower(argumentParser.getFlagValue("-id", boolToString(configInstrumentDecision))) == "true";
    configInstrumentMCDC = stringToLower(argumentParser.getFlagValue("-imcdc", boolToString(configInstrumentMCDC))) == "true";
    configCompleteElse = stringToLower(argumentParser.getFlagValue("-celse", boolToString(configCompleteElse))) == "true";

    configMergeBatchCalculation = stringToLower(argumentParser.getFlagValue("-mbc", boolToString(configMergeBatchCalculation))) == "true";
}

std::string ILTranslator::getRefComment(const std::vector<ILRef*>* refList)
{
    if(!refList)
        return "";
    string commentStr;
    for(int i = 0; i < refList->size(); i++)
    {
        ILRef* ref = (*refList)[i];
        vector<string> actorStrSp = stringSplit(ref->actor, ".");
        string actorStr = "";
        if(!actorStrSp.empty())
            actorStr = actorStrSp.back();
        if(i != 0)
            commentStr += ",\n";
        if (!actorStr.empty() && !ref->path.empty())
            commentStr += actorStr + " named \"" + ref->path + "\"";
        else if(!actorStr.empty())
            commentStr += actorStr;
        else if(!ref->path.empty())
            commentStr += "\"" + ref->path + "\"";
        else if(i != 0)
            commentStr = commentStr.substr(0, commentStr.length() - CONST_NUM_2);
    }
    if(stringTrim(commentStr).empty())
        return "";
    if(stringSplit(commentStr, "\n").size() > 1)
        commentStr = "\n" + commentStr;
    return commentStr;
}


string ILTranslator::getErrorStr()
{
	string ret = "ILTranslator\nError Code: " + to_string(errorCode) + "\n";
	
	ret += "\n";

	int errorCodeIndex = -errorCode - ILTranslator::ErrorHeadFileTemplateCanNotFind;

	if(errorCodeIndex >= 0 && errorCodeIndex < errorStrList.size())
		ret += errorStrList[errorCodeIndex];
	else
		ret += "Unknow error code.";

	ret += "\n";

	ret += "Message: " + errorStr;

	return ret;
}

void ILTranslator::setCurrentError(int errorCode, string msg)
{
	ILTranslator::errorCode = errorCode;
	ILTranslator::errorStr = msg;
    
    cout << getErrorStr() << endl;
}
