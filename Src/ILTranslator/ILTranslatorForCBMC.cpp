#include "ILTranslatorForCBMC.h"

#include <fstream>

#include "ILTransFile.h"
#include "ILTransMain.h"

#include "../Common/Utility.h"
#include "../ILBasic/ILParser.h"
#include "../TCGBasic/TCGConfig.h"
#include "../ILBasic/ILFile.h"
#include "../ILBasic/ILStructDefine.h"
#include "../ILBasic/ILMember.h"
#include "../ILBasic/ILRef.h"
#include "../ILBasic/ILExternFile.h"
#include "ILTransExternFile.h"
#include "ILTranslator.h"
#include "ILTranslatorUtility.h"
#include "ILTransMetadataExporter.h"
#include "ILTransStatement.h"
#include "ILTransStatementForCBMC.h"
#include "../ILBasic/Expression.h"


using namespace std;

bool ILTranslatorForCBMC::configGenerateMultiEntries = false;
bool ILTranslatorForCBMC::configGenerateComment = true;
bool ILTranslatorForCBMC::configUnreachable = false;
bool ILTranslatorForCBMC::configTautology = false;
int ILTranslatorForCBMC::tautologyTempVariableCount = 0;
bool ILTranslatorForCBMC::configBranchCover = false;
bool ILTranslatorForCBMC::configRangeConstraint = false;
bool ILTranslatorForCBMC::configGlobalVariableReset = false;
int ILTranslatorForCBMC::configCutOffArray = true;
int ILTranslatorForCBMC::configMultiStep = 1;
bool ILTranslatorForCBMC::configGenSolvingPara = true;

ILParser* ILTranslatorForCBMC::iLParser;
TCGConfig* ILTranslatorForCBMC::tcgConfig;

namespace
{
    const int CONST_NUM_2 = 2;
}

ILTranslatorForCBMC::ILTranslatorForCBMC(ILParser* iLParser, TCGConfig* tcgConfig)
{
    ILTranslator::iLParser = iLParser;
    this->iLParser = iLParser;
    this->tcgConfig = tcgConfig;
}


int ILTranslatorForCBMC::translate(string outputDir)
{
    ILTransMetadataExporter::reset();
    ILTranslatorUtility::buildILFunctionCallGraph(iLParser);

	int res;
	for(int i = 0; i < iLParser->files.size(); i++)
	{
		string name = iLParser->files[i]->name;
		string code;
		ILTransFile transFile;
		if((res = transFile.translate(iLParser->files[i], code)) < 0)
			return res;

		code = codeIndentProcess(code);
		fileCodes[name] = stringTrim(code);
	}

    for(int i = 0; i < iLParser->externFiles.size(); i++) {
		string name = iLParser->externFiles[i]->name;
		string code;
		ILTransExternFile transExternFile;
		if((res = transExternFile.translate(iLParser->externFiles[i], code)) < 0)
			return res;

        code = removeRelativeHeadFileInCode(code);

		fileCodes[name] = stringTrim(code);
	}

	string mainCode;
	ILTransMain iLTransMain;
	if((res = iLTransMain.translate(iLParser, mainCode)) < 0)
	{
		ILTranslator::setCurrentError(res, "");
		return res;
	}

	mainCode = codeIndentProcess(mainCode);
	mainCode = stringTrim(mainCode);
	//fileCodes["main"] = mainCode;


    //ILTranslator::transCodeHeadFile += "#include <assert.h>\n";
    ILTranslator::transCodeHeadFile = stringReplaceAll(ILTranslator::transCodeHeadFile, "#include <string.h>", "");


	string defineCode;
	defineCode += ILTranslator::transCodeHeadFile;
	defineCode += ILTranslator::transCodeMacro;
	defineCode += ILTranslator::transCodeTypeDefine;
	defineCode += ILTranslator::transCodeEnumDefine;
	defineCode += ILTranslator::transCodeStructDefine;
	defineCode += ILTranslator::transCodeGlobalVariable;
	defineCode += ILTranslator::transCodeUtilityFunction;
	defineCode += ILTranslator::transCodeFunctionDefine;
	defineCode += ILTranslator::transCodeFunction;
	defineCode = codeIndentProcess(defineCode);
	defineCode = stringTrim(defineCode);


	string outputCode = defineCode + "\n";

	map<string, string>::iterator iter = fileCodes.begin();
	for(;iter!=fileCodes.end();iter++)
	{
		outputCode += iter->second + "\n";
	}
	outputCode += mainCode;

    string outputFileName = "main";
    outputFileName += (configMultiStep == 1) ? "" : (string("_") + to_string(configMultiStep));
    outputFileName += ".c";

    createFolder(outputDir);
    writeFile(outputDir + "/" + outputFileName, outputCode);

    ILTransMetadataExporter::flush(outputDir, iLParser);
	return 1;
}

void ILTranslatorForCBMC::setConfigGenerateMultiEntries(bool value)
{
    configGenerateMultiEntries = value;
}

void ILTranslatorForCBMC::setConfigGenerateComment(bool value)
{
    configGenerateComment = value;
}

void ILTranslatorForCBMC::setConfigUnreachable(bool value)
{
    configUnreachable = value;
}

void ILTranslatorForCBMC::setConfigTautology(bool value)
{
    configTautology = value;
}

