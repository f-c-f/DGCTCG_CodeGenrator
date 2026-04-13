#include "MIRScheduleGraphInport.h"

#include "BranchInfo.h"
#include "CodeGenerationTable.h"
#include "MIRScheduleGraphDataflowNode.h"
#include <algorithm>

using namespace std;

vector<MIRScheduleGraphOutport*> MIRScheduleGraphInport::getRealSrcOutport(const CGTActorExe* containerActorExe) const
{
    vector<MIRScheduleGraphOutport*> ret;
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
        ret.push_back(srcOutport);
    }
    

    return ret;
}


void MIRScheduleGraphInport::release()
{
}
