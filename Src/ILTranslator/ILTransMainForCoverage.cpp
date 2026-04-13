#include "ILTransMain.h"

#include <set>

#include "ILTranslator.h"
#include "ILTranslatorForCoverage.h"
#include "../Common/Utility.h"
#include "../ILBasic/Expression.h"

#include "../ILBasic/ILFunction.h"
#include "../ILBasic/ILInput.h"
#include "../ILBasic/ILOutput.h"
#include "../ILBasic/ILParser.h"
#include "../ILBasic/ILFile.h"
#include "../ILBasic/ILGlobalVariable.h"
#include "../ILBasic/ILStructDefine.h"
#include "../ILBasic/ILMember.h"
#include "ILTransInstrumentBitmapHelper.h"
#include "ILTranslatorUtility.h"
#include "../ILBasic/ILAnalyzeSchedule.h"
#include "../ILBasic/ILAnalyzeScheduleNode.h"
#include "../ILBasic/ILBranch.h"
#include "../ILBasic/ILSchedule.h"
#include "../ILBasic/StmtLocalVariable.h"

using namespace std;

namespace
{
    int execInputCopyId = 0;
    int inputsDataLengthTotal = 0;
}

int ILTransMain::translateForCoverage(const ILParser* parser, string& code)
{
    iLParser = parser;
    string commentStr;
    
    int res = translateForCoverageHeadFile();
    if(res < 0)
        return res;
    
	res = translateMainInstanceGlobalVariable();
    if(res < 0)
        return res;
    
    //if (ILTranslatorForCoverage::testCaseFileFolder.empty())
    //    return -1; // 不存在TestCaseFile

    string mainCodeStr;
    res = translateForCoverageMainFunction(parser, mainCodeStr);
    if(res < 0)
        return res;
    transCodeFunction += mainCodeStr;

    //string stepCodeStr;
    //res = translateForTCGSSStepFunction(parser, stepCodeStr);
    //if(res < 0)
    //    return res;
    //transCodeFunction += stepCodeStr;

	string retCode;
	retCode += transCodeHeadFile + transCodeStructDefine;
	retCode += transCodeMacro + transCodeTypeDefine;
	retCode += transCodeGlobalVariable + transCodeUtilityFunction;
	retCode += transCodeFunction;
	code = retCode;
	return 1;
}


int ILTransMain::translateForCoverageHeadFile()
{
    transCodeHeadFile += "#include <stdio.h>\n";
    transCodeHeadFile += "#include <vector>\n";
    transCodeHeadFile += "#include <set>\n";
    transCodeHeadFile += "#include <string>\n";
    transCodeHeadFile += "#include <string.h>\n";
    transCodeHeadFile += "#include <time.h>\n";
    transCodeHeadFile += "#include \"Utility.h\"\n";
    transCodeHeadFile += "#include \"BitmapCov.h\"\n";
    transCodeHeadFile += "#include \"ArgumentParser.h\"\n";
    return 1;
}

string ILTransMain::translateForCoverageInfoInit()
{
    string retCode = "// Coverage init code\n";
    retCode += "BitmapCov::munitBitmapLength = " + std::to_string(ILTransInstrumentBitmapHelper::munitBitmapCount) + ";\n";
	retCode += "BitmapCov::branchBitmapLength = " + std::to_string(ILTransInstrumentBitmapHelper::branchBitmapCount) + ";\n";
	retCode += "BitmapCov::condBitmapLength = " + std::to_string(ILTransInstrumentBitmapHelper::condBitmapCount) + ";\n";
	retCode += "BitmapCov::decBitmapLength = " + std::to_string(ILTransInstrumentBitmapHelper::decBitmapCount) + ";\n";
	retCode += "BitmapCov::mcdcBitmapLength = " + std::to_string(ILTransInstrumentBitmapHelper::mcdcBitmapCount) + ";\n";
	
	retCode += "static const unsigned char _mcdcMetaInit[] = { ";
	for (int i = 0; i < ILTransInstrumentBitmapHelper::mcdcBitmapCount; i++) {
		std::map<int, int>::iterator itr = ILTransInstrumentBitmapHelper::mcdcConditionNumInDecision.find(i);
		if (itr != ILTransInstrumentBitmapHelper::mcdcConditionNumInDecision.end()) {
			int condNum = itr->second;
			retCode += std::to_string(condNum) + ", ";
		}
		else {
			printf("strange! not found mcdc condition? ");
		}
	}
	retCode += "};\n";
	retCode += "BitmapCov::mcdcMeta = new unsigned char[BitmapCov::mcdcBitmapLength];\n";
	retCode += "memcpy(BitmapCov::mcdcMeta, _mcdcMetaInit, BitmapCov::mcdcBitmapLength);\n";
    retCode += "BitmapCov::init();\n";
    //retCode += "StateSearch::initStateSearch(argc, argv);\n";
    //retCode += "StateSearchBasicBranchRegister::loadStateSearchBranchData();\n";
    //retCode += "StateSearch::registStateBranch(StateSearchBasicBranchRegister::StateSearch_Branch, StateSearchBasicBranchRegister::StateSearch_Branch_Depth);\n";

    //retCode += "vector<pair<int, string>> StateSearch_BranchReadGlobalVar;\n";
    //retCode += "vector<pair<int, string>> StateSearch_BranchWriteGlobalVar;\n";
    //retCode += "StateSearchBasicBranchRelation::loadStateSearchBranchRelation(StateSearch_BranchReadGlobalVar, StateSearch_BranchWriteGlobalVar);\n";
    //retCode += "StateSearch::registStateBranchRelation(StateSearch_BranchReadGlobalVar, StateSearch_BranchWriteGlobalVar);\n";

    
    //ILTranslatorForTCGStateSearch::transCodeBranchRegister += translateForTCGSSMainStateSearchRegistBranch();
    //ILTranslatorForTCGStateSearch::transCodeBranchRelation += translateForTCGSSMainStateSearchBranchRelation();

    
    return retCode;
}

