#pragma once

#include <string>
#include <vector>

#include "SLObject.h"



class SLActionPort;
class SLInport;
class SLOutport;

class SLBlock :
	public SLObject
{
public:
	std::string type;
	std::string name;
	
	std::vector<SLInport*> inports;
	std::vector<SLOutport*> outports;
	std::vector<SLActionPort*> actionPorts;

	std::string systemRef;		//For BlockType="SubSystem"
	std::string sourceBlockType;	//For BlockType="Reference"

	void release();

	static bool isActorBlockType(std::string blockType);
private:
	static std::vector<std::string> nonActorBlockType;
};
