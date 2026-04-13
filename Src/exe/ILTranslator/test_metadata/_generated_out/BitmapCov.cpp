#include "BitmapCov.h"
#include <string.h>
#include <iostream>
#include <iomanip>
#include <sstream>
using namespace std;



int BitmapCov::munitBitmapLength = 0;
int BitmapCov::branchBitmapLength = 0;
int BitmapCov::condBitmapLength = 0;
int BitmapCov::decBitmapLength = 0;
int BitmapCov::mcdcBitmapLength = 0;
int BitmapCov::mcdcResultBitmapLength = 0;

unsigned int* BitmapCov::mcdcResultBitmapOffset = nullptr;

unsigned int BitmapCov::mcdcDecTemp = 0;
unsigned char* BitmapCov::mcdcMeta = nullptr;



unsigned char* BitmapCov::munitBitmap = nullptr;
unsigned char* BitmapCov::branchBitmap = nullptr;
unsigned char* BitmapCov::condBitmap = nullptr;
unsigned char* BitmapCov::decBitmap = nullptr;
unsigned char* BitmapCov::mcdcResultBitmap = nullptr;
//std::vector<std::set<unsigned int>*>* BitmapCov::mcdcBitmap = nullptr;



unsigned char* BitmapCov::munitAllBitmap = nullptr;
unsigned char* BitmapCov::branchAllBitmap = nullptr;
unsigned char* BitmapCov::condAllBitmap = nullptr;
unsigned char* BitmapCov::decAllBitmap = nullptr;
unsigned char* BitmapCov::mcdcResultAllBitmap = nullptr;
//std::vector<std::set<unsigned int>*>* BitmapCov::mcdcAllBitmap = nullptr;
int BitmapCov::munitCov = 0;
int BitmapCov::branchCov = 0;
int BitmapCov::condCov = 0;
int BitmapCov::decCov = 0;
int BitmapCov::mcdcCov = 0;
//int BitmapCov::totalMcdcCov = 0;



//unsigned char* BitmapCov::munitRecordBitmap = nullptr;
//unsigned char* BitmapCov::branchRecordBitmap = nullptr;
//unsigned char* BitmapCov::condRecordBitmap = nullptr;
//unsigned char* BitmapCov::decRecordBitmap = nullptr;
//unsigned char* BitmapCov::mcdcResultRecordBitmap = nullptr;
//int BitmapCov::munitRecordCov = 0;
//int BitmapCov::branchRecordCov = 0;
//int BitmapCov::condRecordCov = 0;
//int BitmapCov::decRecordCov = 0;
//int BitmapCov::mcdcRecordCov = 0;
////int BitmapCov::totalMcdcRecordCov = 0;

BitmapCov::BitmapCovRecord BitmapCov::bitmapCovRecord;

std::vector<MCDCFunc> BitmapCov::mcdcFuncs;

//unsigned int* BitmapCov::mcdcDecTemp = nullptr;
//std::vector<std::set<unsigned int>*>* BitmapCov::mcdcBitmap = nullptr;
//int BitmapCov::mcdcBitmapLength = 0;

