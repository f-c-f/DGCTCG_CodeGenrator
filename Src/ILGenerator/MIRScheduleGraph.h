#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>

#include "../ILBasic/ILFile.h"


class MIRScheduleGraphInport;
class MIRScheduleGraphOutport;
class MIRScheduleGraph
{
public:
    std::string name;

	enum MIRScheduleGraphType {
		TypeDataflow,
		TypeStateflow,
	};

	MIRScheduleGraphType type;
    
    // 输入输出端口相对外部是正常的输入输出端口，
    // 但相对内部，输入端口则对应outport，输出端口则对应inport
    // 模型函数的调度计算是纯内部逻辑的计算，所以这里输入输出是反过来的
    std::vector<MIRScheduleGraphOutport*> inports;
	std::vector<MIRScheduleGraphInport*> outports;
    
    virtual void release();
};
