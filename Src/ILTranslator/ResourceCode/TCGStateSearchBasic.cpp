/* Include Windows headers first to avoid std::byte (C++17) conflict with Windows byte typedef */
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOCRYPT
#include <windows.h>
#endif

#include "TCGStateSearchBasic.h"
#include "TCGStateSearchBasicBranchRegister.h"
#include "TCGStateSearchBasicDistanceRegister.h"
#include "TCGStateSearchBasicBranchRelation.h"

#include "TCGStateSearchBasicInputRegister.h"
#include "TCGStateSearchBasicParameterRegister.h"
#include "TCGStateSearchBasicGlobalVariableRegister.h"

#include "tinyxml2.h"
#include "Utility.h"
#include "BitmapCov.h"

#include <time.h>
#include <string.h>
#include <algorithm>
#include <queue>
#include <random>
#include <unordered_map>
#include <fstream>
#include <sstream>

#include "ArgumentParser.h"
#include <string.h>

#include <assert.h>

using namespace std;

string StateSearch::exePath;

clock_t StateSearch::testStartTime = 0;

int StateSearch::solveTimes = 0;
StateSearch::StateTraceTree StateSearch::stateTraceTree;
int StateSearch::achievedCount = 0;
int StateSearch::searchedCount = 0;
map<int, string> StateSearch::stateBranchList;
vector<int> StateSearch::branchSolveOrder;
vector<pair<int, string>> StateSearch::branchReadGlobalVar;
vector<pair<int, string>> StateSearch::branchWriteGlobalVar;
vector<StateSearch::GlobalVariableData> StateSearch::globalVariableList;
int StateSearch::globalVariableDataLengthTotal = 0;
vector<StateSearch::ModelInputVariableData> StateSearch::modelInputList;
int StateSearch::inputsDataLengthTotal = 0;
vector<StateSearch::ModelParameterVariableData> StateSearch::modelParameterList;
int StateSearch::parametersDataLengthTotal = 0;
int StateSearch::unwindCount = 0;
string StateSearch::cbmcParameter;
long long StateSearch::startTime;

vector<pair<StateSearch::StateTraceNode*, int>> StateSearch::priorityQueueToSolve; // First: ?????     Second: ???Id
vector<int> StateSearch::priorityQueueToSolveForRoot; // First: ?????     Second: ???Id

string StateSearch::paraBinaryTestCaseFilePath;
string StateSearch::paraCSVTestCaseFilePath;
int StateSearch::paraMaxRunTime;
int StateSearch::paraMaxRunIterator;
bool StateSearch::paraGenerateTestCaseOnce = false;
bool StateSearch::paraNoRandom = false;
bool StateSearch::paraHeuristics = true;
bool StateSearch::paraCanBePaused = false;
string StateSearch::paraOutputDir = "";
string StateSearch::paraOutputBinaryDir = "";


bool StateSearch::paraSimpleLog = true;

int StateSearch::paraSolveMultiple = 1;

int StateSearch::paraOutputSomeLongSequence = 0;
string StateSearch::paraOutputLongDir = "";
string StateSearch::paraOutputLongBinaryDir = "";
int StateSearch::paraOutputLongSequenceMaxLen = 0;

int StateSearch::paraDistanceGuided = false;

int StateSearch::finish = 0;

uint8_t* StateSearch::hashBuffer = nullptr;
uint8_t* StateSearch::hashBufferCutArray = nullptr;

map<uint64_t, bool> StateSearch::allDataHashMap;
map<uint64_t, bool> StateSearch::allDataHashCutArrayMap;

map<int64_t, vector<int>> StateSearch::stateCanNotSolveMap;

map<string, int> StateSearch::typeSizeMap = {make_pair("u8", 1), 
                                             make_pair("i8", 1), 
                                             make_pair("u16", 2), 
                                             make_pair("i16", 2), 
                                             make_pair("u32", 4), 
                                             make_pair("i32", 4), 
                                             make_pair("u64", 8), 
                                             make_pair("i64", 8), 
                                             make_pair("f32", 4), 
                                             make_pair("f64", 8)};

map<int, StateSearch::BranchDistanceData> StateSearch::branchDistanceDataMap; // key: distanceId  value: BranchDistanceData

map<int, StateSearch::ArrayDistanceData> StateSearch::arrayDistanceDataMap; // key: distanceId  value: ArrayDistanceData

//map<int, int> StateSearch::branchDistanceIdToBranchId;

//map<int, double> StateSearch::branchDistance;


//map<string, double> StateSearch::distanceRecordValueOriMap;
//map<string, double> StateSearch::distanceRecordValueNewMap;

string StateSearch::InputVariableRecord::toString()
{
    string retCode = "(" + this->pointer->type + ")";
    retCode += getVariableValueStr(this->pointer->type, this->data, 0);
    return retCode;
}

string StateSearch::ParameterVariableRecord::toString()
{
    string retCode = "(" + this->pointer->type + ")";
    retCode += getVariableValueStr(this->pointer->type, this->data, 0);
    return retCode;
}

void StateSearch::initStateSearch(int argc, char *argv[])
{
    StateSearch::exePath = getExeDirPath();

    ArgumentParser argumentParser;
	paraBinaryTestCaseFilePath = "";
	paraCSVTestCaseFilePath = "";
	paraMaxRunTime = 432000000; //???????5?????
	paraMaxRunIterator = 999999999;

    string paraGenerateTestCaseOnce = "false";
    string paraNoRandom = "false";
    string paraHeuristics = "true";
    string paraCanBePaused = "false";
    string paraOutputDir = exePath + "/TestCaseOutput";
    string paraOutputBinaryDir = exePath + "/TestCaseOutput/Binary";
    string paraSimpleLog = "true";
    int paraOutputSomeLongSequence = 0; // N???????????????????????????????500???????????N??????????
    string paraOutputLongDir = exePath + "/LongTestCases";
    string paraOutputLongBinaryDir = exePath + "/LongTestCases/Binary";
    int paraOutputLongSequenceMaxLen = 4096;
    string paraDistanceGuided = "false";
    
    paraSolveMultiple = 1;

    string helpStr;
    helpStr += "Usage: " + string(argv[0]) + " [options]\n";
    helpStr += "Options:\n";
    helpStr += "  -tb <path>                 Binary TestCase Folder Path\n";
    helpStr += "  -tc <path>                 CSV TestCase Folder Path\n";
    helpStr += "  -t <time>                  Max Run Time (default: 432000000, 5 days)\n";
    helpStr += "  -i <iterator>              Max Run Iterator (default: 999999999)\n";
    helpStr += "  -once <true/false>         Generate TestCase Once (default: false)\n";
    helpStr += "  -noRand <true/false>       No Random (default: false)\n";
    helpStr += "  -heuristics <true/false>   Heuristics (default: true)\n";
    helpStr += "  -canBePaused <true/false>  Can Be Paused (default: false)\n";
    helpStr += "  -o <path>                  Output Dir\n";
    helpStr += "  -ob <path>                 Output Binary Dir\n";
    helpStr += "  -sl <true/false>           Simple Log (default: true)\n";
    helpStr += "  -sm <number>               Solve Multiple (default: 1)\n";
    helpStr += "  -outputLong <number>       Output Some Long Sequence, cond: noRand==false (default: 0)\n"; // ????fuzz parameter???????input sequence
    helpStr += "  -ol <path>                 Output Long TestCase Dir\n";
    helpStr += "  -olb <path>                Output Long TestCase Binary Dir\n";
    helpStr += "  -outputLongMaxLen <number> Output Long TestCase Max Length (default: 4096)\n";
    helpStr += "  -distanceGuided <true/false> Distance Guided (default: false)\n";



    argumentParser.setHelpStr(helpStr);
    argumentParser.parseArguments(argc, argv);

	paraBinaryTestCaseFilePath = argumentParser.getFlagValue("-tb", paraBinaryTestCaseFilePath);
	paraCSVTestCaseFilePath    = argumentParser.getFlagValue("-tc", paraCSVTestCaseFilePath);
	paraMaxRunTime             = stringToInt(argumentParser.getFlagValue("-t", to_string(paraMaxRunTime)));
	paraMaxRunIterator         = stringToInt(argumentParser.getFlagValue("-i", to_string(paraMaxRunIterator)));
    paraGenerateTestCaseOnce   = argumentParser.getFlagValue("-once", paraGenerateTestCaseOnce);
    paraNoRandom               = argumentParser.getFlagValue("-noRand", paraNoRandom);
    paraHeuristics             = argumentParser.getFlagValue("-heuristics", paraHeuristics);
    paraCanBePaused            = argumentParser.getFlagValue("-canBePaused", paraCanBePaused);
    paraOutputDir              = argumentParser.getFlagValue("-o", paraOutputDir);
    paraOutputBinaryDir        = argumentParser.getFlagValue("-ob", paraOutputBinaryDir);
    paraSimpleLog              = argumentParser.getFlagValue("-sl", paraSimpleLog);
    paraSolveMultiple          = stringToInt(argumentParser.getFlagValue("-sm", to_string(paraSolveMultiple)));
    paraOutputSomeLongSequence = stringToInt(argumentParser.getFlagValue("-outputLong", to_string(paraOutputSomeLongSequence)));
    paraOutputLongDir          = argumentParser.getFlagValue("-ol", paraOutputLongDir);
    paraOutputLongBinaryDir    = argumentParser.getFlagValue("-olb", paraOutputLongBinaryDir);
    paraOutputLongSequenceMaxLen = stringToInt(argumentParser.getFlagValue("-outputLongMaxLen", to_string(paraOutputLongSequenceMaxLen)));
    paraDistanceGuided         = argumentParser.getFlagValue("-distanceGuided", paraDistanceGuided);

    
    stringstream ss;
    ss << "StateSearch Start:" << endl;
    ss << argumentParser.flagToString() << endl;
    
    string logStr = ss.str();
    std::cout << logStr;
    appendFile(exePath + "/log.txt", logStr);

    StateSearch::paraGenerateTestCaseOnce = (paraGenerateTestCaseOnce == "true");
    StateSearch::paraNoRandom = (paraNoRandom == "true");
    StateSearch::paraHeuristics = (paraHeuristics == "true");
    StateSearch::paraCanBePaused = (paraCanBePaused == "true");
    StateSearch::paraOutputDir = paraOutputDir;
    StateSearch::paraOutputBinaryDir = paraOutputBinaryDir;
    StateSearch::paraSimpleLog = (paraSimpleLog == "true");
    StateSearch::paraOutputSomeLongSequence = paraOutputSomeLongSequence;
    StateSearch::paraOutputLongDir = paraOutputLongDir;
    StateSearch::paraOutputLongBinaryDir = paraOutputLongBinaryDir;
    StateSearch::paraOutputLongSequenceMaxLen = paraOutputLongSequenceMaxLen;
    StateSearch::paraDistanceGuided = (paraDistanceGuided == "true");

    srand((unsigned)time(nullptr));
    startTime = clock();

    stateTraceTree.root = new StateTraceNode();
    stateTraceTree.root->inputVariablesStep = new InputVariableStep();
    stateTraceTree.root->globalVariablesStep = new GlobalVariableStep();
    stateTraceTree.root->exceptBranchId = -1;
    stateTraceTree.root->achieveExceptBranch = true;
    stateTraceTree.root->depth = 0;
    stateTraceTree.nodeCount = 1;
    stateTraceTree.currentNode = stateTraceTree.root;
    stateTraceTree.allNodeList.push_back(stateTraceTree.root);
    
    stateTraceTree.root->branchIdCovered = new unsigned char[BitmapCov::branchBitmapLength];
    stateTraceTree.root->branchIdCoveredTotal = new unsigned char[BitmapCov::branchBitmapLength];
    
    if(isFileExist("cbmcParameter.txt"))
    {
        cbmcParameter = readFile("cbmcParameter.txt");
    }

    
    if(!isFolderExist(exePath + "/Temp")) {
        createFolder(exePath + "/Temp");
    }
    if(!isFolderExist(exePath + "/TestCaseOutput")) {
        createFolder(exePath + "/TestCaseOutput");
    }
    if(!isFolderExist(exePath + "/TestCaseOutput/Binary")) {
        createFolder(exePath + "/TestCaseOutput/Binary");
    }
    if (!isFolderExist(exePath + "/LongTestCases")) {
        createFolder(exePath + "/LongTestCases");
    }
    if (!isFolderExist(exePath + "/LongTestCases/Binary")) {
        createFolder(exePath + "/LongTestCases/Binary");
    }

    StateSearchBasicBranchRegister::loadStateSearchBranchData();
    StateSearch::registStateBranch();
    vector<pair<int, string>> StateSearch_BranchReadGlobalVar;
    vector<pair<int, string>> StateSearch_BranchWriteGlobalVar;
    StateSearchBasicBranchRelation::loadStateSearchBranchRelation(StateSearch_BranchReadGlobalVar, StateSearch_BranchWriteGlobalVar);
    StateSearch::registStateBranchRelation(StateSearch_BranchReadGlobalVar, StateSearch_BranchWriteGlobalVar);

}

