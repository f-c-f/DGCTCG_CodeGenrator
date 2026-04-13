#include "PTConverter.h"
using namespace std;


#include "PTInport.h"
#include "PTOutport.h"
#include "PTProperty.h"
#include "PTRelation.h"
#include "PTLink.h"
#include "PTObject.h"
#include "PTEntity.h"
#include "PTActor.h"

int PTConverter::convert(ParserForPtolemy* parserForPtolemy, MIRModel* mIRModel)
{
	int res;
	
	this->parserForPtolemy = parserForPtolemy;
	this->mIRModel = mIRModel;

	res = convert(mIRModel);
	if(res < 0)
		return res;

	return 1;
}

int PTConverter::convert(MIRModel* mIRModel)
{
	int res;
	mIRModel->release();
	mIRModel->objType = MIRObject::TypeModel;
    mIRModel->mainFunction = parserForPtolemy->rootEntity->name;
    mIRModel->modelSrcType = "Ptolemy";

	for(int i = 0;i < parserForPtolemy->entityList.size();i++)
	{
		res = convertEntity(parserForPtolemy->entityList[i], mIRModel);
		if(res < 0)
			return res;
	}

    res = mapActors();
	if(res < 0)
		return res;
    

	return 1;
}

int PTConverter::convertEntity(PTEntity* entity, MIRModel* mIRModel)
{
	int res;
	MIRFunctionDataflow* mIRFunction = new MIRFunctionDataflow();
    mIRFunction->type = MIRFunction::TypeDataflow;
	mIRFunction->objType = MIRObject::TypeFunction;
	mIRFunction->name = entity->name;

    if(mIRFunction->name == "system_root")
        mIRModel->mainFunction = "system_root";

	mIRModel->functions.push_back(mIRFunction);
	
	for(int i = 0;i < entity->inports.size();i++)
	{
		res = convertEntityInport(entity->inports[i], mIRFunction);
		if(res < 0)
			return res;
	}
	for(int i = 0;i < entity->outports.size();i++)
	{
		res = convertEntityOutport(entity->outports[i], mIRFunction);
		if(res < 0)
			return res;
	}
	for(int i = 0;i < entity->actors.size();i++)
	{
		res = convertActor(entity->actors[i], mIRFunction);
		if(res < 0)
			return res;
	}
	
	res = convertParameter(entity, mIRFunction);
	if(res < 0)
		return res;

    res = convertLink(entity, mIRFunction);
	if(res < 0)
		return res;
    

	return 1;
}

int PTConverter::convertActor(PTActor* actor, MIRFunctionDataflow* mIRFunction)
{	
	int res;
	MIRActor* mIRActor = new MIRActor();
	mIRActor->objType = MIRObject::TypeActor;
	string blockName;
	res = trimNameStr(actor->name, blockName);
	if(res < 0)
		return res;
	mIRActor->name = blockName;

    string blockType;
	if(actor->type == "SubSystem")
		blockType = "Function";
	else
		blockType = actor->type;

    res = trimTypeStr(blockType, blockType);
	if(res < 0)
		return res;

    mIRActor->type = blockType;

	mIRFunction->actors.push_back(mIRActor);

	
	for(int i = 0;i < actor->inports.size();i++)
	{
		res = convertActorInport(actor->inports[i], mIRActor);
		if(res < 0)
			return res;
	}
	for(int i = 0;i < actor->outports.size();i++)
	{
		res = convertActorOutport(actor->outports[i], mIRActor);
		if(res < 0)
			return res;
	}

	

	
	res = convertParameter(actor, mIRActor);
	if(res < 0)
		return res;

	return 1;
}