void BitmapCov::init()
{
    munitCov = 0;
    branchCov = 0;
    condCov = 0;
    decCov = 0;
    mcdcCov = 0;
    //totalMcdcCov = 0;
    mcdcResultBitmapOffset = new unsigned int[BitmapCov::mcdcBitmapLength];
    memset(mcdcResultBitmapOffset, 0, BitmapCov::mcdcBitmapLength * sizeof(unsigned int));
    mcdcResultBitmapLength = 0;
    for (int k = 0; k < BitmapCov::mcdcBitmapLength; k++) {
        mcdcResultBitmapOffset[k] = mcdcResultBitmapLength * 2;
        mcdcResultBitmapLength += BitmapCov::mcdcMeta[k];
    }
    mcdcResultBitmapLength *= 2;

    munitAllBitmap = new unsigned char[BitmapCov::munitBitmapLength];
    memset(munitAllBitmap, 0, BitmapCov::munitBitmapLength * sizeof(unsigned char));
    branchAllBitmap = new unsigned char[BitmapCov::branchBitmapLength];
    memset(branchAllBitmap, 0, BitmapCov::branchBitmapLength * sizeof(unsigned char));
    condAllBitmap = new unsigned char[BitmapCov::condBitmapLength];
    memset(condAllBitmap, 0, BitmapCov::condBitmapLength * sizeof(unsigned char));
    decAllBitmap = new unsigned char[BitmapCov::decBitmapLength];
    memset(decAllBitmap, 0, BitmapCov::decBitmapLength * sizeof(unsigned char));
    //mcdcAllBitmap = new std::vector<std::set<unsigned int>*>();
    //BitmapCov::CreateMCDCBitmap(mcdcAllBitmap, BitmapCov::mcdcBitmapLength);
    mcdcResultAllBitmap = new unsigned char[BitmapCov::mcdcResultBitmapLength];
    memset(mcdcResultAllBitmap, 0, BitmapCov::mcdcResultBitmapLength * sizeof(unsigned char));


    bitmapCovRecord.munitBitmap = new unsigned char[BitmapCov::munitBitmapLength];
    memset(bitmapCovRecord.munitBitmap, 0, BitmapCov::munitBitmapLength * sizeof(unsigned char));
    bitmapCovRecord.branchBitmap = new unsigned char[BitmapCov::branchBitmapLength];
    memset(bitmapCovRecord.branchBitmap, 0, BitmapCov::branchBitmapLength * sizeof(unsigned char));
    bitmapCovRecord.condBitmap = new unsigned char[BitmapCov::condBitmapLength];
    memset(bitmapCovRecord.condBitmap, 0, BitmapCov::condBitmapLength * sizeof(unsigned char));
    bitmapCovRecord.decBitmap = new unsigned char[BitmapCov::decBitmapLength];
    memset(bitmapCovRecord.decBitmap, 0, BitmapCov::decBitmapLength * sizeof(unsigned char));
    //bitmapCovRecord.mcdcBitmap = new std::vector<std::set<unsigned int>*>();
    //BitmapCov::CreateMCDCBitmap(bitmapCovRecord.mcdcBitmap, BitmapCov::mcdcBitmapLength);
    bitmapCovRecord.mcdcResultBitmap = new unsigned char[BitmapCov::mcdcResultBitmapLength];
    memset(bitmapCovRecord.mcdcResultBitmap, 0, BitmapCov::mcdcResultBitmapLength * sizeof(unsigned char));

    //std::vector<std::vector<unsigned int>*>* mcdcAllBitmapTestCaseIndex = new std::vector<std::vector<unsigned int>*>();
    //BitmapCov::CreateMCDCTestCaseIndexVector(mcdcAllBitmapTestCaseIndex, BitmapCov::mcdcBitmapLength);

    unsigned int* allMcdcCov = new unsigned int[BitmapCov::mcdcBitmapLength];
    memset(allMcdcCov, 0, BitmapCov::mcdcBitmapLength * sizeof(unsigned int));
    BitmapCov::munitBitmap = new unsigned char[BitmapCov::munitBitmapLength];
    memset(munitBitmap, 0, BitmapCov::munitBitmapLength * sizeof(unsigned char));
    BitmapCov::branchBitmap = new unsigned char[BitmapCov::branchBitmapLength];
    memset(branchBitmap, 0, BitmapCov::branchBitmapLength * sizeof(unsigned char));
    BitmapCov::condBitmap = new unsigned char[BitmapCov::condBitmapLength];
    memset(condBitmap, 0, BitmapCov::condBitmapLength * sizeof(unsigned char));
    BitmapCov::decBitmap = new unsigned char[BitmapCov::decBitmapLength];
    memset(decBitmap, 0, BitmapCov::decBitmapLength * sizeof(unsigned char));
    //BitmapCov::mcdcDecTemp = new unsigned int[BitmapCov::mcdcBitmapLength];
    //memset(mcdcDecTemp, 0, BitmapCov::mcdcBitmapLength * sizeof(unsigned char));
    //BitmapCov::mcdcBitmap = new std::vector<std::set<unsigned int>*>();
    //BitmapCov::CreateMCDCBitmap(BitmapCov::mcdcBitmap, BitmapCov::mcdcBitmapLength);
    BitmapCov::mcdcResultBitmap = new unsigned char[BitmapCov::mcdcResultBitmapLength];
    memset(mcdcResultBitmap, 0, BitmapCov::mcdcResultBitmapLength * sizeof(unsigned char));

    registerMCDCFunctions();
}

