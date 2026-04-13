#pragma once

class ILCalculate;
class ILFunction;
class ILTransFile;
class Statement;
class Expression;
class ILStructDefine;
class ILAnalyzeSchedule;

class ILTransCalculate
{
public:
    int translate(const ILCalculate* calculate, ILTransFile* file, const ILAnalyzeSchedule* iLTranslateSchedule) const;
};

