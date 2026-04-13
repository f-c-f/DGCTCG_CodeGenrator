#pragma once

#include "ParserForSimulink.h"

#include <vector>
#include <string>


#include "SLSubsystem.h"
#include "SLBlock.h"
#include "SLLine.h"
#include "SLInport.h"
#include "SLOutport.h"
#include "SLActionPort.h"


#include "../MIRBasic/MIRObject.h"
#include "../MIRBasic/MIRModel.h"
#include "../MIRBasic/MIRFunction.h"
#include "../MIRBasic/MIRActor.h"
#include "../MIRBasic/MIRInport.h"
#include "../MIRBasic/MIRActionPort.h "
#include "../MIRBasic/MIROutport.h"
#include "../MIRBasic/MIRRelation.h"
#include "../MIRBasic/MIRFunctionDataflow.h"




class SLConverter
{
public:
	ParserForSimulink* parserForSimulink;
	MIRModel* mIRModel;

	enum {
		ErrorLineSrcDstStrInvalid = 80001,

	};

	int convert(ParserForSimulink* parserForSimulink, MIRModel* mIRModel);

	int convert(MIRModel* mIRModel);
	int convertSubsystem(SLSubsystem* subsystem, MIRModel* mIRModel);
	int convertBlock(SLBlock* block, MIRFunctionDataflow* mIRFunction);
	int convertLine(SLLine* line, MIRFunctionDataflow* mIRFunction);
	
	int convertSubsystemInport(SLInport* inport, MIRFunction* mIRFunction);
	int convertSubsystemOutport(SLOutport* outport, MIRFunction* mIRFunction);
	int convertSubsystemActionPort(SLActionPort* actionPort, MIRFunction* mIRFunction);

	int convertBlockInport(SLInport* inport, MIRActor* mIRActor);
	int convertBlockOutport(SLOutport* outport, MIRActor* mIRActor);
	int convertBlockActionPort(SLActionPort* actionPort, MIRActor* mIRActor);

	int convertParameter(SLObject* object, MIRObject* mIRObject);

	int convertLineSrcDstStr(std::string str, std::string &ret);

	int trimNameStr(std::string str, std::string &ret);
	int trimTypeStr(std::string str, std::string &ret);
};
