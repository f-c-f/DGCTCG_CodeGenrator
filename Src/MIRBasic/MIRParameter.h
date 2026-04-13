#pragma once

#include <string>
#include <vector>

#include "MIRObject.h"




class MIRParameter :
	public MIRObject
{
public:
	MIRParameter();
	MIRParameter(std::string name, std::string value) :name(name), value(value) {};
	std::string name;
	std::string value;
	void release();

    MIRParameter* clone();
};
