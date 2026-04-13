#include "CGTActorExeTransActorInfo.h"

CGTActorExeTransParameter* CGTActorExeTransActorInfo::getParameterByName(std::string name)
{
    for(int i = 0; i < parameters.size(); i++)
    {
        if(parameters[i]->name == name)
            return parameters[i];
    }
    return nullptr;
}

std::string CGTActorExeTransActorInfo::getParameterValueByName(std::string name)
{
    for(int i = 0; i < parameters.size(); i++)
    {
        if(parameters[i]->name == name)
            return parameters[i]->value;
    }
    return "";
}

void CGTActorExeTransActorInfo::release()
{
	for (auto inport : inports)
	{
		inport->release();
		delete inport;
	}
	for (auto outport : outports)
	{
		outport->release();
		delete outport;
	}
}
