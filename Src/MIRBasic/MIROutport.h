#pragma once

#include <string>
#include <vector>

#include "MIRObject.h"




class MIROutport :
	public MIRObject
{
public:
	std::string name;
	std::string type;
	int isAddress = 0;
	std::vector<int> arraySize;
	std::string defaultValue;
	
	std::string fullName;

	void release();
};
