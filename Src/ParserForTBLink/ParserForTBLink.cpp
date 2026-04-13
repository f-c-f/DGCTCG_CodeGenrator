#include "ParserForTBLink.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <queue>

#include "TBActor.h"
#include "TBInport.h"
#include "TBOutport.h"
#include "TBProperty.h"
#include "TBRelation.h"
#include "TBLinkNode.h"
#include "TBObject.h"
#include "TBModelEntirety.h"
#include "TBProgramModelPage.h"
#include "TBBranch.h"
#include "TBLinkInterface.h"

using namespace std;

int ParserForTBLink::parseTBLink(string filePath)
{
    int res;
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError ret = doc.LoadFile(filePath.c_str());
	if (ret != 0) {
		fprintf(stderr, "Fail to load TBLink model file: %s\n", filePath.c_str());
        fprintf(stderr, "%s\n", doc.ErrorStr());
		res = -ErrorLoadTBLinkModelFail;
		return res;
	}

	tinyxml2::XMLElement* root = doc.RootElement();
	if (!root) {
		fprintf(stderr, "No root element found in TBLink model file: %s\n", filePath.c_str());
		res = -ErrorLoadTBLinkModelFail;
		return res;
	}

	res = parseModelEntirety(root);
	if (res < 0) {
		return res;
	}

	return res;
}

int ParserForTBLink::parseModelEntirety(tinyxml2::XMLElement* root)
{
	int res;
	
	// 创建根实体
	rootEntity = new TBModelEntirety();
	objectList.push_back(rootEntity);
	rootEntity->objType = TBObject::TypeModelEntirety;
	rootEntity->parent = nullptr;
	
	// 解析ModelPage子元素
	tinyxml2::XMLElement* modelPage = root->FirstChildElement("ModelPage");
	while (modelPage) {
		res = parseProgramModelPage(rootEntity, modelPage);
		if (res < 0) {
			return res;
		}
		modelPage = modelPage->NextSiblingElement("ModelPage");
	}

	return 0;
}

int ParserForTBLink::parseProgramModelPage(TBModelEntirety* parent, tinyxml2::XMLElement* root)
{
	int res;
	TBProgramModelPage* programModelPage = new TBProgramModelPage();
	parent->programModelPage.push_back(programModelPage);
	objectList.push_back(programModelPage);
	programModelPage->objType = TBObject::TypeProgramModelPage;
	programModelPage->parent = parent;
	
	// 解析属性
	const char* id = root->Attribute("Id");
	const char* name = root->Attribute("ModelPageName");
	const char* type = root->Attribute("ModelPageType");
	
	if (id) programModelPage->id = atoll(id);
	if (name) programModelPage->name = string(name);
	if (type) programModelPage->parameters["ModelPageType"] = string(type);

	// 解析Actor子元素
	tinyxml2::XMLElement* actor = root->FirstChildElement("Actor");
	while (actor) {
		res = parseActor(programModelPage, actor);
		if (res < 0) {
			return res;
		}
		actor = actor->NextSiblingElement("Actor");
	}

	// 解析LinkNode子元素
	tinyxml2::XMLElement* linkNode = root->FirstChildElement("LinkNode");
	while (linkNode) {
		res = parseLinkNode(programModelPage, linkNode);
		if (res < 0) {
			return res;
		}
		linkNode = linkNode->NextSiblingElement("LinkNode");
	}

	// 解析Relation子元素
	tinyxml2::XMLElement* relation = root->FirstChildElement("Relation");
	while (relation) {
		res = parseRelation(programModelPage, relation);
		if (res < 0) {
			return res;
		}
		relation = relation->NextSiblingElement("Relation");
	}

	return 0;
}

