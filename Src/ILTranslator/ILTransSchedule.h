#pragma once
#include <map>
#include <string>
#include <vector>
#include <unordered_map>

class ILSchedule;
class ILTransFile;
class ILCalculate;
class ILScheduleNode;
class ILRef;
class Statement;
class Expression;
class ILTransSubsystemUtility;
class ILStructDefine;
class ILFunction;


// 用于对子系统进行参数打包

class ILAnalyzeSchedule;

class ILTransSchedule
{
public:
    int translate(const ILSchedule* schedule, ILTransFile* file) const;
private:

    int modifyForPackage(ILCalculate* const calculate, ILTransSubsystemUtility* subsystemUtil, ILStructDefine* const structDefine) const;
    int processSubsystemForPackage(ILCalculate* const calculate, ILTransSubsystemUtility* subsystemUtil, ILStructDefine* const structDefine) const;
    // int updateSubsystemExpression(Expression* expression, std::string name) const;
    int packageCalculate(ILScheduleNode* const scheduleNode, ILTransSubsystemUtility* subsystemUtil, ILStructDefine* const structDefine) const;
    void updateExpressionForPackage(const ILFunction* function, ILTransSubsystemUtility* subsystemUtil, Expression* expression, ILStructDefine* structDefine) const;
    int processStatementForPackage(const ILFunction* function, ILTransSubsystemUtility* subsystemUtil, Statement* const statement, ILStructDefine* const structDefine) const;
};

