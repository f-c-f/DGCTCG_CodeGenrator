#include "SLConverter.h"
using namespace std;


int SLConverter::convert(ParserForSimulink* parserForSimulink, MIRModel* mIRModel)
{
	int res;
	
	this->parserForSimulink = parserForSimulink;
	this->mIRModel = mIRModel;

	res = convert(mIRModel);
	if(res < 0)
		return res;

	return 1;
}

int SLConverter::convert(MIRModel* mIRModel)
{
	int res;
	mIRModel->release();
	mIRModel->objType = MIRObject::TypeModel;
    mIRModel->modelSrcType = "Simulink";

	for(int i = 0;i < parserForSimulink->subsystems.size();i++)
	{
		res = convertSubsystem(parserForSimulink->subsystems[i], mIRModel);
		if(res < 0)
			return res;
	}
	return 1;
}

int SLConverter::convertSubsystem(SLSubsystem* subsystem, MIRModel* mIRModel)
{
	int res;
	MIRFunctionDataflow* mIRFunction = new MIRFunctionDataflow();
    mIRFunction->type = MIRFunction::TypeDataflow;
	mIRFunction->objType = MIRObject::TypeFunction;
	mIRFunction->name = subsystem->name;

    if(mIRFunction->name == "system_root")
        mIRModel->mainFunction = "system_root";

	mIRModel->functions.push_back(mIRFunction);
	
	for(int i = 0;i < subsystem->inports.size();i++)
	{
		res = convertSubsystemInport(subsystem->inports[i], mIRFunction);
		if(res < 0)
			return res;
	}
	for(int i = 0;i < subsystem->outports.size();i++)
	{
		res = convertSubsystemOutport(subsystem->outports[i], mIRFunction);
		if(res < 0)
			return res;
	}
	for(int i = 0;i < subsystem->actionPorts.size();i++)
	{
		res = convertSubsystemActionPort(subsystem->actionPorts[i], mIRFunction);
		if(res < 0)
			return res;
	}
	for(int i = 0;i < subsystem->blocks.size();i++)
	{
		res = convertBlock(subsystem->blocks[i], mIRFunction);
		if(res < 0)
			return res;
	}
	for(int i = 0;i < subsystem->lines.size();i++)
	{
		res = convertLine(subsystem->lines[i], mIRFunction);
		if(res < 0)
			return res;
	}
	
	res = convertParameter(subsystem, mIRFunction);
	if(res < 0)
		return res;

	return 1;
}

int SLConverter::convertBlock(SLBlock* block, MIRFunctionDataflow* mIRFunction)
{	
	int res;
	MIRActor* mIRActor = new MIRActor();
	mIRActor->objType = MIRObject::TypeActor;
	string blockName;
	res = trimNameStr(block->name, blockName);
	if(res < 0)
		return res;
	mIRActor->name = blockName;

    string blockType;
	if(block->type == "SubSystem")
		blockType = "Function";
	else if(block->type == "Reference")
		blockType = block->sourceBlockType;
	else
		blockType = block->type;

    res = trimTypeStr(blockType, blockType);
	if(res < 0)
		return res;

    
    vector<string> blockTypeSp = stringSplit(blockType, ".");
    if(blockTypeSp.size() > 1)
        blockType = blockTypeSp[blockTypeSp.size() - 1];

    mIRActor->type = blockType;

	mIRFunction->actors.push_back(mIRActor);

	
	for(int i = 0;i < block->inports.size();i++)
	{
		res = convertBlockInport(block->inports[i], mIRActor);
		if(res < 0)
			return res;
	}
	for(int i = 0;i < block->outports.size();i++)
	{
		res = convertBlockOutport(block->outports[i], mIRActor);
		if(res < 0)
			return res;
	}
	for(int i = 0;i < block->actionPorts.size();i++)
	{
		res = convertBlockActionPort(block->actionPorts[i], mIRActor);
		if(res < 0)
			return res;
	}

	

	if(!block->systemRef.empty())
	{
		mIRActor->parametersOfXML["FunctionRef"] = block->systemRef;
	}

    res = convertParameter(block, mIRActor);
    if (res < 0)
        return res;

	return 1;
}

