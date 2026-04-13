#pragma once
#include <string>
#include "PTObject.h"

class PTInport :
    public PTObject
{
public:
	std::string fullName;//MultiplyDivide.multiply
	std::string name;//multiply
	std::string type = "#Default";//int float or struct  ......
	
	std::vector<int> arraySize;
};
