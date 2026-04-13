#pragma once
#include <string>
#include "TBObject.h"

class TBLinkInterface;
class TBLinkNode :
    public TBObject
{
public:
	std::string linkNodeType = "TypeDataFlow"; // TypeControlFlow
	
	std::vector<TBLinkInterface*> linkInterfaces;

};
