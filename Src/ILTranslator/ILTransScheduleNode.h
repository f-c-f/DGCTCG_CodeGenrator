#pragma once
class ILScheduleNode;
class ILTransFile;
class ILAnalyzeSchedule;

class ILTransScheduleNode
{
public:
    int translate(const ILScheduleNode* scheduleNode, ILTransFile* file, const ILAnalyzeSchedule* iLTranslateSchedule) const;
};
