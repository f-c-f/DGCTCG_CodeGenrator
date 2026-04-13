#include "MIRScheduleGraphActionPort.h"

#include "BranchInfo.h"
#include "CodeGenerationTable.h"
#include "MIRScheduleGraphDataflowNode.h"
#include <algorithm>

MIRScheduleGraphOutport* MIRScheduleGraphActionPort::getRealSrcOutport(const CGTActorExe* containerActorExe) const
{
    MIRScheduleGraphOutport* srcOutport = nullptr;
    const BranchPathDataSrc& realBranchPathDataSrc = containerActorExe->realBranchPathDataSrc;
    bool foundRealSrc = false;
    // 在该ActorExe的realBranchPathDataSrc中找inport对应的数据源
    // 如果一个inport的数据源端口不在realBranchPathDataSrc中，就不是真正的数据源端口
    for(int j = 0; j < this->srcOutports.size(); j++)
    {
        srcOutport = this->srcOutports[j];
        const std::vector<MIRScheduleGraphOutport*> &realSrcList = realBranchPathDataSrc.srcs;
        if(find(realSrcList.begin(), realSrcList.end(), srcOutport) == realSrcList.end())
        {
            continue;
        }
        foundRealSrc = true;
        break;
    }

    if(!foundRealSrc || !srcOutport)
    {
        return nullptr;
    }

    return srcOutport;
}