void StateSearch::initStateSearch2()
{

    // State Search Regist Global Variables code
    StateSearchBasicGlobalVariableRegister::registAllGlobalVariables();
    // State Search Regist Parameters code
    StateSearchBasicParameterRegister::registAllModelParameters();
    // State Search Regist Inputs code
    StateSearchBasicInputRegister::registAllModelInputs();
    /* Main Loop of model */
    BitmapCov::step();

    TCGStateSearchBasicDistanceRegister::registAllDistance();

    collectGlobalVariableStep(stateTraceTree.root);
    
    if(!paraBinaryTestCaseFilePath.empty() && isFolderExist(paraBinaryTestCaseFilePath)){
        StateSearch::runExistTestCaseBinary(paraBinaryTestCaseFilePath);
    }
    if(!paraCSVTestCaseFilePath.empty() && isFolderExist(paraCSVTestCaseFilePath)){
        StateSearch::runExistTestCaseCSV(paraCSVTestCaseFilePath);
    }


	// ?????????????????????????????????
    // for (int i = 0; StateSearch::paraHeuristics && i < BitmapCov::branchBitmapLength; i++)
    // {
    //     int branchId = StateSearch::branchSolveOrder[i];
    //     StateSearch::priorityQueueToSolveForRoot.push_back(branchId);
    // }

}

void StateSearch::terminateStateSearch()
{
    string exePath = getExeDirPath();
    

    if(hashBuffer)
        delete[] hashBuffer;
    if(hashBufferCutArray)
        delete[] hashBufferCutArray;

    if(stateTraceTree.nodeCount < 100)
    {
        string tempPlainPath = exePath + "/Temp/StateTraceTreeViz_svg_final.svg";
        StateSearch::printStateTraceTree(tempPlainPath);
    }
}

void StateSearch::runExistTestCaseCSV(string dirPath)
{
    vector<string> fileList;
    listFiles(dirPath, fileList);
    for(int i = 0; i < fileList.size(); i++)
    {
        string fileName = dirPath + "/" + fileList[i];

        if(!(stringEndsWith(fileName, ".csv") || stringEndsWith(fileName, "CSV")))
            continue;

        string csvText = readFile(fileName);
        if(csvText.empty())
            continue;

        vector<string> csvLines = stringSplit(csvText, "\n");
        if(csvLines.size() <= 1)
            continue;

        int inputLength = 0;
        for(int j = 0; j < StateSearch::modelInputList.size(); j++)
        {
            inputLength += StateSearch::modelInputList[j].length;
        }
        int size = inputLength * csvLines.size();
        char* data = (char*)malloc(size);
        int offset = 0;
        for(int j = 0; j < csvLines.size(); j++)
        {
            vector<string> csvLineData = stringSplit(csvLines[j], ",");
            for(int k = 1; k < csvLineData.size(); k++)
            {
                getVariableValueBinary(csvLineData[k], StateSearch::modelInputList[k - 1].type, data, offset);
                offset += StateSearch::modelInputList[k - 1].length;
            }
        }
        
        string logStr = "TestCaseFile: " + fileName + "\n";
        std::cout << logStr;
        
        //exePath = getExeDirPath();
        appendFile(exePath + "/log.txt", logStr);

        runExistTestCaseBinaryOne(data, size, i);
        
        free(data);
    }
}

void StateSearch::runExistTestCaseBinary(string dirPath)
{
    
    vector<string> fileList;
    if(!isFolderExist(dirPath))
    {
        string logStr = "No Exist TestCase Binary Folder!\n";
        std::cout << logStr;
        //exePath = getExeDirPath();
        appendFile(exePath + "/log.txt", logStr);
        return;
    }
    listFiles(dirPath, fileList);
    for(int i = 0; i < fileList.size(); i++)
    {
        string fileName = dirPath + "/" + fileList[i];
        int size;
        char* data = readFileBinary(fileName, size);

        int sizeBuf = size;
        if (size < parametersDataLengthTotal + inputsDataLengthTotal) {
            sizeBuf = parametersDataLengthTotal + inputsDataLengthTotal;
        }
        else if ((sizeBuf - parametersDataLengthTotal) % inputsDataLengthTotal != 0) {
            sizeBuf += (inputsDataLengthTotal - ((sizeBuf - parametersDataLengthTotal) % inputsDataLengthTotal));
        }
        char* dataBuf = new char[sizeBuf];
        memset((void*)dataBuf, 0, sizeBuf);
        memcpy((void*)dataBuf, data, size);

        string logStr = "TestCaseFile: " + fileName + "\n";
        std::cout << logStr;
        //exePath = getExeDirPath();
        appendFile(exePath + "/log.txt", logStr);

        runExistTestCaseBinaryOne(dataBuf, sizeBuf, i);
        
        delete[] data;
    }
}

void StateSearch::runExistTestCaseBinaryOne(char* data, int size, int testCaseIndex)
{
    stateTraceTree.currentNode = stateTraceTree.root;
    int stepTotol = (size - parametersDataLengthTotal) / inputsDataLengthTotal;

    InputVariableStep inputVariablesStepTemp;
    inputVariablesStepTemp.stepInputs.resize(stepTotol);

    int parameterOffset = 0;
    for (int i = 0; i < modelParameterList.size(); i++)
    {
        ParameterVariableRecord refRecord;
        refRecord.data = new char[modelParameterList[i].length];
        memcpy(refRecord.data, data + parameterOffset, modelParameterList[i].length);
        refRecord.pointer = &(modelParameterList[i]);
        inputVariablesStepTemp.parameters.push_back(refRecord);
        parameterOffset += modelParameterList[i].length;
    }

    for(int step = 0; step < stepTotol; step++)
    {
        // BitmapCov::clearCurrentBitmap();
        
        InputVariableCuple* inputCuple = &inputVariablesStepTemp.stepInputs[step];

        int offset = 0;
        for(int i = 0; i < StateSearch::modelInputList.size(); i++)
        {
            InputVariableRecord targetRecord;
            targetRecord.data = new char[StateSearch::modelInputList[i].length];
            memcpy(targetRecord.data, data + parametersDataLengthTotal + step * inputsDataLengthTotal + offset, StateSearch::modelInputList[i].length);
            offset += StateSearch::modelInputList[i].length;
            //targetRecord.length = refRecord->length;
            //targetRecord.type = refRecord->type;
            //targetRecord.name = refRecord->name;
            targetRecord.pointer = &(StateSearch::modelInputList[i]);
            inputCuple->allInputVariable.push_back(targetRecord);
        }
    }


    //addStateTraceNode(stateTraceTree.currentNode, -1, &inputVariablesStepTemp);

    //stateTraceTree.currentNode->inputVariablesStep = inputVariablesStepTemp;

    stateTraceTree.currentNode->runMode = StateTraceNode::RunMode::ExistTestCase;

    bool newBranchCovered = tryStateBranchInnerMultiStep(stateTraceTree.currentNode, -1, &inputVariablesStepTemp);
    stateTraceTree.currentNode->achieveExceptBranch = true;
    stateTraceTree.currentNode->isExistTestCaseNode = true;

    //BitmapCov::step();
    achievedCount = 0;
    for(int j = 0; j < BitmapCov::branchBitmapLength; j++) {
        if(BitmapCov::branchAllBitmap[j] != 0)
            achievedCount++;
    }
    
    string logStr = "TestCase " + stringFillBlank(to_string(testCaseIndex), 4, true) + ": ";
    logStr += "Achieved: " + to_string(BitmapCov::branchCov) + "/" + to_string(BitmapCov::branchBitmapLength);
    logStr += " run " + to_string(stepTotol) + " steps\n";
    logStr += "TotalCover:    ";
    for(int i = 0; i < BitmapCov::branchBitmapLength; i++)
    {
        logStr += to_string(BitmapCov::branchAllBitmap[i] - 0);
    }
    logStr += "\n";
    
    std::cout << logStr;
    //string exePath = getExeDirPath();
    appendFile(exePath + "/log.txt", logStr);
}


void StateSearch::registGlobalVariables(string name, string type, vector<int>& arraySize, char* address, int length)
{
    GlobalVariableData globalVariableData;
    globalVariableData.address = address;
    globalVariableData.length = length;
    globalVariableData.type = type;
    globalVariableData.name = name;
    globalVariableData.arraySize = arraySize;

    globalVariableList.push_back(globalVariableData);


    GlobalVariableRecord globalVariableRecord;
    globalVariableRecord.data = new char[length];
    ::memcpy(globalVariableRecord.data, address, length);
    //globalVariableRecord.length = length;
    //globalVariableRecord.type = type;
    //globalVariableRecord.name = name;
    //globalVariableRecord.arraySize = arraySize;
    globalVariableRecord.pointer = &(globalVariableList[globalVariableList.size() - 1]);

    stateTraceTree.root->globalVariablesStep->allGlobalVariable.push_back(globalVariableRecord);

    globalVariableDataLengthTotal += length;
}

void StateSearch::registModelInputs(string name, string type, int length)
{
    ModelInputVariableData inputRecord;
    inputRecord.name = name;
    inputRecord.type = type;
    inputRecord.length = length;
    inputsDataLengthTotal += length;
    modelInputList.push_back(inputRecord);
}

void StateSearch::registModelParameters(string name, string type, char* address, int length)
{
    ModelParameterVariableData parameterRecord;
    parameterRecord.name = name;
    parameterRecord.type = type;
    parameterRecord.address = address;
    parameterRecord.length = length;
    parametersDataLengthTotal += length;
    modelParameterList.push_back(parameterRecord);
}

void StateSearch::registStateBranch()
{
    // ??????????????CBMC???
    stateBranchList = StateSearchBasicBranchRegister::StateSearch_Branch;
    vector<pair<int, int>> vtMap;
    for (auto it = StateSearchBasicBranchRegister::StateSearch_Branch_Depth.begin(); it != StateSearchBasicBranchRegister::StateSearch_Branch_Depth.end(); it++)
        vtMap.push_back(make_pair(it->first, it->second));

    // ???????????????????????
    sort(vtMap.begin(), vtMap.end(), 
        [](const pair<int, int> &x, const pair<int, int> &y) -> int {
        return x.second < y.second;
    });

    for (auto it = vtMap.begin(); it != vtMap.end(); it++)
    {
        branchSolveOrder.push_back(it->first);
    }

    // ?????????????????????????
    auto it = StateSearchBasicBranchRegister::StateSearch_Branch_Parent.begin();
    for (; it != StateSearchBasicBranchRegister::StateSearch_Branch_Parent.end(); it++)
    {
        if (it->second != -1)
        {
            StateSearchBasicBranchRegister::StateSearch_Branch_Children[it->second].push_back(it->first);
        }
    }

}

void StateSearch::registStateBranchRelation(vector<pair<int, string>>& branchReadGlobalVar,
    vector<pair<int, string>>& branchWriteGlobalVar)
{
    StateSearch::branchReadGlobalVar = branchReadGlobalVar;
    StateSearch::branchWriteGlobalVar = branchWriteGlobalVar;
}

namespace 
{
    int lastTestTreeNodeId = 0;
}

