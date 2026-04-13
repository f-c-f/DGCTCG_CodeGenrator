#pragma once


#include "../ILBasic/ILAnalyzeSchedule.h"

class StmtBatchCalculation;
class ILBatchCalculationGraphNodeInput;
class ILBatchCalculationGraphNodeParameter;
class StmtLocalVariable;
class ILBatchInstruction;
class StmtFor;
class ILBatchCalculationGraphNode;
class ILBatchCalculationGraph;
class ILRef;
class Statement;
class ILBranch;
class ILCalculate;
class Expression;
class ILAnalyzeScheduleNode;
class ILParser;
class ILSchedule;
class ILAnalyzeScheduleNodeVariableRef;


class ILBatchCalculationComplexOptimizer
{
public:
	int optimize(ILSchedule* schedule, ILParser* parser);
    
private:
	ILSchedule* schedule = nullptr;
	ILParser* iLParser = nullptr;

    ILAnalyzeSchedule ops;

	int optimize();

	int optimizeFFT(StmtBatchCalculation* stmt);
	int optimizeFFTGetBest(StmtBatchCalculation* stmt, std::string& retStmtStr);
	int optimizeFFTGetBest_rfft_1d_f32_Ooura8g(StmtBatchCalculation* stmt, std::string& retStmtStr);

    
	int optimizeDCT(StmtBatchCalculation* stmt);
	int optimizeDCTGetBest(StmtBatchCalculation* stmt, std::string& retStmtStr);
	int optimizeDCTGetBest_dct_1d_f32_Ooura8g(StmtBatchCalculation* stmt, std::string& retStmtStr);

    
	int optimizeConv(StmtBatchCalculation* stmt);
	int optimizeConvGetBest(StmtBatchCalculation* stmt, std::string& retStmtStr);
	int optimizeConvGetBest_conv_1d_f32_ARMv8(StmtBatchCalculation* stmt, std::string& retStmtStr);
	int optimizeConvGetBest_conv_1d_f32_Intel(StmtBatchCalculation* stmt, std::string& retStmtStr);


    int insertCodeToStmtList(std::vector<Statement*>* stmtList, int insertIndex, std::string code);
};
