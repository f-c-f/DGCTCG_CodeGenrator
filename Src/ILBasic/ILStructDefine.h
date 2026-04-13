#pragma once
#include <string>
#include <vector>
#include "ILObject.h"


class ILMember;
class ILRef;
class ILStructDefine:
	public ILObject
{
public:
    ILStructDefine();
    explicit ILStructDefine(const ILObject* parent);
    ~ILStructDefine() override;

	//属性
	std::string name;

	//子节点
	std::vector<ILRef*> refs;
	std::vector<ILMember*> members;
    
	ILMember* getMemberByName(std::string name);
	ILMember* getMemberByIndex(int index);

	void release() override;
};

