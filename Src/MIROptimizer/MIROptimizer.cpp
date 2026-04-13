#include "MIROptimizer.h"


#include "MIRActorParallelizationOptimizer.h"
#include "../Common/Utility.h"
#include "../MIRBasic/MIRModel.h"
#include "../MIRBasic/MIRFunction.h"
#include "..\ILGenerator\MIRScheduleAnalyzer.h"


using namespace std;


int MIROptimizer::errorCode = 0;
string MIROptimizer::errorStr;

int MIROptimizer::optimize(MIRModel* model)
{
	this->model = model;
	int res = this->optimize();
	if(res < 0)
	{
		MIROptimizer::setCurrentError(res, MIROptimizer::errorStr);
		return res;
	}
	return 1;
}


string MIROptimizer::getErrorStr()
{
	string ret = "MIROptimizer\nError Code: " + to_string(errorCode) + "\n";
	
	ret += "\n";

	int errorCodeIndex = -errorCode - MIROptimizer::ErrorInvalidStatementType;

	if(errorCodeIndex >= 0 && errorCodeIndex < errorStrList.size())
		ret += errorStrList[errorCodeIndex];
	else
		ret += "Unknow error code.";

	ret += "\n";

	ret += "Message: " + this->errorStr;

	return ret;
}

void MIROptimizer::setCurrentError(int errorCode, string msg)
{
	MIROptimizer::errorCode = errorCode;
	MIROptimizer::errorStr = msg;
}

int MIROptimizer::optimize()
{
    string exePath = getExeDirPath();
    if(!isFolderExist(exePath + "/temp/"))
        createFolder(exePath + "/temp");

    int res;

    
    MIRScheduleAnalyzer scheduleAnalyzer;
    res = scheduleAnalyzer.scheduleAnalyze(model, &schedule);
    if(res < 0)
	{
		cout << "Error: " << res << endl;
		cout << scheduleAnalyzer.getErrorStr() << endl;
		return res;
	}



	cout << "Start to actor parallelization...\n";
	//组件并行化优化
	res = optimizeActorParallelization();
	if(res < 0)
		return res;
    

	return 1;
}

int MIROptimizer::optimizeActorParallelization()
{
    int res;
    for(int i = 0; i < model->functions.size(); i++)
    {
        MIRFunction* function = model->functions[i];
        if(function->type != MIRFunction::TypeDataflow)
            continue;
        MIRActorParallelizationOptimizer optimizer;
        res = optimizer.optimize((MIRFunctionDataflow*)model->functions[i], model);
        if(res < 0)
            return res;
    }

	return 1;
}
