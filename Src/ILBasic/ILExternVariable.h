#pragma once
#include <string>
#include <vector>
#include "ILObject.h"


class Expression;
class ILRef;
class ILExternVariable:
	public ILObject
{
public:
    ILExternVariable();
    explicit ILExternVariable(const ILObject* parent);
    ~ILExternVariable() override;

	//属性
	std::string name;
	std::string type;
	int isAddress = 0;
	std::vector<int> arraySize;
	//Expression* defaultValue = nullptr;
    //bool isStatic = false;
    int align = 0;
    bool isVolatile = false;
    bool isConst = false;

	//子节点
	std::vector<ILRef*> refs;

	void release() override;
};

