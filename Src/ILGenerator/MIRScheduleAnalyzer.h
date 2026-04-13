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
#include "MIRScheduleGraphDataflow.h"


class MIRScheduleGraphActionPort;
class MIRScheduleGraphStateflow;
class MIRFunctionStateflow;
class MIRFunctionDataflow;

class MIRScheduleAnalyzer
{
public:
	int scheduleAnalyze(MIRModel* model, MIRSchedule* retSchedule);

	MIRModel* model = nullptr;

    MIRSchedule* schedule = nullptr;


    enum {
		ErrorUnknownMIRFunctionType = 120001,
		ErrorInnerOutportNotFound,
		ErrorInnerInportOrActionPortNotFound,
		ErrorInnerTopologicalSortNodeNotFound,
		ErrorInnerCanNotFindDelayStoreActorExeByDelayRead,
        ErrorLoadActorReturnILXMLFail,
        ErrorUnknownFunctionName,
        ErrorInnerScheduleAnalyzeFailed,
    };


    // 由于该MIRScheduleAnalyzer不创建MIRSchedule对象，
    // 所以不负责释放MIRSchedule对象
	void release();
	
	std::string getErrorStr();
private:
	int errorCode = 0;
    int scheduleAnalyze(MIRSchedule* schedule);

	void setCurrentError(int errorCode);

    int collectScheduleFunction(MIRSchedule* schedule, MIRFunction* function);
    
    int calculateScheduleGraph(MIRScheduleGraph* schedule);
};

