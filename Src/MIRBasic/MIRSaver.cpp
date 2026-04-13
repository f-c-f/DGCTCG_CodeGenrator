#include "MIRSaver.h"

#include "MIRFunctionDataflow.h"
#include "MIRFunctionStateflow.h"
#include <algorithm>

using namespace std;

int MIRSaver::save(MIRModel* model, string outputFilePath)
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement* root = doc.NewElement("Root");
	int res;

	res = saveModelEntity(model, root, &doc);
	if(res < 0)
		return res;

	
	doc.InsertEndChild(root);
	tinyxml2::XMLPrinter printer;
	doc.Print(&printer);

	//printf("%s\n", printer.CStr());

	doc.SaveFile(outputFilePath.c_str());

    return 1;
}

int MIRSaver::saveModelEntity(MIRModel* model, tinyxml2::XMLElement* root, tinyxml2::XMLDocument* doc)
{
	int res;

	tinyxml2::XMLElement* xml = doc->NewElement("ModelEntity");

    xml->SetAttribute("MainFunction", model->mainFunction.c_str());
	xml->SetAttribute("ModelSrcType", model->modelSrcType.c_str());

	for(int i = 0;i < model->functions.size();i++)
	{
		res = saveFunction(model->functions[i], xml, doc);
		if(res < 0)
			return res;
	}

	root->InsertEndChild(xml);
	return 1;
}

int MIRSaver::saveFunction(MIRFunction* function, tinyxml2::XMLElement* root, tinyxml2::XMLDocument* doc)
{
    if(function->type == MIRFunction::TypeDataflow)
    {
        return saveFunctionDataflow(reinterpret_cast<MIRFunctionDataflow*>(function), root, doc);
    }
    else if(function->type == MIRFunction::TypeStateflow)
    {
        return saveFunctionStateflow(reinterpret_cast<MIRFunctionStateflow*>(function), root, doc);
    }
    return -ErrorUnknownMIRFunctionType;


}

int MIRSaver::saveFunctionDataflow(MIRFunctionDataflow* function, tinyxml2::XMLElement* root,
    tinyxml2::XMLDocument* doc)
{
    
	int res;

	tinyxml2::XMLElement* xml = doc->NewElement("Function");
	xml->SetAttribute("Name", function->name.c_str());
	xml->SetAttribute("Type", "Dataflow");
	
	for(int i = 0;i < function->inports.size();i++)
	{
		res = saveInport(function->inports[i], xml, doc);
		if(res < 0)
			return res;
	}
	for(int i = 0;i < function->outports.size();i++)
	{
		res = saveOutport(function->outports[i], xml, doc);
		if(res < 0)
			return res;
	}
	for(int i = 0;i < function->actionPorts.size();i++)
	{
		res = saveActionPort(function->actionPorts[i], xml, doc);
		if(res < 0)
			return res;
	}
	for(int i = 0;i < function->actors.size();i++)
	{
		res = saveActor(function->actors[i], xml, doc);
		if(res < 0)
			return res;
	}
	for(int i = 0;i < function->relations.size();i++)
	{
		res = saveRelation(function->relations[i], xml, doc);
		if(res < 0)
			return res;
	}
    
	root->InsertEndChild(xml);
	return 1;
}

int MIRSaver::saveFunctionStateflow(MIRFunctionStateflow* function, tinyxml2::XMLElement* root,
                                    tinyxml2::XMLDocument* doc)
{
	int res;

	tinyxml2::XMLElement* xml = doc->NewElement("Function");
	xml->SetAttribute("Name", function->name.c_str());
	xml->SetAttribute("Type", "Stateflow");
	
	for(int i = 0;i < function->inports.size();i++)
	{
		res = saveInport(function->inports[i], xml, doc);
		if(res < 0)
			return res;
	}
	for(int i = 0;i < function->outports.size();i++)
	{
		res = saveOutport(function->outports[i], xml, doc);
		if(res < 0)
			return res;
	}
	for(int i = 0;i < function->actionPorts.size();i++)
	{
		res = saveActionPort(function->actionPorts[i], xml, doc);
		if(res < 0)
			return res;
	}
    
	root->InsertEndChild(xml);
	return 1;
}

