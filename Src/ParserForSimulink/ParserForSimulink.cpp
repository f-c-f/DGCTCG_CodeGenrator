#include "ParserForSimulink.h"
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <queue>

#include "../Common/unzip.h"

#include "SLSubsystem.h"
#include "SLBlock.h"
#include "SLLine.h"
#include "SLInport.h"
#include "SLOutport.h"
#include "SLActionPort.h"
using namespace std;

int ParserForSimulink::parseSL(string filePath)
{
	HZIP hz = OpenZip(filePath.c_str(),0);
	if(!hz)
	{
		fprintf(stderr, "Fail to load model file: %s\n", filePath.c_str());
		return -ErrorLoadSLModelFail;
	}
	ZIPENTRY ze; 
	GetZipItem(hz,-1,&ze); 
	int numitems=ze.index;

	int res = 1;

	for (int zi=0; zi<numitems; zi++)
	{
		ZIPENTRY ze; 
		//int i; FindZipItem(hz,_T("simple.jpg"),true,&i,&ze);
		GetZipItem(hz,zi,&ze); // fetch individual details

		string innerPath(ze.name);
		if(!stringStartsWith(innerPath, "simulink/systems/"))
			continue;
		string subsystemName = innerPath.substr(17, innerPath.length() - 17 - 4);
		//cout << ze.unc_size << "\t " << ze.name << endl;
        if(subsystemName.find("/") != string::npos)
            continue;

		char *ibuf = new char[ze.unc_size];
		UnzipItem(hz,zi, ibuf, ze.unc_size);

		tinyxml2::XMLDocument doc;
		tinyxml2::XMLError ret = doc.LoadFile(ibuf, (int)ze.unc_size);
		if (ret != 0) {
			fprintf(stderr, "Fail to load model file: %s/%s\n", filePath.c_str(),innerPath.c_str());
			res = -ErrorLoadSLInnerModelFail;
			break;
		}
		delete[] ibuf;

		tinyxml2::XMLElement* root = doc.RootElement();
		//tinyxml2::XMLPrinter printer;
		//doc.Print(&printer);
		//cout << printer.CStr() << endl << "===========================" << endl;

		//cout << subsystemName << endl;

		res = parseSubsystem(root, subsystemName);
		if(res < 0)
		{
			break;
		}
	}
	CloseZip(hz);

	return res;
}

int ParserForSimulink::parseSubsystem(tinyxml2::XMLElement* root, string name)
{
	int res;
	SLSubsystem* subsystem = new SLSubsystem();
	subsystems.push_back(subsystem);
	objectList.push_back(subsystem);
	subsystem->objType = SLObject::TypeSubsystem;
	subsystem->parent = NULL;
	subsystem->name = name;
	string sid = name.substr(7,name.length() - 7);
	if(sid == "root")
		subsystem->sid = -1;
	else
		subsystem->sid = stringToInt(sid);

	
	res = parseParameter(subsystem, root);
	if(res < 0)
		return res;

	res = parseBlock(subsystem, root);
	if(res < 0)
		return res;

	res = parseSubsystemPort(subsystem, root);
	if(res < 0)
		return res;

	res = parseLine(subsystem, root);
	if(res < 0)
		return res;
	

	return 1;
}

