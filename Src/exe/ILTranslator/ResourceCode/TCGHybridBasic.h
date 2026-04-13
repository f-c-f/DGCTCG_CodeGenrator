#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include <algorithm>
#include <map>

#include "BitmapCov.h"

using namespace std;

class TCGHybrid
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
        char* dataOri = nullptr;
    };

    struct ParameterVariableData
    {
        int length;
        string type;
        string name;
    };

    struct InputVariableData
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

        InputVariableData* pointer = nullptr;

    };
  struct InputVariableCuple
    {
        vector<InputVariableRecord> allInputVariable;

    public:
        InputVariableCuple* clone();
        void release();
    };
        struct BranchDistanceData
    {
        int branchId = 0; // ???????????branchId
        string varName;
        char distanceOpType = 0; // ?????'>'??'<'
        double targetValue = 0; // ???????????????? ????????????????�A??
        
        // 记录到目前为止的最大步数距离
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

        ParameterVariableData* pointer = nullptr;

        string toString() const;
    };
    struct InputVariableStep
     {
    vector<InputVariableCuple> stepInputs;
    vector<ParameterVariableRecord> parameters;
    bool formJumpSolvedFile = false;
     };
    struct TestCase
    {
        //vector<float> timeList;
        vector<InputVariableStep*> inputList;
        //int length = 0;

        vector<ParameterVariableRecord> parameters;
    };
    static InputVariableCuple* generateOneDistanceDataInput(vector<InputVariableCuple*>* maxStepInput); 
    static void randomRun(int nTimes);
            //string代表距离变量名，double代表距离值
    static map<string, double> distanceRecordValueV1; // 记录当前Step各距离值（原始值）
    static map<string, double> distanceRecordValueV2; // 记录当前Step各距离值（处理后值）

    static map<string, int> distanceRecordV1Array; // 记录当前Step各数组距离值（原始值）
    static map<string, int> distanceRecordV2Array; // 记录当前Step各数组距离值（处理后值）
    //输入修改为一个InputVariableCuple指针
    static void updateBranchDistanceData( InputVariableCuple* stepInput);
    static double distanceFunction_value(double value, char opType, double target);
    static void updateArrayDistanceValue_pre();
    static void updateArrayDistanceValue_post();
    static void updateArrayDistanceData( InputVariableCuple* stepInput);
    static void updateBranchDistanceValue(const char* varName, double var);
    static int distanceFunction_array(unsigned char* addr, int length, unsigned char* defaultValue);
    static void outputTestCasesBinary(vector<TestCase>& testCases, string outputDirBin);
    static map<int, BranchDistanceData> branchDistanceDataMap; // key: distanceId  value: BranchDistanceData
    struct ArrayDistanceData
    {
        string varName;
        unsigned char* arrayValue = nullptr; //??????????

        unsigned char* defaultArrayValue = nullptr; //???????????
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

    // 用来记录程序一次Step执行后的全部输入参数数据
    struct InputVariableOneStep
    {
        vector<InputVariableRecord> allInputVariable;
    };
    


    struct TestResult
    {
        unsigned char* branchCoverage = nullptr;
        int maxCoverageDepth = 0;
    };

    struct TestCaseBinary
    {
        string filePath;
        char* data = nullptr;
        int length = 0;
        string fileName;
        uint64_t hash;

        TestResult* testResult = nullptr;
    };

    

    struct CoverageInfo
    {
        int munitCov;
        int branchCov;
        int condCov;
        int decCov;
        int mcdcCov;
    };

    static void initTCGHybrid(int argc, char *argv[]);
    static void initTCGHybrid2();
    static bool stepTCGHybrid();

    static void terminateTCGHybrid();

    static void registGlobalVariables(string name, string type, vector<int>& arraySize, char* address, int length);
    static void registInputs(string name, string type, int length);
    static void registParameters(string name, string type, int length);

    
    static bool minimizeTestCasesBinary(string inputTestCasesDir, vector<TestCaseBinary>& testCasesBinary);
    static bool minimizeTestCasesBinary(string inputTestCasesDir, vector<TestCaseBinary>& testCasesBinary,bool enableDistanceGuided);
    static bool tryStateBranchInner2(StateTraceNode* stateTraceNode);
    
    static bool callFuzzer();
    static bool callHSTCG();
    static bool callSTCGForSolve();
    static bool callParaFuzzer();
    static int copyMaxFileToInputSquenceDir(string srcDir, int fileCount);

    // 重置某次记录的全局变量
    static void resetGlobalVariables();

    static string exePath;

    static void* fuzzerProcessHandle;
    static void* hstcgProcessHandle;
    static void* paraFuzzerProcessHandle;

    static string paraBinaryTestCaseFilePath;
	static int paraMaxRunTime;
    static bool paraEnableFuzzer;
    static bool paraEnableHSTCG;
    static bool paraEnableHSTCGDistanceGuided;
    static bool paraEnableSTCG;
    static bool paraEnableParaFuzzer;
    static int paraSleepTime;

    static bool finish;
    
    static vector<GlobalVariableData> globalVariableList;
    static vector<InputVariableData> inputList;
    static int inputsDataLengthTotal;
    static vector<ParameterVariableData> modelParameterList;
    static int parametersDataLengthTotal;

    static unordered_map<int64_t, bool> fileHashReadList;

    static map<int64_t, BitmapCov::BitmapCovRecord*> testCaseBinaryCovRecordMap;

    static long long startTime;
    static CoverageInfo coverageInfo;
    static int coverageInfoRepeatCount;

private:
    static bool hybridSliceStallTrackingReady;
    static CoverageInfo hybridSliceLastCovSnapshot;
    static long long hybridSliceLastProgressClockTicks;
    /** 由 stepTCGHybrid 末尾调用：聚合覆盖约 10s 无增长则写出 SliceOut 请求 JSON */
    static void hybridCoverageStallSliceTickAfterStep();
    static void hybridEmitSliceRequestJsonForStall();
};


bool tryStateBranch(char* data, int size); // 在程序主体中实现（自动生成）
bool tryStateBranch(char* data, int size, bool enableDistanceGuided); // 在程序主体中实现（自动生成）