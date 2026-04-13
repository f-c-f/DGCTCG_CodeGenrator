#include "ILTransMain.h"

#include "ILTranslator.h"
#include "../Common/Utility.h"
#include "../ILBasic/Expression.h"

#include "../ILBasic/ILFunction.h"
#include "../ILBasic/ILInput.h"
#include "../ILBasic/ILOutput.h"
#include "../ILBasic/ILParser.h"
#include "../ILBasic/ILFile.h"

using namespace std;

int ILTransMain::translateForC(const ILParser* parser, string& code)
{
    iLParser = parser;
    

	int res = translateMainInstanceGlobalVariable();
    if(res < 0)
        return res;

    string mainCodeStr;

    res = translateForCMainFunction(parser, mainCodeStr);
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

int ILTransMain::translateForCMainFunction(const ILParser* parser, std::string& code)
{
    string retCode;
    string commentStr;
	const ILConfig* config = &(parser->config);
    int iterationCount = config->iterationCount;

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


	retCode += commentStr + "int main(int argc, char *argv[]) {\n";
    
    if(ILTranslatorForC::configTestTime)
    {
        retCode += "clock_t testStartTime = clock();\n";
        retCode += "int testCount = (argc > 1 ? atoi(argv[1]) : 1);\n";
        retCode += "for (int ti = 0; ti < testCount; ti++) {\n";
    }
    if(ILTranslatorForC::configTestCorrectness)
    {
        retCode += "FILE *file = fopen(argv[1], \"r\"); // Open the CSV file for reading\n";
        retCode += "if (file == NULL) {\n";
        retCode += "    perror(\"Error opening file\");\n";
        retCode += "    return 1;\n";
        retCode += "}\n";
        retCode += "float step;\n";

        iterationCount = 16777216;
    }


	if(!config->mainInitFunction.empty()) {
		ILFunction* mainInitFunction = parser->findILFunction(config->mainInitFunction);
        if(!mainInitFunction)
            return -ILTranslator::ErrorCanNotFindMainInitFunction;

        //生成主Init函数的参数变�?        retCode += translateMainInitParameter(mainInitFunction);
        //生成主Init函数，包括其参数
        retCode += translateMainInitFuncionCall(mainInitFunction);

		//retCode += config->mainInitFunction;
        //retCode += (mainInitFunction->inputs.size() == 1) ? "(&mainState);\n" : "(&mainState, NULL);\n";
	}


    if(ILTranslator::configGenerateComment)
    {
        commentStr = "/* Main Loop of model */\n";
    }
	retCode += commentStr;
    retCode += "for (int i = 0; i < " + to_string(iterationCount) + "; i++) {\n";

    
    //生成主函数的参数变量
    retCode += translateMainExecParameter(mainExecFunction);

    if(ILTranslatorForC::configTestCorrectness)
    {
        retCode += translateMainExecParameterAssignment(mainExecFunction);
    }

	//生成主函数，包括其参�?	
    retCode += translateMainExecFuncionCall(mainExecFunction);
    //生成主函数输出值的打印代码
    retCode += translateMainExecOutputPrint(mainExecFunction);
	retCode += "}\n";
	
    
    retCode += ILTranslatorForC::configTestTime ? "}\n" : "";

    if(ILTranslatorForC::configTestTime)
    {
        retCode += "clock_t testEndTime = clock();\n";
        retCode += "long long testCostTime = (testEndTime - testStartTime);\n";
        retCode += "#undef printf(format, ...)\n";
        retCode += "printf(\"%ld\\n\", testCostTime);\n";
        retCode += "#define printf(format, ...)\n";
        retCode += "#define printf(format,)\n";
    }


    if(ILTranslatorForC::configTestCorrectness)
    {
        retCode += "fclose(file); // Close the file\n";
    }

	retCode += "}\n";

    code = retCode;
    return 1;
}
