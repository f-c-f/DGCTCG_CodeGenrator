#pragma once
#include <string>
#include "TBObject.h"


class TBLinkInterface :
    public TBObject
{
public:
	std::string linkRelationType = "TypeDataRelation"; // TypeControlRealtion
};