void StateSearch::selectState(bool &timeOut)
{
    timeOut = false;
    string printStr;
    bool prioritySolved = false;
    while(!StateSearch::priorityQueueToSolveForRoot.empty() || !StateSearch::priorityQueueToSolve.empty())
    {
        clock_t testEndTime = clock();
        if(testEndTime - StateSearch::testStartTime > StateSearch::paraMaxRunTime)
        {
            timeOut = true;
            return;
        }
        int branchId = 0;
        StateTraceNode* node = nullptr;

        if(!StateSearch::priorityQueueToSolveForRoot.empty())
        {
            auto last = StateSearch::priorityQueueToSolveForRoot[StateSearch::priorityQueueToSolveForRoot.size() - 1];
            StateSearch::priorityQueueToSolveForRoot.pop_back();
            node = stateTraceTree.root;
            branchId = last;
        }
        else
        {
            auto last = StateSearch::priorityQueueToSolve[StateSearch::priorityQueueToSolve.size() - 1];
            StateSearch::priorityQueueToSolve.pop_back(); //erase(StateSearch::priorityQueueToSolve.begin() + StateSearch::priorityQueueToSolve.size() - 1);
            branchId = last.second;
            node = last.first;
        }
        
        if(BitmapCov::branchAllBitmap[branchId] || node->solvedBranchResultMap.find(branchId) != node->solvedBranchResultMap.end())
        {
            continue;
        }

        //if(!StateSearch::paraSimpleLog)
        {
            printStr = "Priority solving Branch " + to_string(branchId + 1) + " on Node " + to_string(node->id) + "\n";
            string logStr;
            logStr = printStr;
            std::cout << logStr;

            //string exePath = getExeDirPath();
            appendFile(exePath + "/log.txt", logStr + "\n");
        }

        solveInputVariable(node, branchId);
        if (node->solvedBranchResultMap[branchId])
        {
            //???????????????
            //if (stateTraceTree.currentNode != node)
            //    resetGlobalVariables(node);
            stateTraceTree.currentNode = node;
            stateTraceTree.selectedBranchId = branchId;
            
            prioritySolved = true;
            break;
        }
    }

    if (prioritySolved)
    {
        return;
    }
    printStr = "Select State ... ";
    bool existBranchCanSolved = false;

    vector<int> unreachedBranchId;
    for(int i = 0; i < BitmapCov::branchBitmapLength; i++) {
        int branchId = branchSolveOrder[i];
        if(BitmapCov::branchAllBitmap[branchId]) {
            continue;
        }
        unreachedBranchId.push_back(branchId);
    }
    //clock_t time = clock();
    //shuffle(unreachedBranchId.begin(), unreachedBranchId.end(), std::default_random_engine(time));
    printStr += "Remain: " + stringFillBlank(to_string(unreachedBranchId.size()),8) + " ";

    for(int i = 0; i < unreachedBranchId.size(); i++) {
        int branchId = unreachedBranchId[i];
        string printStr2;
        printStr2 = "Try: " + stringFillBlank(to_string(branchId + 1), 8) + " ";
        //?????????????????????
        bool canSolved = false;
        for(int j = lastTestTreeNodeId; j < stateTraceTree.allNodeList.size(); j++)
        {
            clock_t testEndTime = clock();
            if(testEndTime - StateSearch::testStartTime > StateSearch::paraMaxRunTime)
            {
                timeOut = true;
                return;
            }
            StateTraceNode* node = stateTraceTree.allNodeList[j];
            if(node->solvedBranchResultMap.find(branchId) == node->solvedBranchResultMap.end() &&
                node->achieveExceptBranch) //(node->achieveExceptBranch || (node->achieveNewStateHashCutArray && !node->isIntermediateNode))
            {
                string printStr3;
                printStr3 = "Node: " + stringFillBlank(to_string(j), 8, true) + "/" + stringFillBlank(to_string(stateTraceTree.allNodeList.size()),8);

                string logStr;
                logStr = printStr + printStr2 + printStr3;
                std::cout << logStr << endl;
                //string exePath = getExeDirPath();
                appendFile(exePath + "/log.txt", logStr + "\n");

                // ??????
                solveInputVariable(node, branchId);

                //for(int k = 0; k < printStr.length()+printStr2.length()+printStr3.length(); k++)
                //{
                //    std::cout << "\b";
                //}
                //std::cout << "\n";
                if(node->solvedBranchResultMap[branchId])
                {
                    //???????????????
                    //if(stateTraceTree.currentNode != node)
                    //    resetGlobalVariables(node);
                    stateTraceTree.currentNode = node;
                    stateTraceTree.selectedBranchId = branchId;

                    canSolved = true;
                    break;
                }
                else
                {
                    // ???????????????????????????
                    continue;
                }
            }
        }

        if(canSolved)
        {
            existBranchCanSolved = true;
            break;
        }
    }

    if(!existBranchCanSolved)
    {
        //???????????????????
        stateTraceTree.currentNode = nullptr;
        lastTestTreeNodeId = stateTraceTree.nodeCount;
        //int rand_ = rand() % stateTraceTree.allNodeList.size();
        //stateTraceTree.currentNode = stateTraceTree.allNodeList[rand_];
    }
}

void StateSearch::randomRun(int nTimes)
{
    //string exePath = getExeDirPath();
    string logStr;
    logStr = "Random Run " + to_string(nTimes) + " Steps.\n";
    std::cout << logStr;
    appendFile(exePath + "/log.txt", logStr);

    if (stateTraceTree.allNodeList.empty()) {
        stateTraceTree.currentNode = stateTraceTree.root;
        if (!stateTraceTree.currentNode) return;
    } else {
        int rand_ = rand() % stateTraceTree.allNodeList.size();
        stateTraceTree.currentNode = stateTraceTree.allNodeList[rand_];
    }
    resetGlobalVariables(stateTraceTree.currentNode);
    vector<int> unreachedBranchId;
    for(int i = 0; i < BitmapCov::branchBitmapLength; i++) {
        int branchId = branchSolveOrder[i];
        if(BitmapCov::branchAllBitmap[branchId]) {
            continue;
        }
        unreachedBranchId.push_back(branchId);
    }
    int lastBranchCov = BitmapCov::branchCov;

    InputVariableStep inputVariablesStepTemp;
    //if (StateSearch::modelParameterList.size() > 0)
    //{
    //    inputVariablesStepTemp.parameters.resize(StateSearch::modelParameterList.size());
    //    for (int i = 0; i < StateSearch::modelParameterList.size(); i++)
    //    {
    //        ParameterVariableRecord refRecord;
    //        refRecord.data = 
    //        refRecord.pointer = &(StateSearch::modelParameterList[i]);
    //        inputVariablesStepTemp.parameters[i] = refRecord;
    //    }
    //}


    //cout << "444444" << endl;


    int targetBranchDistanceId = -1;
    int targetArrayDistanceId = -1;
    if (StateSearch::paraDistanceGuided)
    {
        // ??1/3????????????????????????????
        int pCount = 1;

        // ??branchDistanceDataMap?????????????????????????????????????????????
        vector<int> targetDistance;
        for (auto branchDistanceData : branchDistanceDataMap)
        {
            BranchDistanceData* bdd = &branchDistanceData.second;

            if (!BitmapCov::branchAllBitmap[bdd->branchId] && 
                findIndexInVector(targetDistance, branchDistanceData.first) == -1 &&
                bdd->maxStepDistance > 0)
            {
                targetDistance.push_back(branchDistanceData.first);
            }
        }
        if (targetDistance.size() > 0)
        {
            pCount++;
            rand_ = rand() % targetDistance.size();
            targetBranchDistanceId = targetDistance[rand_];
        }

        if (!arrayDistanceDataMap.empty())
        {
            pCount++;
            rand_ = rand() % arrayDistanceDataMap.size();
            targetArrayDistanceId = rand_;
        }

        if (pCount == 3)
        {
            rand_ = rand() % pCount; // 0?????????1?????????????2?????????????
            if (rand_ == 0) {
                targetBranchDistanceId = -1;
                targetArrayDistanceId = -1;
            }
            else if (rand_ == 1) {
                targetArrayDistanceId = -1;
            }
            else {
                targetBranchDistanceId = -1;
            }
        }
        else if (pCount == 2 && targetBranchDistanceId != -1)
        {
            rand_ = rand() % pCount;  // 0?????????1?????????????
            if (rand_ == 0) {
                targetBranchDistanceId = -1;
            }
        }
        else if (pCount == 2 && targetArrayDistanceId != -1)
        {
            rand_ = rand() % pCount; // 0?????????1?????????????
            if (rand_ == 0) {
                targetArrayDistanceId = -1;
            }
        }
    }

    if (targetBranchDistanceId == -1 && targetArrayDistanceId == -1)
    {
        inputVariablesStepTemp.stepInputs.resize(nTimes);

        if (!stateTraceTree.allInputVariableStepList.empty())
        {
            for (int i = 0; i < nTimes; i++)
            {
                InputVariableCuple* inputCuple = nullptr;
                for (int retry = 0; retry < 32 && !inputCuple; retry++) {
                    rand_ = rand() % stateTraceTree.allInputVariableStepList.size();
                    InputVariableStep* tempInput = &stateTraceTree.allInputVariableStepList[rand_];
                    if (!tempInput->stepInputs.empty()) {
                        rand_ = rand() % tempInput->stepInputs.size();
                        inputCuple = &tempInput->stepInputs[rand_];
                    }
                }
                if (inputCuple) {
                    inputVariablesStepTemp.stepInputs[i].allInputVariable = inputCuple->allInputVariable;
                } else {
                    InputVariableCuple* randomCuple = generateRandomInputCuple();
                    if (randomCuple) {
                        inputVariablesStepTemp.stepInputs[i].allInputVariable = randomCuple->allInputVariable;
                        randomCuple->allInputVariable.clear();
                        randomCuple->release();
                        delete randomCuple;
                    }
                }
            }
        }
        else
        {
            // allInputVariableStepList为空则选择随机一个节点的随机一个stepInput
            for (int i = 0; i < nTimes; i++)
            {
                if (stateTraceTree.allNodeList.size() <= 1)
                {
                    // 仅有根节点，生成完全随机输入（避免 rand()%0 未定义行为）
                    for (size_t j = 0; j < StateSearch::modelInputList.size(); j++)
                    {
                        InputVariableRecord rec;
                        rec.pointer = &StateSearch::modelInputList[j];
                        rec.data = (char*)generateRandomDataByType(StateSearch::modelInputList[j].type, StateSearch::modelInputList[j].length);
                        inputVariablesStepTemp.stepInputs[i].allInputVariable.push_back(rec);
                    }
                }
                else
                {
                    rand_ = rand() % (stateTraceTree.allNodeList.size() - 1) + 1;
                    StateTraceNode* temp = stateTraceTree.allNodeList[rand_];
                    if (temp->inputVariablesStep && !temp->inputVariablesStep->stepInputs.empty())
                    {
                        rand_ = rand() % temp->inputVariablesStep->stepInputs.size();
                        InputVariableCuple* inputCuple = &temp->inputVariablesStep->stepInputs[rand_];
                        inputVariablesStepTemp.stepInputs[i].allInputVariable = inputCuple->allInputVariable;
                    }
                    else
                    {
                        for (size_t j = 0; j < StateSearch::modelInputList.size(); j++)
                        {
                            InputVariableRecord rec;
                            rec.pointer = &StateSearch::modelInputList[j];
                            rec.data = (char*)generateRandomDataByType(StateSearch::modelInputList[j].type, StateSearch::modelInputList[j].length);
                            inputVariablesStepTemp.stepInputs[i].allInputVariable.push_back(rec);
                        }
                    }
                }
            }
        }
        stateTraceTree.currentNode->runMode = StateTraceNode::RunMode::SolvedInputRandom;
    }
    else if (targetArrayDistanceId != -1)
    {
        logStr = "Array Distance Guided: distance " + to_string(targetArrayDistanceId) + ".\n";
        logStr += "VarName: " + arrayDistanceDataMap[targetArrayDistanceId].varName + ".\n";
        std::cout << logStr;
        appendFile(exePath + "/log.txt", logStr);

        
        inputVariablesStepTemp.stepInputs.resize(nTimes);
        for (int i = 0; i < nTimes; i++)
        {
            InputVariableCuple* inputCuple = generateOneDistanceDataInput(&arrayDistanceDataMap[targetArrayDistanceId].maxStepInput);
            inputVariablesStepTemp.stepInputs[i].allInputVariable = inputCuple->allInputVariable;
            delete inputCuple;
        }
        stateTraceTree.currentNode->runMode = StateTraceNode::RunMode::DistanceGuided;
    }
    else if (targetBranchDistanceId != -1)
    {
        logStr = "Branch Distance Guided: distance " + to_string(targetBranchDistanceId) + ".\n";
        logStr += "Max Step Distance: " + to_string(branchDistanceDataMap[targetBranchDistanceId].maxStepDistance) + ".\n";
        logStr += "VarName: " + branchDistanceDataMap[targetBranchDistanceId].varName + ".\n";
        std::cout << logStr;
        appendFile(exePath + "/log.txt", logStr);

        
        inputVariablesStepTemp.stepInputs.resize(nTimes);
        for (int i = 0; i < nTimes; i++)
        {
            InputVariableCuple* inputCuple = generateOneDistanceDataInput(&branchDistanceDataMap[targetBranchDistanceId].maxStepInput);
            inputVariablesStepTemp.stepInputs[i].allInputVariable = inputCuple->allInputVariable;
            delete inputCuple;
        }
        stateTraceTree.currentNode->runMode = StateTraceNode::RunMode::DistanceGuided;
    }


    tryStateBranchInnerMultiStep(stateTraceTree.currentNode, -1, &inputVariablesStepTemp);

    if (lastBranchCov < BitmapCov::branchCov) {
        stateTraceTree.currentNode->achieveExceptBranch = true;
    }
}