bool BitmapCov::CondRecord(bool res, int condOrDecIdx)
{
	if (condOrDecIdx >= 0) {
		// handle condition
		if (res) {
			BitmapCov::condBitmap[condOrDecIdx] = 1;
		}
		else {
			BitmapCov::condBitmap[condOrDecIdx+1] = 1;
		}
	}
	else {
		// handle decision
		if (res) {
			// for decision id, it is set to decid+1 when parameter incoming. 
			// thus, here need to -1. 
			BitmapCov::decBitmap[(-condOrDecIdx-1)] = 1;
		}
		else {
			BitmapCov::decBitmap[(-condOrDecIdx-1)+1] = 1;
		}
	}
	return res;
}

// condIdx 现更名为：condOrDecIdx，正数为条件覆盖（记录从0 2 4 8），负数为决策覆盖（记录从-1 -3 -5）
// decCondIdx 现更名为：mcdcInnerIdx，为单个复合表达的内部id，0为该表达式本身，1为第一个条件，2为第二个条件，以此类推
// decIdx 现更名为：mcdcIdx，为MCDC的id
bool BitmapCov::CondAndMCDCRecord(bool res, int condOrDecIdx, int mcdcInnerIdx, int mcdcIdx)
{
	BitmapCov::CondRecord(res, condOrDecIdx);

    	// handle mcdc
    //	if (vect.size() == 0) {
    //		vect.push_back(0);
    //	}
    //	int lastPos = vect.size() - 1;
    BitmapCov::mcdcDecTemp |= ((res ? 1 : 0) << mcdcInnerIdx);
    //	vect[lastPos] |= ((res ? 1 : 0) << mcdcInnerIdx);
    if (mcdcInnerIdx == 0) {

        MCDCFunc mcdcFunc = BitmapCov::mcdcFuncs[mcdcIdx];
        mcdcFunc(mcdcDecTemp, BitmapCov::mcdcResultBitmap + mcdcResultBitmapOffset[mcdcIdx]);

        //vect.push_back(0);
    	//std::set<unsigned int>* vect = BitmapCov::mcdcBitmap->at(mcdcIdx);
    	//vect->insert(BitmapCov::mcdcDecTemp);
    	BitmapCov::mcdcDecTemp = 0;
    }
    //	BitmapCov::mcdcBitmap[mcdcIdx] = vect;
	return res;
}


void BitmapCov::CreateMCDCBitmap(std::vector<std::set<unsigned int>*>* mcdcBitmap, int bitmapLen)
{
    for (int i = 0; i < bitmapLen; i++) {
        std::set<unsigned int>* s = new std::set<unsigned int>();
        mcdcBitmap->push_back(s);
    }
}
void BitmapCov::ClearMCDCBitmap(std::vector<std::set<unsigned int>*>* mcdcBitmap, int bitmapLen)
{
    for (int i = 0; i < bitmapLen; i++) {
        std::set<unsigned int>* s = mcdcBitmap->at(i);
        s->clear();
    }
}
void BitmapCov::DeleteMCDCBitmap(std::vector<std::set<unsigned int>*>* mcdcBitmap, int bitmapLen)
{
    for (int i = 0; i < bitmapLen; i++) {
        std::set<unsigned int>* s = mcdcBitmap->at(i);
        delete s;
    }
    delete mcdcBitmap;
}
//void BitmapCov::CreateMCDCTestCaseIndexVector(std::vector<std::vector<unsigned int>*>* mcdcAllBitmapTestCaseIndex, int bitmapLen)
//{
//    for (int i = 0; i < bitmapLen; i++) {
//        std::vector<unsigned int>* v = new std::vector<unsigned int>();
//        mcdcAllBitmapTestCaseIndex->push_back(v);
//    }
//}
//void BitmapCov::DeleteMCDCTestCaseIndexVector(std::vector<std::vector<unsigned int>*>* mcdcAllBitmapTestCaseIndex, int bitmapLen)
//{
//    for (int i = 0; i < bitmapLen; i++) {
//        std::vector<unsigned int>* v = mcdcAllBitmapTestCaseIndex->at(i);
//        delete v;
//    }
//    delete mcdcAllBitmapTestCaseIndex;
//}

