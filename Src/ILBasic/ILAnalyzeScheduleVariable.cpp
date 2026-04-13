#include "ILAnalyzeScheduleVariable.h"

ILAnalyzeScheduleVariableType::ILAnalyzeScheduleVariableType()
{
}

ILAnalyzeScheduleVariableType::ILAnalyzeScheduleVariableType(std::string name, std::string type, int isAddress, std::vector<int> arraySize)
    : name(name), type(type), isAddress(isAddress), arraySize(arraySize)
{
}

ILAnalyzeScheduleVariableType::~ILAnalyzeScheduleVariableType()
{
}

ILAnalyzeScheduleVariable* ILAnalyzeScheduleVariable::getTopParent() const
{
    ILAnalyzeScheduleVariable* ret = const_cast<ILAnalyzeScheduleVariable*>(this);
    while(ret->parent)
    {
        ret = ret->parent;
    }
    return ret;
}

int ILAnalyzeScheduleVariable::release()
{
    auto iter = members.begin();
    for(; iter != members.end(); iter++)
    {
        iter->second->release();
        delete iter->second;
    }
    members.clear();
    return 1;
}
