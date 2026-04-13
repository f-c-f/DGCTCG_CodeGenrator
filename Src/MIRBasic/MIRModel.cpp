#include "MIRModel.h"
#include "MIRFunction.h"

MIRFunction* MIRModel::getFunctionByName(std::string name)
{
    for(int i = 0;i < functions.size(); i++)
	{
		if(functions[i]->name == name)
            return functions[i];
	}
    return nullptr;
}

void MIRModel::release()
{
    for(int i = 0;i < functions.size(); i++)
	{
		functions[i]->release();
        delete functions[i];
	}
    functions.clear();
}
