#pragma once
#include <string>
#include <vector>
#include "ILObject.h"


class ILExternFile:
	public ILObject
{
public:
    ILExternFile();
    explicit ILExternFile(const ILObject* parent);
    ~ILExternFile() override;

	// Ù–‘
	std::string name;
    

	void release() override;
};