int ParserForSimulink::parseSubsystemPort(SLSubsystem* subsystem, tinyxml2::XMLElement* root)
{
	int res;
	tinyxml2::XMLElement *c = root->FirstChildElement("Block");
	while (c)
	{
		string blockType;
		if(c->FindAttribute("BlockType"))
			blockType = string(c->Attribute("BlockType"));
		else
			return -ErrorSLBlockHasNoAttributeBlockType;

		if(SLBlock::isActorBlockType(blockType))
		{
			c = c->NextSiblingElement("Block");
			continue;
		}

        
		if(blockType == "Inport")
		{
			SLInport* inport = new SLInport();
			subsystem->inports.push_back(inport);
			objectList.push_back(inport);
			inport->parent = subsystem;
			inport->objType = SLObject::TypeInport;

			

			if(c->FindAttribute("Name"))
				inport->name = string(c->Attribute("Name"));
			else
				return -ErrorSLInportHasNoAttributeName;

			if(c->FindAttribute("SID"))
				inport->sid = stringToInt(string(c->Attribute("SID")));
			else
				return -ErrorSLInportHasNoAttributeSID;

			res = parseParameter(inport, c);
			if(res < 0)
				return res;

            

			inport->portId = parseSubsystemPortId(inport);
            inport->type = parseSubsystemPortType(inport);
            inport->arraySize = parseSubsystemPortArraySize(inport);

		}
		else if(blockType == "Outport")
		{
			SLOutport* outport = new SLOutport();
			subsystem->outports.push_back(outport);
			objectList.push_back(outport);
			outport->parent = subsystem;
			outport->objType = SLObject::TypeOutport;

			outport->type = "#Default";

			if(c->FindAttribute("Name"))
				outport->name = string(c->Attribute("Name"));
			else
				return -ErrorSLOutportHasNoAttributeName;

			if(c->FindAttribute("SID"))
				outport->sid = stringToInt(string(c->Attribute("SID")));
			else
				return -ErrorSLOutportHasNoAttributeSID;

			res = parseParameter(outport, c);
			if(res < 0)
				return res;

			outport->portId = parseSubsystemPortId(outport);
            outport->type = parseSubsystemPortType(outport);
            outport->arraySize = parseSubsystemPortArraySize(outport);

		}
		else if(blockType == "ActionPort")
		{
			SLActionPort* actionPort = new SLActionPort();
			subsystem->actionPorts.push_back(actionPort);
			objectList.push_back(actionPort);
			actionPort->parent = subsystem;
			actionPort->objType = SLObject::TypeActionPort;

			actionPort->type = "IfAction";

			if(c->FindAttribute("Name"))
				actionPort->name = string(c->Attribute("Name"));
			else
				return -ErrorSLActionPortHasNoAttributeName;



			if(c->FindAttribute("SID"))
				actionPort->sid = stringToInt(string(c->Attribute("SID")));
			else
				return -ErrorSLActionPortHasNoAttributeSID;

			res = parseParameter(actionPort, c);
			if(res < 0)
				return res;

		}
		else if(blockType == "TriggerPort")
		{
			SLActionPort* actionPort = new SLActionPort();
			subsystem->actionPorts.push_back(actionPort);
			objectList.push_back(actionPort);
			actionPort->parent = subsystem;
			actionPort->objType = SLObject::TypeActionPort;

			actionPort->type = "Trigger";

			if(c->FindAttribute("Name"))
				actionPort->name = string(c->Attribute("Name"));
			else
				return -ErrorSLActionPortHasNoAttributeName;



			if(c->FindAttribute("SID"))
				actionPort->sid = stringToInt(string(c->Attribute("SID")));
			else
				return -ErrorSLActionPortHasNoAttributeSID;

			res = parseParameter(actionPort, c);
			if(res < 0)
				return res;

		}
		else if(blockType == "EnablePort")
		{
			SLActionPort* actionPort = new SLActionPort();
			subsystem->actionPorts.push_back(actionPort);
			objectList.push_back(actionPort);
			actionPort->parent = subsystem;
			actionPort->objType = SLObject::TypeActionPort;

			actionPort->type = "Enable";

			if(c->FindAttribute("Name"))
				actionPort->name = string(c->Attribute("Name"));
			else
				return -ErrorSLActionPortHasNoAttributeName;



			if(c->FindAttribute("SID"))
				actionPort->sid = stringToInt(string(c->Attribute("SID")));
			else
				return -ErrorSLActionPortHasNoAttributeSID;

			res = parseParameter(actionPort, c);
			if(res < 0)
				return res;

		}

		
		c = c->NextSiblingElement("Block");
	}
	return 1;
}

int ParserForSimulink::parseSubsystemPortId(SLObject* port)
{
	if(port->parameters.find("Port") != port->parameters.end())
	{
		return stringToInt(port->parameters["Port"]);
	}
    return 1;
}

std::vector<int> ParserForSimulink::parseSubsystemPortArraySize(SLObject* port)
{
    vector<int> arraySize;
    if(port->parameters.find("PortDimensions") != port->parameters.end())
    {
        string portDimensionsStr = port->parameters["PortDimensions"];
        if(stringStartsWith(portDimensionsStr, "["))
        {
            portDimensionsStr = portDimensionsStr.substr(1, portDimensionsStr.length() - 2);
        }
        vector<string> arraySizeStrSplit = stringSplit(portDimensionsStr, ",");
	    for(int i = 0;i < arraySizeStrSplit.size();i++)
	    {
		    arraySize.push_back(stringToInt(arraySizeStrSplit[i]));
	    }
    }
    if(arraySize.size() == 1 && arraySize[0] == 1)
        return vector<int>();
    return arraySize;
}

