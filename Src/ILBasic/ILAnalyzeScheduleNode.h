#pragma once

#include <string>
#include <vector>

class ILCalculate;
class Expression;
class Statement;
class ILBranch;
class ILScheduleNode;
class ILOutput;
class ILInput;
class ILLocalVariable;
class ILAnalyzeScheduleVariable;
class ILAnalyzeScheduleNode;

class ILAnalyzeScheduleNodeVariableRef
{
public:

    ILAnalyzeScheduleNodeVariableRef();
    ILAnalyzeScheduleNodeVariableRef(const ILAnalyzeScheduleNode* node, const Expression* expression, const ILAnalyzeScheduleVariable* variable, const Expression* expressionFull);
    ~ILAnalyzeScheduleNodeVariableRef();


    const ILAnalyzeScheduleNode* node = nullptr;

    const Expression* expression = nullptr; // ????????????????
    const Expression* expressionFull = nullptr; // ???????

    const ILAnalyzeScheduleVariable* variable = nullptr; // ????????????array[i], (array+offset)?????array?????
    ILAnalyzeScheduleNodeVariableRef* clone() const;
};

class ILAnalyzeScheduleNode
{
public:
    int id = -1; // ????????

    enum ScheduleNodeType
    {
        TypeGlobalVariable,
        TypeInput,
        TypeOutput,
        TypeLocalVariable,
        TypeBranch,
        TypeStatement,
        TypeUnknown,
    };

    ScheduleNodeType nodeType = TypeUnknown;
    
    ILLocalVariable* iLLocalVariable = nullptr; // ??TypeLocalVariable
    ILBranch* iLBranch = nullptr; // ??TypeBranch
    ILCalculate* iLCalculate = nullptr; // ??TypeStatement
    Statement* statement = nullptr; // ??TypeStatement

    int depth = 0;
    ILAnalyzeScheduleNode* parent = nullptr;
    std::vector<ILAnalyzeScheduleNode*> children;
    ILAnalyzeScheduleNode* prevNode = nullptr;
    ILAnalyzeScheduleNode* nextNode = nullptr;


    // ????????????????
    // ??????????????????????
    // ?????????????????
    // ??????????
    std::vector<ILAnalyzeScheduleNodeVariableRef*> variableStored;
    std::vector<ILAnalyzeScheduleNodeVariableRef*> variableRead;

    std::vector<void*> domain; // ?????????????????

    bool isReadVariable(std::string variableName);
    bool isStoreVariable(std::string variableName);

    std::vector<Expression*> getNodeExpressions();

    void release();
};

