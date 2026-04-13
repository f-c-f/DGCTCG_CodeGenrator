#include "MIRParser.h"

#include "MIRFunctionDataflow.h"
#include "../Common/Utility.h"

using namespace std;

int MIRParser::parseMIR(string xmlFilePath, MIRModel* retModel)
{
    model = retModel;
    
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError ret = doc.LoadFile(xmlFilePath.c_str());
	if (ret != 0) {
		fprintf(stderr, "Fail to load MIR file: %s\n", xmlFilePath.c_str());
		return -ErrorLoadMIRFileFail;
	}
	tinyxml2::XMLElement* root = doc.RootElement();

    model->name = getFileNameWithoutSuffixByPath(xmlFilePath);

	int res = parseMIR(root);
	if(res < 0)
		return res;

	res = calculateRelationObj();
	if(res < 0)
		return res;


	return 1;
}

int MIRParser::parseMIR(tinyxml2::XMLElement* root)
{
	int res;

	//this->model = new MIRModel();

	res = parseModelEntity(this->model, root);
	if(res < 0)
	{
		setCurrentError(res);
		return res;
	}
	return 1;
}

void MIRParser::setCurrentError(int errorCode)
{
	this->errorCode = errorCode;
}

string MIRParser::getErrorStr()
{
	string ret;
	ret = "MIRParser\nError Code: " + to_string(errorCode) + "\n";
	if(currentReadXMLElement)
		ret += "XML line: " + to_string(currentReadXMLLineNum);
	else
		ret += "XML line: Unclear.";
	
	ret += "\n";

	int errorCodeIndex = -errorCode - 1;

	//if(errorCodeIndex >= 0 && errorCodeIndex < errorStrList.size())
	//	ret += errorStrList[errorCodeIndex];
	//else
	//	ret += "Unknow error code.";

	return ret;
}

void MIRParser::setCurrentReadXMLElement(tinyxml2::XMLElement* ele)
{
	currentReadXMLLineNum = ele->GetLineNum();
	currentReadXMLElement = ele;
}

int MIRParser::parseModelEntity(MIRModel* model, tinyxml2::XMLElement* root)
{
	int res;
	tinyxml2::XMLElement *c = root->FirstChildElement("ModelEntity");
	if(!c)
		return 0;//-ErrorMIRFunctionMissModelEntity;
	setCurrentReadXMLElement(c);
	model->objType = MIRObject::TypeModel;
	model->parent = NULL;

    if(c->FindAttribute("MainFunction"))
    {
         model->mainFunction = string(c->Attribute("MainFunction"));
    }
	else
	{
		return -ErrorMIRModelEntityHasNoAttributeMainFunction;
	}

	if(c->FindAttribute("ModelSrcType"))
    {
         model->modelSrcType = string(c->Attribute("ModelSrcType"));
    }

	res = parseFunction(model, c);
	if(res < 0)
		return res;

	return 1;
}

int MIRParser::parseFunction(MIRModel* model, tinyxml2::XMLElement* root)
{
	tinyxml2::XMLElement *c = root->FirstChildElement("Function");
	if(!c)
		return 0;//-ErrorMIRModelEntityMissFunction;
	while (c)
	{
		setCurrentReadXMLElement(c);

        string type;
        if(c->FindAttribute("Type"))
        {
             type = string(c->Attribute("Type"));
        }
		else
		{
		    return -ErrorMIRFunctionHasNoAttributeType;
		}
        
        if(type == "Dataflow")
        {
            int res = parseFunctionDataflow(model, c);
            if(res < 0)
                return res;
        }
        else if(type == "Stateflow")
        {
            int res = parseFunctionStateflow(model, c);
            if(res < 0)
                return res;
        }
        else
        {
            return -ErrorMIRFunctionHasNoAttributeType;
        }


		c = c->NextSiblingElement("Function");
	}
	return 1;
}

int MIRParser::parseFunctionDataflow(MIRModel* model, tinyxml2::XMLElement* c)
{
	MIRFunctionDataflow* function = new MIRFunctionDataflow();
	function->parent = model;
	function->objType = MIRObject::TypeFunction;
	function->type = MIRFunction::TypeDataflow;

	model->functions.push_back(function);
	objectList.push_back(function);

	if(c->FindAttribute("Name"))
		function->name = string(c->Attribute("Name"));
	else
		return -ErrorMIRFunctionHasNoAttributeName;

	int res;
	res = parseActor(function, c);
	if(res < 0)
		return res;
	res = parseFunctionInport(function, c);
	if(res < 0)
		return res;
	res = parseFunctionOutport(function, c);
	if(res < 0)
		return res;
	res = parseFunctionActionPort(function, c);
	if(res < 0)
		return res;
	res = parseFunctionRelation(function, c);
	if(res < 0)
		return res;

    return 1;
}

