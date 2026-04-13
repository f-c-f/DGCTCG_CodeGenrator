#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>


class MIRScheduleGraph;
class MIRInport;
class MIRScheduleGraphActionPort;
class MIROutport;
class MIRScheduleGraphInport;
class MIRScheduleGraphDataflowNode;

class MIRScheduleGraphOutport
{
public:
	MIROutport* outportFromActor = nullptr;
	MIRInport* inportFromFunction = nullptr;
	MIRScheduleGraphDataflowNode* containerNode = nullptr;
	MIRScheduleGraph* containerGraph = nullptr;

	std::vector<MIRScheduleGraphInport*> dstInports;
	std::vector<MIRScheduleGraphActionPort*> dstActionPorts;

    void release();
};