int ParserForTBLink::parseActor(TBProgramModelPage* parent, tinyxml2::XMLElement* root)
{
	int res;
	TBActor* actor = new TBActor();
	parent->actors.push_back(actor);
	objectList.push_back(actor);
	actor->objType = TBObject::TypeActor;
	actor->parent = parent;
	
	// 解析属性
	const char* id = root->Attribute("Id");
	const char* type = root->Attribute("Type");
	const char* name = root->Attribute("Name");
	const char* x = root->Attribute("X");
	const char* y = root->Attribute("Y");
	const char* width = root->Attribute("Width");
	const char* height = root->Attribute("Height");
	
	if (id) actor->id = atoll(id);
	if (type) actor->type = string(type);
	if (name) actor->name = string(name);
	if (x) actor->parameters["X"] = string(x);
	if (y) actor->parameters["Y"] = string(y);
	if (width) actor->parameters["Width"] = string(width);
	if (height) actor->parameters["Height"] = string(height);

	// 解析LinkInterface子元素
	tinyxml2::XMLElement* linkInterface = root->FirstChildElement("LinkInterface");
	while (linkInterface) {
		res = parseLinkInterface(actor, linkInterface);
		if (res < 0) {
			return res;
		}
		linkInterface = linkInterface->NextSiblingElement("LinkInterface");
	}

	// 解析Port子元素
	tinyxml2::XMLElement* port = root->FirstChildElement("Port");
	while (port) {
		const char* portType = port->Attribute("PortType");
		if (portType) {
			if (string(portType) == "TypeInport") {
				res = parseInport(actor, port);
			} else if (string(portType) == "TypeOutport") {
				res = parseOutport(actor, port);
			}
			if (res < 0) {
				return res;
			}
		}
		port = port->NextSiblingElement("Port");
	}

	// 解析Branch子元素
	tinyxml2::XMLElement* branch = root->FirstChildElement("Branch");
	while (branch) {
		res = parseBranch(actor, branch);
		if (res < 0) {
			return res;
		}
		branch = branch->NextSiblingElement("Branch");
	}

	// 解析Property子元素
	tinyxml2::XMLElement* property = root->FirstChildElement("Property");
	while (property) {
		res = parseProperty(actor, property);
		if (res < 0) {
			return res;
		}
		property = property->NextSiblingElement("Property");
	}

	return 0;
}

int ParserForTBLink::parseLinkInterface(TBActor* parent, tinyxml2::XMLElement* root)
{
	TBLinkInterface* linkInterface = new TBLinkInterface();
	parent->linkInterfaces.push_back(linkInterface);
	objectList.push_back(linkInterface);
	linkInterface->objType = TBObject::TypeLinkInterface;
	linkInterface->parent = parent;
	
	// 解析属性
	const char* id = root->Attribute("Id");
	const char* linkDirection = root->Attribute("LinkDirection");
	const char* linkType = root->Attribute("LinkType");
	const char* linkRelationType = root->Attribute("LinkRelationType");
	
	if (id) linkInterface->id = atoll(id);
	if (linkDirection) linkInterface->parameters["LinkDirection"] = string(linkDirection);
	if (linkType) linkInterface->parameters["LinkType"] = string(linkType);
	if (linkRelationType) linkInterface->linkRelationType = string(linkRelationType);

	return 0;
}

int ParserForTBLink::parseInport(TBActor* parent, tinyxml2::XMLElement* root)
{
	int res;
	TBInport* inport = new TBInport();
	parent->inports.push_back(inport);
	objectList.push_back(inport);
	inport->objType = TBObject::TypeInport;
	inport->parent = parent;
	
	// 解析属性
	const char* id = root->Attribute("Id");
	const char* name = root->Attribute("Name");
	const char* dataType = root->Attribute("DataType");
	const char* value = root->Attribute("Value");
	const char* arraySize = root->Attribute("ArraySize");
	
	if (id) inport->id = atoll(id);
	if (name) inport->name = string(name);
	if (dataType) inport->dataType = string(dataType);
	if (value) inport->value = string(value);
	if (arraySize) inport->parameters["ArraySize"] = string(arraySize);

	// 解析LinkInterface子元素
	tinyxml2::XMLElement* linkInterface = root->FirstChildElement("LinkInterface");
	while (linkInterface) {
		res = parseLinkInterface(inport, linkInterface);
		if (res < 0) {
			return res;
		}
		linkInterface = linkInterface->NextSiblingElement("LinkInterface");
	}

	return 0;
}

