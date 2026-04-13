/* Include Windows headers first to avoid std::byte (C++17) conflict with Windows byte typedef */
#define WIN32_LEAN_AND_MEAN
#define NOCRYPT
#include <windows.h>

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

vector<pair<StateSearch::StateTraceNode*, int>> StateSearch::priorityQueueToSolve; // First: ״̬�ڵ�     Second: ��֧Id
vector<int> StateSearch::priorityQueueToSolveForRoot; // First: ״̬�ڵ�     Second: ��֧Id

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
	paraMaxRunTime = 432000000; //�������5��ʱ��
	paraMaxRunIterator = 999999999;

    string paraGenerateTestCaseOnce = "false";
    string paraNoRandom = "false";
    string paraHeuristics = "true";
    string paraCanBePaused = "false";
    string paraOutputDir = exePath + "/TestCaseOutput";
    string paraOutputBinaryDir = exePath + "/TestCaseOutput/Binary";
    string paraSimpleLog = "true";
    int paraOutputSomeLongSequence = 0; // N�������ִ��ʱ���Ƿ����һЩ�����У�ÿ500�����������N����������
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
    helpStr += "  -outputLong <number>       Output Some Long Sequence, cond: noRand==false (default: 0)\n"; // ����fuzz parameterʱ�ṩ����input sequence
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


	// ��ʱ���θ�״̬���������з�֧���������
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
        //std::cout << logStr;
        
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
        //std::cout << logStr;
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
    
    //std::cout << logStr;
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
    // ��¼ÿ����֧��Ӧ��CBMC��ǩ
    stateBranchList = StateSearchBasicBranchRegister::StateSearch_Branch;
    vector<pair<int, int>> vtMap;
    for (auto it = StateSearchBasicBranchRegister::StateSearch_Branch_Depth.begin(); it != StateSearchBasicBranchRegister::StateSearch_Branch_Depth.end(); it++)
        vtMap.push_back(make_pair(it->first, it->second));

    // ���շ�֧���������ǳ�������
    sort(vtMap.begin(), vtMap.end(), 
        [](const pair<int, int> &x, const pair<int, int> &y) -> int {
        return x.second < y.second;
    });

    for (auto it = vtMap.begin(); it != vtMap.end(); it++)
    {
        branchSolveOrder.push_back(it->first);
    }

    // ���ݸ��׹�ϵ���ÿ����֧�ĺ��ӷ�֧
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
            //std::cout << logStr;

            //string exePath = getExeDirPath();
            appendFile(exePath + "/log.txt", logStr + "\n");
        }

        solveInputVariable(node, branchId);
        if (node->solvedBranchResultMap[branchId])
        {
            //�ܽ������ִ��·��
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
        //�Ƿ����нڵ㶼�޷�����÷�֧
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
                //std::cout << logStr << endl;
                //string exePath = getExeDirPath();
                appendFile(exePath + "/log.txt", logStr + "\n");

                // ��û���
                solveInputVariable(node, branchId);

                //for(int k = 0; k < printStr.length()+printStr2.length()+printStr3.length(); k++)
                //{
                //    std::cout << "\b";
                //}
                //std::cout << "\n";
                if(node->solvedBranchResultMap[branchId])
                {
                    //�ܽ������ִ��·��
                    //if(stateTraceTree.currentNode != node)
                    //    resetGlobalVariables(node);
                    stateTraceTree.currentNode = node;
                    stateTraceTree.selectedBranchId = branchId;

                    canSolved = true;
                    break;
                }
                else
                {
                    // ���ܽ�����������������һ���ڵ�
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
        //ȫ����֧��ȫ���ڵ㶼���ɽ�
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
    //std::cout << logStr;
    appendFile(exePath + "/log.txt", logStr);



    //int rand_ = rand() % stateTraceTree.allNodeList.size();
    //stateTraceTree.currentNode = stateTraceTree.allNodeList[rand_];
    int rand_ = 0;
    if (stateTraceTree.allNodeList.empty()) {
        stateTraceTree.currentNode = stateTraceTree.root;
        if (!stateTraceTree.currentNode) return;
    } else {
        rand_ = rand() % stateTraceTree.allNodeList.size();
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
        // ��1/3�ĸ���ѡ���֧���뵼��������뵼�����
        int pCount = 1;

        // ��branchDistanceDataMap��Ѱ�Ҿ��뵼��ķ�֧������÷�֧δ�����ǣ������ȸ��Ǹ÷�֧
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
            rand_ = rand() % pCount; // 0���������1������֧���뵼��2����������뵼��
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
            rand_ = rand() % pCount;  // 0���������1������֧���뵼��
            if (rand_ == 0) {
                targetBranchDistanceId = -1;
            }
        }
        else if (pCount == 2 && targetArrayDistanceId != -1)
        {
            rand_ = rand() % pCount; // 0���������1����������뵼��
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
                rand_ = rand() % stateTraceTree.allInputVariableStepList.size();
                InputVariableStep* tempInput = &stateTraceTree.allInputVariableStepList[rand_];
                rand_ = rand() % tempInput->stepInputs.size();
                InputVariableCuple* inputCuple = &tempInput->stepInputs[rand_];
                inputVariablesStepTemp.stepInputs[i].allInputVariable = inputCuple->allInputVariable;
               /* InputVariableCuple* inputCuple = nullptr;
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
                }*/
            }
            
        }
        else
        {
            // allInputVariableStepListΪ����ѡ�����һ���ڵ�����һ��stepInput
            for (int i = 0; i < nTimes; i++)
            {
                //rand_ = rand() % (stateTraceTree.allNodeList.size() - 1) + 1;
                //StateTraceNode* temp = stateTraceTree.allNodeList[rand_];

                //rand_ = rand() % temp->inputVariablesStep->stepInputs.size();
                //InputVariableCuple* inputCuple = &temp->inputVariablesStep->stepInputs[rand_];
                //inputVariablesStepTemp.stepInputs[i].allInputVariable = inputCuple->allInputVariable;
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
        //std::cout << logStr;
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
        //std::cout << logStr;
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
    // ��¼֮ǰ��������Щ��֧
    unsigned char* lastBranchCovered = new unsigned char[BitmapCov::branchBitmapLength];
    memcpy(lastBranchCovered, BitmapCov::branchAllBitmap, BitmapCov::branchBitmapLength);

    stateTraceTree.currentNode = baseStateNode;

    // ���������������ݴ���ģ�Ͳ���ʱ������Ҫ���µ�ģ�Ͳ�������ִ��ǰ���step
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

    // ��ִ̬����֮���ռ������Ϣ
    // �ռ���ǰ�ڵ�ķ�֧�������
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

    // �ռ���ǰ�ڵ��ȫ�ֱ�����Ϣ
    collectGlobalVariableStep(stateTraceTree.currentNode);

    // ��������ʽ��������
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

    

    // ��¼״̬�����ڵ�
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

    // ��¼��֧������Ϣ
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
        //std::cout << logStr;
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

    if(StateSearch::paraHeuristics && !stateTraceNode->solvedBranchResultMap[branchId]) // ���ɽ⣬���¼��ǰ״̬�޷�����÷�֧�����ڸ÷�֧�ĺ����ֵܽڵ���ӽڵ㣬�������ڸ�״̬�����
    {
        stateCanNotSolveMap[stateTraceNode->dataHashCutArray].push_back(branchId);
    }
}

/**
 * 求解输入变量的多步方法
 * 该方法负责为指定的状态节点和分支ID生成求解输入变量的多步骤方案
 * 
 * @param stateTraceNode 状态跟踪节点，包含当前状态的全局变量信息
 * @param branchId 目标分支ID，用于指定要求解的程序分支
 * @param step 求解步数，用于区分单步(1)和多步(>1)处理模式
 */
void StateSearch::solveInputVariableMultiStep(StateTraceNode* stateTraceNode, int branchId, int step)
{
    // 将状态节点的哈希值转换为字符串，用于生成唯一的文件名
    std::string hashStr = uint64ToString(stateTraceNode->dataHashCutArray);
    
    // 根据步数生成文件名前缀：单步为空字符串，多步为"N_"格式
    string stepPerfix = (step == 1) ? "" : (to_string(step) + "_");
    
    // 构造结果文件名：格式为"Hash_[N_]哈希值_分支ID.xml"
    string resultFileName = "Hash_" + stepPerfix + hashStr + "_" + to_string(branchId) + ".xml";
    string resultFilePath = "Temp/" + resultFileName;
    
    // 跳转标志，表示是否从已存在的结果文件中跳转求解
    bool jumped = false;
    
    // 如果启用启发式优化且结果文件已存在且非空，则跳过求解过程
    if (StateSearch::paraHeuristics && isFileExist(resultFilePath) && getFileSize(resultFilePath) > 0)
    {
        // 如果不是简单日志模式，输出跳转求解信息
        if (!StateSearch::paraSimpleLog)
        {
            string logStr;
            logStr = "* * Jump Solving, Hash File Found: " + resultFileName + "\n";
            //std::cout << logStr;
            //string exePath = getExeDirPath();
            appendFile(exePath + "/log.txt", logStr);
        }
        jumped = true;
    }
    else
    {
        // 增加求解次数计数器
        solveTimes++;

        // 将当前求解ID添加到状态节点的求解ID列表中
        stateTraceNode->solveId.push_back(solveTimes);

        // 生成包含新全局变量状态的求解代码文件
        string solvingCodePath = generateCodeWithNewGlobalVariable(stateTraceNode, step);

        // 调用CBMC求解器进行程序验证
        callCBMC(branchId, solvingCodePath, resultFilePath);

        // 注释的代码：旧版本的文件复制操作
        //string sourceFilePath = "Temp/out" + to_string(solveTimes) + ".txt";
        //copyFileOne(sourceFilePath, resultFilePath);
    }
    
    // 解析CBMC求解器的结果文件，提取输入变量数据
    parseCBMCResult(stateTraceNode, branchId, resultFilePath, jumped);
}
namespace 
{
    // 代码模板缓存数组，用于存储不同步数的代码模板
    // 索引0：单步模板，索引1：多步模板
    string codeTemplate[2];
}

/**
 * 生成包含新全局变量的代码文件
 * 该函数根据状态跟踪节点的全局变量状态生成相应的C代码文件，用于CBMC求解器分析
 * 
 * @param stateTraceNode 状态跟踪节点，包含当前状态的全局变量信息
 * @param multiStep 多步标志，用于区分单步(1)和多步(>1)处理模式
 * @return 返回生成的代码文件路径
 */
string StateSearch::generateCodeWithNewGlobalVariable(StateTraceNode* stateTraceNode, int multiStep)
{
    // 将状态节点的哈希值转换为字符串，用于生成唯一的文件名
    string hashStr = uint64ToString(stateTraceNode->dataHashCutArray);

    // 根据多步标志生成文件名后缀：单步为空字符串，多步为"_N"格式
    string multiStepStr = (multiStep == 1) ? "" : (string("_") + to_string(multiStep));

    // 构造生成的代码文件名：格式为"Temp/Code[_N]_哈希值.c"
    string fileName = "Temp/Code" + multiStepStr + "_" + hashStr + ".c";
    
    // 如果目标文件已存在，直接返回文件路径（避免重复生成）
    if(isFileExist(fileName))
        return fileName;

    // 获取当前状态节点的全局变量步骤信息
    GlobalVariableStep* currentGlobalVariables = stateTraceNode->globalVariablesStep;
    
    // 如果代码模板尚未加载，则进行初始化
    if(codeTemplate[multiStep - 1].empty())
    {
        // 从模板文件读取基础代码模板
        // 注释的代码：旧版本使用"CodeForSolver.c"作为模板
        //codeTemplate = readFile("CodeForSolver.c");
        //if(codeTemplate.empty())
        
        // 根据多步标志读取相应的模板文件：main.c 或 main_N.c
        codeTemplate[multiStep - 1] = readFile("main" + multiStepStr + ".c");
        
        // 遍历所有全局变量，在模板中为每个变量创建占位符
        for(int i = 0; i < currentGlobalVariables->allGlobalVariable.size(); i++)
        {
            GlobalVariableRecord* globalVariableRecord = &(currentGlobalVariables->allGlobalVariable[i]);
            
            // 构造目标注释字符串，用于在模板中定位变量声明位置
            string targetStr = "// Need to be modified: Variable " + globalVariableRecord->pointer->name;
            
            // 找到目标注释的结束位置
            size_t endPos = codeTemplate[multiStep - 1].find(targetStr) + targetStr.length() - 1;
            
            // 找到该行的开始位置（从换行符后开始）
            size_t startPos = codeTemplate[multiStep - 1].rfind("\n", endPos) + 1;
            
            // 将整行替换为变量占位符，格式为"${变量名}$"
            codeTemplate[multiStep - 1] = codeTemplate[multiStep - 1].replace(startPos, endPos - startPos + 1, "${" + globalVariableRecord->pointer->name + "}$");
        }
    }

    // 基于模板创建当前代码副本
    string code = codeTemplate[multiStep - 1];
    
    // 遍历所有全局变量，将占位符替换为实际的变量赋值代码
    for(int i = 0; i < currentGlobalVariables->allGlobalVariable.size(); i++)
    {
        GlobalVariableRecord* globalVariableRecord = &(currentGlobalVariables->allGlobalVariable[i]);
        
        // 生成该全局变量的赋值语句字符串
        string varStr = getVariableStr(globalVariableRecord);
        
        // 将占位符"${变量名}$"替换为实际的赋值代码
        code = stringReplaceAll(code, "${" + globalVariableRecord->pointer->name + "}$", varStr);
    }

    // 将生成的代码写入文件
    writeFile(fileName, code);
    
    // 返回生成的代码文件路径
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

/**
 * 将变量数据转换为字符串表示
 * @param type 变量类型（如"u8", "i32", "f64"等）
 * @param addr 变量数据的内存地址
 * @param offset 在指定类型大小基础上的偏移量
 * @return 变量值的字符串表示
 */
string StateSearch::getVariableValueStr(string type, char* addr, int offset)
{
    string retCode;
    
    // 处理8位无符号整数
    if(type == "u8")
    {
        unsigned char ret;
        ::memcpy(&ret, addr + offset * sizeof(unsigned char), sizeof(unsigned char));
        retCode += to_string(ret);
    }
    // 处理8位有符号整数
    if(type == "i8")
    {
        char ret;
        ::memcpy(&ret, addr + offset * sizeof(char), sizeof(char));
        retCode += to_string(ret);
    }
    // 处理16位有符号整数
    if(type == "i16")
    {
        short ret;
        ::memcpy(&ret, addr + offset * sizeof(short), sizeof(short));
        retCode += to_string(ret);
    }
    // 处理32位有符号整数
    if(type == "i32")
    {
        int ret;
        ::memcpy(&ret, addr + offset * sizeof(int), sizeof(int));
        retCode += to_string(ret);
    }
    // 处理64位有符号整数
    if(type == "i64")
    {
        long long ret;
        ::memcpy(&ret, addr + offset * sizeof(long long), sizeof(long long));
        retCode += to_string(ret);
    }
    // 处理16位无符号整数
    if(type == "u16")
    {
        unsigned short ret;
        ::memcpy(&ret, addr + offset * sizeof(unsigned short), sizeof(unsigned short));
        retCode += to_string(ret);
    }
    // 处理32位无符号整数
    if(type == "u32")
    {
        unsigned int ret;
        ::memcpy(&ret, addr + offset * sizeof(unsigned int), sizeof(unsigned int));
        retCode += to_string(ret);
    }
    // 处理64位无符号整数
    if(type == "u64")
    {
        unsigned long long ret;
        ::memcpy(&ret, addr + offset * sizeof(unsigned long long), sizeof(unsigned long long));
        retCode += to_string(ret);
    }
    // 处理32位浮点数
    if(type == "f32")
    {
        float ret;
        ::memcpy(&ret, addr + offset * sizeof(float), sizeof(float));
        retCode += to_string(ret);
    }
    // 处理64位浮点数
    if(type == "f64")
    {
        double ret;
        ::memcpy(&ret, addr + offset * sizeof(double), sizeof(double));
        retCode += to_string(ret);
    }
    
    // 处理特殊浮点值：将无穷大和NaN转换为有效数值
    if(retCode == "inf")
        retCode = "9999999999";
    else if(retCode == "nan")
        retCode = "0";
    
    return retCode;
}

/**
 * 将文本数据转换为二进制格式并存储到指定内存位置
 * @param textData 要转换的文本数据
 * @param type 目标变量类型（如"u8", "i32", "f64"等）
 * @param addr 目标内存地址
 * @param offset 在目标地址基础上的字节偏移量
 */
void StateSearch::getVariableValueBinary(string textData, string type, char* addr, int offset)
{
    // 处理8位无符号整数
    if(type == "u8")
    {
        unsigned char ret;
        int t = atoi(textData.c_str());
        ret = t;
        ::memcpy(addr + offset, &ret, sizeof(unsigned char));
    }
    // 处理8位有符号整数
    else if(type == "i8")
    {
        char ret;
        int t = atoi(textData.c_str());
        ret = t;
        ::memcpy(addr + offset, &ret, sizeof(char));
    }
    // 处理16位有符号整数
    else if(type == "i16")
    {
        short ret;
        int t = atoi(textData.c_str());
        ret = t;
        ::memcpy(addr + offset, &ret, sizeof(short));
    }
    // 处理32位有符号整数
    else if(type == "i32")
    {
        int ret = atoi(textData.c_str());
        ::memcpy(addr + offset, &ret, sizeof(int));
    }
    // 处理64位有符号整数
    else if(type == "i64")
    {
        long long ret = atoll(textData.c_str());
        ::memcpy(addr + offset, &ret, sizeof(long long));
    }
    // 处理16位无符号整数
    else if(type == "u16")
    {
        unsigned short ret;
        int t = atoi(textData.c_str());
        ret = t;
        ::memcpy(addr + offset, &ret, sizeof(unsigned short));
    }
    // 处理32位无符号整数
    else if(type == "u32")
    {
        unsigned int ret = atoi(textData.c_str());
        ::memcpy(addr + offset, &ret, sizeof(unsigned int));
    }
    // 处理64位无符号整数
    else if(type == "u64")
    {
        unsigned long long ret = atoll(textData.c_str());
        ::memcpy(addr + offset, &ret, sizeof(unsigned long long));
    }
    // 处理32位浮点数
    else if(type == "f32")
    {
        float ret = atof(textData.c_str());
        ::memcpy(addr + offset, &ret, sizeof(float));
    }
    // 处理64位浮点数
    else if(type == "f64")
    {
        double ret = atof(textData.c_str());
        ::memcpy(addr + offset, &ret, sizeof(double));
    }
}

/**
 * 生成全局变量的赋值代码字符串
 * 该函数根据全局变量记录生成相应的C代码赋值语句
 * 
 * @param variableRecord 全局变量记录，包含变量的元数据和数据
 * @return 返回生成的C代码赋值语句字符串
 */
string StateSearch::getVariableStr(GlobalVariableRecord* variableRecord)
{
    string retCode;
    string indexStr;
    
    // 计算数组的总元素个数
    int count = 1;
    for(int i = 0; i < variableRecord->pointer->arraySize.size(); i++)
    {
        count *= variableRecord->pointer->arraySize[i];
    }

    // 如果不是数组类型（arraySize为空），生成简单的赋值语句
    if(variableRecord->pointer->arraySize.empty())
    {
        retCode += variableRecord->pointer->name + " = ";
        retCode += getVariableValueStr(variableRecord, 0);
        retCode += ";\n";
    }
    
    // 如果是数组类型，为每个数组元素生成赋值语句
    for(int i = 0; i < count && count > 1; i++)
    {
        // 如果设置了展开计数限制且当前索引超过限制，则停止生成
        if(i >= StateSearch::unwindCount && StateSearch::unwindCount != 0)
            break;
            
        // 生成指针解引用形式的赋值语句：*（变量名 + 偏移） = 值;
        retCode += "*(" + variableRecord->pointer->name + " + " + to_string(i) + ")" + " = ";
        retCode += getVariableValueStr(variableRecord, i);
        retCode += ";\n";
    }
    
    return retCode;
}


/**
 * 调用CBMC求解器进行程序验证
 * 该函数构造CBMC命令行参数并执行求解器，将结果输出到指定文件
 * 
 * @param branchId 目标分支ID，用于指定要验证的程序分支
 * @param solvingCodePath 待验证的C代码文件路径
 * @param resultPath 求解结果输出文件路径
 */
void StateSearch::callCBMC(int branchId, string solvingCodePath, string resultPath)
{
    // CBMC可执行文件名
    string callCBMC = "cbmc.exe";

    string cmd;
    // 构造CBMC命令行参数
    string para = "--gcc";                              // 使用GCC前端解析器
    para += " --arch i386";                             // 设置目标架构为i386
    para += " --cover cover";                           // 启用覆盖率分析
    //para += " --z3";                                  // 可选：使用Z3求解器（已注释）
    para += cbmcParameter;                              // 添加自定义CBMC参数
    para += " --xml-ui";                                // 输出XML格式结果
    para += " --partial-loops";                         // 启用部分循环展开
    para += " --reachability-slice";                    // 启用可达性切片优化
    para += " --slice-formula";                         // 启用公式切片优化
    para += " --property " + stateBranchList[branchId]; // 指定要验证的属性/分支
    
    // 如果设置了循环展开次数，添加相应参数
    if(StateSearch::unwindCount != 0)
        para += " --unwind " + to_string(StateSearch::unwindCount);
    
    para += " " + solvingCodePath;                      // 添加源代码文件路径
    
    // 构造完整的命令行，重定向输出到结果文件
    cmd = callCBMC + " " + para + " > " + resultPath;
    
    // 将执行的命令记录到日志文件
    appendFile("Temp/cmd.txt", cmd + "\n");

    // 执行CBMC命令
    system(cmd.c_str());
}

/**
 * 解析CBMC求解器的XML结果文件
 * 该函数负责解析CBMC生成的XML格式测试结果，提取输入变量和参数数据，
 * 并将解析结果存储到相应的状态节点中
 * 
 * @param stateTraceNode 状态跟踪节点，用于存储解析结果
 * @param branchId 目标分支ID，用于匹配特定的测试目标
 * @param resultFilePath XML结果文件的路径
 * @param formJumpSolvedFile 是否来自跳转求解文件的标志
 */
void StateSearch::parseCBMCResult(StateTraceNode* stateTraceNode, int branchId, string resultFilePath, bool formJumpSolvedFile)
{
    // 获取当前处理的状态节点
    StateTraceNode* currentNode = stateTraceNode;
    
    // 创建XML文档对象用于解析
    tinyxml2::XMLDocument doc;
    string resultFile = resultFilePath; //"Temp/out" + to_string(solveTimes) + ".txt";
    
    // 加载XML文件
	tinyxml2::XMLError ret = doc.LoadFile(resultFile.c_str());
    if (ret != 0) {
        // 文件加载失败时输出错误信息并返回
		fprintf(stderr, "Fail to load CBMC result file: %s\n", resultFile.c_str());
        return;
	}
    
    // 获取XML根元素
    tinyxml2::XMLElement* root = doc.RootElement();

    // 遍历所有的"test"元素，每个test元素包含一个测试用例
    const tinyxml2::XMLElement *c = root->FirstChildElement("test");
	while (c) {
        // 创建输入变量步骤对象来存储当前测试用例的数据
        InputVariableStep inputs;
        inputs.formJumpSolvedFile = formJumpSolvedFile;
        
        // 查找"inputs"子元素
        const tinyxml2::XMLElement *c2 = c->FirstChildElement("inputs");
	    if(c2) {
            // 获取第一个"input"子元素
	        const tinyxml2::XMLElement *c3 = c2->FirstChildElement("input");

            // 首先解析参数变量（如果存在）
            while (c3 && StateSearch::parametersDataLengthTotal > 0) {
                ParameterVariableRecord para;
                // 设置参数指针，指向对应的模型参数定义
                para.pointer = &StateSearch::modelParameterList[inputs.parameters.size()];
                // 解析参数值的二进制数据
                para.data = parseCBMCResultOneValue(c3);

                // 将参数记录添加到输入步骤中
                inputs.parameters.push_back(para);

                // 移动到下一个input元素
                c3 = c3->NextSiblingElement("input");

                // 如果已解析完所有参数，跳出循环
                if (inputs.parameters.size() == StateSearch::modelParameterList.size()) {
                    break;
                }
            }

            // 然后解析输入变量，按步骤组织
            InputVariableCuple inputCuple;
	        while (c3) {
                InputVariableRecord input;
                //string name = string(c3->Attribute("id")); // 可选：获取输入变量名称

                // 设置输入变量指针，指向对应的模型输入定义
                input.pointer = &StateSearch::modelInputList[inputCuple.allInputVariable.size()];
                // 解析输入值的二进制数据
                input.data = parseCBMCResultOneValue(c3);
                
                // 将输入记录添加到当前输入组合中
                inputCuple.allInputVariable.push_back(input);

                // 如果当前组合已包含所有输入变量，则完成一个步骤
                if(inputCuple.allInputVariable.size() == StateSearch::modelInputList.size()) {
                    inputs.stepInputs.push_back(inputCuple);
                    inputCuple.allInputVariable.clear(); // 清空以准备下一个步骤
                }
                

                // 移动到下一个input元素
	            c3 = c3->NextSiblingElement("input");
	        }
	    }

        // 如果成功解析到有效的输入步骤，将其添加到全局输入变量步骤列表
        if(!inputs.stepInputs.empty() && !inputs.stepInputs[0].allInputVariable.empty()) {
            stateTraceTree.allInputVariableStepList.push_back(inputs);
        }
        
        // 查找"goal"元素，用于匹配目标分支
        c2 = c->FirstChildElement("goal");
	    while(c2) {
            // 获取目标路径ID
	        string pathIdStr = string(c2->Attribute("id"));

            // 如果路径ID与目标分支匹配，将输入数据关联到该分支
            if(stateBranchList.at(branchId) == pathIdStr) {
                currentNode->solvedStateSelectInputList[branchId] = inputs;
            }
            
            // 移动到下一个goal元素
            c2 = c2->NextSiblingElement("goal");
	    }

        // 移动到下一个test元素
		c = c->NextSiblingElement("test");
	}
    
    // 如果成功为目标分支找到了求解结果，更新求解状态计数
    if(currentNode->solvedStateSelectInputList.find(branchId) != currentNode->solvedStateSelectInputList.end()) {
        if(currentNode->solvedBranchResultMap.find(branchId) == currentNode->solvedBranchResultMap.end()) {
            // 首次求解成功，设置计数为1
            currentNode->solvedBranchResultMap[branchId] = 1;
        } else {
            // 增加求解成功次数
            currentNode->solvedBranchResultMap[branchId]++;
        }
    }
}

/**
 * 解析CBMC结果中的单个值
 * 从XML元素中提取数值并转换为对应类型的二进制数据
 * 
 * @param input XML元素，包含要解析的数值信息
 * @return 返回指向包含解析数据的内存指针，调用者负责释放内存
 */
char* StateSearch::parseCBMCResultOneValue(const tinyxml2::XMLElement* input)
{
    char* retData = nullptr;
    
    // 尝试获取整数类型元素
    const tinyxml2::XMLElement* c4_1 = input->FirstChildElement("integer");
    // 尝试获取浮点类型元素
    const tinyxml2::XMLElement* c4_2 = input->FirstChildElement("float");
    
    if (c4_1) {
        // 处理整数类型
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
        // 处理浮点类型
        string type = "float";
        
        // 检查是否有c_type属性
        if (c4_2->FindAttribute("c_type")) {
            type = string(c4_2->Attribute("c_type"));
        }
        else {
            // 根据width属性确定类型
            string width = string(c4_2->Attribute("width"));
            type = width == "64" ? "double" : "float";
        }
        
        if (type == "float") {
            float value = c4_2->FloatText();
            // 处理NaN值，将其设为0
            if (isnan(value))
                value = 0;
            retData = new char[sizeof(value)];
            ::memcpy(retData, &value, sizeof(value));
        }
        else if (type == "double") {
            double value = c4_2->DoubleText();
            // 处理NaN值，将其设为0
            if (isnan(value))
                value = 0;
            retData = new char[sizeof(value)];
            ::memcpy(retData, &value, sizeof(value));
        }
    }
    
    return retData;
}

/**
 * 状态搜索主函数
 * 负责控制状态搜索的整个流程，包括状态选择、随机运行、分支处理等
 * 
 * @param iteratorVariable 迭代变量，用于跟踪当前执行次数
 * @param timeOut 超时标志，如果搜索超时则设置为true
 * @return 返回当前已覆盖的分支数量，超时或完成时返回0，错误时返回-1
 */
int StateSearch::stateSearch(int& iteratorVariable, bool &timeOut)
{
    // 获取当前状态跟踪节点
    StateTraceNode* currentNode = stateTraceTree.currentNode;
    
    // 增加搜索计数器
    searchedCount++;

    // 选择要处理的状态，检查是否超时
    selectState(timeOut);
    if(timeOut)
        return 0;

    // 如果禁用随机运行且当前节点为空，则标记搜索完成
    if(StateSearch::paraNoRandom && !stateTraceTree.currentNode)
    {
        StateSearch::finish = 1;
        return 0;
    }

    // 通过测试判断输入变量列表为空时的处理
    // stateTraceTree.allInputVariableStepList为空的情况
    //if(stateTraceTree.allInputVariableStepList.empty())
    //    return -1;

    // 如果当前节点为空，执行随机运行直到找到可用节点
    if(!stateTraceTree.currentNode)
    {
        while(!stateTraceTree.currentNode)
        {
            // 执行500步随机运行
            randomRun(500);

            // 如果找到了当前节点且未禁用随机运行且需要输出长序列
            if (stateTraceTree.currentNode && 
                !StateSearch::paraNoRandom && 
                paraOutputSomeLongSequence > 0)
            {
                outputLongSequence();
            }
        }
        /* Rescan all nodes after random run: otherwise lastTestTreeNodeId==nodeCount makes the next
           selectState inner loop j in [nodeCount,size) empty, so only the single new leaf is tried. */
        //lastTestTreeNodeId = 0;

        // 更新已达成的分支覆盖数量
        achievedCount = BitmapCov::branchCov;

        // 输出新的覆盖测试用例
        outputNewCoverageTestCase();
    
        // 打印当前状态搜索状态
        printTCGSSState(iteratorVariable);

        return BitmapCov::branchCov;
    }
  
    // 重新获取当前节点（防止在随机运行中发生变化）
    currentNode = stateTraceTree.currentNode;
    if(!currentNode)
    {
        return -1;
    }

    // 检查是否已经有子节点尝试过相同的分支，如果有则直接切换到该子节点
    int hadChildNode = -1;
    for(int i = 0; i < currentNode->children.size(); i++) {
        StateTraceNode* child = currentNode->children[i];
        if(child->exceptBranchId == stateTraceTree.selectedBranchId) {
            hadChildNode = i;
            break;
        }
    }
    if(hadChildNode != -1) {
        // 切换到已存在的子节点
        stateTraceTree.currentNode = currentNode->children[hadChildNode];
        return achievedCount;
    }
    
    // 设置当前节点的运行模式为正常模式
    currentNode->runMode = StateTraceNode::RunMode::Normal;
    
    // 尝试执行多步状态分支处理
    bool tryRes = tryStateBranchInnerMultiStep(currentNode, stateTraceTree.selectedBranchId, &currentNode->solvedStateSelectInputList[stateTraceTree.selectedBranchId]);
    
    // 更新已达成的分支覆盖数量
    achievedCount = BitmapCov::branchCov;

    // 输出新的覆盖测试用例
    outputNewCoverageTestCase();
    
    // 打印当前状态搜索状态
    printTCGSSState(iteratorVariable);

    return BitmapCov::branchCov;
}

/**
 * 收集全局变量步骤信息到状态跟踪节点
 * 此函数负责：
 * 1. 初始化哈希缓冲区（如果尚未初始化）
 * 2. 复制所有全局变量的当前状态
 * 3. 计算状态哈希值用于状态去重
 * 4. 标记是否达到新的状态哈希
 * 
 * @param stateTraceNode 状态跟踪节点，用于存储收集的全局变量信息
 */
void StateSearch::collectGlobalVariableStep(StateTraceNode* stateTraceNode)
{
    // 初始化哈希缓冲区（仅在第一次调用时）
    if(!hashBuffer)
    {
        // 分配完整全局变量数据长度的哈希缓冲区
        hashBuffer = new uint8_t[globalVariableDataLengthTotal];
        
        // 如果设置了展开计数，则分配用于裁剪数组的额外缓冲区
        if(StateSearch::unwindCount != 0)
        {
            hashBufferCutArray = new uint8_t[globalVariableDataLengthTotal];
            memset((void*)hashBufferCutArray, 0, globalVariableDataLengthTotal);
        }
    }

    // 用于跟踪在缓冲区中的偏移位置
    int addressOffset = 0;
    int addressOffsetCutArray = 0;
    
    // 创建新的全局变量步骤对象
    GlobalVariableStep* globalVariablesStepTemp = new GlobalVariableStep();
    
    // 遍历所有全局变量并收集其当前状态
    for(int i = 0; i < globalVariableList.size(); i++)
    {
        GlobalVariableData* ref = &globalVariableList[i];
        GlobalVariableRecord targetRecord;
        
        // 为目标记录分配内存并复制全局变量数据
        targetRecord.data = new char[ref->length];
        memcpy(targetRecord.data, ref->address, ref->length);

        // 将全局变量数据复制到完整哈希缓冲区
        memcpy(hashBuffer + addressOffset, ref->address, ref->length);
        addressOffset += ref->length;

        // 如果启用了展开计数，处理裁剪数组的哈希
        if(StateSearch::unwindCount != 0)
        {
            int typeSize = typeSizeMap[ref->type];
            int copyCount = StateSearch::unwindCount * typeSize;
            
            // 确保不超过实际变量长度
            if(copyCount > ref->length)
                copyCount = ref->length;
                
            memcpy(hashBufferCutArray + addressOffsetCutArray, ref->address, copyCount);
            addressOffsetCutArray += copyCount;
        }
        
        // 注释掉的代码：直接赋值方式（现在使用指针引用）
        //targetRecord.length = ref->length;
        //targetRecord.type = ref->type;
        //targetRecord.name = ref->name;
        //targetRecord.arraySize = ref->arraySize;
        
        // 设置指向原始全局变量数据的指针
        targetRecord.pointer = ref;

        // 将记录添加到全局变量步骤中
        globalVariablesStepTemp->allGlobalVariable.push_back(targetRecord);
    }
    
    // 将收集的全局变量步骤赋值给状态跟踪节点
    stateTraceNode->globalVariablesStep = globalVariablesStepTemp;

    // 计算完整数据的64位CRC哈希值
    uint64_t hash = crc64(hashBuffer, globalVariableDataLengthTotal);
    stateTraceNode->dataHash = hash;

    // 计算裁剪数组的哈希值（如果启用了展开计数）
    if(StateSearch::unwindCount != 0) {
        uint64_t hash = crc64(hashBufferCutArray, globalVariableDataLengthTotal);
        stateTraceNode->dataHashCutArray = hash;
    } else {
        // 如果未启用展开计数，裁剪数组哈希与完整哈希相同
        stateTraceNode->dataHashCutArray = stateTraceNode->dataHash;
    }

    // 检查是否达到了新的完整状态哈希
    if (StateSearch::allDataHashMap.find(stateTraceNode->dataHash) == StateSearch::allDataHashMap.end())
    {
        stateTraceNode->achieveNewStateHash = true;
        StateSearch::allDataHashMap[stateTraceNode->dataHash] = true;
    }
    
    // 检查是否达到了新的裁剪数组状态哈希
    if (StateSearch::allDataHashCutArrayMap.find(stateTraceNode->dataHashCutArray) == StateSearch::allDataHashCutArrayMap.end())
    {
        stateTraceNode->achieveNewStateHashCutArray = true;
        StateSearch::allDataHashCutArrayMap[stateTraceNode->dataHashCutArray] = true;
    }

    // 注释掉的调试代码：用于打印哈希值的十六进制表示
    //std::stringstream ss;
    //ss << std::hex << std::uppercase << std::setfill('0') << std::setw(16) << hash;
    //std::string hashStr = ss.str();
    //
    //std::cout << "\nHash: " << hashStr << "\n";
}

/**
 * 收集输入变量步骤信息到状态跟踪节点
 * @param stateTraceNode 状态跟踪节点，用于存储输入变量步骤信息
 * @param parameters 输入变量步骤参数，包含要复制的输入变量数据
 */
void StateSearch::collectInputVariableStep(StateTraceNode* stateTraceNode, InputVariableStep* parameters)
{
    // 创建新的输入变量步骤对象
    InputVariableStep* inputVariablesStepTemp = new InputVariableStep();
    
    // 调整步骤输入容器大小以匹配参数中的步骤数量
    inputVariablesStepTemp->stepInputs.resize(parameters->stepInputs.size());
    
    // 遍历所有输入步骤
    for(int i = 0; i < parameters->stepInputs.size(); i++)
    {
        // 获取源输入变量组合和目标输入变量组合的引用
        InputVariableCuple* refCuple = &parameters->stepInputs[i];
        InputVariableCuple* targetCuple = &inputVariablesStepTemp->stepInputs[i];

        // 调整目标组合中输入变量容器的大小
        targetCuple->allInputVariable.resize(refCuple->allInputVariable.size());
        
        // 遍历组合中的所有输入变量
        for (int j = 0; j < refCuple->allInputVariable.size(); j++)
        {
            // 获取源记录和目标记录的引用
            InputVariableRecord* refRecord = &refCuple->allInputVariable[j];
            InputVariableRecord* targetRecord = &targetCuple->allInputVariable[j];

            // 复制指针引用（指向变量元数据）
            targetRecord->pointer = refRecord->pointer;
            
            // 为目标记录分配新的数据内存空间
            targetRecord->data = new char[refRecord->pointer->length];
            
            // 将源记录的数据复制到目标记录
            memcpy(targetRecord->data, refRecord->data, refRecord->pointer->length);
        }
    }
    
    // 将创建的输入变量步骤赋值给状态跟踪节点
    stateTraceNode->inputVariablesStep = inputVariablesStepTemp;
    
    // 复制跳转解决文件标志
    stateTraceNode->inputVariablesStep->formJumpSolvedFile = parameters->formJumpSolvedFile;
}

//根据目标分支与一步迭代的数据更新状态树节点
/**
 * 向状态跟踪树添加新的状态节点
 * @param stateTraceNode 父状态节点
 * @param expectStateBranchId 期望的状态分支ID
 * @param inputVariableStep 输入变量步骤数据
 */
void StateSearch::addStateTraceNode(StateTraceNode* stateTraceNode, int expectStateBranchId, InputVariableStep* inputVariableStep)
{
    // 创建新的状态跟踪节点
    StateTraceNode* nodeTemp = new StateTraceNode();
    nodeTemp->id = stateTraceTree.nodeCount;
    
    // 将新节点添加为当前节点的子节点
    stateTraceTree.currentNode->children.push_back(nodeTemp);
    
    // 更新父节点的分支子节点搜索计数
    if(stateTraceTree.currentNode->branchChildSearchedCount.find(expectStateBranchId) == stateTraceTree.currentNode->branchChildSearchedCount.end())
        stateTraceTree.currentNode->branchChildSearchedCount[expectStateBranchId] = 0;
    stateTraceTree.currentNode->branchChildSearchedCount[expectStateBranchId]++;

    // 设置新节点的基本属性
    nodeTemp->parent = stateTraceTree.currentNode;
    nodeTemp->depth = nodeTemp->parent->depth + inputVariableStep->stepInputs.size();
    nodeTemp->exceptBranchId = expectStateBranchId;
    
    // 为新节点分配分支覆盖信息的内存空间
    nodeTemp->branchIdCovered = new unsigned char[BitmapCov::branchBitmapLength];
    nodeTemp->branchIdCoveredTotal = new unsigned char[BitmapCov::branchBitmapLength];

    // 收集并设置新节点的输入变量步骤信息
    collectInputVariableStep(nodeTemp, inputVariableStep);

    // 更新状态跟踪树的当前节点和统计信息
    stateTraceTree.currentNode = nodeTemp;
    stateTraceTree.nodeCount++;
    stateTraceTree.allNodeList.push_back(nodeTemp);

    // 注释：未解决节点列表的添加操作已被注释掉
    //stateTraceTree.unsolvedNodeList.push_back(nodeTemp);
}

/*
bool StateSearch::tryStateBranchInner(StateTraceNode* stateTraceNode, int expectStateBranchId, InputVariableStep* replaceInput)
{
    // ��¼֮ǰ��������Щ��֧
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

    // ��¼Step��������
    
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


    // ��¼״̬�����ڵ�
    if(stateTraceTree.currentNode->achieveExceptBranch)
        return true;
    
    return false;
}
*/

/**
 * 重置全局变量到指定状态节点的状态
 * @param stateTraceNode 状态跟踪节点，包含要恢复的全局变量状态
 */
void StateSearch::resetGlobalVariables(StateTraceNode* stateTraceNode)
{
    // 获取该状态节点的全局变量步骤信息
    GlobalVariableStep* globalVariables = stateTraceNode->globalVariablesStep;
    
    // 遍历所有全局变量，将其恢复到节点记录的状态
    for(int i = 0; i < globalVariables->allGlobalVariable.size(); i++)
    {
        GlobalVariableRecord* record = &(globalVariables->allGlobalVariable[i]);
        // 将保存的数据复制回全局变量的实际地址
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
/**
 * 输出节点测试用例
 * @param node 状态跟踪节点
 * @param outputCSV 是否输出CSV格式
 * @param outputBin 是否输出二进制格式
 * @param outputDirCSV CSV输出目录
 * @param outputDirBin 二进制输出目录
 */
void StateSearch::outputNodeTestCase(StateTraceNode* node, bool outputCSV, bool outputBin, string outputDirCSV, string outputDirBin)
{
    // 如果两种格式都不输出，直接返回
    if(!(outputCSV || outputBin))
        return;

    TestCase testCase;
    // testCase.length = node->depth; // 旧的长度记录方式
    int testCaseLen = 0;
    StateTraceNode* p = node;
    StateTraceNode* parameterNode = nullptr;
    
    // 从当前节点向上遍历到根节点，收集输入变量步骤
    while (p && p != stateTraceTree.root) {
        // 查找包含参数的节点
        if (!parameterNode && p->inputVariablesStep && !p->inputVariablesStep->parameters.empty())
        {
            parameterNode = p;
        }
        // 收集输入步骤
        if (p->inputVariablesStep && !p->inputVariablesStep->stepInputs.empty())
        {
            testCase.inputList.insert(testCase.inputList.begin(), p->inputVariablesStep);
            testCaseLen += p->inputVariablesStep->stepInputs.size();
        }
        p = p->parent;
    }
    
    // 如果测试用例长度为1，复制一份以确保至少有两个步骤
    if (testCaseLen == 1)
    {
        testCase.inputList.push_back(testCase.inputList[0]);
    }
    
    // 设置参数：如果找到了参数节点，使用该节点的参数；否则使用模型的默认参数
    if (parameterNode)
    {
        testCase.parameters = parameterNode->inputVariablesStep->parameters;
    }
    else
    {
        // 从模型参数列表创建参数记录
        for (int i = 0; i < StateSearch::modelParameterList.size(); i++)
        {
            ParameterVariableRecord para;
            para.pointer = &StateSearch::modelParameterList[i];
            para.data = new char[para.pointer->length];
            memcpy(para.data, para.pointer->address, para.pointer->length);
            testCase.parameters.push_back(para);
        }
    }

    // 将测试用例添加到向量中
    vector<TestCase> tss;
    tss.push_back(testCase);

    // 根据输出格式标志输出测试用例
    if(outputCSV)
        outputTestCasesCSV(tss, outputDirCSV);
    // outputTestCasesXLS(tss); // XLS格式输出已注释

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
    
    //std::cout << logStr;
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
        Sleep(3000);
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

// ����ÿ500������һ���ݶȣ����ÿ���ݶ��е�N������������NΪparaOutputSomeLongSequence
// ÿ��randomRun֮����ã�һ��currentNode�ĳ��ȴ����µ��ݶȣ������
namespace {
    int lastOutputLongSequenceLength = 500; // ֻ��¼500, 1000, 1500�������ݶ�
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
            continue; //�����¸��ǵķ�֧���ж���һ��
        }

        int branchId = i;
        // 检查当前分支是否有子分支
        if(StateSearchBasicBranchRegister::StateSearch_Branch_Children.find(branchId) == StateSearchBasicBranchRegister::StateSearch_Branch_Children.end())
        {
            continue; // 如果该分支没有子分支，跳过当前循环
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
    // ��������µķ�֧���ǣ��ͳ���������������֧�����ȼ������ڶ����صķ�֧��Ҳ���շ�֧�����ǳ������

    set<string> globalWriteList;
    for(int i = BitmapCov::branchBitmapLength - 1; i >=0; i--)
    {
        int branchId = StateSearch::branchSolveOrder[i];
        if(!(BitmapCov::branchAllBitmap[branchId] && !lastBranchCovered[branchId]))
        {
            continue; //�����¸��ǵķ�֧���ж���һ��
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

        // distanceRecordValueV1��ֵ��distanceRecordValueV2��һ����ֵ
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

            //std::cout << logStr;
            
            appendFile(exePath + "/log.txt", logStr);
        }
        else if (stepDistance == distanceData->maxStepDistance && distanceData->maxStepInput.size() < 5)
        {
            distanceData->maxStepInput.push_back(stepInput->clone());
        }
    }

    // ÿ��Step�����������µ�ǰ������������ֵ
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

    // 确保distanceRecordValueV1能存储当前节点父节点的距离变量值
    if (!paEmpty && v1Empty) // ���ڵ�����Ǹ�������ǰ�ڵ㲻�����Ǹ������ͼ̳и��ڵ�����
    {
        v1->emplace(varName, pa->distanceRecordValueV2[varName]);
    }
    else if (v1Empty) // ���ڵ㲻�����Ǹ�������ǰ�ڵ㲻�����Ǹ�������ֱ�Ӹ�ֵ
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

            //std::cout << logStr;

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
