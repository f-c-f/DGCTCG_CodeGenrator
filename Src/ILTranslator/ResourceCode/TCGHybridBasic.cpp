/* Include Windows headers first to avoid std::byte (C++17) conflict with Windows byte typedef */
#define WIN32_LEAN_AND_MEAN
#define NOCRYPT
#include <Windows.h>

#include "TCGHybridBasic.h"
#include "tinyxml2.h"
#include "Utility.h"

#include <time.h>
#include <string.h>
#include <algorithm>
#include <queue>
#include <random>
#include <unordered_map>

#include "ArgumentParser.h"
#include "BasicExcel.hpp"
#include "TCGHybridBasicBranchRegister.h"    // ��ֹ windows.h �ж��� byte���������꣩
#include "TCGHybridBasicGlobalVariableRegister.h"
#include "TCGHybridBasicParameterRegister.h"
#include "TCGHybridBasicInputRegister.h"
using namespace YExcel;
using namespace std;

string TCGHybrid::exePath;

void* TCGHybrid::fuzzerProcessHandle = nullptr;
void* TCGHybrid::hstcgProcessHandle = nullptr;
void* TCGHybrid::paraFuzzerProcessHandle = nullptr;

vector<TCGHybrid::GlobalVariableData> TCGHybrid::globalVariableList;
vector<TCGHybrid::InputVariableData> TCGHybrid::inputList;
int TCGHybrid::inputsDataLengthTotal = 0;
vector<TCGHybrid::ParameterVariableData> TCGHybrid::modelParameterList;
int TCGHybrid::parametersDataLengthTotal;
long long TCGHybrid::startTime;

string TCGHybrid::paraBinaryTestCaseFilePath;
int TCGHybrid::paraMaxRunTime;
bool TCGHybrid::paraEnableFuzzer;
bool TCGHybrid::paraEnableHSTCG;
bool TCGHybrid::paraEnableHSTCGDistanceGuided;
bool TCGHybrid::paraEnableSTCG;
bool TCGHybrid::paraEnableParaFuzzer;
int TCGHybrid::paraSleepTime;

bool TCGHybrid::finish = false;

unordered_map<int64_t, bool> TCGHybrid::fileHashReadList;

map<int64_t, BitmapCov::BitmapCovRecord*> TCGHybrid::testCaseBinaryCovRecordMap;

TCGHybrid::CoverageInfo TCGHybrid::coverageInfo = {};
int TCGHybrid::coverageInfoRepeatCount = 0;


void TCGHybrid::updateBranchDistanceValue(const char* /*varName*/, double /*var*/)
{
    // Hybrid mode currently does not use distance-guided search directly.
    // This hook is kept so that generated code compiling for Hybrid can
    // still call TCGHybrid::updateBranchDistanceValue(...) without errors.
}


