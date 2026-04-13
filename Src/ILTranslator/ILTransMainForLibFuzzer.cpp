#include "ILTransInstrumentBitmapHelper.h"
#include "ILTransMain.h"

#include "ILTranslator.h"
#include "ILTranslatorForLibFuzzer.h"
#include "../Common/Utility.h"
#include "../ILBasic/Expression.h"

#include "../ILBasic/ILFunction.h"
#include "../ILBasic/ILInput.h"
#include "../ILBasic/ILOutput.h"
#include "../ILBasic/ILParser.h"
#include "../ILBasic/ILFile.h"
#include "../ILBasic/ILStructDefine.h"
#include "../ILBasic/ILMember.h"
#include "../TCGBasic/TCGConfig.h"

using namespace std;
namespace 
{
    int execInputCopyId = 0;
    int inputsDataLengthTotal = 0;

    int parameterCopyId = 0;
    int parasDataLengthTotal = 0;
}

int ILTransMain::translateForLibFuzzer(const ILParser* parser, string& code)
{
    iLParser = parser;
    
    int res = translateForLibFuzzerHeadFile();
    if(res < 0)
        return res;

	res = translateMainInstanceGlobalVariable();
    if(res < 0)
        return res;

    int branchArraySize = ILTransInstrumentBitmapHelper::branchBitmapCount;
    branchArraySize = branchArraySize == 0 ? 1 : branchArraySize;
    transCodeGlobalVariable += "unsigned char BranchCoverageLibFuzzer[" + to_string(branchArraySize) + "];\n";
    transCodeGlobalVariable += "long long BranchCoverageStartTime = 0;\n";

    if (ILTranslatorForLibFuzzer::configTargetModelPara){
        transCodeGlobalVariable += "char files[1000][256];\n";
        transCodeGlobalVariable += "uint8_t* fileData = NULL;\n";
        transCodeGlobalVariable += "size_t fileSize = 0;\n";
        transCodeGlobalVariable += "long long lastUpdateTime = 0;\n";
        transCodeGlobalVariable += "const int updateInterval = 10;\n";
        transCodeGlobalVariable += "long long iteratorCount = 0;\n";
    }

    string mainCodeStr;

    res = translateForLibFuzzerMainFunction(parser, mainCodeStr);
    if(res < 0)
        return res;
    transCodeFunction += mainCodeStr;

	string retCode;
	retCode += transCodeHeadFile + transCodeStructDefine;
	retCode += transCodeMacro + transCodeTypeDefine;
	retCode += transCodeGlobalVariable + transCodeUtilityFunction;
	retCode += transCodeFunction;
	code = retCode;
    
	return 1;
}

int ILTransMain::translateForLibFuzzerMainFunction(const ILParser* parser, std::string& code)
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


	retCode += commentStr + "extern \"C\" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {\n";
    




    string parametersCopyCode = translateForLibFuzzerMainParameterCopy(&iLParser->config);
    string inputsCopyCode = translateForLibFuzzerMainExecInputCopy(mainExecFunction);

    

    retCode += "int parameterLengthTotal = " + to_string(parasDataLengthTotal) + ";\n";
    retCode += "int inputsDataLengthTotal = " + to_string(inputsDataLengthTotal) + ";\n";
    retCode += "int sizeBuf = size;\n";

    if (!ILTranslatorForLibFuzzer::configTargetModelPara) {
        retCode += "if (size < parameterLengthTotal + inputsDataLengthTotal) {\n";
        retCode += "    sizeBuf = parameterLengthTotal + inputsDataLengthTotal;\n";
        retCode += "} else if ((sizeBuf - parameterLengthTotal) % inputsDataLengthTotal != 0) {\n";
        retCode += "    sizeBuf += (inputsDataLengthTotal - ((sizeBuf - parameterLengthTotal) % inputsDataLengthTotal));\n";
        retCode += "}\n";
    }
    else {
        retCode += "if (sizeBuf < parameterLengthTotal) {\n";
        retCode += "    sizeBuf = parameterLengthTotal;\n";
        retCode += "}\n";
    }

    retCode += "const uint8_t* dataBuf = new uint8_t[sizeBuf];\n";
    retCode += "memset((void*)dataBuf, 0, sizeBuf);\n";
    retCode += "memcpy((void*)dataBuf, data, size);\n";

    if (ILTranslatorForLibFuzzer::configTargetModelPara) {
        retCode += translateForLibFuzzerMainTargetParaReloadInputSequence();
    }

    retCode += parametersCopyCode;

    retCode += "BranchCoverageRegister(dataBuf, sizeBuf);\n";

    if (!config->mainInitFunction.empty()) {
        ILFunction* mainInitFunction = parser->findILFunction(config->mainInitFunction);
        if (!mainInitFunction)
            return -ILTranslator::ErrorCanNotFindMainInitFunction;

        //çćä¸ťInitĺ˝ć°çĺć°ĺé?        
        retCode += translateMainInitParameter(mainInitFunction);
        //çćä¸ťInitĺ˝ć°ďźĺćŹĺśĺć°
        retCode += translateMainInitFuncionCall(mainInitFunction);

        //retCode += config->mainInitFunction;
        //retCode += (mainInitFunction->inputs.size() == 1) ? "(&mainState);\n" : "(&mainState, NULL);\n";
    }

    if(ILTranslator::configGenerateComment)
    {
        commentStr = "/* Main Loop of model */\n";
    }

    string iteratorCountStr;
    if (ILTranslatorForLibFuzzer::configMaxIteratorCount == -1)
    {
        iteratorCountStr = "16777216";
    }
    else
    {
        iteratorCountStr = to_string(ILTranslatorForLibFuzzer::configMaxIteratorCount);
    }
    retCode += commentStr + "for (int i = 0; i < " + iteratorCountStr + "; i++) {\n";

    if (!ILTranslatorForLibFuzzer::configTargetModelPara) {
        retCode += "if((i + 1) * inputsDataLengthTotal + parameterLengthTotal > sizeBuf) {\n";
    }
    else {
        retCode += "if ((i + 1) * inputsDataLengthTotal + parameterLengthTotal > fileSize) {\n";
    }
    retCode += "break;\n";
    retCode += "}\n";
    
    //生成主函数的参数变量
    retCode += translateMainExecParameter(mainExecFunction);
    retCode += inputsCopyCode;
	//生成主函数，包括其参�?	
    retCode += translateMainExecFuncionCall(mainExecFunction);
    //生成主函数输出值的打印代码
    //retCode += translateMainExecOutputPrint(mainExecFunction);
	retCode += "}\n";
    retCode += "delete[] dataBuf;\n";
    retCode += "BranchCoverageRecordTestCase();\n";

    if (ILTranslatorForLibFuzzer::configTargetModelPara) {
        retCode += "iteratorCount++;\n";
    }
    retCode += "return 0;\n";

	retCode += "}\n";

    code = retCode;
    return 1;
}