std::string ParserForSimulink::parseSubsystemPortType(SLObject* port)
{
    string retType = "#Default";
    if(port->parameters.find("OutDataTypeStr") != port->parameters.end()) {
        retType = port->parameters.at("OutDataTypeStr");
        if(basicTypeNameMap.find(retType) != basicTypeNameMap.end()) {
            retType = basicTypeNameMap.at(retType);
        }
    }
    return retType;
}

int ParserForSimulink::parseSLBlockMaskParameter(SLBlock* block, tinyxml2::XMLElement* root)
{
    int res;
	tinyxml2::XMLElement *c = root->FirstChildElement("Mask");

    if(!c)
        return 0;


	c = c->FirstChildElement("MaskParameter");

	while (c)
	{
		string paraName;
		string paraValue;


		if(c->FindAttribute("Name"))
			paraName = string(c->Attribute("Name"));
		else
			return -ErrorSLFunctionHasNoAttributeMaskParameterName;

        tinyxml2::XMLElement *cc = c->FirstChildElement("Value");
        if(cc)
			paraValue = string(cc->GetText());
		else
			return -ErrorSLFunctionHasNoAttributeMaskParameterValue;

        block->parameters[paraName] = paraValue;
        
		c = c->NextSiblingElement("MaskParameter");
    }
    return 0;
}

int ParserForSimulink::parseBlock(SLSubsystem* subsystem, tinyxml2::XMLElement* root)
{
	int res;
	tinyxml2::XMLElement *c = root->FirstChildElement("Block");
	while (c)
	{
		string blockType;
		if(c->FindAttribute("BlockType"))
			blockType = string(c->Attribute("BlockType"));
		else
			return -ErrorSLBlockHasNoAttributeBlockType;

		if(!SLBlock::isActorBlockType(blockType))
		{
			c = c->NextSiblingElement("Block");
			continue;
		}


		SLBlock* block = new SLBlock();
		subsystem->blocks.push_back(block);
		objectList.push_back(block);
		block->parent = subsystem;
		block->objType = SLObject::TypeBlock;

		block->type = blockType;


		if(c->FindAttribute("Name"))
			block->name = string(c->Attribute("Name"));
		else
			return -ErrorSLBlockHasNoAttributeName;

		if(c->FindAttribute("SID"))
			block->sid = stringToInt(string(c->Attribute("SID")));
		else
			return -ErrorSLBlockHasNoAttributeSID;

		res = parseParameter(block, c);
		if(res < 0)
			return res;

		res = parseBlockPort(block, c);
		if(res < 0)
			return res;

		if(block->type == "SubSystem")
		{
			tinyxml2::XMLElement *cc = c->FirstChildElement("System");
			if(cc && cc->FindAttribute("Ref"))
				block->systemRef = string(cc->Attribute("Ref"));
			else
				return -ErrorSLBlockSubSystemHasNoRef;

            
	        res = parseSLBlockMaskParameter(block, c);
	        if(res < 0)
		        return res;
		}
		else if(block->type == "Reference")
		{
			block->sourceBlockType = block->parameters["SourceType"];
		}

		
		c = c->NextSiblingElement("Block");
	}
	return 1;
}

namespace 
{
    map<string, string> blockDefaultPorts = {
        make_pair("DotProduct", "[2,1]"),
        make_pair("Switch", "[3,1]"),
        make_pair("Constant", "[0,1]"),
        make_pair("DataStoreMemory", "[0,0]"),
        make_pair("Terminator", "[1,0]"),
        make_pair("Assertion", "[1,0]"),
    };
}

