#pragma once

#include <map>
#include <string>
#include <vector>
#include <time.h>

#include "tinyxml2.h"

using namespace std;

class StateSearch
{
public:
    struct StateTraceNode;

    // 用于记录程序中的全部全局变量
    struct GlobalVariableData
    {
        char* address = nullptr;
        int length;
        string type;
        string name;
        vector<int> arraySize;
    };

    // 用来记录程序运行过程中的一个全局变量数据
    struct GlobalVariableRecord
    {
        char* data = nullptr;
        //int length;
        //string type;
        //string name;
        //vector<int> arraySize;
        GlobalVariableData* pointer = nullptr;
    };

    // 用来记录程序一次Step执行后的全部全局变量数据
    struct GlobalVariableStep
    {
        vector<GlobalVariableRecord> allGlobalVariable;
    };

    struct ModelParameterVariableData
    {
        char* address = nullptr;
        int length;
        string type;
        string name;
    };

    struct ParameterVariableRecord
    {
        char* data = nullptr;
        //int length;
        //string type;
        //string name;

        ModelParameterVariableData* pointer = nullptr;

        string toString();
    };


    struct ModelInputVariableData
    {
        int length;
        string type;
        string name;
    };

    // 用来记录程序运行过程中的一个输入参数数据
    struct InputVariableRecord
    {
        char* data = nullptr;
        //int length;
        //string type;
        //string name;

        ModelInputVariableData* pointer = nullptr;

        string toString();
    };

    struct InputVariableCuple
    {
        vector<InputVariableRecord> allInputVariable;

    public:
        InputVariableCuple* clone();
        void release();
    };

    // 用来记录程序一次Step执行后的全部输入参数数据
    struct InputVariableStep
    {
        vector<InputVariableCuple> stepInputs;

        vector<ParameterVariableRecord> parameters;
        
        bool formJumpSolvedFile = false;

    };
    
    struct StateTraceNode
    {
        int id = -1;
        StateTraceNode* parent = nullptr;
        vector<StateTraceNode*> children;
        map<int, int> branchChildSearchedCount;

        // 每个状态搜索节点都存储全部输入和全局变量，真实存储在这里
        InputVariableStep* inputVariablesStep = nullptr;
        GlobalVariableStep* globalVariablesStep = nullptr;

        uint64_t dataHash = 0;
        uint64_t dataHashCutArray = 0;

        int exceptBranchId = -1;
        bool achieveExceptBranch = false;
        bool achieveNewBranch = false;
        bool achieveNewStateHash = false;
        bool achieveNewStateHashCutArray = false;

        unsigned char* branchIdCovered = nullptr;
        unsigned char* branchIdCoveredTotal = nullptr;

        vector<int> solveId;

        // 用于记录当前用于状态选择的输入，记录所有状态Branch的输入
        map<int, InputVariableStep> solvedStateSelectInputList;
        map<int, int> solvedBranchResultMap; //记录当前节点求解不同分支的结果

        bool isExistTestCaseNode = false;
        //bool isIntermediateNode = false;

        int depth = -1;


        map<string, double> distanceRecordValueV1; // 用于记录当前Step执行前的距离数据相关的值
        map<string, double> distanceRecordValueV2; // 用于记录当前Step执行后的距离数据相关的值

        map<string, int> distanceRecordV1Array; // 用于记录当前Step执行前的数组距离数据
        map<string, int> distanceRecordV2Array; // 用于记录当前Step执行后的数组距离数据

        enum RunMode {
            Normal,
            SolvedInputRandom,
            DistanceGuided,
            ExistTestCase,
        };

        RunMode runMode = Normal;
        //map<int, double> branchDistance; // key: distanceId  value: distance
    };

    struct ArrayDistanceData
    {
        string varName;
        unsigned char* arrayValue = nullptr; //保存数组地址

        unsigned char* defaultArrayValue = nullptr; //保存具体数据
        int arrayDataLength = 0;

        int maxStepDistance = 0;
        vector<InputVariableCuple*> maxStepInput;


        ArrayDistanceData() {};
        ArrayDistanceData(string varName, unsigned char* arrayValue, int arrayDataLength)
        {
            this->varName = varName;
            this->arrayValue = arrayValue;
            this->arrayDataLength = arrayDataLength;
            this->defaultArrayValue = new unsigned char[arrayDataLength];
            memcpy(this->defaultArrayValue, arrayValue, arrayDataLength);
        }
    };
    static map<int, ArrayDistanceData> arrayDistanceDataMap; // key: distanceId  value: ArrayDistanceData


    struct BranchDistanceData
    {
        int branchId = 0; // 这个距离对应的branchId
        string varName;
        char distanceOpType = 0; // 只区分'>'和'<'
        double targetValue = 0; // 数值距离时，目标值； 数组距离时，目标数组长度

        double maxStepDistance = 0;
        //StateSearch::StateTraceNode* maxStepStateNode = nullptr;

        vector<InputVariableCuple*> maxStepInput;

        //char* address = nullptr;
        //int length;
        //string type;
        //string name;

