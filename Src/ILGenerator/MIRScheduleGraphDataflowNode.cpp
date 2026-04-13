#include "MIRScheduleGraphDataflowNode.h"

#include "MIRScheduleGraphInport.h"
#include "MIRScheduleGraphOutport.h"
#include "../MIRBasic/MIRActor.h"
#include "../MIRBasic/MIRInport.h"
#include "../MIRBasic/MIROutport.h"

MIRScheduleGraphInport* MIRScheduleGraphDataflowNode::getInportByName(std::string portName)
{
    for(int i = 0; i < inports.size(); i++)
    {
        if(inports[i]->inportFromActor && inports[i]->inportFromActor->name == portName)
        {
            return inports[i];
        }
        else if(inports[i]->outportFromFunction && inports[i]->outportFromFunction->name == portName)
        {
            return inports[i];
        }
    }
    return nullptr;
}

MIRScheduleGraphOutport* MIRScheduleGraphDataflowNode::getOutportByName(std::string portName)
{
    for(int i = 0; i < outports.size(); i++)
    {
        if(outports[i]->outportFromActor && outports[i]->outportFromActor->name == portName)
        {
            return outports[i];
        }
        if(outports[i]->inportFromFunction && outports[i]->inportFromFunction->name == portName)
        {
            return outports[i];
        }
    }
    return nullptr;
}

void MIRScheduleGraphDataflowNode::release()
{
}

