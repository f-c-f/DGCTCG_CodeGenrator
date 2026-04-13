#include "MIRFunction.h"
#include "MIRActor.h"
#include "MIRRelation.h"
#include "MIRInport.h"
#include "MIROutport.h"
#include "MIRActionPort.h"
using namespace std;
MIRInport* MIRFunction::getInportByName(std::string name)
{
    for(int i = 0;i < inports.size(); i++)
	{
		if(inports[i]->name == name)
            return inports[i];
	}
    return nullptr;
}

MIROutport* MIRFunction::getOutportByName(std::string name)
{
    for(int i = 0;i < outports.size(); i++)
	{
		if(outports[i]->name == name)
            return outports[i];
	}
    return nullptr;
}

MIRActionPort* MIRFunction::getActionPortByName(std::string name)
{
    for(int i = 0;i < actionPorts.size(); i++)
	{
		if(actionPorts[i]->name == name)
            return actionPorts[i];
	}
    return nullptr;
}


void MIRFunction::release()
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
}
