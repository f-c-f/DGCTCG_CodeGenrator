#include "ILTransForLibFuzzerTestCaseConverter.h"

#include "ILTransInstrumentBitmapHelper.h"
#include "../Common/Utility.h"
#include "../ILBasic/ILParser.h"
#include "../ILBasic/ILConfig.h"
#include "../ILBasic/ILFile.h"
#include "../TCGBasic/TCGConfig.h"
#include "ILTranslator.h"


#include "ILTranslatorForLibFuzzer.h"

#include "../ILBasic/ILFunction.h"
#include "../ILBasic/ILInput.h"
#include "../ILBasic/ILStructDefine.h"
#include "../ILBasic/ILMember.h"
#include "../ILBasic/Expression.h"


using namespace std;


ILTransForLibFuzzerTestCaseConverter::ILTransForLibFuzzerTestCaseConverter(const ILParser* parser)
{
    this->parser = parser;
}

int ILTransForLibFuzzerTestCaseConverter::translate(std::string outputDir)
{
    int res;
    string oriOutputDir = outputDir;
    if(!stringEndsWith(outputDir, "/") && !stringEndsWith(outputDir, "\\"))
        outputDir += "/";
    outputDir += "TestCaseConverter";
    if(!isFolderExist(outputDir))
        createFolder(outputDir);

    res = translateMainFile(outputDir);
    if(res < 0)
        return res;

    copyFileOne(oriOutputDir + "/define.h", outputDir + "/define.h");

    res = copyResourceFiles(outputDir);
    if(res < 0)
        return res;

    res = generateOneKeyBuildScript(outputDir);
    if(res < 0)
        return res;



    return 1;
}

namespace 
{
    const vector<string> copyFileList = {
        "Utility.cpp",
        "Utility.h",
        "BasicExcel.cpp",
        "BasicExcel.hpp",
        "LibFuzzerUtility.c",
        "LibFuzzerUtility.h",
        "TestCaseConverter.cpp",
        "TestCaseConverter.h",
        "TestCaseConverterMain.cpp",
    };
}

int ILTransForLibFuzzerTestCaseConverter::copyResourceFiles(std::string outputDir) const
{
    string exeFilePath = getExeDirPath();
    for(int i = 0; i < copyFileList.size(); i++)
    {
        if(!isFileExist(exeFilePath + "/ResourceCode/" + copyFileList[i]))
            return -ILTranslator::ErrorResourceFileNotExist;
        copyFileOne(exeFilePath + "/ResourceCode/" + copyFileList[i], outputDir + "/" + copyFileList[i]);
    }
    return 1;
}

int ILTransForLibFuzzerTestCaseConverter::generateOneKeyBuildScript(std::string outputDir) const
{
    string scriptStr;
    scriptStr = "g++ -O2";
    for(int i = 0; i < copyFileList.size(); i++)
    {
        if(stringEndsWith(copyFileList[i], ".h") || stringEndsWith(copyFileList[i], ".hpp"))
            continue;
        scriptStr += " " + copyFileList[i];
    }
    scriptStr += " BinaryToTestCase.cpp";
    scriptStr += " -o Converter.exe\n";
    writeFile(outputDir + "/Build.bat", scriptStr);

    scriptStr = "Converter.exe ../Out/ ../TestCaseOutput\n";
    writeFile(outputDir + "/Run.bat", scriptStr);

    return 1;
}


namespace 
{
    int execInputCopyId = 0;
    int inputsDataLengthTotal = 0;

    int parameterCopyId = 0;
    int parasDataLengthTotal = 0;
}

int ILTransForLibFuzzerTestCaseConverter::translateMainFile(std::string outputDir)
{
    int res = translateMainFileHeadFile();
    if(res < 0)
        return res;
    
    string mainCodeStr;

    res = translateForLibFuzzerMainFunction(mainCodeStr);
    if(res < 0)
        return res;
    transCodeFunction += mainCodeStr;

	string retCode;
	retCode += transCodeHeadFile;
	retCode += transCodeFunction;

    retCode = codeIndentProcess(retCode);

    writeFile(outputDir + "/BinaryToTestCase.cpp", retCode);
    
	return 1;
}

