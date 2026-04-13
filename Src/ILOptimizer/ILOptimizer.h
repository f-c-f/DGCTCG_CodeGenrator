#pragma once
#include <string>
#include <vector>


class ILSchedule;
class ILGlobalVariable;
class ILParser;
class MIRModel;

class ILOptimizer
{
public:
	int optimize(ILParser* iLParser, MIRModel* mirModel);

    static void setConfigTargetDevice(std::string value);
    static std::string configTargetDevice;
    static bool configVariableFolding;
    static void setConfigVariableFolding(bool value);
    static bool configExpressionSimplification;
    static void setConfigExpressionSimplification(bool value);
    static bool configCodeBlockReduction;
    static void setConfigCodeBlockReduction(bool value);
	static bool configSIMD;
	static void setConfigSIMD(bool value);
	static bool configDataTruncation;
	static void setConfigDataTruncation(bool value);
	static bool configLoopReshaping;
	static void setConfigLoopReshaping(bool value);
    static bool configArrayFolding;
    static void setConfigArrayFolding(bool value);


	enum {
		ErrorInvalidStatementType = 40001,
		ErrorInvalidILBranchType,
		ErrorCreateExpNode,
		ErrorVariableInExpressionCanNotFindReference,
		ErrorVariableInExpressionCanNotFindDefination,
		ErrorVariableRedefinition,
		ErrorInternal,
		ErrorCanNotFindCodeGeneratorForCBMC,
		ErrorCanNotFindRunCBMC,
		ErrorCanNotFindCBMC,
		ErrorCanNotFindSetupMSVC,
		ErrorILCalculateMissRef,
		ErrorVariableCanNotFindReference,
		ErrorBatchAssignCanNotFindDataSource,
		ErrorBatchAssignDoNotHaveSIMDInstructionSupport,
		ErrorBatchReadDataCanNotFindDataSource,		  
		ErrorBatchReadDataDoNotHaveSIMDInstructionSupport,
		ErrorInvalidFFTInputOrOutputSize,
        ErrorInvalidDCTInputOrOutputSize,
        ErrorInvalidConvInputOrOutputSize,
	};

	std::vector<std::string> errorStrList = {
		"ErrorInvalidStatementType,					      ",
		"ErrorInvalidILBranchType,					      ",
		"ErrorCreateExpNode,						      ",
		"ErrorVariableInExpressionCanNotFindReference,    ",
		"ErrorVariableInExpressionCanNotFindDefination,   ",
		"ErrorVariableRedefinition,					      ",
		"ErrorInternal,								      ",
		"ErrorCanNotFindCodeGeneratorForCBMC,		      ",
		"ErrorCanNotFindRunCBMC,					      ",
		"ErrorCanNotFindCBMC,						      ",
		"ErrorCanNotFindSetupMSVC,					      ",
		"ErrorILCalculateMissRef,					      ",
		"ErrorVariableCanNotFindReference,			      ",
		"ErrorBatchAssignCanNotFindDataSource,		      ",
		"ErrorBatchAssignDoNotHaveSIMDInstructionSupport  ",
		"ErrorBatchReadDataCanNotFindDataSource,		  ",
		"ErrorBatchReadDataDoNotHaveSIMDInstructionSupport",
		"ErrorInvalidFFTInputOrOutputSize                 ",
        "ErrorInvalidDCTInputOrOutputSize,                ",
        "ErrorInvalidConvInputOrOutputSize,               ",
	};

	static std::vector<ILGlobalVariable*> globalVariables;

	std::string getErrorStr();
	static void setCurrentError(int errorCode, std::string msg);
private:
	static int errorCode;
	static std::string errorStr;

	int optimize() const;
	ILParser* iLParser = nullptr;
	MIRModel* mirModel = nullptr;

	//对组件组合表达模式进行优化
	int optimizeActorPattern() const;

	//对数据截断进行优化
	int optimizeDataTruncation() const;

	//对循环进行优化
	int optimizeIterator() const;

	//对不可达分支进行优化
	int optimizeUnreachableBranch() const;

	//硬件指令优化
	int optimizeHardwareInstruction() const;

	//对永真分支进行优化
	int optimizeTautologyBranch() const;


    //对函数中的批量计算语句进行优化，如Batch Add
	int optimizeBatchCalculation() const;
    
    //对函数中的复杂批量计算语句进行优化，如FFT
	int optimizeBatchCalculationComplex() const;

	//对函数中的调度块进行优化
	int optimizeScheduleLogic() const;
	int colletGlobalVariable() const;//收集全局变量信息，以供后面的表达式分析使用
	int optimizeSchedule(ILSchedule* schedule) const;//对函数中的调度块进行优化


	//对经过优化后的分支进行分支类型转化
	//比如if(cond){空}else{...} 转化为 if(!cond){...}
	int optimizeBranchType() const;

    //对批量计算进行折叠优化，避免重复数组拷贝
    int optimizeArrayFolding() const;

};