int PTConverter::convertLink(PTEntity* entity, MIRFunctionDataflow* mIRFunction)
{
    //并查集算法确定联通关系
    map<string, string> relationUnionParent;
    for(int i = 0; i < entity->links.size(); i++)
    {
        PTLink* link = entity->links[i];
        if(!link->relation1.empty() && !link->relation2.empty())
        {
            string p1 = link->relation1;
            while(relationUnionParent.find(p1) != relationUnionParent.end())
            {
                if(p1 == relationUnionParent[p1])
                    break;
                p1 = relationUnionParent[p1];
            }
            string p2 = link->relation2;
            while(relationUnionParent.find(p2) != relationUnionParent.end())
            {
                if(p2 == relationUnionParent[p2])
                    break;
                p2 = relationUnionParent[p2];
            }
            relationUnionParent[p1] = p2;
        }
    }
    //统计联通域
    map<string, vector<string>> relationPortMap;
    for(int i = 0; i < entity->links.size(); i++)
    {
        PTLink* link = entity->links[i];
        if(!link->port.empty() && !link->relation.empty())
        {
            string p = link->relation;
            while(relationUnionParent.find(p) != relationUnionParent.end())
            {
                if(p == relationUnionParent[p])
                    break;
                p = relationUnionParent[p];
            }

            vector<string> portSp = stringSplit(link->port, ".");
            string portName;
            if(portSp.size() == 2)
            {
                if(portSp[1][0] >= 'a' && portSp[1][0] <= 'z')
                    portSp[1][0] = portSp[1][0] - 32;
                portName = portSp[0] + "." + portSp[1];
            }
            else
            {
                portName = portSp[0];
            }
            
            relationPortMap[p].push_back(portName);
        }
    }
    // 收集端口名和端口映射
    vector<pair<string, MIRObject*>> srcPortList;
    vector<pair<string, MIRObject*>> dstPortList;
    for(int i = 0; i < mIRFunction->actors.size(); i++)
    {
        MIRActor* actor = mIRFunction->actors[i];
        for(int j = 0; j < actor->outports.size(); j++)
        {
            MIROutport* outport = actor->outports[j];
            srcPortList.push_back(make_pair(actor->name + "." + outport->name, outport));
        }
        for(int j = 0; j < actor->inports.size(); j++)
        {
            MIRInport* inport = actor->inports[j];
            dstPortList.push_back(make_pair(actor->name + "." + inport->name, inport));
        }
    }
    for(int j = 0; j < mIRFunction->outports.size(); j++)
    {
        MIROutport* outport = mIRFunction->outports[j];
        dstPortList.push_back(make_pair(outport->name, outport));
    }
    for(int j = 0; j < mIRFunction->inports.size(); j++)
    {
        MIRInport* inport = mIRFunction->inports[j];
        srcPortList.push_back(make_pair(inport->name, inport));
    }

    //创建Relation
    auto relationPortMapIter = relationPortMap.begin();
    for(; relationPortMapIter != relationPortMap.end(); relationPortMapIter++)
    {
        MIRRelation* relation = new MIRRelation();
        relation->objType = MIRObject::TypeRelation;
        mIRFunction->relations.push_back(relation);

        vector<string>& ports = relationPortMapIter->second;
        for(int i = 0; i < ports.size(); i++)
        {
            string portFullName = ports[i];
            for(int j = 0; j < srcPortList.size(); j++)
            {
                if(srcPortList[j].first == portFullName)
                {
                    relation->srcStrs.push_back(portFullName);
                    relation->srcObjs.push_back(srcPortList[j].second);
                }
            }
            for(int j = 0; j < dstPortList.size(); j++)
            {
                if(dstPortList[j].first == portFullName)
                {
                    relation->dstStrs.push_back(portFullName);
                    relation->dstObjs.push_back(dstPortList[j].second);
                }
            }
        }
    }
    return 1;
}


int PTConverter::convertEntityInport(PTInport* inport, MIRFunction* mIRFunction)
{
	int res;
	MIRInport* mIRInport = new MIRInport();
	mIRInport->objType = MIRObject::TypeInport;
    mIRInport->parent = mIRFunction;
    string inportName;
    res = trimNameStr(inport->name, inportName);
	if(res < 0)
		return res;
    if(inportName[0] >= 'a' && inportName[0] <= 'z')
        inportName[0] = inportName[0] - 32;
	mIRInport->name = inportName;
	mIRInport->type = inport->type;
	mIRInport->arraySize = inport->arraySize;

	mIRFunction->inports.push_back(mIRInport);
	
	res = convertParameter(inport, mIRInport);
	if(res < 0)
		return res;

	return 1;
}

