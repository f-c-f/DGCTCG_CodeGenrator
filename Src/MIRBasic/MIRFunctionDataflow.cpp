#include "MIRFunctionDataflow.h"
#include "MIRActor.h"
#include "MIRRelation.h"
#include "MIRInport.h"
#include "MIROutport.h"
#include <set>
using namespace std;

vector<MIRRelation*> MIRFunctionDataflow::getRelationByPort(MIRObject* portObj)
{
	vector<MIRRelation*> res;
    for(int i = 0;i < relations.size(); i++)
	{
        MIRRelation* relation = relations[i];
		for(int j = 0; j < relation->srcObjs.size(); j++)
		{
		    if(relation->srcObjs[j] == portObj)
                res.push_back(relation);
		}
        for(int j = 0; j < relation->dstObjs.size(); j++)
		{
		    if(relation->dstObjs[j] == portObj)
				res.push_back(relation);
		}
	}
    return res;
}

MIRActor* MIRFunctionDataflow::getActorByName(std::string name)
{
	for (int i = 0; i < actors.size(); i++)
	{
		if (actors[i]->name == name)
			return actors[i];
	}
	return nullptr;
}

std::vector<MIROutport*> MIRFunctionDataflow::getLinkedOutportsByInport(MIRInport* inport)
{
    set<MIROutport*> resSet;
    for(int i = 0; i < this->relations.size(); i++)
    {
        MIRRelation* relation = this->relations[i];
        bool isContain = false;
        for(int j = 0; j < relation->dstObjs.size(); j++)
        {
            if(relation->dstObjs[j] == inport)
            {
                isContain = true;
                break;
            }
        }
        if(!isContain)
            continue;

        for(int j = 0; j < relation->srcObjs.size(); j++)
        {
            resSet.insert((MIROutport*)relation->srcObjs[j]);
        }
    }

    vector<MIROutport*> ret(resSet.begin(), resSet.end());
    return ret;
}

std::vector<MIRInport*> MIRFunctionDataflow::getLinkedInportsByOutport(MIROutport* outport)
{
    set<MIRInport*> resSet;
    for(int i = 0; i < this->relations.size(); i++)
    {
        MIRRelation* relation = this->relations[i];
        bool isContain = false;
        for(int j = 0; j < relation->srcObjs.size(); j++)
        {
            if(relation->srcObjs[j] == outport)
            {
                isContain = true;
                break;
            }
        }
        if(!isContain)
            continue;

        for(int j = 0; j < relation->dstObjs.size(); j++)
        {
            resSet.insert((MIRInport*)relation->dstObjs[j]);
        }
    }

    vector<MIRInport*> ret(resSet.begin(), resSet.end());
    return ret;
}

void MIRFunctionDataflow::deleteActor(MIRActor* actors)
{
    for(int i = 0; i < this->actors.size(); i++)
    {
        if(this->actors[i] == actors)
        {
            actors->release();
            delete actors;
            this->actors.erase(this->actors.begin() + i);
            break;
        }
    }
}

void MIRFunctionDataflow::deleteRelation(MIRRelation* relation)
{
    for(int i = 0; i < this->relations.size(); i++)
    {
        if(this->relations[i] == relation)
        {
            relation->release();
            delete relation;
            this->relations.erase(this->relations.begin() + i);
            break;
        }
    }
}

void MIRFunctionDataflow::release()
{
    MIRFunction::release();
	for(int i = 0;i < actors.size(); i++)
	{
		actors[i]->release();
		delete actors[i];
	}
	actors.clear();
	for(int i = 0;i < relations.size(); i++)
	{
		relations[i]->release();
		delete relations[i];
	}
	relations.clear();
}
