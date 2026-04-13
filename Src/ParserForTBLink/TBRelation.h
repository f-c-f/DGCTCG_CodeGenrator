#pragma once
#include <string>
#include "TBObject.h"

class TBRelation :
    public TBObject
{
public:
	std::string relationType = "TypeData"; // TypeControl
	int startLinkInterface = 0;
	int endLinkInterface = 0;

};
