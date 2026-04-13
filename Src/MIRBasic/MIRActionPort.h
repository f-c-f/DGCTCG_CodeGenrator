#pragma once

#include <string>
#include <vector>

#include "MIRObject.h"




class MIRActionPort :
	public MIRObject
{
public:
	std::string name;
	std::string type;
	
	std::string fullName;

	void release();
};
