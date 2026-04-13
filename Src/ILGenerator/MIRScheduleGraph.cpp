#include "MIRScheduleGraph.h"

#include "MIRScheduleGraphInport.h"
#include "MIRScheduleGraphOutport.h"

void MIRScheduleGraph::release()
{
    for(int i = 0; i < inports.size(); i++)
    {
        inports[i]->release();
        delete inports[i];
    }
    inports.clear();
    
    for(int i = 0; i < outports.size(); i++)
    {
        outports[i]->release();
        delete outports[i];
    }
    outports.clear();
}