void BitmapCov::clearCurrentBitmap()
{
    memset(BitmapCov::munitBitmap, 0, BitmapCov::munitBitmapLength);
    memset(BitmapCov::branchBitmap, 0, BitmapCov::branchBitmapLength);
    memset(BitmapCov::condBitmap, 0, BitmapCov::condBitmapLength);
    memset(BitmapCov::decBitmap, 0, BitmapCov::decBitmapLength);
    memset(BitmapCov::mcdcResultBitmap, 0, BitmapCov::mcdcResultBitmapLength);
    //ClearMCDCBitmap(BitmapCov::mcdcBitmap, BitmapCov::mcdcBitmapLength);
}

void BitmapCov::clearTotalBitmap()
{
    memset(BitmapCov::munitAllBitmap, 0, BitmapCov::munitBitmapLength);
    memset(BitmapCov::branchAllBitmap, 0, BitmapCov::branchBitmapLength);
    memset(BitmapCov::condAllBitmap, 0, BitmapCov::condBitmapLength);
    memset(BitmapCov::decAllBitmap, 0, BitmapCov::decBitmapLength);
    memset(BitmapCov::mcdcResultAllBitmap, 0, BitmapCov::mcdcResultBitmapLength);
    //ClearMCDCBitmap(BitmapCov::mcdcAllBitmap, BitmapCov::mcdcBitmapLength);

    BitmapCov::munitCov = 0;
    BitmapCov::branchCov = 0;
    BitmapCov::condCov = 0;
    BitmapCov::decCov = 0;
    BitmapCov::mcdcCov = 0;
    //BitmapCov::totalMcdcCov = 0;
}


bool BitmapCov::step()
{
    bool enc_new1 = processBitmap(BitmapCov::munitBitmap, BitmapCov::munitBitmapLength, munitAllBitmap, munitCov);
    bool enc_new2 = processBitmap(BitmapCov::branchBitmap, BitmapCov::branchBitmapLength, branchAllBitmap, branchCov);
    bool enc_new3 = processBitmap(BitmapCov::condBitmap, BitmapCov::condBitmapLength, condAllBitmap, condCov);
    bool enc_new4 = processBitmap(BitmapCov::decBitmap, BitmapCov::decBitmapLength, decAllBitmap, decCov);
    bool enc_new5 = processBitmap(BitmapCov::mcdcResultBitmap, BitmapCov::mcdcResultBitmapLength, mcdcResultAllBitmap, mcdcCov);
    return enc_new2;
}


bool BitmapCov::processBitmap(unsigned char* bitmap, int bitmapLen, unsigned char* allBitmap, int& allCov)
{
    int inMunitCov = 0;
    for (int k = 0; k < bitmapLen; k++) {
        allBitmap[k] = allBitmap[k] | bitmap[k];
    }
    for (int k = 0; k < bitmapLen; k++) {
        if (allBitmap[k] > 0) {
            inMunitCov++;
        }else {
        }
    }
    bool newCov = false;
    if (allCov < inMunitCov) {
        allCov = inMunitCov;
        newCov = true;
    }
    if (newCov) {
        return true;
    }
    return false;
}

