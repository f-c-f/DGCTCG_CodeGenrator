#pragma once

#include <string>
#include <vector>

#include "MIRObject.h"




class MIRActor;
class MIRRelation;
class MIRInport;
class MIROutport;
class MIRActionPort;
class MIRFunction :
	public MIRObject
{
public:

    enum FunctionType
    {
        TypeDataflow,
        TypeStateflow,
        TypeUnknown,
    };

    FunctionType type = TypeUnknown;

	std::string name;

	std::vector<MIRInport*> inports;
	std::vector<MIROutport*> outports;
	std::vector<MIRActionPort*> actionPorts;
    
    MIRInport* getInportByName(std::string name);
    MIROutport* getOutportByName(std::string name);
    MIRActionPort* getActionPortByName(std::string name);


	void release();
};