int ILTransMain::translateForLibFuzzerHeadFile()
{
    //transCodeHeadFile += "#include \"LibFuzzerUtility.h\"\n";
    transCodeHeadFile += "#include <stdint.h>\n";
    transCodeHeadFile += "#include <stddef.h>\n";
    transCodeHeadFile += "#include <string.h>\n";
    return 1;
}

std::string ILTransMain::translateForLibFuzzerMainTargetParaReloadInputSequence() const
{
    string ret;

    ret += "if (iteratorCount % 100 == 0){\n";
    ret += "    long long currentTime = time(NULL);\n";
    ret += "    if (currentTime - lastUpdateTime >= updateInterval) {\n";
    ret += "        int numFiles = getFilesInDirectory(\"InputSequence\", files, 1000, 1);\n";
    ret += "        if (numFiles == 0) {\n";
    ret += "            return 0;\n";
    ret += "        }\n";
    ret += "        srand(time(NULL));\n";
    ret += "        int randomIndex = rand() % numFiles;\n";
    ret += "        char* selectedFile = files[randomIndex];\n";
    ret += "        fileSize = readFileData(selectedFile, &fileData, parameterLengthTotal + inputsDataLengthTotal);\n";
    ret += "        if (fileData == NULL) {\n";
    ret += "            return 0;\n";
    ret += "        }\n";
    ret += "        printf(\"===> Reload Input Sequence Data, file size: %d\\n\", fileSize);\n";
    ret += "        lastUpdateTime = currentTime;\n";
    ret += "        BranchCoverageRegister2(fileData + parameterLengthTotal, fileSize - parameterLengthTotal);\n";
    ret += "    }\n";
    ret += "}\n";

    return ret;
}

std::string ILTransMain::translateForLibFuzzerMainExecInputCopy(const ILFunction* mainExecFunction) const
{
    string ret;
    execInputCopyId = 0;
    inputsDataLengthTotal = 0;
    for(int i = 0; i < mainExecFunction->inputs.size(); i++) {
        ILInput* input = mainExecFunction->inputs[i];
		if(mainExecFunction->inputs[i]->type == iLParser->config.mainCompositeState) {
		    continue;
		}
        string paraName = "exec_" + input->name;
        ret += translateForLibFuzzerMainExecInputCopyTraverse(paraName, input->type, input->arraySize, input->isAddress);
	}
    return ret;
}


