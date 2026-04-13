#pragma once

#include <string>
#include <vector>

#include "MIRFunction.h"
#include "MIRObject.h"

class MIRFunctionDataflow :
	public MIRFunction
{
public:
    
	std::vector<MIRActor*> actors;
	std::vector<MIRRelation*> relations;

    std::vector<MIRRelation*> getRelationByPort(MIRObject* portObj);
	MIRActor* getActorByName(std::string name);

    std::vector<MIROutport*> getLinkedOutportsByInport(MIRInport* inport);
    std::vector<MIRInport*> getLinkedInportsByOutport(MIROutport* outport);
    
    void deleteActor(MIRActor* actors);
    void deleteRelation(MIRRelation* relation);

	void release();
};
