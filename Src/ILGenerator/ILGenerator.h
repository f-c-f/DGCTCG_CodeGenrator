#pragma once
#include <string>

class MIRScheduleGraph;
class MIRModel;
class MIRSchedule;
class ILParser;

class ILGenerator
{
public:
    static MIRSchedule* schedule;
    static MIRModel* model;
    static ILParser* retILParser;

    int generateIL(MIRSchedule* schedule, MIRModel* model, ILParser* retILParser);

    static int generateInnerFunction(std::string functionName);

    static void setConfigTopologyBasedOrder(bool value);
    static bool configTopologyBasedOrder;

private:
    static int generateInnerFunction(MIRScheduleGraph* scheduleGraph);

    int generateStateInstanceVariableForSubsystems();
};

