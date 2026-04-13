#pragma once

class MIRModel;
class MIRScheduleGraphStateflow;
class MIRSchedule;
class ILParser;

class ILGeneratorStateflow
{
public:
    int generateIL(MIRScheduleGraphStateflow* scheduleGraph, MIRModel* model, ILParser* retILParser);

};

