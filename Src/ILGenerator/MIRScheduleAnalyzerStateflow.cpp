#include "MIRScheduleAnalyzerStateflow.h"

#include "MIRScheduleAnalyzer.h"

#include "MIRPreprocessor.h"
#include "MIRScheduleGraphActionPort.h"
#include "MIRScheduleGraphDataflowNode.h"
#include "MIRScheduleGraphInport.h"
#include "MIRScheduleGraphOutport.h"
#include "MIRScheduleGraphStateflow.h"
#include "../Common/Utility.h"
#include "../MIRBasic/MIRFunctionDataflow.h"
#include "../MIRBasic/MIRFunctionStateflow.h"

using namespace std;

MIRScheduleAnalyzerStateflow::MIRScheduleAnalyzerStateflow(MIRScheduleAnalyzer* bassAnalyzer)
{
    this->bassAnalyzer = bassAnalyzer;
}

MIRScheduleAnalyzerStateflow::~MIRScheduleAnalyzerStateflow()
{
}

int MIRScheduleAnalyzerStateflow::collectScheduleFunction(MIRSchedule* schedule, MIRFunction* function)
{
    if(function->type == MIRFunction::TypeStateflow)
    {
        return collectScheduleFunctionStateflow(schedule, reinterpret_cast<MIRFunctionStateflow*>(function));
    }
    return -MIRScheduleAnalyzer::ErrorUnknownMIRFunctionType;
}

int MIRScheduleAnalyzerStateflow::collectScheduleFunctionStateflow(MIRSchedule* schedule, MIRFunctionStateflow* function)
{
    MIRScheduleGraphStateflow* graph = new MIRScheduleGraphStateflow();
    schedule->scheduleGraphs.push_back(graph);
    graph->name = function->name;

    for(int i = 0; i < function->inports.size(); i++)
    {
        int res = collectScheduleFunctionOutports(graph, function->outports[i]);
        if(res < 0)
            return res;
    }
    for(int i = 0; i < function->outports.size(); i++)
    {
        int res = collectScheduleFunctionInports(graph, function->inports[i]);
        if(res < 0)
            return res;
    }
    return 1;
}

int MIRScheduleAnalyzerStateflow::collectScheduleFunctionInports(MIRScheduleGraph* scheduleGraph,
    MIRInport* inport)
{
    MIRScheduleGraphOutport* graphInport = new MIRScheduleGraphOutport();
    scheduleGraph->inports.push_back(graphInport);
    graphInport->containerGraph = scheduleGraph;
    graphInport->inportFromFunction = inport;
    return 1;
}

int MIRScheduleAnalyzerStateflow::collectScheduleFunctionOutports(MIRScheduleGraph* scheduleGraph,
    MIROutport* outport)
{
    MIRScheduleGraphInport* graphOutport = new MIRScheduleGraphInport();
    scheduleGraph->outports.push_back(graphOutport);
    graphOutport->containerGraph = scheduleGraph;
    graphOutport->outportFromFunction = outport;
    return 1;
}



int MIRScheduleAnalyzerStateflow::calculateScheduleGraph(MIRScheduleGraph* schedule)
{
    if(schedule->type == MIRScheduleGraph::TypeStateflow)
    {
        return calculateScheduleGraphStateflow(reinterpret_cast<MIRScheduleGraphStateflow*>(schedule));
    }
    return -MIRScheduleAnalyzer::ErrorUnknownMIRFunctionType;
}

int MIRScheduleAnalyzerStateflow::calculateScheduleGraphStateflow(MIRScheduleGraphStateflow* schedule)
{
    return 1;
}