bool StateSearch::tryStateBranchInnerMultiStep(StateTraceNode* baseStateNode, int expectStateBranchId, InputVariableStep* inputVariableStep)
{
    // ??????????????????
    unsigned char* lastBranchCovered = new unsigned char[BitmapCov::branchBitmapLength];
    memcpy(lastBranchCovered, BitmapCov::branchAllBitmap, BitmapCov::branchBitmapLength);

    stateTraceTree.currentNode = baseStateNode;

    // ???????????????????????????????????????????????????????step
    if (!inputVariableStep->parameters.empty()) //baseStateNode != stateTraceTree.root && 
    {
        resetGlobalVariables(stateTraceTree.root);
        for (int i = 0; i < inputVariableStep->parameters.size(); i++)
        {
            ParameterVariableRecord* parameterVariableRecord = &inputVariableStep->parameters[i];
            memcpy(parameterVariableRecord->pointer->address, parameterVariableRecord->data, parameterVariableRecord->pointer->length);
        }
        vector<StateTraceNode*> parentNodeList;
        StateTraceNode* tempNode = baseStateNode;
        while (tempNode != stateTraceTree.root)
        {
            parentNodeList.insert(parentNodeList.begin(), tempNode);
            tempNode = tempNode->parent;
        }
        for (int i = 0; i < parentNodeList.size(); i++)
        {
            StateTraceNode* parentNode = parentNodeList[i];
            for (int j = 0; j < parentNode->inputVariablesStep->stepInputs.size(); j++)
            {
                InputVariableCuple* inputVariableCuple = &parentNode->inputVariablesStep->stepInputs[j];

                tryStateBranchInnerOneStep(inputVariableCuple);
            }
        }
    }
    else
    {
        resetGlobalVariables(baseStateNode);
    }
    
    addStateTraceNode(baseStateNode, expectStateBranchId, inputVariableStep);

    for (int i = 0; i < inputVariableStep->stepInputs.size(); i++)
    {
        InputVariableCuple* inputVariableCuple = &inputVariableStep->stepInputs[i];

        tryStateBranchInnerOneStep(inputVariableCuple);
    }

    // ????????????????????
    // ???????????????????
    int newBranchCoveredCount = 0;
    for (int i = 0; i < BitmapCov::branchBitmapLength; i++) {
         if (BitmapCov::branchAllBitmap[i] && !lastBranchCovered[i])
             newBranchCoveredCount++;
    }

    stateTraceTree.currentNode->achieveNewBranch = newBranchCoveredCount > 0;
    stateTraceTree.currentNode->exceptBranchId = expectStateBranchId;
    if(expectStateBranchId != -1)
        stateTraceTree.currentNode->achieveExceptBranch = BitmapCov::branchBitmap[expectStateBranchId] != 0;

    memcpy(stateTraceTree.currentNode->branchIdCovered, BitmapCov::branchBitmap, BitmapCov::branchBitmapLength);
    memcpy(stateTraceTree.currentNode->branchIdCoveredTotal, BitmapCov::branchAllBitmap, BitmapCov::branchBitmapLength);

    // ???????????????????
    collectGlobalVariableStep(stateTraceTree.currentNode);

    // ?????????????????
    // if (StateSearch::paraHeuristics && newBranchCoveredCount > 0)
    // {
    //     addPriorityQueueToSolveForChildrenBranch(stateTraceTree.currentNode, lastBranchCovered);
    // }
    if (StateSearch::paraHeuristics &&
        stateTraceTree.currentNode->parent &&
        stateTraceTree.currentNode->dataHashCutArray != stateTraceTree.currentNode->parent->dataHashCutArray)
    {
        addPriorityQueueToSolveForDataWrite(stateTraceTree.currentNode, lastBranchCovered);
    }

    delete[] lastBranchCovered;

    

    // ????????????
    if (stateTraceTree.currentNode->achieveExceptBranch)
        return true;

    return false;
}

bool StateSearch::tryStateBranchInnerOneStep(InputVariableCuple* inputVariableCuple)
{
    BitmapCov::clearCurrentBitmap();

    if (StateSearch::paraDistanceGuided &&
        stateTraceTree.currentNode->runMode != StateTraceNode::RunMode::DistanceGuided)
    {
        updateArrayDistanceValue_pre();
    }

    char* data = (char*)malloc(StateSearch::inputsDataLengthTotal);
    char* addr = data;
    for (int i = 0; i < inputVariableCuple->allInputVariable.size(); i++)
    {
        memcpy(addr,
            inputVariableCuple->allInputVariable[i].data,
            inputVariableCuple->allInputVariable[i].pointer->length);
        addr += inputVariableCuple->allInputVariable[i].pointer->length;
    }
    tryStateBranch(data, StateSearch::inputsDataLengthTotal);
    free(data);

    BitmapCov::step();

    // ?????????????
    //stateTraceTree.currentNode->branchDistance = StateSearch::branchDistance;
    if (StateSearch::paraDistanceGuided &&
        stateTraceTree.currentNode->runMode != StateTraceNode::RunMode::DistanceGuided)
    {
        updateBranchDistanceData(stateTraceTree.currentNode, inputVariableCuple);

        updateArrayDistanceValue_post();
        updateArrayDistanceData(stateTraceTree.currentNode, inputVariableCuple);
    }

    return false;
}


void StateSearch::solveInputVariable(StateTraceNode* stateTraceNode, int branchId)
{
    //stateTraceNode->solvedBranchResultMap[branchId] = false;
    
    
    //
    //std::cout << "\nHash: " << hashStr << "\n";
    
    bool jumpSolve = false;
    int breakerBranchId = -1;
    if(StateSearch::paraHeuristics && 
        stateCanNotSolveMap.find(stateTraceNode->dataHashCutArray) != stateCanNotSolveMap.end())
    {
        vector<int> &branchIdList = stateCanNotSolveMap[stateTraceNode->dataHashCutArray];
        int currentBranch = branchId;
        if(StateSearchBasicBranchRegister::StateSearch_Branch_Parent[currentBranch] != -1)
            currentBranch = StateSearchBasicBranchRegister::StateSearch_Branch_Parent[currentBranch];
        else if(StateSearchBasicBranchRegister::StateSearch_Branch_Brother[currentBranch] != -1)
            currentBranch = StateSearchBasicBranchRegister::StateSearch_Branch_Brother[currentBranch];
        else
            currentBranch = -1;
        while(currentBranch != -1)
        {
            if(std::find(branchIdList.begin(), branchIdList.end(), currentBranch) != branchIdList.end())
            {
                jumpSolve = true;
                breakerBranchId = currentBranch;
                break;
            }
            if(StateSearchBasicBranchRegister::StateSearch_Branch_Parent[currentBranch] != -1)
                currentBranch = StateSearchBasicBranchRegister::StateSearch_Branch_Parent[currentBranch];
            else if(StateSearchBasicBranchRegister::StateSearch_Branch_Brother[currentBranch] != -1)
                currentBranch = StateSearchBasicBranchRegister::StateSearch_Branch_Brother[currentBranch];
            else
                currentBranch = -1;
        }
    }

    if(StateSearch::paraHeuristics && jumpSolve && !StateSearch::paraSimpleLog)
    {
        string logStr;
        logStr = "- - Jump Solving, Breaker Branch Found: " + to_string(breakerBranchId) + "\n";
        std::cout << logStr;
        //string exePath = getExeDirPath();
        appendFile(exePath + "/log.txt", logStr);
        return;
    }

    solveInputVariableMultiStep(stateTraceNode, branchId, StateSearch::paraSolveMultiple);
    // if (!stateTraceNode->solvedBranchResultMap[branchId]) {
    // 
    //     string logStr;
    //     logStr = "Try multi-step solving\n";
    //     std::cout << logStr;
    //     string exePath = getExeDirPath();
    //     appendFile(exePath + "/log.txt", logStr);
    // 
    //     solveInputVariableMultiStep(stateTraceNode, branchId, 2);
    // }

    if(StateSearch::paraHeuristics && !stateTraceNode->solvedBranchResultMap[branchId]) // ??????????????????????????????????????????????????????????????????
    {
        stateCanNotSolveMap[stateTraceNode->dataHashCutArray].push_back(branchId);
    }
}

void StateSearch::solveInputVariableMultiStep(StateTraceNode* stateTraceNode, int branchId, int step)
{
    std::string hashStr = uint64ToString(stateTraceNode->dataHashCutArray);
    string stepPerfix = (step == 1) ? "" : (to_string(step) + "_");
    string resultFileName = "Hash_" + stepPerfix + hashStr + "_" + to_string(branchId) + ".xml";
    string resultFilePath = "Temp/" + resultFileName;
    bool jumped = false;
    if (StateSearch::paraHeuristics && isFileExist(resultFilePath) && getFileSize(resultFilePath) > 0)
    {
        if (!StateSearch::paraSimpleLog)
        {
            string logStr;
            logStr = "* * Jump Solving, Hash File Found: " + resultFileName + "\n";
            std::cout << logStr;
            //string exePath = getExeDirPath();
            appendFile(exePath + "/log.txt", logStr);
        }
        jumped = true;
    }
    else
    {
        solveTimes++;

        stateTraceNode->solveId.push_back(solveTimes);

        string solvingCodePath = generateCodeWithNewGlobalVariable(stateTraceNode, step);

        callCBMC(branchId, solvingCodePath, resultFilePath);

        //string sourceFilePath = "Temp/out" + to_string(solveTimes) + ".txt";

        //copyFileOne(sourceFilePath, resultFilePath);
    }
    parseCBMCResult(stateTraceNode, branchId, resultFilePath, jumped);
}

namespace 
{
    string codeTemplate[2];
}

string StateSearch::generateCodeWithNewGlobalVariable(StateTraceNode* stateTraceNode, int multiStep)
{
    string hashStr = uint64ToString(stateTraceNode->dataHashCutArray);

    string multiStepStr = (multiStep == 1) ? "" : (string("_") + to_string(multiStep));

    string fileName = "Temp/Code" + multiStepStr + "_" + hashStr + ".c";
    if(isFileExist(fileName))
        return fileName;

    GlobalVariableStep* currentGlobalVariables = stateTraceNode->globalVariablesStep;
    if(codeTemplate[multiStep - 1].empty())
    {
        //codeTemplate = readFile("CodeForSolver.c");
        //if(codeTemplate.empty())
        codeTemplate[multiStep - 1] = readFile("main" + multiStepStr + ".c");
        for(int i = 0; i < currentGlobalVariables->allGlobalVariable.size(); i++)
        {
            GlobalVariableRecord* globalVariableRecord = &(currentGlobalVariables->allGlobalVariable[i]);
            string targetStr = "// Need to be modified: Variable " + globalVariableRecord->pointer->name;
            size_t endPos = codeTemplate[multiStep - 1].find(targetStr) + targetStr.length() - 1;
            size_t startPos = codeTemplate[multiStep - 1].rfind("\n", endPos) + 1;
            codeTemplate[multiStep - 1] = codeTemplate[multiStep - 1].replace(startPos, endPos - startPos + 1, "${" + globalVariableRecord->pointer->name + "}$");
        }
    }

    string code = codeTemplate[multiStep - 1];
    for(int i = 0; i < currentGlobalVariables->allGlobalVariable.size(); i++)
    {
        GlobalVariableRecord* globalVariableRecord = &(currentGlobalVariables->allGlobalVariable[i]);
        string varStr = getVariableStr(globalVariableRecord);
        code = stringReplaceAll(code, "${" + globalVariableRecord->pointer->name + "}$", varStr);
    }

    writeFile(fileName, code);
    return fileName;
}

string StateSearch::getVariableValueStr(GlobalVariableRecord* variableRecord, int offset)
{
    return getVariableValueStr(variableRecord->pointer->type, variableRecord->data, offset);
}

string StateSearch::getVariableValueStr(InputVariableRecord* variableRecord)
{
    return getVariableValueStr(variableRecord->pointer->type, variableRecord->data, 0);
}

