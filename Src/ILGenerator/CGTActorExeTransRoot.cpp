#include "CGTActorExeTransRoot.h"
#include "CGTActorExeTransModelInfo.h"
#include "CGTActorExeTransFunctionInfo.h"
#include "CGTActorExeTransActorInfo.h"
#include "CGTActorExeTransSourceOutportInfo.h"

void CGTActorExeTransRoot::release()
{
    if(modelInfo)
    {
        modelInfo->release();
        delete modelInfo;
    }
    if(functionInfo)
    {
        functionInfo->release();
        delete functionInfo;
    }
    if(actorInfo)
    {
        actorInfo->release();
        delete actorInfo;
    }
    if(sourceOutportInfo)
    {
        sourceOutportInfo->release();
        delete sourceOutportInfo;
    }

}
