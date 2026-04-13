#include "ILHardwareOptimizer.h"
#include "../MIRBasic/MIRModel.h"
#include "ILHardwareOptimizerForTricore.h"
#include "ILHardwareOptimizerForRISCV.h"
#include "ILHardwareOptimizerForARM.h"
using namespace std;

int ILHardwareOptimizer::optimize(ILSchedule* schedule, ILParser* parser, MIRModel* mirModel, string platform)
{
	this->schedule = schedule;
	this->iLParser = parser;
	this->mirModel = mirModel;
	this->platform = platform;


	int res = optimize();
	if (res < 0)
		return res;


	return 1;
}

int ILHardwareOptimizer::optimize() const
{
	if (platform == "Tricore")
	{
		ILHardwareOptimizerForTricore optimizer;
		optimizer.optimize(schedule, iLParser, mirModel);
	}
	else if (platform == "RISC-V")
	{
		ILHardwareOptimizerForRISCV optimizer;
		optimizer.optimize(schedule, iLParser, mirModel);
	}
	else if (platform == "ARMv8")
	{
		ILHardwareOptimizerForARM optimizer;
		optimizer.optimize(schedule, iLParser, mirModel);
	}

	return 0;
}
