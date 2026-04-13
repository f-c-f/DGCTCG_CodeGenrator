#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <cstring>
#include <vector>
#include <map>
#include "SimulatorTestCase.h"
#include "ArgumentParser.h"
//#include "Utility.h"

using namespace std;

int SimulatorTestCase::offset = 0;
string SimulatorTestCase::binaryTestCaseFilePath = "";
char* SimulatorTestCase::testCaseData;

int SimulatorTestCase::initTestCase()
{    
    ifstream file(binaryTestCaseFilePath, ios::binary);

    size_t dotPosition = binaryTestCaseFilePath.find_last_of(".");
    std::string fileExtension = "";
    if (dotPosition != std::string::npos)
        fileExtension = binaryTestCaseFilePath.substr(dotPosition + 1);
    
    if(file.fail() || fileExtension.empty() || fileExtension != "bin")
    {
        cout << "Illegel test case file path!" << endl;
        return 0;
    }

    // 获取文件大小
    file.seekg(0, ios::end);
    int size = file.tellg();
    file.seekg(0);

    // 分配内存
    testCaseData = (char*)malloc(size);
    if (!testCaseData)
    {
        cout << "Memory allocation failed" << endl;
        return 0;
    }

    // 读取文件内容到testCaseData
    file.read(testCaseData, size);
    // 检查文件是否为空
    if (file.gcount() != size)
    {
        cout << "Failed to read file content" << endl;
        free(testCaseData);
        return 0;
    }

    // 设置偏移量为0
    offset = 0;

    return 1;
}

u8* SimulatorTestCase::takeTestCase(int dataTypeSize)
{
    u8* dataAddr = (u8*)SimulatorTestCase::testCaseData + offset;
    offset += dataTypeSize;
	return dataAddr;
}

void SimulatorTestCase::releaseTestCase()
{
    free(testCaseData);
}
