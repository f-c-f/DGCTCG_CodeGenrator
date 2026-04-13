#include "TBConverter.h"
#include "TBLinkInterface.h"
#include "TBBranch.h"
#include <set>
#include <map>

using namespace std;

int TBConverter::convert(ParserForTBLink* parserForTBLink, MIRModel* mIRModel)
{
	int res;
	
	this->parserForTBLink = parserForTBLink;
	this->mIRModel = mIRModel;

	res = convert(mIRModel);
	if(res < 0)
		return res;

	return 1;
}

int TBConverter::convert(MIRModel* mIRModel)
{
	int res;
	mIRModel->release();
	mIRModel->objType = MIRObject::TypeModel;
    mIRModel->mainFunction = "TBLinkModel";
    mIRModel->modelSrcType = "TBLink";

    //cout << "TBConverter::convert" << endl;

	// 遍历所有程序模型页面
	for(int i = 0; i < parserForTBLink->rootEntity->programModelPage.size(); i++)
	{
		res = convertProgramModelPage(parserForTBLink->rootEntity->programModelPage[i], mIRModel);
		if(res < 0)
			return res;
	}

    res = mapActors();
	if(res < 0)
		return res;
    

	return 1;
}


// 创建LinkInterface ID到端口的映射表
map<long long, MIRObject*> linkInterfaceMap;

MIRFunctionDataflow* currentFunction = nullptr;

int TBConverter::convertProgramModelPage(TBProgramModelPage* programModelPage, MIRModel* mIRModel)
{
	int res;
	MIRFunctionDataflow* mIRFunction = new MIRFunctionDataflow();
    mIRFunction->type = MIRFunction::TypeDataflow;
	mIRFunction->objType = MIRObject::TypeFunction;
	mIRFunction->name = programModelPage->name;

    currentFunction = mIRFunction;

    if(mIRFunction->name == "ProgramPage1")
        mIRModel->mainFunction = "ProgramPage1";

	mIRModel->functions.push_back(mIRFunction);
	
    
	calculateLinkNodeRelatedAllLinkInterfaces(programModelPage);


	// 转换所有Actor
	for(int i = 0; i < programModelPage->actors.size(); i++)
	{
        // cout << "programModelPage->actors[i]->type: " << programModelPage->actors[i]->type << endl;

        if (programModelPage->actors[i]->type == "Inport")
        {
            MIRInport* mIRInport = new MIRInport();
            mIRInport->objType = MIRObject::TypeInport;
            mIRInport->parent = mIRFunction;
            mIRInport->name = programModelPage->actors[i]->name;
            mIRInport->type = programModelPage->actors[i]->outports[0]->dataType;
            if (mIRInport->type == "default")
                mIRInport->type = "double";
            mIRInport->arraySize = vector<int>();
            mIRFunction->inports.push_back(mIRInport);

            for(int j = 0; j < programModelPage->actors[i]->outports[0]->linkInterfaces.size(); j++)
            {
                TBLinkInterface* linkInterface = programModelPage->actors[i]->outports[0]->linkInterfaces[j];
                linkInterfaceMap[linkInterface->id] = mIRInport;
            }
        }
        else if (programModelPage->actors[i]->type == "Outport")
        {
            MIROutport* mIROutport = new MIROutport();
            mIROutport->objType = MIRObject::TypeOutport;
            mIROutport->parent = mIRFunction;
            mIROutport->name = programModelPage->actors[i]->name;
            mIROutport->type = programModelPage->actors[i]->inports[0]->dataType;
            if (mIROutport->type == "default")
                mIROutport->type = "double";
            mIROutport->arraySize = vector<int>();
            mIRFunction->outports.push_back(mIROutport);
            
            for(int j = 0; j < programModelPage->actors[i]->inports[0]->linkInterfaces.size(); j++)
            {
                TBLinkInterface* linkInterface = programModelPage->actors[i]->inports[0]->linkInterfaces[j];
                linkInterfaceMap[linkInterface->id] = mIROutport;
            }
        }
        else
        {
            res = convertActor(programModelPage->actors[i], mIRFunction);
            if(res < 0)
                return res;
        }
	}
	
    for(int i = 0; i < programModelPage->properties.size(); i++)
	{
		res = convertProgramModelPageProperty(programModelPage->properties[i], mIRFunction);
		if(res < 0)
			return res;
	}
    
	res = convertParameter(programModelPage, mIRFunction);
	if(res < 0)
		return res;

    res = convertRelation(programModelPage, mIRFunction);
	if(res < 0)
		return res;
    

	return 1;
}

