#pragma once

#include <string>
#include <vector>

#include "CGTActorExeTransObject.h"
#include "CGTActorExeTransInport.h"
#include "CGTActorExeTransOutport.h"
#include "CGTActorExeTransParameter.h"
#include "CGTActorExeTransActionPort.h"

class CGTActorExeTransActorInfo :
	public CGTActorExeTransObject 
{
public:
	std::string name;
	std::string type;
	std::vector<CGTActorExeTransInport*> inports;
	std::vector<CGTActorExeTransOutport*> outports;
	std::vector<CGTActorExeTransActionPort*> actionPorts;
	std::vector<CGTActorExeTransParameter*> parameters;

    CGTActorExeTransParameter* getParameterByName(std::string name);
    std::string getParameterValueByName(std::string name);

	void release();
};
