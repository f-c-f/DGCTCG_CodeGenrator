#include <iostream>

#include "ILGenerator.h"
#include "../MIRBasic/MIRParser.h"
#include "../ILGenerator/MIRSchedule.h"
#include "../ILGenerator/MIRScheduleAnalyzer.h"

#include "../ILBasic/ILParser.h"
#include "../ILBasic/ILSaver.h"

#include "../Common/ArgumentParser.h"
using namespace std;

int main(int argc, char** argv)
{
	//string inputFilePath =	"..\\..\\test\\MIRTest\\MulAddSplit_MIR.xml";
	//string outputFilePath =		"..\\..\\test\\ILGenerated\\MulAddSplit_MIR_IL.xml";
    
	string inputFilePath =	"..\\..\\test\\subsystem_MIR.xml";
	string outputFilePath =		"..\\..\\test\\subsystem_MIR_IL.xml";
    string topologyBasedOrder = "true";

	ArgumentParser argumentParser;
	string helpStr;
	helpStr += "-help    help information\n";
	helpStr += "-h       help information\n";
	helpStr += "-i       input file path of MIR file\n";
	helpStr += "-o       output file path of generated IL code\n";
	helpStr += "-t       generate actors by topology order";
	argumentParser.setHelpStr(helpStr);
	argumentParser.setDefaultValue("-i", inputFilePath);
	argumentParser.setDefaultValue("-o", outputFilePath);
	argumentParser.setDefaultValue("-t", topologyBasedOrder);
	if(argumentParser.parseArguments(argc, argv) == 0)
		return 0;

	inputFilePath =		    argumentParser.getFlagValue("-i");
	outputFilePath =	    argumentParser.getFlagValue("-o");
	topologyBasedOrder =	argumentParser.getFlagValue("-t");

	cout << "-i: " << inputFilePath << endl;
	cout << "-o: " << outputFilePath << endl;
	cout << "-t: " << topologyBasedOrder << endl;

    MIRModel model;
	MIRParser parser;
	int res = parser.parseMIR(inputFilePath, &model);
	if(res < 0)
	{
		cout << "Error: " << res << endl;
		cout << parser.getErrorStr() << endl;
		return res;
	}

    ILGenerator::setConfigTopologyBasedOrder(topologyBasedOrder == "true");

    MIRSchedule schedule;
    MIRScheduleAnalyzer scheduleAnalyzer;
    res = scheduleAnalyzer.scheduleAnalyze(&model, &schedule);
    if(res < 0)
	{
		cout << "Error: " << res << endl;
		cout << scheduleAnalyzer.getErrorStr() << endl;
		return res;
	}


	//MIRSaver mirSaver;
	//string retMIR;
	//mirSaver.save(&model, "mir.txt");

    ILParser iLParser;
    ILGenerator iLGenerator;
    
    res = iLGenerator.generateIL(&schedule, &model, &iLParser);
    if(res < 0)
	{
		cout << "Error: " << res << endl;
		return res;
	}

    tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* root = doc.NewElement("Root");
    doc.InsertEndChild(root);
    
    ILSaver iLSaver;
    string retXML;
    iLSaver.saveToString(&iLParser, retXML);
    
	//printf("%s\n", retXML.c_str());

    iLSaver.save(&iLParser, outputFilePath);

    iLParser.release();

	std::cout << "Done!\n";
	return 1;
}
