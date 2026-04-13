#include <iostream>
#include <string>
#include "SimulatorBasic.h"
#include "SimulatorTestCase.h"
#include "ArgumentParser.h"

using namespace std;

int SimulatorBasic::totalStep = 0;

int SimulatorBasic::initArgument(int argc, char** argv)
{
    ArgumentParser argumentParser;
    argumentParser.parseArguments(argc, argv);
    if(!argumentParser.hasFlagValue("-b"))
    {
        cout << "Import test case file with \"-b\" flag!" << endl;
        return 0;
    }
    if (!argumentParser.hasFlagValue("-s"))
    {
        cout << "Set simulation step with \"-s\" flag!" << endl;
        return 0;
    }

    argumentParser.setDefaultValue("-b", SimulatorTestCase::binaryTestCaseFilePath);
    SimulatorTestCase::binaryTestCaseFilePath = argumentParser.getFlagValue("-b");
    if (SimulatorTestCase::binaryTestCaseFilePath == "")
    {
        cout << "Test case file should be palced after \"-b\" flag!" << endl;
        return 0;
    }

    argumentParser.setDefaultValue("-s", to_string(totalStep));
    string stepInString = argumentParser.getFlagValue("-s");
    if (stepInString == "" || stepInString == "0")
    {
        cout << "Setp number should be greater than 0" << endl;
        return 0;
    }
    totalStep = stoi(stepInString);
    
    cout << "Total step: " << totalStep << endl;        
    cout << "Test case: " << SimulatorTestCase::binaryTestCaseFilePath << endl;
    return 1;

}
