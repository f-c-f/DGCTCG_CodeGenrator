#include "SLBlock.h"

#include "SLLine.h"
#include "SLInport.h"
#include "SLOutport.h"
#include "SLActionPort.h"

#include <algorithm>

using namespace std;

vector<string> SLBlock::nonActorBlockType = {
	"Inport",
	"Outport",
	"EnablePort",
	"ActionPort",
    "TriggerPort",
};
void SLBlock::release()
{
	for(int i = 0;i < inports.size(); i++)
	{
		inports[i]->release();
		delete inports[i];
	}
	inports.clear();

	for(int i = 0;i < outports.size(); i++)
	{
		outports[i]->release();
		delete outports[i];
	}
	outports.clear();

	for(int i = 0;i < actionPorts.size(); i++)
	{
		actionPorts[i]->release();
		delete actionPorts[i];
	}
	actionPorts.clear();
}

bool SLBlock::isActorBlockType(string blockType)
{
	if(find(nonActorBlockType.begin(), nonActorBlockType.end(), blockType) ==
		nonActorBlockType.end())
	{
		return true;
	}
	return false;
}