int PTConverter::convertEntityOutport(PTOutport* outport, MIRFunction* mIRFunction)
{
	int res;
	MIROutport* mIROutport = new MIROutport();
	mIROutport->objType = MIRObject::TypeOutport;
    mIROutport->parent = mIRFunction;
    string outportName;
    res = trimNameStr(outport->name, outportName);
	if(res < 0)
		return res;
    if(outportName[0] >= 'a' && outportName[0] <= 'z')
        outportName[0] = outportName[0] - 32;
	mIROutport->name = outportName;
	mIROutport->type = outport->type;
	mIROutport->arraySize = outport->arraySize;

	mIRFunction->outports.push_back(mIROutport);
	
	res = convertParameter(outport, mIROutport);
	if(res < 0)
		return res;

	return 1;
}


int PTConverter::convertActorInport(PTInport* inport, MIRActor* mIRActor)
{
	int res;
	MIRInport* mIRInport = new MIRInport();
	mIRInport->objType = MIRObject::TypeInport;
    mIRInport->parent = mIRActor;
    string inportName;
    res = trimNameStr(inport->name, inportName);
	if(res < 0)
		return res;
    if(inportName[0] >= 'a' && inportName[0] <= 'z')
        inportName[0] = inportName[0] - 32;
	mIRInport->name = inportName;
	mIRInport->type = inport->type;
	mIRInport->arraySize = inport->arraySize;

	mIRActor->inports.push_back(mIRInport);
	
	res = convertParameter(inport, mIRInport);
	if(res < 0)
		return res;

	return 1;
}

int PTConverter::convertActorOutport(PTOutport* outport, MIRActor* mIRActor)
{
	int res;
	MIROutport* mIROutport = new MIROutport();
	mIROutport->objType = MIRObject::TypeOutport;
    mIROutport->parent = mIRActor;
    string outportName;
    res = trimNameStr(outport->name, outportName);
	if(res < 0)
		return res;
    if(outportName[0] >= 'a' && outportName[0] <= 'z')
        outportName[0] = outportName[0] - 32;
	mIROutport->name = outportName;
	mIROutport->type = outport->type;
	mIROutport->arraySize = outport->arraySize;

	mIRActor->outports.push_back(mIROutport);
	
	res = convertParameter(outport, mIROutport);
	if(res < 0)
		return res;

	return 1;
}

int PTConverter::convertParameter(PTObject* object, MIRObject* mIRObject)
{
	map<string, string>::iterator iter = object->parameters.begin();
	for(;iter!=object->parameters.end();iter++)
	{
        string name = iter->first;
        if(name[0] >= 'a' && name[0] <= 'z')
            name[0] = name[0] - 32;

		mIRObject->parametersOfXML[name] = iter->second;
	}
	return 1;
}


int PTConverter::trimNameStr(string str, string& ret)
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

int PTConverter::trimTypeStr(std::string str, std::string& ret)
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

PTConverter::MapActor* PTConverter::findMapActor(std::string name)
{
    for(int i = 0; i < mapActorList.size(); i++)
    {
        if(mapActorList[i].name == name)
            return &mapActorList[i];
    }
    return nullptr;
}

PTConverter::MapParameter* PTConverter::findMapParameter(MapActor* mapActor, std::string name)
{
    for(int i = 0; i < mapActor->mapParameters.size(); i++)
    {
        if(mapActor->mapParameters[i].name == name)
            return &mapActor->mapParameters[i];
    }
    return nullptr;
}

PTConverter::MapInport* PTConverter::findMapInport(MapActor* mapActor, std::string name)
{
    for(int i = 0; i < mapActor->mapInports.size(); i++)
    {
        if(mapActor->mapInports[i].name == name)
            return &mapActor->mapInports[i];
    }
    return nullptr;
}

