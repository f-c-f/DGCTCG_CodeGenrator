#pragma once
#include "../Common/Utility.h"
#include "../Common/tinyxml2.h"

#include "CGTActorExeTransRoot.h"
#include "CGTActorExeTransModelInfo.h"
#include "CGTActorExeTransFunctionInfo.h"
#include "CGTActorExeTransActorInfo.h"
#include "CGTActorExeTransInport.h"
#include "CGTActorExeTransOutport.h"
#include "CGTActorExeTransSourceOutportInfo.h"
#include "CGTActorExeTransSourceOutport.h"

class CGTActorExeTransParser {
public:
	//CGTActorExeTransParser类的主要功能，即：解析ILGen同DLL文件之间传输的中间表示，CGTActorExeTrans
	int parseCGTActorExeTrans(std::string content);
	int parseCGTActorExeTrans(const char* content, int length);
	int parseCGTActorExeTrans(tinyxml2::XMLElement* root);

	enum {
		ErrorLoadCGTActorExeTransFileFail =130001,
		ErrorCGTActorExeTransFileMissModelInfo,
		ErrorCGTActorExeTransModelInfoHasNoAttributeName,
		ErrorCGTActorExeTransFileMissFunctionInfo,
		ErrorCGTActorExeTransFunctionInfoHasNoAttributeName,
		ErrorCGTActorExeTransFileMissActorInfo,
		ErrorCGTActorExeTransActorInfoHasNoAttributeName,
		ErrorCGTActorExeTransActorInfoHasNoAttributeType,
		ErrorCGTActorExeTransActorInfoMissInport,
		ErrorCGTActorExeTransInportHasNoAttributeName,
		ErrorCGTActorExeTransInportHasNoAttributeType,
		ErrorCGTActorExeTransInportHasNoAttributeSourceOutport,
		ErrorCGTActorExeTransActorInfoMissOutport,
		ErrorCGTActorExeTransOutportHasNoAttributeName,
		ErrorCGTActorExeTransOutportHasNoAttributeType,
		ErrorCGTActorExeTransActionPortHasNoAttributeName,
		ErrorCGTActorExeTransActionPortHasNoAttributeType,
		ErrorCGTActorExeTransActionPortHasNoAttributeSourceOutport,
		ErrorCGTActorExeTransParameterHasNoAttributeName,
		ErrorCGTActorExeTransParameterHasNoAttributeValue,
		ErrorCGTActorExeTransFileMissSourceOutportInfo,
		ErrorCGTActorExeTransSourceOutportInfoMissSourceOutport,
		ErrorCGTActorExeTransSourceOutportHasNoAttributeName,
		ErrorCGTActorExeTransSourceOutportHasNoAttributeType,
		ErrorCGTActorExeTransSourceOutportHasNoAttributeActorType,
	};

	void release();
	std::string getErrorStr();

	CGTActorExeTransRoot* transRoot = nullptr;
private:
	int errorCode = 0;
	void setCurrentError(int errorCode);

	int currentReadXMLLineNum = -1;
	tinyxml2::XMLElement* currentReadXMLElement = nullptr;
	void setCurrentReadXMLElement(tinyxml2::XMLElement* ele);

	int parseCGTActorExeTransModelInfo(tinyxml2::XMLElement* root);
	int parseCGTActorExeTransFunctionInfo(tinyxml2::XMLElement* root);
	int parseCGTActorExeTransActorInfo(tinyxml2::XMLElement* root);
	int parseCGTActorExeTransParameter(CGTActorExeTransActorInfo* actorInfo, tinyxml2::XMLElement* root);
	int parseCGTActorExeTransInport(CGTActorExeTransActorInfo* actorInfo, tinyxml2::XMLElement* root);
	int parseCGTActorExeTransOutport(CGTActorExeTransActorInfo* actorInfo, tinyxml2::XMLElement* root);
	int parseCGTActorExeTransActionPort(CGTActorExeTransActorInfo* actorInfo, tinyxml2::XMLElement* root);
	int parseCGTActorExeTransSourceOutportInfo(tinyxml2::XMLElement* root);
	int parseCGTActorExeTransSourceOutport(CGTActorExeTransSourceOutportInfo* sourceOutportInfo, tinyxml2::XMLElement* root);

    std::vector<int> parseUtilityArraySize(std::string str) const;
};
