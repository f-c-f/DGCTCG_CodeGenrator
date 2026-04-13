#include "ILOptimizer.h"
#include "ILScheduleOptimizer.h"
#include "ILUnreachableBranchOptimizer.h"
#include "ILTautologyBranchOptimizer.h"
#include "ILBranchTypeOptimizer.h"
#include "ILActorPatternOptimizer.h"
#include "ILBatchCalculationComplexOptimizer.h"
#include "ILBatchCalculationOptimizer.h"
#include "ILActorPatternDataTruncator.h"
#include "ILHardwareOptimizer.h"

//#include "ILActorPatternForIterator.h"


#include "../Common/Utility.h"
#include "../ILBasic/ILFile.h"
#include "../ILBasic/ILParser.h"
#include "../MIRBasic/MIRModel.h"
#include "../ILBasic/ILFunction.h"
#include "ILActorPatternIterator.h"
#include "ILBatchFoldingOptimizer.h"

using namespace std;


string ILOptimizer::configTargetDevice;
bool ILOptimizer::configVariableFolding = false;
bool ILOptimizer::configExpressionSimplification = false;
bool ILOptimizer::configCodeBlockReduction = false;
bool ILOptimizer::configSIMD = false;
bool ILOptimizer::configDataTruncation = false;
bool ILOptimizer::configLoopReshaping = false;
bool ILOptimizer::configArrayFolding = false;

vector<ILGlobalVariable*> ILOptimizer::globalVariables;

int ILOptimizer::errorCode = 0;
string ILOptimizer::errorStr;

int ILOptimizer::optimize(ILParser* iLParser, MIRModel* mirModel)
{
	this->iLParser = iLParser;
	this->mirModel = mirModel;
	int res = this->optimize();
	if(res < 0)
	{
		ILOptimizer::setCurrentError(res, ILOptimizer::errorStr);
		return res;
	}
	return 1;
}

void ILOptimizer::setConfigTargetDevice(std::string value)
{
    configTargetDevice = value;
    //string targetDeviceFile = getExeDirPath() + "/BatchInstructionProvider-" + configTargetDevice + ".txt";
    //if(!isFileExist(targetDeviceFile))
    //{
    //    configTargetDevice = "";
    //}
}

void ILOptimizer::setConfigVariableFolding(bool value)
{
    configVariableFolding = value;
}

void ILOptimizer::setConfigExpressionSimplification(bool value)
{
    configExpressionSimplification = value;
}

void ILOptimizer::setConfigCodeBlockReduction(bool value)
{
    configCodeBlockReduction = value;
}

void ILOptimizer::setConfigSIMD(bool value)
{
	configSIMD = value;
}

void ILOptimizer::setConfigDataTruncation(bool value)
{
	configDataTruncation = value;
}

void ILOptimizer::setConfigLoopReshaping(bool value)
{
	configLoopReshaping = value;
}

void ILOptimizer::setConfigArrayFolding(bool value)
{
    configArrayFolding = value;
}

string ILOptimizer::getErrorStr()
{
	string ret = "ILOptimizer\nError Code: " + to_string(errorCode) + "\n";
	
	ret += "\n";

	int errorCodeIndex = -errorCode - ILOptimizer::ErrorInvalidStatementType;

	if(errorCodeIndex >= 0 && errorCodeIndex < errorStrList.size())
		ret += errorStrList[errorCodeIndex];
	else
		ret += "Unknow error code.";

	ret += "\n";

	ret += "Message: " + this->errorStr;

	return ret;
}

void ILOptimizer::setCurrentError(int errorCode, string msg)
{
	ILOptimizer::errorCode = errorCode;
	ILOptimizer::errorStr = msg;
}