int SLConverter::convertLine(SLLine* line, MIRFunctionDataflow* mIRFunction)
{
	int res;
	MIRRelation* mIRRelation = new MIRRelation();
	mIRRelation->objType = MIRObject::TypeRelation;

	mIRFunction->relations.push_back(mIRRelation);

	for(int i = 0;i < line->srcs.size();i++)
	{
		string srcStr;
		res = convertLineSrcDstStr(line->srcs[i], srcStr);
		if(res < 0)
			return res;
		mIRRelation->srcStrs.push_back(srcStr);
	}
	for(int i = 0;i < line->dsts.size();i++)
	{
		string dstStr;
		res = convertLineSrcDstStr(line->dsts[i], dstStr);
		if(res < 0)
			return res;
		mIRRelation->dstStrs.push_back(dstStr);
	}
	
	res = convertParameter(line, mIRRelation);
	if(res < 0)
		return res;

	return 1;
}

int SLConverter::convertSubsystemInport(SLInport* inport, MIRFunction* mIRFunction)
{
	int res;
	MIRInport* mIRInport = new MIRInport();
	mIRInport->objType = MIRObject::TypeInport;
    string inportName;
    res = trimNameStr(inport->name, inportName);
	if(res < 0)
		return res;
	mIRInport->name = inportName;
	mIRInport->type = inport->type;
	mIRInport->arraySize = inport->arraySize;

	mIRFunction->inports.push_back(mIRInport);
	
	res = convertParameter(inport, mIRInport);
	if(res < 0)
		return res;

	return 1;
}

int SLConverter::convertSubsystemOutport(SLOutport* outport, MIRFunction* mIRFunction)
{
	int res;
	MIROutport* mIROutport = new MIROutport();
	mIROutport->objType = MIRObject::TypeOutport;
    string outportName;
    res = trimNameStr(outport->name, outportName);
	if(res < 0)
		return res;
	mIROutport->name = outportName;
	mIROutport->type = outport->type;
	mIROutport->arraySize = outport->arraySize;

	mIRFunction->outports.push_back(mIROutport);
	
	res = convertParameter(outport, mIROutport);
	if(res < 0)
		return res;

	return 1;
}

int SLConverter::convertSubsystemActionPort(SLActionPort* actionPort, MIRFunction* mIRFunction)
{
	int res;
	MIRActionPort* mIRActionPort = new MIRActionPort();
	mIRActionPort->objType = MIRObject::TypeActionPort;
	string actionPortName;
	res = trimNameStr(actionPort->name, actionPortName);
	if(res < 0)
		return res;
	mIRActionPort->name = actionPortName;
	mIRActionPort->type = actionPort->type;

	mIRFunction->actionPorts.push_back(mIRActionPort);
	
	res = convertParameter(actionPort, mIRActionPort);
	if(res < 0)
		return res;

	return 1;
}

int SLConverter::convertBlockInport(SLInport* inport, MIRActor* mIRActor)
{
	int res;
	MIRInport* mIRInport = new MIRInport();
	mIRInport->objType = MIRObject::TypeInport;
    string inportName;
    res = trimNameStr(inport->name, inportName);
	if(res < 0)
		return res;
	mIRInport->name = inportName;
	mIRInport->type = inport->type;
	mIRInport->arraySize = inport->arraySize;

	mIRActor->inports.push_back(mIRInport);
	
	res = convertParameter(inport, mIRInport);
	if(res < 0)
		return res;

	return 1;
}

int SLConverter::convertBlockOutport(SLOutport* outport, MIRActor* mIRActor)
{
	int res;
	MIROutport* mIROutport = new MIROutport();
	mIROutport->objType = MIRObject::TypeOutport;
    string outportName;
    res = trimNameStr(outport->name, outportName);
	if(res < 0)
		return res;
	mIROutport->name = outportName;
	mIROutport->type = outport->type;
	mIROutport->arraySize = outport->arraySize;

	mIRActor->outports.push_back(mIROutport);
	
	res = convertParameter(outport, mIROutport);
	if(res < 0)
		return res;

	return 1;
}

int SLConverter::convertBlockActionPort(SLActionPort* actionPort, MIRActor* mIRActor)
{
	int res;
	MIRActionPort* mIRActionPort = new MIRActionPort();
	mIRActionPort->objType = MIRObject::TypeActionPort;
	string actionPortName;
	res = trimNameStr(actionPort->name, actionPortName);
	if(res < 0)
		return res;
	mIRActionPort->name = actionPortName;
	mIRActionPort->type = actionPort->type;

	mIRActor->actionPorts.push_back(mIRActionPort);

	res = convertParameter(actionPort, mIRActionPort);
	if(res < 0)
		return res;

	return 1;
}

