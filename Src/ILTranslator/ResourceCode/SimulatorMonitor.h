#ifndef _MONITOR_H_
#define _MONITOR_H_

#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <iostream>

#define MAX_DATALEN 1024000
#define MAX_STEPLEN 4096000

typedef int					i32;
typedef float				f32;
typedef long long			i64;
typedef double				f64;
typedef long double			f128;
typedef char				i8;
typedef short				i16;
typedef unsigned int		u32;
typedef unsigned long long	u64;
typedef unsigned char		u8;
typedef unsigned short		u16;

using namespace std;

struct MonitorOutputData
{
	int outID;
	//string name;
	u8* data;
	int dataLen;
	int arraySize;
	string dataType;
	int dataTypeSize;
	//vector<int> stepIdList;
	int* stepIdList;
	int stepIdListLen;
	FILE* dfile;
	FILE* sfile;
};

class SimulatorMonitor
{
public:
	static int stepId;
	//static map<string, MonitorOutputData*> OutputData;
	static MonitorOutputData** OutputData;
	//static map<string, void*> StructFuncPointer;

public:
	static void monitorNextStep();
	static void monitorRegisterOutputData(int outID, int totalstep, string dataType, int dataTypeSize);
	static void monitorRecordValue(int outID, u8* dataAddr);
	//static void monitorSave(FILE* filePath);
	static void monitorPrint();
	//void DataTypeWriterFuncRegister(string dataType, FUNC funcAddr);

private:
	static MonitorOutputData* getMonitorOutputData(string name);
	static void recordValue(u8* dataAddr, MonitorOutputData* MOD);
	//static void outputValue(u8* dataAddr, string dataType, FILE* filePath);
	static void printValue(u8 * dataAddr, string dataType, int stepNum);

};

#endif // _MONITOR_H

//void system_11_state_toString(u8* dataAddr)
//{
//    system_11_state value;
//
//    memcpy(&value, dataAddr, sizeof(vlaue));
//
//    ....
//}