string StateSearch::getVariableValueStr(ParameterVariableRecord* variableRecord)
{
    return getVariableValueStr(variableRecord->pointer->type, variableRecord->data, 0);
}

string StateSearch::getVariableValueStr(string type, char* addr, int offset)
{
    string retCode;
    if(type == "u8")
    {
        unsigned char ret;
        ::memcpy(&ret, addr + offset * sizeof(unsigned char), sizeof(unsigned char));
        retCode += to_string(ret);
    }
    if(type == "i8")
    {
        char ret;
        ::memcpy(&ret, addr + offset * sizeof(char), sizeof(char));
        retCode += to_string(ret);
    }
    if(type == "i16")
    {
        short ret;
        ::memcpy(&ret, addr + offset * sizeof(short), sizeof(short));
        retCode += to_string(ret);
    }
    if(type == "i32")
    {
        int ret;
        ::memcpy(&ret, addr + offset * sizeof(int), sizeof(int));
        retCode += to_string(ret);
    }
    if(type == "i64")
    {
        long long ret;
        ::memcpy(&ret, addr + offset * sizeof(long long), sizeof(long long));
        retCode += to_string(ret);
    }
    if(type == "u16")
    {
        unsigned short ret;
        ::memcpy(&ret, addr + offset * sizeof(unsigned short), sizeof(unsigned short));
        retCode += to_string(ret);
    }
    if(type == "u32")
    {
        unsigned int ret;
        ::memcpy(&ret, addr + offset * sizeof(unsigned int), sizeof(unsigned int));
        retCode += to_string(ret);
    }
    if(type == "u64")
    {
        unsigned long long ret;
        ::memcpy(&ret, addr + offset * sizeof(unsigned long long), sizeof(unsigned long long));
        retCode += to_string(ret);
    }
    if(type == "f32")
    {
        float ret;
        ::memcpy(&ret, addr + offset * sizeof(float), sizeof(float));
        retCode += to_string(ret);
    }
    if(type == "f64")
    {
        double ret;
        ::memcpy(&ret, addr + offset * sizeof(double), sizeof(double));
        retCode += to_string(ret);
    }
    if(retCode == "inf")
        retCode = "9999999999";
    else if(retCode == "nan")
        retCode = "0";
    return retCode;
}

void StateSearch::getVariableValueBinary(string textData, string type, char* addr, int offset)
{
    string retCode;
    if(type == "u8")
    {
        unsigned char ret;
        int t = atoi(textData.c_str());
        ret = t;
        ::memcpy(addr + offset, &ret, sizeof(unsigned char));
    }
    else if(type == "i8")
    {
        char ret;
        int t = atoi(textData.c_str());
        ret = t;
        ::memcpy(addr + offset, &ret, sizeof(char));
    }
    else if(type == "i16")
    {
        short ret;
        int t = atoi(textData.c_str());
        ret = t;
        ::memcpy(addr + offset, &ret, sizeof(short));
    }
    else if(type == "i32")
    {
        int ret = atoi(textData.c_str());
        ::memcpy(addr + offset, &ret, sizeof(int));
    }
    else if(type == "i64")
    {
        long long ret = atoll(textData.c_str());
        ::memcpy(addr + offset, &ret, sizeof(long long));
    }
    else if(type == "u16")
    {
        unsigned short ret;
        int t = atoi(textData.c_str());
        ret = t;
        ::memcpy(addr + offset, &ret, sizeof(unsigned short));
    }
    else if(type == "u32")
    {
        unsigned int ret = atoi(textData.c_str());
        ::memcpy(addr + offset, &ret, sizeof(unsigned int));
    }
    else if(type == "u64")
    {
        unsigned long long ret = atoll(textData.c_str());
        ::memcpy(addr + offset, &ret, sizeof(unsigned long long));
    }
    else if(type == "f32")
    {
        float ret = atof(textData.c_str());
        ::memcpy(addr + offset, &ret, sizeof(float));
    }
    else if(type == "f64")
    {
        double ret = atof(textData.c_str());
        ::memcpy(addr + offset, &ret, sizeof(double));
    }
}

string StateSearch::getVariableStr(GlobalVariableRecord* variableRecord)
{
    string retCode;
    string indexStr;
    

    int count = 1;
    for(int i = 0; i < variableRecord->pointer->arraySize.size(); i++)
    {
        count *= variableRecord->pointer->arraySize[i];
    }

    if(variableRecord->pointer->arraySize.empty())
    {
        retCode += variableRecord->pointer->name + " = ";
        retCode += getVariableValueStr(variableRecord, 0);
        retCode += ";\n";
    }
    for(int i = 0; i < count && count > 1; i++)
    {
        if(i >= StateSearch::unwindCount && StateSearch::unwindCount != 0)
            break;
        retCode += "*(" + variableRecord->pointer->name + " + " + to_string(i) + ")" + " = ";
        retCode += getVariableValueStr(variableRecord, i);
        retCode += ";\n";
    }
    
    return retCode;
}


void StateSearch::callCBMC(int branchId, string solvingCodePath, string resultPath)
{
    string callCBMC = "cbmc.exe";

    string cmd;
	string para = "--gcc";
    para += " --arch i386";
    para += " --cover cover";
    //para += " --z3";
    para += cbmcParameter;
    para += " --xml-ui";
    para += " --partial-loops";
    para += " --reachability-slice";
    para += " --slice-formula";
    para += " --property " + stateBranchList[branchId];
    if(StateSearch::unwindCount != 0)
        para += " --unwind " + to_string(StateSearch::unwindCount);
    para += " " + solvingCodePath;
    cmd = callCBMC + " " + para + " > " + resultPath;
    appendFile("Temp/cmd.txt", cmd + "\n");

    system(cmd.c_str());
}

void StateSearch::parseCBMCResult(StateTraceNode* stateTraceNode, int branchId, string resultFilePath, bool formJumpSolvedFile)
{
    StateTraceNode* currentNode = stateTraceNode;
    tinyxml2::XMLDocument doc;
    string resultFile = resultFilePath; //"Temp/out" + to_string(solveTimes) + ".txt";
	tinyxml2::XMLError ret = doc.LoadFile(resultFile.c_str());
    if (ret != 0) {
		fprintf(stderr, "Fail to load CBMC result file: %s\n", resultFile.c_str());
        return;
	}
    tinyxml2::XMLElement* root = doc.RootElement();

    const tinyxml2::XMLElement *c = root->FirstChildElement("test");
	while (c) {
        InputVariableStep inputs;
        inputs.formJumpSolvedFile = formJumpSolvedFile;
        const tinyxml2::XMLElement *c2 = c->FirstChildElement("inputs");
	    if(c2) {
	        const tinyxml2::XMLElement *c3 = c2->FirstChildElement("input");

            while (c3 && StateSearch::parametersDataLengthTotal > 0) {
                ParameterVariableRecord para;
                para.pointer = &StateSearch::modelParameterList[inputs.parameters.size()];
                para.data = parseCBMCResultOneValue(c3);

                inputs.parameters.push_back(para);

                c3 = c3->NextSiblingElement("input");

                if (inputs.parameters.size() == StateSearch::modelParameterList.size()) {
                    break;
                }
            }

            InputVariableCuple inputCuple;
	        while (c3) {
                InputVariableRecord input;
                //string name = string(c3->Attribute("id"));

                input.pointer = &StateSearch::modelInputList[inputCuple.allInputVariable.size()];
                input.data = parseCBMCResultOneValue(c3);
                
                inputCuple.allInputVariable.push_back(input);

                if(inputCuple.allInputVariable.size() == StateSearch::modelInputList.size()) {
                    inputs.stepInputs.push_back(inputCuple);
                    inputCuple.allInputVariable.clear();
                }
	            c3 = c3->NextSiblingElement("input");
	        }
	    }

        if(!inputs.stepInputs.empty() && !inputs.stepInputs[0].allInputVariable.empty()) {
            stateTraceTree.allInputVariableStepList.push_back(inputs);
        }
        c2 = c->FirstChildElement("goal");
	    while(c2) {
	        string pathIdStr = string(c2->Attribute("id"));

            if(stateBranchList.at(branchId) == pathIdStr) {
                currentNode->solvedStateSelectInputList[branchId] = inputs;
            }
            c2 = c2->NextSiblingElement("goal");
	    }

		c = c->NextSiblingElement("test");
	}
    if(currentNode->solvedStateSelectInputList.find(branchId) != currentNode->solvedStateSelectInputList.end()) {
        if(currentNode->solvedBranchResultMap.find(branchId) == currentNode->solvedBranchResultMap.end()) {
            currentNode->solvedBranchResultMap[branchId] = 1;
        } else {
            currentNode->solvedBranchResultMap[branchId]++;
        }
    }
}

char* StateSearch::parseCBMCResultOneValue(const tinyxml2::XMLElement* input)
{
    char* retData = nullptr;
    const tinyxml2::XMLElement* c4_1 = input->FirstChildElement("integer");
    const tinyxml2::XMLElement* c4_2 = input->FirstChildElement("float");
    if (c4_1) {
        string type = string(c4_1->Attribute("c_type"));
        if (type == "int") {
            int value = c4_1->IntText();
            retData = new char[sizeof(value)];
            ::memcpy(retData, &value, sizeof(value));
        }
        else if (type == "long long") {
            long long value = c4_1->Int64Text();
            retData = new char[sizeof(value)];
            ::memcpy(retData, &value, sizeof(value));
        }
        else if (type == "unsigned int") {
            unsigned int value = c4_1->IntText();
            retData = new char[sizeof(value)];
            ::memcpy(retData, &value, sizeof(value));
        }
        else if (type == "unsigned char") {
            unsigned char value = c4_1->IntText();
            retData = new char[sizeof(value)];
            ::memcpy(retData, &value, sizeof(value));
        }
        else if (type == "char") {
            char value = c4_1->IntText();
            retData = new char[sizeof(value)];
            ::memcpy(retData, &value, sizeof(value));
        }
        else if (type == "short" || type == "short int") {
            short value = c4_1->IntText();
            retData = new char[sizeof(value)];
            ::memcpy(retData, &value, sizeof(value));
        }
        else if (type == "unsigned short") {
            unsigned short value = c4_1->IntText();
            retData = new char[sizeof(value)];
            ::memcpy(retData, &value, sizeof(value));
        }

    }
    else if (c4_2) {
        string type = "float";
        if (c4_2->FindAttribute("c_type")) {
            type = string(c4_2->Attribute("c_type"));
        }
        else {
            string width = string(c4_2->Attribute("width"));
            type = width == "64" ? "double" : "float";
        }
        if (type == "float") {
            float value = c4_2->FloatText();
            if (isnan(value))
                value = 0;
            retData = new char[sizeof(value)];
            ::memcpy(retData, &value, sizeof(value));
        }
        else if (type == "double") {
            double value = c4_2->DoubleText();
            if (isnan(value))
                value = 0;
            retData = new char[sizeof(value)];
            ::memcpy(retData, &value, sizeof(value));
        }
    }
    return retData;
}

int StateSearch::stateSearch(int& iteratorVariable, bool &timeOut)
{
    StateTraceNode* currentNode = stateTraceTree.currentNode;
    
    searchedCount++;

    selectState(timeOut);
    if(timeOut)
        return 0;


    if(StateSearch::paraNoRandom && !stateTraceTree.currentNode)
    {
        StateSearch::finish = 1;
        return 0;
    }

    // ??????????????????????????????????
    // stateTraceTree.allInputVariableStepList?????
    //if(stateTraceTree.allInputVariableStepList.empty())
    //    return -1;

    if(!stateTraceTree.currentNode)
    {
        while(!stateTraceTree.currentNode)
        {
            randomRun(500);

            if (stateTraceTree.currentNode && 
                !StateSearch::paraNoRandom && 
                paraOutputSomeLongSequence > 0)
            {
                outputLongSequence();
            }
        }
        /* Rescan all nodes after random run: otherwise lastTestTreeNodeId==nodeCount makes the next
           selectState inner loop j in [nodeCount,size) empty, so only the single new leaf is tried. */
        lastTestTreeNodeId = 0;

        achievedCount = BitmapCov::branchCov;

        

        outputNewCoverageTestCase();
    
        printTCGSSState(iteratorVariable);

        return BitmapCov::branchCov;
    }
  

    currentNode = stateTraceTree.currentNode;
    if(!currentNode)
    {
        return -1;
    }

    
    // ?????????????????????????????????????????
    int hadChildNode = -1;
    for(int i = 0; i < currentNode->children.size(); i++) {
        StateTraceNode* child = currentNode->children[i];
        if(child->exceptBranchId == stateTraceTree.selectedBranchId) {
            hadChildNode = i;
            break;
        }
    }
    if(hadChildNode != -1) {
        stateTraceTree.currentNode = currentNode->children[hadChildNode];
        return achievedCount;
    }
    

    currentNode->runMode = StateTraceNode::RunMode::Normal;
    bool tryRes = tryStateBranchInnerMultiStep(currentNode, stateTraceTree.selectedBranchId, &currentNode->solvedStateSelectInputList[stateTraceTree.selectedBranchId]);
    
        
    achievedCount = BitmapCov::branchCov;

    outputNewCoverageTestCase();
    
    printTCGSSState(iteratorVariable);

    return BitmapCov::branchCov;
}

