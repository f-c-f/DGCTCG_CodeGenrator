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

int ILTransMain::translateForSimulator(const ILParser* parser, string& code)
{
    iLParser = parser;
    

	int res = translateMainInstanceGlobalVariable();
    if(res < 0)
        return res;

    string mainCodeStr;

    res = translateForSimulatorMainFunction(parser, mainCodeStr);
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

int ILTransMain::translateForSimulatorMainFunction(const ILParser* parser, std::string& code)
{
    string retCode;
    string commentStr;
	const ILConfig* config = &(parser->config);

    //retCode += "#define TOTAL_STEP " + to_string(config->iterationCount) + "\n";
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

    retCode += "Start_Time = clock();\n";

    retCode += "if (!SimulatorBasic::initArgument(argc, argv))\n";
    retCode += "\t\treturn 0;\n";
    retCode += "if (!SimulatorTestCase::initTestCase())\n";
    retCode += "\t\treturn 0;\n";

    retCode += "SimulatorMonitor::OutputData = new MonitorOutputData*[" + to_string(ILTranslatorForSimulator::needMonitor.size()) + "]; \n";
        
    for(auto it = ILTranslatorForSimulator::needMonitor.begin(); it != ILTranslatorForSimulator::needMonitor.end(); it++)
    {
        //auto outID = it->second;
        retCode += "SimulatorMonitor::monitorRegisterOutputData(" + to_string(it->second.first);
        
        retCode += ", SimulatorBasic::totalStep, \"" + it->second.second + "\", ";

        int datatypeLength = 0;
        string simulinkType = it->second.second;
        auto basicType = ILParser::basicDataTypeMap.find(simulinkType);
        if (basicType != ILParser::basicDataTypeMap.end())
            datatypeLength = ILParser::basicDataTypeWidthMap.find(basicType->second)->second;
        else
            datatypeLength = ILParser::basicDataTypeWidthMap.find(simulinkType)->second;

        retCode += to_string(datatypeLength);

        retCode += ");\n";
    }

    /*for (auto it = ILTranslatorForSimulator::needDiagnose.begin(); it != ILTranslatorForSimulator::needDiagnose.end(); it++)
    {
        retCode += "SimulatorDiagnose::diagnoseRegisterCalculator(\"" + it->first;

        retCode += "\", \"" + it->second + "\");\n";
    }*/

    if(ILTranslatorForSimulator::configTest)
    {
        retCode += "clock_t testStartTime = clock();\n";
        retCode += "int testCount = (argc > 1 ? atoi(argv[1]) : 1);\n";
        retCode += "for (int ti = 0; ti < testCount; ti++) {\n";
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
	retCode += commentStr + "for (int step = 0; step < SimulatorBasic::totalStep; step++) {\n";

    
    //生成主函数的参数变量
    retCode += translateMainExecParameter(mainExecFunction);
	//生成主函数，包括其参�?	
    retCode += translateMainExecFuncionCall(mainExecFunction);
    //生成主函数输出值的打印代码
    //retCode += translateMainExecOutputPrint(mainExecFunction);

    if (ILTranslatorForSimulator::needMonitor.begin() != ILTranslatorForSimulator::needMonitor.end())
        retCode += "SimulatorMonitor::monitorNextStep();\n";
	retCode += "}\n";
	
    //if (ILTranslatorForSimulator::needMonitor.begin() != ILTranslatorForSimulator::needMonitor.end())
    //    retCode += "SimulatorMonitor::monitorPrint();\n";

    retCode += "clock_t detect_time = clock();\n";
    retCode += "double time_used = ((double)(detect_time - Start_Time)) / CLOCKS_PER_SEC;\n";
    retCode += "printf(\"Simulation Time : % .3f seconds\\n\", time_used);\n\n";

    retCode += "SimulatorTestCase::releaseTestCase();\n";
    
    retCode += ILTranslatorForSimulator::configTest ? "}\n" : "";

    if(ILTranslatorForSimulator::configTest)
    {
        retCode += "clock_t testEndTime = clock();\n";
        retCode += "long long testCostTime = (testEndTime - testStartTime);\n";
        retCode += "#undef printf(format, ...)\n";
        retCode += "printf(\"%ld\\n\", testCostTime);\n";
        retCode += "#define printf(format, ...)\n";
        retCode += "#define printf(format,)\n";
    }

    retCode += "return 1;\n";

	retCode += "}\n";

    code = retCode;
    return 1;
}
