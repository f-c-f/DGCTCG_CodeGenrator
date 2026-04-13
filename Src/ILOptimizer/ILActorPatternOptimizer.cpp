#include "ILActorPatternOptimizer.h"
#include "ILActorPatternDataSelector.h"
#include "../MIRBasic/MIRModel.h"
using namespace std;

int ILActorPatternOptimizer::optimize(ILSchedule* schedule, ILParser* parser, MIRModel* mirModel)
{
	this->schedule = schedule;
	this->iLParser = parser;
	this->mirModel = mirModel;
	int res = optimize();
	if(res < 0)
		return res;

	return 1;
}

int ILActorPatternOptimizer::optimize() const
{
	int res;
	vector<ILActorPattern*> iLActorPatternList;
	ILActorPatternDataSelector iLActorPatternDataSelector;

	iLActorPatternList.push_back(reinterpret_cast<ILActorPattern*>(&iLActorPatternDataSelector));

	for(int i = 0; i < iLActorPatternList.size(); i++)
	{
		ILActorPattern* iLActorPattern = iLActorPatternList[i];
		res = iLActorPattern->optimize(this->schedule, this->iLParser, this->mirModel);
		if(res < 0)
			return res;
	}

	return 1;
}