int ILTransForLibFuzzerTestCaseConverter::translateForLibFuzzerMainFunction(std::string& code)
{
    string retCode;
    string commentStr;
	const ILConfig* config = &(parser->config);
    if(ILTranslator::configGenerateComment)
    {
        commentStr = "/* Main Function of model */\n";
    }

    ILFunction* mainExecFunction = nullptr;
    if(!config->mainExecFunction.empty()) {
		mainExecFunction = parser->findILFunction(config->mainExecFunction);
        if(!mainExecFunction)
            return -ILTranslator::ErrorCanNotFindMainExecFunction;
    }
    else
    {
        return -ILTranslator::ErrorCanNotFindMainExecFunction;
    }

    int branchArraySize = ILTransInstrumentBitmapHelper::branchBitmapCount;
    branchArraySize = branchArraySize == 0 ? 1 : branchArraySize;
    retCode += "unsigned char BranchCoverageLibFuzzer[" + to_string(branchArraySize) + "];\n";
    retCode += "long long BranchCoverageStartTime = 0;\n";

	retCode += commentStr + "TestCaseConverter::TestCase TestOneInput(const uint8_t *data, size_t size) {\n";
    retCode += "TestCaseConverter::TestCase tempTestCase;\n";

    // 该语句必须在"inputsDataLengthTotal"变量使用之前，因为该函数会计算该变量的值

    string parametersCopyCode = translateForLibFuzzerMainParameterCopy(&parser->config);
    string inputsCopyCode = translateForLibFuzzerMainExecInputCopy(mainExecFunction);
    retCode += "int parameterLengthTotal = " + to_string(parasDataLengthTotal) + ";\n";
    retCode += "int inputsDataLengthTotal = " + to_string(inputsDataLengthTotal) + ";\n";
    retCode += "int sizeBuf = size;\n";
    retCode += "if (size < parameterLengthTotal + inputsDataLengthTotal) {\n";
    retCode += "    sizeBuf = parameterLengthTotal + inputsDataLengthTotal;\n";
    retCode += "} else if ((sizeBuf - parameterLengthTotal) % inputsDataLengthTotal != 0) {\n";
    retCode += "    sizeBuf += (inputsDataLengthTotal - ((sizeBuf - parameterLengthTotal) % inputsDataLengthTotal));\n";
    retCode += "}\n";
    retCode += "const uint8_t* dataBuf = new uint8_t[sizeBuf];\n";
    retCode += "memset((void*)dataBuf, 0, sizeBuf);\n";
    retCode += "memcpy((void*)dataBuf, data, size);\n";

    retCode += translateMainParameter(&parser->config);
    retCode += parametersCopyCode;

    if(ILTranslator::configGenerateComment)
    {
        commentStr = "/* Main Loop of model */\n";
    }
	retCode += commentStr + "for (int i = 0; i < 16777216; i++) {\n";

    retCode += "if((i + 1) * inputsDataLengthTotal > sizeBuf) {\n";
    retCode += "break;\n";
    retCode += "}\n";
    retCode += "TestCaseConverter::InputVariableOneStep* inputVariableOneStep = new TestCaseConverter::InputVariableOneStep();\n";
    //生成主函数的参数变量
    retCode += translateMainExecInput(mainExecFunction);

    
    retCode += inputsCopyCode;

    retCode += "tempTestCase.timeList.push_back(i);\n";
    retCode += "tempTestCase.inputList.push_back(inputVariableOneStep);\n";
    retCode += "tempTestCase.length = i + 1;\n";

	retCode += "}\n";

    retCode += "delete[] dataBuf;\n";
    retCode += "return tempTestCase;\n";

	retCode += "}\n";

    code += retCode;
    return 1;
}

int ILTransForLibFuzzerTestCaseConverter::translateMainFileHeadFile()
{
    transCodeHeadFile += "#include <stdint.h>\n";
    transCodeHeadFile += "#include <stddef.h>\n";
    transCodeHeadFile += "#include <string.h>\n";
    transCodeHeadFile += "#include <stdio.h>\n";
    transCodeHeadFile += "#include <iostream>\n";
    transCodeHeadFile += "#include \"define.h\"\n";
    transCodeHeadFile += "#include \"LibFuzzerUtility.h\"\n";
    transCodeHeadFile += "#include \"TestCaseConverter.h\"\n";
    transCodeHeadFile += "#include \"Utility.h\"\n";
    transCodeHeadFile += "using namespace std;\n";
    
    return 1;
}

std::string ILTransForLibFuzzerTestCaseConverter::translateForLibFuzzerMainExecInputCopy(const ILFunction* mainExecFunction) const
{
    string ret;
    execInputCopyId = 0;
    inputsDataLengthTotal = 0;
    for(int i = 0; i < mainExecFunction->inputs.size(); i++) {
        ILInput* input = mainExecFunction->inputs[i];
		if(mainExecFunction->inputs[i]->type == parser->config.mainCompositeState) {
		    continue;
		}
        string paraName = "exec_" + input->name;
        ret += translateForLibFuzzerMainExecInputCopyTraverse(paraName, input->type, input->arraySize, input->isAddress, false);
	}
    return ret;
}


