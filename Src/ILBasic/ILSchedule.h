#pragma once
#include <vector>
#include <string>
#include "ILObject.h"
#include <unordered_map>

class ILScheduleNode;
class ILLocalVariable;
class ILCalculate;
class ILSchedule:
	public ILObject
{
public:
    ILSchedule();
    explicit ILSchedule(const ILObject* parent);
    ~ILSchedule() override;

	std::vector<ILLocalVariable*> localVariables;
	std::vector<ILScheduleNode*> scheduleNodes;

	

	void release() override;

};