void TCGHybrid::initTCGHybrid(int argc, char *argv[])
{
    exePath = getExeDirPath();

    ArgumentParser argumentParser;
	paraBinaryTestCaseFilePath = "";
	paraMaxRunTime = 432000000; //�������5��ʱ��
    

    string helpString;
    helpString += "Usage: TCGHybridBasic.exe [options]\n";
    helpString += "Options:\n";
    helpString += "  -tb <path> : Binary Test Case File Path\n";
    helpString += "  -t <time> : Max Run Time\n";
    helpString += "  -fuzzer <true/false> : Enable Fuzzer, default is true\n";
    helpString += "  -hstcg <true/false> : Enable HSTCG, default is true\n";
    helpString += "  -hstcgdg  <true/false> : Enable HSTCG distance guided, default is false\n";
    helpString += "  -stcg <true/false> : Enable STCG, default is true\n";
    helpString += "  -parafuzzer <true/false> : Enable ParaFuzzer, default is false\n";
    helpString += "  -sleep <time> : Sleep Time (ms), default is 0\n";
    argumentParser.setHelpStr(helpString);

    argumentParser.parseArguments(argc, argv);

	paraBinaryTestCaseFilePath = argumentParser.getFlagValue("-tb", paraBinaryTestCaseFilePath);
	paraMaxRunTime =			 stringToInt(argumentParser.getFlagValue("-t", to_string(paraMaxRunTime)));
    paraEnableFuzzer = argumentParser.getFlagValue("-fuzzer", "true") == "true";
    paraEnableHSTCG = argumentParser.getFlagValue("-hstcg", "true") == "true";
    paraEnableHSTCGDistanceGuided = argumentParser.getFlagValue("-hstcgdg", "false") == "true";
    paraEnableSTCG = argumentParser.getFlagValue("-stcg", "true") == "true";
    paraEnableParaFuzzer = argumentParser.getFlagValue("-parafuzzer", "false") == "true";
    paraSleepTime = stringToInt(argumentParser.getFlagValue("-sleep", "0"));

    cout << argumentParser.flagToString() << endl;
    

    srand((unsigned)time(nullptr));
    startTime = clock();

    if(!isFolderExist(exePath + "/TestCaseOutputAll")) {
        createFolder(exePath + "/TestCaseOutputAll");
    }
    if(!isFolderExist(exePath + "/TestCases")) {
        createFolder(exePath + "/TestCases");
    }

    TCGHybridBasicBranchRegister::loadTCGHybridBranchData();
}

void TCGHybrid::initTCGHybrid2()
{
    // TCG Hybrid Regist Global Variables code
    TCGHybridBasicGlobalVariableRegister::registAllGlobalVariables();
    // TCG Hybrid Regist Parameters code
    TCGHybridBasicParameterRegister::registAllParameters();
    // TCG Hybrid Regist Inputs code
    TCGHybridBasicInputRegister::registAllInputs();
    /* Main Loop of model */
    BitmapCov::step();
    BitmapCov::recordTotal();
    if (paraEnableFuzzer) {
        TCGHybrid::callFuzzer();
    }
    if (paraEnableHSTCG) {
        TCGHybrid::callHSTCG();
    }
    if(paraEnableFuzzer || paraEnableHSTCG) {
        cout << "Sleep 3s" << endl;
        _sleep(3000);
    }
}

bool TCGHybrid::stepTCGHybrid()
{
    if (TCGHybrid::finish || isFileExist("finish.txt")) {
        return false;
    }
    vector<TCGHybrid::TestCaseBinary> testCasesBinary;

    bool res = TCGHybrid::minimizeTestCasesBinary(exePath + "/Fuzz/In", testCasesBinary);

    if (paraEnableSTCG) {
        TCGHybrid::callSTCGForSolve();
    }

    if (paraSleepTime > 0) {
        cout << "Sleep " << paraSleepTime/1000 << "s" << endl;
        _sleep(paraSleepTime);
    }

    if (paraEnableParaFuzzer) {
        TCGHybrid::callParaFuzzer();
    }

    return true;
}


void TCGHybrid::terminateTCGHybrid()
{
    
    string logStr = BitmapCov::printCoverage();
    string exePath = getExeDirPath();
    appendFile(exePath + "/log.txt", logStr);
    
    for(int i = 0; i < globalVariableList.size(); i++)
    {
        GlobalVariableData* globalVariableData = &(globalVariableList[i]);
        delete[] globalVariableData->dataOri;
    }
    if (fuzzerProcessHandle)
        terminateExe(fuzzerProcessHandle);
    if (hstcgProcessHandle)
        terminateExe(hstcgProcessHandle);
    if (paraFuzzerProcessHandle)
        terminateExe(paraFuzzerProcessHandle);
    for(auto& record : testCaseBinaryCovRecordMap){
        BitmapCov::releaseRecord(record.second);
    }

    BitmapCov::release();

}


void TCGHybrid::registGlobalVariables(string name, string type, vector<int>& arraySize, char* address, int length)
{
    GlobalVariableData globalVariableData;
    globalVariableData.address = address;
    globalVariableData.length = length;
    globalVariableData.type = type;
    globalVariableData.name = name;
    globalVariableData.arraySize = arraySize;
    globalVariableData.dataOri = new char[length];
    ::memcpy(globalVariableData.dataOri, address, length);

    globalVariableList.push_back(globalVariableData);

}