int MIRParser::parseFunctionStateflow(MIRModel* model, tinyxml2::XMLElement* root)
{
    return 1;
}

int MIRParser::parseActor(MIRFunctionDataflow* function, tinyxml2::XMLElement* root)
{
	tinyxml2::XMLElement *c = root->FirstChildElement("Actor");
	if(!c)
		return 0;//-ErrorMIRFunctionMissActor;
	while (c)
	{
		setCurrentReadXMLElement(c);
		MIRActor* actor = new MIRActor();
		actor->parent = function;
		actor->objType = MIRObject::TypeActor;
		
		function->actors.push_back(actor);
		objectList.push_back(actor);

		if(c->FindAttribute("Name"))
			actor->name = string(c->Attribute("Name"));
		else
			return -ErrorMIRActorHasNoAttributeName;

		if(c->FindAttribute("Type"))
			actor->type = string(c->Attribute("Type"));
		else
			return -ErrorMIRActorHasNoAttributeType;

		int res;
		res = parseActorInport(actor, c);
		if(res < 0)
			return res;
		res = parseActorOutport(actor, c);
		if(res < 0)
			return res;
		res = parseActorActionPort(actor, c);
		if(res < 0)
			return res;
		res = parseActorParameter(actor, c);
		if(res < 0)
			return res;
		
		c = c->NextSiblingElement("Actor");
	}
	return 1;
}

int MIRParser::parseFunctionRelation(MIRFunctionDataflow* function, tinyxml2::XMLElement* root)
{
	tinyxml2::XMLElement *c = root->FirstChildElement("Relation");
	if(!c)
		return 0;//-ErrorMIRFunctionMissRelation;
	while (c)
	{
		setCurrentReadXMLElement(c);

		MIRRelation* relation = new MIRRelation();
		relation->parent = function;
		relation->objType = MIRObject::TypeRelation;
		
		function->relations.push_back(relation);
		objectList.push_back(relation);

		tinyxml2::XMLElement *cc;

		cc = c->FirstChildElement("Src");
		//if(!cc)
		//	return -ErrorMIRRelationMissSrc;
		while (cc)
		{
			setCurrentReadXMLElement(cc);
			string src;
			if(cc->FindAttribute("Src"))
				src = string(cc->Attribute("Src"));
			else
				return -ErrorMIRRelationSrcHasNoAttributeSrc;
			relation->srcStrs.push_back(src);

			cc = cc->NextSiblingElement("Src");
		}

		cc = c->FirstChildElement("Dst");
		//if(!cc)
		//	return -ErrorMIRRelationMissSrc;
		while (cc)
		{
			setCurrentReadXMLElement(cc);
			string dst;
			if(cc->FindAttribute("Dst"))
				dst = string(cc->Attribute("Dst"));
			else
				return -ErrorMIRRelationSrcHasNoAttributeDst;
			relation->dstStrs.push_back(dst);

			cc = cc->NextSiblingElement("Dst");
		}

		c = c->NextSiblingElement("Relation");
	}
	return 1;

}

int MIRParser::parseFunctionInport(MIRFunction* function, tinyxml2::XMLElement* root)
{
	tinyxml2::XMLElement *c = root->FirstChildElement("Inport");
	if(!c)
		return 0;//-ErrorMIRFunctionMissInport;
	while (c)
	{
		setCurrentReadXMLElement(c);
		MIRInport* inport = new MIRInport();
		inport->parent = function;
		inport->objType = MIRObject::TypeInport;
		
		function->inports.push_back(inport);
		objectList.push_back(inport);

		if(c->FindAttribute("Name"))
			inport->name = string(c->Attribute("Name"));
		else
			return -ErrorMIRInportHasNoAttributeName;
		if(c->FindAttribute("DefaultValue"))
			inport->defaultValue = string(c->Attribute("DefaultValue"));

		if(c->FindAttribute("IsAddress"))
			inport->isAddress = c->BoolAttribute("IsAddress");
		else
			inport->isAddress = false;

		if(c->FindAttribute("Type"))
			inport->type = string(c->Attribute("Type"));
		else
			return -ErrorMIRInportHasNoAttributeType;

		if(c->FindAttribute("ArraySize"))
		{
			string arraySizeStr = string(c->Attribute("ArraySize"));
			vector<string> arraySizeStrSplit = stringSplit(arraySizeStr, ",");
			for(int i = 0;i < arraySizeStrSplit.size();i++)
			{
				inport->arraySize.push_back(stringToInt(arraySizeStrSplit[i]));
			}
		}

		inport->fullName = inport->name;
		
		c = c->NextSiblingElement("Inport");
	}
	return 1;
}

