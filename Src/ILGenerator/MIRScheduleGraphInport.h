#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>


class CGTActorExe;
class MIROutport;
class MIRScheduleGraph;
class MIRInport;
class MIRScheduleGraphOutport;
class MIRScheduleGraphDataflowNode;

class MIRScheduleGraphInport
{
public:
    
	MIRInport* inportFromActor = nullptr;
	MIROutport* outportFromFunction = nullptr;
	MIRScheduleGraphDataflowNode* containerNode = nullptr;
	MIRScheduleGraph* containerGraph = nullptr;

	std::vector<MIRScheduleGraphOutport*> srcOutports;

    std::vector<MIRScheduleGraphOutport*> getRealSrcOutport(const CGTActorExe* containerActorExe) const;

    void release();
};