void StateSearch::collectGlobalVariableStep(StateTraceNode* stateTraceNode)
{
    if(!hashBuffer)
    {
        hashBuffer = new uint8_t[globalVariableDataLengthTotal];
        if(StateSearch::unwindCount != 0)
        {
            hashBufferCutArray = new uint8_t[globalVariableDataLengthTotal];
            memset((void*)hashBufferCutArray, 0, globalVariableDataLengthTotal);
        }
    }

    int addressOffset = 0;
    int addressOffsetCutArray = 0;
    GlobalVariableStep* globalVariablesStepTemp = new GlobalVariableStep();
    for(int i = 0; i < globalVariableList.size(); i++)
    {
        GlobalVariableData* ref = &globalVariableList[i];
        GlobalVariableRecord targetRecord;
        targetRecord.data = new char[ref->length];
        memcpy(targetRecord.data, ref->address, ref->length);

        memcpy(hashBuffer + addressOffset, ref->address, ref->length);
        addressOffset += ref->length;

        if(StateSearch::unwindCount != 0)
        {
            int typeSize = typeSizeMap[ref->type];
            int copyCount = StateSearch::unwindCount * typeSize;
            if(copyCount > ref->length)
                copyCount = ref->length;
            memcpy(hashBufferCutArray + addressOffsetCutArray, ref->address, copyCount);
            addressOffsetCutArray += copyCount;
        }
        

        //targetRecord.length = ref->length;
        //targetRecord.type = ref->type;
        //targetRecord.name = ref->name;
        //targetRecord.arraySize = ref->arraySize;
        targetRecord.pointer = ref;

        globalVariablesStepTemp->allGlobalVariable.push_back(targetRecord);
    }
    stateTraceNode->globalVariablesStep = globalVariablesStepTemp;

    uint64_t hash = crc64(hashBuffer, globalVariableDataLengthTotal);
    stateTraceNode->dataHash = hash;

    if(StateSearch::unwindCount != 0) {
        uint64_t hash = crc64(hashBufferCutArray, globalVariableDataLengthTotal);
        stateTraceNode->dataHashCutArray = hash;
    } else {
        stateTraceNode->dataHashCutArray = stateTraceNode->dataHash;
    }

    if (StateSearch::allDataHashMap.find(stateTraceNode->dataHash) == StateSearch::allDataHashMap.end())
    {
        stateTraceNode->achieveNewStateHash = true;
        StateSearch::allDataHashMap[stateTraceNode->dataHash] = true;
    }
    if (StateSearch::allDataHashCutArrayMap.find(stateTraceNode->dataHashCutArray) == StateSearch::allDataHashCutArrayMap.end())
    {
        stateTraceNode->achieveNewStateHashCutArray = true;
        StateSearch::allDataHashCutArrayMap[stateTraceNode->dataHashCutArray] = true;
    }

    //std::stringstream ss;
    //ss << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << hash;
    //std::string hashStr = ss.str();
    //
    //std::cout << "\nHash: " << hashStr << "\n";
}

void StateSearch::collectInputVariableStep(StateTraceNode* stateTraceNode, InputVariableStep* parameters)
{
    InputVariableStep* inputVariablesStepTemp = new InputVariableStep();
    inputVariablesStepTemp->stepInputs.resize(parameters->stepInputs.size());
    for(int i = 0; i < parameters->stepInputs.size(); i++)
    {
        InputVariableCuple* refCuple = &parameters->stepInputs[i];
        InputVariableCuple* targetCuple = &inputVariablesStepTemp->stepInputs[i];

        targetCuple->allInputVariable.resize(refCuple->allInputVariable.size());
        for (int j = 0; j < refCuple->allInputVariable.size(); j++)
        {
            InputVariableRecord* refRecord = &refCuple->allInputVariable[j];
            InputVariableRecord* targetRecord = &targetCuple->allInputVariable[j];

            targetRecord->pointer = refRecord->pointer;
            targetRecord->data = new char[refRecord->pointer->length];
            memcpy(targetRecord->data, refRecord->data, refRecord->pointer->length);

        }
    }
    stateTraceNode->inputVariablesStep = inputVariablesStepTemp;
    stateTraceNode->inputVariablesStep->formJumpSolvedFile = parameters->formJumpSolvedFile;
}

void StateSearch::addStateTraceNode(StateTraceNode* stateTraceNode, int expectStateBranchId, InputVariableStep* inputVariableStep)
{
    StateTraceNode* nodeTemp = new StateTraceNode();
    nodeTemp->id = stateTraceTree.nodeCount;
    stateTraceTree.currentNode->children.push_back(nodeTemp);
    if(stateTraceTree.currentNode->branchChildSearchedCount.find(expectStateBranchId) == stateTraceTree.currentNode->branchChildSearchedCount.end())
        stateTraceTree.currentNode->branchChildSearchedCount[expectStateBranchId] = 0;
    stateTraceTree.currentNode->branchChildSearchedCount[expectStateBranchId]++;

    nodeTemp->parent = stateTraceTree.currentNode;
    nodeTemp->depth = nodeTemp->parent->depth + inputVariableStep->stepInputs.size();
    nodeTemp->exceptBranchId = expectStateBranchId;
    nodeTemp->branchIdCovered = new unsigned char[BitmapCov::branchBitmapLength];
    nodeTemp->branchIdCoveredTotal = new unsigned char[BitmapCov::branchBitmapLength];

   
    collectInputVariableStep(nodeTemp, inputVariableStep);

    stateTraceTree.currentNode = nodeTemp;
    stateTraceTree.nodeCount++;
    stateTraceTree.allNodeList.push_back(nodeTemp);

    //stateTraceTree.unsolvedNodeList.push_back(nodeTemp);
}

/*
bool StateSearch::tryStateBranchInner(StateTraceNode* stateTraceNode, int expectStateBranchId, InputVariableStep* replaceInput)
{
    // ??????????????????
    unsigned char* lastBranchCovered = new unsigned char[BitmapCov::branchBitmapLength];
    memcpy(lastBranchCovered, BitmapCov::branchAllBitmap, BitmapCov::branchBitmapLength);

    BitmapCov::clearCurrentBitmap();

    addStateTraceNode(stateTraceNode, expectStateBranchId, replaceInput);
    
    tryStateBranchInner2(stateTraceTree.currentNode);
    
    bool newBranchCovered = BitmapCov::step();


    collectGlobalVariableStep(stateTraceTree.currentNode);
    
    if(StateSearch::paraHeuristics && newBranchCovered)
    {
        addPriorityQueueToSolveForChildrenBranch(stateTraceTree.currentNode, lastBranchCovered);
    }
    if(StateSearch::paraHeuristics &&
        stateTraceTree.currentNode->parent && 
        stateTraceTree.currentNode->dataHash != stateTraceTree.currentNode->parent->dataHash)
    {
        addPriorityQueueToSolveForDataWrite(stateTraceTree.currentNode, lastBranchCovered);
    }

    delete[] lastBranchCovered;

    // ???Step????????
    
    memcpy(stateTraceTree.currentNode->branchIdCovered, BitmapCov::branchBitmap, BitmapCov::branchBitmapLength);
    memcpy(stateTraceTree.currentNode->branchIdCoveredTotal, BitmapCov::branchAllBitmap, BitmapCov::branchBitmapLength);
    stateTraceTree.currentNode->exceptBranchId = expectStateBranchId;
    if(replaceInput)
    {
        int rand_ = rand() % 10000;
        stateTraceTree.currentNode->achieveExceptBranch = rand_ < 20 ? true : false;
    }
    else
    {
        stateTraceTree.currentNode->achieveExceptBranch = BitmapCov::branchBitmap[expectStateBranchId] != 0;
    }
    stateTraceTree.currentNode->achieveExceptBranch = newBranchCovered;
    stateTraceTree.currentNode->achieveNewBranch = newBranchCovered;


    // ????????????
    if(stateTraceTree.currentNode->achieveExceptBranch)
        return true;
    
    return false;
}
*/

void StateSearch::resetGlobalVariables(StateTraceNode* stateTraceNode)
{
    GlobalVariableStep* globalVariables = stateTraceNode->globalVariablesStep;
    for(int i = 0; i < globalVariables->allGlobalVariable.size(); i++)
    {
        GlobalVariableRecord* record = &(globalVariables->allGlobalVariable[i]);
        memcpy(record->pointer->address, record->data, record->pointer->length);
    }
}

void StateSearch::printStateTraceTree(StateTraceNode* node, string& nodeCode, string& linkCode)
{
    nodeCode += to_string((long long)node);
    nodeCode += "[label=\"";
    nodeCode += "ID:";
    nodeCode += to_string(node->id);
    nodeCode += "\\nExcept:";
    nodeCode += to_string(node->exceptBranchId+1);
    nodeCode += "\\nSolveId:";
    for(int i = 0; i < node->solveId.size(); i++){
        nodeCode += to_string(node->solveId[i]+1) + ",";
    }
    nodeCode += "\\nSol:";
    for(int i = 0; i < BitmapCov::branchBitmapLength; i++){
        if(node->solvedStateSelectInputList.find(i) != node->solvedStateSelectInputList.end()){
            nodeCode += to_string(i+1) + ",";
        }
    }
    //nodeCode += "\\nInputs:";
    //for(int i = 0; i < node->inputVariablesStep->allInputVariable.size(); i++){
    //    InputVariableRecord& input = node->inputVariablesStep->allInputVariable[i];
    //    nodeCode += input.toString() + ", ";
    //}
    nodeCode += "\\nAch:";
    for(int i = 0; i < BitmapCov::branchBitmapLength; i++){
        if(node->branchIdCovered[i]){
            nodeCode += to_string(i+1) + ",";
        }
    }
    nodeCode += "\\nTot:";
    for(int i = 0; i < BitmapCov::branchBitmapLength; i++){
        if(node->branchIdCoveredTotal[i]){
            nodeCode += to_string(i+1) + ",";
        }
    }

    nodeCode += "\", width=\"1.200000\", height=\"0.600000\"];\n";

    for(int i = 0; i < node->children.size(); i++)
    {
        linkCode += to_string((long long)node) + "->" + to_string((long long)node->children[i]) + ";\n";
    }
    for(int i = 0; i < node->children.size(); i++)
    {
        printStateTraceTree(node->children[i], nodeCode, linkCode);
    }
}

void StateSearch::printStateTraceTree(string outputPath)
{
    string vizCode;
    
    vizCode += "digraph {\n";
	vizCode += "graph [overlap=scalexy, rankdir=LR, ranksep=\"0.800000\", nodesep=\"1.300000\"];\n";
	vizCode += "splines=spline\n";
	vizCode += "ratio=0.6\n";
	vizCode += "node [shape=record];\n";

    string nodeCode;
    string linkCode;
    printStateTraceTree(stateTraceTree.root, nodeCode, linkCode);

    vizCode += nodeCode;
    vizCode += linkCode;
    vizCode += "}\n";

    string tempDotPath = "Temp/StateTraceTreeViz.dot";

    writeFile(tempDotPath, vizCode);

    //string exePath = getExeDirPath();
    //string callGraphViz = exePath + "/Graphviz/dot.exe";
    string callGraphViz = "dot.exe";
    string tempPlainPath = outputPath; //exePath + "/Temp/StateTraceTreeViz_svg.svg";
    string cmd;
	string para = "\"" + tempDotPath + "\"" + " -Tsvg" + " -o " + "\"" + tempPlainPath + "\"";
	systemCallWithTimeOut(callGraphViz, para, 5000, exePath + "/Temp/log.txt");
}


