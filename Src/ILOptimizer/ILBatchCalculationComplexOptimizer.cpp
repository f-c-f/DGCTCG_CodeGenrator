#include "ILBatchCalculationComplexOptimizer.h"

#include <algorithm>
#include <unordered_map>
#include <math.h>


#include "ILOptimizer.h"

#include "../ILBasic/ILAnalyzeScheduleNode.h"
#include "ILBatchInstructionProvider.h"
#include "ILBatchCalculationGraph.h"

#include "../ILBasic/ILSchedule.h"
#include "../ILBasic/ILParser.h"
#include "../ILBasic/ILExpressionParser.h"
#include "../ILBasic/Expression.h"
#include "../ILBasic/StmtBatchCalculation.h"
#include "../ILBasic/StmtLocalVariable.h"
#include "../ILBasic/StmtFor.h"
#include "../ILBasic/StmtExpression.h"
#include "../Common/Utility.h"
#include "../ILBasic/ILFile.h"
#include "../ILBasic/ILHeadFile.h"
#include "../ILBasic/ILLocalVariable.h"
#include "../ILBasic/ILGlobalVariable.h"
#include "../ILBasic/ILCalculate.h"
#include "../ILBasic/ILExternFile.h"
#include "../ILBasic/ILExternVariable.h"
#include "../ILBasic/ILRef.h"

#include "ILBatchCalculationComplexUtility.h"

using namespace std;

namespace
{
    const int CONST_NUM_2 = 2;
    const int CONST_NUM_3 = 3;
    const int CONST_NUM_16 = 16;
}


int ILBatchCalculationComplexOptimizer::optimize(ILSchedule* schedule, ILParser* parser)
{
    this->schedule = schedule;
    this->iLParser = parser;

    int res = ops.collectSchedule(schedule, parser);
    if(res < 0)
        return res;
    
    res = optimize();
    if(res < 0)
        return res;
    
	return 1;
}


int ILBatchCalculationComplexOptimizer::insertCodeToStmtList(std::vector<Statement*>* stmtList, int insertIndex,
    std::string code)
{
    vector<string> expList = stringSplit(code, "\n");
    for(int i = 0; i < expList.size(); i++)
    {
        if(expList[i].empty())
            continue;
        StmtExpression* stmtNew = new StmtExpression();
        stmtNew->parentStatement = (*stmtList)[0]->parentStatement;
        ILExpressionParser expParser;
        stmtNew->expression = expParser.parseExpression(expList[i]);
        stmtList->insert(stmtList->begin() + insertIndex + i + 1, stmtNew);
    }
    return 1;
}

int ILBatchCalculationComplexOptimizer::optimize()
{
    const vector<ILAnalyzeScheduleNode*>* scheduleNodes = ops.getScheduleNodeList();
    vector<ILAnalyzeScheduleNode*> ret;
    for(int i = scheduleNodes->size() - 1; i >= 0; i--) {
        ILAnalyzeScheduleNode* scheduleNode = (*scheduleNodes)[i];
        if(scheduleNode->nodeType != ILAnalyzeScheduleNode::TypeStatement ||
            scheduleNode->statement->type != Statement::BatchCalculation) {
            continue;
        }
        StmtBatchCalculation* stmt = (StmtBatchCalculation*)scheduleNode->statement;
        if(stmt->operationType < StmtBatchCalculation::TypeDotMul) {
            continue;
        }
        if(stmt->operationType == StmtBatchCalculation::TypeFFT) {
            optimizeFFT(stmt);
        } else if(stmt->operationType == StmtBatchCalculation::TypeDCT) {
            optimizeDCT(stmt);
        } else if(stmt->operationType == StmtBatchCalculation::TypeConv) {
            optimizeConv(stmt);
        }
    }
	return 1;
}

int ILBatchCalculationComplexOptimizer::optimizeFFT(StmtBatchCalculation* stmt)
{
    if(stmt->inputs.size() != 1 || stmt->outputs.size() != 1 ||
        stmt->inputs[0].arraySize.size() != 1 || stmt->outputs[0].arraySize.size() != 1 ||
        stmt->inputs[0].arraySize[0] != stmt->outputs[0].arraySize[0]) 
    {
        return -ILOptimizer::ErrorInvalidFFTInputOrOutputSize;
    }
    vector<Statement*>* stmtList = stmt->parentStatement->getStatementList();
    int insertIndex = findIndexInVector(*stmtList, (Statement*)stmt);

    string stmtStr = "";

    int res = optimizeFFTGetBest(stmt, stmtStr);
    if(res < 0)
        return res;

    if(stmtStr.empty()) {
        return 0;
    }
    
    res = insertCodeToStmtList(stmtList, insertIndex, stmtStr);
    if(res < 0)
        return res;

    stmt->release();
    delete stmt;
    stmtList->erase(stmtList->begin() + insertIndex);
    
    return 1;
}

