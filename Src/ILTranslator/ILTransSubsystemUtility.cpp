#include "ILTransSubsystemUtility.h"

#include "ILTranslator.h"
#include "../ILBasic/ILCalculate.h"
#include "../ILBasic/ILRef.h"
#include "../ILBasic/ILSchedule.h"
#include "../ILBasic/ILScheduleNode.h"
#include "../ILBasic/ILOutput.h"
#include "../ILBasic/ILBranch.h"
#include "ILTranslatorForC.h"

bool ILTransSubsystemUtility::isScheduleExist() const
{
	return this->schedule != nullptr;
}

bool ILTransSubsystemUtility::resetSchedule(ILSchedule* const schedule)
{
	if (schedule == nullptr)
		return false;
	// 清空两个容器里面的内容
	this->subsystems.clear();
	this->outputNameforLocalVariables.clear();
	this->schedule = schedule;
	int res = findSubsystemInSchedule();
	if (res < 0)
		return false;
	return true;
}

bool ILTransSubsystemUtility::isSubsystemInSchedule(const ILScheduleNode* scheduleNode) const
{
	if (scheduleNode->objType == ILObject::TypeCalculate &&
		static_cast<const ILCalculate*>(scheduleNode)->getRefActorType() == "Function")
		return true;
	return false;
}

int ILTransSubsystemUtility::findSubsystemInSchedule()
{
	int res;
	for (auto& scheduleNode : schedule->scheduleNodes)
	{
		res	= findSubsystemInSchedule(scheduleNode);
		if (res < 0)
			return res;
	}
	return 1;
}

int ILTransSubsystemUtility::findSubsystemInSchedule(ILScheduleNode* const scheduleNode)
{
	int res;
	if (scheduleNode->objType == ILObject::TypeBranch)
	{
		const ILBranch* branch = static_cast<const ILBranch*>(scheduleNode);
		for (auto& node : branch->scheduleNodes)
		{
			res = findSubsystemInSchedule(node);
			if (res < 0)
				return res;
		}
	}
	else if(scheduleNode->objType == ILObject::TypeCalculate) {
		// 处理subsystem
		if (isSubsystemInSchedule(scheduleNode))
		{
			const ILCalculate* calculate = static_cast<const ILCalculate*>(scheduleNode);
			bool isFoundRef = false;
			for (auto& ref : calculate->refs)
			{
				if (ref->actor == "CompositeActor")
				{
					std::string self = ref->path + "_" + "instance";
					this->subsystems.push_back({ calculate, ref, self });
					isFoundRef = true;
					break;
				}
			}
			if (!isFoundRef)
				return -ILTranslator::ErrorCanNotFindRefInSubsystemCalculate;
			for (auto& output : calculate->outputs)
			{
				this->outputNameforLocalVariables[calculate->name].push_back(calculate->name + "_" + output->name);
			}
		}
	}
	return 1;
}

std::vector<std::tuple<const ILCalculate*, const ILRef*, std::string>>::iterator ILTransSubsystemUtility::findCalculateInSubsystems(const ILCalculate* calculate)
{
	// 判断一个calculate是否为子系统
	// 用在打包参数
	for (std::vector<std::tuple<const ILCalculate*, const ILRef*, std::string>>::iterator it = subsystems.begin(); it != subsystems.end(); it++)
	{
		const ILCalculate* calculateInSubsystem = std::get<0>(*it);
		if (calculateInSubsystem->name == calculate->name &&
			calculateInSubsystem->refs[0]->actor == calculate->refs[0]->actor &&
			calculateInSubsystem->refs[0]->path == calculate->refs[0]->path
			)
		{
			return it;
		}
	}
	return subsystems.end();
}

std::vector<std::tuple<const ILCalculate*, const ILRef*, std::string>>::iterator ILTransSubsystemUtility::getCalculateInSubsystemsByName(std::string name)
{
	// 确定为子系统才可使用
	for (auto it = subsystems.begin(); it != subsystems.end(); it++)
	{
		if (name == std::get<0>(*it)->name)
			return it;
	}
	return subsystems.end();
}


bool ILTransSubsystemUtility::isSubsystemOutput(std::string name) const
{
	for (auto& subsystemOutput : outputNameforLocalVariables)
	{
		for (auto& outputName : subsystemOutput.second)
		{
			if (outputName == name)
				return true;
		}
	}
	return false;
}

std::string ILTransSubsystemUtility::getSubsystemNameByOutputName(std::string name) const
{
	for (auto& subsystemOutput : outputNameforLocalVariables)
	{
		for (auto& outputName : subsystemOutput.second)
		{
			if (outputName == name)
				return subsystemOutput.first;
		}
	}
	return "";
}