int MIRParser::parseFunctionOutport(MIRFunction* function, tinyxml2::XMLElement* root)
{
	tinyxml2::XMLElement *c = root->FirstChildElement("Outport");
	if(!c)
		return 0;//-ErrorMIRFunctionMissOutport;
	while (c)
	{
		setCurrentReadXMLElement(c);
		MIROutport* outport = new MIROutport();
		outport->parent = function;
		outport->objType = MIRObject::TypeOutport;
		
		function->outports.push_back(outport);
		objectList.push_back(outport);

		if(c->FindAttribute("Name"))
			outport->name = string(c->Attribute("Name"));
		else
			return -ErrorMIROutportHasNoAttributeName;
		if(c->FindAttribute("DefaultValue"))
			outport->defaultValue = string(c->Attribute("DefaultValue"));

		if(c->FindAttribute("IsAddress"))
			outport->isAddress = c->BoolAttribute("IsAddress");
		else
			outport->isAddress = false;

		if(c->FindAttribute("Type"))
			outport->type = string(c->Attribute("Type"));
		else
			return -ErrorMIROutportHasNoAttributeType;
		if(c->FindAttribute("ArraySize"))
		{
			string arraySizeStr = string(c->Attribute("ArraySize"));
			vector<string> arraySizeStrSplit = stringSplit(arraySizeStr, ",");
			for(int i = 0;i < arraySizeStrSplit.size();i++)
			{
				outport->arraySize.push_back(stringToInt(arraySizeStrSplit[i]));
			}
		}
		
		outport->fullName = outport->name;

		c = c->NextSiblingElement("Outport");
	}
	return 1;
}

int MIRParser::parseFunctionActionPort(MIRFunction* function, tinyxml2::XMLElement* root)
{
	tinyxml2::XMLElement *c = root->FirstChildElement("ActionPort");
	if(!c)
		return 0;//-ErrorMIRFunctionMissActionPort;
	while (c)
	{
		setCurrentReadXMLElement(c);
		MIRActionPort* actionPort = new MIRActionPort();
		actionPort->parent = function;
		actionPort->objType = MIRObject::TypeActionPort;
		
		function->actionPorts.push_back(actionPort);
		objectList.push_back(actionPort);

		if(c->FindAttribute("Name"))
			actionPort->name = string(c->Attribute("Name"));
		else
			return -ErrorMIRActionPortHasNoAttributeName;
		
		if(c->FindAttribute("Type"))
			actionPort->type = string(c->Attribute("Type"));
		else
			return -ErrorMIRActionPortHasNoAttributeType;
		
		actionPort->fullName = actionPort->name;

		c = c->NextSiblingElement("ActionPort");
	}
	return 1;
}

int MIRParser::parseActorInport(MIRActor* actor, tinyxml2::XMLElement* root)
{
	tinyxml2::XMLElement *c = root->FirstChildElement("Inport");
	if(!c)
		return 0;//-ErrorMIRActorMissInport;
	while (c)
	{
		setCurrentReadXMLElement(c);
		MIRInport* inport = new MIRInport();
		inport->parent = actor;
		inport->objType = MIRObject::TypeInport;
		
		actor->inports.push_back(inport);
		objectList.push_back(inport);

		if(c->FindAttribute("Name"))
			inport->name = string(c->Attribute("Name"));
		else
			return -ErrorMIRInportHasNoAttributeName;
		if(c->FindAttribute("DefaultValue"))
			inport->defaultValue = string(c->Attribute("DefaultValue"));

		if(c->FindAttribute("IsAddress"))
			inport->isAddress = c->BoolAttribute("IsAddress");
		else
			inport->isAddress = false;

		if(c->FindAttribute("Type"))
			inport->type = string(c->Attribute("Type"));
		else
			return -ErrorMIRInportHasNoAttributeType;

		if(c->FindAttribute("ArraySize"))
		{
			string arraySizeStr = string(c->Attribute("ArraySize"));
			vector<string> arraySizeStrSplit = stringSplit(arraySizeStr, ",");
			for(int i = 0;i < arraySizeStrSplit.size();i++)
			{
				inport->arraySize.push_back(stringToInt(arraySizeStrSplit[i]));
			}
		}

		inport->fullName = actor->name + "." + inport->name;

		c = c->NextSiblingElement("Inport");
	}
	return 1;
}