int ILTransMain::translateForCoverageMainFunction(const ILParser* parser, std::string& code)
{
    int res = 0;
    string retCode;
    string commentStr;
    const ILConfig* config = &(parser->config);
    if(ILTranslator::configGenerateComment) {
        commentStr = "/* Main Function of model */\n";
    }

    ILFunction* mainExecFunction = nullptr;
    if (!config->mainExecFunction.empty()) {
        mainExecFunction = parser->findILFunction(config->mainExecFunction);
        if (!mainExecFunction)
            return -ILTranslator::ErrorCanNotFindMainExecFunction;
    }
    else
    {
        return -ILTranslator::ErrorCanNotFindMainExecFunction;
    }

	retCode += commentStr + "int main(int argc, char *argv[]) {\n";

    string codeCoverageInfoInit = translateForCoverageInfoInit();
    retCode += codeCoverageInfoInit;
    

    string testCaseCopyCode = translateForCoverageMainExecParameterCopy(mainExecFunction);

    // 读取测试用例
    retCode += "int inputsDataLengthTotal = " + to_string(inputsDataLengthTotal) + ";\n";
    retCode += translateForCoverageFilesName();
    //retCode += "int sizeBuf;\n";
    //retCode += "const char* dataBuf = readFileBinary(\"" + ILTranslatorForCoverage::testCaseFileFolder + "\", sizeBuf);\n";
    //retCode += "if (sizeBuf % inputsDataLengthTotal != 0) {\n";
    //retCode += "    sizeBuf += (inputsDataLengthTotal - (sizeBuf % inputsDataLengthTotal));\n";
    //retCode += "}\n";
    // Main Loop
    if (ILTranslator::configGenerateComment)
    {
        commentStr = "/* Main Loop of model */\n";
    }

    retCode += commentStr; 
    
    retCode += "for (const auto& file: files) {\n";
    if (!config->mainInitFunction.empty()) {
        ILFunction* mainInitFunction = parser->findILFunction(config->mainInitFunction);
        if (!mainInitFunction)
            return -ILTranslator::ErrorCanNotFindMainInitFunction;

        //生成主Init函数的参数变�?        retCode += translateMainInitParameter(mainInitFunction);
        //生成主Init函数，包括其参数
        retCode += translateMainInitFuncionCall(mainInitFunction);
    }
    retCode += "int sizeBuf;\n";
    retCode += "const char* dataBuf = readFileBinary(folder + file, sizeBuf); \n";
    retCode += "if(!dataBuf) {\n";
    retCode += "    std::cout << \"Read Test Case File Failed. File Name: \" << folder + file << std::endl;\n";
    retCode += "    continue;\n";
    retCode += "}\n";
    retCode += "if (sizeBuf % inputsDataLengthTotal != 0) {\n";
    retCode += "    sizeBuf += (inputsDataLengthTotal - (sizeBuf % inputsDataLengthTotal));\n";
    retCode += "}\n";

    retCode += "for (int i = 0; true; i++) {\n";
    retCode += "if((i + 1) * inputsDataLengthTotal > sizeBuf) {\n";
    retCode += "break;\n";
    retCode += "}\n";

    //生成主函数的参数变量
    retCode += translateMainExecParameter(mainExecFunction);
    retCode += testCaseCopyCode;
    //生成主函数，包括其参�?    
    retCode += translateMainExecFuncionCall(mainExecFunction);
	retCode += "}\n";
    retCode += "BitmapCov::step();\n";
    retCode += "std::cout << \"Coverage Info (File Name: \" << file << \"):\" << std::endl;\n";
    string codeCoverageOutput = translateForCoverageInfoOutput();
    retCode += codeCoverageOutput;
    retCode += "delete[] dataBuf;\n";
    retCode += "}\n";
    retCode += "}\n";

    code = retCode;
    return 1;
}