void TCGHybrid::registInputs(string name, string type, int length)
{
    InputVariableData inputRecord;
    inputRecord.name = name;
    inputRecord.type = type;
    inputRecord.length = length;
    inputsDataLengthTotal += length;
    inputList.push_back(inputRecord);
}

void TCGHybrid::registParameters(string name, string type, int length)
{
    ParameterVariableData parameterRecord;
    parameterRecord.name = name;
    parameterRecord.type = type;
    parameterRecord.length = length;
    parametersDataLengthTotal += length;
    modelParameterList.push_back(parameterRecord);
}

namespace{
    int TestCaseOutPutCount = 0;
}

bool compareTestCaseBinary(const TCGHybrid::TestCaseBinary &x, const TCGHybrid::TestCaseBinary &y) {
    bool xFormSTCG = stringStartsWith(x.fileName, "TestCaseFromSTCG_");
    bool yFormSTCG = stringStartsWith(y.fileName, "TestCaseFromSTCG_");

    return x.length < y.length;

    //if(xFormSTCG && yFormSTCG) {
    //    return x.fileName < y.fileName;
    //} else if(xFormSTCG) {
    //    return true;
    //} else if(yFormSTCG) {
    //    return false;
    //} else {
    //    return x.length < y.length;
    //}
}


bool TCGHybrid::minimizeTestCasesBinary(string inputTestCasesDir, vector<TestCaseBinary>& testCasesBinary)
{
    string exePath = getExeDirPath();

    string testCasesTempDir = exePath + "/TestCases";
    string testCasesAllDir = exePath + "/TestCaseOutputAll";

    vector<string> fileList;
    listFiles(inputTestCasesDir, fileList);

    if(fileList.empty())
        return false;

    testCasesBinary.resize(fileList.size());

    string logStr = "Read " + to_string(fileList.size()) + " TestCase Files.\n";
    cout << logStr;
    appendFile(exePath + "/log.txt", logStr);

    for(int i = 0; i < fileList.size(); i++)
    {
        string fileName = inputTestCasesDir + "/" + fileList[i];
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

        delete[] data;

        testCasesBinary[i].fileName = fileList[i];
        testCasesBinary[i].filePath = fileName;
        testCasesBinary[i].data = dataBuf;
        testCasesBinary[i].length = sizeBuf;
        testCasesBinary[i].hash = crc64((const uint8_t*)dataBuf, sizeBuf);
    }

    sort(testCasesBinary.begin(), testCasesBinary.end(), compareTestCaseBinary);

    //string cmdTemp;
    //cmdTemp = "rd /s/q \"" + testCasesTempDir + "\"";
    //system(cmdTemp.c_str());
    //cmdTemp = "mkdir \"" + testCasesTempDir + "\"";
    //system(cmdTemp.c_str());


    BitmapCov::setRecordTotalBitmap();

    int offset = 0;
    for(int i = 0; i - offset < testCasesBinary.size(); i++)
    {
        string filePath = testCasesBinary[i-offset].filePath;
        int size = testCasesBinary[i-offset].length;
        char* data = testCasesBinary[i-offset].data;
        int64_t hash = testCasesBinary[i-offset].hash;

        string fileName = testCasesBinary[i-offset].fileName;

        if(testCaseBinaryCovRecordMap.find(hash) == testCaseBinaryCovRecordMap.end())
        {
            string logStr = "Run " + fileName + "\n";
            cout << logStr;
            appendFile(exePath + "/log.txt", logStr);
            BitmapCov::clearCurrentBitmap();
            resetGlobalVariables();
            tryStateBranch(data, size);
            BitmapCov::BitmapCovRecord* record = BitmapCov::recordCurrent();
            testCaseBinaryCovRecordMap[hash] = record;
        }
        else
        {
            string logStr = "Jump Run " + fileName + "\n";
            cout << logStr;
            appendFile(exePath + "/log.txt", logStr);
            BitmapCov::setRecordCurrentBitmap(testCaseBinaryCovRecordMap[hash]);
        }
        

        bool enc_new1 = BitmapCov::processBitmap(BitmapCov::munitBitmap, BitmapCov::munitBitmapLength, BitmapCov::munitAllBitmap, BitmapCov::munitCov);
        bool enc_new2 = BitmapCov::processBitmap(BitmapCov::branchBitmap, BitmapCov::branchBitmapLength, BitmapCov::branchAllBitmap, BitmapCov::branchCov);
        bool enc_new3 = BitmapCov::processBitmap(BitmapCov::condBitmap, BitmapCov::condBitmapLength, BitmapCov::condAllBitmap, BitmapCov::condCov);
        bool enc_new4 = BitmapCov::processBitmap(BitmapCov::decBitmap, BitmapCov::decBitmapLength, BitmapCov::decAllBitmap, BitmapCov::decCov);
        bool enc_new5 = BitmapCov::processBitmap(BitmapCov::mcdcResultBitmap, BitmapCov::mcdcResultBitmapLength, BitmapCov::mcdcResultAllBitmap, BitmapCov::mcdcCov);
        //bool enc_new5 = BitmapCov::processMCDCBitmap();
        

        if(enc_new1 || enc_new2 || enc_new3 || enc_new4 || enc_new5)
        {
            
            TestResult* testRes = new TestResult();
            testCasesBinary[i-offset].testResult = testRes;
            testRes->branchCoverage = new unsigned char[BitmapCov::branchBitmapLength];
            memcpy(testRes->branchCoverage, (const char*)BitmapCov::branchBitmap, BitmapCov::branchBitmapLength);
            testRes->maxCoverageDepth = 0;

            for(int j = 0; j < BitmapCov::branchBitmapLength; j++)
            {
                if(!testRes->branchCoverage[j])
                {
                    continue;
                }
                if (TCGHybridBasicBranchRegister::TCGHybrid_Branch_Depth[j] > testRes->maxCoverageDepth)
                {
                    testRes->maxCoverageDepth = TCGHybridBasicBranchRegister::TCGHybrid_Branch_Depth[j];
                }
            }
        }
        else
        {
            delete[] testCasesBinary[i-offset].data;
            testCasesBinary.erase(testCasesBinary.begin() + (i - offset));
            offset++;
        }
    }

    
    if(testCasesBinary.empty())
        return false;

    sort(testCasesBinary.begin(), testCasesBinary.end(), 
               [](const TestCaseBinary &x, const TestCaseBinary &y) -> int {
           return x.testResult->maxCoverageDepth > y.testResult->maxCoverageDepth;
       });


    for(int i = 0; i < testCasesBinary.size(); i++)
    {
        if(fileHashReadList.find(testCasesBinary[i].hash) != fileHashReadList.end())
        {
            delete[] testCasesBinary[i].data;
            continue;
        }
        fileHashReadList[testCasesBinary[i].hash] = true;

        TestCaseOutPutCount++;
        writeFile(testCasesTempDir + "/" + stringFillChar(to_string(TestCaseOutPutCount), 10, '0', true) + ".bin", testCasesBinary[i].data, testCasesBinary[i].length);
        writeFile(testCasesAllDir + "/" + stringFillChar(to_string(TestCaseOutPutCount), 10, '0', true) + ".bin", testCasesBinary[i].data, testCasesBinary[i].length);
        delete[] testCasesBinary[i].data;
        delete[] testCasesBinary[i].testResult->branchCoverage;
        delete testCasesBinary[i].testResult;
    }

    if(coverageInfo.munitCov >= BitmapCov::munitCov &&
        coverageInfo.branchCov >= BitmapCov::branchCov &&
        coverageInfo.condCov >= BitmapCov::condCov &&
        coverageInfo.decCov >= BitmapCov::decCov &&
        coverageInfo.mcdcCov >= BitmapCov::mcdcCov)
    {
        coverageInfoRepeatCount++;
    }
    else
    {
        coverageInfo.munitCov = BitmapCov::munitCov > coverageInfo.munitCov ? BitmapCov::munitCov : coverageInfo.munitCov;
        coverageInfo.branchCov = BitmapCov::branchCov > coverageInfo.branchCov ? BitmapCov::branchCov : coverageInfo.branchCov;
        coverageInfo.condCov = BitmapCov::condCov > coverageInfo.condCov ? BitmapCov::condCov : coverageInfo.condCov;
        coverageInfo.decCov = BitmapCov::decCov > coverageInfo.decCov ? BitmapCov::decCov : coverageInfo.decCov;
        coverageInfo.mcdcCov = BitmapCov::mcdcCov > coverageInfo.mcdcCov ? BitmapCov::mcdcCov : coverageInfo.mcdcCov;
        coverageInfoRepeatCount = 0;
    }

    logStr = BitmapCov::printCoverage();
    appendFile(exePath + "/log.txt", logStr);

    if (BitmapCov::munitCov == BitmapCov::munitBitmapLength &&
        BitmapCov::branchCov == BitmapCov::branchBitmapLength &&
        BitmapCov::condCov == BitmapCov::condBitmapLength &&
        BitmapCov::decCov == BitmapCov::decBitmapLength &&
        BitmapCov::mcdcCov == BitmapCov::mcdcResultBitmapLength) {
        TCGHybrid::finish = true;
    }

    return true;
}