PTConverter::MapOutport* PTConverter::findMapOutport(MapActor* mapActor, std::string name)
{
    for(int i = 0; i < mapActor->mapOutports.size(); i++)
    {
        if(mapActor->mapOutports[i].name == name)
            return &mapActor->mapOutports[i];
    }
    return nullptr;
}

int PTConverter::mapActors()
{
    int res;
    res = loadActorMapRule();
	if(res < 0)
		return res;

    res = mapActorsTraverseFunction(mIRModel);
	if(res < 0)
		return res;



    return 1;
}

int PTConverter::loadActorMapRule()
{
    int res;
    string exePath = getExeDirPath();
	tinyxml2::XMLDocument doc;
    string filePath = exePath + "/MapRules.xml";
	tinyxml2::XMLError ret = doc.LoadFile(filePath.c_str());
	if (ret != 0) {

		fprintf(stderr, "Fail to load model file: %s\n", filePath.c_str());
        fprintf(stderr, "%s\n", doc.ErrorStr());
		res = -ParserForPtolemy::ErrorLoadMapRulesFileFail;
		return res;
	}

	tinyxml2::XMLElement* root = doc.RootElement();

    tinyxml2::XMLElement *actor = root->FirstChildElement("Actor");
	while (actor)
	{
        MapActor mapActor;
        mapActor.name = string(actor->Attribute("Name"));
        if(actor->FindAttribute("Value"))
	        mapActor.value = string(actor->Attribute("Value"));

        tinyxml2::XMLElement *para = actor->FirstChildElement("Parameter");
	    while (para)
	    {
            MapParameter mapParameter;
            mapParameter.name = string(para->Attribute("Name"));
            if(para->FindAttribute("Value"))
	            mapParameter.value = string(para->Attribute("Value"));

            mapActor.mapParameters.push_back(mapParameter);
            para = para->NextSiblingElement("Parameter");
        }
        tinyxml2::XMLElement *inport = actor->FirstChildElement("Inport");
	    while (inport)
	    {
            MapInport mapInport;
            mapInport.name = string(inport->Attribute("Name"));
            if(inport->FindAttribute("Value"))
	            mapInport.value = string(inport->Attribute("Value"));

            mapActor.mapInports.push_back(mapInport);
            inport = inport->NextSiblingElement("Inport");
        }
        tinyxml2::XMLElement *outport = actor->FirstChildElement("Outport");
	    while (outport)
	    {
            MapOutport mapOutport;
            mapOutport.name = string(outport->Attribute("Name"));
            if(outport->FindAttribute("Value"))
	            mapOutport.value = string(outport->Attribute("Value"));

            mapActor.mapOutports.push_back(mapOutport);
            outport = outport->NextSiblingElement("Outport");
        }

        mapActorList.push_back(mapActor);
		actor = actor->NextSiblingElement("Actor");
	}
    return 1;
}

int PTConverter::mapActorsTraverseFunction(MIRModel* model)
{
    int res;
    for(int i = 0; i < model->functions.size(); i++)
    {
        if(model->functions[i]->type != MIRFunction::TypeDataflow)
            continue;
        res = mapActorsTraverseActor((MIRFunctionDataflow*)model->functions[i]);
        if(res < 0)
            return res;
        res = mapActorsUpdateLinkPort((MIRFunctionDataflow*)model->functions[i]);
        if(res < 0)
            return res;
        
    }
    return 1;
}

int PTConverter::mapActorsTraverseActor(MIRFunctionDataflow* function)
{
    int res;
    for(int i = 0; i < function->actors.size(); i++)
    {
        MIRActor* actor = function->actors[i];
        MapActor* mapActor = findMapActor(actor->type);

        if(actor->type == "Expression") {
            res = mapActorsTraverseActorHandleExpression(actor);
            if(res < 0)
                return res;
        }



        if(!mapActor)
            continue;
        if(!mapActor->value.empty())
            actor->type = mapActor->value;

        res = mapActorsTraverseParameter(actor, mapActor);
        if(res < 0)
            return res;
        res = mapActorsTraverseInport(actor, mapActor);
        if(res < 0)
            return res;
        res = mapActorsTraverseOutport(actor, mapActor);
        if(res < 0)
            return res;
    }
    return 1;
}

