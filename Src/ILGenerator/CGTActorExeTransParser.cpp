#include "CGTActorExeTransParser.h"

#include <string>
#include <vector>


using namespace std;

int CGTActorExeTransParser::parseCGTActorExeTrans(std::string content)
{
	tinyxml2::XMLDocument doc;
	transRoot = new CGTActorExeTransRoot();
	tinyxml2::XMLError ret = doc.LoadFile(content.c_str(), content.length());
	if (ret != 0) {
		fprintf(stderr, "Fail to load CGTActorExeTrans file from string");
		return -ErrorLoadCGTActorExeTransFileFail;
	}
	tinyxml2::XMLElement* root = doc.RootElement();

	int res = parseCGTActorExeTrans(root);
	if (res < 0)
		return res;
	return 1;
}

int CGTActorExeTransParser::parseCGTActorExeTrans(const char* content, int length)
{

	tinyxml2::XMLDocument doc;
	transRoot = new CGTActorExeTransRoot();
	tinyxml2::XMLError ret = doc.LoadFile(content, length);
	if (ret != 0) {
		fprintf(stderr, "Fail to load CGTActorExeTrans file from string");
		return -ErrorLoadCGTActorExeTransFileFail;
	}
	tinyxml2::XMLElement* root = doc.RootElement();

	int res = parseCGTActorExeTrans(root);
	if (res < 0)
		return res;

	return 1;
}

int CGTActorExeTransParser::parseCGTActorExeTrans(tinyxml2::XMLElement* root)
{
	int res;
	// parse model info
	res= parseCGTActorExeTransModelInfo(root);
	if (res < 0)
	{
		setCurrentError(res);
		return res;
	}

	// parse function info
	res = parseCGTActorExeTransFunctionInfo(root);
	if (res < 0)
	{
		setCurrentError(res);
		return res;
	}

	// parse actor info
	res = parseCGTActorExeTransActorInfo(root);
	if (res < 0)
	{
		setCurrentError(res);
		return res;
	}

	// parse source outport info
	res = parseCGTActorExeTransSourceOutportInfo(root);
	if (res < 0)
	{
		setCurrentError(res);
		return res;
	}
    return 1;
}

void CGTActorExeTransParser::setCurrentError(int errorCode)
{
	this->errorCode = errorCode;
}

std::string CGTActorExeTransParser::getErrorStr()
{
	std::string ret;
	ret = "CGTActorExeTransParser\nError Code: " + std::to_string(errorCode) + "\n";
	if (currentReadXMLElement)
		ret += "XML line: " + std::to_string(currentReadXMLLineNum);
	else
		ret += "XML line: Unclear.";

	ret += "\n";

	int errorCodeIndex = -errorCode - 1;

	//if(errorCodeIndex >= 0 && errorCodeIndex < errorStrList.length())
	//	ret += errorStrList[errorCodeIndex];
	//else
	//	ret += "Unknow error code.";

	return ret;
}

void CGTActorExeTransParser::setCurrentReadXMLElement(tinyxml2::XMLElement* ele)
{
	currentReadXMLLineNum = ele->GetLineNum();
	currentReadXMLElement = ele;
}

int CGTActorExeTransParser::parseCGTActorExeTransModelInfo(tinyxml2::XMLElement* root)
{
	int res;
	tinyxml2::XMLElement* c = root->FirstChildElement("ModelInfo");
	if (!c)
		return -ErrorCGTActorExeTransFileMissModelInfo;
	setCurrentReadXMLElement(c);
	CGTActorExeTransModelInfo* modelInfo = new CGTActorExeTransModelInfo();
	modelInfo->parent = NULL;
	modelInfo->objType = CGTActorExeTransObject::TypeModelInfo;
	transRoot->modelInfo = modelInfo;
	if (c->FindAttribute("Name"))
		modelInfo->name = std::string(c->Attribute("Name"));
	else
		return -ErrorCGTActorExeTransModelInfoHasNoAttributeName;
    return 1;
}

int CGTActorExeTransParser::parseCGTActorExeTransFunctionInfo(tinyxml2::XMLElement* root)
{
	int res;
	tinyxml2::XMLElement* c = root->FirstChildElement("FunctionInfo");
	if (!c)
		return -ErrorCGTActorExeTransFileMissFunctionInfo;
	setCurrentReadXMLElement(c);
	CGTActorExeTransFunctionInfo* functionInfo = new CGTActorExeTransFunctionInfo();
	functionInfo->parent = NULL;
	functionInfo->objType = CGTActorExeTransObject::TypeFunctionInfo;
	transRoot->functionInfo = functionInfo;
	if (c->FindAttribute("Name"))
		functionInfo->name = std::string(c->Attribute("Name"));
	else
		return -ErrorCGTActorExeTransFunctionInfoHasNoAttributeName;
    return 1;
}

