#pragma once
#include <string>
#include "TBObject.h"

class TBProperty :
    public TBObject
{
public:
	std::string name; //value
	std::string value; //2
};
