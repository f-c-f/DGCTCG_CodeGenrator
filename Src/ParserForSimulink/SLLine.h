#pragma once

#include <string>
#include <vector>

#include "SLObject.h"




class SLLine :
	public SLObject
{
public:
	std::vector<std::string> srcs;
	std::vector<std::string> dsts;

	void release();
};
