#include "ILAnalyzeScheduleNode.h"

#include <algorithm>

#include "ILAnalyzeScheduleVariable.h"

#include "../ILBasic/ILBranch.h"
#include "../ILBasic/ILSchedule.h"
#include "../ILBasic/ILLocalVariable.h"
#include "../ILBasic/StmtLocalVariable.h"
#include "../ILBasic/StmtIf.h"
#include "../ILBasic/StmtElse.h"
#include "../ILBasic/StmtElseIf.h"

using namespace std;


ILAnalyzeScheduleNodeVariableRef::ILAnalyzeScheduleNodeVariableRef()
{

}

ILAnalyzeScheduleNodeVariableRef::ILAnalyzeScheduleNodeVariableRef(
    const ILAnalyzeScheduleNode* node,
    const Expression* expression,
    const ILAnalyzeScheduleVariable* variable,
    const Expression* expressionFull)
    : node(node), expression(expression), variable(variable), expressionFull(expressionFull)
{

}

ILAnalyzeScheduleNodeVariableRef::~ILAnalyzeScheduleNodeVariableRef()
{

}

ILAnalyzeScheduleNodeVariableRef* ILAnalyzeScheduleNodeVariableRef::clone() const
{
    ILAnalyzeScheduleNodeVariableRef* ret = new ILAnalyzeScheduleNodeVariableRef(node, expression, variable, expressionFull);
    return ret;
}

bool ILAnalyzeScheduleNode::isReadVariable(std::string variableName)
{
    for(int k = 0; k < this->variableRead.size(); k++)
    {
        const ILAnalyzeScheduleVariable* var = this->variableRead[k]->variable;
        while(var)
        {
            if(var->name == variableName)
            {
                return true;
            }
            var = var->parent;
        }
    }
    return false;
}

bool ILAnalyzeScheduleNode::isStoreVariable(std::string variableName)
{
    for(int k = 0; k < this->variableStored.size(); k++)
    {
        const ILAnalyzeScheduleVariable* var = this->variableStored[k]->variable;
        while(var)
        {
            if(var->name == variableName)
            {
                return true;
            }
            var = var->parent;
        }
    }
    return false;
}

std::vector<Expression*> ILAnalyzeScheduleNode::getNodeExpressions()
{
    std::vector<Expression*> ret;
    if (this->nodeType == TypeStatement)
    {
        Statement* statement = this->statement;
        return(statement->innerExpressions);
    }
    else if (this->nodeType == TypeBranch)
    {
        ret.push_back(this->iLBranch->condition);
    }
    else if (this->nodeType == TypeLocalVariable)
    {
        if (this->iLLocalVariable->defaultValue != nullptr)
            ret.push_back(this->iLLocalVariable->defaultValue);
    }
    
    return ret;
}

void ILAnalyzeScheduleNode::release()
{
    for (int i = 0; i < variableStored.size(); i++)
    {
        delete variableStored[i];
    }
    variableStored.clear();
    for (int i = 0; i < variableRead.size(); i++)
    {
        delete variableRead[i];
    }
    variableRead.clear();
}