int ILOptimizer::optimize() const
{
    string exePath = getExeDirPath();
    if(!isFolderExist(exePath + "/temp/"))
        createFolder(exePath + "/temp");

    int res;

	// 测试用途
	//for (int i = 0; i < iLParser->files.size(); i++)
	//{
	//	ILFile* ilFile = iLParser->files[i];
	//	for (int j = 0; j < ilFile->functions.size(); j++)
	//	{
	//		ILActorPatternIterator forIterator;
	//		ILFunction* ilFunction = ilFile->functions[j];
	//		int res = forIterator.optimize(ilFunction->schedule, iLParser, this->mirModel);
	//		if (res < 0)
	//			return res;
	//	}
	//}


	//cout << "Start to optimize the actor pattern...\n";
	////对组件组合模式进行优化
	//res = optimizeActorPattern();
	//if(res < 0)
	//	return res;

    if(ILOptimizer::configCodeBlockReduction) {
	    cout << "Start to optimize the unreachable branches...\n";
	    //对不可达分支进行优化
	    res = optimizeUnreachableBranch();
	    if(res < 0)
		    return res;
        
        cout << "Start to optimize the branch types...\n";
	    //对经过优化后的分支进行分支类型转化
	    //比如if(cond){空}else{...} 转化为 if(!cond){...}
	    res = optimizeBranchType();
	    if(res < 0)
		    return res;

	    cout << "Start to optimize the branches with tautology condition...\n";
	    //对永真分支进行优化
	    res = optimizeTautologyBranch();
	    if(res < 0)
		    return res;
    }
    
	res = colletGlobalVariable();
	if(res < 0)
		return res;

	if (ILOptimizer::configTargetDevice!="")
	//硬件指令优化
	{
		cout << "Start to use hardware optimization...\n";
		cout << "Target Device: " << configTargetDevice << endl;
		
		res = optimizeHardwareInstruction();
		if (res < 0)
			return res;
	}
    if (ILOptimizer::configArrayFolding)
    {
        cout << "Start to optimize the array folding...\n";
        //对函数中的调度块进行优化
        res = optimizeArrayFolding();
        if (res < 0)
            return res;

    }

	if (ILOptimizer::configSIMD)
	{
		cout << "Start to optimize the batch calculation...\n";
		//对函数中的批量计算语句进行优化
		res = optimizeBatchCalculation();
		if (res < 0)
			return res;

		cout << "Start to optimize the complex batch calculation...\n";
		//对函数中的复杂批量计算语句进行优化
		res = optimizeBatchCalculationComplex();
		if (res < 0)
			return res;
	}


    
    if(ILOptimizer::configVariableFolding || ILOptimizer::configExpressionSimplification) {
	    cout << "Start to optimize the schedule logic...\n";
	    //对函数中的调度块进行优化
	    res = optimizeScheduleLogic();
	    if(res < 0)
		    return res;
    }
    
    if(ILOptimizer::configCodeBlockReduction) {
	    cout << "Start to optimize the branch types...\n";
	    //对经过优化后的分支进行分支类型转化
	    //比如if(cond){空}else{...} 转化为 if(!cond){...}
	    res = optimizeBranchType();
	    if(res < 0)
		    return res;
    }

	if (ILOptimizer::configDataTruncation)
	{
		cout << "Start to optimize the actors that require data truncation...\n";
		res = optimizeDataTruncation();
		if (res < 0)
			return res;
	}

	if (ILOptimizer::configLoopReshaping)
	{
		cout << "Start to optimize the actors that require loop reshaping...\n";
		res = optimizeIterator();
		if (res < 0)
			return res;
	}
	return 1;
}

int ILOptimizer::optimizeDataTruncation() const
{
	for (int i = 0; i < iLParser->files.size(); i++)
	{
		ILFile* ilFile = iLParser->files[i];
		for (int j = 0; j < ilFile->functions.size(); j++)
		{
			if (ilFile->functions[j]->type == ILFunction::FunctionExec)
			{
				ILActorPatternDataTruncator iLActorPatternDataTruncator;
				ILFunction* ilFunction = ilFile->functions[j];
				int res = iLActorPatternDataTruncator.optimize(ilFunction->schedule, iLParser, this->mirModel);
				if (res < 0)
					return res;
			}
		}
	}
	return 1;
}

int ILOptimizer::optimizeIterator() const
{
	for (int i = 0; i < iLParser->files.size(); i++)
	{
		ILFile* ilFile = iLParser->files[i];
		for (int j = 0; j < ilFile->functions.size(); j++)
		{
			ILActorPatternIterator forIterator;
			ILFunction* ilFunction = ilFile->functions[j];
			int res = forIterator.optimize(ilFunction->schedule, iLParser, this->mirModel);
			if (res < 0)
				return res;
		}
	}
	return 0;
}

int ILOptimizer::optimizeActorPattern() const
{
	for(int i = 0; i< iLParser->files.size();i++)
	{
		ILFile* ilFile = iLParser->files[i];
		for(int j = 0; j< ilFile->functions.size();j++)
		{
			ILActorPatternOptimizer iLActorPatternOptimizer;
			ILFunction* ilFunction = ilFile->functions[j];
			int res = iLActorPatternOptimizer.optimize(ilFunction->schedule, iLParser, this->mirModel);
			if(res < 0)
				return res;
		}
	}
	return 1;
}


