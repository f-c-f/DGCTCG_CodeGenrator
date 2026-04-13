#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <vector>


#include "ParserForTBLink.h"
#include "TBConverter.h"
#include "../MIRBasic/MIRModel.h"
#include "../MIRBasic/MIRSaver.h"


using namespace std;

#include "../Common/ArgumentParser.h"

int main(int argc, char *argv[])
{
	string inputFilePath = "../../Src/ParserForTBLink/IfElse3.xml";
	string outputFilePath = "C:/Users/sadamu/Desktop/TBLinkIfElse3_MIR.xml";

	ArgumentParser argumentParser;
	string helpStr;
	helpStr += "-help    help information\n";
	helpStr += "-h       help information\n";
	helpStr += "-i       input file path of TBLink model\n";
	helpStr += "-o       output file path of MIR";
	argumentParser.setHelpStr(helpStr);
	argumentParser.setDefaultValue("-i", inputFilePath);
	argumentParser.setDefaultValue("-o", outputFilePath);
	if(argumentParser.parseArguments(argc, argv) == 0)
		return 0;

	inputFilePath = argumentParser.getFlagValue("-i");
	outputFilePath = argumentParser.getFlagValue("-o");

	cout << "-i: " << inputFilePath << endl;
	cout << "-o: " << outputFilePath << endl;

	int res;
	ParserForTBLink tbLinkParser;
	res = tbLinkParser.parseTBLink(inputFilePath);
	if(res < 0)
	{
		tbLinkParser.release();
		cout << "Error:" + to_string(res) << endl;
		return res;
	}

	cout << "Successfully parsed TBLink model!" << endl;
	cout << "Root entity: " << (tbLinkParser.rootEntity ? "Found" : "Not found") << endl;
	
	if (tbLinkParser.rootEntity) {
		cout << "Number of program model pages: " << tbLinkParser.rootEntity->programModelPage.size() << endl;
		
		for (size_t i = 0; i < tbLinkParser.rootEntity->programModelPage.size(); i++) {
			TBProgramModelPage* page = tbLinkParser.rootEntity->programModelPage[i];
			cout << "Page " << i << ": " << page->name << endl;
			cout << "  Number of actors: " << page->actors.size() << endl;
			cout << "  Number of relations: " << page->relations.size() << endl;
			cout << "  Number of link nodes: " << page->linkNodes.size() << endl;
		}
	}

	// 转换为MIR模型
	cout << "\nConverting to MIR model..." << endl;
	MIRModel mIRModel;
	TBConverter tbConverter;
	res = tbConverter.convert(&tbLinkParser, &mIRModel);
	if(res < 0)
	{
		cout << "Error converting to MIR: " + to_string(res) << endl;
		tbLinkParser.release();
		return res;
	}

	cout << "Successfully converted to MIR model!" << endl;
	cout << "MIR Model main function: " << mIRModel.mainFunction << endl;
	cout << "Number of functions: " << mIRModel.functions.size() << endl;

	MIRSaver mIRSaver;
	res = mIRSaver.save(&mIRModel, outputFilePath);
	if(res < 0)
	{
		cout << "Error:" + to_string(res);
		return res;
	}

    tbLinkParser.release();
    mIRModel.release();
	
	std::cout << "Done!\n";
	return 0;
}
