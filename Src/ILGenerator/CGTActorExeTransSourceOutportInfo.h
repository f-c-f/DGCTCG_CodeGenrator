#pragma once

#include "CGTActorExeTransObject.h"
#include "CGTActorExeTransSourceOutport.h"

class CGTActorExeTransSourceOutportInfo 
	:public CGTActorExeTransObject 
{
public:
	std::vector<CGTActorExeTransSourceOutport*> sourceOutports;

	void release();
};
