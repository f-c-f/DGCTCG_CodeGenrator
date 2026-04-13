#include "ILTransMain.h"

#include "ILTranslator.h"
#include "../Common/Utility.h"
#include "../ILBasic/Expression.h"

#include "../ILBasic/ILStructDefine.h"
#include "../ILBasic/ILMember.h"
#include "../ILBasic/ILGlobalVariable.h"
#include "../ILBasic/ILFunction.h"
#include "../ILBasic/ILInput.h"
#include "../ILBasic/ILOutput.h"
#include "../ILBasic/ILParser.h"
#include "../ILBasic/ILFile.h"
#include "../TCGBasic/TCGConfig.h"

using namespace std;

int ILTransMain::translateForCBMC(const ILParser* parser, string& code)
{
    int res;

    iLParser = parser;
    string commentStr;
    
	res = translateMainInstanceGlobalVariable();
    if(res < 0)
        return res;
    
    if(ILTranslatorForCBMC::configGlobalVariableReset)
    {
        string globalVariableResetCode;
        res = translateForCBMCGlobalVariableResetFunction(parser, globalVariableResetCode);
        if(res < 0)
            return res;
        transCodeFunction += globalVariableResetCode;
    }
    

    string mainCodeStr;
    res = translateForCBMCMainFunction(parser, mainCodeStr);
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

int ILTransMain::translateForCBMCMainFunction(const ILParser* parser, std::string& code)
{
    int res;
    string retCode;
    string commentStr;
	const ILConfig* config = &(parser->config);
    if(ILTranslator::configGenerateComment)
    {
        commentStr = "/* Main Function of model */\n";
    }
    
	retCode += commentStr + "int main() {\n";
    
    
    if(ILTranslatorForCBMC::configGenerateMultiEntries) {
        res = translateMainFunctionMultiEntries(parser, retCode);
    } else {
        res = translateMainFunctionOneEntry(parser, retCode);
    }
    if(res < 0)
        return res;
    retCode += "return 0;\n";
	retCode += "}\n";

    code = retCode;
    return 1;
}

int ILTransMain::translateForCBMCGlobalVariableResetFunction(const ILParser* parser, std::string& code)
{
    string retCode = "void globalVariableInit() {\n";
    for(int i = 0; i < iLParser->files.size(); i++)
    {
        ILFile* iLFile = iLParser->files[i];
        for(int j = 0; j < iLFile->globalVariables.size(); j++)
        {
            ILGlobalVariable* iLGlobalVariable = iLFile->globalVariables[j];
            string codeTemp = translateForCBMCGlobalVariableResetFunctionOne(iLGlobalVariable);
            retCode += codeTemp;
        }
    }
    retCode += "}\n";
    code = retCode;
    return 1;
}

std::string ILTransMain::translateForCBMCGlobalVariableResetFunctionOne(ILGlobalVariable* iLGlobalVariable)
{
    string retCode;
    if(iLGlobalVariable->isAddress)
        return retCode;
    translateForCBMCGlobalVariableResetFunctionOne(iLGlobalVariable->name, iLGlobalVariable->type, iLGlobalVariable->arraySize, retCode);
    return retCode;
}

void ILTransMain::translateForCBMCGlobalVariableResetFunctionOne(std::string varName, std::string type,
    std::vector<int> arraySize, std::string& retCode)
{
    string simpleType = ILParser::getRealDataType(type);
    simpleType = ILParser::getBasicDataTypeSimple(simpleType);

    string tempCode;
    if(ILParser::isBasicType(simpleType)) {
        tempCode += varName;
        tempCode += "; // Need to be modified: Variable ";
        tempCode += varName + "\n";
        
        retCode += tempCode;
        return;
    }

    if(!arraySize.empty()) {
        vector<int> arraySizeNew;
        arraySizeNew.insert(arraySizeNew.begin(), arraySize.begin() + 1, arraySize.end());
        for(int i = 0; i < arraySize[0] && i < ILTranslatorForCBMC::configCutOffArray; i++)
        {
            string varNameNew = varName + "[" + to_string(i) + "]";
            translateForCBMCGlobalVariableResetFunctionOne(varNameNew, type, arraySizeNew, retCode);
        }
        return;
    }

    ILStructDefine* structDefine = ILParser::findStructType(type);
    for(int i = 0; i < structDefine->members.size(); i++)
    {
        ILMember* iLMember = structDefine->members[i];
        if(iLMember->isAddress)
            continue;
        string varNameNew = varName + "." + iLMember->name;
        string typeNew = iLMember->type;
        vector<int> arraySizeNew = iLMember->arraySize;
        translateForCBMCGlobalVariableResetFunctionOne(varNameNew, typeNew, arraySizeNew, retCode);
    }
}

int ILTransMain::translateMainFunctionOneEntry(const ILParser* parser, std::string& code) const
{

    if(!parser->config.mainInitFunction.empty()) {
		ILFunction* mainInitFunction = parser->findILFunction(parser->config.mainInitFunction);
        if(!mainInitFunction)
            return -ILTranslator::ErrorCanNotFindMainInitFunction;

        //生成主Init函数的参数变�?        
        code += translateMainInitParameter(mainInitFunction);

        //生成主Init函数，包括其参数
        code += translateMainInitFuncionCall(mainInitFunction);

		//retCode += config->mainInitFunction;
        //retCode += (mainInitFunction->inputs.size() == 1) ? "(&mainState);\n" : "(&mainState, NULL);\n";
	}

    if(ILTranslatorForCBMC::configGlobalVariableReset)
    {
        code += "globalVariableInit();\n";
    }

    if (ILTranslatorForCBMC::configBranchCover && ILTranslatorForCBMC::configGenSolvingPara)
    {
        for (int i = 0; i < iLParser->config.parameters.size(); i++)
        {
            code += translateMainFunctionParameterCover(&iLParser->config.parameters[i]);
        }
    }

	if(!parser->config.mainExecFunction.empty()) {
		ILFunction* mainExecFunction = parser->findILFunction(parser->config.mainExecFunction);
        if(!mainExecFunction)
            return -ILTranslator::ErrorCanNotFindMainExecFunction;
    
        for(int i = 0; i < ILTranslatorForCBMC::configMultiStep; i++)
        {
            //生成主函数的参数变量
            code += translateMainExecParameter(mainExecFunction, i + 1);
            //生成主函数，包括其参�?            
            code += translateMainExecFuncionCall(mainExecFunction, i + 1);
        }
	}
    return 1;
}

int ILTransMain::translateMainFunctionMultiEntries(const ILParser* parser, std::string& code) const
{
    for(int i = 0; i < parser->files.size(); i++)
    {
        ILFile* iLFile = parser->files[i];
        for(int j = 0 ; j < iLFile->functions.size(); j++)
        {
            ILFunction* iLFunction = iLFile->functions[j];
            if(iLFunction->type != ILFunction::FunctionExec)
            {
                continue;
            }
            code += "{ // Call " + iLFunction->name + " for verification.\n";

            code += translateMainExecParameter(iLFunction);
            code += translateMainExecFuncionCall(iLFunction);


            code += "}\n";
        }
    }
    return 1;
}

std::string ILTransMain::translateMainFunctionInputCover(ILInput* input, int step) const
{
    string ret;
    string stepPerfix = (step == 1) ? "" : (to_string(step) + "_");

    string paraName = "exec_" + stepPerfix + input->name;
    ret = translateMainFunctionInputCoverTraverse(paraName, input->type, input->arraySize, input->isAddress);
    return ret;
}

std::string ILTransMain::translateMainFunctionInputCoverTraverse(std::string name, std::string type,
    std::vector<int> arraySize, bool isAddress) const
{
    string ret;
    if(isAddress)
        return ret;
    type = ILParser::getRealDataType(type);
    type = ILParser::getBasicDataTypeSimple(type);
    if(ILParser::isBasicType(type) && arraySize.empty())
    {
        ret = "__CPROVER_input(\"" + name + "\", " + name + ");\n";
        return ret;
    }
    if(!arraySize.empty())
    {
        int count = arraySize[0];
        arraySize.erase(arraySize.begin());
        for(int i = 0; i < count && i < ILTranslatorForCBMC::configCutOffArray; i++)
        {
            ret += translateMainFunctionInputCoverTraverse(name + "[" + to_string(i) + "]", type, arraySize, isAddress);
        }
        return ret;
    }

    ILStructDefine* structDefine = ILParser::findStructType(type);
    for(int i = 0; i < structDefine->members.size(); i++)
    {
        ILMember* member = structDefine->members[i];
        ret += translateMainFunctionInputCoverTraverse(name + "." + member->name, member->type, member->arraySize, member->isAddress);
    }
    return ret;
}


std::string ILTransMain::translateMainFunctionParameterCover(const ILConfigParameter* para) const
{
    string ret;

    string paraName = para->name;
    ret = translateMainFunctionParameterCoverTraverse(paraName, para->type, para->arraySize, para->isAddress);
    return ret;
}

std::string ILTransMain::translateMainFunctionParameterCoverTraverse(std::string name, std::string type,
    std::vector<int> arraySize, bool isAddress) const
{
    string ret;
    if (isAddress)
        return ret;
    type = ILParser::getRealDataType(type);
    type = ILParser::getBasicDataTypeSimple(type);
    if (ILParser::isBasicType(type) && arraySize.empty())
    {
        string simplifyType = ILParser::getBasicDataTypeSimple(type);

        ret = name + " = " + ILTranslator::cbmcTypeMap.at(simplifyType) + ";\n";
        ret += "__CPROVER_input(\"" + name + "\", " + name + ");\n";
        return ret;
    }
    if (!arraySize.empty())
    {
        int count = arraySize[0];
        arraySize.erase(arraySize.begin());
        for (int i = 0; i < count && i < ILTranslatorForCBMC::configCutOffArray; i++)
        {
            ret += translateMainFunctionParameterCoverTraverse(name + "[" + to_string(i) + "]", type, arraySize, isAddress);
        }
        return ret;
    }

    ILStructDefine* structDefine = ILParser::findStructType(type);
    for (int i = 0; i < structDefine->members.size(); i++)
    {
        ILMember* member = structDefine->members[i];
        ret += translateMainFunctionParameterCoverTraverse(name + "." + member->name, member->type, member->arraySize, member->isAddress);
    }
    return ret;
}


std::string ILTransMain::translateMainFunctionRangeConstraint(int step) const
{
    std::string ret;
    TCGConfig* config = ILTranslatorForCBMC::tcgConfig;

    string stepPerfix = (step == 1) ? "" : (to_string(step) + "_");

    for (auto& input : config->inputs)
    {
        for (auto& parameter : input.parameters)
        {
            std::string name = "exec_" + stepPerfix + parameter.expression;
            if (!parameter.max.empty())
            {
                ret += "__CPROVER_asusme(" + name + " <= " + parameter.max + ");\n";
            }
            if (!parameter.min.empty())
            {
                ret += "__CPROVER_asusme(" + name + " >= " + parameter.min + ");\n";
            }
        }
    }

    return ret;
}
