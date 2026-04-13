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

#include "MIRSchedule.h"
#include "MIRScheduleAnalyzer.h"
#include "MIRScheduleGraphDataflow.h"


class MIRScheduleGraphActionPort;
class MIRScheduleGraphStateflow;
class MIRFunctionStateflow;
class MIRFunctionDataflow;

class MIRScheduleAnalyzerStateflow
{
public:
    MIRScheduleAnalyzerStateflow(MIRScheduleAnalyzer* bassAnalyzer);
    ~MIRScheduleAnalyzerStateflow();

    int collectScheduleFunction(MIRSchedule* schedule, MIRFunction* function);
    int calculateScheduleGraph(MIRScheduleGraph* schedule);

private:
    MIRScheduleAnalyzer* bassAnalyzer = nullptr;

    
    int collectScheduleFunctionStateflow(MIRSchedule* schedule, MIRFunctionStateflow* function);
    
    int collectScheduleFunctionInports(MIRScheduleGraph* scheduleGraph, MIRInport* inport);
    int collectScheduleFunctionOutports(MIRScheduleGraph* scheduleGraph, MIROutport* outport);
   
    int calculateScheduleGraphStateflow(MIRScheduleGraphStateflow* schedule);
};