        BranchDistanceData() {};
        BranchDistanceData(int branchId, string varName, char distanceOpType, double targetValue)
        {
            this->branchId = branchId;
            this->varName = varName;
            this->distanceOpType = distanceOpType;
            this->targetValue = targetValue;
        }
    };

    // 距离数据收集流程：
    // 1. 代码中执行距离函数，将距离数据收集到StateSearch::branchDistance中
    // 2. 在收集每次动态执行的覆盖率的同时，收集每个StateTraceNode的距离数据，收集到各Node的map<int, double> branchDistance中
    // 3. 统计子节点的距离数据，将最大的距离数据保存在map<int, BranchDistanceData> branchDistanceDataMap中，用于后续随机执行的输入选择
    // 4. 随机执行时，当带有距离函数的分支未被覆盖时，从branchDistanceDataMap中选择最大的距离数据，生成新的长的输入序列。这期间不应继续分析距离数据
    // 
    // 用于收集对应分支上使得距离step最大的一步/多步输入数据，也就是状态节点上的输入数据
    // 一个distance要求只对应一个branch，一个branch可以对应多个distance
    static map<int, BranchDistanceData> branchDistanceDataMap; // key: distanceId  value: BranchDistanceData
    //static map<int, int> branchDistanceIdToBranchId; // key: distanceId  value: branchId


    struct StateTraceTree
    {
        StateTraceNode* root = nullptr;
        StateTraceNode* currentNode = nullptr;
        int selectedBranchId = -1;
        int nodeCount = 0;
        
        vector<StateTraceNode*> allNodeList;
        vector<InputVariableStep> allInputVariableStepList;
    };


    struct TestCase
    {
        //vector<float> timeList;
        vector<InputVariableStep*> inputList;
        //int length = 0;

        vector<ParameterVariableRecord> parameters;
    };

    static void initStateSearch(int argc, char *argv[]);
    static void initStateSearch2();
    static void terminateStateSearch();

    static void runExistTestCaseCSV(string dirPath);
    static void runExistTestCaseBinary(string dirPath);
    static void runExistTestCaseBinaryOne(char* data, int size, int testCaseIndex);
    
    static void registGlobalVariables(string name, string type, vector<int>& arraySize, char* address, int length);
    static void registModelInputs(string name, string type, int length);
    static void registModelParameters(string name, string type, char* address, int length);

    // 注册状态Branch
    static void registStateBranch();

    // 注册状态Branch的关系
    static void registStateBranchRelation(vector<pair<int, string>>& branchReadGlobalVar, vector<pair<int, string>>& branchWriteGlobalVar);

    // 状态搜索时的状态选择函数，返回一个BranchId
    static void selectState(bool &timeOut);

    static void randomRun(int nTimes);
    
    static bool tryStateBranchInnerMultiStep(StateTraceNode* baseStateNode, int expectStateBranchId, InputVariableStep* inputVariableStep);
    // 在状态树的当前节点上执行一步，不会改变当前节点(之后可能还要在该节点上执行多步)
    static bool tryStateBranchInnerOneStep(InputVariableCuple* inputVariableCuple);

    //static bool tryStateBranchInner(StateTraceNode* stateTraceNode, int expectStateBranchId, InputVariableStep* replaceInput = nullptr);
    //static bool tryStateBranchInner2(StateTraceNode* stateTraceNode); 

    static void addPriorityQueueToSolveForDataWrite(StateTraceNode* stateTraceNode, unsigned char* lastBranchCovered);
    static void addPriorityQueueToSolveForChildrenBranch(StateTraceNode* stateTraceNode, unsigned char* lastBranchCovered);

    // 用某次的记录恢复全局变量
    static void resetGlobalVariables(StateTraceNode* stateTraceNode);


    // 根据 当前的全局变量 求解
    static void solveInputVariable(StateTraceNode* stateTraceNode, int branchId);
    static void solveInputVariableMultiStep(StateTraceNode* stateTraceNode, int branchId, int step);

    // 生成带有新全局变量的代码文件
    static string generateCodeWithNewGlobalVariable(StateTraceNode* stateTraceNode, int multiStep); // 返回代码文件路径
    static string getVariableStr(GlobalVariableRecord* variableRecord);
    static string getVariableValueStr(GlobalVariableRecord* variableRecord, int offset);
    static string getVariableValueStr(InputVariableRecord* variableRecord);
    static string getVariableValueStr(ParameterVariableRecord* variableRecord);
    static string getVariableValueStr(string type, char* addr, int offset);
    static void getVariableValueBinary(string textData, string type, char* addr, int offset);

    // 调用CBMC求解
    static void callCBMC(int branchId, string solvingCodePath, string resultPath);

    // 解析CBMC输出文件获得输入参数
    static void parseCBMCResult(StateTraceNode* stateTraceNode, int branchId, string resultFilePath, bool formJumpSolvedFile);
    static char* parseCBMCResultOneValue(const tinyxml2::XMLElement* input);

    static int stateSearch(int& iteratorVariable, bool &timeOut);

