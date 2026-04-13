#include "CGTActorExeTransSaver.h"

using namespace std;

int CGTActorExeTransSaver::save(const CGTActorExeTransRoot* transRoot, std::string& content)
{
	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();
	tinyxml2::XMLElement* root = doc->NewElement("Root");
	int res;

	res = saveModelInfo(transRoot->modelInfo, root, doc);
	if (res < 0)
		return res;
	res = saveFunctionInfo(transRoot->functionInfo, root, doc);
	if (res < 0)
		return res;
	res = saveActorInfo(transRoot->actorInfo, root, doc);
	if (res < 0)
		return res;
	res = saveSourceOutportInfo(transRoot->sourceOutportInfo, root, doc);
	if (res < 0)
		return res;

	doc->InsertEndChild(root);
	tinyxml2::XMLPrinter* printer = new tinyxml2::XMLPrinter();
	doc->Print(printer);

	content = printer->CStr();
	return 1;
}

int CGTActorExeTransSaver::saveModelInfo(const CGTActorExeTransModelInfo* modelInfo, tinyxml2::XMLElement* root, tinyxml2::XMLDocument* doc)
{
	int res;
	tinyxml2::XMLElement* xml = doc->NewElement("ModelInfo");
	if (modelInfo->name.empty())
	{
		res = -ErrorCGTActorExeTransModelInfoHasNoAttributeName;
		return res;
	}
	xml->SetAttribute("Name", modelInfo->name.c_str());
	root->InsertEndChild(xml);
	return 1;
}

int CGTActorExeTransSaver::saveFunctionInfo(const CGTActorExeTransFunctionInfo* functionInfo, tinyxml2::XMLElement* root, tinyxml2::XMLDocument* doc)
{
	int res;
	tinyxml2::XMLElement* xml = doc->NewElement("FunctionInfo");
	if (functionInfo->name.empty())
	{
		res = -ErrorCGTActorExeTransFunctionInfoHasNoAttributeName;
		return res;
	}
	xml->SetAttribute("Name", functionInfo->name.c_str());
	root->InsertEndChild(xml);
	return 1;
}

int CGTActorExeTransSaver::saveActorInfo(const CGTActorExeTransActorInfo* actorInfo, tinyxml2::XMLElement* root, tinyxml2::XMLDocument* doc)
{
	int res;
	tinyxml2::XMLElement* xml = doc->NewElement("ActorInfo");
	if (actorInfo->name.empty())
	{
		res = -ErrorCGTActorExeTransActorInfoHasNoAttributeName;
		return res;
	}
	xml->SetAttribute("Name", actorInfo->name.c_str());
	if (actorInfo->type.empty())
	{
		res = -ErrorCGTActorExeTransActorInfoHasNoAttributeType;
		return res;
	}
	xml->SetAttribute("Type", actorInfo->type.c_str());

	for (auto inport : actorInfo->inports)
	{
		res = saveInport(inport, xml, doc);
		if (res < 0)
			return res;
	}

	for (auto outport : actorInfo->outports)
	{
		res = saveOutport(outport, xml, doc);
		if (res < 0)
			return res;
	}

    for (auto actionPort : actorInfo->actionPorts)
	{
		res = saveActionPort(actionPort, xml, doc);
		if (res < 0)
			return res;
	}

    for (auto parameter : actorInfo->parameters)
	{
		res = saveParameter(parameter, xml, doc);
		if (res < 0)
			return res;
	}

	root->InsertEndChild(xml);
	return 1;
}

int CGTActorExeTransSaver::saveParameter(const CGTActorExeTransParameter* parameter, tinyxml2::XMLElement* root, tinyxml2::XMLDocument* doc)
{
	int res;
	tinyxml2::XMLElement* xml = doc->NewElement("Parameter");
	if (parameter->name.empty())
	{
		res = -ErrorCGTActorExeTransParameterHasNoAttributeName;
		return res;
	}
	xml->SetAttribute("Name", parameter->name.c_str());
	//if (parameter->value.empty())
	//{
	//	res = -ErrorCGTActorExeTransParameterHasNoAttributeValue;
	//	return res;
	//}
	xml->SetAttribute("Value", parameter->value.c_str());

	root->InsertEndChild(xml);
	return 1;
}

int CGTActorExeTransSaver::saveInport(const CGTActorExeTransInport* inport, tinyxml2::XMLElement* root, tinyxml2::XMLDocument* doc)
{
	int res;
	tinyxml2::XMLElement* xml = doc->NewElement("Inport");
	if (inport->name.empty())
	{
		res = -ErrorCGTActorExeTransInportHasNoAttributeName;
		return res;
	}
	xml->SetAttribute("Name", inport->name.c_str());
	if (inport->type.empty())
	{
		res = -ErrorCGTActorExeTransInportHasNoAttributeType;
		return res;
	}
	xml->SetAttribute("Type", inport->type.c_str());
	if (inport->sourceOutport.empty())
	{
		res = -ErrorCGTActorExeTransInportHasNoAttributeSourceOutport;
		return res;
	}
    if(inport->isAddress)
		xml->SetAttribute("IsAddress", inport->isAddress);
	if(inport->arraySize.size() > 0)
	{
		string arraySizeStr;
		for(int i = 0;i < inport->arraySize.size(); i++)
		{
			if(i != 0)
				arraySizeStr += ",";
			arraySizeStr += to_string(inport->arraySize[i]);
		}
		xml->SetAttribute("ArraySize", arraySizeStr.c_str());
	}
	xml->SetAttribute("SourceOutport", inport->sourceOutport.c_str());
	root->InsertEndChild(xml);
	return 1;
}

