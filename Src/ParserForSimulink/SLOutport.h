#pragma once

#include <string>
#include <vector>

#include "SLObject.h"




class SLOutport :
	public SLObject
{
public:
	std::string name;
	std::string type;
	std::vector<int> arraySize;

	int portId;

	void release();
};