int SLConverter::convertParameter(SLObject* object, MIRObject* mIRObject)
{
	map<string, string>::iterator iter = object->parameters.begin();
	for(;iter!=object->parameters.end();iter++)
	{
        string name = iter->first;
        if(name[0] >= 'a' && name[0] <= 'z' && !(object->objType == SLObject::TypeBlock && ((SLBlock*)object)->type == "SubSystem"))
            name[0] = name[0] - 32;
		mIRObject->parametersOfXML[name] = iter->second;
	}
	return 1;
}

int SLConverter::convertLineSrcDstStr(string str, string &ret)
{
	int res;
	//11#out:1
	int sid;//block唯一id
	int index;//第i个端口

	size_t pos1 = str.find("#");
	size_t pos2 = str.find(":");
	if(pos1 == string::npos && pos2 == string::npos)
	{
		return -SLConverter::ErrorLineSrcDstStrInvalid;
	}
	
	string sidStr;
	string ioaStr;//in out action
	string indexStr;

	if(pos2 != string::npos)
	{
		sidStr = str.substr(0,pos1);
		ioaStr = str.substr(pos1+1,pos2-pos1-1);
		indexStr = str.substr(pos2+1,str.length() - pos2 - 1);
	}
	else
	{
		sidStr = str.substr(0,pos1);
		ioaStr = str.substr(pos1+1,str.length() - pos1 - 1);
	}

	

	sid = stringToInt(sidStr);
	index = stringToInt(indexStr);

	string retStr;

	for(int i = 0;i < parserForSimulink->objectList.size(); i++)
	{
		SLObject* obj = parserForSimulink->objectList[i];
		if(sid != obj->sid)
			continue;
		if(obj->objType == SLObject::TypeSubsystem)
			continue;

		if(obj->objType == SLObject::TypeInport && ioaStr == "out")
		{
			SLInport* inport = (SLInport*)obj;
            string inportName;
			res = trimNameStr(inport->name, inportName);
			if(res < 0)
				return res;
			retStr = inportName;
		}
		else if(obj->objType == SLObject::TypeOutport && ioaStr == "in")
		{
			SLOutport* outport = (SLOutport*)obj;
            string outportName;
			res = trimNameStr(outport->name, outportName);
			if(res < 0)
				return res;
			retStr = outportName;
		}
		else if(obj->objType == SLObject::TypeBlock)
		{
			SLBlock* block = (SLBlock*)obj;
			string blockName;
			res = trimNameStr(block->name, blockName);
			if(res < 0)
				return res;

			if(ioaStr == "in")
			{
				retStr = blockName + ".#DefaultInport" + to_string(index);
			}
			else if(ioaStr == "out")
			{
				retStr = blockName + ".#DefaultOutport" + to_string(index);
			}
			else if(ioaStr == "ifaction")
			{
				retStr = blockName + ".#DefaultIfAction";
			}
			else if(ioaStr == "trigger")
			{
				retStr = blockName + ".#DefaultTrigger";
			}
			else if(ioaStr == "enable")
			{
				retStr = blockName + ".#DefaultEnable";
			}
			
		}

		break;
	}

	if(retStr.empty())
	{
		return -SLConverter::ErrorLineSrcDstStrInvalid;
	}
	ret = retStr;
	return 1;
}

int SLConverter::trimNameStr(string str, string& ret)
{
	string retStr;
	for(int i = 0; i < str.length(); i++)
	{
		if(str[i] == ' ')
			retStr += "_";
        else if(str[i] == '-')
			;
        else if(str[i] == '\'')
			retStr += "_a_";
		else if(str[i] == '\n' || str[i] == '\r')
			;
		else
			retStr += str[i];
	}
	ret = retStr;
	return 1;
}

int SLConverter::trimTypeStr(std::string str, std::string& ret)
{
	string retStr;
	for(int i = 0; i < str.length(); i++)
	{
		if(str[i] == ' ')
			;
        else if(str[i] == '-')
			;
		else if(str[i] == '\n' || str[i] == '\r')
			;
		else
			retStr += str[i];
	}
	ret = retStr;
	return 1;
}