int ILBatchCalculationComplexOptimizer::optimizeFFTGetBest(StmtBatchCalculation* stmt, std::string& retStmtStr)
{
    int res;
    retStmtStr = "";
    int n = stmt->inputs[0].arraySize[0];
    string type = stmt->inputs[0].type;

    if(!(n & (n - 1))) //判断n是否为2的整数次幂
    {
        if(type == "f32")
        {
            res = optimizeFFTGetBest_rfft_1d_f32_Ooura8g(stmt, retStmtStr);
            if(res < 0)
            {
                return res;
            }
        }
    }
    return 0;
}

int ILBatchCalculationComplexOptimizer::optimizeFFTGetBest_rfft_1d_f32_Ooura8g(StmtBatchCalculation* stmt,
    std::string& retStmtStr)
{
    int res;

    int n = stmt->inputs[0].arraySize[0];
    string type = stmt->inputs[0].type;

    ILCalculate* iLCalculate = (ILCalculate*)stmt->parentStatement->parentILObject;

    retStmtStr = "rfft_1d_f32_Ooura8g(" + to_string(n) + ", ";
    retStmtStr += stmt->inputs[0].name + ", ";
    retStmtStr += stmt->outputs[0].name + ", ";
    retStmtStr += iLCalculate->name + "_ip" + ", ";
    retStmtStr += iLCalculate->name + "_w" + ");\n";

    
    ILFile* containerILFile = iLCalculate->getContainerILFile();
    if(!containerILFile)
        return ILOptimizer::ErrorInternal;


    ILExternVariable* externVarIp = new ILExternVariable();
    externVarIp->parent = containerILFile;
    externVarIp->name = iLCalculate->name + "_ip";
    externVarIp->arraySize.push_back(2 + (int)sqrt(n / 2));
    externVarIp->type = "i32";
    for(int i = 0; i < iLCalculate->refs.size(); i++)
    {
        externVarIp->refs.push_back(iLCalculate->refs[i]->clone(externVarIp));
    }
    containerILFile->externVariables.push_back(externVarIp);

    ILExternVariable* externVarW = new ILExternVariable();
    externVarW->parent = containerILFile;
    externVarW->name = iLCalculate->name + "_w";
    externVarW->arraySize.push_back(n / 2);
    externVarW->type = type;
    for(int i = 0; i < iLCalculate->refs.size(); i++)
    {
        externVarW->refs.push_back(iLCalculate->refs[i]->clone(externVarW));
    }
    containerILFile->externVariables.push_back(externVarW);

    ILExternFile* externFileImplHeadFile = new ILExternFile();
    externFileImplHeadFile->parent = nullptr;
    externFileImplHeadFile->name = "rfft_1d_f32_Ooura8g.h";
    iLParser->externFiles.push_back(externFileImplHeadFile);
    
    ILExternFile* externFileImplSourceFile = new ILExternFile();
    externFileImplSourceFile->parent = nullptr;
    externFileImplSourceFile->name = "rfft_1d_f32_Ooura8g.c";
    iLParser->externFiles.push_back(externFileImplSourceFile);
    
    ILExternFile* externFileImplDataFile = new ILExternFile();
    externFileImplDataFile->parent = nullptr;
    externFileImplDataFile->name = iLCalculate->name + "_data.c";
    iLParser->externFiles.push_back(externFileImplDataFile);

    res = FFT::rfft_1d_f32_Ooura8g::generateDataFile(type, n, iLCalculate->name, iLCalculate->name + "_data.c");
    if(res < 0)
        return res;
    return 1;
}


int ILBatchCalculationComplexOptimizer::optimizeDCT(StmtBatchCalculation* stmt)
{
    if(stmt->inputs.size() != 1 || stmt->outputs.size() != 1 ||
        stmt->inputs[0].arraySize.size() != 1 || stmt->outputs[0].arraySize.size() != 1 ||
        stmt->inputs[0].arraySize[0] != stmt->outputs[0].arraySize[0]) 
    {
        return -ILOptimizer::ErrorInvalidDCTInputOrOutputSize;
    }
    vector<Statement*>* stmtList = stmt->parentStatement->getStatementList();
    int insertIndex = findIndexInVector(*stmtList, (Statement*)stmt);

    string stmtStr = "";

    int res = optimizeDCTGetBest(stmt, stmtStr);
    if(res < 0)
        return res;

    if(stmtStr.empty()) {
        return 0;
    }
    
    res = insertCodeToStmtList(stmtList, insertIndex, stmtStr);
    if(res < 0)
        return res;

    stmt->release();
    delete stmt;
    stmtList->erase(stmtList->begin() + insertIndex);
    
    return 1;
}

