#pragma once
#include <string>
#include <vector>
#include "ILObject.h"


class ILRef;
class ILUtilityFunction:
	public ILObject
{
public:
    ILUtilityFunction();
    explicit ILUtilityFunction(const ILObject* parent);
    ~ILUtilityFunction() override;

	//属性
	std::string name;

	//子节点
	std::vector<ILRef*> refs;

	void release() override;
};

