#pragma once

#include <string>
#include <vector>
#include <iostream>

int stringStartsWith(std::string s, std::string sub);

int stringEndsWith(std::string s, std::string sub);

std::vector<std::string> stringSplit(const std::string& s, const std::string& seperator);

std::string stringTrim(std::string s);

std::vector<std::string> stringTrimList(std::vector<std::string> strList);

std::string stringFillBlank(std::string s, int length, bool left = false);

std::string stringFillChar(std::string s, int length, char c, bool left = false);

int stringToInt(std::string str);

float stringToFloat(std::string str);

std::string uint64ToString(unsigned long long value);

std::string codeIndentProcess(std::string code);

std::string codeTrimAllLines(std::string code);

std::string codeRemoveDoubleBlank(std::string code);

std::string codeRemoveComment(std::string code);

int stringSubStrCount(const std::string& str, const std::string& sub);

std::string stringGetBlanks(int count);

void getFullPermutation(std::vector<int> dimension, std::vector<std::vector<int>>& ret);

bool isFolderExist(std::string folder);

bool isFileExist(std::string file);

bool createFolder(std::string folder);

std::string getExeDirPath();

std::string getExeFilePath();

std::string getParentPath(std::string path, int parentCount = 1);

void setCurrentDirPath(std::string path);

std::string readFile(std::string path);
void writeFile(std::string path, std::string content);
void writeFile(std::string path, char* data, int size);
void appendFile(std::string path, std::string content);

std::string stringToLower(std::string str);
std::string stringToUpper(std::string str);


void debugLog(std::string str, int width, bool pure);
void debugLog(std::string str, bool pure);
void debugLog(std::string str);

void log(std::string str, int width, bool pure);
void log(std::string str, bool pure);
void log(std::string str);


std::string stringRepeat(std::string s, int count);

std::string stringReplaceAll(std::string s, std::string src, std::string dst);

//系统调用，time参数为毫秒
int systemCallWithTimeOut(std::string exeFile, std::string parameter, int time, std::string redirectOutputFile);

void getCombineNumDFS(int N, int depth, std::vector<std::vector<int>> &result, std::vector<bool> &used);

std::vector<std::vector<int>> getCombineNum(int N);

std::string getFileNameWithoutSuffixByPath(std::string path);

bool areTwoIntVectorSame(std::vector<int> &v1, std::vector<int> &v2);

std::string getFileRealPath(std::string filePath);

void copyFileOne(std::string src, std::string dst);

template <typename T>
int findIndexInVector(std::vector<T>& v, T var)
{
    for(int i = 0; i < v.size(); i++)
    {
        if(v[i] == var)
            return i;
    }
    return -1;
}

std::string stringGetWord(std::string& code);

std::string stringGetNumber(std::string& code);

void listFiles(std::string dir, std::vector<std::string>& files);

//读取成功后必须使用delete[]回收内存
char* readFileBinary(std::string path, int& length);

void* asyncExecute(const char* cmd);

void terminateExe(void* handle);

unsigned long long crc64(const unsigned char *data, size_t len);

bool isProcessRunning(void* processId);

void deleteFile(std::string filePath);

long long getFileSize(std::string filename);

unsigned char* generateRandomDataByType(std::string type, int dataLength);