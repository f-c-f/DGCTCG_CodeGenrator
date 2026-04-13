#pragma once
#include <string>
#include "PTObject.h"


class PTLink :
    public PTObject
{
public:
	std::string port;
	std::string relation;

	std::string relation1;
	std::string relation2;
};
