#include "MIRRelation.h"

void MIRRelation::deleteSrc(MIRObject* srcObj)
{
    for (int i = 0; i < srcObjs.size(); i++)
    {
        if (srcObjs[i] == srcObj)
        {
            srcObjs.erase(srcObjs.begin() + i);
            srcStrs.erase(srcStrs.begin() + i);
            break;
        }
    }
}

void MIRRelation::deleteDst(MIRObject* dstObj)
{
    for (int i = 0; i < dstObjs.size(); i++)
    {
        if (dstObjs[i] == dstObj)
        {
            dstObjs.erase(dstObjs.begin() + i);
            dstStrs.erase(dstStrs.begin() + i);
            break;
        }
    }
}

void MIRRelation::release()
{
}
