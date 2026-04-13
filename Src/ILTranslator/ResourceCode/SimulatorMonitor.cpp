#include "SimulatorMonitor.h"
#include <stdio.h>
#include <string>
#include <cstring>
#include <iostream>

using namespace std;

int SimulatorMonitor::stepId = 0;
//map<string, MonitorOutputData*> SimulatorMonitor::OutputData;
MonitorOutputData** SimulatorMonitor::OutputData;

void SimulatorMonitor::monitorNextStep()
{
	SimulatorMonitor::stepId++;
}

void SimulatorMonitor::monitorRegisterOutputData(int outID, int totalstep, string dataType, int dataTypeSize)
{
	MonitorOutputData* MOD = new MonitorOutputData();
	MOD->outID = outID;
	MOD->arraySize = 0;
	//MOD->dataType = dataType;
	MOD->dataTypeSize = dataTypeSize;

	//MOD->data = new u8[totalstep * MOD->dataTypeSize];
	MOD->data = new u8[MAX_DATALEN];
	MOD->stepIdList = new int[MAX_STEPLEN];
	MOD->dataLen = 0;
	MOD->stepIdListLen = 0;
	MOD->dfile = nullptr;
	MOD->sfile = nullptr;
	OutputData[outID] = MOD;
}

void SimulatorMonitor::monitorRecordValue(int outID, u8* dataAddr)
{

	MonitorOutputData* MOD = SimulatorMonitor::OutputData[outID];

	SimulatorMonitor::recordValue(dataAddr, MOD);
}

//void SimulatorMonitor::DataTypeWriterFuncRegister(string dataType, FUNC funcAddr)
//{
//	StructFuncPointer[dataType] = funcAddr;
//}

//void SimulatorMonitor::monitorSave(FILE* filePath)
//{
//	map<string, MonitorOutputData*>::iterator iter = OutputData.begin();
//	for (int i = 0; iter != OutputData.end(); i++)
//	{
//		MonitorOutputData* MOD = iter->second;
//		int dataTypeSize = MOD->dataTypeSize;
//		for (int i = 0; i < MOD->arraySize; i++)
//		{
//			outputValue(MOD->data + i * dataTypeSize, MOD->dataType, filePath);
//
//		}
//
//	}
//}

//void SimulatorMonitor::monitorPrint()
//{
//	map<string, MonitorOutputData*>::iterator iter;
//	for (iter = OutputData[0]; iter != OutputData[]; iter++)
//	{
//		MonitorOutputData* MOD = iter->second;
//		int stepNum;
//		cout << MOD->name << ": " << endl;
//		for (int i = 0; i < MOD->arraySize; i++)
//		{
//			stepNum = MOD->stepIdList[i];
//			printValue(MOD->data + i * MOD->dataTypeSize, MOD->dataType, stepNum);
//		}
//		cout << endl;		
//	}
//}

//MonitorOutputData* SimulatorMonitor::getMonitorOutputData(string name)
//{
//	auto it = OutputData.find(name);
//	if (it != OutputData.end())
//		return it->second;
//	return nullptr;
//}

void SimulatorMonitor::recordValue(u8* dataAddr, MonitorOutputData* MOD)
{
	int dataTypeSize = MOD->dataTypeSize;
	if (MOD->dataLen + dataTypeSize > MAX_DATALEN)
	{
		if (!MOD->dfile)
		{
			char buf[10] = "data/d";
			sprintf(&buf[6], "%d", MOD->outID);
			MOD->dfile = fopen(buf, "wb");
		}
		int r = fwrite(MOD->data, 1, MOD->dataLen, MOD->dfile);
		MOD->dataLen = 0;
	}
	memcpy(MOD->data + MOD->dataLen, dataAddr, dataTypeSize);
	if (MOD->stepIdListLen + 4 > MAX_STEPLEN)
	{
		if (!MOD->sfile)
		{
			char buf[10] = "data/s";
			sprintf(&buf[6], "%d", MOD->outID);
			MOD->sfile = fopen(buf, "wb");
		}
		fwrite(MOD->stepIdList, sizeof(int), MOD->stepIdListLen, MOD->sfile);
		MOD->stepIdListLen = 0;
	}
	MOD->stepIdList[MOD->stepIdListLen] = stepId;
	//MOD->stepIdList.push_back(stepId);
	MOD->arraySize++;
	MOD->dataLen += dataTypeSize;
	MOD->stepIdListLen += 1;
}

//void SimulatorMonitor::outputValue(u8* dataAddr, string dataType, FILE* filePath)
//{
//
//	if (dataType == "f32")
//	{
//		int value = 0;
//		memcpy(&value, dataAddr, sizeof(int));
//		string str = to_string(value);
//		fwrite(str.c_str(), sizeof(str), 4, filePath);
//	}
//	else if ()
//		;
//
//	else
//	{
//		foreach(auto p in StructFuncPointer)
//		{
//			if (p->first == dataType)
//			{
//				void* funcPointer = p->second;
//
//				funcPointer(dataAddr);
//
//				break;
//			}
//
//		}
//	}
//
//}

void SimulatorMonitor::printValue(u8* dataAddr, string dataType, int stepNum)
{
	if (dataType == "i32" || dataType == "sint32")
	{
		i32 val = *(i32*)dataAddr;
		cout <<"step " << stepNum << ": " << val << "\t";
	}
	else if (dataType == "f32")
	{
		f32 val = *(f32*)dataAddr;
		cout << "step " << stepNum << ": " << val << "\t";
	}
	else if (dataType == "i64")
	{
		i64 val = *(i64*)dataAddr;
		cout << "step " << stepNum << ": " << val << "\t";
	}
	else if (dataType == "f64")
	{
		f64 val = *(f64*)dataAddr;
		cout << "step " << stepNum << ": " << val << "\t";
	}
	else if (dataType == "f128")
	{
		f128 val = *(f128*)dataAddr;
		cout << "step " << stepNum << ": " << val << "\t";
	}
	else if (dataType == "i8")
	{
		i8 val = *(i8*)dataAddr;
		cout << "step " << stepNum << ": " << val << "\t";
	}
	else if (dataType == "i16" || dataType == "short int")
	{
		i16 val = *(i16*)dataAddr;
		cout << "step " << stepNum << ": " << val << "\t";
	}
	else if (dataType == "u32")
	{
		u32 val = *(u32*)dataAddr;
		cout << "step " << stepNum << ": " << val << "\t";
	}
	else if (dataType == "u64")
	{
		u64 val = *(u64*)dataAddr;
		cout << "step " << stepNum << ": " << val << "\t";
	}
	else if (dataType == "u8")
	{
		u8 val = *(u8*)dataAddr;
		cout << "step " << stepNum << ": " << val << "\t";
	}

}




