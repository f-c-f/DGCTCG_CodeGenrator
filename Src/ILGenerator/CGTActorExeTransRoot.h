#pragma once

#include "CGTActorExeTransObject.h"

class CGTActorExeTransModelInfo;
class CGTActorExeTransFunctionInfo;
class CGTActorExeTransActorInfo;
class CGTActorExeTransSourceOutportInfo;
class CGTActorExeTransRoot : 
	public CGTActorExeTransObject
{
public:
	CGTActorExeTransModelInfo* modelInfo = nullptr;
	CGTActorExeTransFunctionInfo* functionInfo = nullptr;
	CGTActorExeTransActorInfo* actorInfo = nullptr;
	CGTActorExeTransSourceOutportInfo* sourceOutportInfo = nullptr;
	void release();
};