bool TCGHybrid::callFuzzer()
{
    string exePath = getExeDirPath();
    string fuzzerPath = exePath + "/Fuzz/Fuzzer.exe";
    string cmd = fuzzerPath + " -print_final_stats=1 -detect_leaks=0 -max_total_time=3600 -reload=5 -len_control=20 In";
    setCurrentDirPath(exePath + "/Fuzz");
    fuzzerProcessHandle = asyncExecute(cmd.c_str());
    setCurrentDirPath(exePath);
    return true;
}

bool TCGHybrid::callHSTCG()
{
    string exePath = getExeDirPath();
    string hstcgPath = exePath + "/HSTCG/HSTCG.exe";
    string cmd = hstcgPath + " -ob ../Fuzz/In";
    if(TCGHybrid::paraEnableParaFuzzer) {
        cmd += " -outputLong 5";
        // cmd += " -outputLongMaxLen 1200";
    }
    if(TCGHybrid::paraEnableHSTCGDistanceGuided) {
        cmd += " -distanceGuided true";
    }

    setCurrentDirPath(exePath + "/HSTCG");
    hstcgProcessHandle = asyncExecute(cmd.c_str());
    setCurrentDirPath(exePath);
    return true;
}

namespace {
    int lastSTCGTestCaseId = 0;
    int pureRunSTCGTimeBaseLine = 10000;
}
bool TCGHybrid::callSTCGForSolve()
{
    string exePath = getExeDirPath();

    string cmdTemp;
    cmdTemp = "rd /s/q \"" + exePath + "/STCG/TestCaseOutput\"";
    system(cmdTemp.c_str());
    cmdTemp = "mkdir \"" + exePath + "/STCG/TestCaseOutput\"";
    system(cmdTemp.c_str());
    cmdTemp = "mkdir \"" + exePath + "/STCG/TestCaseOutput/Binary\"";
    system(cmdTemp.c_str());

    //writeFile(exePath + "/TempInputForSTCG/temp.bin", testCaseBinary.data, testCaseBinary.length);

    setCurrentDirPath(exePath + "/STCG");

    string cmd = "STCG.exe"; 
    //if(coverageInfoRepeatCount < 2) // ����Fuzz����
    

    int pureRunSTCGTime = pureRunSTCGTimeBaseLine * 15 / 10;
    cmd += " -tb " + exePath + "/Fuzz/In"; //  -noRand true
    //cmd += " -once true";
    cmd += " -t " + to_string(pureRunSTCGTime);
    cmd += " -distanceGuided true";
    if (TCGHybrid::paraEnableParaFuzzer) {
        cmd += " -outputLong 5";
        // cmd += " -outputLongMaxLen 1200";
    }

    string logStr = cmd + "\n";
    cout << logStr;
    appendFile(exePath + "/log.txt", logStr);

    int res = system(cmd.c_str());
    
    if(res == 1) {
        cmd += " -sm 2";
        logStr = cmd + "\n";
        cout << logStr;
        appendFile(exePath + "/log.txt", logStr);
        res = system(cmd.c_str());
    }
    
    
    setCurrentDirPath(exePath);

    vector<string> fileList;
    string STCGOutputDir = exePath + "/STCG/TestCaseOutput/Binary";
    if(!isFolderExist(STCGOutputDir))
        return false;
    listFiles(STCGOutputDir, fileList);
    if(fileList.empty())
        return false;
    for(int i = 0; i < fileList.size(); i++)
    {
        lastSTCGTestCaseId++;
        string fileNameSrc = STCGOutputDir + "/" + fileList[i];
        string fileNameDst1 = exePath + "/Fuzz/In/TestCaseFromSTCG_" + stringFillChar(to_string(lastSTCGTestCaseId), 10, '0', true) + ".bin";
        //string fileNameDst2 = exePath + "/TestCaseOutputAll/TestCaseFromSTCG_" + to_string(lastSTCGTestCaseId) + ".bin";
        copyFileOne(fileNameSrc, fileNameDst1);
        //copyFileOne(fileNameSrc, fileNameDst2);
    }
    return true;
}

