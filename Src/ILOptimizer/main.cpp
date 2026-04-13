#include <iostream>
#include "../ILBasic/ILParser.h"
#include "../MIRBasic/MIRParser.h"
#include "../ILBasic/ILFormatter.h"
#include "../ILBasic/ILExternFile.h"
#include "../ILBasic/ILSaver.h"
#include "ILOptimizer.h"

#include "../Common/ArgumentParser.h"
#include "../Common/Utility.h"
using namespace std;


int updateConfiguration(int argc, char** argv, string* inputILFilePath, string* inputMIRFilePath, string* outputFilePath, string* needFormatILFile)
{
    string targetDevice = "Intel"; //"Intel"   "ARMv8"    "Tricore"     "RISC-V"
    string variableFolding = "true";
    string expressionSimplification = "true";
    string codeBlockReduction = "true";
	string simd = "false";
	string dataTruncation = "false";
	string loopReshaping = "false";
    *needFormatILFile = "false";
    string arrayFolding = "false";

	ArgumentParser argumentParser;
	string helpStr;
	helpStr += "-help    help information\n";
	helpStr += "-h       help information\n";
	helpStr += "-i       input file path of IL code file\n";
	helpStr += "-m       input file path of MIR file\n"; // optional
	helpStr += "-o       output file path of optimized IL code\n";
	helpStr += "-t       target device (Intel, ARMv8, Tricore, RISC-V)\n";
	helpStr += "-v       variable folding (default is true)\n";
	helpStr += "-e       expression simplification (default is true)\n";
	helpStr += "-c       code block reduction (default is true)\n";
	helpStr += "-simd    use SIMD instruction (default is false)\n";
	helpStr += "-dt      data truncation (default is false)\n";
	helpStr += "-lp      loop reshaping (default is false)\n";
	helpStr += "-f       need format IL code file (default is false)\n";
    helpStr += "-af      array folding (default is false)\n";

	argumentParser.setHelpStr(helpStr);

	if(argumentParser.parseArguments(argc, argv) == 0)
		return 0;

	*inputILFilePath =		    argumentParser.getFlagValue("-i", *inputILFilePath);
	*inputMIRFilePath =         argumentParser.getFlagValue("-m", *inputMIRFilePath);
	*outputFilePath =	        argumentParser.getFlagValue("-o", *outputFilePath);
	targetDevice =	            argumentParser.getFlagValue("-t", targetDevice);
	variableFolding =	        argumentParser.getFlagValue("-v", variableFolding);
	expressionSimplification =	argumentParser.getFlagValue("-e", expressionSimplification);
	codeBlockReduction =	    argumentParser.getFlagValue("-c", codeBlockReduction);
	simd =                      argumentParser.getFlagValue("-simd", simd);
	dataTruncation =            argumentParser.getFlagValue("-dt", dataTruncation);
	loopReshaping =             argumentParser.getFlagValue("-lp", loopReshaping);
	*needFormatILFile =	        argumentParser.getFlagValue("-f", *needFormatILFile);
    arrayFolding =              argumentParser.getFlagValue("-af", arrayFolding);


	cout << argumentParser.flagToString() << endl;
    
    ILOptimizer::setConfigTargetDevice(targetDevice);
    ILOptimizer::setConfigVariableFolding(variableFolding == "true");
    ILOptimizer::setConfigExpressionSimplification(expressionSimplification == "true");
    ILOptimizer::setConfigCodeBlockReduction(codeBlockReduction == "true");
	ILOptimizer::setConfigSIMD(simd == "true");
	ILOptimizer::setConfigDataTruncation(dataTruncation == "true");
	ILOptimizer::setConfigLoopReshaping(loopReshaping == "true");
    ILOptimizer::setConfigArrayFolding(arrayFolding == "true");

    return 1;
}