int TBConverter::convertProgramModelPageProperty(TBProperty* property, MIRFunction* mIRFunction)
{
    // 将TBProperty转换为MIRFunction的参数
    // 处理参数名称
    string paramName;
    int res = trimNameStr(property->name, paramName);
    if(res < 0)
        return res;
    if(paramName[0] >= 'a' && paramName[0] <= 'z')
        paramName[0] = paramName[0] - 32;
    
    // 将参数添加到MIRFunction的parametersOfXML中
    mIRFunction->parametersOfXML[paramName] = property->value;
    
    return 1;
}


int TBConverter::convertActor(TBActor* actor, MIRFunctionDataflow* mIRFunction)
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

	
	for(int i = 0; i < actor->inports.size(); i++)
	{
		res = convertActorInport(actor->inports[i], mIRActor);
		if(res < 0)
			return res;
	}
	for(int i = 0; i < actor->outports.size(); i++)
	{
		res = convertActorOutport(actor->outports[i], mIRActor);
		if(res < 0)
			return res;
	}
    for(int i = 0; i < actor->branches.size(); i++)
    {
        res = convertActorOutport(actor->branches[i], mIRActor);
        if(res < 0)
            return res;
    }

    for(int i = 0; i < actor->propertys.size(); i++)
    {
        res = convertActorProperty(actor->propertys[i], mIRActor);
        if(res < 0)
            return res;
    }

    if(actor->linkInterfaces.size() > 0)
    {
        MIRActionPort* actionPort = new MIRActionPort();
        actionPort->objType = MIRObject::TypeActionPort;
        actionPort->parent = mIRActor;
        actionPort->name = "ActionPort";
        actionPort->type = "IfAction";
        mIRActor->actionPorts.push_back(actionPort);

        for(int i = 0; i < actor->linkInterfaces.size(); i++)
        {
            TBLinkInterface* linkInterface = actor->linkInterfaces[i];
            linkInterfaceMap[linkInterface->id] = actionPort;
        }
    }

	res = convertParameter(actor, mIRActor);
	if(res < 0)
		return res;

	return 1;
}

int TBConverter::convertRelation(TBProgramModelPage* programModelPage, MIRFunctionDataflow* mIRFunction)
{
    // 使用linkInterfaceColor来创建MIRRelation
    // 首先按连通域分组LinkInterface
    map<long long, vector<long long>> colorGroups;
    map<long long, long long>::iterator colorIter;
    
    for(colorIter = linkInterfaceColor.begin(); colorIter != linkInterfaceColor.end(); colorIter++)
    {
        long long linkInterfaceId = colorIter->first;
        long long color = colorIter->second;
        colorGroups[color].push_back(linkInterfaceId);
    }
    
    // 为每个连通域创建MIRRelation
    map<long long, vector<long long>>::iterator groupIter;
    for(groupIter = colorGroups.begin(); groupIter != colorGroups.end(); groupIter++)
    {
        vector<long long>& linkInterfaceIds = groupIter->second;
        
        // 分离源和目标
        vector<MIRObject*> srcObjs;
        vector<MIRObject*> dstObjs;
        vector<string> srcStrs;
        vector<string> dstStrs;
        
        for(size_t i = 0; i < linkInterfaceIds.size(); i++)
        {
            long long linkInterfaceId = linkInterfaceIds[i];
            auto mapIter = linkInterfaceMap.find(linkInterfaceId);
            
            if(mapIter != linkInterfaceMap.end())
            {
                MIRObject* obj = mapIter->second;
                
                // Function的Inport可以作为源
                if(obj->objType == MIRObject::TypeInport && obj->parent->objType == MIRObject::TypeFunction)
                {
                    srcObjs.push_back(obj);
                    MIRInport* inport = static_cast<MIRInport*>(obj);
                    srcStrs.push_back(inport->name);
                }
                // Function的Outport可以作为源
                else if(obj->objType == MIRObject::TypeOutport && obj->parent->objType == MIRObject::TypeFunction)
                {
                    dstObjs.push_back(obj);
                    MIROutport* outport = static_cast<MIROutport*>(obj);
                    dstStrs.push_back(outport->name);
                }
                // MIROutport可以作为源
                else if(obj->objType == MIRObject::TypeOutport)
                {
                    srcObjs.push_back(obj);
                    MIROutport* outport = static_cast<MIROutport*>(obj);
                    MIRActor* actor = static_cast<MIRActor*>(outport->parent);
                    srcStrs.push_back(actor->name + "." + outport->name);
                }
                // MIRActionPort和MIRInport可以作为目标
                else if(obj->objType == MIRObject::TypeActionPort || obj->objType == MIRObject::TypeInport)
                {
                    dstObjs.push_back(obj);
                    
                    if(obj->objType == MIRObject::TypeActionPort)
                    {
                        MIRActionPort* actionPort = static_cast<MIRActionPort*>(obj);
                        MIRActor* actor = static_cast<MIRActor*>(actionPort->parent);
                        dstStrs.push_back(actor->name + "." + actionPort->name);
                    }
                    else if(obj->objType == MIRObject::TypeInport)
                    {
                        MIRInport* inport = static_cast<MIRInport*>(obj);
                        MIRActor* actor = static_cast<MIRActor*>(inport->parent);
                        dstStrs.push_back(actor->name + "." + inport->name);
                    }
                }
            }
        }
        
        // 只有当源和目标都存在时才创建MIRRelation
        if(!srcObjs.empty() && !dstObjs.empty())
        {
            MIRRelation* relation = new MIRRelation();
            relation->objType = MIRObject::TypeRelation;
            relation->srcObjs = srcObjs;
            relation->dstObjs = dstObjs;
            relation->srcStrs = srcStrs;
            relation->dstStrs = dstStrs;
            mIRFunction->relations.push_back(relation);
        }
    }
    
    return 1;
}

