#include "CGTActorExeTransSourceOutportInfo.h"

void CGTActorExeTransSourceOutportInfo::release()
{
	for (auto sourceOutport : sourceOutports)
	{
		sourceOutport->release();
		delete sourceOutport;
	}
}
