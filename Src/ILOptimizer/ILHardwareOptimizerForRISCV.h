#pragma once
#include <string>
#include "ILHardwareOptimizer.h"

class ILParser;
class ILSchedule;
class MIRModel;
class ILHardwareOptimizer;
class MIRActor;
class ILCalculate;

class ILHardwareOptimizerForRISCV
{
public:
	int optimize(ILSchedule* schedule, ILParser* parser, MIRModel* mirModel);
private:
	int traverseILSchedule(ILSchedule* schedule);
	MIRActor* getMIRActorByILCalculate(ILCalculate* calculate);
	int processBatchSum(ILCalculate* calculate);
	int processBatchMul(ILCalculate* calculate);
	int processBatchABS(ILCalculate* calculate);
	int processBatchSQRT(ILCalculate* calculate);
	int optimize();
	ILCalculate* getILCalculateByName(std::string name);
	ILSchedule* schedule = nullptr;
	ILParser* iLParser = nullptr;
	MIRModel* mirModel = nullptr;
};
