#pragma once
#include <map>
#include <string>
#include <vector>

class Expression;
class ILCalculate;
class Statement;
class ILObject;
class ILScheduleNode;
class ILOutput;
class ILInput;
class ILLocalVariable;
class ILGlobalVariable;

class ILAnalyzeScheduleVariableType
{
public:
    ILAnalyzeScheduleVariableType();
    ILAnalyzeScheduleVariableType(std::string name, std::string type, int isAddress, std::vector<int> arraySize);
    ~ILAnalyzeScheduleVariableType();

	std::string name;
	std::string type;
	int isAddress = 0;
	std::vector<int> arraySize;
};

class ILAnalyzeScheduleVariable{
public:
    enum VariableType
    {
        TypeInput,
        TypeOutput,
        TypeLocalVariable,
        TypeGlobalVariable,
        TypeStatementVariable,
        TypeUnknown,
    };

    VariableType variableType = TypeUnknown;
    
	std::string name;
	std::string type;
	int isAddress = 0;
	std::vector<int> arraySize;

    //Expression* expression = nullptr; // 表示实际引用的表达式

    // 表示该变量在代码中的调用名
    // 比如：如果该变量的父级变量是地址，则该变量的callName为aaa->bbb
    std::string callName;

    // 全局级为对应的ILFile
    // 函数级为对应的ILFunction
    // 分支级为对应的ILBranch 或 Statement
    std::vector<void*> domain; // 表示该变量的作用域，作用域嵌套表示

    ILAnalyzeScheduleVariable* parent = nullptr;

    //key: 索引号 或 成员名   value: 对应的成员
    std::map<std::string, ILAnalyzeScheduleVariable*> members;

    ILGlobalVariable* iLGlobalVariable = nullptr; // 对应 TypeGlobalVariable
    ILLocalVariable* iLLocalVariable = nullptr;   // 对应 TypeLocalVariable
    ILInput* iLInput = nullptr;                   // 对应 TypeInput
    ILOutput* iLOutput = nullptr;                 // 对应 TypeOutput
    ILCalculate* iLCalculate = nullptr;           // 对应 TypeStatementVariable
    Statement* statement = nullptr;               // 对应 TypeStatementVariable
    //Expression* expression = nullptr;             // 对应 TypeStatementVariable

    ILAnalyzeScheduleVariable* referedGlobalVariable = nullptr; // 表示该局部变量引用的全局变量

    std::string referedConstantValue; // 表示该局部变量引用的常量值

    ILAnalyzeScheduleVariable* getTopParent() const;

    int release();
};
