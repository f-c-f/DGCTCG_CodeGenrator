#pragma once
#include <string>
#include <vector>
#include "PTObject.h"

class PTPort;
class PTProperty;

class PTActor :
    public PTObject
{
public:
	std::string type;//type of actor		"AbsoluteValue"  ¡¢  "AddSubtract"
	std::string name;//full name			"JsfDeployWg_JsfCreateJobset_BooleanSwitch3"
    std::string className;//type in ptolemy		"ptolemy.actor.lib.StringConst"
	std::string actorNameOri;//short name		"BooleanSwitch3"


	std::vector<PTInport*> inports;
	std::vector<PTOutport*> outports;

};
