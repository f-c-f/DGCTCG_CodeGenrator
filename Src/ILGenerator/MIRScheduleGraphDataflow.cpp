#include "MIRScheduleGraphDataflow.h"

#include "MIRScheduleGraphDataflowNode.h"

void MIRScheduleGraphDataflow::release()
{
    CGT.release();

    MIRScheduleGraph::release();
    for(int i = 0; i < nodes.size(); i++)
    {
        nodes[i]->release();
        delete nodes[i];
    }
    nodes.clear();
    
}
