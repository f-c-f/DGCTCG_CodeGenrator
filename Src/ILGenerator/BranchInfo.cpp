#include "BranchInfo.h"

bool sortBranchInfoByBranchPathLength(const std::pair<BranchPath, BranchPathDataSrc>& a,
    const std::pair<BranchPath, BranchPathDataSrc>& b)
{
    const BranchPath &bp1 = a.first;
    const BranchPath &bp2 = b.first;
    if(bp1.branchs.size() == bp2.branchs.size())
    {
        for(int i = 0; i < bp1.branchs.size(); i++)
        {
            if(bp1.branchs[i].branchNode < bp2.branchs[i].branchNode)
                return true;
            if(bp1.branchs[i].branchNode > bp2.branchs[i].branchNode)
                return false;

            if(bp1.branchs[i].branchId < bp2.branchs[i].branchId)
                return true;
            if(bp1.branchs[i].branchId > bp2.branchs[i].branchId)
                return false;
        }
    }
    return bp1.branchs.size() < bp2.branchs.size();
}
