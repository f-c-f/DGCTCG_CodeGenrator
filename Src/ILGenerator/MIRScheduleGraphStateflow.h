#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include "MIRScheduleGraph.h"



class MIRScheduleGraphStateflow: public MIRScheduleGraph
{
public:
    
    void release() override;
};