namespace 
{
    int testCaseIdCSV = 0;
}
void StateSearch::outputNodeTestCase(StateTraceNode* node, bool outputCSV, bool outputBin, string outputDirCSV, string outputDirBin)
{
    if(!(outputCSV || outputBin))
        return;

    TestCase testCase;
    //testCase.length = node->depth;
    int testCaseLen = 0;
    StateTraceNode* p = node;
    StateTraceNode* parameterNode = nullptr;
    while (p && p != stateTraceTree.root) {
        if (!parameterNode && p->inputVariablesStep && !p->inputVariablesStep->parameters.empty())
        {
            parameterNode = p;
        }
        if (p->inputVariablesStep && !p->inputVariablesStep->stepInputs.empty())
        {
            testCase.inputList.insert(testCase.inputList.begin(), p->inputVariablesStep);
            testCaseLen += p->inputVariablesStep->stepInputs.size();
        }
        p = p->parent;
    }
    if (testCaseLen == 1)
    {
        testCase.inputList.push_back(testCase.inputList[0]);
    }
    if (parameterNode)
    {
        testCase.parameters = parameterNode->inputVariablesStep->parameters;
    }
    else
    {
        for (int i = 0; i < StateSearch::modelParameterList.size(); i++)
        {
            ParameterVariableRecord para;
            para.pointer = &StateSearch::modelParameterList[i];
            para.data = new char[para.pointer->length];
            memcpy(para.data, para.pointer->address, para.pointer->length);
            testCase.parameters.push_back(para);
        }
    }

    vector<TestCase> tss;
    tss.push_back(testCase);

    if(outputCSV)
        outputTestCasesCSV(tss, outputDirCSV);
    // outputTestCasesXLS(tss);

    if (outputBin)
        outputTestCasesBinary(tss, outputDirBin);
}

void StateSearch::outputTestCasesCSV(vector<StateSearch::TestCase>& testCases, string outputDirCSV)
{
    if(testCases.empty())
        return;
    
    //string exePath = getExeDirPath();

    string outputDir = outputDirCSV;

    string csvHeadFileName = outputDir + "/TestCaseHead.csv";
    string csvHeadFile;

    csvHeadFile += "Time";
    for(int i = 0; i < modelInputList.size(); i++)
    {
        ModelInputVariableData* input = &modelInputList[i];
        csvHeadFile += ",";
        csvHeadFile += input->name;
    }
    writeFile(csvHeadFileName, csvHeadFile);

    if (!modelParameterList.empty())
    {
        string csvParameterHeadFileName = outputDir + "/TestCaseParameterHead.csv";
        string csvParameterHeadFile;

        for (int i = 0; i < modelParameterList.size(); i++)
        {
            ModelParameterVariableData* para = &modelParameterList[i];
            if(i != 0)
                csvParameterHeadFile += ",";
            csvParameterHeadFile += para->name;
        }
        writeFile(csvParameterHeadFileName, csvParameterHeadFile);
    }
    
    for(int i = 0; i < testCases.size(); i++)
    {
        testCaseIdCSV++;
        string csvFileName = outputDir + "/TestCase" + to_string(testCaseIdCSV) + ".csv";
        string csvFile;
        float time = 0;
        for(int j = 0; j < testCases[i].inputList.size(); j++)
        {
            InputVariableStep* inputVariableStep = testCases[i].inputList[j];

            for (int k = 0; k < inputVariableStep->stepInputs.size(); k++)
            {
                InputVariableCuple* inputVariableCuple = &inputVariableStep->stepInputs[k];

                csvFile += (k == 0 && j == 0) ? "" : "\n";
                csvFile += to_string(time);

                for (int m = 0; m < inputVariableCuple->allInputVariable.size(); m++)
                {
                    InputVariableRecord* input = &inputVariableCuple->allInputVariable[m];
                    csvFile += ",";
                    csvFile += getVariableValueStr(input);
                }
                time += 1;
            }
        }
        writeFile(csvFileName, csvFile);
        
        if (!modelParameterList.empty())
        {
            string csvParameterFileName = outputDir + "/TestCaseParameter" + to_string(testCaseIdCSV) + ".csv";
            string csvParameterFile;
            for (int j = 0; j < testCases[i].parameters.size(); j++)
            {
                ParameterVariableRecord* para = &testCases[i].parameters[j];
                if(j != 0)
                    csvParameterFile += ",";
                csvParameterFile += getVariableValueStr(para);
            }
            writeFile(csvParameterFileName, csvParameterFile);
        }
        
        
        string logStr;
        long long curTime = clock();
        long long testCostTime = (curTime - startTime);
        
        logStr += "NewTestCase: " + stringFillBlank("TestCase" + to_string(testCaseIdCSV) + ".csv", 18);
        logStr += " CostTimes: " + to_string(testCostTime) + "\n";
        std::cout << logStr;
        
        appendFile(exePath + "/log.txt", logStr);
        
        logStr = stringFillBlank("TestCase" + to_string(testCaseIdCSV) + ".csv", 20);
        logStr += " CostTimes: " + to_string(testCostTime) + "\n";
        appendFile(outputDir + "/TestCaseGenTime.txt", logStr);
        
        
        
        logStr = BitmapCov::printCoverage();

        appendFile(exePath + "/log.txt", logStr);
        

        if (!StateSearch::paraSimpleLog)
        {
            stringstream ss;
            for (int i = 0; i < BitmapCov::branchBitmapLength; i++) {
                if (BitmapCov::branchAllBitmap[i])
                    ss << 1;
                else
                    ss << 0;
            }
            ss << std::endl;

            logStr = ss.str();
            cout << logStr;
            appendFile(exePath + "/log.txt", logStr);
        }
    }
}

namespace 
{
    int testCaseIdBin = 0;
}
void StateSearch::outputTestCasesBinary(vector<TestCase>& testCases, string outputDirBin)
{
    if(testCases.empty())
        return;
    
    //string exePath = getExeDirPath();
    string outputBinaryDir = outputDirBin;

    for(int i = 0; i < testCases.size(); i++)
    {
        string binFileName = outputBinaryDir + "/TestCase" + to_string(testCaseIdBin+1) + ".bin";
        testCaseIdBin++;
        //string csvFile;
        ofstream file;
        file.open(binFileName, ios::out|ios::binary);

        for (int j = 0; j < testCases[i].parameters.size(); j++)
        {
            ParameterVariableRecord* para = &testCases[i].parameters[j];
            file.write(para->data, para->pointer->length);
        }

        for (int j = 0; j < testCases[i].inputList.size(); j++)
        {
            InputVariableStep* inputVariableStep = testCases[i].inputList[j];

            for (int k = 0; k < inputVariableStep->stepInputs.size(); k++)
            {
                InputVariableCuple* inputVariableCuple = &inputVariableStep->stepInputs[k];

                for (int m = 0; m < inputVariableCuple->allInputVariable.size(); m++)
                {
                    InputVariableRecord* input = &inputVariableCuple->allInputVariable[m];
                    file.write(input->data, input->pointer->length);
                }
            }
        }

        file.close();
    }
}

void StateSearch::printTCGSSState(int iteratorVariable)
{
    
    string logStr;

    long long curTime = clock();
    long long testCostTime = (curTime - startTime);

    logStr += "Time: " + stringFillBlank(to_string(iteratorVariable), 6) + " ";
    //logStr += "Iter: " + stringFillBlank(to_string(iteratorVariable), 6) + " ";
    logStr += "NodeCount: " + stringFillBlank(to_string(stateTraceTree.nodeCount), 6) + " ";
    logStr += "CurNode: " + stringFillBlank(to_string(stateTraceTree.currentNode->id), 6) + " ";
    logStr += "CurDepth: " + stringFillBlank(to_string(stateTraceTree.currentNode->depth), 6) + " ";
    logStr += "Parent: " + stringFillBlank(to_string(stateTraceTree.currentNode->parent->id), 6) + " ";
    logStr += "Path: " + stringFillBlank(to_string(stateTraceTree.selectedBranchId + 1), 6) + " ";
    logStr += "SolveTimes: " + stringFillBlank(to_string(solveTimes), 6) + " ";
    logStr += "Achieved: " + stringFillBlank(to_string(achievedCount), 6, true) + "/" + stringFillBlank(to_string(BitmapCov::branchBitmapLength), 6) + " ";

    logStr +=  "CostTimes: " + to_string(testCostTime) + " ";
    logStr +=  "\n";
    
    std::cout << logStr;
    //string exePath = getExeDirPath();
    appendFile(exePath + "/log.txt", logStr);
}

namespace 
{
    int lastTestOutputNodeId = 0;
}
void StateSearch::outputNewCoverageTestCase()
{
    for(int i = lastTestOutputNodeId; i < stateTraceTree.allNodeList.size(); i++)
    {
        StateTraceNode* node = stateTraceTree.allNodeList[i];
        if(!node->achieveNewBranch || node->isExistTestCaseNode) // || node->isIntermediateNode
            continue;
        
        outputNodeTestCase(node, true, true, StateSearch::paraOutputDir, StateSearch::paraOutputBinaryDir);

        if(StateSearch::paraGenerateTestCaseOnce && !node->inputVariablesStep->formJumpSolvedFile)
            StateSearch::finish = 2;
    }
    lastTestOutputNodeId = stateTraceTree.allNodeList.size();
}

bool StateSearch::isGlobalVariableChanged(StateTraceNode* stateTraceNode, string varName)
{


    return false;
}

void StateSearch::handlePause()
{
    while (StateSearch::paraCanBePaused && isFileExist("pause.txt"))
    {
        std::cout << "Pause: 3000 ms.\n";
        _sleep(3000);
    }
}

//char* StateSearch::generatedRandomParameterData(StateSearch::ModelParameterVariableData* para)
//{
//    if (para->type == "i32")
//    {
//
//    }
//    return nullptr;
//}

// ?????500??????????????????????????N????????????N?paraOutputSomeLongSequence
// ???randomRun??????????currentNode????????????????????
namespace {
    int lastOutputLongSequenceLength = 500; // ????500, 1000, 1500?????????
    int curentLengthOutputCount = 0;
    unordered_map<StateSearch::StateTraceNode*, bool> outputNodes;
}

void StateSearch::outputLongSequence()
{
    StateTraceNode* currentNode = stateTraceTree.currentNode;
    if (!currentNode)
        return;
    int currentLength = currentNode->depth;
    if (currentLength > StateSearch::paraOutputLongSequenceMaxLen)
        return;
    currentLength = currentLength / 500 * 500;
    if (currentLength < lastOutputLongSequenceLength)
        return;
    if (currentLength - lastOutputLongSequenceLength >= 500)
        curentLengthOutputCount = 0;
    if (curentLengthOutputCount >= StateSearch::paraOutputSomeLongSequence)
        return;
    if (outputNodes.find(currentNode) != outputNodes.end())
        return;

    lastOutputLongSequenceLength = currentLength;
    curentLengthOutputCount++;
    outputNodes[currentNode] = true;

    outputNodeTestCase(currentNode, false, true, StateSearch::paraOutputLongDir, StateSearch::paraOutputLongBinaryDir);

}



void StateSearch::addPriorityQueueToSolveForChildrenBranch(StateTraceNode* stateTraceNode, unsigned char* lastBranchCovered)
{
    for(int i = BitmapCov::branchBitmapLength - 1; i >=0; i--)
    {
        if(!(BitmapCov::branchAllBitmap[i] && !lastBranchCovered[i]))
        {
            continue; //???????????????????????
        }

        int branchId = i;
        if(StateSearchBasicBranchRegister::StateSearch_Branch_Children.find(branchId) == StateSearchBasicBranchRegister::StateSearch_Branch_Children.end())
        {
            continue;
        }

        for(int j = 0; j < StateSearchBasicBranchRegister::StateSearch_Branch_Children[branchId].size(); j++)
        {
            int childBranchId = StateSearchBasicBranchRegister::StateSearch_Branch_Children[branchId][j];
            if (BitmapCov::branchAllBitmap[childBranchId])
                continue;

            string logStr;
            logStr = "AddPriorityQueueForChildrenBranch  NewBranchFound: " + stringFillBlank(to_string(branchId + 1), 6) + " Insert:" + stringFillBlank(to_string(childBranchId + 1), 6) + "\n";
            
            std::cout << logStr;
            //string exePath = getExeDirPath();
            appendFile(exePath + "/log.txt", logStr);

            StateSearch::priorityQueueToSolve.push_back(make_pair(stateTraceNode, childBranchId));
        }
    }
}

