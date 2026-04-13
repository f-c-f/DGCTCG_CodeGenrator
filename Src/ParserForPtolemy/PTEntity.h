#pragma once
#include <string>
#include <vector>
#include "PTObject.h"

class PTInport;
class PTOutport;
class PTProperty;
class PTActor;

class PTRelation;
class PTLink;

class PTEntity :
    public PTObject
{
public:
	std::string classType;//type of actor		"AbsoluteValue"  ¡¢  "AddSubtract"
	std::string name;//name			"BooleanSwitch3"
    std::string className;//type in ptolemy		"ptolemy.actor.lib.StringConst"


	std::vector<PTInport*> inports;
	std::vector<PTOutport*> outports;
    
	std::vector<PTActor*> actors;
    
	std::vector<PTRelation*> relations;
	std::vector<PTLink*> links;
};
