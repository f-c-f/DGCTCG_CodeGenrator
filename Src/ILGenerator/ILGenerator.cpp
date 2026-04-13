#include "ILGenerator.h"
#include "../ILBasic/ILParser.h"
#include "../ILBasic/ILGlobalVariable.h"
#include "MIRSchedule.h"
#include "MIRScheduleGraph.h"
#include "ILGeneratorDataflow.h"
#include "ILGeneratorStateflow.h"
#include "MIRScheduleGraphDataflow.h"

bool ILGenerator::configTopologyBasedOrder = false;
MIRSchedule* ILGenerator::schedule = nullptr;
MIRModel* ILGenerator::model = nullptr;
ILParser* ILGenerator::retILParser = nullptr;

int ILGenerator::generateIL(MIRSchedule* schedule, MIRModel* model, ILParser* retILParser)
{
    int res;
    ILGenerator::schedule = schedule;
    ILGenerator::model = model;
    ILGenerator::retILParser = retILParser;

    retILParser->config.iterationCount = 10;
    retILParser->config.mainInitFunction = schedule->model->mainFunction + "_Init";
    retILParser->config.mainExecFunction = schedule->model->mainFunction + "_Update";
    retILParser->config.mainCompositeState = schedule->model->mainFunction + "_state";
    
    for(int i = 0; i < schedule->scheduleGraphs.size(); i++)
    {
        MIRScheduleGraph* scheduleGraph = schedule->scheduleGraphs[i];
        if(scheduleGraph->type == MIRScheduleGraph::TypeDataflow)
        {
            MIRScheduleGraphDataflow* scheduleGraphDataFlow = (MIRScheduleGraphDataflow*)scheduleGraph;
            if(scheduleGraphDataFlow->name != schedule->model->mainFunction) {
                continue;
            }

            // 只翻译主函数，其它函数在翻译到各个复合组件时递归翻译，因为复合组件输入端口需要进行类型推导
            ILGeneratorDataflow iLGeneratorDataflow;
            res = iLGeneratorDataflow.generateIL((MIRScheduleGraphDataflow*)scheduleGraph, model, retILParser);
            if(res < 0)
                return res;
        }
    }

    res = generateStateInstanceVariableForSubsystems();
    if(res < 0)
        return res;

    return 1;
}

int ILGenerator::generateInnerFunction(std::string functionName)
{
    int res;
    for(int i = 0; i < schedule->scheduleGraphs.size(); i++)
    {
        MIRScheduleGraph* scheduleGraph = schedule->scheduleGraphs[i];
        if(scheduleGraph->name == functionName)
        {
            res = generateInnerFunction(scheduleGraph);
            return res;
            break;
        }
    }
    return 0;
}

int ILGenerator::generateInnerFunction(MIRScheduleGraph* scheduleGraph)
{
    int res;
    if(scheduleGraph->type == MIRScheduleGraph::TypeDataflow)
    {
        ILGeneratorDataflow iLGeneratorDataflow;
        res = iLGeneratorDataflow.generateIL((MIRScheduleGraphDataflow*)scheduleGraph, model, retILParser);
        if(res < 0)
            return res;
    }
    else if(scheduleGraph->type == MIRScheduleGraph::TypeStateflow)
    {
        ILGeneratorStateflow iLGeneratorStateflow;
        res = iLGeneratorStateflow.generateIL((MIRScheduleGraphStateflow*)scheduleGraph, model, retILParser);
        if(res < 0)
            return res;
    }
    return 0;
}

int ILGenerator::generateStateInstanceVariableForSubsystems()
{
    for(int i = 0; i < retILParser->files.size(); i++)
    {
        ILFile* file = retILParser->files[i];
        ILGlobalVariable* stateInstanceVar = new ILGlobalVariable();
        stateInstanceVar->parent = file;
        file->globalVariables.push_back(stateInstanceVar);

        stateInstanceVar->name = file->name + "_instance";
        stateInstanceVar->type = file->name + "_state";
    }
    return 1;
}

void ILGenerator::setConfigTopologyBasedOrder(bool value)
{
    configTopologyBasedOrder = value;
}
