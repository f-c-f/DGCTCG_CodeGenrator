#pragma once
#include <string>
#include <vector>
#include "ILObject.h"


class ILRef;
class ILMacro:
	public ILObject
{
public:
    ILMacro();
    explicit ILMacro(const ILObject* parent);
    ~ILMacro() override;

	//属性
	std::string name;
	std::string value;
	std::string hasIfnodefCheck;
	
	//子节点
	std::vector<ILRef*> refs;

	void release() override;
};