namespace {
    map<string, bool> copyInputFileMap;
    map<string, bool> copyOutputFileMap;
    clock_t lastCopyTime = 0;
    int copyCountTotal = 0;
}

bool TCGHybrid::callParaFuzzer()
{
    clock_t currentTime = clock();
    if (currentTime - lastCopyTime < 3000)
        return false;
    lastCopyTime = currentTime;

    string exePath = getExeDirPath();

    // ��Fuzz/InĿ¼�������������ļ���FuzzPara/InputSequenceĿ¼��һֱֻ������������
    copyCountTotal += copyMaxFileToInputSquenceDir(exePath + "/Fuzz/In", 3);
    if(TCGHybrid::paraEnableHSTCG)
        copyCountTotal += copyMaxFileToInputSquenceDir(exePath + "/HSTCG/LongTestCases/Binary", 3);
    if (TCGHybrid::paraEnableSTCG)
        copyCountTotal += copyMaxFileToInputSquenceDir(exePath + "/STCG/LongTestCases/Binary", 3);

    if (copyCountTotal <= 0)
        return false;

    if (paraFuzzerProcessHandle)
    {
        // ��ParaFuzzer/OutĿ¼�µ��ļ�������Fuzz/In/Ŀ¼
        vector<string> fileList;
        listFiles(exePath + "/ParaFuzzer/Out", fileList);
        for (int i = 0; i < fileList.size(); i++)
        {
            string fileName = fileList[i];
            if (copyOutputFileMap.find(fileName) != copyOutputFileMap.end())
                continue;
            string filePath = exePath + "/ParaFuzzer/Out/" + fileName;
            string fileNameDst = exePath + "/Fuzz/In/" + fileName;
            copyFileOne(filePath, fileNameDst);
            copyOutputFileMap[fileName] = true;
        }
    }

    if (!paraFuzzerProcessHandle)
    {
        string exePath = getExeDirPath();
        string paraFuzzerPath = exePath + "/FuzzPara/Fuzzer.exe";
        string cmd = paraFuzzerPath;
        cmd += " -print_final_stats=1";
        cmd += " -detect_leaks=0";
        cmd += " -max_total_time=3600";
        //cmd += " -reload=5";
        cmd += " -len_control=0";
        cmd += " -max_len=" + to_string(parametersDataLengthTotal);
        cmd += " In";

        setCurrentDirPath(exePath + "/FuzzPara");
        paraFuzzerProcessHandle = asyncExecute(cmd.c_str());
        setCurrentDirPath(exePath);
    }


    
    return false;
}

