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

#include "MIRScheduleAnalyzerDataflow.h"
#include "MIRScheduleAnalyzerStateflow.h"

using namespace std;

int MIRScheduleAnalyzer::scheduleAnalyze(MIRModel* model, MIRSchedule* retSchedule)
{
    this->schedule = retSchedule;
    this->model = model;
    retSchedule->model = model;

    MIRPreprocessor mIRPreprocessor;
    int res = mIRPreprocessor.preprocessMIR(model);
    if(res < 0)
    {
        return res;
    }

    res = scheduleAnalyze(retSchedule);
    if(res < 0)
    {
        return res;
    }
    
    return 0;
}

void MIRScheduleAnalyzer::release()
{
}

string MIRScheduleAnalyzer::getErrorStr()
{
    return string();
}

void MIRScheduleAnalyzer::setCurrentError(int errorCode)
{
}

int MIRScheduleAnalyzer::scheduleAnalyze(MIRSchedule* schedule)
{
    for(int i = 0; i < model->functions.size(); i++)
    {
        int res = collectScheduleFunction(schedule, model->functions[i]);
        if(res < 0)
        {
            return res;
        }
    }

    for(int i = 0; i < schedule->scheduleGraphs.size(); i++)
    {
        //if(i != 1)
        //    continue;
        int res = calculateScheduleGraph(schedule->scheduleGraphs[i]);
        if(res < 0)
        {
            return res;
        }
    }
    return 1;
}

int MIRScheduleAnalyzer::collectScheduleFunction(MIRSchedule* schedule, MIRFunction* function)
{
    if(function->type == MIRFunction::TypeDataflow)
    {
        MIRScheduleAnalyzerDataflow analyzer(this);
        return analyzer.collectScheduleFunction(schedule, function);
    }
    else if(function->type == MIRFunction::TypeStateflow)
    {
        MIRScheduleAnalyzerStateflow analyzer(this);
        return analyzer.collectScheduleFunction(schedule, function);
    }
    return -ErrorUnknownMIRFunctionType;
}

int MIRScheduleAnalyzer::calculateScheduleGraph(MIRScheduleGraph* schedule)
{
    if(schedule->type == MIRScheduleGraph::TypeDataflow)
    {
        MIRScheduleAnalyzerDataflow analyzer(this);
        return analyzer.calculateScheduleGraph(schedule);
    }
    else if(schedule->type == MIRScheduleGraph::TypeStateflow)
    {
        MIRScheduleAnalyzerStateflow analyzer(this);
        return analyzer.calculateScheduleGraph(schedule);
    }
    return -ErrorUnknownMIRFunctionType;
}
