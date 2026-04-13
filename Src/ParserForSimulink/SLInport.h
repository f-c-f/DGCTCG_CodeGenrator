#pragma once

#include <string>
#include <vector>

#include "SLObject.h"




class SLInport :
	public SLObject
{
public:
	std::string name;
	std::string type;
	std::vector<int> arraySize;

	int portId;

	void release();
};