int main(int argc, char** argv)
{
    
	//comment = true;
	//string inputFilePath = "D:/LAB/MBD/设计思路/代码生成/中间代码格式样例.xml";
	//string inputFilePath = "D:/LAB/MBD/CodeGenerator/ILTranslatorForC/test/Output/Branch_CompositeActor_IR.xml";
	//string inputFilePath = "../../test/IRFormatted/Branch_CompositeActor_IR_BranchOpTest.xml";
	//string inputFilePath = "C:\\Users\\sadamu\\Desktop\\对接\\对接2021年10月27日\\测试模型新\\filter_high_pass_IR_Formatted.xml";
	//string inputFilePath = "../../test/ILGenerated/filter_fir_IL.xml";
	string inputILFilePath = "C:\\Users\\sadamu\\Desktop\\对接\\对接2022年3月21日\\UnreachableTest_IL.xml";
	string inputMIRFilePath = "";
    //comment = true;
    //string outputFilePath = "D:/LAB/MBD/设计思路/代码生成/中间代码格式样例Optimized.xml";
	//string outputFilePath = "D:/LAB/MBD/CodeGenerator/ILTranslatorForC/test/Output/Branch_CompositeActor_IR_Optimized.xml";
	//string outputFilePath = "../../test/IROptimized/Branch_CompositeActor_IR_BranchOpTest_Optimized.xml";
	//string outputFilePath = "C:\\Users\\sadamu\\Desktop\\对接\\对接2021年10月27日\\测试模型新\\filter_high_pass_IR_Formatted_Optimized.xml";
	//string outputFilePath = "../../test/IROptimized/filter_fir_IL_Optimized.xml";
	string outputFilePath = "C:\\Users\\sadamu\\Desktop\\对接\\对接2022年3月21日\\UnreachableTest_IL_Optimized.xml";

    string needFormatILFile;
    int res = updateConfiguration(argc, argv, &inputILFilePath, &inputMIRFilePath, &outputFilePath, &needFormatILFile);
    if(res == 0)
        return 0;

    //comment = true
    //if(needFormatILFile == "true")
    //{
    //    string inputFileName = getFileNameWithoutSuffixByPath(inputFilePath);
    //    string exeFilePath = getExeDirPath();
    //    string formatFilePath = exeFilePath + "/temp/" + inputFileName + "_Formatted.xml";
    //    ILFormatter formatter;
    //    res = formatter.format(inputFilePath, formatFilePath);
    //    if(res < 0)
	//    {
	//	    cout << "Error: " << res << endl;
	//	    cout << formatter.getErrorStr() << endl;
	//	    return res;
	//    }
    //    inputFilePath = formatFilePath;
    //}

	ILParser parser;
	res = parser.parseIL(inputILFilePath);
	if(res < 0)
	{
		cout << "Error: " << res << endl;
		cout << parser.getErrorStr() << endl;
		return res;
	}

	MIRParser mirParser;
	MIRModel mirModel;
	if (!inputMIRFilePath.empty())
		res = mirParser.parseMIR(inputMIRFilePath, &mirModel);

	ILOptimizer optimizer;
	if (inputMIRFilePath.empty())
		res = optimizer.optimize(&parser, nullptr);
	else
		res = optimizer.optimize(&parser, &mirModel);
	if(res < 0)
	{
		cout << "Error: " << res << endl;
		cout << optimizer.getErrorStr() << endl;
		return res;
	}
    
	ILSaver saver;
	res = saver.save(&parser, outputFilePath);
	if(res < 0)
	{
		cout << "Error: " << res << endl;
		return res;
	}
    
    for(int i = 0; i < parser.externFiles.size(); i++)
    {
        string exePath = getExeDirPath();
        string externFilePathSrc = exePath + "/temp/" + parser.externFiles[i]->name;
        
        string outputILFileDir = getParentPath(outputFilePath);
        string outputILFileName = getFileNameWithoutSuffixByPath(outputFilePath);
        
        if(isFileExist(externFilePathSrc)) {
            string externFilePathDstDir = outputILFileDir + "/" + outputILFileName + "_ExternFile/";
            if(!isFolderExist(externFilePathDstDir))
                createFolder(externFilePathDstDir);
            string externFilePathDst = externFilePathDstDir + parser.externFiles[i]->name;

            copyFileOne(externFilePathSrc, externFilePathDst);
        }
    }

	std::cout << "Done!\n";

	return 1;
}

