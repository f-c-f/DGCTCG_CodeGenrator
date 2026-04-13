#pragma once
#include<string>
#include<vector>
#include<map>

struct DiagnoseOption
{
    bool IntegerOverflow = true;
    bool IntegerSaturation = true;
    bool ParameterDowncast = true;
    bool ParameterOverflow = true;
    bool ParameterPrecisionLoss = true;
    bool Int32ToFloatConv = true;
    bool ArrayOutOfBound = true;
    bool DevideByZero = true;
    //bool option9 = true;
    //bool option10 = true;
};

class SimulatorConfig
{
public:
    std::vector<std::string> actorPathNeedRecord;
    DiagnoseOption diagOption;
};
