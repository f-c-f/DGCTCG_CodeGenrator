#pragma once

#include <string>
#include "CGTActorExeTransObject.h"
class CGTActorExeTransInport :
	public CGTActorExeTransObject
{
public:
	std::string name;
	std::string type;
	int isAddress = 0;
	std::vector<int> arraySize;
	std::string sourceOutport;

	void release() {};
};