int MIRParser::parseActorOutport(MIRActor* actor, tinyxml2::XMLElement* root)
{
	tinyxml2::XMLElement *c = root->FirstChildElement("Outport");
	if(!c)
		return 0;//-ErrorMIRActorMissOutport;
	while (c)
	{
		setCurrentReadXMLElement(c);
		MIROutport* outport = new MIROutport();
		outport->parent = actor;
		outport->objType = MIRObject::TypeOutport;
		
		actor->outports.push_back(outport);
		objectList.push_back(outport);

		if(c->FindAttribute("Name"))
			outport->name = string(c->Attribute("Name"));
		else
			return -ErrorMIROutportHasNoAttributeName;
		if(c->FindAttribute("DefaultValue"))
			outport->defaultValue = string(c->Attribute("DefaultValue"));

		if(c->FindAttribute("IsAddress"))
			outport->isAddress = c->BoolAttribute("IsAddress");
		else
			outport->isAddress = false;

		if(c->FindAttribute("Type"))
			outport->type = string(c->Attribute("Type"));
		else
			return -ErrorMIROutportHasNoAttributeType;
		if(c->FindAttribute("ArraySize"))
		{
			string arraySizeStr = string(c->Attribute("ArraySize"));
			vector<string> arraySizeStrSplit = stringSplit(arraySizeStr, ",");
			for(int i = 0;i < arraySizeStrSplit.size();i++)
			{
				outport->arraySize.push_back(stringToInt(arraySizeStrSplit[i]));
			}
		}
		
		outport->fullName = actor->name + "." + outport->name;

		c = c->NextSiblingElement("Outport");
	}
	return 1;
}

int MIRParser::parseActorParameter(MIRActor* actor, tinyxml2::XMLElement* root)
{
    tinyxml2::XMLElement *c = root->FirstChildElement("Parameter");
	if(!c)
		return 0;//-ErrorMIRActorMissOutport;
	while (c)
	{
		setCurrentReadXMLElement(c);
		MIRParameter* parameter = new MIRParameter();
		parameter->parent = actor;
		parameter->objType = MIRObject::TypeParameter;
		
		actor->parameters.push_back(parameter);
		objectList.push_back(parameter);

		if(c->FindAttribute("Name"))
			parameter->name = string(c->Attribute("Name"));
		else
			return -ErrorMIRParameterHasNoAttributeName;
        if(c->FindAttribute("Value"))
			parameter->value = string(c->Attribute("Value"));
		else
			return -ErrorMIRParameterHasNoAttributeValue;
		
		
		c = c->NextSiblingElement("Parameter");
	}
	return 1;
}

int MIRParser::parseActorActionPort(MIRActor* actor, tinyxml2::XMLElement* root)
{
	tinyxml2::XMLElement *c = root->FirstChildElement("ActionPort");
	if(!c)
		return 0;//-ErrorMIRActorMissActionPort;
	while (c)
	{
		setCurrentReadXMLElement(c);
		MIRActionPort* actionPort = new MIRActionPort();
		actionPort->parent = actor;
		actionPort->objType = MIRObject::TypeActionPort;
		
		actor->actionPorts.push_back(actionPort);
		objectList.push_back(actionPort);

		if(c->FindAttribute("Name"))
			actionPort->name = string(c->Attribute("Name"));
		else
			return -ErrorMIRActionPortHasNoAttributeName;
		
		if(c->FindAttribute("Type"))
			actionPort->type = string(c->Attribute("Type"));
		else
			return -ErrorMIRActionPortHasNoAttributeType;
		
		actionPort->fullName = actor->name + "." + actionPort->name;

		c = c->NextSiblingElement("ActionPort");
	}
	return 1;
}

