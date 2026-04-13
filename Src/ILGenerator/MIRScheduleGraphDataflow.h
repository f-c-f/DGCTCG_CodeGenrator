#pragma once
#include <string>
#include <vector>
#include <map>
#include <iostream>

#include "CodeGenerationTable.h"
#include "MIRScheduleGraph.h"


class MIRActor;
class MIRScheduleGraphDataflowNode;

class MIRScheduleGraphDataflow: public MIRScheduleGraph
{
public:
    std::vector<MIRScheduleGraphDataflowNode*> nodes; // 未经分层的
	std::map<int, std::vector<MIRScheduleGraphDataflowNode*>> layers; // 经过分层的

    std::map<MIRActor*, MIRScheduleGraphDataflowNode*> actorToNodeMap; // 用于基于Actor快速获取Node

    // 用于处理Function Call Generator + Demux组件组合完成的实现函数先后调用的模型表达
    // 这里采用在构建调度图时，特别处理每个Function节点，确认他们之间的函数调用关系
    // 对于每个Function节点先寻找其前面Demux组件必要的前几个Function节点，建立顺序关系
    // 再判断前几个Function节点，是否存在自身为前驱，存在则断开原有顺序关系，改为和自身建立前后顺序
    std::map<MIRScheduleGraphDataflowNode*, std::vector<MIRScheduleGraphDataflowNode*>> necessaryPredecessorNodeMap;

    CodeGenerationTable CGT;

    void release() override;
};
