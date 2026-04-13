#include "MIRActor.h"

#include "MIRRelation.h"
#include "MIRInport.h"
#include "MIROutport.h"
#include "MIRActionPort.h"
using namespace std;

MIRInport* MIRActor::getInportByName(std::string name) const
{
    for(int i = 0;i < inports.size(); i++)
	{
		if(inports[i]->name == name)
            return inports[i];
	}
    return nullptr;
}

MIROutport* MIRActor::getOutportByName(std::string name) const
{
    for(int i = 0;i < outports.size(); i++)
	{
		if(outports[i]->name == name)
            return outports[i];
	}
    return nullptr;
}

MIRActionPort* MIRActor::getActionPortByName(std::string name) const
{
    for(int i = 0;i < actionPorts.size(); i++)
	{
		if(actionPorts[i]->name == name)
            return actionPorts[i];
	}
    return nullptr;
}

MIRParameter* MIRActor::getParameterByName(std::string name) const
{
    for(int i = 0;i < parameters.size(); i++)
	{
		if(parameters[i]->name == name)
            return parameters[i];
	}
    return nullptr;
}

MIRParameter* MIRActor::addParameter(string name, string value)
{
    MIRParameter* parameter = new MIRParameter(name, value);
    parameters.push_back(parameter);
    return parameter;
}

void MIRActor::release()
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
    
    for(int i = 0;i < parameters.size(); i++)
	{
		parameters[i]->release();
		delete parameters[i];
	}
    parameters.clear();
}