int CGTActorExeTransParser::parseCGTActorExeTransActorInfo(tinyxml2::XMLElement* root)
{
	int res;
	tinyxml2::XMLElement* c = root->FirstChildElement("ActorInfo");
	if (!c)
		return -ErrorCGTActorExeTransFileMissActorInfo;
	setCurrentReadXMLElement(c);
	CGTActorExeTransActorInfo* actorInfo = new CGTActorExeTransActorInfo();
	actorInfo->parent = NULL;
	actorInfo->objType = CGTActorExeTransObject::TypeActorInfo;
	transRoot->actorInfo = actorInfo;
	if (c->FindAttribute("Name"))
		actorInfo->name = std::string(c->Attribute("Name"));
	else
		return -ErrorCGTActorExeTransActorInfoHasNoAttributeName;
	if (c->FindAttribute("Type"))
		actorInfo->type = std::string(c->Attribute("Type"));
	else
		return -ErrorCGTActorExeTransActorInfoHasNoAttributeType;

	// parse inport
	res = parseCGTActorExeTransInport(actorInfo, c);
	if (res < 0)
		return res;
	// parse outport
	res = parseCGTActorExeTransOutport(actorInfo, c);
	if (res < 0)
		return res;
    // parse actionPort
	res = parseCGTActorExeTransActionPort(actorInfo, c);
	if (res < 0)
		return res;
	// parse parameter
    res = parseCGTActorExeTransParameter(actorInfo, c);
	if (res < 0)
		return res;
    return 1;
}

int CGTActorExeTransParser::parseCGTActorExeTransParameter(CGTActorExeTransActorInfo* actorInfo,
    tinyxml2::XMLElement* root)
{
    int res;
	tinyxml2::XMLElement* c = root->FirstChildElement("Parameter");
	if (!c)
		return 0;//-ErrorCGTActorExeTransActorInfoMissInport;
	while (c)
	{
		setCurrentReadXMLElement(c);
		CGTActorExeTransParameter* parameter = new CGTActorExeTransParameter();
        parameter->parent = actorInfo;
		parameter->objType = CGTActorExeTransObject::TypeParameter;

		actorInfo->parameters.push_back(parameter);

        if (c->FindAttribute("Name"))
			parameter->name = std::string(c->Attribute("Name"));
		else
			return -ErrorCGTActorExeTransParameterHasNoAttributeName;
		if (c->FindAttribute("Value"))
			parameter->value = std::string(c->Attribute("Value"));
		else
			return -ErrorCGTActorExeTransParameterHasNoAttributeValue;

        c = c->NextSiblingElement("Parameter");
    }

    return 1;
}

int CGTActorExeTransParser::parseCGTActorExeTransInport(CGTActorExeTransActorInfo* actorInfo, tinyxml2::XMLElement* root)
{
	int res;
	tinyxml2::XMLElement* c = root->FirstChildElement("Inport");
    if (!c)
		return 0;//-ErrorCGTActorExeTransActorInfoMissInport;
	while (c)
	{
		setCurrentReadXMLElement(c);
		CGTActorExeTransInport* inport = new CGTActorExeTransInport();
		inport->parent = actorInfo;
		inport->objType = CGTActorExeTransObject::TypeInport;

		actorInfo->inports.push_back(inport);

		if (c->FindAttribute("Name"))
			inport->name = std::string(c->Attribute("Name"));
		else
			return -ErrorCGTActorExeTransInportHasNoAttributeName;
		if (c->FindAttribute("Type"))
			inport->type = std::string(c->Attribute("Type"));
		else
			return -ErrorCGTActorExeTransInportHasNoAttributeType;
		if (c->FindAttribute("SourceOutport"))
			inport->sourceOutport = std::string(c->Attribute("SourceOutport"));
		else
			return -ErrorCGTActorExeTransInportHasNoAttributeSourceOutport;

        if (c->FindAttribute("ArraySize"))
            inport->arraySize =  parseUtilityArraySize(string(c->Attribute("ArraySize")));


		c = c->NextSiblingElement("Inport");
	}
    return 1;
}

int CGTActorExeTransParser::parseCGTActorExeTransOutport(CGTActorExeTransActorInfo* actorInfo, tinyxml2::XMLElement* root)
{
	int res;
	tinyxml2::XMLElement* c = root->FirstChildElement("Outport");
	if (!c)
		return 0;//-ErrorCGTActorExeTransActorInfoMissInport;
	while (c)
	{
		setCurrentReadXMLElement(c);
		CGTActorExeTransOutport* outport = new CGTActorExeTransOutport();
		outport->parent = actorInfo;
		outport->objType = CGTActorExeTransObject::TypeOutport;

		actorInfo->outports.push_back(outport);

		if (c->FindAttribute("Name"))
			outport->name = std::string(c->Attribute("Name"));
		else
			return -ErrorCGTActorExeTransOutportHasNoAttributeName;
		if (c->FindAttribute("Type"))
			outport->type = std::string(c->Attribute("Type"));
		else
			return -ErrorCGTActorExeTransOutportHasNoAttributeType;
        
        if (c->FindAttribute("ArraySize"))
            outport->arraySize =  parseUtilityArraySize(string(c->Attribute("ArraySize")));

		c = c->NextSiblingElement("Outport");
	}
    return 1;
}

