#pragma once
#include <string>
#include "ILObject.h"

class ILConfigParameter
{
public:
    std::string name;
    std::string type;
    int isAddress = 0;
    std::vector<int> arraySize;
    std::string defaultValue;
};

class ILConfig:
	public ILObject
{
public:

	int iterationCount = -1;
	std::string mainInitFunction;
	std::string mainExecFunction;
	std::string mainCompositeState;

    ILConfig();
    ILConfig(const ILObject* parent);
    ~ILConfig();

    std::vector<ILConfigParameter> parameters;

};
