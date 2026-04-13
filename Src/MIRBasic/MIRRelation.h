#pragma once

#include <string>
#include <vector>

#include "MIRObject.h"


class MIROutport;

class MIRRelation :
	public MIRObject
{
public:
	std::vector<std::string> srcStrs;
	std::vector<std::string> dstStrs;

	// srcObj是MIRActor的MIROutport、也可以是MIRFunction的MIRInport
    // dstObj可以是MIRActor的MIRInport、MIRActor的MIRActionPort、MIRFunction的MIROutport
	std::vector<MIRObject*> srcObjs;
	std::vector<MIRObject*> dstObjs;

    void deleteSrc(MIRObject*);
    void deleteDst(MIRObject*);

	void release();
};