int CGTActorExeTransParser::parseCGTActorExeTransActionPort(CGTActorExeTransActorInfo* actorInfo,
    tinyxml2::XMLElement* root)
{
	int res;
	tinyxml2::XMLElement* c = root->FirstChildElement("ActionPort");
	if (!c)
		return 0;//-ErrorCGTActorExeTransActorInfoMissInport;
	while (c)
	{
		setCurrentReadXMLElement(c);
		CGTActorExeTransActionPort* actionPort = new CGTActorExeTransActionPort();
		actionPort->parent = actorInfo;
		actionPort->objType = CGTActorExeTransObject::TypeActionPort;

		actorInfo->actionPorts.push_back(actionPort);

		if (c->FindAttribute("Name"))
			actionPort->name = std::string(c->Attribute("Name"));
		else
			return -ErrorCGTActorExeTransOutportHasNoAttributeName;
		if (c->FindAttribute("Type"))
			actionPort->type = std::string(c->Attribute("Type"));
		else
			return -ErrorCGTActorExeTransActionPortHasNoAttributeType;
        if (c->FindAttribute("SourceOutport"))
			actionPort->sourceOutport = std::string(c->Attribute("SourceOutport"));
		else
			return -ErrorCGTActorExeTransActionPortHasNoAttributeSourceOutport;
		c = c->NextSiblingElement("ActionPort");
	}
    return 1;
}

int CGTActorExeTransParser::parseCGTActorExeTransSourceOutportInfo(tinyxml2::XMLElement* root) 
{
	int res;
	tinyxml2::XMLElement* c = root->FirstChildElement("SourceOutportInfo");
	if (!c)
		return 0;//-ErrorCGTActorExeTransActorInfoMissInport;
	setCurrentReadXMLElement(c);
	CGTActorExeTransSourceOutportInfo* sourceOutportInfo = new CGTActorExeTransSourceOutportInfo();
	sourceOutportInfo->parent = NULL;
	sourceOutportInfo->objType = CGTActorExeTransObject::TypeSourceOutportInfo;
	transRoot->sourceOutportInfo= sourceOutportInfo;
	// parse SourceOutport
	res = parseCGTActorExeTransSourceOutport(sourceOutportInfo, c);
	if(res<0)
		return res;
    return 1;
}

int CGTActorExeTransParser::parseCGTActorExeTransSourceOutport(CGTActorExeTransSourceOutportInfo* sourceOutportInfo, tinyxml2::XMLElement* root)
{
	int res;
	tinyxml2::XMLElement* c = root->FirstChildElement("SourceOutport");
	if (!c)
		return 0;//-ErrorCGTActorExeTransSourceOutportInfoMissSourceOutport;
	while (c)
	{
		setCurrentReadXMLElement(c);
		CGTActorExeTransSourceOutport* sourceOutport = new CGTActorExeTransSourceOutport();
		sourceOutport->parent = sourceOutportInfo;
		sourceOutport->objType = CGTActorExeTransObject::TypeSourceOutport;

		sourceOutportInfo->sourceOutports.push_back(sourceOutport);

		if (c->FindAttribute("Name"))
			sourceOutport->name = std::string(c->Attribute("Name"));
		else
			return -ErrorCGTActorExeTransSourceOutportHasNoAttributeName;
		if (c->FindAttribute("Type"))
			sourceOutport->type = std::string(c->Attribute("Type"));
		else
			return -ErrorCGTActorExeTransSourceOutportHasNoAttributeType;

		// SourceOutport可以没有ActorName属性
		if (c->FindAttribute("ActorName"))
			sourceOutport->actorName = std::string(c->Attribute("ActorName"));

		if (c->FindAttribute("ActorType"))
			sourceOutport->actorType = std::string(c->Attribute("ActorType"));
		else
			return -ErrorCGTActorExeTransSourceOutportHasNoAttributeActorType;

        if (c->FindAttribute("ArraySize"))
            sourceOutport->arraySize =  parseUtilityArraySize(string(c->Attribute("ArraySize")));

		c = c->NextSiblingElement("SourceOutport");
	}
    return 1;
}

vector<int> CGTActorExeTransParser::parseUtilityArraySize(std::string str) const
{
    vector<int> ret;
	vector<string> arraySizeStrSplit = stringSplit(str, ",");
	for(int i = 0;i < arraySizeStrSplit.size();i++)
	{
		ret.push_back(stringToInt(arraySizeStrSplit[i]));
	}
    return ret;
}

void CGTActorExeTransParser::release()
{

}
