#pragma once
#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include "MIRScheduleGraph.h"


class CGTActorExe;
class MIRScheduleGraphActionPort;
class MIRActor;

class MIRScheduleGraphDataflowNode;
class MIRScheduleGraphDataflow;

class MIRScheduleGraphDataflowNode
{
public:
	MIRActor* actor = nullptr;
	int depth = -1;
	//std::vector<ScheduleGraphDataflowNode*> successors;

	std::vector<MIRScheduleGraphInport*> inports;
	std::vector<MIRScheduleGraphOutport*> outports;
	std::vector<MIRScheduleGraphActionPort*> actionPorts;



	MIRScheduleGraphDataflow* graph = nullptr;

	//string <= branchAddr , 
	//std::map<std::string, std::set<MIRScheduleGraphOutport*> > branchInfo;
	//bool isBranchInfoMerged = false;

	//std::set<ScheduleGraphDataflowNode*> allPredecessors;
    
	// bool isAllBranchHaveSameSrc();

    MIRScheduleGraphInport* getInportByName(std::string portName);
    MIRScheduleGraphOutport* getOutportByName(std::string portName);
    
    void release();


};
