#include "MIRParameter.h"


MIRParameter::MIRParameter()
{
}

void MIRParameter::release()
{
}

MIRParameter* MIRParameter::clone()
{
    MIRParameter* ret = new MIRParameter();
    ret->name = name;
    ret->value = value;
    return ret;
}