int ParserForSimulink::parseBlockPort(SLBlock* block, tinyxml2::XMLElement* root)
{
	string portParameterString;
    //解析2022B版本Simulink组件端口
    tinyxml2::XMLElement *portCounts = root->FirstChildElement("PortCounts");
    if(portCounts)
    {
        string inPortCount = portCounts->FindAttribute("in") ? string(portCounts->Attribute("in")) : "0";
        string outPortCount = portCounts->FindAttribute("out") ? string(portCounts->Attribute("out")) : "0";
        string enablePortCount = portCounts->FindAttribute("enable") ? string(portCounts->Attribute("enable")) : "0";
        string ifActionPortCount = portCounts->FindAttribute("ifaction") ? string(portCounts->Attribute("ifaction")) : "0";
        string triggerPortCount = portCounts->FindAttribute("trigger") ? string(portCounts->Attribute("trigger")) : "0";
        portParameterString = "[" + inPortCount + "," + outPortCount + "," + enablePortCount + "," + triggerPortCount + ",0,0,0," + ifActionPortCount + "]";
        block->parameters["Ports"] = portParameterString;
    }
	if(block->parameters.find("Ports") == block->parameters.end())
    {
        if(blockDefaultPorts.find(block->type) != blockDefaultPorts.end())
            portParameterString = blockDefaultPorts.at(block->type);
        else
		    portParameterString = "[1,1]";//Block端口默认为1个输入1个输出

	}
    else
	{
	    portParameterString = stringTrim(block->parameters["Ports"]);
	}
	
	//去除两端的"[]"
	portParameterString = portParameterString.substr(1,portParameterString.length()-2);
	vector<string> portParameterStringSp = stringSplit(portParameterString, ",");
	for(int i = 0;i < portParameterStringSp.size();i++)
	{
		portParameterStringSp[i] = stringTrim(portParameterStringSp[i]);
	}
	/*port type*/
	// 0 : inports
	// 1 : outports
	// 2 : enable ports
	// 3 : trigger ports
	// 4 : state
	// 5 : LConn
	// 6 : RConn
	// 7 : ifaction


	for(int i = 0;i < portParameterStringSp.size();i++)
	{
		int portCount = stringToInt(portParameterStringSp[i]);
		
		for(int j = 0;j < portCount;j++)
		{
			if(i == 0)
			{
				SLInport* inport = new SLInport();
				inport->portId = j + 1;
				inport->name = "#DefaultInport" + to_string(inport->portId);
				inport->type = "#Default";
				block->inports.push_back(inport);
			}
			else if(i == 1)
			{
				SLOutport* outport = new SLOutport();
				outport->portId = j + 1;
				outport->name = "#DefaultOutport" + to_string(outport->portId);
				outport->type = "#Default";
				block->outports.push_back(outport);
			}
            else if(i == 2)
			{
				SLActionPort* enablePort = new SLActionPort();
				enablePort->name = "#DefaultEnable";
				enablePort->type = "Enable";
				block->actionPorts.push_back(enablePort);
			}
			else if(i == 3)
			{
				SLActionPort* actionPort = new SLActionPort();
				actionPort->name = "#DefaultTrigger";
				actionPort->type = "Trigger";
				block->actionPorts.push_back(actionPort);
			}
			else if(i == 7)
			{
				SLActionPort* actionPort = new SLActionPort();
				actionPort->name = "#DefaultIfAction";
				actionPort->type = "IfAction";
				block->actionPorts.push_back(actionPort);
			}
		}
	}

    

	return 1;
}

int ParserForSimulink::parseLine(SLSubsystem* subsystem, tinyxml2::XMLElement* root)
{
	tinyxml2::XMLElement *c = root->FirstChildElement("Line");
	while (c)
	{
		SLLine* line = new SLLine();
		line->sid = -1;
		subsystem->lines.push_back(line);
		objectList.push_back(line);
		line->parent = subsystem;
		line->objType = SLObject::TypeLine;

		queue<tinyxml2::XMLElement*> children;
		children.push(c);
		bool firstDeal = true;
		while(!children.empty())
		{
			tinyxml2::XMLElement* front = children.front();
			children.pop();
			if(!firstDeal && string(front->Name()) == "P")
			{
				string eleName;
				if(front->FindAttribute("Name"))
					eleName = string(front->Attribute("Name"));

				if(eleName == "Src")
				{
					string srcName = string(front->GetText());
					line->srcs.push_back(srcName);
				}
				else if(eleName == "Dst")
				{
					string dstName = string(front->GetText());
					line->dsts.push_back(dstName);
				}
			}
			else
			{
				firstDeal = false;
			}
			
			tinyxml2::XMLElement *cc = front->FirstChildElement();
			while (cc)
			{
				children.push(cc);
				cc = cc->NextSiblingElement();
			}
		}

		c = c->NextSiblingElement("Line");
	}
	return 1;
}


int ParserForSimulink::parseParameter(SLObject* object, tinyxml2::XMLElement* root)
{
	tinyxml2::XMLElement *c = root->FirstChildElement("P");
	while (c)
	{
		string name;
		if(c->FindAttribute("Name"))
			name = string(c->Attribute("Name"));
		else
			return -ErrorSLObjectHasNoAttributeName;


		string value = string(c->GetText() != nullptr ? c->GetText() : "");

		object->parameters[name] = value;

		c = c->NextSiblingElement("P");
	}
    c = root->FirstChildElement("InstanceData");
    if(c)
        parseParameter(object, c);
	return 1;
}

void ParserForSimulink::release()
{
	for(int i = 0;i < subsystems.size(); i++)
	{
		subsystems[i]->release();
		delete subsystems[i];
	}
	subsystems.clear();
}
