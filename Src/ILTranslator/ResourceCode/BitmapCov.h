#pragma once
#include <vector>
#include <string>
#include <map>
#include <set>

typedef void(*MCDCFunc)(int, unsigned char*);

class BitmapCov {
public:
    /*struct McdcBitmapResult {
        bool encNew;
        bool encRefineNew;
        std::set<unsigned int> encRefineOlds;
    };*/

    struct BitmapCovRecord {
           unsigned char* munitBitmap;
           unsigned char* branchBitmap;
           unsigned char* condBitmap;
           unsigned char* decBitmap;
           //std::vector<std::set<unsigned int>*>* mcdcBitmap;
           unsigned char* mcdcResultBitmap; //不能记录单个用例的覆盖，是多个用例累加的mcdc覆盖

           int munitCov;
           int branchCov;
           int condCov;
           int decCov;
           int mcdcCov;
       };

	static unsigned char* branchBitmap;
	static unsigned char* munitBitmap;
	static unsigned char* condBitmap;
	static unsigned char* decBitmap;
	//static std::vector<std::set<unsigned int>*>* mcdcBitmap;
    static unsigned char* mcdcResultBitmap;

	static int munitBitmapLength;
	static int branchBitmapLength;
	static int condBitmapLength;
	static int decBitmapLength;
    static int mcdcBitmapLength;
    static int mcdcResultBitmapLength;

    static unsigned int* mcdcResultBitmapOffset;

    static unsigned char* mcdcMeta;
	static unsigned int mcdcDecTemp;


    static unsigned char* munitAllBitmap;
    static unsigned char* branchAllBitmap;
    static unsigned char* condAllBitmap;
    static unsigned char* decAllBitmap;
    //static std::vector<std::set<unsigned int>*>* mcdcAllBitmap;
    static unsigned char* mcdcResultAllBitmap;
    static int munitCov;
    static int branchCov;
    static int condCov;
    static int decCov;
    static int mcdcCov;
    //static int totalMcdcCov;



    //static unsigned char* munitRecordBitmap;
    //static unsigned char* branchRecordBitmap;
    //static unsigned char* condRecordBitmap;
    //static unsigned char* decRecordBitmap;
    //static unsigned char* mcdcResultRecordBitmap;
    //static int munitRecordCov;
    //static int branchRecordCov;
    //static int condRecordCov;
    //static int decRecordCov;
    //static int mcdcRecordCov;
    ////static int totalMcdcRecordCov;

    static BitmapCovRecord bitmapCovRecord;

    static std::vector<MCDCFunc> mcdcFuncs;

    static void init();

	static bool CondRecord(bool res, int condIdx);
	static bool CondAndMCDCRecord(bool res, int condOrDecIdx, int decCondIdx, int decIdx);

    static void CreateMCDCBitmap(std::vector<std::set<unsigned int>*>* mcdcBitmap, int bitmapLen);
    static void ClearMCDCBitmap(std::vector<std::set<unsigned int>*>* mcdcBitmap, int bitmapLen);
    static void DeleteMCDCBitmap(std::vector<std::set<unsigned int>*>* mcdcBitmap, int bitmapLen);
    //static void CreateMCDCTestCaseIndexVector(std::vector<std::vector<unsigned int>*>* mcdcAllBitmapTestCaseIndex, int bitmapLen);
    //static void DeleteMCDCTestCaseIndexVector(std::vector<std::vector<unsigned int>*>* mcdcAllBitmapTestCaseIndex, int bitmapLen);

    static void clearCurrentBitmap();
    static void clearTotalBitmap();
    static bool step();
    static BitmapCovRecord* recordCurrent();
    static void recordTotal();
    static bool setRecordCurrentBitmap(BitmapCovRecord* record);
    static void setRecordTotalBitmap();

    static int countBitmapCov(unsigned char* bitmap, int bitmapLen);
    static bool processBitmap(unsigned char* bitmap, int bitmapLen, unsigned char* allBitmap, int& allCov);
    static bool processMCDCBitmap();


    //static McdcBitmapResult processMCDCBitmap(std::vector<std::set<unsigned int>*>* bitmap, int bitmapLen, std::vector<std::set<unsigned int>*>* allBitmap, std::vector< std::vector<unsigned int>*>* allBitmapTestCaseIndex, int currMcdcTestCaseIndex, unsigned int* allMcdcCov, unsigned char* mcdcMeta, int totalMcdcCov, int& allCov);

    //static int CountOneInBinary(unsigned int n);

    static std::string printCoverage();

    static void release();
    static void releaseRecord(BitmapCovRecord* record);
};

void registerMCDCFunctions();