/*
bool BitmapCov::processMCDCBitmap()
{
    // 将单个用例的MCDC数据并入总体
    for (int k = 0; k < BitmapCov::mcdcBitmapLength; k++) {
        std::set<unsigned int>* mcdcBitmapSetK = BitmapCov::mcdcBitmap->at(k);
        std::set<unsigned int>* allMcdcBitmapSetK = BitmapCov::mcdcAllBitmap->at(k);
        allMcdcBitmapSetK->insert(mcdcBitmapSetK->begin(), mcdcBitmapSetK->end());
    }

    // 统计总体MCDC覆盖率
    // 按每个set统计各个条件组和的MDCD覆盖率，比如：
    // 一个set中，存在如下数据：
    // 01000 1
    // 00000 0
    // 00010 0
    // 根据第一第二行，就可确认第二位的覆盖率为2

    int mcdcResId = 0;
    for (int k = 0; k < BitmapCov::mcdcBitmapLength; k++) {
        std::set<unsigned int>* allMcdcBitmapSetK = BitmapCov::mcdcAllBitmap->at(k);

        int innerConditionCount = BitmapCov::mcdcMeta[k];

        for (int i = 0; i < innerConditionCount; i++)
        {
            int mask = (1 << (i + 1)) | 1;
            bool cov = false;

            std::set<unsigned int>::iterator it = allMcdcBitmapSetK->begin();
            for(; it != allMcdcBitmapSetK->end(); it++)
            {
                int v = *it ^ mask;

                if(allMcdcBitmapSetK->count(v))
                {
                    cov = true;
                    break;
                }
            }
            if(cov) {
                BitmapCov::mcdcResultBitmap[mcdcResId] = 1;
            }
            mcdcResId++;
        }
    }

    bool newCov = processBitmap(mcdcResultBitmap, mcdcResultBitmapLength, mcdcResultAllBitmap, mcdcCov);

    return newCov;
}*/

string BitmapCov::printCoverage()
{
    stringstream ss;

    ss << "Munit    : ";
    ss << std::setw(5) << std::right << BitmapCov::munitCov;
    ss << "/";
    ss << std::setw(5) << std::left << BitmapCov::munitBitmapLength;
    ss << "(";
    ss << std::setprecision(5) << (float)(BitmapCov::munitCov * 100) / BitmapCov::munitBitmapLength;
    ss << "%)";
    ss << std::endl;
    ss << "Branch   : ";
    ss << std::setw(5) << std::right << BitmapCov::branchCov;
    ss << "/";
    ss << std::setw(5) << std::left << BitmapCov::branchBitmapLength;
    ss << "(";
    ss << std::setprecision(5) << (float)(BitmapCov::branchCov * 100) / BitmapCov::branchBitmapLength;
    ss << "%)";
    ss << std::endl;
    ss << "Condition: ";
    ss << std::setw(5) << std::right << BitmapCov::condCov;
    ss << "/";
    ss << std::setw(5) << std::left << BitmapCov::condBitmapLength;
    ss << "(";
    ss << std::setprecision(5) << (float)(BitmapCov::condCov * 100) / BitmapCov::condBitmapLength;
    ss << "%)";
    ss << std::endl;
    ss << "Decision : ";
    ss << std::setw(5) << std::right << BitmapCov::decCov;
    ss << "/";
    ss << std::setw(5) << std::left << BitmapCov::decBitmapLength;
    ss << "(";
    ss << std::setprecision(5) << (float)(BitmapCov::decCov * 100) / BitmapCov::decBitmapLength;
    ss << "%)";
    ss << std::endl;
    ss << "MCDC     : ";
    ss << std::setw(5) << std::right << BitmapCov::mcdcCov;
    ss << "/";
    ss << std::setw(5) << std::left << BitmapCov::mcdcResultBitmapLength;
    ss << "(";
    ss << std::setprecision(5) << (float)(BitmapCov::mcdcCov * 100) / BitmapCov::mcdcResultBitmapLength;
    ss << "%)";
    ss << std::endl;

    
    cout << ss.str();

    return ss.str();
}

void BitmapCov::release()
{
    delete[] munitAllBitmap;
    delete[] branchAllBitmap;
    delete[] condAllBitmap;
    delete[] decAllBitmap;
    //DeleteMCDCBitmap(mcdcAllBitmap, BitmapCov::mcdcBitmapLength);
    delete[] mcdcResultAllBitmap;

    delete[] bitmapCovRecord.munitBitmap;
    delete[] bitmapCovRecord.branchBitmap;
    delete[] bitmapCovRecord.condBitmap;
    delete[] bitmapCovRecord.decBitmap;
    //DeleteMCDCBitmap(bitmapCovRecord.mcdcBitmap, BitmapCov::mcdcBitmapLength);
    delete[] bitmapCovRecord.mcdcResultBitmap;

    delete[] BitmapCov::munitBitmap;
    delete[] BitmapCov::branchBitmap;
    delete[] BitmapCov::condBitmap;
    delete[] BitmapCov::decBitmap;
    //delete[] BitmapCov::mcdcDecTemp;
    //DeleteMCDCBitmap(BitmapCov::mcdcBitmap, BitmapCov::mcdcBitmapLength);
    delete[] mcdcResultBitmap;

    delete[] BitmapCov::mcdcMeta;
    delete[] BitmapCov::mcdcResultBitmapOffset;
}

