#pragma once
#include <vector>
#include <string>
#include <unordered_map>
class ILCalculate;
class ILRef;
class ILSchedule;
class ILScheduleNode;

class ILTransSubsystemUtility
{
public:
    /*ILTransSubsystemUtility(ILCalculate* calculate) : subsystem(calculate) {
    };*/
    ILTransSubsystemUtility(const ILSchedule* schedule) : schedule(schedule)
    {};

    int findSubsystemInSchedule();
    int findSubsystemInSchedule(ILScheduleNode* const scheduleNode);
    bool isScheduleExist() const;
    bool resetSchedule(ILSchedule* const schedule);
    bool isSubsystemOutput(std::string name) const;
    std::string getSubsystemNameByOutputName(std::string name) const;
    std::vector<std::tuple<const ILCalculate*, const ILRef*, std::string>>::iterator findCalculateInSubsystems(const ILCalculate* calculate);
    std::vector<std::tuple<const ILCalculate*, const ILRef*, std::string>>::iterator getCalculateInSubsystemsByName(std::string name);
    // first: subsystem对应的calculate
    // second: calculate内部的ref，用于找到对应的Function
    // third: 存储变量的self结构体
    std::vector<std::tuple<const ILCalculate*, const ILRef*, std::string>> subsystems;
    // key: subsystem 
    // value: output name for local variables
    std::unordered_map<std::string, std::vector<std::string>> outputNameforLocalVariables;
private:
    const ILSchedule* schedule = nullptr;
    bool isSubsystemInSchedule(const ILScheduleNode* scheduleNode) const;
};