std::string ILTransForLibFuzzerTestCaseConverter::translateForLibFuzzerMainExecInputCopyTraverse(std::string name, std::string type,
    std::vector<int> arraySize, bool isAddress, bool outOfArrayCut) const
{
    string ret;
    if(isAddress)
        return ret;
    type = ILParser::getRealDataType(type);
    type = ILParser::getBasicDataTypeSimple(type);
    TCGConfig* config = ILTranslatorForLibFuzzer::tcgConfig;
    TCGConfig::TCGInput::TCGParameter* parameterPtr = nullptr;

    if(ILParser::isBasicType(type) && arraySize.empty())
    {
        bool isConfigParameter = false;
        for (auto& input : config->inputs)
        {
            for (auto& parameter : input.parameters)
            {
                if (name == "exec_" + parameter.expression)
                {
                    isConfigParameter = true;
                    parameterPtr = &parameter;
                    break;
                }
            }
            if (isConfigParameter)
                break;
        }

        int typeWidth = ILParser::getBasicDataTypeWidth(type);

        if(!outOfArrayCut)
        {
            if (!isConfigParameter)
            {
                ret += "memcpy(&" + name + ", ";
                ret += "dataBuf + parameterLengthTotal + i * inputsDataLengthTotal + ";
                ret += to_string(inputsDataLengthTotal);
                ret += ", ";
                ret += to_string(typeWidth) + ");\n";
            } else {
                ret += "DataMapping_" + parameterPtr->type + "((uint8_t*)&" + name + ", ";
                ret += "dataBuf + parameterLengthTotal + i * inputsDataLengthTotal + ";
                ret += to_string(inputsDataLengthTotal);
                ret += ", ";
                ret += to_string(typeWidth) + ", ";
                ret += ILTranslatorForLibFuzzer::getParameterTypeMin(parameterPtr->type, parameterPtr->min) + ", ";
                ret += ILTranslatorForLibFuzzer::getParameterTypeMax(parameterPtr->type, parameterPtr->max) + ");\n";
            }
        }

        string oriPortName = name.substr(5, name.length() - 5);
        string oriPortNameDef = oriPortName;
        oriPortNameDef = stringReplaceAll(oriPortNameDef, "[", "_");
        oriPortNameDef = stringReplaceAll(oriPortNameDef, "]", "_");
        ret += "TestCaseConverter::InputVariableRecord record_";
        ret += oriPortNameDef + "((char*)&" + name + ", ";
        ret += to_string(typeWidth) + ", ";
        ret += "\"" + type + "\", ";
        ret += "\"" + oriPortName + "\");\n";
        
        ret += "inputVariableOneStep->allInputVariable.push_back(record_" + oriPortNameDef + ");\n";

        if (!outOfArrayCut)
        {
            inputsDataLengthTotal += typeWidth;
            execInputCopyId++;
        }

        return ret;
    }
    if(!arraySize.empty())
    {
        int count = arraySize[0];
        arraySize.erase(arraySize.begin());
        for(int i = 0; i < count; i++) // && i < ILTranslatorForLibFuzzer::configCutOffArray
        {
            ret += translateForLibFuzzerMainExecInputCopyTraverse(name + "[" + to_string(i) + "]", type, arraySize, isAddress, i >= ILTranslatorForLibFuzzer::configCutOffArray);
        }
        return ret;
    }

    ILStructDefine* structDefine = ILParser::findStructType(type);
    for(int i = 0; i < structDefine->members.size(); i++)
    {
        ILMember* member = structDefine->members[i];
        ret += translateForLibFuzzerMainExecInputCopyTraverse(name + "." + member->name, member->type, member->arraySize, member->isAddress, false);
    }
    return ret;
}

std::string ILTransForLibFuzzerTestCaseConverter::translateMainExecInput(const ILFunction* mainExecFunction) const
{
    string ret;

	for (int i = 0; i < mainExecFunction->inputs.size(); i++) {
		if (mainExecFunction->inputs[i]->type == parser->config.mainCompositeState)
			continue;

		ret += translateMainInput(mainExecFunction->inputs[i], "exec_");
	}
        
    return ret;
}

std::string ILTransForLibFuzzerTestCaseConverter::translateMainInput(const ILInput* input, std::string perfix) const
{
    string variableStr;
    string type = ILParser::convertDataType(input->type);
    variableStr += type;
	if(input->isAddress > 0)
		variableStr += " " + stringRepeat("*", input->isAddress);
	variableStr += " " + perfix + input->name;
	if(!input->arraySize.empty())
	{
		for(int i = 0;i < input->arraySize.size(); i++)
		{
			variableStr += "[" + to_string(input->arraySize[i]) + "]";
		}
	}
	if(input->defaultValue)
	{
		variableStr += " = " + input->defaultValue->expressionStr;
	}
    else if(!ILParser::isBasicType(type) && !ILTranslator::translateForCBMC)
    {
        variableStr += " = {}";
    }
	variableStr += ";\n";
    return variableStr;
}


