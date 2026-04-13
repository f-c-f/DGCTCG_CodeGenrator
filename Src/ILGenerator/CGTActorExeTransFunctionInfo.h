#pragma once

#include <string>
#include "CGTActorExeTransObject.h"

class CGTActorExeTransFunctionInfo 
	:public CGTActorExeTransObject 
{
public:

	std::string name;

	void release() {};
};
