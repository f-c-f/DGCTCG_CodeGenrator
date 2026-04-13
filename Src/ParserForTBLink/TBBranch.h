#pragma once
#include <string>
#include <vector>
#include "TBObject.h"

class TBLinkInterface;

class TBBranch :
    public TBObject
{
public:
	int branchId;

	std::vector<TBLinkInterface*> linkInterfaces;
};
