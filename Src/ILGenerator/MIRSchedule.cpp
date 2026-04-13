#include "MIRSchedule.h"

#include "MIRScheduleGraph.h"


void MIRSchedule::release()
{
    for(int i = 0; i < scheduleGraphs.size(); i++)
    {
        scheduleGraphs[i]->release();
        delete scheduleGraphs[i];
    }
    scheduleGraphs.clear();
    
}

