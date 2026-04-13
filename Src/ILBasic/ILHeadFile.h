#pragma once
#include <string>
#include <vector>
#include "ILObject.h"


class ILRef;
class ILHeadFile:
	public ILObject
{
public:
    ILHeadFile();
    explicit ILHeadFile(const ILObject* parent);
    ~ILHeadFile() override;

	//属性
	std::string name;
	
	//子节点
	std::vector<ILRef*> refs;

	void release() override;
};

