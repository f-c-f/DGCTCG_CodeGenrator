#pragma once
#include "CGTActorExeTransObject.h"

class CGTActorExeTransSourceOutport : 
	public CGTActorExeTransObject
{
public:

	std::string name;
	std::string type;
	int isAddress = 0;
	std::vector<int> arraySize;
    
	std::string actorName;
	std::string actorType;

	void release() {};
};
