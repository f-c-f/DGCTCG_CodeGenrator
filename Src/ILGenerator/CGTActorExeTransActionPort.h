#pragma once

#include <string>
#include "CGTActorExeTransObject.h"
class CGTActorExeTransActionPort : 
	public CGTActorExeTransObject
{
public:
	std::string name;
	std::string type;
    
	std::string sourceOutport;

	void release() {};
};
