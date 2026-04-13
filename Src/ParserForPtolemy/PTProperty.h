#pragma once
#include <string>
#include "PTObject.h"

class PTProperty :
    public PTObject
{
public:
	std::string name; //value
	std::string value; //2
	std::string className; //ptolemy.data.expr.Parameter
};
