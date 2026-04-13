#pragma once
#include <string>
#include <vector>
#include <map>
#include "../Common/tinyxml2.h"
#include <iostream>

#include "MIRObject.h"
#include "MIRModel.h"
#include "MIRFunction.h"
#include "MIRActor.h"
#include "MIRInport.h"
#include "MIROutport.h"
#include "MIRActionPort.h "
#include "MIRRelation.h"
#include "MIRSaver.h"

#include "../Common/Utility.h"


class MIRFunctionDataflow;

class MIRParser
{
public:
	//MIRParser类的主要功能，即：解析模型中间表示MIR
	int parseMIR(std::string xmlFilePath, MIRModel* retModel);

	int parseMIR(tinyxml2::XMLElement* root);

	MIRModel* model;

	enum {
		ErrorLoadMIRFileFail = 110001,
		ErrorMIRFunctionMissModelEntity,
		ErrorMIRModelEntityMissFunction,
		ErrorMIRModelEntityHasNoAttributeMainFunction,
		ErrorMIRFunctionHasNoAttributeType,
		ErrorMIRFunctionHasNoAttributeName,
		ErrorMIRFunctionMissActor,
		ErrorMIRActorHasNoAttributeName,
		ErrorMIRActorHasNoAttributeType,
		ErrorMIRFunctionMissRelation,
		ErrorMIRRelationMissSrc,
		ErrorMIRRelationMissDst,
		ErrorMIRRelationSrcHasNoAttributeSrc,
		ErrorMIRRelationSrcHasNoAttributeDst,
		ErrorMIRFunctionMissInport,
		ErrorMIRInportHasNoAttributeName,
		ErrorMIRInportHasNoAttributeType,
		ErrorMIRFunctionMissOutport,
		ErrorMIROutportHasNoAttributeName,
		ErrorMIROutportHasNoAttributeType,
		ErrorMIRFunctionMissActionPort,
		ErrorMIRActionPortHasNoAttributeName,
		ErrorMIRActionPortHasNoAttributeType,
		ErrorMIRParameterHasNoAttributeName,
		ErrorMIRParameterHasNoAttributeValue,
		ErrorMIRActorMissInport,
		ErrorMIRActorMissOutport,
		ErrorMIRActorMissActionPort,
		ErrorMIRObjectMissParameter,
		ErrorMIRParameterOfXMLHasNoAttributeName,
		ErrorMIRParameterOfXMLHasNoAttributeValue,
		ErrorMIRRelationCanNotFindSrc,
		ErrorMIRRelationCanNotFindDst,
	};

	void release();
	
	std::string getErrorStr();
private:
	int errorCode = 0;
	void setCurrentError(int errorCode);

	int currentReadXMLLineNum = -1;
	tinyxml2::XMLElement* currentReadXMLElement = NULL;
	void setCurrentReadXMLElement(tinyxml2::XMLElement* ele);

	int parseModelEntity(MIRModel* model, tinyxml2::XMLElement* root);
	int parseFunction(MIRModel* model, tinyxml2::XMLElement* root);
	int parseFunctionDataflow(MIRModel* model, tinyxml2::XMLElement* c);
	int parseFunctionStateflow(MIRModel* model, tinyxml2::XMLElement* c);
	int parseActor(MIRFunctionDataflow* function, tinyxml2::XMLElement* root);
	int parseFunctionRelation(MIRFunctionDataflow* function, tinyxml2::XMLElement* root);
	int parseFunctionInport(MIRFunction* function, tinyxml2::XMLElement* root);
	int parseFunctionOutport(MIRFunction* function, tinyxml2::XMLElement* root);
	int parseFunctionActionPort(MIRFunction* function, tinyxml2::XMLElement* root);
	int parseActorInport(MIRActor* actor, tinyxml2::XMLElement* root);
	int parseActorOutport(MIRActor* actor, tinyxml2::XMLElement* root);
	int parseActorParameter(MIRActor* actor, tinyxml2::XMLElement* root);
	int parseActorActionPort(MIRActor* actor, tinyxml2::XMLElement* root);
	int parseParameter(MIRObject* object, tinyxml2::XMLElement* root);

	//通过relation的src和dst的名字找到对应的MIRInport、MIROutport、MIRActionPort
	int calculateRelationObj();
	int calculateRelationObj(MIRRelation* relation);
	int calculateRelationObjSrc(MIRRelation* relation, std::string& srcStr);
	int calculateRelationObjDst(MIRRelation* relation, std::string& dstStr);

	std::vector<MIRObject*> objectList;
};