int ILOptimizer::optimizeUnreachableBranch() const
{
	ILUnreachableBranchOptimizer iLUnreachableBranchOptimizer;
	int res = iLUnreachableBranchOptimizer.optimize(iLParser);
	if(res < 0)
		return res;
	return 1;
}

int ILOptimizer::optimizeTautologyBranch() const
{
	ILTautologyBranchOptimizer iLTautologyBranchOptimizer;
	int res = iLTautologyBranchOptimizer.optimize(iLParser);
	if(res < 0)
		return res;
	return 1;
}

int ILOptimizer::optimizeBatchCalculation() const
{
	for(int i = 0; i< iLParser->files.size();i++)
	{
		ILFile* ilFile = iLParser->files[i];
		for(int j = 0; j< ilFile->functions.size();j++)
		{
			ILFunction* ilFunction = ilFile->functions[j];

			ILBatchCalculationOptimizer iLBatchCalculationOptimizer;
	        int res = iLBatchCalculationOptimizer.optimize(ilFunction->schedule, iLParser);
	        if(res < 0)
		        return res;
		}
	}
	return 1;
}

int ILOptimizer::optimizeHardwareInstruction() const
{
	for (int i = 0; i < iLParser->files.size(); i++)
	{
		ILFile* ilFile = iLParser->files[i];
		for (int j = 0; j < ilFile->functions.size(); j++)
		{
			ILFunction* ilFunction = ilFile->functions[j];
			if (ilFunction->type == ILFunction::FunctionExec)
			{
				ILHardwareOptimizer iLHardwareOptimizer;
				int res = iLHardwareOptimizer.optimize(ilFunction->schedule, iLParser, this->mirModel, configTargetDevice);
				if (res < 0)
					return res;
			}
		}
	}
	return 1;
}

int ILOptimizer::optimizeBatchCalculationComplex() const
{
    for(int i = 0; i< iLParser->files.size();i++)
	{
		ILFile* ilFile = iLParser->files[i];
		for(int j = 0; j< ilFile->functions.size();j++)
		{
			ILFunction* ilFunction = ilFile->functions[j];

			ILBatchCalculationComplexOptimizer iLBatchCalculationComplexOptimizer;
	        int res = iLBatchCalculationComplexOptimizer.optimize(ilFunction->schedule, iLParser);
	        if(res < 0)
		        return res;
		}
	}
	return 1;
}

int ILOptimizer::optimizeScheduleLogic() const
{
	for(int i = 0; i< iLParser->files.size();i++)
	{
		ILFile* ilFile = iLParser->files[i];
		for(int j = 0; j< ilFile->functions.size();j++)
		{
			ILFunction* ilFunction = ilFile->functions[j];
			int res = optimizeSchedule(ilFunction->schedule);
			if(res < 0)
				return res;
		}
	}
	return 1;
}

int ILOptimizer::colletGlobalVariable() const
{
	for(int i = 0; i< iLParser->files.size();i++)
	{
		ILFile* ilFile = iLParser->files[i];
		for(int j = 0; j< ilFile->globalVariables.size();j++)
		{
			this->globalVariables.push_back(ilFile->globalVariables[j]);
		}
	}
	return 1;
}

int ILOptimizer::optimizeSchedule(ILSchedule* schedule) const
{
	ILScheduleOptimizer iLScheduleOptimizer;
	int res = iLScheduleOptimizer.optimize(schedule, iLParser);
	if(res < 0)
		return res;
	return 1;
}



int ILOptimizer::optimizeBranchType() const
{
	ILBranchTypeOptimizer iLBranchTypeOptimizer;
	int res = iLBranchTypeOptimizer.optimize(iLParser);
	if(res < 0)
		return res;
	return 1;
}

int ILOptimizer::optimizeArrayFolding() const
{
    for (int i = 0; i < iLParser->files.size(); i++)
    {
        ILFile* ilFile = iLParser->files[i];
        for (int j = 0; j < ilFile->functions.size(); j++)
        {
            ILFunction* ilFunction = ilFile->functions[j];

            ILBatchFoldingOptimizer iLBatchFoldingOptimizer;
            int res = iLBatchFoldingOptimizer.optimize(ilFunction->schedule, iLParser);
            if (res < 0)
                return res;
        }
    }
    return 1;
}
