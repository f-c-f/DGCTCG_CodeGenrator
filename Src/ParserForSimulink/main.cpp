#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "ParserForSimulink.h"
#include "SLConverter.h"
#include "../MIRBasic/MIRSaver.h"

using namespace std;


#include "../Common/ArgumentParser.h"

int main(int argc, char *argv[])
{
	string inputFilePath = "D:\\LAB\\论文\\喻泽弘-处理器流水线代码生成论文\\实验\\model\\bandstop_filter_float_2019a.slx";
	string outputFilePath = "D:\\LAB\\论文\\喻泽弘-处理器流水线代码生成论文\\实验\\model\\bandstop_filter_float_2019a_MIR.xml";


	ArgumentParser argumentParser;
	string helpStr;
	helpStr += "-help    help information\n";
	helpStr += "-h       help information\n";
	helpStr += "-i       input file path of Simulink model\n";
	helpStr += "-o       output file path of MIR";
	argumentParser.setHelpStr(helpStr);
	argumentParser.setDefaultValue("-i", inputFilePath);
	argumentParser.setDefaultValue("-o", outputFilePath);
	if(argumentParser.parseArguments(argc, argv) == 0)
		return 0;

	inputFilePath =		argumentParser.getFlagValue("-i");
	outputFilePath =	argumentParser.getFlagValue("-o");

	cout << "-i: " << inputFilePath << endl;
	cout << "-o: " << outputFilePath << endl;

	int res;
	ParserForSimulink slParser;
	res = slParser.parseSL(inputFilePath);
	if(res < 0)
	{
		slParser.release();
		cout << "Error:" + to_string(res);
		return res;
	}

	MIRModel mIRModel;
	SLConverter slConverter;
	res = slConverter.convert(&slParser, &mIRModel);
	if(res < 0)
	{
		cout << "Error:" + to_string(res);
		return res;
	}

	MIRSaver mIRSaver;
	res = mIRSaver.save(&mIRModel, outputFilePath);
	if(res < 0)
	{
		cout << "Error:" + to_string(res);
		return res;
	}

	
	std::cout << "Done!\n";
	return 0;
}
