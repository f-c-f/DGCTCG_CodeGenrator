#include "ParserForPtolemy.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <queue>

#include "PTInport.h"
#include "PTOutport.h"
#include "PTProperty.h"
#include "PTRelation.h"
#include "PTLink.h"
#include "PTObject.h"
#include "PTEntity.h"
#include "PTActor.h"


using namespace std;

int ParserForPtolemy::parsePT(string filePath)
{
    int res;
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError ret = doc.LoadFile(filePath.c_str());
	if (ret != 0) {

		fprintf(stderr, "Fail to load model file: %s\n", filePath.c_str());
        fprintf(stderr, "%s\n", doc.ErrorStr());
		res = -ErrorLoadPTModelFail;
		return res;
	}

	tinyxml2::XMLElement* root = doc.RootElement();
	//tinyxml2::XMLPrinter printer;
	//doc.Print(&printer);
	//cout << printer.CStr() << endl << "===========================" << endl;

	//cout << subsystemName << endl;


    tinyxml2::XMLElement *entity = doc.FirstChildElement("entity");
	while (entity)
	{
		res = parseEntity(entity);
        if(res < 0)
	    {
		    return res;
	    }
		entity = entity->NextSiblingElement("entity");
	}

	return res;
}

int ParserForPtolemy::parseEntity(tinyxml2::XMLElement* root)
{
	int res;
	PTEntity* entity = new PTEntity();
	entityList.push_back(entity);
	objectList.push_back(entity);
	entity->objType = PTObject::TypeEntity;
	entity->parent = NULL;
	entity->name = string(root->Attribute("name"));
	entity->className = string(root->Attribute("class"));
    vector<string> classNameSp = stringSplit(entity->className, ".");
    entity->classType = classNameSp[classNameSp.size() - 1];

    if(!rootEntity)
        rootEntity = entity;

    res = parseEntityPort(entity, root);
    if(res < 0)
		return res;


    tinyxml2::XMLElement *c = root->FirstChildElement("entity");
	while (c)
	{
        string strClass = string(c->Attribute("class"));
        if(strClass == "ptolemy.actor.TypedCompositeActor")
        {
            res = parseEntity(c);
            if(res < 0)
	        {
		        return res;
	        }

        }
        res = parseActor(entity ,c);
        if(res < 0)
	    {
		    return res;
	    }
		
		c = c->NextSiblingElement("entity");
	}

    
	res = parseProperty(entity, root);
	if(res < 0)
		return res;

    res = parseLink(entity, root);
	if(res < 0)
		return res;
    

	return 1;
}

int ParserForPtolemy::parseEntityPort(PTEntity* parent, tinyxml2::XMLElement* root)
{
	int res;
	tinyxml2::XMLElement *c = root->FirstChildElement("port");
	while (c)
	{
        string strTypedIOPort;
        if(c->FindAttribute("class"))
			strTypedIOPort = string(c->Attribute("class"));
		else
			return -ErrorEntityPortHasNoAttributeClass;

        if(strTypedIOPort != "ptolemy.actor.TypedIOPort")
        {
            c = c->NextSiblingElement("port");
            continue;
        }

        string portDataType = "float";
        string portType;
        tinyxml2::XMLElement *cc = c->FirstChildElement("property");
        while(cc)
        {
            string nameStr;
			nameStr = string(cc->Attribute("name"));
            if(nameStr == "output") {
                portType = "Outport";
            } else if(nameStr == "input") {
                portType = "Inport";
            } else if(nameStr == "_type") {
                portDataType = string(cc->Attribute("value"));
                break;
            }
            cc = cc->NextSiblingElement("property");
        }
        if(portType.empty())
        {
            return -ErrorEntityPortHasNoAttributePortType;
        }



		if(portType == "Inport")
		{
			PTInport* inport = new PTInport();
			parent->inports.push_back(inport);
			objectList.push_back(inport);
			inport->parent = parent;
			inport->objType = PTObject::TypeInport;

			if(c->FindAttribute("name"))
				inport->name = string(c->Attribute("name"));
			else
				return -ErrorPTInportHasNoAttributeName;

            inport->type = portDataType;

            res = parseProperty(inport, c);
            if(res < 0)
		        return res;
		}
		else if(portType == "Outport")
		{
			PTOutport* outport = new PTOutport();
			parent->outports.push_back(outport);
			objectList.push_back(outport);
			outport->parent = parent;
			outport->objType = PTObject::TypeOutport;

			if(c->FindAttribute("name"))
				outport->name = string(c->Attribute("name"));
			else
				return -ErrorPTOutportHasNoAttributeName;

            outport->type = portDataType;

            res = parseProperty(outport, c);
            if(res < 0)
		        return res;
		}
		
		c = c->NextSiblingElement("port");
	}
	return 1;
}