void StateSearch::addPriorityQueueToSolveForDataWrite(StateTraceNode* stateTraceNode, unsigned char* lastBranchCovered)
{
    // ?????????????????????????????????????????????????????????????????????????????

    set<string> globalWriteList;
    for(int i = BitmapCov::branchBitmapLength - 1; i >=0; i--)
    {
        int branchId = StateSearch::branchSolveOrder[i];
        if(!(BitmapCov::branchAllBitmap[branchId] && !lastBranchCovered[branchId]))
        {
            continue; //???????????????????????
        }
        for (auto branchWV : StateSearch::branchWriteGlobalVar)
        {
            if(branchWV.first == branchId)
            {
                globalWriteList.insert(branchWV.second);
            }
        }
    }
    set<int> branchRelated;
    if(!globalWriteList.empty())
    {
        for (auto branchRV : StateSearch::branchReadGlobalVar)
        {
            if (std::find(globalWriteList.begin(), globalWriteList.end(), branchRV.second) != globalWriteList.end())
            {
                branchRelated.insert(branchRV.first);
            }
        }
    }

    if (!branchRelated.empty())
    {
        for (int i = BitmapCov::branchBitmapLength - 1; i >= 0; i--)
        //for (int i = 0; i < BitmapCov::branchBitmapLength; i++)
        {
            int branchId = StateSearch::branchSolveOrder[i];
            if (BitmapCov::branchAllBitmap[branchId])
            {
                continue;
            }
            if (std::find(branchRelated.begin(), branchRelated.end(), branchId) != branchRelated.end())
            {
                if(!StateSearch::paraSimpleLog)
                {
                    string logStr;
                    logStr = "AddPriorityQueueForDataWrite  Insert:" + stringFillBlank(to_string(branchId + 1), 6) + "\n";

                    std::cout << logStr;
                    //string exePath = getExeDirPath();
                    appendFile(exePath + "/log.txt", logStr);
                }

                StateSearch::priorityQueueToSolve.push_back(make_pair(stateTraceNode, branchId));
            }
        }
    }
}

double StateSearch::distanceFunction_value(double value, char opType, double target)
{
    double distance = 0;
    switch (opType)
    {
    case '<':
        if (value < target)
            distance = 0;
        else
            distance = value - target;
        break;
    case '>':
        if (value > target)
            distance = 0;
        else
            distance = target - value;
        break;
    default:
        break;
    }


    //StateSearch::branchDistance[distanceId] = distance;

    return distance;
}


int StateSearch::distanceFunction_array(unsigned char* addr, int length, unsigned char* defaultValue)
{
    int distance = length;

    for (int i = 0; i < length; i++)
    {
        if (addr[i] != defaultValue[i])
        {
            distance--;
        }
    }

    return distance;
}

void StateSearch::updateBranchDistanceData(StateTraceNode* stateTraceNode, InputVariableCuple* stepInput)
{
    //StateTraceNode* parent = stateTraceNode->parent;
    //if(parent == stateTraceTree.root)
    //    return;
    for (int i = 0; i < StateSearch::branchDistanceDataMap.size(); i++)
    {
        int distanceId = i;
        StateSearch::BranchDistanceData* distanceData = &StateSearch::branchDistanceDataMap[distanceId];

        // distanceRecordValueV1?????distanceRecordValueV2????????
        if (stateTraceNode->distanceRecordValueV1.find(distanceData->varName) ==
            stateTraceNode->distanceRecordValueV1.end())
        {
            continue;
        }

        double lastValue = stateTraceNode->distanceRecordValueV1[distanceData->varName];
        double lastDistance = distanceFunction_value(lastValue, distanceData->distanceOpType, distanceData->targetValue);

        double currentValue = stateTraceNode->distanceRecordValueV2[distanceData->varName];
        double currentDistance = distanceFunction_value(currentValue, distanceData->distanceOpType, distanceData->targetValue);

        double stepDistance = lastDistance - currentDistance;

        if (stepDistance > distanceData->maxStepDistance)
        {
            distanceData->maxStepDistance = stepDistance;

            for (auto data : distanceData->maxStepInput)
            {
                data->release();
            }
            distanceData->maxStepInput.clear();

            distanceData->maxStepInput.push_back(stepInput->clone());

            string logStr = "update distance: " + to_string(distanceId) + "\n";
            logStr += "varName: " + distanceData->varName + "\n";
            logStr += "maxStepDistance: " + to_string(distanceData->maxStepDistance) + "\n";
            logStr += "currentDistance: " + to_string(currentDistance) + "\n";

            std::cout << logStr;
            
            appendFile(exePath + "/log.txt", logStr);
        }
        else if (stepDistance == distanceData->maxStepDistance && distanceData->maxStepInput.size() < 5)
        {
            distanceData->maxStepInput.push_back(stepInput->clone());
        }
    }

    // ???Step???????????????????????????
    for (int i = 0; i < StateSearch::branchDistanceDataMap.size(); i++)
    {
        int distanceId = i;
        StateSearch::BranchDistanceData* distanceData = &StateSearch::branchDistanceDataMap[distanceId];
        stateTraceNode->distanceRecordValueV1[distanceData->varName] = stateTraceNode->distanceRecordValueV2[distanceData->varName];
    }
}


void StateSearch::updateBranchDistanceValue(const char* varName, double var)
{
    if (!StateSearch::paraDistanceGuided)
        return;

    map<string, double>* v1 = &stateTraceTree.currentNode->distanceRecordValueV1;
    bool v1Empty = (v1->find(varName) == v1->end());
    StateTraceNode* pa = stateTraceTree.currentNode->parent;
    bool paEmpty = pa ? (pa->distanceRecordValueV2.find(varName) == pa->distanceRecordValueV2.end()) : true;

    // ???distanceRecordValueV1?????????????????????????
    if (!paEmpty && v1Empty) // ?????????????????????????????????????????????
    {
        v1->emplace(varName, pa->distanceRecordValueV2[varName]);
    }
    else if (v1Empty) // ????????????????????????????????????????
    {
        v1->emplace(varName, var);
    }
    stateTraceTree.currentNode->distanceRecordValueV2[varName] = var;

    //if (string(varName) == "ComplexParaTest2_DW->A" && var < 0) //!(fabs(var - 30) < 0.0001)
    //{
    //    cout << "updateDistanceValue: " << varName << " = " << var << endl;
    //}
    //if (distanceRecordValueOriMap.find(varName) == distanceRecordValueOriMap.end())
    //{
    //    distanceRecordValueOriMap[varName] = var;
    //}
}


void StateSearch::updateArrayDistanceValue_pre()
{
    StateTraceNode* curNode = stateTraceTree.currentNode;
    if (!curNode)
        return;
    for (int i = 0; i < arrayDistanceDataMap.size(); i++)
    {
        int distanceId = i;
        StateSearch::ArrayDistanceData* distanceData = &arrayDistanceDataMap[distanceId];

        if (curNode->distanceRecordV2Array.find(distanceData->varName) != curNode->distanceRecordV2Array.end())
        {
            curNode->distanceRecordV1Array[distanceData->varName] = curNode->distanceRecordV2Array[distanceData->varName];
            continue;
        }
        else
        {
            int distance = distanceFunction_array(distanceData->arrayValue, distanceData->arrayDataLength, distanceData->defaultArrayValue);
            curNode->distanceRecordV1Array[distanceData->varName] = distance;
        }

    }

}

void StateSearch::updateArrayDistanceValue_post()
{
    StateTraceNode* curNode = stateTraceTree.currentNode;
    if (!curNode)
        return;

    for (int i = 0; i < arrayDistanceDataMap.size(); i++)
    {
        int distanceId = i;
        StateSearch::ArrayDistanceData* distanceData = &arrayDistanceDataMap[distanceId];

        int distance = distanceFunction_array(distanceData->arrayValue, distanceData->arrayDataLength, distanceData->defaultArrayValue);
        curNode->distanceRecordV2Array[distanceData->varName] = distance;
    }
}

void StateSearch::updateArrayDistanceData(StateTraceNode* stateTraceNode, InputVariableCuple* stepInput)
{
    for (int i = 0; i < StateSearch::arrayDistanceDataMap.size(); i++)
    {
        int distanceId = i;
        StateSearch::ArrayDistanceData* distanceData = &StateSearch::arrayDistanceDataMap[distanceId];

        int lastDistance = stateTraceNode->distanceRecordV1Array[distanceData->varName];

        int currentDistance = stateTraceNode->distanceRecordV2Array[distanceData->varName];

        int stepDistance = lastDistance - currentDistance;

        if (stepDistance > distanceData->maxStepDistance)
        {
            distanceData->maxStepDistance = stepDistance;

            for (auto data : distanceData->maxStepInput)
            {
                data->release();
            }
            distanceData->maxStepInput.clear();

            distanceData->maxStepInput.push_back(stepInput->clone());

            string logStr = "update distance: " + to_string(distanceId) + "\n";
            logStr += "varName: " + distanceData->varName + "\n";
            logStr += "maxStepDistance: " + to_string(distanceData->maxStepDistance) + "\n";
            logStr += "currentDistance: " + to_string(currentDistance) + "\n";

            std::cout << logStr;

            appendFile(exePath + "/log.txt", logStr);
        }
        else if (stepDistance == distanceData->maxStepDistance && distanceData->maxStepInput.size() < 5)
        {
            distanceData->maxStepInput.push_back(stepInput->clone());
        }
    }

}

StateSearch::InputVariableCuple* StateSearch::generateOneDistanceDataInput(vector<StateSearch::InputVariableCuple*>* maxStepInput)
{
    if (maxStepInput->empty())
        return nullptr;

    vector<bool> columnSame;

    InputVariableCuple* inputFirst = maxStepInput->at(0);
    int inputVarCount = inputFirst->allInputVariable.size();
    columnSame.resize(inputVarCount, true);

    for (int i = 1; i < maxStepInput->size(); i++)
    {
        InputVariableCuple* input = maxStepInput->at(i);

        for (int j = 0; j < inputVarCount; j++)
        {
            InputVariableRecord* recordFirst = &inputFirst->allInputVariable[j];
            InputVariableRecord* record = &input->allInputVariable[j];

            if (columnSame[j] && memcmp(record->data, recordFirst->data, record->pointer->length) != 0)
            {
                columnSame[j] = false;
            }
        }
    }

    InputVariableCuple* newInput = new InputVariableCuple();
    newInput->allInputVariable.resize(inputVarCount);
    for (int j = 0; j < inputVarCount; j++)
    {
        if (columnSame[j])
        {
            InputVariableRecord* recordFirst = &inputFirst->allInputVariable[j];
            InputVariableRecord newRecord;
            newRecord.pointer = recordFirst->pointer;
            newRecord.data = new char[recordFirst->pointer->length];
            memcpy(newRecord.data, recordFirst->data, recordFirst->pointer->length);
            newInput->allInputVariable[j] = newRecord;
        }
        else
        {
            InputVariableRecord newRecord;
            newRecord.pointer = inputFirst->allInputVariable[j].pointer;
            newRecord.data = (char*)generateRandomDataByType(newRecord.pointer->type, newRecord.pointer->length);
            newInput->allInputVariable[j] = newRecord;
        }
    }

    return newInput;
}

StateSearch::InputVariableCuple* StateSearch::generateRandomInputCuple()
{
    if (modelInputList.empty())
        return nullptr;
    InputVariableCuple* newInput = new InputVariableCuple();
    newInput->allInputVariable.resize(modelInputList.size());
    for (size_t j = 0; j < modelInputList.size(); j++)
    {
        InputVariableRecord newRecord;
        newRecord.pointer = &modelInputList[j];
        newRecord.data = (char*)generateRandomDataByType(modelInputList[j].type, modelInputList[j].length);
        newInput->allInputVariable[j] = newRecord;
    }
    return newInput;
}

StateSearch::InputVariableCuple* StateSearch::InputVariableCuple::clone()
{
    InputVariableCuple* newCuple = new InputVariableCuple();
    newCuple->allInputVariable.resize(this->allInputVariable.size());
    for (int i = 0; i < this->allInputVariable.size(); i++)
    {
        InputVariableRecord* record = &this->allInputVariable[i];
        InputVariableRecord newRecord;
        newRecord.pointer = record->pointer;
        newRecord.data = new char[record->pointer->length];
        memcpy(newRecord.data, record->data, record->pointer->length);
        newCuple->allInputVariable[i] = newRecord;
    }
    return newCuple;
}

void StateSearch::InputVariableCuple::release()
{
    for (int i = 0; i < this->allInputVariable.size(); i++)
    {
        InputVariableRecord* record = &this->allInputVariable[i];
        delete[] record->data;
    }
    this->allInputVariable.clear();
}