namespace {
    struct MaxSizeFileList {
        string dir;
        vector<pair<string, int>> fileSize;
    };
    map<string, MaxSizeFileList> maxSizeFileListMap;
}
int TCGHybrid::copyMaxFileToInputSquenceDir(string srcDir, int fileCount)
{
    vector<string> fileList;
    listFiles(srcDir, fileList);
    if (fileList.empty())
        return 0;
    vector<pair<string, int>> fileListSize;
    for (int i = 0; i < fileList.size(); i++)
    {
        string fileName = fileList[i];
        string filePath = srcDir + "/" + fileName;
        int size = getFileSize(filePath);
        //if (size > parametersDataLengthTotal + inputsDataLengthTotal)
        fileListSize.push_back(make_pair(fileName, size));
    }
    sort(fileListSize.begin(), fileListSize.end(),
        [](const pair<string, int>& x, const pair<string, int>& y) -> int {
        return x.second > y.second;
    });

    if (maxSizeFileListMap.find(srcDir) == maxSizeFileListMap.end())
    {
        MaxSizeFileList maxSizeFileList;
        maxSizeFileList.dir = srcDir;
        maxSizeFileListMap[srcDir] = maxSizeFileList;
    }

    MaxSizeFileList* maxSizeFileList = &(maxSizeFileListMap[srcDir]);

    // ���maxSizeFileList->fileSize�е��ļ����ڲ���fileListSize��ǰfileCount�еģ���ɾ����Ӧ���ļ�
    map<string, bool> deleteFileList;
    map<string, bool> copyFileList;
    for (int i = 0; i < maxSizeFileList->fileSize.size(); i++)
    {
        deleteFileList[maxSizeFileList->fileSize[i].first] = true;
    }
    maxSizeFileList->fileSize.clear();
    int copyCount = 0;
    for (int i = 0; i < fileListSize.size() && i < fileCount; i++)
    {
        string fileName = fileListSize[i].first;
        maxSizeFileList->fileSize.push_back(fileListSize[i]);
        if (deleteFileList.find(fileName) != deleteFileList.end())
        {
            deleteFileList.erase(fileName);
        }
        else
        {
            copyFileList[fileName] = true;
            copyCount++;
        }
    }
    for (auto it = deleteFileList.begin(); it != deleteFileList.end(); it++)
    {
        string fileName = it->first;
        string fileNameDst = exePath + "/FuzzPara/InputSequence/" + fileName;
        deleteFile(fileNameDst);
    }
    for (auto it = copyFileList.begin(); it != copyFileList.end(); it++)
    {
        string fileName = it->first;
        string filePath = srcDir + "/" + fileName;
        string fileNameDst = exePath + "/FuzzPara/InputSequence/" + fileName;
        copyFileOne(filePath, fileNameDst);
    }

    //for (int i = 0; i < fileListSize.size() && i < fileCount; i++)
    //{
    //    string fileName = fileListSize[i].first;
    //    if (copyInputFileMap.find(fileName) != copyInputFileMap.end())
    //        continue;
    //    string filePath = srcDir + "/" + fileName;
    //    string fileNameDst = exePath + "/FuzzPara/InputSequence/" + fileName;
    //    copyFileOne(filePath, fileNameDst);
    //    copyInputFileMap[fileName] = true;
    //    copyCount++;
    //}
    return copyCount;
}

void TCGHybrid::resetGlobalVariables()
{
    for(int i = 0; i < globalVariableList.size(); i++)
    {
        GlobalVariableData* globalVariableData = &(globalVariableList[i]);
        char* addr = globalVariableData->address;
        int size = globalVariableData->length;
        memcpy(addr, globalVariableData->dataOri, size);
    }
}
