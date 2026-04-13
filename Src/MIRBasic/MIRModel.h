#pragma once

#include <string>
#include <vector>

#include "MIRObject.h"




class MIRActor;
class MIRRelation;
class MIRInport;
class MIROutport;
class MIRFunction;
class MIRModel:
	public MIRObject
{
public:
    std::string name;
    std::string mainFunction;
    std::string modelSrcType; //表示原始不同建模工具的模型类型如Simulink、Ptolemy、TBLink

	std::vector<MIRFunction*> functions;
    
    MIRFunction* getFunctionByName(std::string name);

	void release();
};
