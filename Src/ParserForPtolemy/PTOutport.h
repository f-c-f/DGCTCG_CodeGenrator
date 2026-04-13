#pragma once
#include <string>
#include "PTObject.h"

class PTOutport :
    public PTObject
{
public:
	std::string fullName;//MultiplyDivide.multiply
	std::string name;//multiply
	std::string type = "#Default";//int float or struct  ......
	
	std::vector<int> arraySize;
};
