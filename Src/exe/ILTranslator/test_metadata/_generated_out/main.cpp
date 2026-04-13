#include "define.h"
#include <stdio.h>
#include <vector>
#include <set>
#include <string>
#include <string.h>
#include <time.h>
#include "Utility.h"
#include "BitmapCov.h"
#include "TCGHybridBasic.h"
int main(int argc, char *argv[]) {
    clock_t testStartTime = clock();
    clock_t testEndTime = clock();
    // TCG Hybrid init code
    BitmapCov::munitBitmapLength = 43;
    BitmapCov::branchBitmapLength = 563;
    BitmapCov::condBitmapLength = 314;
    BitmapCov::decBitmapLength = 310;
    BitmapCov::mcdcBitmapLength = 155;
    static const unsigned char _mcdcMetaInit[] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, };
    BitmapCov::mcdcMeta = new unsigned char[BitmapCov::mcdcBitmapLength];
    memcpy(BitmapCov::mcdcMeta, _mcdcMetaInit, BitmapCov::mcdcBitmapLength);
    BitmapCov::init();
    TCGHybrid::initTCGHybrid(argc, argv);
    // init code
    int32_T init_DynamicCompressionModel_U_Table_Values;
    boolean_T init_DynamicCompressionModel_U_In2;
    int32_T init_DynamicCompressionModel_U_In3;
    boolean_T init_DynamicCompressionModel_U_Table_Values_e;
    int32_T init_DynamicCompressionModel_Y_proc_sig;
    DynamicCompressionModel_init(&DynamicCompressionModel_instance, &init_DynamicCompressionModel_U_Table_Values, &init_DynamicCompressionModel_U_In2, &init_DynamicCompressionModel_U_In3, &init_DynamicCompressionModel_U_Table_Values_e, &init_DynamicCompressionModel_Y_proc_sig);
    TCGHybrid::initTCGHybrid2();
    int i;
    for(i = 0; testEndTime - testStartTime < TCGHybrid::paraMaxRunTime; i++)
    {
        if (!TCGHybrid::stepTCGHybrid())
        {
            break;
        }
        testEndTime = clock();
    }
    cout << "Achieved: " << BitmapCov::branchCov << "/" << BitmapCov::branchBitmapLength;
    testEndTime = clock();
    long long testCostTime = (testEndTime - testStartTime);
    cout << " run " << i << " times, cost : " << testCostTime << " ms\n";
    TCGHybrid::terminateTCGHybrid();
    return 0;
}
bool tryStateBranch(char* data, int length)
{
    // exec code
    bool enoughData = false;
    if(data && length >= TCGHybrid::inputsDataLengthTotal) {
        enoughData = true;
    }
    int32_T exec_DynamicCompressionModel_U_Table_Values = {};
    boolean_T exec_DynamicCompressionModel_U_In2 = {};
    int32_T exec_DynamicCompressionModel_U_In3 = {};
    boolean_T exec_DynamicCompressionModel_U_Table_Values_e = {};
    int32_T exec_DynamicCompressionModel_Y_proc_sig;
    if(enoughData){
        memcpy(&exec_DynamicCompressionModel_U_Table_Values, data + 0, 4);
        memcpy(&exec_DynamicCompressionModel_U_In2, data + 4, 1);
        memcpy(&exec_DynamicCompressionModel_U_In3, data + 5, 4);
        memcpy(&exec_DynamicCompressionModel_U_Table_Values_e, data + 9, 1);
    }
    DynamicCompressionModel_step(&DynamicCompressionModel_instance, exec_DynamicCompressionModel_U_Table_Values, exec_DynamicCompressionModel_U_In2, exec_DynamicCompressionModel_U_In3, exec_DynamicCompressionModel_U_Table_Values_e, &exec_DynamicCompressionModel_Y_proc_sig);
    return true;
}