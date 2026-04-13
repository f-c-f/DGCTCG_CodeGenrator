#pragma once
#include <string>

#include "../Common/Utility.h"
#include "../Common/tinyxml2.h"

#include "CGTActorExeTransObject.h"
#include "CGTActorExeTransRoot.h"
#include "CGTActorExeTransModelInfo.h"
#include "CGTActorExeTransFunctionInfo.h"
#include "CGTActorExeTransActorInfo.h"
#include "CGTActorExeTransInport.h"
#include "CGTActorExeTransOutport.h"
#include "CGTActorExeTransSourceOutportInfo.h"
#include "CGTActorExeTransSourceOutport.h"


class CGTActorExeTransSaver
{
public:
	int save(const CGTActorExeTransRoot* transRoot, std::string& content);

	int saveModelInfo(const CGTActorExeTransModelInfo* modelInfo, tinyxml2::XMLElement* root, tinyxml2::XMLDocument* doc);
	int saveFunctionInfo(const CGTActorExeTransFunctionInfo* functionInfo, tinyxml2::XMLElement* root, tinyxml2::XMLDocument* doc);
	int saveActorInfo(const CGTActorExeTransActorInfo* actorInfo, tinyxml2::XMLElement* root, tinyxml2::XMLDocument* doc);
    int saveParameter(const CGTActorExeTransParameter* parameter, tinyxml2::XMLElement* root, tinyxml2::XMLDocument* doc);
    int saveInport(const CGTActorExeTransInport* inport, tinyxml2::XMLElement* root, tinyxml2::XMLDocument* doc);
	int saveOutport(const CGTActorExeTransOutport* outport, tinyxml2::XMLElement* root, tinyxml2::XMLDocument* doc);
	int saveActionPort(const CGTActorExeTransActionPort* actionPort, tinyxml2::XMLElement* root, tinyxml2::XMLDocument* doc);
	int saveSourceOutportInfo(const CGTActorExeTransSourceOutportInfo* sourceOutportInfo, tinyxml2::XMLElement* root, tinyxml2::XMLDocument* doc);
	int saveSourceOutport(const CGTActorExeTransSourceOutport* sourceOutport, tinyxml2::XMLElement* root, tinyxml2::XMLDocument* doc);

	enum {
		ErrorUnknownCGTActorExeTransFileRoot = 140001,
		ErrorCGTActorExeTransModelInfoHasNoAttributeName,
		ErrorCGTActorExeTransFunctionInfoHasNoAttributeName,
		ErrorCGTActorExeTransActorInfoHasNoAttributeName,
		ErrorCGTActorExeTransActorInfoHasNoAttributeType,
		ErrorCGTActorExeTransInportHasNoAttributeName,
		ErrorCGTActorExeTransInportHasNoAttributeType,
		ErrorCGTActorExeTransInportHasNoAttributeSourceOutport,
		ErrorCGTActorExeTransOutportHasNoAttributeName,
		ErrorCGTActorExeTransOutportHasNoAttributeType,
		ErrorCGTActorExeTransActionPortHasNoAttributeName,
		ErrorCGTActorExeTransActionPortHasNoAttributeType,
		ErrorCGTActorExeTransParameterHasNoAttributeName,
		ErrorCGTActorExeTransParameterHasNoAttributeValue,
		ErrorCGTActorExeTransSourceOutportHasNoAttributeName,
		ErrorCGTActorExeTransSourceOutportHasNoAttributeType,
		ErrorCGTActorExeTransSourceOutportHasNoAttributeActorType,
	};
};