int MIRSaver::saveActor(MIRActor* actor, tinyxml2::XMLElement* root, tinyxml2::XMLDocument* doc)
{
	int res;

	tinyxml2::XMLElement* xml = doc->NewElement("Actor");
	xml->SetAttribute("Name", actor->name.c_str());
	xml->SetAttribute("Type", actor->type.c_str());
	
	for(int i = 0;i < actor->inports.size();i++)
	{
		res = saveInport(actor->inports[i], xml, doc);
		if(res < 0)
			return res;
	}
	for(int i = 0;i < actor->outports.size();i++)
	{
		res = saveOutport(actor->outports[i], xml, doc);
		if(res < 0)
			return res;
	}
	for(int i = 0;i < actor->actionPorts.size();i++)
	{
		res = saveActionPort(actor->actionPorts[i], xml, doc);
		if(res < 0)
			return res;
	}
    for(int i = 0;i < actor->parameters.size();i++)
	{
		res = saveActorParameter(actor->parameters[i], xml, doc);
		if(res < 0)
			return res;
	}
	res = saveXMLParameter(actor, xml, doc);
	if(res < 0)
		return res;

	root->InsertEndChild(xml);
	return 1;
}

int MIRSaver::saveActorParameter(MIRParameter* parameter, tinyxml2::XMLElement* root, tinyxml2::XMLDocument* doc)
{
    int res;
	tinyxml2::XMLElement* xml = doc->NewElement("Parameter");
	xml->SetAttribute("Name", parameter->name.c_str());
	xml->SetAttribute("Value", parameter->value.c_str());
	
	root->InsertEndChild(xml);
	return 1;
}

int MIRSaver::saveRelation(MIRRelation* relation, tinyxml2::XMLElement* root, tinyxml2::XMLDocument* doc)
{
	int res;

	tinyxml2::XMLElement* xml = doc->NewElement("Relation");
	for(int i = 0;i < relation->srcStrs.size();i++)
	{
		tinyxml2::XMLElement* srcXml = doc->NewElement("Src");
		srcXml->SetAttribute("Src", relation->srcStrs[i].c_str());
		xml->InsertEndChild(srcXml);
	}
	for(int i = 0;i < relation->dstStrs.size();i++)
	{
		tinyxml2::XMLElement* dstXml = doc->NewElement("Dst");
		dstXml->SetAttribute("Dst", relation->dstStrs[i].c_str());
		xml->InsertEndChild(dstXml);
	}
	root->InsertEndChild(xml);
	return 1;
}

int MIRSaver::saveInport(MIRInport* inport, tinyxml2::XMLElement* root, tinyxml2::XMLDocument* doc)
{
	int res;
	tinyxml2::XMLElement* xml = doc->NewElement("Inport");
	xml->SetAttribute("Name", inport->name.c_str());
	xml->SetAttribute("Type", inport->type.c_str());
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
	if(!inport->defaultValue.empty())
		xml->SetAttribute("DefaultValue", inport->defaultValue.c_str());

	root->InsertEndChild(xml);
	return 1;
}

int MIRSaver::saveOutport(MIROutport* outport, tinyxml2::XMLElement* root, tinyxml2::XMLDocument* doc)
{
	int res;
	tinyxml2::XMLElement* xml = doc->NewElement("Outport");
	xml->SetAttribute("Name", outport->name.c_str());
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
	if(!outport->defaultValue.empty())
		xml->SetAttribute("DefaultValue", outport->defaultValue.c_str());


	root->InsertEndChild(xml);
	return 1;
}

int MIRSaver::saveActionPort(MIRActionPort* actionPort, tinyxml2::XMLElement* root, tinyxml2::XMLDocument* doc)
{
	int res;
	tinyxml2::XMLElement* xml = doc->NewElement("ActionPort");
	xml->SetAttribute("Name", actionPort->name.c_str());
	xml->SetAttribute("Type", actionPort->type.c_str());
	

	root->InsertEndChild(xml);
	return 1;
}

vector<string> MIRSaver::filterParameter = {
	"Ports",
	"Position",
	"ZOrder", 
	"ContentPreviewEnabled",
	"SourceType",
	"ForegroundColor",
	"IconShape",
	"IconShape",
	"IconShape",
	"IconShape",
	"IconShape",
};
int MIRSaver::saveXMLParameter(MIRObject* object, tinyxml2::XMLElement* root, tinyxml2::XMLDocument* doc)
{
	map<string, string>::iterator iter = object->parametersOfXML.begin();
	for(;iter!=object->parametersOfXML.end();iter++)
	{
		tinyxml2::XMLElement* xml = doc->NewElement("Parameter");
		if(find(MIRSaver::filterParameter.begin(),MIRSaver::filterParameter.end(),iter->first) != MIRSaver::filterParameter.end())
			continue;
		
		xml->SetAttribute("Name", iter->first.c_str());
		xml->SetAttribute("Value", iter->second.c_str());
	
		root->InsertEndChild(xml);
	}

	return 1;
}
