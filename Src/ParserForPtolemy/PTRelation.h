#pragma once
#include <string>
#include "PTObject.h"

class PTRelation :
    public PTObject
{
public:
	std::string name;
	std::string guard;
	std::string setval;
	std::string output;
	std::string refinementName;
};
