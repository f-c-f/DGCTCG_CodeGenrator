#pragma once

#include <map>
#include <string>
#include <vector>

#include "../Common/tinyxml2.h"
#include "../Common/Utility.h"


class PTActor;
class PTInport;
class PTOutport;
class PTProperty;
class PTRelation;
class PTLink;
class PTObject;
class PTEntity;

class ParserForPtolemy
{
public:
	int parsePT(std::string filePath);

	enum {
		ErrorLoadPTModelFail = 190001,
		ErrorEntityPortHasNoAttributeClass,
		ErrorEntityPortHasNoAttributePortType,
		ErrorPTInportHasNoAttributeName,
		ErrorPTOutportHasNoAttributeName,
		ErrorPTPropertyHasNoAttributeName,
		ErrorPTPropertyHasNoAttributeClass,
		ErrorPTParameterHasNoAttributeValue,
        ErrorRelationHasNoAttributeName,

        ErrorLoadMapRulesFileFail,

	};
    PTEntity* rootEntity = nullptr;

	std::vector<PTEntity*> entityList;

	std::vector<PTObject*> objectList;

	void release();
private:

	int parseEntity(tinyxml2::XMLElement* root);
	int parseEntityPort(PTEntity* parent, tinyxml2::XMLElement* root);
	int parseActor(PTEntity* parent, tinyxml2::XMLElement* root);
	int parseActorPort(PTActor* parent, tinyxml2::XMLElement* root);
    
	int parseProperty(PTObject* parent, tinyxml2::XMLElement* root);


	int parseRelation(PTEntity* parent, tinyxml2::XMLElement* root);
	int parseLink(PTEntity* parent, tinyxml2::XMLElement* root);

    std::map<std::string, std::string> basicTypeNameMap = {
        std::pair<std::string, std::string>("int32", "i32"),
        std::pair<std::string, std::string>("int64", "i64"),
        std::pair<std::string, std::string>("float32", "f32"),
        std::pair<std::string, std::string>("float64", "f64"),
        std::pair<std::string, std::string>("single", "f32"),
        std::pair<std::string, std::string>("float", "f32"),
        std::pair<std::string, std::string>("double", "f64"),
    };
};
