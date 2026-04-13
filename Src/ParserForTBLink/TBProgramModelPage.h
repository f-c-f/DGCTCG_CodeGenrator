
#pragma once
#include <string>
#include <vector>
#include "TBObject.h"

class TBActor;
class TBRelation;
class TBLinkNode;

class TBProgramModelPage :
    public TBObject
{
public:
	std::string name;

	std::vector<TBActor*> actors;
	std::vector<TBRelation*> relations;
	std::vector<TBLinkNode*> linkNodes;
    std::vector<TBProperty*> properties;

};
