#pragma once
#include <string>
#include "ILHardwareOptimizer.h"

class ILParser;
class ILSchedule;
class MIRModel;
class ILHardwareOptimizer;
class MIRActor;
class ILCalculate;

class ILHardwareOptimizerForTricore
{
public:
	int optimize(ILSchedule* schedule, ILParser* parser, MIRModel* mirModel);
private:
	int traverseILSchedule(ILSchedule* schedule);
	MIRActor* getMIRActorByILCalculate(ILCalculate* calculate);
	int processSingleSum(ILCalculate* calculate);
	int processBatchSum(ILCalculate* calculate);
	int processSingleMul(ILCalculate* calculate);
	int processBatchMul(ILCalculate* calculate);
	int processSingleSQRT(ILCalculate* calculate);
	int processBatchSQRT(ILCalculate* calculate);
	int processBatchABS(ILCalculate* calculate);
	int optimize();
	ILSchedule* schedule = nullptr;
	ILParser* iLParser = nullptr;
	MIRModel* mirModel = nullptr;
};
