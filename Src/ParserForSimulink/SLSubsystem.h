#pragma once

#include <string>
#include <vector>

#include "SLObject.h"




class SLBlock;
class SLLine;
class SLInport;
class SLOutport;
class SLActionPort;
class SLSubsystem :
	public SLObject
{
public:
	std::string name;

	std::vector<SLInport*> inports;
	std::vector<SLOutport*> outports;
	std::vector<SLActionPort*> actionPorts;

	std::vector<SLBlock*> blocks;
	std::vector<SLLine*> lines;
	
	void release();
};
