#pragma once

#include <string>
#include <vector>

#include "SLObject.h"

using namespace std;


class SLActionPort :
	public SLObject
{
public:
	std::string name;
	std::string type;

	void release();
};