std::string ILTransForLibFuzzerTestCaseConverter::translateForLibFuzzerMainParameterCopy(const ILConfig* config) const
{
    string ret;
    parameterCopyId = 0;
    parasDataLengthTotal = 0;
    for (int i = 0; i < config->parameters.size(); i++) {
        const ILConfigParameter* para = &config->parameters[i];
        string paraName = "para_" + para->name;
        ret += translateForLibFuzzerMainParameterCopyTraverse(paraName, para->type, para->arraySize, para->isAddress, false);
    }
    return ret;
}


std::string ILTransForLibFuzzerTestCaseConverter::translateForLibFuzzerMainParameterCopyTraverse(std::string name, std::string type,
    std::vector<int> arraySize, bool isAddress, bool outOfArrayCut) const
{
    string ret;
    if (isAddress)
        return ret;
    type = ILParser::getRealDataType(type);
    type = ILParser::getBasicDataTypeSimple(type);

    if (ILParser::isBasicType(type) && arraySize.empty())
    {
        
        int typeWidth = ILParser::getBasicDataTypeWidth(type);

        if (!outOfArrayCut)
        {
            ret += "memcpy(&" + name + ", ";
            ret += "dataBuf + ";
            ret += to_string(parasDataLengthTotal);
            ret += ", ";
            ret += to_string(typeWidth) + ");\n";
        }


        string oriPortName = name.substr(5, name.length() - 5);
        string oriPortNameDef = oriPortName;
        oriPortNameDef = stringReplaceAll(oriPortNameDef, "[", "_");
        oriPortNameDef = stringReplaceAll(oriPortNameDef, "]", "_");
        ret += "TestCaseConverter::Parameter* parameter_";
        ret += oriPortNameDef + " = new TestCaseConverter::Parameter((char*)&" + name + ", ";
        ret += to_string(typeWidth) + ", ";
        ret += "\"" + type + "\", ";
        ret += "\"" + oriPortName + "\");\n";

        ret += "tempTestCase.parameters.push_back(parameter_" + oriPortNameDef + ");\n";

        if (!outOfArrayCut)
        {
            parasDataLengthTotal += typeWidth;
            parameterCopyId++;
        }

        return ret;
    }
    if (!arraySize.empty())
    {
        int count = arraySize[0];
        arraySize.erase(arraySize.begin());
        for (int i = 0; i < count; i++) // && i < ILTranslatorForLibFuzzer::configCutOffArray
        {
            ret += translateForLibFuzzerMainParameterCopyTraverse(name + "[" + to_string(i) + "]", type, arraySize, isAddress, i >= ILTranslatorForLibFuzzer::configCutOffArray);
        }
        return ret;
    }

    ILStructDefine* structDefine = ILParser::findStructType(type);
    for (int i = 0; i < structDefine->members.size(); i++)
    {
        ILMember* member = structDefine->members[i];
        ret += translateForLibFuzzerMainParameterCopyTraverse(name + "." + member->name, member->type, member->arraySize, member->isAddress, false);
    }
    return ret;
}

std::string ILTransForLibFuzzerTestCaseConverter::translateMainParameter(const ILConfig* config) const
{
    string ret;

    for (int i = 0; i < config->parameters.size(); i++) {
        const ILConfigParameter* para = &config->parameters[i];


        ret += translateMainParameter(para, "para_");
    }

    return ret;
}

std::string ILTransForLibFuzzerTestCaseConverter::translateMainParameter(const ILConfigParameter* para, std::string perfix) const
{
    string variableStr;
    string type = ILParser::convertDataType(para->type);
    variableStr += type;
    if (para->isAddress > 0)
        variableStr += " " + stringRepeat("*", para->isAddress);
    variableStr += " " + perfix + para->name;
    if (!para->arraySize.empty())
    {
        for (int i = 0; i < para->arraySize.size(); i++)
        {
            variableStr += "[" + to_string(para->arraySize[i]) + "]";
        }
    }
    if (!para->defaultValue.empty())
    {
        variableStr += " = " + para->defaultValue;
    }
    else if (!ILParser::isBasicType(type) && !ILTranslator::translateForCBMC)
    {
        variableStr += " = {}";
    }
    variableStr += ";\n";
    return variableStr;
}
