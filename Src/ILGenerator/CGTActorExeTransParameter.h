#pragma once

#include <string>
#include "CGTActorExeTransObject.h"
class CGTActorExeTransParameter :
	public CGTActorExeTransObject
{
public:
	std::string name;
	std::string value;

	void release() {};
};