int inportValueConstActorId = 0;

int TBConverter::convertActorInport(TBInport* inport, MIRActor* mIRActor)
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
	mIRInport->type = inport->dataType;
    if (mIRInport->type == "default")
        mIRInport->type = "double";
	mIRInport->arraySize = inport->arraySize;

	mIRActor->inports.push_back(mIRInport);
	
	res = convertParameter(inport, mIRInport);
	if(res < 0)
		return res;

    if (!inport->value.empty())
    {
        inportValueConstActorId++;
        MIRActor* newConstActor = new MIRActor();
        newConstActor->objType = MIRObject::TypeActor;
        newConstActor->name = "InportValueConst" + to_string(inportValueConstActorId);
        newConstActor->type = "Constant";
        newConstActor->parent = currentFunction;
        currentFunction->actors.push_back(newConstActor);

        MIROutport* newConstOutport = new MIROutport();
        newConstOutport->objType = MIRObject::TypeOutport;
        newConstOutport->parent = newConstActor;
        newConstOutport->name = "Out";
        newConstOutport->type = "double";
        newConstOutport->arraySize = vector<int>();
        newConstActor->outports.push_back(newConstOutport);

        MIRRelation* relation = new MIRRelation();
        relation->objType = MIRObject::TypeRelation;
        relation->srcObjs.push_back(newConstOutport);
        relation->dstObjs.push_back(mIRInport);
        relation->srcStrs.push_back(newConstActor->name + "." + newConstOutport->name);
        relation->dstStrs.push_back(mIRActor->name + "." + mIRInport->name);
        currentFunction->relations.push_back(relation);
    }

    for(int i = 0; i < inport->linkInterfaces.size(); i++)
    {
        TBLinkInterface* linkInterface = inport->linkInterfaces[i];
        linkInterfaceMap[linkInterface->id] = mIRInport;
    }

	return 1;
}

int TBConverter::convertActorOutport(TBOutport* outport, MIRActor* mIRActor)
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
	mIROutport->type = outport->dataType;
    if (mIROutport->type == "default")
        mIROutport->type = "double";
	mIROutport->arraySize = outport->arraySize;

	mIRActor->outports.push_back(mIROutport);
	
	res = convertParameter(outport, mIROutport);
	if(res < 0)
		return res;

    for(int i = 0; i < outport->linkInterfaces.size(); i++)
    {
        TBLinkInterface* linkInterface = outport->linkInterfaces[i];
        linkInterfaceMap[linkInterface->id] = mIROutport;
    }

	return 1;
}


int TBConverter::convertActorOutport(TBBranch* branch, MIRActor* mIRActor)
{
	int res;
	MIROutport* mIROutport = new MIROutport();
	mIROutport->objType = MIRObject::TypeOutport;
    mIROutport->parent = mIRActor;
    string outportName = "Branch" + to_string(branch->branchId);
    
	mIROutport->name = outportName;
	mIROutport->type = "double";
	mIROutport->arraySize = vector<int>();

	mIRActor->outports.push_back(mIROutport);
	
    for(int i = 0; i < branch->linkInterfaces.size(); i++)
    {
        TBLinkInterface* linkInterface = branch->linkInterfaces[i];
        linkInterfaceMap[linkInterface->id] = mIROutport;
    }

	return 1;
}