void BitmapCov::releaseRecord(BitmapCovRecord* record)
{
    delete[] record->munitBitmap;
    delete[] record->branchBitmap;
    delete[] record->condBitmap;
    delete[] record->decBitmap;
    //DeleteMCDCBitmap(record->mcdcBitmap, BitmapCov::mcdcBitmapLength);
    delete[] record->mcdcResultBitmap;
    delete record;
}

void BitmapCov::recordTotal()
{
    memcpy(bitmapCovRecord.munitBitmap, munitAllBitmap, BitmapCov::munitBitmapLength);
    memcpy(bitmapCovRecord.branchBitmap, branchAllBitmap, BitmapCov::branchBitmapLength);
    memcpy(bitmapCovRecord.condBitmap, condAllBitmap, BitmapCov::condBitmapLength);
    memcpy(bitmapCovRecord.decBitmap, decAllBitmap, BitmapCov::decBitmapLength);
    memcpy(bitmapCovRecord.mcdcResultBitmap, mcdcResultAllBitmap, BitmapCov::mcdcResultBitmapLength);
    //for (int k = 0; k < BitmapCov::mcdcBitmapLength; k++) {
    //    std::set<unsigned int>* mcdcBitmapSetK = BitmapCov::mcdcBitmap->at(k);
    //    std::set<unsigned int>* tempMcdcBitmapSetK = bitmapCovRecord.mcdcBitmap->at(k);
    //    tempMcdcBitmapSetK->insert(mcdcBitmapSetK->begin(), mcdcBitmapSetK->end());
    //}

    bitmapCovRecord.munitCov = munitCov;
    bitmapCovRecord.branchCov = branchCov;
    bitmapCovRecord.condCov = condCov;
    bitmapCovRecord.decCov = decCov;
    bitmapCovRecord.mcdcCov = mcdcCov;
    //totalMcdcRecordCov = totalMcdcCov;
}


void BitmapCov::setRecordTotalBitmap()
{
    
    memcpy(munitAllBitmap, bitmapCovRecord.munitBitmap, BitmapCov::munitBitmapLength);
    memcpy(branchAllBitmap, bitmapCovRecord.branchBitmap, BitmapCov::branchBitmapLength);
    memcpy(condAllBitmap, bitmapCovRecord.condBitmap, BitmapCov::condBitmapLength);
    memcpy(decAllBitmap, bitmapCovRecord.decBitmap, BitmapCov::decBitmapLength);
    memcpy(mcdcResultAllBitmap, bitmapCovRecord.mcdcResultBitmap, BitmapCov::mcdcResultBitmapLength);
    //for (int k = 0; k < BitmapCov::mcdcBitmapLength; k++) {
    //    std::set<unsigned int>* mcdcBitmapSetK = bitmapCovRecord.mcdcBitmap->at(k);
    //    std::set<unsigned int>* tempMcdcBitmapSetK = mcdcAllBitmap->at(k);
    //    tempMcdcBitmapSetK->clear();
    //    tempMcdcBitmapSetK->insert(mcdcBitmapSetK->begin(), mcdcBitmapSetK->end());
    //}

    munitCov = bitmapCovRecord.munitCov;
    branchCov = bitmapCovRecord.branchCov;
    condCov = bitmapCovRecord.condCov;
    decCov = bitmapCovRecord.decCov;
    mcdcCov = bitmapCovRecord.mcdcCov;
    //totalMcdcCov = totalMcdcRecordCov;

}