int CGTActorExeTransSaver::saveOutport(const CGTActorExeTransOutport* outport, tinyxml2::XMLElement* root, tinyxml2::XMLDocument* doc)
{
	int res;
	tinyxml2::XMLElement* xml = doc->NewElement("Outport");
	if (outport->name.empty())
	{
		res = -ErrorCGTActorExeTransOutportHasNoAttributeName;
		return res;
	}
	xml->SetAttribute("Name", outport->name.c_str());
	if (outport->type.empty())
	{
		res = -ErrorCGTActorExeTransOutportHasNoAttributeType;
		return res;
	}
	xml->SetAttribute("Type", outport->type.c_str());
    if(outport->isAddress)
		xml->SetAttribute("IsAddress", outport->isAddress);
	if(outport->arraySize.size() > 0)
	{
		string arraySizeStr;
		for(int i = 0;i < outport->arraySize.size(); i++)
		{
			if(i != 0)
				arraySizeStr += ",";
			arraySizeStr += to_string(outport->arraySize[i]);
		}
		xml->SetAttribute("ArraySize", arraySizeStr.c_str());
	}
	root->InsertEndChild(xml);
	return 1;
}

int CGTActorExeTransSaver::saveActionPort(const CGTActorExeTransActionPort* actionPort, tinyxml2::XMLElement* root, tinyxml2::XMLDocument* doc)
{
	int res;
	tinyxml2::XMLElement* xml = doc->NewElement("ActionPort");
	if (actionPort->name.empty())
	{
		res = -ErrorCGTActorExeTransActionPortHasNoAttributeName;
		return res;
	}
	xml->SetAttribute("Name", actionPort->name.c_str());
	if (actionPort->type.empty())
	{
		res = -ErrorCGTActorExeTransActionPortHasNoAttributeType;
		return res;
	}
	xml->SetAttribute("Type", actionPort->type.c_str());
    xml->SetAttribute("SourceOutport", actionPort->sourceOutport.c_str());
	root->InsertEndChild(xml);
	return 1;
}

int CGTActorExeTransSaver::saveSourceOutportInfo(const CGTActorExeTransSourceOutportInfo* sourceOutportInfo, tinyxml2::XMLElement* root, tinyxml2::XMLDocument* doc)
{
	int res;
	tinyxml2::XMLElement* xml = doc->NewElement("SourceOutportInfo");
	
	for (auto sourceOutport : sourceOutportInfo->sourceOutports)
	{
		res = saveSourceOutport(sourceOutport, xml, doc);
		if (res < 0)
			return res;
	}
	root->InsertEndChild(xml);
	return 1;
}

int CGTActorExeTransSaver::saveSourceOutport(const CGTActorExeTransSourceOutport* sourceOutport, tinyxml2::XMLElement* root, tinyxml2::XMLDocument* doc)
{
	int res;
	tinyxml2::XMLElement* xml = doc->NewElement("SourceOutport");
	if (sourceOutport->name.empty())
	{
		res = -ErrorCGTActorExeTransSourceOutportHasNoAttributeName;
		return res;
	}
	xml->SetAttribute("Name", sourceOutport->name.c_str());
	if (sourceOutport->type.empty())
	{
		res = -ErrorCGTActorExeTransSourceOutportHasNoAttributeType;
		return res;
	}
	xml->SetAttribute("Type", sourceOutport->type.c_str());
    if(sourceOutport->isAddress)
		xml->SetAttribute("IsAddress", sourceOutport->isAddress);
	if(sourceOutport->arraySize.size() > 0)
	{
		string arraySizeStr;
		for(int i = 0;i < sourceOutport->arraySize.size(); i++)
		{
			if(i != 0)
				arraySizeStr += ",";
			arraySizeStr += to_string(sourceOutport->arraySize[i]);
		}
		xml->SetAttribute("ArraySize", arraySizeStr.c_str());
	}
	xml->SetAttribute("ActorName", sourceOutport->actorName.c_str());
	if (sourceOutport->actorType.empty())
	{
		res = -ErrorCGTActorExeTransSourceOutportHasNoAttributeActorType;
		return res;
	}
	xml->SetAttribute("ActorType", sourceOutport->actorType.c_str());
	root->InsertEndChild(xml);
	return 1;
}