std::string ILTransMain::translateForLibFuzzerMainExecInputCopyTraverse(std::string name, std::string type,
    std::vector<int> arraySize, bool isAddress) const
{
    string ret;
    if(isAddress)
        return ret;
    type = ILParser::getRealDataType(type);
    type = ILParser::getBasicDataTypeSimple(type);
    TCGConfig* config = ILTranslatorForLibFuzzer::tcgConfig;
    
    if(ILParser::isBasicType(type) && arraySize.empty())
    {
        bool isConfigParameter = false;
        TCGConfig::TCGInput::TCGParameter* parameterPtr = nullptr;
        for (int i = 0; i < config->inputs.size() && ILTranslatorForLibFuzzer::configRangeConstraint; i++)
        {
            auto& input = config->inputs[i];
            for (auto& parameter : input.parameters)
            {
                if (name == "exec_" + parameter.expression && 
                    (!parameter.min.empty() || !parameter.max.empty()))
                {
                    isConfigParameter = true;
                    parameterPtr = &parameter;
                    break;
                }
            }
            if (isConfigParameter)
                break;
        }
        
        if (!isConfigParameter)
        {
            int typeWidth = ILParser::getBasicDataTypeWidth(type);
            ret += "memcpy(&" + name + ", ";
            if (!ILTranslatorForLibFuzzer::configTargetModelPara) {
                ret += "dataBuf + parameterLengthTotal + i * inputsDataLengthTotal + ";
            }
            else {
                ret += "fileData + parameterLengthTotal + i * inputsDataLengthTotal + ";
            }
            ret += to_string(inputsDataLengthTotal);
            ret += ", ";
            //ret += "stateTraceNode->inputVariablesOneStep->allInputVariable[" + to_string(execInputCopyId) + "].data, ";
            //ret += "sizeof(" + name + "));\n";
            ret += to_string(typeWidth) + ");\n";
            inputsDataLengthTotal += typeWidth;
            execInputCopyId++;
            return ret;
        }
        else {
            int typeWidth = ILParser::getBasicDataTypeWidth(type);
            ret += "DataMapping_" + parameterPtr->type + "((uint8_t*)&" + name + ", ";
            if (!ILTranslatorForLibFuzzer::configTargetModelPara) {
                ret += "dataBuf + parameterLengthTotal + i * inputsDataLengthTotal + ";
            }
            else {
                ret += "fileData + parameterLengthTotal + i * inputsDataLengthTotal + ";
            }
            ret += to_string(inputsDataLengthTotal);
            ret += ", ";
            //ret += "stateTraceNode->inputVariablesOneStep->allInputVariable[" + to_string(execInputCopyId) + "].data, ";
            //ret += "sizeof(" + name + "));\n";
            ret += to_string(typeWidth) + ", ";
            ret += ILTranslatorForLibFuzzer::getParameterTypeMin(parameterPtr->type, parameterPtr->min) + ", ";
            ret += ILTranslatorForLibFuzzer::getParameterTypeMax(parameterPtr->type, parameterPtr->max) + ");\n";
            /*ret += stoi(parameterPtr->min) + ", ";
            ret += stoi(parameterPtr->max) + "); \n";*/
            inputsDataLengthTotal += typeWidth;
            execInputCopyId++;
            return ret;
        }
    }
    if(!arraySize.empty())
    {
        int count = arraySize[0];
        arraySize.erase(arraySize.begin());
        for(int i = 0; i < count && i < ILTranslatorForLibFuzzer::configCutOffArray; i++)
        {
            ret += translateForLibFuzzerMainExecInputCopyTraverse(name + "[" + to_string(i) + "]", type, arraySize, isAddress);
        }
        return ret;
    }

    ILStructDefine* structDefine = ILParser::findStructType(type);
    for(int i = 0; i < structDefine->members.size(); i++)
    {
        ILMember* member = structDefine->members[i];
        ret += translateForLibFuzzerMainExecInputCopyTraverse(name + "." + member->name, member->type, member->arraySize, member->isAddress);
    }
    return ret;
}


std::string ILTransMain::translateForLibFuzzerMainParameterCopy(const ILConfig* config) const
{
    string ret;
    parameterCopyId = 0;
    parasDataLengthTotal = 0;
    for (int i = 0; i < config->parameters.size(); i++) {
        const ILConfigParameter* para = &config->parameters[i];
        
        string paraName = para->name;
        ret += translateForLibFuzzerMainParameterCopyTraverse(paraName, para->type, para->arraySize, para->isAddress);
    }
    return ret;
}


std::string ILTransMain::translateForLibFuzzerMainParameterCopyTraverse(std::string name, std::string type,
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
        ret += "dataBuf + ";
        ret += to_string(parasDataLengthTotal);
        ret += ", ";
        ret += to_string(typeWidth) + ");\n";
        parasDataLengthTotal += typeWidth;
        parameterCopyId++;
        return ret;
    }
    if (!arraySize.empty())
    {
        int count = arraySize[0];
        arraySize.erase(arraySize.begin());
        for (int i = 0; i < count && i < ILTranslatorForLibFuzzer::configCutOffArray; i++)
        {
            ret += translateForLibFuzzerMainParameterCopyTraverse(name + "[" + to_string(i) + "]", type, arraySize, isAddress);
        }
        return ret;
    }

    ILStructDefine* structDefine = ILParser::findStructType(type);
    for (int i = 0; i < structDefine->members.size(); i++)
    {
        ILMember* member = structDefine->members[i];
        ret += translateForLibFuzzerMainParameterCopyTraverse(name + "." + member->name, member->type, member->arraySize, member->isAddress);
    }
    return ret;
}
