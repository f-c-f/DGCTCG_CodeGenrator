#pragma once

#include <vector>

class MIRScheduleGraphOutport;
class MIRScheduleGraphDataflowNode;

// 分支标记算法所需数据结构
struct Branch
{
    MIRScheduleGraphDataflowNode* branchNode = nullptr;
    int branchId = 0; //默认采用分支输出端口的索引号
};

struct BranchPath
{
    std::vector<Branch> branchs;
};

struct BranchPathDataSrc
{
    std::vector<MIRScheduleGraphOutport*> srcs;
};

struct BranchInfo
{

    std::vector<std::pair<BranchPath, BranchPathDataSrc>> branchPathWithSrc;
};

bool sortBranchInfoByBranchPathLength(const std::pair<BranchPath, BranchPathDataSrc> &a, const std::pair<BranchPath, BranchPathDataSrc> &b);

