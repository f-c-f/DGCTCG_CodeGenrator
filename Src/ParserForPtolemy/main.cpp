#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <string.h>

#include "ParserForPtolemy.h"
#include "PTConverter.h"
#include "../MIRBasic/MIRSaver.h"

using namespace std;


#include "../Common/ArgumentParser.h"

int main(int argc, char *argv[])
{
	string inputFilePath = "../../test/ModelPtolemy/Branch.xml";
	string outputFilePath = "../../test/MIRTest/PtBranchComplex_MIR.xml";


	ArgumentParser argumentParser;
	string helpStr;
	helpStr += "-help    help information\n";
	helpStr += "-h       help information\n";
	helpStr += "-i       input file path of Ptolemy model\n";
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
	ParserForPtolemy ptParser;
	res = ptParser.parsePT(inputFilePath);
	if(res < 0)
	{
		ptParser.release();
		cout << "Error:" + to_string(res);
		return res;
	}

	MIRModel mIRModel;
	PTConverter ptConverter;
	res = ptConverter.convert(&ptParser, &mIRModel);
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

    ptParser.release();
    mIRModel.release();
	
	std::cout << "Done!\n";
	return 0;
}