    // 根据约束求解的结果或者随机选择的输入数据设置一个节点的输入数据(进行动态执行)
    static void collectInputVariableStep(StateTraceNode* stateTraceNode, InputVariableStep* parameters);
    static void collectGlobalVariableStep(StateTraceNode* stateTraceNode);
    // 会改变状态树的当前节点
    static void addStateTraceNode(StateTraceNode* stateTraceNode, int expectStateBranchId, InputVariableStep* inputVariableStep);
    

    static void printStateTraceTree(StateTraceNode* node, string& nodeCode, string& linkCode);
    static void printStateTraceTree(string outputPath);

    static void outputNodeTestCase(StateTraceNode* node, bool outputCSV, bool outputBin, string outputDirCSV, string outputDirBin);
    static void outputTestCasesCSV(vector<TestCase>& testCases, string outputDir);
    //static void outputTestCasesXLS(vector<TestCase>& testCases);
    static void outputTestCasesBinary(vector<TestCase>& testCases, string outputDir);
    //static vector<TestCase> collectTestCases();

    static void printTCGSSState(int iteratorVariable);

    static void outputNewCoverageTestCase();

    static bool isGlobalVariableChanged(StateTraceNode* stateTraceNode, string varName);

    static void handlePause();
    //static void calculateStateNodeHash(StateTraceNode* node);

    //static char* generatedRandomParameterData(StateSearch::ModelParameterVariableData* para);

    static void outputLongSequence();

    // 数值型距离函数，只计算数值和目标的差值
    static double distanceFunction_value(double value, char opType, double target);

    // 忽略这些注释 // 数组型距离函数，计算数组中剩余的不是defaltValue的元素个数
    // 忽略这些注释 // 其中length指的是char的长度
    // 忽略这些注释 // unitLen是bit的长度，而不是字节的长度，因为考虑到一些按位运算的情况
    // 忽略这些注释 // 目前只考虑unitLen是8的倍数 和 untitLen是1的情况
    static int distanceFunction_array(unsigned char* addr, int length, unsigned char* defaultValue);

    static void updateBranchDistanceValue(const char* varName, double var);

    static void updateBranchDistanceData(StateTraceNode* stateTraceNode, InputVariableCuple* stepInput);

    static void updateArrayDistanceValue_pre();
    static void updateArrayDistanceValue_post();

    static void updateArrayDistanceData(StateTraceNode* stateTraceNode, InputVariableCuple* stepInput);

    static InputVariableCuple* generateOneDistanceDataInput(vector<InputVariableCuple*>* maxStepInput);
    static InputVariableCuple* generateRandomInputCuple();

    //template <typename T>
    //T recordDistanceValue(T var, string& varName)
    //{
    //    if (distanceRecordValueOriMap.find(varName) == distanceRecordValueOriMap.end())
    //    {
    //        distanceRecordValueOriMap[varName] = var;
    //    }
    //    return var;
    //}
    static string exePath;

    static clock_t testStartTime;

    static string paraBinaryTestCaseFilePath;
    static string paraCSVTestCaseFilePath;
	static int paraMaxRunTime;
	static int paraMaxRunIterator;
    static bool paraGenerateTestCaseOnce;
    static bool paraNoRandom;
    static bool paraHeuristics;
    static bool paraCanBePaused;
    static string paraOutputDir;
    static string paraOutputBinaryDir;
    static bool paraSimpleLog;
    static int paraSolveMultiple;
    static int paraOutputSomeLongSequence;
    static string paraOutputLongDir;
    static string paraOutputLongBinaryDir;
    static int paraOutputLongSequenceMaxLen;

    static int paraDistanceGuided;

    static int finish;
    
    // 尝试一个分支，判断其是否可以走期望分支
    static int solveTimes;
    static StateTraceTree stateTraceTree;
    static int achievedCount;
    static int searchedCount;
    static map<int, string> stateBranchList;
    static vector<int> branchSolveOrder;
    static vector<pair<int, string>> branchReadGlobalVar;
    static vector<pair<int, string>> branchWriteGlobalVar;
    static vector<GlobalVariableData> globalVariableList;
    static int globalVariableDataLengthTotal;
    static vector<ModelInputVariableData> modelInputList;
    static int inputsDataLengthTotal;
    static vector<ModelParameterVariableData> modelParameterList;
    static int parametersDataLengthTotal;
    static int unwindCount;

    static vector<pair<StateTraceNode*, int>> priorityQueueToSolve; // First: 状态节点     Second: 分支Id
    static vector<int> priorityQueueToSolveForRoot;

    static string cbmcParameter;
    static long long startTime;

    static uint8_t* hashBuffer;
    static uint8_t* hashBufferCutArray;

    static map<uint64_t, bool> allDataHashMap;
    static map<uint64_t, bool> allDataHashCutArrayMap;

    static map<int64_t, vector<int>> stateCanNotSolveMap; // First: 状态节点的(数据)Hash值     Second: 不能求解的分支Id列表

    static map<string, int> typeSizeMap; // First: 类型名     Second: 类型大小

    //static map<int, double> branchDistance; // key: distanceId  value: distance
};


bool tryStateBranch(char* data, int size); // 在程序主体中实现（自动生成）