int PTConverter::mapActorsTraverseParameter(MIRActor* actor, MapActor* mapActor)
{
    int res;
    std::map<std::string, std::string> newParametersOfXML;

    auto iter = actor->parametersOfXML.begin();
    for(; iter != actor->parametersOfXML.end(); iter++)
    {
        MapParameter* mapParameter = findMapParameter(mapActor, iter->first);
        
        if(!mapParameter)
        {
            newParametersOfXML[iter->first] = iter->second;
        }
        else
        {
            newParametersOfXML[mapParameter->value] = iter->second;
        }
    }
    actor->parametersOfXML = newParametersOfXML;
    return 1;
}

int PTConverter::mapActorsTraverseInport(MIRActor* actor, MapActor* mapActor)
{
    int res;
    for(int i = 0; i < actor->inports.size(); i++)
    {
        MIRInport* port = actor->inports[i];
        MapInport* mapPort = findMapInport(mapActor, port->name);
        if(!mapPort)
            continue;
        if(!mapPort->value.empty())
            port->name = mapPort->value;
    }
    return 1;
}

int PTConverter::mapActorsTraverseOutport(MIRActor* actor, MapActor* mapActor)
{
    int res;
    for(int i = 0; i < actor->outports.size(); i++)
    {
        MIROutport* port = actor->outports[i];
        MapOutport* mapPort = findMapOutport(mapActor, port->name);
        if(!mapPort)
            continue;
        if(!mapPort->value.empty())
            port->name = mapPort->value;
    }
    return 1;
}

int PTConverter::mapActorsUpdateLinkPort(MIRFunctionDataflow* function)
{
    // 由于端口名映射，可能导致Relation信息失效
    for(int i = 0; i < function->relations.size(); i++)
    {
        MIRRelation* relation = function->relations[i];
        relation->srcStrs.clear();
        relation->dstStrs.clear();

        for(int j = 0; j < relation->srcObjs.size(); j++)
        {

            if(relation->srcObjs[j]->objType == MIRObject::TypeInport)
            {
                MIRInport* port = static_cast<MIRInport*>(relation->srcObjs[j]);
                relation->srcStrs.push_back(port->name);
            }
            else
            {
                MIROutport* port = static_cast<MIROutport*>(relation->srcObjs[j]);
                relation->srcStrs.push_back(((MIRActor*)port->parent)->name + "." + port->name);
            }
        }
        for(int j = 0; j < relation->dstObjs.size(); j++)
        {

            if(relation->dstObjs[j]->objType == MIRObject::TypeInport)
            {
                MIRInport* port = static_cast<MIRInport*>(relation->dstObjs[j]);
                relation->dstStrs.push_back(((MIRActor*)port->parent)->name + "." + port->name);
            }
            else
            {
                MIRInport* port = static_cast<MIRInport*>(relation->dstObjs[j]);
                relation->dstStrs.push_back(port->name);
            }
        }
    }
    return 1;
}

bool PTConverter::charIsWord(char c) const
{
    if(c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' || c >= '0' && c <= '9')
        return true;
    return false;
}

int PTConverter::mapActorsTraverseActorHandleExpression(MIRActor* actor)
{
    if(actor->parametersOfXML.find("Expression") == actor->parametersOfXML.end())
        return 1;
    string expStr = actor->parametersOfXML.at("Expression");
    for(int i = 0; i < actor->inports.size(); i++)
    {
        MIRInport* inport = actor->inports[i];
        string portName = inport->name;
        int portNameLen = portName.length();
        portName[0] = portName[0] + 32;
        size_t pos = expStr.find(portName, 0);
        while(pos != string::npos)
        {
            if((pos == 0 || !charIsWord(expStr[pos - 1])) &&
                (pos + portNameLen >= expStr.length() || !charIsWord(expStr[pos + portNameLen])))
            {
                expStr[pos] = expStr[pos] - 32;
            }
            pos = expStr.find(portName, pos);
        }
    }
    actor->parametersOfXML["Expression"] = expStr;
    return 1;
}
