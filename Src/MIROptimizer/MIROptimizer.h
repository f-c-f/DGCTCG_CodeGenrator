#pragma once
#include <string>
#include <vector>

#include "../ILGenerator/MIRSchedule.h"

class MIRModel;
class ILSchedule;
class ILGlobalVariable;
class ILParser;

class MIROptimizer
{
public:
	int optimize(MIRModel* model);
    

	enum {
		ErrorInvalidStatementType = 160001,
	};

	std::vector<std::string> errorStrList = {
		"ErrorInvalidStatementType,					      ",
	};
    

	std::string getErrorStr();
	static void setCurrentError(int errorCode, std::string msg);
private:
	static int errorCode;
	static std::string errorStr;

	int optimize();
	MIRModel* model = nullptr;
    
    MIRSchedule schedule;

	//对组件组合表达模式进行优化
	int optimizeActorParallelization();
    


};