std::string ILTransMain::translateForCoverageInfoOutput()
{
    string retCode;
    retCode += "BitmapCov::printCoverage();\n";
    retCode += "std::cout << std::endl;\n";
    return retCode;
}

std::string ILTransMain::translateForCoverageMainExecParameterCopy(const ILFunction* mainExecFunction) const
{
    string ret;
    execInputCopyId = 0;
    inputsDataLengthTotal = 0;
    for (int i = 0; i < mainExecFunction->inputs.size(); i++) {
        ILInput* input = mainExecFunction->inputs[i];
        if (mainExecFunction->inputs[i]->type == iLParser->config.mainCompositeState) {
            continue;
        }
        string paraName = "exec_" + input->name;
        ret += translateForCoverageMainExecParameterCopyTraverse(paraName, input->type, input->arraySize, input->isAddress);
    }
    return ret;
}

std::string ILTransMain::translateForCoverageFilesName()
{
    string ret;
    ret += "ArgumentParser argumentParser;\n";
    ret += "if (argumentParser.parseArguments(argc, argv) == 0) \n return 0;\n";
    ret += "std::vector<std::string> files;\n";
    ret += "std::string folder;\n";
    ret += "folder = argumentParser.getFlagValue(\"-iF\");\n";
    ret += "if (!argumentParser.getFlagValue(\"-i\").empty()) {\n";
    ret += "    files.push_back(argumentParser.getFlagValue(\"-i\"));\n";
    ret += "}\n";
    ret += "if (!folder.empty()) {\n";
    ret += "    listFiles(folder, files);\n";
    ret += "    if (!stringStartsWith(folder, \"\\\\\") && !stringStartsWith(folder, \"/\")) {\n";
    ret += "        folder += \"/\";\n";
    ret += "    }\n";
    ret += "}\n";

    //ret += "std::vector<std::string> files;\n"; //listFiles(\"" + ILTranslatorForCoverage::testCaseFileFolder + "\", files);\n";

    return ret;
}


std::string ILTransMain::translateForCoverageMainExecParameterCopyTraverse(std::string name, std::string type,
    std::vector<int> arraySize, bool isAddress) const
{
    string ret;
    if (isAddress)
        return ret;
    type = ILParser::getRealDataType(type);
    type = ILParser::getBasicDataTypeSimple(type);

    if (ILParser::isBasicType(type) && arraySize.empty())
    {
        int typeWidth = ILParser::getBasicDataTypeWidth(type);
        ret += "memcpy(&" + name + ", ";
        ret += "dataBuf + i * inputsDataLengthTotal + ";
        ret += to_string(inputsDataLengthTotal);
        ret += ", ";
        //ret += "stateTraceNode->inputVariablesOneStep->allInputVariable[" + to_string(execInputCopyId) + "].data, ";
        //ret += "sizeof(" + name + "));\n";
        ret += to_string(typeWidth) + ");\n";
        inputsDataLengthTotal += typeWidth;
        execInputCopyId++;
        return ret;
    }
    if (!arraySize.empty())
    {
        int count = arraySize[0];
        arraySize.erase(arraySize.begin());
        for (int i = 0; i < count; i++)
        {
            ret += translateForCoverageMainExecParameterCopyTraverse(name + "[" + to_string(i) + "]", type, arraySize, isAddress);
        }
        return ret;
    }

    ILStructDefine* structDefine = ILParser::findStructType(type);
    for (int i = 0; i < structDefine->members.size(); i++)
    {
        ILMember* member = structDefine->members[i];
        ret += translateForCoverageMainExecParameterCopyTraverse(name + "." + member->name, member->type, member->arraySize, member->isAddress);
    }
    return ret;
}