int TBConverter::convertActorProperty(TBProperty* property, MIRActor* mIRActor)
{
    // 处理参数名称
    string paramName;
    int res = trimNameStr(property->name, paramName);
    if(res < 0)
        return res;
    if(paramName[0] >= 'a' && paramName[0] <= 'z')
        paramName[0] = paramName[0] - 32;
    
    
    // 同时添加到parametersOfXML中保持兼容性
    mIRActor->parametersOfXML[paramName] = property->value;
    
    return 1;
}


int TBConverter::convertParameter(TBObject* object, MIRObject* mIRObject)
{
	map<string, string>::iterator iter = object->parameters.begin();
	for(;iter!=object->parameters.end();iter++)
	{
        string name = iter->first;
        if(name[0] >= 'a' && name[0] <= 'z')
            name[0] = name[0] - 32;

        if (name == "X" || name == "Y" || name == "Height" || name == "Width" ||
            name == "LinkDirection" || name == "LinkType" || name == "LinkRelationType")
            continue;
		mIRObject->parametersOfXML[name] = iter->second;
	}
	return 1;
}

int TBConverter::trimNameStr(string str, string& ret)
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

int TBConverter::trimTypeStr(std::string str, std::string& ret)
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

// 并查集查找根节点函数
long long TBConverter::findRoot(map<long long, long long>& parent, long long x)
{
    if(parent[x] != x)
    {
        parent[x] = findRoot(parent, parent[x]); // 路径压缩
    }
    return parent[x];
}

// 并查集合并函数
void TBConverter::unionSets(map<long long, long long>& parent, map<long long, long long>& rank, long long x, long long y)
{
    long long rootX = findRoot(parent, x);
    long long rootY = findRoot(parent, y);
    
    if(rootX != rootY)
    {
        if(rank[rootX] < rank[rootY])
        {
            parent[rootX] = rootY;
        }
        else if(rank[rootX] > rank[rootY])
        {
            parent[rootY] = rootX;
        }
        else
        {
            parent[rootY] = rootX;
            rank[rootX]++;
        }
    }
}

void TBConverter::calculateLinkNodeRelatedAllLinkInterfaces(TBProgramModelPage* page)
{
    linkInterfaceColor.clear();
    
    // 第一步：收集所有LinkInterface的ID
    set<long long> allLinkInterfaceIds;
    
    // 从Actor的端口收集
    for(int j = 0; j < page->actors.size(); j++)
    {
        TBActor* actor = page->actors[j];
        
        // Actor的LinkInterface
        for(int k = 0; k < actor->linkInterfaces.size(); k++)
        {
            allLinkInterfaceIds.insert(actor->linkInterfaces[k]->id);
        }
        
        // 端口的LinkInterface
        for(int k = 0; k < actor->inports.size(); k++)
        {
            for(int l = 0; l < actor->inports[k]->linkInterfaces.size(); l++)
            {
                allLinkInterfaceIds.insert(actor->inports[k]->linkInterfaces[l]->id);
            }
        }
        
        for(int k = 0; k < actor->outports.size(); k++)
        {
            for(int l = 0; l < actor->outports[k]->linkInterfaces.size(); l++)
            {
                allLinkInterfaceIds.insert(actor->outports[k]->linkInterfaces[l]->id);
            }
        }
        
        // Branch的LinkInterface
        for(int k = 0; k < actor->branches.size(); k++)
        {
            for(int l = 0; l < actor->branches[k]->linkInterfaces.size(); l++)
            {
                allLinkInterfaceIds.insert(actor->branches[k]->linkInterfaces[l]->id);
            }
        }
    }
    
    // 从LinkNode收集LinkInterface ID
    for(int j = 0; j < page->linkNodes.size(); j++)
    {
        TBLinkNode* linkNode = page->linkNodes[j];
        for(int k = 0; k < linkNode->linkInterfaces.size(); k++)
        {
            allLinkInterfaceIds.insert(linkNode->linkInterfaces[k]->id);
        }
    }
    
    
    // 第二步：构建并查集来找到连通域
    map<long long, long long> parent; // 并查集的父节点映射
    map<long long, long long> rank;   // 并查集的秩
    
    // 初始化并查集
    set<long long>::iterator iter;
    for(iter = allLinkInterfaceIds.begin(); iter != allLinkInterfaceIds.end(); iter++)
    {
        long long id = *iter;
        parent[id] = id;
        rank[id] = 0;
    }
    
    // 第三步：根据Relation建立连接关系
    for(int j = 0; j < page->relations.size(); j++)
    {
        TBRelation* relation = page->relations[j];
        // 将Relation连接的两个LinkInterface合并到同一个连通域
        unionSets(parent, rank, relation->startLinkInterface, relation->endLinkInterface);
    }
    
    // 第四步：根据LinkNode建立连接关系
    for(int j = 0; j < page->linkNodes.size(); j++)
    {
        TBLinkNode* linkNode = page->linkNodes[j];
        
        // LinkNode中的所有LinkInterface都属于同一个连通域
        if(linkNode->linkInterfaces.size() > 1)
        {
            long long firstId = linkNode->linkInterfaces[0]->id;
            for(int k = 1; k < linkNode->linkInterfaces.size(); k++)
            {
                unionSets(parent, rank, firstId, linkNode->linkInterfaces[k]->id);
            }
        }
    }
    
    // 第五步：为每个LinkInterface分配颜色（连通域标识）
    for(iter = allLinkInterfaceIds.begin(); iter != allLinkInterfaceIds.end(); iter++)
    {
        long long id = *iter;
        long long rootId = findRoot(parent, id);
        linkInterfaceColor[id] = rootId; // 使用根节点的ID作为颜色标识
    }
}


