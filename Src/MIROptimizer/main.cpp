#include <iostream>
#include "../MIRBasic/MIRParser.h"
#include "../MIRBasic/MIRSaver.h"
#include "MIROptimizer.h"

#include "../Common/ArgumentParser.h"
#include "../Common/Utility.h"
using namespace std;

int main(int argc, char** argv)
{
	//comment = true;
	//string inputFilePath = "D:/LAB/MBD/设计思路/代码生成/中间代码格式样例.xml";
	//string inputFilePath = "D:/LAB/MBD/CodeGenerator/ILTranslatorForC/test/Output/Branch_CompositeActor_IR.xml";
	//string inputFilePath = "../../test/IRFormatted/Branch_CompositeActor_IR_BranchOpTest.xml";
	//string inputFilePath = "C:\\Users\\sadamu\\Desktop\\对接\\对接2021年10月27日\\测试模型新\\filter_high_pass_IR_Formatted.xml";
	//string inputFilePath = "../../test/ILGenerated/filter_fir_IL.xml";
	string inputFilePath = "../../test/MIRTest/MulAddSplit_MIR.xml";

    //comment = true;
    //string outputFilePath = "D:/LAB/MBD/设计思路/代码生成/中间代码格式样例Optimized.xml";
	//string outputFilePath = "D:/LAB/MBD/CodeGenerator/ILTranslatorForC/test/Output/Branch_CompositeActor_IR_Optimized.xml";
	//string outputFilePath = "../../test/IROptimized/Branch_CompositeActor_IR_BranchOpTest_Optimized.xml";
	//string outputFilePath = "C:\\Users\\sadamu\\Desktop\\对接\\对接2021年10月27日\\测试模型新\\filter_high_pass_IR_Formatted_Optimized.xml";
	//string outputFilePath = "../../test/IROptimized/filter_fir_IL_Optimized.xml";
	string outputFilePath = "../../test/MIROptimized/MulAddSplit_MIR_Optimized.xml";
    
	ArgumentParser argumentParser;
	string helpStr;
	helpStr += "-help    help information\n";
	helpStr += "-h       help information\n";
	helpStr += "-i       input file path of MIR file\n";
	helpStr += "-o       output file path of optimized MIR file\n";
	argumentParser.setHelpStr(helpStr);
	argumentParser.setDefaultValue("-i", inputFilePath);
	argumentParser.setDefaultValue("-o", outputFilePath);

	if(argumentParser.parseArguments(argc, argv) == 0)
		return 0;

	inputFilePath =		argumentParser.getFlagValue("-i");
	outputFilePath =	argumentParser.getFlagValue("-o");

	cout << "-i: " << inputFilePath << endl;
	cout << "-o: " << outputFilePath << endl;

	MIRParser parser;
    MIRModel model;
	int res = parser.parseMIR(inputFilePath, &model);
	if(res < 0)
	{
		cout << "Error: " << res << endl;
		cout << parser.getErrorStr() << endl;
		return res;
	}
    
	MIROptimizer optimizer;
	res = optimizer.optimize(&model);
	if(res < 0)
	{
		cout << "Error: " << res << endl;
		cout << optimizer.getErrorStr() << endl;
		return res;
	}
    
	MIRSaver saver;
	res = saver.save(&model, outputFilePath);
	if(res < 0)
	{
		cout << "Error: " << res << endl;
		return res;
	}
    
	std::cout << "Done!\n";

	return 1;
}
