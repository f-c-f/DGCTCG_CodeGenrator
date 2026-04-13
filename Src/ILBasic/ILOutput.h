#pragma once
#include <string>
#include <vector>
#include "ILObject.h"


class Expression;
class ILRef;
class ILOutput:
	public ILObject
{
public:
    ILOutput();
    explicit ILOutput(const ILObject* parent);
    ~ILOutput() override;

	//属性
	std::string name;
	std::string type;
	int isAddress = 0;
	std::vector<int> arraySize;
	//std::string defaultValue;
    Expression* defaultValue = nullptr;
	std::string sourceStr;
	//ILOutput* source;
    bool isConst = false;
	
	//子节点
	std::vector<ILRef*> refs;

	ILOutput* clone(const ILObject* parent) const;
	void release() override;
};

