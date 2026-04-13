#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "../Common/Utility.h"
#include "../Common/tinyxml2.h"

#include "MIRObject.h"
#include "MIRModel.h"
#include "MIRFunction.h"
#include "MIRActor.h"
#include "MIRInport.h"
#include "MIROutport.h"
#include "MIRActionPort.h "
#include "MIRRelation.h"
#include "MIRSaver.h"


class MIRFunctionStateflow;
class MIRFunctionDataflow;

class MIRSaver
{
public:
	int save(MIRModel* model, std::string outputFilePath);

	enum {
		ErrorUnknownMIRFunctionType = 90001,
	};

	int saveModelEntity(MIRModel* model, tinyxml2::XMLElement* root, tinyxml2::XMLDocument* doc);
	int saveFunction(MIRFunction* function, tinyxml2::XMLElement* root, tinyxml2::XMLDocument* doc);
	int saveFunctionDataflow(MIRFunctionDataflow* function, tinyxml2::XMLElement* root, tinyxml2::XMLDocument* doc);
	int saveFunctionStateflow(MIRFunctionStateflow* function, tinyxml2::XMLElement* root, tinyxml2::XMLDocument* doc);
	int saveActor(MIRActor* actor, tinyxml2::XMLElement* root, tinyxml2::XMLDocument* doc);
    int saveActorParameter(MIRParameter* parameter, tinyxml2::XMLElement* root, tinyxml2::XMLDocument* doc);

	int saveRelation(MIRRelation* relation, tinyxml2::XMLElement* root, tinyxml2::XMLDocument* doc);
	int saveInport(MIRInport* inport, tinyxml2::XMLElement* root, tinyxml2::XMLDocument* doc);
	int saveOutport(MIROutport* outport, tinyxml2::XMLElement* root, tinyxml2::XMLDocument* doc);
	int saveActionPort(MIRActionPort* actionPort, tinyxml2::XMLElement* root, tinyxml2::XMLDocument* doc);

	int saveXMLParameter(MIRObject* object, tinyxml2::XMLElement* root, tinyxml2::XMLDocument* doc);

	static std::vector<std::string> filterParameter;
};

