#pragma once

#include <string>
#include <vector>

#include "MIRObject.h"
#include "MIRParameter.h"


class MIRActionPort;
class MIRInport;
class MIROutport;

class MIRActor :
	public MIRObject
{
public:
	std::string name;
	std::string type;
	
	std::vector<MIRInport*> inports;
	std::vector<MIROutport*> outports;
	std::vector<MIRActionPort*> actionPorts;
	std::vector<MIRParameter*> parameters;
    
    MIRInport* getInportByName(std::string name) const;
    MIROutport* getOutportByName(std::string name) const;
    MIRActionPort* getActionPortByName(std::string name) const;
    MIRParameter* getParameterByName(std::string name) const;

    MIRParameter* addParameter(std::string name, std::string value);

	void release();

private:
};