void ILTranslatorForCBMC::setConfigBranchCover(bool value)
{
    configBranchCover = value;
}

void ILTranslatorForCBMC::setConfigRangeConstraint(bool value)
{
    configRangeConstraint = value;
}

void ILTranslatorForCBMC::setConfigGlobalVariableReset(bool value)
{
    configGlobalVariableReset = value;
}

void ILTranslatorForCBMC::setConfigCutOffArray(int value)
{
    configCutOffArray = value;
    if(configCutOffArray == 0)
        configCutOffArray = 1 << 30;
}

void ILTranslatorForCBMC::setConfigMultiStep(int value)
{
    configMultiStep = value;
}

void ILTranslatorForCBMC::setConfigGenSolvingPara(bool value)
{
    configGenSolvingPara = value;
}

namespace 
{
    vector<string> defaultValueList;
//    vector<int> currentMatchExpressionId;
}
std::string ILTranslatorForCBMC::getCutOffArrayDefaultValueExpStr(string varName, std::string type, const std::vector<int>& arraySize,
    Expression* exp){
    if(exp->expressionStr == "{0}" || exp->expressionStr == "{{0}}")
        return exp->expressionStr;

    defaultValueList.clear();
    generateDefaultValueStructList(varName, type, arraySize);
    Statement stmt;
    ILCCodeParser codeParser;
    int res = codeParser.generateDefaultValueExpAssignStatement(varName, type, arraySize, exp, &stmt);
    if(res < 0) {
        stmt.release();
        return "";
    }
    for(int i = stmt.childStatements.size() - 1; i >= 0; i--)
    {
        Expression* tempExp = stmt.childStatements[i]->getValueExpression();
        if(!tempExp || tempExp->childExpressions.size() != 2)
            continue;
        string varCode = tempExp->childExpressions[0]->expressionStr;
        bool needDelete = false;
        size_t pos1 = varCode.find("[");
        while(pos1 != string::npos)
        {
            size_t pos2 = varCode.find("]", pos1);
            int index = stringToInt(varCode.substr(pos1 + 1, pos2 - pos1 - 1));
            if(index >= ILTranslatorForCBMC::configCutOffArray)
            {
                needDelete = true;
                break;
            }
            pos1 = varCode.find("[", pos2);
        }
        if(needDelete)
        {
            stmt.childStatements[i]->release();
            delete stmt.childStatements[i];
            stmt.childStatements.erase(stmt.childStatements.begin() + i);
        }
    }
    map<string, string> defaultValueSettedMap;
    
    for(int i = 0; i < stmt.childStatements.size(); i++)
    {
        Expression* tempExp = stmt.childStatements[i]->getValueExpression();
        if(!tempExp || tempExp->childExpressions.size() != 2)
            continue;
        defaultValueSettedMap[tempExp->childExpressions[0]->expressionStr] = tempExp->childExpressions[1]->expressionStr;
    }
    stmt.release();

    string ret;
    ret += "{";
    for(int i = 0; i < defaultValueList.size(); i++)
    {
        if(i != 0)
            ret += ", ";
        if(defaultValueSettedMap.find(defaultValueList[i]) != defaultValueSettedMap.end())
            ret += defaultValueSettedMap.at(defaultValueList[i]);
        else
            ret += "0";
    }
    ret += "}";

    return ret;
}

void ILTranslatorForCBMC::generateDefaultValueStructList(std::string name, std::string type, const std::vector<int>& arraySize)
{
    string simpleType = ILParser::getRealDataType(type);
    simpleType = ILParser::getBasicDataTypeSimple(simpleType);

    string tempCode;
    if(ILParser::isBasicType(simpleType) && arraySize.empty()) {
        defaultValueList.push_back(name);
        return;
    }

    if(!arraySize.empty())
    {
        int count = arraySize[0];
        std::vector<int> arraySizeNew = arraySize;
        arraySizeNew.erase(arraySizeNew.begin());
        for(int i = 0; i < count && i < ILTranslatorForCBMC::configCutOffArray; i++)
        {
            generateDefaultValueStructList(name + "[" + to_string(i) + "]", type, arraySizeNew);
        }
        return;
    }

    ILStructDefine* structDefine = ILParser::findStructType(type);
    for(int i = 0; i < structDefine->members.size(); i++)
    {
        ILMember* member = structDefine->members[i];
        generateDefaultValueStructList(name + "." + member->name, member->type, member->arraySize);
    }
}

//
//Expression* ILTranslatorForCBMC::getCompoundInnerExp(Expression* exp, std::vector<int> id)
//{
//    Expression* ret = exp;
//    for(int i = 0; i < id.size(); i++)
//    {
//        ret = ret->childExpressions[id[i]];
//    }
//    return ret;
//}
//
//void ILTranslatorForCBMC::matchExpToDefaultValueStructList(Expression* exp)
//{
//    
//}

std::string ILTranslatorForCBMC::removeRelativeHeadFileInCode(std::string code)
{
    string ret;
    vector<string> lines = stringSplit(code, "\n");
    for(int i = 0; i < lines.size(); i++)
    {
        if(!stringStartsWith(lines[i], "#include")) {
            ret += lines[i] + "\n";
            continue;
        }
        if(lines[i].find("\"") == string::npos)
        {
            ret += lines[i] + "\n";
            continue;
        }

    }
    return ret;
}