int ILBatchCalculationComplexOptimizer::optimizeDCTGetBest(StmtBatchCalculation* stmt, std::string& retStmtStr)
{
    int res;
    retStmtStr = "";
    int n = stmt->inputs[0].arraySize[0];
    string type = stmt->inputs[0].type;

    if(!(n & (n - 1))) //判断n是否为2的整数次幂
    {
        if(type == "f32")
        {
            res = optimizeDCTGetBest_dct_1d_f32_Ooura8g(stmt, retStmtStr);
            if(res < 0)
            {
                return res;
            }
        }
    }
    return 0;
}

int ILBatchCalculationComplexOptimizer::optimizeDCTGetBest_dct_1d_f32_Ooura8g(StmtBatchCalculation* stmt,
    std::string& retStmtStr)
{
    int res;

    int n = stmt->inputs[0].arraySize[0];
    string type = stmt->inputs[0].type;

    ILCalculate* iLCalculate = (ILCalculate*)stmt->parentStatement->parentILObject;

    retStmtStr = "dct_1d_f32_Ooura8g(" + to_string(n) + ", ";
    retStmtStr += stmt->inputs[0].name + ", ";
    retStmtStr += stmt->outputs[0].name + ", ";
    retStmtStr += iLCalculate->name + "_ip" + ", ";
    retStmtStr += iLCalculate->name + "_w" + ");\n";

    
    ILFile* containerILFile = iLCalculate->getContainerILFile();
    if(!containerILFile)
        return ILOptimizer::ErrorInternal;


    ILExternVariable* externVarIp = new ILExternVariable();
    externVarIp->parent = containerILFile;
    externVarIp->name = iLCalculate->name + "_ip";
    externVarIp->arraySize.push_back(2 + (int)sqrt(n / 2));
    externVarIp->type = "i32";
    for(int i = 0; i < iLCalculate->refs.size(); i++)
    {
        externVarIp->refs.push_back(iLCalculate->refs[i]->clone(externVarIp));
    }
    containerILFile->externVariables.push_back(externVarIp);

    ILExternVariable* externVarW = new ILExternVariable();
    externVarW->parent = containerILFile;
    externVarW->name = iLCalculate->name + "_w";
    externVarW->arraySize.push_back(n / 2);
    externVarW->type = type;
    for(int i = 0; i < iLCalculate->refs.size(); i++)
    {
        externVarW->refs.push_back(iLCalculate->refs[i]->clone(externVarW));
    }
    containerILFile->externVariables.push_back(externVarW);

    ILExternFile* externFileImplHeadFile = new ILExternFile();
    externFileImplHeadFile->parent = nullptr;
    externFileImplHeadFile->name = "dct_1d_f32_Ooura8g.h";
    iLParser->externFiles.push_back(externFileImplHeadFile);
    
    ILExternFile* externFileImplSourceFile = new ILExternFile();
    externFileImplSourceFile->parent = nullptr;
    externFileImplSourceFile->name = "dct_1d_f32_Ooura8g.c";
    iLParser->externFiles.push_back(externFileImplSourceFile);
    
    ILExternFile* externFileImplDataFile = new ILExternFile();
    externFileImplDataFile->parent = nullptr;
    externFileImplDataFile->name = iLCalculate->name + "_data.c";
    iLParser->externFiles.push_back(externFileImplDataFile);

    res = DCT::dct_1d_f32_Ooura8g::generateDataFile(type, n, iLCalculate->name, iLCalculate->name + "_data.c");
    if(res < 0)
        return res;
    return 1;
}



int ILBatchCalculationComplexOptimizer::optimizeConv(StmtBatchCalculation* stmt)
{
    if(stmt->inputs.size() != 2 || stmt->outputs.size() != 1 ||
        stmt->inputs[0].arraySize.size() != 1 || 
        stmt->inputs[1].arraySize.size() != 1 || 
        stmt->outputs[0].arraySize.size() != 1 ||
        stmt->inputs[0].arraySize[0] + (int)(stmt->inputs[1].arraySize[0] / 2) * 2 != stmt->outputs[0].arraySize[0]) 
    {
        return -ILOptimizer::ErrorInvalidConvInputOrOutputSize;
    }
    vector<Statement*>* stmtList = stmt->parentStatement->getStatementList();
    int insertIndex = findIndexInVector(*stmtList, (Statement*)stmt);

    string stmtStr = "";

    int res = optimizeConvGetBest(stmt, stmtStr);
    if(res < 0)
        return res;

    if(stmtStr.empty()) {
        return 0;
    }
    
    res = insertCodeToStmtList(stmtList, insertIndex, stmtStr);
    if(res < 0)
        return res;

    stmt->release();
    delete stmt;
    stmtList->erase(stmtList->begin() + insertIndex);
    
    return 1;
}

