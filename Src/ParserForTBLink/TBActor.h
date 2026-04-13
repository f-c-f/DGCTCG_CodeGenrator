#pragma once
#include <string>
#include <vector>
#include "TBObject.h"

class TBInport;
class TBOutport;
class TBProperty;
class TBBranch;
class TBLinkInterface;

class TBActor :
    public TBObject
{
public:
	std::string type;//type of actor		"AbsoluteValue"  ¡¢  "AddSubtract"
	std::string name;//full name			"JsfDeployWg_JsfCreateJobset_BooleanSwitch3"


	std::vector<TBInport*> inports;
	std::vector<TBOutport*> outports;

	std::vector<TBBranch*> branches;
	std::vector<TBLinkInterface*> linkInterfaces;

	std::vector<TBProperty*> propertys;


};