TBConverter::MapActor* TBConverter::findMapActor(std::string name)
{
    for(int i = 0; i < mapActorList.size(); i++)
    {
        if(mapActorList[i].name == name)
            return &mapActorList[i];
    }
    return nullptr;
}

TBConverter::MapParameter* TBConverter::findMapParameter(MapActor* mapActor, std::string name)
{
    for(int i = 0; i < mapActor->mapParameters.size(); i++)
    {
        if(mapActor->mapParameters[i].name == name)
            return &mapActor->mapParameters[i];
    }
    return nullptr;
}

TBConverter::MapInport* TBConverter::findMapInport(MapActor* mapActor, std::string name)
{
    for(int i = 0; i < mapActor->mapInports.size(); i++)
    {
        if(mapActor->mapInports[i].name == name)
            return &mapActor->mapInports[i];
    }
    return nullptr;
}

TBConverter::MapOutport* TBConverter::findMapOutport(MapActor* mapActor, std::string name)
{
    for(int i = 0; i < mapActor->mapOutports.size(); i++)
    {
        if(mapActor->mapOutports[i].name == name)
            return &mapActor->mapOutports[i];
    }
    return nullptr;
}

int TBConverter::mapActors()
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

int TBConverter::loadActorMapRule()
{
    int res;
    string exePath = getExeDirPath();
	tinyxml2::XMLDocument doc;
    string filePath = exePath + "/MapRules.xml";
	tinyxml2::XMLError ret = doc.LoadFile(filePath.c_str());
	if (ret != 0) {
		// 如果文件不存在，返回成功（可选）
		return 1;
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
            mapParameter.isNew = false;
            if(para->FindAttribute("New"))
                mapParameter.isNew = para->Attribute("New");

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

int TBConverter::mapActorsTraverseFunction(MIRModel* model)
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

int TBConverter::mapActorsTraverseActor(MIRFunctionDataflow* function)
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
        
        for(int i = 0; i < mapActor->mapParameters.size(); i++)
        {
            if(mapActor->mapParameters[i].isNew)
            {
                MIRParameter* parameter = new MIRParameter();
                parameter->objType = MIRObject::TypeParameter;
                parameter->parent = actor;
                parameter->name = mapActor->mapParameters[i].name;
                parameter->value = mapActor->mapParameters[i].value;
                actor->parameters.push_back(parameter);
            }
        }
    }
    return 1;
}

int TBConverter::mapActorsTraverseParameter(MIRActor* actor, MapActor* mapActor)
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

int TBConverter::mapActorsTraverseInport(MIRActor* actor, MapActor* mapActor)
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

int TBConverter::mapActorsTraverseOutport(MIRActor* actor, MapActor* mapActor)
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

int TBConverter::mapActorsUpdateLinkPort(MIRFunctionDataflow* function)
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
            else if(relation->dstObjs[j]->objType == MIRObject::TypeActionPort)
            {
                MIRActionPort* port = static_cast<MIRActionPort*>(relation->dstObjs[j]);
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

bool TBConverter::charIsWord(char c) const
{
    if(c >= 'A' && c <= 'Z' || c >= 'a' && c <= 'z' || c >= '0' && c <= '9')
        return true;
    return false;
}

int TBConverter::mapActorsTraverseActorHandleExpression(MIRActor* actor)
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