BitmapCov::BitmapCovRecord* BitmapCov::recordCurrent()
{
    BitmapCovRecord* bitmapCovRecordTemp = new BitmapCovRecord();

    bitmapCovRecordTemp->munitBitmap = new unsigned char[BitmapCov::munitBitmapLength];
    bitmapCovRecordTemp->branchBitmap = new unsigned char[BitmapCov::branchBitmapLength];
    bitmapCovRecordTemp->condBitmap = new unsigned char[BitmapCov::condBitmapLength];
    bitmapCovRecordTemp->decBitmap = new unsigned char[BitmapCov::decBitmapLength];
    //bitmapCovRecordTemp->mcdcBitmap = new std::vector<std::set<unsigned int>*>();
    //BitmapCov::CreateMCDCBitmap(bitmapCovRecordTemp->mcdcBitmap, BitmapCov::mcdcBitmapLength);
    bitmapCovRecordTemp->mcdcResultBitmap = new unsigned char[BitmapCov::mcdcResultBitmapLength];

    memcpy(bitmapCovRecordTemp->munitBitmap, munitBitmap, BitmapCov::munitBitmapLength);
    memcpy(bitmapCovRecordTemp->branchBitmap, branchBitmap, BitmapCov::branchBitmapLength);
    memcpy(bitmapCovRecordTemp->condBitmap, condBitmap, BitmapCov::condBitmapLength);
    memcpy(bitmapCovRecordTemp->decBitmap, decBitmap, BitmapCov::decBitmapLength);
    memcpy(bitmapCovRecordTemp->mcdcResultBitmap, mcdcResultBitmap, BitmapCov::mcdcResultBitmapLength);
    //for (int k = 0; k < BitmapCov::mcdcBitmapLength; k++) {
    //    std::set<unsigned int>* mcdcBitmapSetK = BitmapCov::mcdcBitmap->at(k);
    //    std::set<unsigned int>* tempMcdcBitmapSetK = bitmapCovRecordTemp->mcdcBitmap->at(k);
    //    tempMcdcBitmapSetK->insert(mcdcBitmapSetK->begin(), mcdcBitmapSetK->end());
    //}

    bitmapCovRecordTemp->munitCov = countBitmapCov(munitBitmap, BitmapCov::munitBitmapLength);
    bitmapCovRecordTemp->branchCov = countBitmapCov(branchBitmap, BitmapCov::branchBitmapLength);
    bitmapCovRecordTemp->condCov = countBitmapCov(condBitmap, BitmapCov::condBitmapLength);
    bitmapCovRecordTemp->decCov = countBitmapCov(decBitmap, BitmapCov::decBitmapLength);
    bitmapCovRecordTemp->mcdcCov = countBitmapCov(mcdcResultBitmap, BitmapCov::mcdcResultBitmapLength);

    return bitmapCovRecordTemp;
}

int BitmapCov::countBitmapCov(unsigned char* bitmap, int bitmapLen)
{
    int cov = 0;
    for (int k = 0; k < bitmapLen; k++) {
        if (bitmap[k] > 0) {
            cov++;
        }
    }
    return cov;
}

bool BitmapCov::setRecordCurrentBitmap(BitmapCovRecord* record)
{
    memcpy(munitBitmap, record->munitBitmap, BitmapCov::munitBitmapLength);
    memcpy(branchBitmap, record->branchBitmap, BitmapCov::branchBitmapLength);
    memcpy(condBitmap, record->condBitmap, BitmapCov::condBitmapLength);
    memcpy(decBitmap, record->decBitmap, BitmapCov::decBitmapLength);
    memcpy(mcdcResultBitmap, record->mcdcResultBitmap, BitmapCov::mcdcResultBitmapLength);
    //for (int k = 0; k < BitmapCov::mcdcBitmapLength; k++) {
    //    std::set<unsigned int>* mcdcBitmapSetK = record->mcdcBitmap->at(k);
    //    std::set<unsigned int>* tempMcdcBitmapSetK = mcdcBitmap->at(k);
    //    tempMcdcBitmapSetK->clear();
    //    tempMcdcBitmapSetK->insert(mcdcBitmapSetK->begin(), mcdcBitmapSetK->end());
    //}
    return true;
}