int ParserForTBLink::parseLinkInterface(TBInport* parent, tinyxml2::XMLElement* root)
{
	TBLinkInterface* linkInterface = new TBLinkInterface();
	parent->linkInterfaces.push_back(linkInterface);
	objectList.push_back(linkInterface);
	linkInterface->objType = TBObject::TypeLinkInterface;
	linkInterface->parent = parent;
	
	// 解析属性
	const char* id = root->Attribute("Id");
	const char* linkDirection = root->Attribute("LinkDirection");
	const char* linkType = root->Attribute("LinkType");
	const char* linkRelationType = root->Attribute("LinkRelationType");
	
	if (id) linkInterface->id = atoll(id);
	if (linkDirection) linkInterface->parameters["LinkDirection"] = string(linkDirection);
	if (linkType) linkInterface->parameters["LinkType"] = string(linkType);
	if (linkRelationType) linkInterface->linkRelationType = string(linkRelationType);

	return 0;
}

int ParserForTBLink::parseOutport(TBActor* parent, tinyxml2::XMLElement* root)
{
	int res;
	TBOutport* outport = new TBOutport();
	parent->outports.push_back(outport);
	objectList.push_back(outport);
	outport->objType = TBObject::TypeOutport;
	outport->parent = parent;
	
	// 解析属性
	const char* id = root->Attribute("Id");
	const char* name = root->Attribute("Name");
	const char* dataType = root->Attribute("DataType");
	const char* value = root->Attribute("Value");
	const char* arraySize = root->Attribute("ArraySize");
	
	if (id) outport->id = atoll(id);
	if (name) outport->name = string(name);
	if (dataType) outport->dataType = string(dataType);
	if (value) outport->value = string(value);
	if (arraySize) outport->parameters["ArraySize"] = string(arraySize);

	// 解析LinkInterface子元素
	tinyxml2::XMLElement* linkInterface = root->FirstChildElement("LinkInterface");
	while (linkInterface) {
		res = parseLinkInterface(outport, linkInterface);
		if (res < 0) {
			return res;
		}
		linkInterface = linkInterface->NextSiblingElement("LinkInterface");
	}

	return 0;
}

int ParserForTBLink::parseLinkInterface(TBOutport* parent, tinyxml2::XMLElement* root)
{
	TBLinkInterface* linkInterface = new TBLinkInterface();
	parent->linkInterfaces.push_back(linkInterface);
	objectList.push_back(linkInterface);
	linkInterface->objType = TBObject::TypeLinkInterface;
	linkInterface->parent = parent;
	
	// 解析属性
	const char* id = root->Attribute("Id");
	const char* linkDirection = root->Attribute("LinkDirection");
	const char* linkType = root->Attribute("LinkType");
	const char* linkRelationType = root->Attribute("LinkRelationType");
	
	if (id) linkInterface->id = atoll(id);
	if (linkDirection) linkInterface->parameters["LinkDirection"] = string(linkDirection);
	if (linkType) linkInterface->parameters["LinkType"] = string(linkType);
	if (linkRelationType) linkInterface->linkRelationType = string(linkRelationType);

	return 0;
}

int ParserForTBLink::parseBranch(TBActor* parent, tinyxml2::XMLElement* root)
{
	int res;
	TBBranch* branch = new TBBranch();
	parent->branches.push_back(branch);
	objectList.push_back(branch);
	branch->objType = TBObject::TypeBranch;
	branch->parent = parent;
	
	// 解析属性
	const char* id = root->Attribute("Id");
	const char* branchId = root->Attribute("BranchId");
	
	if (id) branch->id = atoll(id);
	if (branchId) branch->branchId = atoi(branchId);

	// 解析LinkInterface子元素
	tinyxml2::XMLElement* linkInterface = root->FirstChildElement("LinkInterface");
	while (linkInterface) {
		res = parseLinkInterface(branch, linkInterface);
		if (res < 0) {
			return res;
		}
		linkInterface = linkInterface->NextSiblingElement("LinkInterface");
	}

	return 0;
}

int ParserForTBLink::parseLinkInterface(TBBranch* parent, tinyxml2::XMLElement* root)
{
	TBLinkInterface* linkInterface = new TBLinkInterface();
	parent->linkInterfaces.push_back(linkInterface);
	objectList.push_back(linkInterface);
	linkInterface->objType = TBObject::TypeLinkInterface;
	linkInterface->parent = parent;
	
	// 解析属性
	const char* id = root->Attribute("Id");
	const char* linkDirection = root->Attribute("LinkDirection");
	const char* linkType = root->Attribute("LinkType");
	const char* linkRelationType = root->Attribute("LinkRelationType");
	
	if (id) linkInterface->id = atoll(id);
	if (linkDirection) linkInterface->parameters["LinkDirection"] = string(linkDirection);
	if (linkType) linkInterface->parameters["LinkType"] = string(linkType);
	if (linkRelationType) linkInterface->linkRelationType = string(linkRelationType);

	return 0;
}