int ILBatchCalculationComplexOptimizer::optimizeConvGetBest(StmtBatchCalculation* stmt, std::string& retStmtStr)
{
    int res = 0;
    retStmtStr = "";
    int n = stmt->inputs[0].arraySize[0];
    string type = stmt->inputs[0].type;

    if(!(n & (n - 1))) //判断n是否为2的整数次幂
    {
        if(type == "f32")
        {
            if(ILOptimizer::configTargetDevice == "Intel")
                res = optimizeConvGetBest_conv_1d_f32_Intel(stmt, retStmtStr);
            else if(ILOptimizer::configTargetDevice == "ARMv8")
                res = optimizeConvGetBest_conv_1d_f32_ARMv8(stmt, retStmtStr);
            if(res < 0) {
                return res;
            }
        }
    }
    return 0;
}

int ILBatchCalculationComplexOptimizer::optimizeConvGetBest_conv_1d_f32_Intel(StmtBatchCalculation* stmt,
    std::string& retStmtStr)
{
    int res;

    int n = stmt->inputs[0].arraySize[0];
    string type = stmt->inputs[0].type;
    int kernelSize = stmt->inputs[1].arraySize[0];

    ILCalculate* iLCalculate = (ILCalculate*)stmt->parentStatement->parentILObject;

    retStmtStr = "conv_1d_f32_Intel(";
    retStmtStr += to_string(n) + ", ";
    retStmtStr += stmt->inputs[0].name + ", ";
    retStmtStr += to_string(kernelSize) + ", ";
    retStmtStr += stmt->inputs[1].name + ", ";
    retStmtStr += stmt->outputs[0].name + ");\n";

    
    ILFile* containerILFile = iLCalculate->getContainerILFile();
    if(!containerILFile)
        return ILOptimizer::ErrorInternal;

    
    ILExternFile* externFileImplHeadFile = new ILExternFile();
    externFileImplHeadFile->parent = nullptr;
    externFileImplHeadFile->name = "conv_1d_f32_Intel.h";
    iLParser->externFiles.push_back(externFileImplHeadFile);
    
    ILExternFile* externFileImplSourceFile = new ILExternFile();
    externFileImplSourceFile->parent = nullptr;
    externFileImplSourceFile->name = "conv_1d_f32_Intel.c";
    iLParser->externFiles.push_back(externFileImplSourceFile);
    
    return 1;
}



int ILBatchCalculationComplexOptimizer::optimizeConvGetBest_conv_1d_f32_ARMv8(StmtBatchCalculation* stmt,
                                                                              std::string& retStmtStr)
{
    int res;

    int n = stmt->inputs[0].arraySize[0];
    string type = stmt->inputs[0].type;
    int kernelSize = stmt->inputs[1].arraySize[0];

    ILCalculate* iLCalculate = (ILCalculate*)stmt->parentStatement->parentILObject;

    retStmtStr = "conv_1d_f32_ARMv8(";
    retStmtStr += to_string(n) + ", ";
    retStmtStr += stmt->inputs[0].name + ", ";
    retStmtStr += to_string(kernelSize) + ", ";
    retStmtStr += stmt->inputs[1].name + ", ";
    retStmtStr += stmt->outputs[0].name + ");\n";

    
    ILFile* containerILFile = iLCalculate->getContainerILFile();
    if(!containerILFile)
        return ILOptimizer::ErrorInternal;

    
    ILExternFile* externFileImplHeadFile = new ILExternFile();
    externFileImplHeadFile->parent = nullptr;
    externFileImplHeadFile->name = "conv_1d_f32_ARMv8.h";
    iLParser->externFiles.push_back(externFileImplHeadFile);
    
    ILExternFile* externFileImplSourceFile = new ILExternFile();
    externFileImplSourceFile->parent = nullptr;
    externFileImplSourceFile->name = "conv_1d_f32_ARMv8.c";
    iLParser->externFiles.push_back(externFileImplSourceFile);
    
    return 1;
}

