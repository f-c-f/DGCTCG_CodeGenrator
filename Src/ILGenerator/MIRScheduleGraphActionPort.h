#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>


class CGTActorExe;
class MIRActionPort;
class MIRScheduleGraphOutport;
class MIRScheduleGraphDataflowNode;

class MIRScheduleGraphActionPort
{
public:
	MIRActionPort* actionPort = nullptr;
	MIRScheduleGraphDataflowNode* containerNode = nullptr;
	std::vector<MIRScheduleGraphOutport*> srcOutports;

    MIRScheduleGraphOutport* getRealSrcOutport(const CGTActorExe* containerActorExe) const;

    void release();
};
