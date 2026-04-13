#pragma once

#include <map>
#include <string>
#include <vector>

#include "../Common/tinyxml2.h"
#include "../Common/Utility.h"


class TBActor;
class TBInport;
class TBOutport;
class TBProperty;
class TBRelation;
class TBLink;
class TBObject;
class TBModelEntirety;
class TBProgramModelPage;
class TBBranch;
class TBLinkNode;
class TBLinkInterface;


class ParserForTBLink
{
public:
	int parseTBLink(std::string filePath);

	enum {
		ErrorLoadTBLinkModelFail = 190001,
		ErrorEntityPortHasNoAttributeClass,
		ErrorEntityPortHasNoAttributePortType,
		ErrorTBInportHasNoAttributeName,
		ErrorTBOutportHasNoAttributeName,
		ErrorTBPropertyHasNoAttributeName,
		ErrorTBPropertyHasNoAttributeClass,
		ErrorPTParameterHasNoAttributeValue,
        ErrorRelationHasNoAttributeName,

        ErrorLoadMapRulesFileFail,

	};
    TBModelEntirety* rootEntity = nullptr;

	std::vector<TBObject*> objectList;

	void release();
private:

	int parseModelEntirety(tinyxml2::XMLElement* root);
	int parseProgramModelPage(TBModelEntirety* parent, tinyxml2::XMLElement* root);
	int parseProperty(TBProgramModelPage* parent, tinyxml2::XMLElement* root);

	int parseActor(TBProgramModelPage* parent, tinyxml2::XMLElement* root);
	int parseLinkInterface(TBActor* parent, tinyxml2::XMLElement* root);
	int parseInport(TBActor* parent, tinyxml2::XMLElement* root);
	int parseLinkInterface(TBInport* parent, tinyxml2::XMLElement* root);
	int parseOutport(TBActor* parent, tinyxml2::XMLElement* root);
	int parseLinkInterface(TBOutport* parent, tinyxml2::XMLElement* root);
	int parseBranch(TBActor* parent, tinyxml2::XMLElement* root);
	int parseLinkInterface(TBBranch* parent, tinyxml2::XMLElement* root);
	int parseProperty(TBActor* parent, tinyxml2::XMLElement* root);


	int parseRelation(TBProgramModelPage* parent, tinyxml2::XMLElement* root);
	int parseLinkNode(TBProgramModelPage* parent, tinyxml2::XMLElement* root);
	int parseLinkInterface(TBLinkNode* parent, tinyxml2::XMLElement* root);


};
