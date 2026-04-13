#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include "../Common/tinyxml2.h"

#include "../MIRBasic/MIRObject.h"
#include "../MIRBasic/MIRModel.h"
#include "../MIRBasic/MIRFunction.h"
#include "../MIRBasic/MIRActor.h"
#include "../MIRBasic/MIRInport.h"
#include "../MIRBasic/MIROutport.h"
#include "../MIRBasic/MIRActionPort.h "
#include "../MIRBasic/MIRRelation.h"
#include "../MIRBasic/MIRSaver.h"

#include "../Common/Utility.h"

#include "../ILBasic/ILConfig.h"

class MIRScheduleGraph;
class MIRSchedule
{
public:
	MIRModel* model = nullptr;

    std::vector<MIRScheduleGraph*> scheduleGraphs;
    

    void release();
};