int MIRParser::parseParameter(MIRObject* object, tinyxml2::XMLElement* root)
{
	tinyxml2::XMLElement *c = root->FirstChildElement("Parameter");
	if(!c)
		return 0;//-ErrorMIRObjectMissParameter;
	while (c)
	{
		setCurrentReadXMLElement(c);
		
		string name;
		string value;

		if(c->FindAttribute("Name"))
			name = string(c->Attribute("Name"));
		else
			return -ErrorMIRParameterHasNoAttributeName;
		
		if(c->FindAttribute("Value"))
			value = string(c->Attribute("Value"));
		else
			return -ErrorMIRParameterHasNoAttributeValue;

		object->parametersOfXML[name] = value;

		c = c->NextSiblingElement("Parameter");
	}
	return 1;
}

int MIRParser::calculateRelationObj()
{
	int res;
	for(int i = 0; i < objectList.size(); i++)
	{
		if(objectList[i]->objType == MIRObject::TypeRelation)
		{
			res = calculateRelationObj((MIRRelation*)objectList[i]);
			if(res < 0)
				return res;
		}
	}
	return 1;
}

int MIRParser::calculateRelationObj(MIRRelation* relation)
{
	int res;
	for (int i = 0; i < relation->srcStrs.size(); i++)
	{
		res = calculateRelationObjSrc(relation, relation->srcStrs[i]);
		if(res < 0)
			return res;
	}
	for (int i = 0; i < relation->dstStrs.size(); i++)
	{
		res = calculateRelationObjDst(relation, relation->dstStrs[i]);
		if(res < 0)
			return res;
	}
	return 1;
}
int MIRParser::calculateRelationObjSrc(MIRRelation* relation, string& srcStr)
{
	int res;
	MIRObject* srcObj = NULL;
	for(int i = 0; i < objectList.size(); i++)
	{
		if(objectList[i]->objType == MIRObject::TypeInport &&
			objectList[i]->parent &&
			objectList[i]->parent->objType == MIRObject::TypeFunction &&
			relation->parent == objectList[i]->parent)
		{
			MIRInport* inport = (MIRInport*)objectList[i];
			if(inport->fullName == srcStr)
			{
				srcObj = inport;
				break;
			}
		}
		else if(objectList[i]->objType == MIRObject::TypeOutport &&
			objectList[i]->parent &&
			objectList[i]->parent->objType == MIRObject::TypeActor &&
            relation->parent == objectList[i]->parent->parent)
		{
			MIROutport* outport = (MIROutport*)objectList[i];
			if(outport->fullName == srcStr)
			{
				srcObj = outport;
				break;
			}
		}
	}
	if(!srcObj)
	{
		return -ErrorMIRRelationCanNotFindSrc;
	}
	relation->srcObjs.push_back(srcObj);
	return 1;
}
int MIRParser::calculateRelationObjDst(MIRRelation* relation, string& dstStr)
{
	int res;
	MIRObject* dstObj = NULL;
	for(int i = 0; i < objectList.size(); i++)
	{
		if(objectList[i]->objType == MIRObject::TypeOutport &&
			objectList[i]->parent &&
			objectList[i]->parent->objType == MIRObject::TypeFunction &&
			relation->parent == objectList[i]->parent)
		{
			MIROutport* outport = (MIROutport*)objectList[i];
			if(outport->fullName == dstStr)
			{
				dstObj = outport;
				break;
			}
		}
		else if(objectList[i]->objType == MIRObject::TypeInport &&
			objectList[i]->parent &&
			objectList[i]->parent->objType == MIRObject::TypeActor &&
            relation->parent == objectList[i]->parent->parent)
		{
			MIRInport* inport = (MIRInport*)objectList[i];
			if(inport->fullName == dstStr)
			{
				dstObj = inport;
				break;
			}
		}
		else if(objectList[i]->objType == MIRObject::TypeActionPort &&
			objectList[i]->parent &&
			objectList[i]->parent->objType == MIRObject::TypeActor &&
            relation->parent == objectList[i]->parent->parent)
		{
			MIRActionPort* actionPort = (MIRActionPort*)objectList[i];
			if(actionPort->fullName == dstStr)
			{
				dstObj = actionPort;
				break;
			}
		}
	}
	if(!dstObj)
	{
		return -ErrorMIRRelationCanNotFindDst;
	}
	relation->dstObjs.push_back(dstObj);
	return 1;
}
