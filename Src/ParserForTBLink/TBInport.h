#pragma once
#include <string>
#include <vector>
#include "TBObject.h"

class TBLinkInterface;

class TBInport :
    public TBObject
{
public:
	std::string name;//multiply
	std::string dataType = "default";//int float or struct  ......
	std::string value;
	
	std::vector<int> arraySize;

	std::vector<TBLinkInterface*> linkInterfaces;
};
