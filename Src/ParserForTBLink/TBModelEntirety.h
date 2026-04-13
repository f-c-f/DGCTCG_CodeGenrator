#pragma once
#include <string>
#include <vector>
#include "TBObject.h"

class TBProgramModelPage;
class TBModelEntirety :
    public TBObject
{
public:
	std::string modelName;

	std::vector<TBProgramModelPage*> programModelPage;
};