int ParserForPtolemy::parseProperty(PTObject* parent, tinyxml2::XMLElement* root)
{
	tinyxml2::XMLElement *c = root->FirstChildElement("property");
	while (c)
	{
		string name;
		string strClass;
		string value;
		if(c->FindAttribute("name"))
			name = string(c->Attribute("name"));
		else
			return -ErrorPTPropertyHasNoAttributeName;
        if(c->FindAttribute("class"))
			strClass = string(c->Attribute("class"));

        if(c->FindAttribute("value"))
			value = string(c->Attribute("value"));

        PTProperty* property = new PTProperty();
        property->objType = PTObject::TypeProperty;
        parent->propertys.push_back(property);
        objectList.push_back(property);
        property->parent = parent;
        property->name = name;
        property->className = strClass;
        property->value = value;


        if(strClass != "ptolemy.data.expr.Parameter" &&
            strClass != "ptolemy.kernel.util.StringAttribute" &&
            strClass != "ptolemy.actor.parameters.PortParameter")
        {
            c = c->NextSiblingElement("property");
            continue;
        }

        
		//if(value.empty())
		//	return -ErrorPTParameterHasNoAttributeValue;

        parent->parameters[name] = value;


		c = c->NextSiblingElement("property");
	}
    
	return 1;
}

int ParserForPtolemy::parseActor(PTEntity* parent, tinyxml2::XMLElement* root)
{
	int res;
	PTActor* actor = new PTActor();
	objectList.push_back(actor);
	actor->objType = PTObject::TypeActor;
    parent->actors.push_back(actor);
	actor->parent = parent;
	actor->name = string(root->Attribute("name"));
	actor->className = string(root->Attribute("class"));
    vector<string> classNameSp = stringSplit(actor->className, ".");
    actor->type = classNameSp[classNameSp.size() - 1];

    res = parseActorPort(actor, root);
    if(res < 0)
		return res;


	res = parseProperty(actor, root);
	if(res < 0)
		return res;


	return 1;
}

int ParserForPtolemy::parseActorPort(PTActor* parent, tinyxml2::XMLElement* root)
{
	int res;
	tinyxml2::XMLElement *c = root->FirstChildElement("port");
	while (c)
	{
        string strTypedIOPort;
        if(c->FindAttribute("class"))
			strTypedIOPort = string(c->Attribute("class"));
		else
			return -ErrorEntityPortHasNoAttributeClass;

        if(strTypedIOPort != "ptolemy.actor.TypedIOPort")
        {
            c = c->NextSiblingElement("port");
            continue;
        }

        string portDataType = "#Default";
        string portType;
        tinyxml2::XMLElement *cc = c->FirstChildElement("property");
        while(cc)
        {
            string nameStr;
			nameStr = string(cc->Attribute("name"));
            if(nameStr == "output") {
                portType = "Outport";
            } else if(nameStr == "input") {
                portType = "Inport";
            } else if(nameStr == "_type") {
                portDataType = string(cc->Attribute("value"));
                break;
            }
            cc = cc->NextSiblingElement("property");
        }
        if(portType.empty())
        {
            return -ErrorEntityPortHasNoAttributePortType;
        }



		if(portType == "Inport")
		{
			PTInport* inport = new PTInport();
			parent->inports.push_back(inport);
			objectList.push_back(inport);
			inport->parent = parent;
			inport->objType = PTObject::TypeInport;

			if(c->FindAttribute("name"))
				inport->name = string(c->Attribute("name"));
			else
				return -ErrorPTInportHasNoAttributeName;

            inport->type = portDataType;


            res = parseProperty(inport, c);
            if(res < 0)
		        return res;
		}
		else if(portType == "Outport")
		{
			PTOutport* outport = new PTOutport();
			parent->outports.push_back(outport);
			objectList.push_back(outport);
			outport->parent = parent;
			outport->objType = PTObject::TypeOutport;

			if(c->FindAttribute("name"))
				outport->name = string(c->Attribute("name"));
			else
				return -ErrorPTOutportHasNoAttributeName;

            outport->type = portDataType;

            res = parseProperty(outport, c);
            if(res < 0)
		        return res;
		}
		
		c = c->NextSiblingElement("port");
	}
	return 1;
}



int ParserForPtolemy::parseRelation(PTEntity* parent, tinyxml2::XMLElement* root)
{
    int res;
	tinyxml2::XMLElement *c = root->FirstChildElement("relation");
	while (c)
	{
        string name;
        if(c->FindAttribute("name"))
			name = string(c->Attribute("name"));
		else
			return -ErrorRelationHasNoAttributeName;

        PTRelation* relation = new PTRelation();
        relation->objType = PTObject::TypeRelation;
        parent->relations.push_back(relation);
        objectList.push_back(relation);
        relation->name = name;

		c = c->NextSiblingElement("relation");
	}
    return 1;
}

int ParserForPtolemy::parseLink(PTEntity* parent, tinyxml2::XMLElement* root)
{
    int res;
	tinyxml2::XMLElement *c = root->FirstChildElement("link");
	while (c)
	{
        PTLink* link = new PTLink();
        link->objType = PTObject::TypeLink;
        parent->links.push_back(link);
        objectList.push_back(link);

        if(c->FindAttribute("port"))
			link->port = string(c->Attribute("port"));
        if(c->FindAttribute("relation"))
			link->relation = string(c->Attribute("relation"));
        if(c->FindAttribute("relation1"))
			link->relation1 = string(c->Attribute("relation1"));
        if(c->FindAttribute("relation2"))
			link->relation2 = string(c->Attribute("relation2"));
        
		c = c->NextSiblingElement("link");
	}
    return 1;
}


void ParserForPtolemy::release()
{
	for(int i = 0;i < objectList.size(); i++)
	{
		delete objectList[i];
	}
	objectList.clear();
	entityList.clear();
}