int ParserForTBLink::parseProperty(TBActor* parent, tinyxml2::XMLElement* root)
{
	TBProperty* property = new TBProperty();
	parent->propertys.push_back(property);
	objectList.push_back(property);
	property->objType = TBObject::TypeProperty;
	property->parent = parent;
	
	// 解析属性
	const char* id = root->Attribute("Id");
	const char* name = root->Attribute("Name");
	const char* type = root->Attribute("Type");
	const char* value = root->Attribute("Value");
	const char* needConfirm = root->Attribute("NeedConfirm");
	
	if (id) property->id = atoll(id);
	if (name) property->name = string(name);
	if (type) property->parameters["Type"] = string(type);
	if (value) property->value = string(value);
	if (needConfirm) property->parameters["NeedConfirm"] = string(needConfirm);

	return 0;
}

int ParserForTBLink::parseRelation(TBProgramModelPage* parent, tinyxml2::XMLElement* root)
{
	TBRelation* relation = new TBRelation();
	parent->relations.push_back(relation);
	objectList.push_back(relation);
	relation->objType = TBObject::TypeRelation;
	relation->parent = parent;
	
	// 解析属性
	const char* id = root->Attribute("Id");
	const char* relationType = root->Attribute("RelationType");
	const char* startLinkInterface = root->Attribute("StartLinkInterface");
	const char* endLinkInterface = root->Attribute("EndLinkInterface");
	
	if (id) relation->id = atoll(id);
	if (relationType) relation->relationType = string(relationType);
	if (startLinkInterface) relation->startLinkInterface = atoi(startLinkInterface);
	if (endLinkInterface) relation->endLinkInterface = atoi(endLinkInterface);

	return 0;
}

int ParserForTBLink::parseLinkNode(TBProgramModelPage* parent, tinyxml2::XMLElement* root)
{
	int res;
	TBLinkNode* linkNode = new TBLinkNode();
	parent->linkNodes.push_back(linkNode);
	objectList.push_back(linkNode);
	linkNode->objType = TBObject::TypeLinkNode;
	linkNode->parent = parent;
	
	// 解析属性
	const char* id = root->Attribute("Id");
	const char* x = root->Attribute("X");
	const char* y = root->Attribute("Y");
	const char* linkNodeType = root->Attribute("LinkNodeType");
	
	if (id) linkNode->id = atoll(id);
	if (x) linkNode->parameters["X"] = string(x);
	if (y) linkNode->parameters["Y"] = string(y);
	if (linkNodeType) linkNode->linkNodeType = string(linkNodeType);

	// 解析LinkInterface子元素
	tinyxml2::XMLElement* linkInterface = root->FirstChildElement("LinkInterface");
	while (linkInterface) {
		res = parseLinkInterface(linkNode, linkInterface);
		if (res < 0) {
			return res;
		}
		linkInterface = linkInterface->NextSiblingElement("LinkInterface");
	}

	return 0;
}

int ParserForTBLink::parseLinkInterface(TBLinkNode* parent, tinyxml2::XMLElement* root)
{
	TBLinkInterface* linkInterface = new TBLinkInterface();
	parent->linkInterfaces.push_back(linkInterface);
	objectList.push_back(linkInterface);
	linkInterface->objType = TBObject::TypeLinkInterface;
	linkInterface->parent = parent;
	
	// 解析属性
	const char* id = root->Attribute("Id");
	const char* linkDirection = root->Attribute("LinkDirection");
	const char* linkType = root->Attribute("LinkType");
	const char* linkRelationType = root->Attribute("LinkRelationType");
	
	if (id) linkInterface->id = atoll(id);
	if (linkDirection) linkInterface->parameters["LinkDirection"] = string(linkDirection);
	if (linkType) linkInterface->parameters["LinkType"] = string(linkType);
	if (linkRelationType) linkInterface->linkRelationType = string(linkRelationType);

	return 0;
}

void ParserForTBLink::release()
{
	for (auto obj : objectList) {
		if (obj) {
			delete obj;
		}
	}
	objectList.clear();
	rootEntity = nullptr;
}
