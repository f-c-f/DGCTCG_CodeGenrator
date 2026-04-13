#pragma once

class ILBranch;
class ILTransFile;
class ILAnalyzeSchedule;

class ILTransBranch
{
public:
    int translate(const ILBranch* branch, ILTransFile* file, const ILAnalyzeSchedule* iLTranslateSchedule) const;

    int translateForCBMC(const ILBranch* branch, ILTransFile* file) const;
    int translateForCoverage(const ILBranch* branch, ILTransFile* file) const;
    int translateForTCGStateSearch(const ILBranch* branch, ILTransFile* file) const;
    int translateForTCGHybrid(const ILBranch* branch, ILTransFile* file) const;

    int translateBasic(const ILBranch* branch, ILTransFile* file, const ILAnalyzeSchedule* iLTranslateSchedule) const;
};
