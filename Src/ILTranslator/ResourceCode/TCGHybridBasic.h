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
    static bool tryStateBranchInner2(StateTraceNode* stateTraceNode);
    
    static bool callFuzzer();
    static bool callHSTCG();
    static bool callSTCGForSolve();
    static bool callParaFuzzer();
    static int copyMaxFileToInputSquenceDir(string srcDir, int fileCount);

    // 用某次的记录恢复全局变量
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
};


bool tryStateBranch(char* data, int size); // 在程序主体中实现（自动生成）
