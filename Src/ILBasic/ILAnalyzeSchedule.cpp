#include "ILAnalyzeSchedule.h"

#include <algorithm>


#include "ILAnalyzeScheduleNode.h"
#include "ILAnalyzeScheduleVariable.h"
#include "../ILBasic/Expression.h"
#include "../ILBasic/ILGlobalVariable.h"
#include "../ILBasic/ILFile.h"
#include "../ILBasic/ILSchedule.h"
#include "../ILBasic/ILFunction.h"
#include "../ILBasic/ILLocalVariable.h"
#include "../ILBasic/ILInput.h"
#include "../ILBasic/ILOutput.h"
#include "../ILBasic/ILCalculate.h"
#include "../ILBasic/ILBranch.h"
#include "../ILBasic/ILStructDefine.h"
#include "../ILBasic/ILMember.h"
#include "../ILBasic/StmtFor.h"
#include "../ILBasic/StmtLocalVariable.h"
#include "../ILBasic/StmtExpression.h"
#include "../ILBasic/Statement.h"
#include "../ILBasic/StmtBatchCalculation.h"
#include "../ILBasic/ILParser.h"
#include "../ILBasic/ILConfig.h"
#include "../Common/Utility.h"

#include "../ILBasic/BasicTypeCalculator.h"

using namespace std;

namespace
{
    const int CONST_NUM_2  = 2;
    const int CONST_NUM_3  = 3;
    const int CONST_NUM_4  = 4;
}


int ILAnalyzeSchedule::collectSchedule(const ILSchedule* schedule, ILParser* parser)
{
    this->schedule = schedule;
    this->iLParser = parser;

    scheduleVariables.clear();

    int res = collectGlobalVariable();
    if(res < 0)
        return res;

    res = collectInputVariable();
    if(res < 0)
        return res;

    res = collectOutputVariable();
    if(res < 0)
        return res;

    res = collectLocalVariable();
    if(res < 0)
        return res;

    res = collectScheduleNode();
    if(res < 0)
        return res;
    
    
	return 1;
}

const std::vector<ILAnalyzeScheduleNode*>* ILAnalyzeSchedule::getScheduleNodeList() const
{
    return &scheduleNodes;
}

std::vector<ILAnalyzeScheduleNode*> ILAnalyzeSchedule::getScheduleNodeListDataFlowOfOneNode(ILAnalyzeScheduleNode* node) const
{
    vector<ILAnalyzeScheduleNode*> ret;

    vector<ILAnalyzeScheduleNode*> treePath;
    vector<ILAnalyzeScheduleNode*> mainStreamBranchNodeList; // 记录当前节点的直系父节点，这些节点不展开数据流

    //treePath.push_back(node);

    // 从当前Node向前遍历兄弟Node和父Node
    ILAnalyzeScheduleNode* cur = node;
    ILAnalyzeScheduleNode* parent;
    do
    {
        ILAnalyzeScheduleNode* prev = cur->prevNode;
        while(prev)
        {
            treePath.insert(treePath.begin(), prev);
            cur = prev;

            if (prev->nodeType == ILAnalyzeScheduleNode::TypeStatement && prev->statement->type == Statement::Case)
                break;
            prev = prev->prevNode;
        }
        parent = cur->parent;
        cur = parent;
        if(parent) {
            treePath.insert(treePath.begin(), parent);
            mainStreamBranchNodeList.push_back(parent);
        }
    } while (parent);


    // 根据得到的NodePath，获取完整的影响数据流的Node列表
    getScheduleNodeListDataFlowOfOneNode(treePath, ret, mainStreamBranchNodeList);
    ret.push_back(node);

    return ret;
}

std::vector<ILAnalyzeScheduleNode*> ILAnalyzeSchedule::getScheduleNodeListContainByOneBranchNode(
    ILAnalyzeScheduleNode* node, bool exceptInnerBranch) const
{
    vector<ILAnalyzeScheduleNode*> ret;
    getScheduleNodeListContainByOneBranchNode(node, ret, exceptInnerBranch);
    return ret;
}


void ILAnalyzeSchedule::getScheduleNodeListDataFlowOfOneNode(std::vector<ILAnalyzeScheduleNode*>& treePath,
                                                             std::vector<ILAnalyzeScheduleNode*>& ret, std::vector<ILAnalyzeScheduleNode*>& mainStreamBranchNodeList) const
{
    for (int i = 0; i < treePath.size(); i++)
    {
        ILAnalyzeScheduleNode* node = treePath[i];
        ret.push_back(node);

        if(!node->children.empty() && findIndexInVector(mainStreamBranchNodeList, node) == -1) {
            getScheduleNodeListDataFlowOfOneNode(node->children, ret, mainStreamBranchNodeList);
        }
    }
}

void ILAnalyzeSchedule::getScheduleNodeListContainByOneBranchNode(ILAnalyzeScheduleNode* node,
    std::vector<ILAnalyzeScheduleNode*>& ret, bool exceptInnerBranch) const
{
    if(node->nodeType == ILAnalyzeScheduleNode::TypeStatement &&
        (node->statement->type == Statement::Case ||
            node->statement->type == Statement::Default))
    {
        ILAnalyzeScheduleNode* tempNode = node->nextNode;
        while(tempNode)
        {
            if (tempNode->nodeType == ILAnalyzeScheduleNode::TypeStatement &&
                    (tempNode->statement->type == Statement::Case ||
                    tempNode->statement->type == Statement::Default))
            {
                break;
            }
            ret.push_back(tempNode);

            if (exceptInnerBranch &&
                (tempNode->nodeType == ILAnalyzeScheduleNode::TypeBranch ||
                    (tempNode->nodeType == ILAnalyzeScheduleNode::TypeStatement &&
                        (tempNode->statement->type == Statement::If ||
                            tempNode->statement->type == Statement::Else ||
                            tempNode->statement->type == Statement::ElseIf ||
                            tempNode->statement->type == Statement::Case ||
                            tempNode->statement->type == Statement::Default))))
            {
                tempNode = tempNode->nextNode;
                continue;
            }
            getScheduleNodeListContainByOneBranchNode(tempNode, ret, exceptInnerBranch);


            tempNode = tempNode->nextNode;
        }
    }
    else
    {
        for (int i = 0; i < node->children.size(); i++)
        {
            ILAnalyzeScheduleNode* tempNode = node->children[i];
            ret.push_back(tempNode);

            if (exceptInnerBranch &&
                (tempNode->nodeType == ILAnalyzeScheduleNode::TypeBranch ||
                    (tempNode->nodeType == ILAnalyzeScheduleNode::TypeStatement &&
                        (tempNode->statement->type == Statement::If ||
                            tempNode->statement->type == Statement::Else ||
                            tempNode->statement->type == Statement::ElseIf ||
                            tempNode->statement->type == Statement::Case ||
                            tempNode->statement->type == Statement::Default))))
            {
                continue;
            }
            getScheduleNodeListContainByOneBranchNode(tempNode, ret, exceptInnerBranch);
        }
    }
    
}

int ILAnalyzeSchedule::collectGlobalVariable()
{

    for(int f = 0; f < iLParser->files.size(); f++)
    {
        ILFile* file = iLParser->files[f];

        for (int i = 0; i < file->globalVariables.size(); i++)
        {
            ILGlobalVariable* iLGlobalVariable = file->globalVariables[i];
            ILAnalyzeScheduleVariable* variable = new ILAnalyzeScheduleVariable();
            variable->variableType = ILAnalyzeScheduleVariable::TypeGlobalVariable;
            variable->iLGlobalVariable = iLGlobalVariable;
            variable->name = iLGlobalVariable->name;
            variable->type = iLGlobalVariable->type;
            variable->isAddress = iLGlobalVariable->isAddress;
            variable->arraySize = iLGlobalVariable->arraySize;

            variable->callName = iLGlobalVariable->name;

            variable->domain = iLGlobalVariable->getDomain();

            variable->parent = nullptr;

            variable->referedGlobalVariable = variable;

            scheduleVariables[variable->name].push_back(variable);


            ILAnalyzeScheduleNode* node = new ILAnalyzeScheduleNode();
            node->id = scheduleNodes.size();
            scheduleNodes.push_back(node);
            if (!scheduleNodeTree.empty()) {
                node->prevNode = scheduleNodeTree[scheduleNodeTree.size() - 1];
                scheduleNodeTree[scheduleNodeTree.size() - 1]->nextNode = node;
            }
            scheduleNodeTree.push_back(node);
            node->nodeType = ILAnalyzeScheduleNode::TypeGlobalVariable;

            node->variableStored.push_back(new ILAnalyzeScheduleNodeVariableRef(node, nullptr, variable, nullptr));
            node->domain = file->getDomain();

        }
    }
    
	return 1;
}

int ILAnalyzeSchedule::collectInputVariable()
{
    ILFunction* function = static_cast<ILFunction*>(schedule->parent);
	ILFile* file = static_cast<ILFile*>(function->parent);

    for(int i = 0; i < function->inputs.size();i++)
	{
        ILAnalyzeScheduleVariable* variable = new ILAnalyzeScheduleVariable();
		variable->variableType = ILAnalyzeScheduleVariable::TypeInput;
		variable->iLInput = function->inputs[i];
        variable->name = function->inputs[i]->name;
        variable->type = function->inputs[i]->type;
        variable->isAddress = function->inputs[i]->isAddress;
        variable->arraySize = function->inputs[i]->arraySize;

        variable->callName = function->inputs[i]->name;
        variable->domain = function->inputs[i]->getDomain();
        variable->parent = nullptr;
        
        scheduleVariables[variable->name].push_back(variable);
        
        ILAnalyzeScheduleNode* node = new ILAnalyzeScheduleNode();
        node->id = scheduleNodes.size();
        scheduleNodes.push_back(node);
        if (!scheduleNodeTree.empty()) {
            node->prevNode = scheduleNodeTree[scheduleNodeTree.size() - 1];
            scheduleNodeTree[scheduleNodeTree.size() - 1]->nextNode = node;
        }
        scheduleNodeTree.push_back(node);
		node->nodeType = ILAnalyzeScheduleNode::TypeInput;
        node->variableStored.push_back(new ILAnalyzeScheduleNodeVariableRef(node, nullptr, variable, nullptr));
        node->domain = function->inputs[i]->getDomain();

        // 设置self输入参数的全局变量引用
        if (stringStartsWith(variable->type, "system_root_state") ||
            stringStartsWith(variable->type, "RT_MODEL_"))
        {
            string type = ILParser::getRealDataType(variable->type);
            for (auto item : scheduleVariables)
            {
                bool find = false;
                vector<ILAnalyzeScheduleVariable*>* varList = &item.second;
                for (int j = 0; j < varList->size(); j++)
                {
                    ILAnalyzeScheduleVariable* var2 = (*varList)[j];
                    string type2 = ILParser::getRealDataType(var2->type);
                    if (var2->variableType == ILAnalyzeScheduleVariable::TypeGlobalVariable && type2 == type)
                    {
                        variable->referedGlobalVariable = var2;
                        find = true;
                        break;
                    }
                }
                if (find)
                    break;
            }
        }
	}

	return 1;
}

int ILAnalyzeSchedule::collectOutputVariable()
{
    ILFunction* function = static_cast<ILFunction*>(schedule->parent);
	ILFile* file = static_cast<ILFile*>(function->parent);

    for(int i = 0; i < function->outputs.size();i++)
	{
        ILAnalyzeScheduleVariable* variable = new ILAnalyzeScheduleVariable();
		variable->variableType = ILAnalyzeScheduleVariable::TypeOutput;
		variable->iLOutput = function->outputs[i];
        variable->name = function->outputs[i]->name;
        variable->type = function->outputs[i]->type;
        variable->isAddress = function->outputs[i]->isAddress;
        variable->arraySize = function->outputs[i]->arraySize;

        variable->callName = function->outputs[i]->name;
        variable->domain = function->outputs[i]->getDomain();
        variable->parent = nullptr;
        
        scheduleVariables[variable->name].push_back(variable);
        

        ILAnalyzeScheduleNode* node = new ILAnalyzeScheduleNode();
        node->id = scheduleNodes.size();
        scheduleNodes.push_back(node);
        if (!scheduleNodeTree.empty()) {
            node->prevNode = scheduleNodeTree[scheduleNodeTree.size() - 1];
            scheduleNodeTree[scheduleNodeTree.size() - 1]->nextNode = node;
        }
        scheduleNodeTree.push_back(node);
		node->nodeType = ILAnalyzeScheduleNode::TypeOutput;
        node->variableStored.push_back(new ILAnalyzeScheduleNodeVariableRef(node, nullptr, variable, nullptr));
        node->domain = function->outputs[i]->getDomain();

	}

	return 1;
}

int ILAnalyzeSchedule::collectLocalVariable()
{
    ILFunction* function = static_cast<ILFunction*>(schedule->parent);
	ILFile* file = static_cast<ILFile*>(function->parent);

    for(int i = 0; i < schedule->localVariables.size();i++)
	{
        ILAnalyzeScheduleVariable* variable = new ILAnalyzeScheduleVariable();
		variable->variableType = ILAnalyzeScheduleVariable::TypeLocalVariable;
		variable->iLLocalVariable = schedule->localVariables[i];
        variable->name = schedule->localVariables[i]->name;
        variable->type = schedule->localVariables[i]->type;
        variable->isAddress = schedule->localVariables[i]->isAddress;
        variable->arraySize = schedule->localVariables[i]->arraySize;

        variable->callName = schedule->localVariables[i]->name;
        variable->domain = schedule->localVariables[i]->getDomain();
        variable->parent = nullptr;
        
        scheduleVariables[variable->name].push_back(variable);
        

        ILAnalyzeScheduleNode* node = new ILAnalyzeScheduleNode();
        node->id = scheduleNodes.size();
        scheduleNodes.push_back(node);
        if (!scheduleNodeTree.empty()) {
            node->prevNode = scheduleNodeTree[scheduleNodeTree.size() - 1];
            scheduleNodeTree[scheduleNodeTree.size() - 1]->nextNode = node;
        }
        scheduleNodeTree.push_back(node);
		node->nodeType = ILAnalyzeScheduleNode::TypeLocalVariable;
        node->variableStored.push_back(new ILAnalyzeScheduleNodeVariableRef(node, nullptr, variable, nullptr));
        node->domain = schedule->localVariables[i]->getDomain();
        node->iLLocalVariable = schedule->localVariables[i];

        if(variable->iLLocalVariable->defaultValue)
        {
            int res = collectScheduleNodeExpressionVariable(variable->iLLocalVariable->defaultValue, node, false);
            if(res < 0)
                return res;
        }

        int res = calculateLocalVarIsReferToGlobalVar(node);
        if (res < 0)
            return res;

        res = calculateLocalVarIsConstant(node);
        if (res < 0)
            return res;
	}

	return 1;
}

int ILAnalyzeSchedule::collectScheduleNode()
{
    int res;
    ILFunction* function = static_cast<ILFunction*>(schedule->parent);
	ILFile* file = static_cast<ILFile*>(function->parent);
    std::vector<void*> domain = {file, function};

	for(int i = 0;i < schedule->scheduleNodes.size(); i++)
	{
		ILScheduleNode* iLScheduleNode = schedule->scheduleNodes[i];
		if(iLScheduleNode->objType == ILObject::TypeCalculate)
		{
			res = collectScheduleNodeTraverseCalculate(static_cast<ILCalculate*>(iLScheduleNode), nullptr);
			if(res < 0)
				return res;
		}
		else if(iLScheduleNode->objType == ILObject::TypeBranch)
		{
			res = collectScheduleNodeTraverseBranch(static_cast<ILBranch*>(iLScheduleNode), nullptr);
			if(res < 0)
				return res;
		}
	}
    return 1;
}

int ILAnalyzeSchedule::collectScheduleNodeTraverseBranch(ILBranch* iLBranch, ILAnalyzeScheduleNode* parent)
{
    int res;
    ILAnalyzeScheduleNode* node = new ILAnalyzeScheduleNode();
    node->id = scheduleNodes.size();
    scheduleNodes.push_back(node);
    if (parent) {
        if (!parent->children.empty()) {
            node->prevNode = parent->children[parent->children.size() - 1];
            parent->children[parent->children.size() - 1]->nextNode = node;
        }
        parent->children.push_back(node);
        node->depth = parent->depth + 1;
    } else {
        if (!scheduleNodeTree.empty()) {
            node->prevNode = scheduleNodeTree[scheduleNodeTree.size() - 1];
            scheduleNodeTree[scheduleNodeTree.size() - 1]->nextNode = node;
        }
        scheduleNodeTree.push_back(node); // 未指明父节点则加入顶级树中
    }
    node->parent = parent;
	node->nodeType = ILAnalyzeScheduleNode::TypeBranch;
    node->domain = iLBranch->getDomain();
    node->iLBranch = iLBranch;

    if(iLBranch->condition)
    {
        res = collectScheduleNodeExpressionVariable(iLBranch->condition, node, false);
        if(res < 0)
            return res;
    }


    for(int i = 0;i < iLBranch->scheduleNodes.size(); i++)
	{
		ILScheduleNode* iLScheduleNode = iLBranch->scheduleNodes[i];
		if(iLScheduleNode->objType == ILObject::TypeCalculate)
		{
			res = collectScheduleNodeTraverseCalculate(static_cast<ILCalculate*>(iLScheduleNode), node);
			if(res < 0)
				return res;
		}
		else if(iLScheduleNode->objType == ILObject::TypeBranch)
		{
			res = collectScheduleNodeTraverseBranch(static_cast<ILBranch*>(iLScheduleNode), node);
			if(res < 0)
				return res;
		}
	}
    
    return 1;
}

int ILAnalyzeSchedule::collectScheduleNodeTraverseCalculate(ILCalculate* iLCalculate, ILAnalyzeScheduleNode* parent)
{
    currentTraverseILCalculate = iLCalculate;
    for(int i = 0;i < iLCalculate->statements.childStatements.size(); i++)
	{
		Statement* statement = iLCalculate->statements.childStatements[i];
		int res = collectScheduleNodeTraverseStatement(statement, iLCalculate, parent);
		if(res < 0)
			return res;
	}
	return 1;
}

int ILAnalyzeSchedule::calculateLocalVarIsReferToGlobalVar(ILAnalyzeScheduleNode* node)
{
    string refVarName;
    Expression* assignExpression = nullptr;
    if (node->nodeType == ILAnalyzeScheduleNode::TypeLocalVariable)
    {
        ILLocalVariable* localVariable = node->iLLocalVariable;
        if (localVariable->defaultValue)
        {
            refVarName = getVariableExpRootVarName(localVariable->defaultValue->expressionStr);
        }
    }
    else if (node->nodeType == ILAnalyzeScheduleNode::TypeStatement)
    {
        Statement* statement = node->statement;
        if (statement->type == Statement::LocalVariable)
        {
            StmtLocalVariable* localVariable = static_cast<StmtLocalVariable*>(statement);
            if (localVariable->defaultValue)
            {
                refVarName = getVariableExpRootVarName(localVariable->defaultValue->expressionStr);
            }
        }
        else if (statement->type == Statement::ExpressionWithSemicolon)
        {
            StmtExpression* expressionStmt = static_cast<StmtExpression*>(statement);
            Expression* expression = expressionStmt->expression;
            if (expression->type == Token::Assign)
            {
                refVarName = getVariableExpRootVarName(expression->childExpressions[1]->expressionStr);
                assignExpression = expression;
            }
        }
    }

    bool find = false;
    ILAnalyzeScheduleVariable* referedGlobalVariable = nullptr;
    if (!refVarName.empty() && scheduleVariables.find(refVarName) != scheduleVariables.end())
    {
        vector<ILAnalyzeScheduleVariable*>* varList = &scheduleVariables[refVarName];

        for (int i = 0; i < varList->size(); i++)
        {
            ILAnalyzeScheduleVariable* var = (*varList)[i];
            if(node->variableStored.empty())
            {
                cout << "Error: node->variableStored is empty in calculateLocalVarIsReferToGlobalVar" << endl;
                cout << "VariableNotFound;" << endl;
            }

            ILAnalyzeScheduleVariable* variable = const_cast<ILAnalyzeScheduleVariable*>(node->variableStored[0]->variable);
            if (var->variableType == ILAnalyzeScheduleVariable::TypeInput &&
                var->referedGlobalVariable)
            {
                variable->referedGlobalVariable = var->referedGlobalVariable;
                referedGlobalVariable = var->referedGlobalVariable;
                find = true;
                break;
            }
            else if (var->variableType == ILAnalyzeScheduleVariable::TypeGlobalVariable)
            {
                variable->referedGlobalVariable = var;
                referedGlobalVariable = var;
                find = true;
                break;
            }
        }
    }

    // 如果一个赋值语句的右边关联到了全局变量，那么就要把左边对应的局部变量也关联到全局变量
    if (find && assignExpression)
    {
        Expression* expression = assignExpression;
        string varRefVarName = getVariableExpRootVarName(expression->childExpressions[0]->expressionStr);
        if (scheduleVariables.find(varRefVarName) != scheduleVariables.end())
        {
            vector<ILAnalyzeScheduleVariable*>* varList = &scheduleVariables[varRefVarName];

            for (int i = 0; i < varList->size(); i++)
            {
                ILAnalyzeScheduleVariable* var = (*varList)[i];
                ILAnalyzeScheduleVariable* variable = const_cast<ILAnalyzeScheduleVariable*>(node->variableStored[0]->variable);
                variable->referedGlobalVariable = referedGlobalVariable;
            }
        }
    }

    if (find) {
        return 1;
    }

    return 0;
}

int ILAnalyzeSchedule::calculateLocalVarIsConstant(ILAnalyzeScheduleNode* node)
{

    bool find = false;
    string refVarName;
    if (node->nodeType == ILAnalyzeScheduleNode::TypeLocalVariable)
    {
        ILLocalVariable* localVariable = node->iLLocalVariable;
        if (localVariable->defaultValue && localVariable->defaultValue->isConstantExpression())
        {
            ILAnalyzeScheduleVariable* variable = const_cast<ILAnalyzeScheduleVariable*>(node->variableStored[0]->variable);
            variable->referedConstantValue = localVariable->defaultValue->expressionStr;
            find = true;
        }
    }
    else if (node->nodeType == ILAnalyzeScheduleNode::TypeStatement)
    {
        Statement* statement = node->statement;
        if (statement->type == Statement::LocalVariable)
        {
            StmtLocalVariable* localVariable = static_cast<StmtLocalVariable*>(statement);
            if (localVariable->defaultValue && localVariable->defaultValue->isConstantExpression())
            {
                ILAnalyzeScheduleVariable* variable = const_cast<ILAnalyzeScheduleVariable*>(node->variableStored[0]->variable);
                variable->referedConstantValue = localVariable->defaultValue->expressionStr;
                find = true;
            }
        }
        else if (statement->type == Statement::ExpressionWithSemicolon)
        {
            StmtExpression* expressionStmt = static_cast<StmtExpression*>(statement);
            Expression* expression = expressionStmt->expression;
            if (expression->type == Token::Assign)
            {
                Expression* v0 = expression->childExpressions[0];
                Expression* v1 = expression->childExpressions[1];
                if (v1->isConstantExpression())
                {
                    ILAnalyzeScheduleVariable* variable = const_cast<ILAnalyzeScheduleVariable*>(node->variableStored[0]->variable);
                    variable->referedConstantValue = v1->expressionStr;

                    string varRefVarName = getVariableExpRootVarName(v0->expressionStr);
                    if (scheduleVariables.find(varRefVarName) != scheduleVariables.end())
                    {
                        vector<ILAnalyzeScheduleVariable*>* varList = &scheduleVariables[varRefVarName];

                        for (int i = 0; i < varList->size(); i++)
                        {
                            ILAnalyzeScheduleVariable* var = (*varList)[i];
                            ILAnalyzeScheduleVariable* variable = const_cast<ILAnalyzeScheduleVariable*>(node->variableStored[0]->variable);
                            variable->referedConstantValue = v1->expressionStr;
                            find = true;
                        }
                    }
                }
                
            }
        }
    }

    if (find) {
        return 1;
    }

    return 0;
}


int ILAnalyzeSchedule::collectScheduleNodeTraverseStatement(Statement* statement, ILCalculate* iLCalculate, ILAnalyzeScheduleNode* parent)
{
    int res = 0;
    bool isBranch = true;
    if (statement->type == Statement::If || statement->type == Statement::Else || 
        statement->type == Statement::ElseIf ||
        statement->type == Statement::While || statement->type == Statement::Switch) {
        res = collectScheduleNodeTraverseStatementBranch(statement, iLCalculate, parent);
    }
    else if(statement->type == Statement::DoWhile)
	{
        res = collectScheduleNodeTraverseStatementDoWhile(statement, iLCalculate, parent);
	}
    else if(statement->type == Statement::For)
	{
        res = collectScheduleNodeTraverseStatementFor(statement, iLCalculate, parent);
	}
    else if(statement->type == Statement::LocalVariable)
	{
        res = collectScheduleNodeTraverseStatementLocalVariable(statement, iLCalculate, parent);
	}
    else if(statement->type == Statement::BatchCalculation)
	{
        res = collectScheduleNodeTraverseStatementBatchCalculation(statement, iLCalculate, parent);
	}
    else
    {
        isBranch = false;
    }
    if(isBranch)
    {
        return res;
    }
    
    Expression* exp = statement->getValueExpression();
    // vector<Statement*>* stmtList = statement->getStatementList();

    ILAnalyzeScheduleNode* node = new ILAnalyzeScheduleNode();
    node->id = scheduleNodes.size();
    scheduleNodes.push_back(node);
    if (parent) {
        if (!parent->children.empty()) {
            node->prevNode = parent->children[parent->children.size() - 1];
            parent->children[parent->children.size() - 1]->nextNode = node;
        }
        parent->children.push_back(node);
        node->depth = parent->depth + 1;
    }
    else {
        if (!scheduleNodeTree.empty()) {
            node->prevNode = scheduleNodeTree[scheduleNodeTree.size() - 1];
            scheduleNodeTree[scheduleNodeTree.size() - 1]->nextNode = node;
        }
        scheduleNodeTree.push_back(node); // 未指明父节点则加入顶级树中
    }
    node->parent = parent;
	node->nodeType = ILAnalyzeScheduleNode::TypeStatement;
    node->domain = statement->getDomain();
    node->statement = statement;
    node->iLCalculate = iLCalculate;

    if(exp)
    {
        res = collectScheduleNodeExpressionVariable(exp, node, false);
        if(res < 0)
            return res;
    }

    res = calculateLocalVarIsReferToGlobalVar(node);
    if (res < 0)
        return res;

    res = calculateLocalVarIsConstant(node);
    if (res < 0)
        return res;

	return 1;
}

int ILAnalyzeSchedule::collectScheduleNodeTraverseStatementBranch(Statement* statement, ILCalculate* iLCalculate, ILAnalyzeScheduleNode* parent)
{
    int res;
    
    Expression* exp = statement->getValueExpression();
    vector<Statement*>* stmtList = statement->getStatementList();

    ILAnalyzeScheduleNode* node = new ILAnalyzeScheduleNode();
    node->id = scheduleNodes.size();
    scheduleNodes.push_back(node);
    if (parent) {
        if (!parent->children.empty()) {
            node->prevNode = parent->children[parent->children.size() - 1];
            parent->children[parent->children.size() - 1]->nextNode = node;
        }
        parent->children.push_back(node);
        node->depth = parent->depth + 1;
    }
    else {
        if (!scheduleNodeTree.empty()) {
            node->prevNode = scheduleNodeTree[scheduleNodeTree.size() - 1];
            scheduleNodeTree[scheduleNodeTree.size() - 1]->nextNode = node;
        }
        scheduleNodeTree.push_back(node); // 未指明父节点则加入顶级树中
    }
    node->parent = parent;
	node->nodeType = ILAnalyzeScheduleNode::TypeStatement;
    node->domain = statement->getDomain();
    node->statement = statement;
    node->iLCalculate = iLCalculate;

    if(exp)
    {
        res = collectScheduleNodeExpressionVariable(exp, node, false);
        if(res < 0)
            return res;
    }


    
    for (int i = 0; stmtList && i < stmtList->size(); i++)
	{
		Statement* statement = (*(stmtList))[i];
		res = collectScheduleNodeTraverseStatement(statement, iLCalculate, node);
		if(res < 0)
			return res;
	}

    return 1;
}

int ILAnalyzeSchedule::collectScheduleNodeTraverseStatementDoWhile(Statement* statement, ILCalculate* iLCalculate, ILAnalyzeScheduleNode* parent)
{
    int res;
    
    Expression* exp = statement->getValueExpression();
    vector<Statement*>* stmtList = statement->getStatementList();


    ILAnalyzeScheduleNode* node = new ILAnalyzeScheduleNode();
    
    for (int i = 0; stmtList &&  i < stmtList->size(); i++)
	{
		Statement* statement = (*(stmtList))[i];
		res = collectScheduleNodeTraverseStatement(statement, iLCalculate, node);
		if(res < 0)
			return res;
	}

    node->id = scheduleNodes.size();
    scheduleNodes.push_back(node);
    if (parent) {
        if (!parent->children.empty()) {
            node->prevNode = parent->children[parent->children.size() - 1];
            parent->children[parent->children.size() - 1]->nextNode = node;
        }
        parent->children.push_back(node);
        node->depth = parent->depth + 1;
    }
    else {
        if (!scheduleNodeTree.empty()) {
            node->prevNode = scheduleNodeTree[scheduleNodeTree.size() - 1];
            scheduleNodeTree[scheduleNodeTree.size() - 1]->nextNode = node;
        }
        scheduleNodeTree.push_back(node); // 未指明父节点则加入顶级树中
    }
    node->parent = parent;
	node->nodeType = ILAnalyzeScheduleNode::TypeStatement;
    node->domain = statement->getDomain();
    node->statement = statement;
    node->iLCalculate = iLCalculate;

    if(exp)
    {
        res = collectScheduleNodeExpressionVariable(exp, node, false);
        if(res < 0)
            return res;
    }


    return 1;
}

int ILAnalyzeSchedule::collectScheduleNodeTraverseStatementFor(Statement* statement, ILCalculate* iLCalculate, ILAnalyzeScheduleNode* parent)
{
    int res = 0;
    Expression* exp = statement->getValueExpression();
    vector<Statement*>* stmtList = statement->getStatementList();
    

	StmtFor* stmtFor = static_cast<StmtFor*>(statement);
	for(int i = 0; i < stmtFor->initial.size();i++) {
		res = collectScheduleNodeTraverseStatement(stmtFor->initial[i], iLCalculate, parent);
        if(res < 0)
            return res;
	}

    ILAnalyzeScheduleNode* node = new ILAnalyzeScheduleNode();
    node->id = scheduleNodes.size();
    scheduleNodes.push_back(node);
    if (parent) {
        if (!parent->children.empty()) {
            node->prevNode = parent->children[parent->children.size() - 1];
            parent->children[parent->children.size() - 1]->nextNode = node;
        }
        parent->children.push_back(node);
        node->depth = parent->depth + 1;
    }
    else {
        if (!scheduleNodeTree.empty()) {
            node->prevNode = scheduleNodeTree[scheduleNodeTree.size() - 1];
            scheduleNodeTree[scheduleNodeTree.size() - 1]->nextNode = node;
        }
        scheduleNodeTree.push_back(node); // 未指明父节点则加入顶级树中
    }
    node->parent = parent;
	node->nodeType = ILAnalyzeScheduleNode::TypeStatement;
    node->domain = statement->getDomain();
    node->statement = statement;
    node->iLCalculate = iLCalculate;
    if(stmtFor->condition)
    {
        res = collectScheduleNodeExpressionVariable(stmtFor->condition, node, false);
        if(res < 0)
            return res;
    }

    for(int i = 0; res >=0 && i < stmtFor->statements.size();i++) {
		res = collectScheduleNodeTraverseStatement(stmtFor->statements[i], iLCalculate, node);
	}
    for(int i = 0; res >=0 && i < stmtFor->iterator.size();i++) {
		res = collectScheduleNodeTraverseStatement(stmtFor->iterator[i], iLCalculate, node);
	}
    if(res < 0){
        return res;
    }
	return 1;
}

int ILAnalyzeSchedule::collectScheduleNodeTraverseStatementLocalVariable(Statement* statement, ILCalculate* iLCalculate, ILAnalyzeScheduleNode* parent)
{
    int res;
    StmtLocalVariable* stmt = static_cast<StmtLocalVariable*>(statement);

    ILAnalyzeScheduleNode* node = new ILAnalyzeScheduleNode();
    node->id = scheduleNodes.size();
    scheduleNodes.push_back(node);
    if (parent) {
        if (!parent->children.empty()) {
            node->prevNode = parent->children[parent->children.size() - 1];
            parent->children[parent->children.size() - 1]->nextNode = node;
        }
        parent->children.push_back(node);
        node->depth = parent->depth + 1;
    }
    else {
        if (!scheduleNodeTree.empty()) {
            node->prevNode = scheduleNodeTree[scheduleNodeTree.size() - 1];
            scheduleNodeTree[scheduleNodeTree.size() - 1]->nextNode = node;
        }
        scheduleNodeTree.push_back(node); // 未指明父节点则加入顶级树中
    }
    node->parent = parent;
	node->nodeType = ILAnalyzeScheduleNode::TypeStatement;
    node->domain = stmt->getDomain();
    node->statement = statement;
    node->iLCalculate = iLCalculate;

    ILAnalyzeScheduleVariable* variable = new ILAnalyzeScheduleVariable();
	variable->variableType = ILAnalyzeScheduleVariable::TypeStatementVariable;
	variable->statement = stmt;
    variable->iLCalculate = currentTraverseILCalculate;
    variable->name = stmt->name;
    variable->type = stmt->dataType;
    variable->isAddress = stmt->isAddress;
    variable->arraySize = stmt->arraySize;

    variable->callName = stmt->name;
    variable->domain = stmt->getDomain();
    variable->parent = nullptr;

    scheduleVariables[variable->name].push_back(variable);

    node->variableStored.push_back(new ILAnalyzeScheduleNodeVariableRef(node, nullptr, variable, nullptr));

    if(stmt->defaultValue)
    {
        res = collectScheduleNodeExpressionVariable(stmt->defaultValue, node, false);
        if(res < 0)
            return res;
    }


    res = calculateLocalVarIsReferToGlobalVar(node);
    if (res < 0)
        return res;

    res = calculateLocalVarIsConstant(node);
    if (res < 0)
        return res;

    return 1;
}

int ILAnalyzeSchedule::collectScheduleNodeTraverseStatementBatchCalculation(Statement* statement,
    ILCalculate* iLCalculate, ILAnalyzeScheduleNode* parent)
{
    int res;
    StmtBatchCalculation* stmt = static_cast<StmtBatchCalculation*>(statement);

    ILAnalyzeScheduleNode* node = new ILAnalyzeScheduleNode();
    node->id = scheduleNodes.size();
    scheduleNodes.push_back(node);
    if (parent) {
        if (!parent->children.empty()) {
            node->prevNode = parent->children[parent->children.size() - 1];
            parent->children[parent->children.size() - 1]->nextNode = node;
        }
        parent->children.push_back(node);
        node->depth = parent->depth + 1;
    }
    else {
        if (!scheduleNodeTree.empty()) {
            node->prevNode = scheduleNodeTree[scheduleNodeTree.size() - 1];
            scheduleNodeTree[scheduleNodeTree.size() - 1]->nextNode = node;
        }
        scheduleNodeTree.push_back(node); // 未指明父节点则加入顶级树中
    }
    node->parent = parent;
	node->nodeType = ILAnalyzeScheduleNode::TypeStatement;
    node->domain = stmt->getDomain();
    node->statement = statement;
    node->iLCalculate = iLCalculate;

    for(int i = 0; i < stmt->inputs.size(); i++)
    {
        string varName = stmt->inputs[i].name;
        if(varName.empty()) {
            continue;
        }
        vector<ILAnalyzeScheduleVariable*>* varList = &(scheduleVariables[varName]);
        ILAnalyzeScheduleVariable* var = getVariableInDomain(varList, node->domain);
        if(!var) {
            return -ILAnalyzeSchedule::ErrorVariableCanNotFindReference;
        }
        node->variableRead.push_back(new ILAnalyzeScheduleNodeVariableRef(node, nullptr, var, nullptr));
    }
    for(int i = 0; i < stmt->outputs.size(); i++)
    {
        string varName = stmt->outputs[i].name;
        vector<ILAnalyzeScheduleVariable*>* varList = &(scheduleVariables[varName]);
        ILAnalyzeScheduleVariable* var = getVariableInDomain(varList, node->domain);
        if(!var) {
            return -ILAnalyzeSchedule::ErrorVariableCanNotFindReference;
        }
        node->variableStored.push_back(new ILAnalyzeScheduleNodeVariableRef(node, nullptr, var, nullptr));
    }
    
    return 1;
}

int ILAnalyzeSchedule::collectScheduleNodeExpressionVariable(Expression* expression, ILAnalyzeScheduleNode* node, bool isAssigned)
{
    int res;
    if(expression->type >= Token::Assign && expression->type <= Token::OrAssign)
    {
        //comment = true;
        //int storeVariableSize = node->variableStored.size();
        if((res = collectScheduleNodeExpressionVariable(expression->childExpressions[0], node, true)) < 0)
            return res;
        else if((res = collectScheduleNodeExpressionVariable(expression->childExpressions[1], node, false)) < 0)
            return res;
    }
    else if(expression->type == Token::Variable || expression->type == Token::Point || expression->type == Token::Arrow || expression->type == Token::ArrayIndex)
    {
        Expression* passExpression = nullptr;
        ILAnalyzeScheduleVariable* var = getVariableByExpressionAndDomain(expression, node->domain, passExpression);
        
        if((res = collectScheduleNodeExpressionVariableInner(expression, node, false)) < 0)
            return res;
        else if (!var)
            return 0;
        // comment = true;
        // -ILOptimizer::ErrorVariableInExpressionCanNotFindDefination;

        ILAnalyzeScheduleVariableType expressionType = getVariableExpressionType(expression, node->domain);
        // bool isExpInFunction = isExpressionInFunction(expression);
        // comment = true;
        //while(var)
        //{
        bool readByAddress = ((passExpression && passExpression->type == Token::AddressOf) || // 默认取地址操作即为写数据
            !expressionType.arraySize.empty() || expressionType.isAddress > 0);
        if(isAssigned || readByAddress) {
            // 默认对数组的读或写操作都为写数据，读数组等价于取数组首地址
            // 默认读写地址类型的数据都为写数据，因为地址被其它变量获取后可能通过指针赋值
            Expression* exp = passExpression ? passExpression : expression;

            node->variableStored.push_back(
                new ILAnalyzeScheduleNodeVariableRef(node, exp, var, expression));

            if(readByAddress) {
                node->variableRead.push_back(
                    new ILAnalyzeScheduleNodeVariableRef(node, exp, var, expression));
            }

        } else {
            if(passExpression){
                node->variableRead.push_back(
                    new ILAnalyzeScheduleNodeVariableRef(node, passExpression, var, expression));
            } else {
                node->variableRead.push_back(
                    new ILAnalyzeScheduleNodeVariableRef(node, expression, var, expression));
            }
        }
    }
    else
    {
        for(int i = 0; i < expression->childExpressions.size(); i++)
        {
            bool isAssignedTemp = (expression->type == Token::AddressOf) ? true : isAssigned;
            if((res = collectScheduleNodeExpressionVariable(expression->childExpressions[i], node, isAssignedTemp)) < 0)
                return res;
        }
    }

    return 1;
}

int ILAnalyzeSchedule::collectScheduleNodeExpressionVariableInner(Expression* expression,
    ILAnalyzeScheduleNode* node, bool isAssigned)
{
    int res;
    if(expression->type == Token::Point || expression->type == Token::Arrow) {

        res = collectScheduleNodeExpressionVariableInner(expression->childExpressions[0], node, false);
        if (res < 0)
            return res;
        if(expression->childExpressions[1]->type == Token::Function) {
            res = collectScheduleNodeExpressionVariableInner(expression->childExpressions[1], node, false);
            if (res < 0)
                return res;
        }

    } else if(expression->type == Token::Bracket) {
        for(int i = 0; i < expression->childExpressions.size(); i++)
        {
            res = collectScheduleNodeExpressionVariable(expression->childExpressions[i], node, false);
            if (res < 0)
                return res;
        }
    } else if(expression->type == Token::Function) {
        for(int i = 0; !expression->childExpressions.empty() && i < expression->childExpressions[0]->childExpressions.size(); i++)
        {
            res = collectScheduleNodeExpressionVariable(expression->childExpressions[0]->childExpressions[i], node, false);
            if (res < 0)
                return res;
        }
    } else {
        for(int i = 1; i < expression->childExpressions.size(); i++)
        {
            res = collectScheduleNodeExpressionVariable(expression->childExpressions[i], node, false);
            if (res < 0)
                return res;
        }
    }
    return 1;
}


int ILAnalyzeSchedule::recollectScheduleNodeExpressionVariable(ILAnalyzeScheduleNode* node)
{
    int res;
    if(!(node->nodeType == ILAnalyzeScheduleNode::TypeLocalVariable || node->nodeType == ILAnalyzeScheduleNode::TypeBranch ||
        node->nodeType == ILAnalyzeScheduleNode::TypeStatement))
        return 0;

    for(int i = 0; i < node->variableRead.size(); i++) {
        delete node->variableRead[i];
    }
    node->variableRead.clear();
    for(int i = 0; i < node->variableStored.size(); i++) {
        delete node->variableStored[i];
    }
    node->variableStored.clear();
    if(node->nodeType == ILAnalyzeScheduleNode::TypeLocalVariable) {
        string variableName = node->iLLocalVariable->name;
        vector<ILAnalyzeScheduleVariable*>* variableList = &(scheduleVariables[variableName]);
        ILAnalyzeScheduleVariable* variable = getVariableInDomain(variableList, node->domain);
        if(!variable) {
            return -ILAnalyzeSchedule::ErrorInternal;
        }
        node->variableStored.push_back(new ILAnalyzeScheduleNodeVariableRef(node, nullptr, variable, nullptr));
        if(node->iLLocalVariable->defaultValue && (res = collectScheduleNodeExpressionVariable(node->iLLocalVariable->defaultValue, node, false)) < 0) {
            return res;
        }
    } else if(node->nodeType == ILAnalyzeScheduleNode::TypeBranch) {
        if(node->iLBranch->condition && (res = collectScheduleNodeExpressionVariable(node->iLBranch->condition, node, false)) < 0) {
            return res;
        }
    } else if(node->nodeType == ILAnalyzeScheduleNode::TypeStatement) {
        Statement* statement = node->statement;
        if(statement->type == Statement::LocalVariable) {
            StmtLocalVariable* stmtLocalVariable = static_cast<StmtLocalVariable*>(statement);
            string variableName = stmtLocalVariable->name;
            vector<ILAnalyzeScheduleVariable*>* variableList = &(scheduleVariables[variableName]);
            ILAnalyzeScheduleVariable* variable = getVariableInDomain(variableList, node->domain);
            if(!variable) {
                return -ILAnalyzeSchedule::ErrorInternal;
            }
            node->variableStored.push_back(new ILAnalyzeScheduleNodeVariableRef(node, nullptr, variable, nullptr));
	    }
        Expression* exp = statement->getValueExpression();
        if(exp && (res = collectScheduleNodeExpressionVariable(exp, node, false)) < 0) {
            return res;
        }
	    return 1;
    }
    return 0;
}

ILAnalyzeScheduleVariable* ILAnalyzeSchedule::getVariableByExpressionAndDomain(Expression* expression,
                                                                                  const std::vector<void*> &domain, Expression* &passExpression)
{
    if(expression->type == Token::Variable) {
        string name = expression->expressionStr;
        if (scheduleVariables.find(name) == scheduleVariables.end()) {
            return nullptr;
        }
        return getVariableInDomain(&(scheduleVariables[name]), domain);
    } else if (expression->type == Token::Point || expression->type == Token::Arrow || expression->type == Token::ArrayIndex) {
        ILAnalyzeScheduleVariable* var = getVariableByExpressionAndDomain(expression->childExpressions[0], domain, passExpression);
        if (!var) {
            return var;
        } else if(passExpression) {
            return var;
        }
        string memberName;
        if (expression->type == Token::Point || expression->type == Token::Arrow) {
            memberName = expression->childExpressions[1]->expressionStr;
        } else if(expression->type == Token::ArrayIndex && expression->childExpressions[1]->childExpressions[0]->type == Token::Number) {
            memberName = expression->childExpressions[1]->expressionStr;
        } else {
            passExpression = expression->childExpressions[0];
            return var;
        }
        return getVariableMember(var, memberName);
    } else if(expression->type == Token::Bracket) {
        return getVariableByExpressionAndDomain(expression->childExpressions[0], domain, passExpression);
    } else if(expression->type == Token::Function) {
        return nullptr;
    } else if(expression->type == Token::TypeConvert) {
        return getVariableByExpressionAndDomain(expression->childExpressions[1], domain, passExpression);
    } else {
        ILAnalyzeScheduleVariable* var = getVariableByExpressionAndDomain(expression->childExpressions[0], domain, passExpression);
        passExpression = expression->childExpressions[0];
        return var;
    }
}


ILAnalyzeScheduleVariable* ILAnalyzeSchedule::getVariableMember(ILAnalyzeScheduleVariable* variable, string memberName) const
{
    if(variable->members.find(memberName) != variable->members.end())
    {
        return variable->members[memberName];
    }
    ILAnalyzeScheduleVariable* var = nullptr;
    if(memberName[0] == '[')
    {
        int indexNum = stringToInt(memberName.substr(1, memberName.length() - CONST_NUM_2));
        if(!variable->arraySize.empty() && variable->arraySize[0] > indexNum)
        {
            var = new ILAnalyzeScheduleVariable();
            var->type = variable->type;
            var->isAddress = variable->isAddress;
            var->arraySize = variable->arraySize;
            var->arraySize.erase(var->arraySize.begin(), var->arraySize.begin() + 1);
            var->callName = variable->callName + memberName;
        }
        else if(variable->isAddress > 0 && variable->arraySize.empty())
        {
            var = new ILAnalyzeScheduleVariable();
            var->type = variable->type;
            var->isAddress = var->isAddress - 1;
            var->arraySize = variable->arraySize;
            var->callName = variable->callName + memberName;
        }
        else
        {
            return nullptr;
        }
    }
    else
    {
        ILStructDefine* structType = ILParser::findStructType(variable->type);
        if(!structType)
            return nullptr;
        ILMember* structMember = structType->getMemberByName(memberName);
        if(!structMember)
            return nullptr;
        var = new ILAnalyzeScheduleVariable();
        var->type = structMember->type;
        var->isAddress = structMember->isAddress;
        var->arraySize = structMember->arraySize;
        var->callName = variable->callName + "." + memberName;
    }
    var->variableType = variable->variableType;
	var->statement = variable->statement;
    var->iLCalculate = variable->iLCalculate;
    var->name = memberName;
    var->domain = variable->domain;
    var->parent = variable;
    variable->members[memberName] = var;
    return var;
}

bool ILAnalyzeSchedule::isSameDomain(const std::vector<void*>& domain1, const std::vector<void*>& domain2) const
{
    if(domain1.size() != domain2.size())
        return false;
    for (int i = 0; i < domain1.size(); i++)
    {
        if(domain1[i] != domain2[i])
            return false;
    }
    return true;
}

bool ILAnalyzeSchedule::isDomainContain(const std::vector<void*>& domainSrc,
    const std::vector<void*>& domainDst) const
{
    if(domainDst.size() > domainSrc.size())
        return false;
    for(int i = 0; i < domainDst.size(); i++)
    {
        if(domainDst[i] != domainSrc[i])
            return false;
    }
    return true;
}

ILAnalyzeScheduleVariable* ILAnalyzeSchedule::getVariableInDomain(
    const std::vector<ILAnalyzeScheduleVariable*>* variableList, const std::vector<void*> &domain) const
{
    vector<void*> curDomain = domain;
    ILAnalyzeScheduleVariable* var = nullptr;
    while(variableList && !var) {
        for(int i = 0; i < variableList->size(); i++)
        {
            if (isSameDomain(curDomain, ((*variableList)[i])->domain))
            {
                var = (*variableList)[i];
                break;
            }
        }
        if(curDomain.empty())
            break;
        curDomain.erase(curDomain.end() - 1, curDomain.end());
    }
    return var;
}

ILAnalyzeScheduleVariable* ILAnalyzeSchedule::getVariableInDomain(std::string variableName,
    const std::vector<void*>& domain) const
{
    if(scheduleVariables.find(variableName) == scheduleVariables.end())
        return nullptr;
    vector<void*> curDomain = domain;
    ILAnalyzeScheduleVariable* var = nullptr;
    while(!curDomain.empty() && !var) {
        for(int i = 0; i < scheduleVariables.at(variableName).size(); i++)
        {
            if (isSameDomain(curDomain, (scheduleVariables.at(variableName)[i])->domain))
            {
                var = scheduleVariables.at(variableName)[i];
                break;
            }
        }
        curDomain.erase(curDomain.end() - 1, curDomain.end());
    }
    return var;
}

ILAnalyzeScheduleVariableType ILAnalyzeSchedule::getVariableExpressionType(const Expression* expression, const std::vector<void*> &domain)
{
    if(expression->type == Token::Variable)
    {
        string name = expression->expressionStr;

        if (scheduleVariables.find(name) == scheduleVariables.end())
            return ILAnalyzeScheduleVariableType();
        
        ILAnalyzeScheduleVariable* var = getVariableInDomain(&(scheduleVariables[name]), domain);
        if (!var)
            return ILAnalyzeScheduleVariableType();

        return ILAnalyzeScheduleVariableType(var->name, var->type, var->isAddress, var->arraySize);
    }
    else if(expression->type == Token::Point || expression->type == Token::Arrow || expression->type == Token::ArrayIndex)
    {
        ILAnalyzeScheduleVariableType type = getVariableExpressionType(expression->childExpressions[0], domain);
        if (type.type.empty())
            return ILAnalyzeScheduleVariableType();
        
        string memberName;
        if (expression->type == Token::Point || expression->type == Token::Arrow)
        {
            memberName = expression->childExpressions[1]->expressionStr;
        }
        else if(expression->type == Token::ArrayIndex && expression->childExpressions[1]->childExpressions[0]->type == Token::Number)
        {
            memberName = expression->childExpressions[1]->expressionStr;
        }
        else{
            memberName = "[0]";
        }
        return getVariableMemberType(&type, memberName);
    }
    else if(expression->type == Token::Number)
    {
        return ILAnalyzeScheduleVariableType(expression->expressionStr, "num", false, vector<int>());
    }
    else if(expression->type == Token::Word)
    {
        return ILAnalyzeScheduleVariableType(expression->expressionStr, "word", false, vector<int>());
    }
    else if(expression->type == Token::String)
    {
        return ILAnalyzeScheduleVariableType(expression->expressionStr, "string", false, vector<int>());
    }
    else if(expression->type == Token::Char)
    {
        return ILAnalyzeScheduleVariableType(expression->expressionStr, "char", false, vector<int>());
    }
    else if(expression->type == Token::TypeConvert)
    {
        string dataTypeStr = expression->childExpressions[0]->expressionStr;
        ILAnalyzeScheduleVariableType ret("", "", false, vector<int>());
        while(dataTypeStr[dataTypeStr.size() - 1] == '*')
        {
            ret.isAddress++;
            dataTypeStr = dataTypeStr.substr(0, dataTypeStr.size() - 1);
        }
        return ret;
    }
    else
    {
        return getVariableExpressionType(expression->childExpressions[0], domain);
    }
}

ILAnalyzeScheduleVariableType ILAnalyzeSchedule::getVariableMemberType(
    ILAnalyzeScheduleVariableType* variableType, std::string memberName) const
{
    if(memberName[0] == '[')
    {
        int indexNum = stringToInt(memberName.substr(1, memberName.length() - CONST_NUM_2));
        if(variableType->arraySize.empty() || variableType->arraySize[0] <= indexNum)
            return ILAnalyzeScheduleVariableType();
        
        ILAnalyzeScheduleVariableType ret = ILAnalyzeScheduleVariableType(variableType->name, variableType->type, variableType->isAddress, variableType->arraySize);
        ret.arraySize.erase(ret.arraySize.begin(), ret.arraySize.begin() + 1);
        return ret;
    }
    else
    {
        ILStructDefine* structType = ILParser::findStructType(variableType->type);
        if(!structType)
            return ILAnalyzeScheduleVariableType();
        ILMember* structMember = structType->getMemberByName(memberName);
        if(!structMember)
            return ILAnalyzeScheduleVariableType();

        return ILAnalyzeScheduleVariableType(structMember->name, structMember->type, structMember->isAddress, structMember->arraySize);
    }
}

bool ILAnalyzeSchedule::isExpressionInFunction(const Expression* expression) const
{
    if(!expression)
        return false;
    Expression* p = expression->parentExpression;
    while(p)
    {
        if(p->type == Token::Function)
        {
            return true;
        }
        p = p->parentExpression;
    }
    return false;
}

bool ILAnalyzeSchedule::isExpressionContainFunction(const Expression* expression) const
{
    if(!expression)
        return false;
    if(expression->type == Token::Function)
        return true;
    for(int i = 0; i <expression->childExpressions.size(); i++)
    {
        if(isExpressionContainFunction(expression->childExpressions[i]))
            return true;
    }
    return false;
}

Expression* ILAnalyzeSchedule::addTypeConvertToExpression(Expression* expression, string type, const std::vector<void*> &domain)
{
    type = ILParser::convertDataType(type);
    bool isAddress;
    string expType = getExpressionBasicType(expression, domain, isAddress);
    expType = ILParser::convertDataType(expType);
    if(expType == type) {
        return expression->clone();
    }
    
    if(expression->parentExpression){
        vector<Expression*>* childExpList = &(expression->parentExpression->childExpressions);
        auto iter = std::find(childExpList->begin(), childExpList->end(), expression);
		expression->parentExpression->childExpressions.erase(iter);
	}

    Expression* typeConvertExp = new Expression(false, Token::TypeConvert, "(", nullptr);
    
    Expression* typeExp = new Expression(false, Token::DataType, type, typeConvertExp);
    typeExp->expressionStr = type;

    Expression* typeConvertTargetExp = new Expression(true, Token::Bracket, "(", typeConvertExp);
    Expression* expOri = expression->clone();
    expOri->parentExpression = typeConvertTargetExp;
    typeConvertTargetExp->childExpressions.push_back(expOri);

    typeConvertExp->childExpressions.push_back(typeExp);
    typeConvertExp->childExpressions.push_back(typeConvertTargetExp);
    
    typeConvertExp->updateExpressionStr();

    return typeConvertExp;
}


std::string ILAnalyzeSchedule::getExpressionBasicType(Expression* expression, const std::vector<void*>& domain, bool &isAddress)
{
    string type1;
    string type2;
    bool isAddress1 = false;
    bool isAddress2 = false;
    if(BasicTypeCalculator::expressionBasicTypeMap.find(expression->type) != BasicTypeCalculator::expressionBasicTypeMap.end()) {
        return BasicTypeCalculator::expressionBasicTypeMap.at(expression->type);
    } else if(expression->type == Token::Variable) {
        string name = expression->expressionStr;
        if (scheduleVariables.find(name) == scheduleVariables.end())
            return "not basic type";
        
        ILAnalyzeScheduleVariable* var = getVariableInDomain(&(scheduleVariables[name]), domain);
        if (!var || !ILParser::isBasicType(var->type)) // || var->isAddress > 0 || !var->arraySize.empty()
            return "not basic type";
        if(var->isAddress > 0 || !var->arraySize.empty())
            isAddress = true;
        return ILParser::getRealDataType(var->type);
    } else if(expression->type == Token::Point || expression->type == Token::Arrow || expression->type == Token::ArrayIndex) {
        ILAnalyzeScheduleVariableType type = getVariableExpressionType(expression, domain);
        if (type.type.empty() || !ILParser::isBasicType(type.type)) // || type.isAddress > 0 || !type.arraySize.empty()
            return "not basic type";
        if(type.isAddress > 0 || !type.arraySize.empty())
            isAddress = true;
        return ILParser::getRealDataType(type.type);
    } else if(expression->type >= Token::SelfAdd && expression->type <= Token::SelfSub ||
        expression->type >= Token::SelfAddLeft && expression->type <= Token::BitNot) {
        type1 = getExpressionBasicType(expression->childExpressions[0], domain, isAddress1);
    } else if(expression->type >= Token::Mul && expression->type <= Token::Or ||
        expression->type >= Token::Assign && expression->type <= Token::OrAssign) {
        type1 = getExpressionBasicType(expression->childExpressions[0], domain, isAddress1);
        type2 = getExpressionBasicType(expression->childExpressions[1], domain, isAddress2);
    } else if(expression->type == Token::TernaryConditional) {
        type1 = getExpressionBasicType(expression->childExpressions[1]->childExpressions[0], domain, isAddress1);
        type2 = getExpressionBasicType(expression->childExpressions[1]->childExpressions[1], domain, isAddress2);
    } else if(expression->type == Token::Bracket || expression->type == Token::TypeConvert) {
        return getExpressionBasicType(expression->childExpressions[0], domain, isAddress);
    } else if(expression->type == Token::DataType) {
        string realTypeExpStr = expression->expressionStr;
        while(stringEndsWith(realTypeExpStr, "*"))
        {
            isAddress = true;
            realTypeExpStr = realTypeExpStr.substr(0, realTypeExpStr.length() - 1);
        }
        return ILParser::isBasicType(realTypeExpStr) ? realTypeExpStr : "not basic type";
    } else if(stringToLower(expression->expressionStr) == "true" || stringToLower(expression->expressionStr) == "false") {
        return "bool";
    } else {
        return "not basic type";
    }
    string retType = BasicTypeCalculator::getExpResultType(type1, type2, expression->type);
    if((expression->type == Token::Add || expression->type == Token::AddAssign) && isAddress1)
        isAddress = true;
    if(ILParser::isBasicType(retType))
        return retType;

    return "not basic type";
}

void ILAnalyzeSchedule::print()
{
    ILFunction* function = static_cast<ILFunction*>(schedule->parent);
    cout << endl;
    cout << "/*****ScheduleOptimizer*****\\" << endl;
    cout << "Function: " << function->name << endl << endl;

    printVariables();


    cout << endl << " -- ScheduleNodes -- " << endl << endl;

    for (int i = 0; i < scheduleNodes.size(); i++)
    {
        ILAnalyzeScheduleNode* node = scheduleNodes[i];
        printNode(node);
    }
    
    cout << endl;
    cout << "\\*****ScheduleOptimizer*****/" << endl << endl;
}

void ILAnalyzeSchedule::printNodeList(vector<ILAnalyzeScheduleNode*>& nodeList)
{
    cout << endl << " -- ScheduleNodes -- " << endl << endl;

    for (int i = 0; i < nodeList.size(); i++)
    {
        ILAnalyzeScheduleNode* node = nodeList[i];
        printNode(node);
    }

    cout << endl;
}

void ILAnalyzeSchedule::printVariables()
{
    auto iter = scheduleVariables.begin();
    for(; iter != scheduleVariables.end(); iter++)
    {
        for(int i = 0; i < iter->second.size(); i++)
        {
            ILAnalyzeScheduleVariable* var = iter->second[i];
            if(var->variableType == ILAnalyzeScheduleVariable::TypeGlobalVariable)
            {
                cout << "GlobalVar: " << var->name << endl;
            }
        }
    }

    iter = scheduleVariables.begin();
    for(; iter != scheduleVariables.end(); iter++)
    {
        for(int i = 0; i < iter->second.size(); i++)
        {
            ILAnalyzeScheduleVariable* var = iter->second[i];
            if(var->variableType == ILAnalyzeScheduleVariable::TypeInput)
            {
                cout << "Input Var: " << var->name << endl;
            }
        }
    }

    iter = scheduleVariables.begin();
    for(; iter != scheduleVariables.end(); iter++)
    {
        for(int i = 0; i < iter->second.size(); i++)
        {
            ILAnalyzeScheduleVariable* var = iter->second[i];
            if(var->variableType == ILAnalyzeScheduleVariable::TypeOutput)
            {
                cout << "OutputVar: " << var->name << endl;
            }
        }
    }

    iter = scheduleVariables.begin();
    for(; iter != scheduleVariables.end(); iter++)
    {
        for(int i = 0; i < iter->second.size(); i++)
        {
            ILAnalyzeScheduleVariable* var = iter->second[i];
            if(var->variableType == ILAnalyzeScheduleVariable::TypeLocalVariable)
            {
                cout << "Local Var: " << var->name << endl;
            }
        }
    }

    iter = scheduleVariables.begin();
    for(; iter != scheduleVariables.end(); iter++)
    {
        for(int i = 0; i < iter->second.size(); i++)
        {
            ILAnalyzeScheduleVariable* var = iter->second[i];
            if(var->variableType == ILAnalyzeScheduleVariable::TypeStatementVariable)
            {
                cout << "Stmt  Var: " << var->name << endl;
            }
        }
    }
}

void ILAnalyzeSchedule::printNode(const ILAnalyzeScheduleNode* node)
{
    cout << stringFillBlank(to_string(node->id), CONST_NUM_4) << "; ";
    string padPrefixStr = "    \033[32m";
    string padStr = "    ";
    string padSuffixStr = "    \033[0m";
    if (node->nodeType == ILAnalyzeScheduleNode::TypeGlobalVariable)
    {
        cout << "GlobalVar: " << node->variableStored[0]->variable->name << endl;
    }
    else if (node->nodeType == ILAnalyzeScheduleNode::TypeInput)
    {
        cout << "Input Var: " << node->variableStored[0]->variable->name << endl;
    }
    else if (node->nodeType == ILAnalyzeScheduleNode::TypeOutput)
    {
        cout << "OutputVar: " << node->variableStored[0]->variable->name << endl;
    }
    else if (node->nodeType == ILAnalyzeScheduleNode::TypeLocalVariable)
    {
        printNodeLocalVariable(node);
    }
    else if (node->nodeType == ILAnalyzeScheduleNode::TypeBranch)
    {
        printNodeBranch(node);
    }
    else if (node->nodeType == ILAnalyzeScheduleNode::TypeStatement)
    {
        printNodeStatement(node);
    }
}

void ILAnalyzeSchedule::printNodeLocalVariable(const ILAnalyzeScheduleNode* node)
{
    string padPrefixStr = "    \033[32m";
    string padStr = "    ";
    string padSuffixStr = "    \033[0m";
    ILLocalVariable* localVariable = node->variableStored[0]->variable->iLLocalVariable;
    string codeStr;
    if(iLParser->findStructType(localVariable->type))
		codeStr += "struct " + localVariable->type;
	else
		codeStr += localVariable->type;

	if(localVariable->isAddress > 0)
		codeStr += " " + stringRepeat("*", localVariable->isAddress);
	codeStr += " " + localVariable->name;
	if(!localVariable->arraySize.empty())
	{
		for(int i = 0;i < localVariable->arraySize.size(); i++)
		{
			codeStr += "[" + to_string(localVariable->arraySize[i]) + "]";
		}
	}
	if(localVariable->defaultValue)
	{
		codeStr += " = " + localVariable->defaultValue->expressionStr;
	}
	
	codeStr += ";";
    
    cout << "Local Var: " << codeStr << endl;
    
    for(int j = 0; j < node->variableStored.size(); j++)
    {
        cout << padPrefixStr;
        for(int k = 0; k < node->domain.size(); k++)
            cout << padStr;
        cout << "Store : " << node->variableStored[j]->variable->callName;
        printNodeVariableExp(node->variableStored[j]);
        cout << padSuffixStr << endl;
    }
    for(int j = 0; j < node->variableRead.size(); j++)
    {
        cout << padPrefixStr;
        for(int k = 0; k < node->domain.size(); k++)
            cout << padStr;
        cout << "Read  : " << node->variableRead[j]->variable->callName;
        printNodeVariableExp(node->variableRead[j]);
        cout << padSuffixStr << endl;
    }
}

void ILAnalyzeSchedule::printNodeBranch(const ILAnalyzeScheduleNode* node)
{
    string padPrefixStr = "    \033[32m";
    string padStr = "    ";
    string padSuffixStr = "    \033[0m";

    cout << "L:" << stringFillBlank(to_string(node->depth), 3);
    for(int j = 0; j < node->domain.size() - CONST_NUM_3; j++)
        cout << "    ";

    if(node->iLBranch->type == ILBranch::TypeIf)
    {
        cout << "if (" << node->iLBranch->condition->expressionStr << ")" << endl;
    }
    else if(node->iLBranch->type == ILBranch::TypeElse)
    {
        cout << "else" << endl;
    }
    else if(node->iLBranch->type == ILBranch::TypeElseIf)
    {
        cout << "else if (" << node->iLBranch->condition->expressionStr << ")" << endl;
    }
    else if(node->iLBranch->type == ILBranch::TypeFor)
    {
        cout << "for (" << node->iLBranch->condition->expressionStr << ")" << endl;
    }
    else if(node->iLBranch->type == ILBranch::TypeWhile)
    {
        cout << "while (" << node->iLBranch->condition->expressionStr << ")" << endl;
    }

    for(int j = 0; j < node->variableStored.size(); j++)
    {
        cout << padPrefixStr;
        for(int k = 0; k < node->domain.size(); k++)
            cout << padStr;
        cout << "Store : " << node->variableStored[j]->variable->callName;
        printNodeVariableExp(node->variableStored[j]);
        cout << padSuffixStr << endl;
    }
    for(int j = 0; j < node->variableRead.size(); j++)
    {
        cout << padPrefixStr;
        for(int k = 0; k < node->domain.size(); k++)
            cout << padStr;
        cout << "Read  : " << node->variableRead[j]->variable->callName;
        printNodeVariableExp(node->variableRead[j]);
        cout << padSuffixStr << endl;
    }
}

void ILAnalyzeSchedule::printNodeStatement(const ILAnalyzeScheduleNode* node)
{
    int indentOffset = 0;
    if(node->statement->type == Statement::While || //node->statement->type == Statement::DoWhile ||
        node->statement->type == Statement::If || node->statement->type == Statement::Else ||
        node->statement->type == Statement::ElseIf || node->statement->type == Statement::For ||
        node->statement->type == Statement::Switch) {
        indentOffset = 1;
    }
    cout << "L:" << stringFillBlank(to_string(node->depth), 3);
    for(int j = 0; j < node->domain.size() - CONST_NUM_2 - indentOffset; j++)
        cout << "    ";

    if(node->statement->type == Statement::While)
    {
        cout << "while (" << node->statement->getValueExpression()->expressionStr << ")" << endl;
    }
    else if(node->statement->type == Statement::DoWhile)
    {
        cout << "do while (" << node->statement->getValueExpression()->expressionStr << ")" << endl;
    }
    else if(node->statement->type == Statement::If)
    {
        cout << "if (" << node->statement->getValueExpression()->expressionStr << ")" << endl;
    }
    else if(node->statement->type == Statement::Else)
    {
        cout << "else" << endl;
    }
    else if(node->statement->type == Statement::ElseIf)
    {
        cout << "else if (" << node->statement->getValueExpression()->expressionStr << ")" << endl;
    }
    else if(node->statement->type == Statement::For)
    {
        cout << "for ( ;" << node->statement->getValueExpression()->expressionStr << "; )" << endl;
    }
    else if(node->statement->type == Statement::Switch)
    {
        cout << "switch (" << node->statement->getValueExpression()->expressionStr << ")" << endl;
    }
    else if(node->statement->type == Statement::Case)
    {
        cout << "case " << node->statement->getValueExpression()->expressionStr << " :" << endl;
    }
    else if(node->statement->type == Statement::Default)
    {
        cout << "default :" << endl;
    }
    else if(node->statement->type == Statement::Break)
    {
        cout << "break;" << endl;
    }
    else if(node->statement->type == Statement::Return)
    {
        if(node->statement->getValueExpression())
            cout << "return " << node->statement->getValueExpression()->expressionStr << ";" << endl;
        else
            cout << "return;" << endl;
    }
    else if(node->statement->type == Statement::LocalVariable)
    {
        printNodeStatementLocalVariable(node);
    }
    else if(node->statement->type == Statement::BatchCalculation)
    {
        printNodeStatementBatchCalculation(node);
    }
    else if(node->statement->type == Statement::ExpressionWithSemicolon)
    {
        cout << node->statement->getValueExpression()->expressionStr << ";" << endl;
    }

    printNodeStatementStoreRead(node);
}

void ILAnalyzeSchedule::printNodeStatementLocalVariable(const ILAnalyzeScheduleNode* node) const
{
    string statementStr;
    const StmtLocalVariable* stmtTemp = static_cast<const StmtLocalVariable*>(node->statement);
	if(iLParser->findStructType(stmtTemp->dataType))
		statementStr += "struct " + stmtTemp->dataType;
	else
		statementStr += stmtTemp->dataType;

	if(stmtTemp->isAddress)
		statementStr += " " + stringRepeat("*", stmtTemp->isAddress);
	statementStr += " " + stmtTemp->name;
	if(!stmtTemp->arraySize.empty())
	{
		for(int i = 0;i < stmtTemp->arraySize.size(); i++)
		{
			statementStr += "[" + to_string(stmtTemp->arraySize[i]) + "]";
		}
	}
	if(stmtTemp->defaultValue)
	{
		statementStr += " = ";
		statementStr += stmtTemp->defaultValue->expressionStr;
	}
	statementStr += ";\n";
    cout << statementStr;
}

void ILAnalyzeSchedule::printNodeStatementBatchCalculation(const ILAnalyzeScheduleNode* node) const
{
    string statementStr;
    const StmtBatchCalculation* stmtTemp = static_cast<const StmtBatchCalculation*>(node->statement);
	
	statementStr += "BatchCalculation;\n";
    cout << statementStr;
}

void ILAnalyzeSchedule::printNodeStatementStoreRead(const ILAnalyzeScheduleNode* node)
{
    string padPrefixStr = "    \033[32m";
    string padStr = "    ";
    string padSuffixStr = "    \033[0m";

    int indentOffset = 0;
    if(node->statement->type == Statement::While || //node->statement->type == Statement::DoWhile ||
        node->statement->type == Statement::If || node->statement->type == Statement::Else ||
        node->statement->type == Statement::ElseIf || node->statement->type == Statement::For ||
        node->statement->type == Statement::Switch) {
        indentOffset = 1;
    }

    for(int j = 0; j < node->variableStored.size(); j++)
    {
        cout << padPrefixStr;
        for(int k = 0; k < node->domain.size() - indentOffset; k++)
            cout << padStr;
        cout << "Store : " << node->variableStored[j]->variable->callName;
        printNodeVariableExp(node->variableStored[j]);
        cout << padSuffixStr << endl;
    }
    for(int j = 0; j < node->variableRead.size(); j++)
    {
        cout << padPrefixStr;
        for(int k = 0; k < node->domain.size() - indentOffset; k++)
            cout << padStr;
        cout << "Read  : " << node->variableRead[j]->variable->callName;
        printNodeVariableExp(node->variableRead[j]);
        cout << padSuffixStr << endl;
    }
}

void ILAnalyzeSchedule::printNodeVariableExp(const ILAnalyzeScheduleNodeVariableRef* variableRef)
{
    //if(variableRef->expression)
    //    cout << " .... Exp: " << variableRef->expression->expressionStr;
    if(variableRef->expressionFull && variableRef->expressionFull != variableRef->expression)
        cout << " .... ExpFul: " << variableRef->expressionFull->expressionStr;

    if(variableRef->expressionFull)
    {
        ILAnalyzeScheduleVariableType type = getVariableExpressionType(variableRef->expressionFull, variableRef->node->domain);

        string typeStr = " <" + type.type + ",";
        if(type.isAddress > 0)
            typeStr += "Addr: "+ to_string(type.isAddress) +",";

        if(!type.arraySize.empty())
        {
            typeStr += "[";
            for(int i = 0;i < type.arraySize.size(); i++)
            {
                if(i==0)
                    typeStr += to_string(type.arraySize[i]);
                else
                    typeStr += "," + to_string(type.arraySize[i]);
            }
            typeStr += "]";
        }
        typeStr += ">";
        cout << typeStr;

        if(isExpressionInFunction(variableRef->expressionFull))
        {
            cout << " <In func>";
        }
    }

    //if(variableRef->realVariable) // variableRef->realVariable == variableRef->variableRef && 
    //    cout << " .... RealVarExp: " << variableRef->realVariable->callName;
    
}

const ILAnalyzeScheduleNode* ILAnalyzeSchedule::getScheduleNodeTopLoop(const ILAnalyzeScheduleNode* node) const
{
    const ILAnalyzeScheduleNode* topLoopNode = nullptr;

    const ILAnalyzeScheduleNode* p = node;
    while(p)
    {
        if(p->nodeType == ILAnalyzeScheduleNode::TypeBranch && (p->iLBranch->type == ILBranch::TypeFor || p->iLBranch->type == ILBranch::TypeWhile) ||
        p->nodeType == ILAnalyzeScheduleNode::TypeStatement && (p->statement->type == Statement::For || p->statement->type == Statement::While || p->statement->type == Statement::DoWhile))
        {
            topLoopNode = p;
        }
        p = p->parent;
    }
    return topLoopNode;
}




int ILAnalyzeSchedule::release()
{
    if(scheduleVariables.empty())
        return 1;
    auto iter = scheduleVariables.begin();
    for(; iter != scheduleVariables.end(); iter++)
    {
        for (int i = 0; i < iter->second.size(); i++)
        {
            iter->second[i]->release();
            delete iter->second[i];
        }
    }
    scheduleVariables.clear();

    for (int i = 0; i < scheduleNodes.size(); i++)
    {
        scheduleNodes[i]->release();
        delete scheduleNodes[i];
    }
    scheduleNodes.clear();

	return 1;
}



/*int ILOptimizeSchedule::releaseScheduleOptimizeNodeAndVariable(
    ILOptimizeScheduleNodeVariableRef* variableStore,
    ILOptimizeScheduleNodeVariableRef* variableRead)
{
    ILOptimizeScheduleNode* nodeStore = variableStore->node;
    ILOptimizeScheduleNode* nodeRead = variableRead->node;
    ILOptimizeScheduleVariable* variable = variableStore->variable;

    //// 在读节点的读变量列表中移除该变量
    //for (int i = 0; i < nodeRead->variableRead.size(); i++)
    //{
    //    if (nodeRead->variableRead[i]->variable == variable)
    //    {
    //        delete nodeRead->variableRead[i];
    //        nodeRead->variableRead.erase(nodeRead->variableRead.begin() + i);
    //        break;
    //    }
    //}

    // 释放变量
    if(nodeStore->nodeType == ILOptimizeScheduleNode::TypeLocalVariable ||
        (nodeStore->nodeType == ILOptimizeScheduleNode::TypeStatement &&
        nodeStore->statement->type == Statement::LocalVariable)) {
        string varName = variable->name;
        std::vector<ILOptimizeScheduleVariable*>* variableList = &(scheduleVariables[varName]);
        auto iterVariableList = find(variableList->begin(), variableList->end(), variable);
        if(iterVariableList == variableList->end())
            return -ILOptimizer::ErrorInternal;
        variableList->erase(iterVariableList);
        variable->release();
        delete variable;
        variable = nullptr;

        if(variableList->empty())
        {
            auto iterScheduleVariables = scheduleVariables.find(varName);
            scheduleVariables.erase(iterScheduleVariables);
        }
    }


    // 释放调度节点，并重新计算id
    int offset = 0;
    for(int i = 0; i < scheduleNodes.size() + offset; i++)
    {
        if(scheduleNodes[i - offset] == nodeStore)
        {
            scheduleNodes[i - offset]->release();
            delete scheduleNodes[i - offset];
            scheduleNodes.erase(scheduleNodes.begin() + i - offset);

            offset = 1;
        }
        else if(offset)
        {
            scheduleNodes[i - offset]->id = i - offset;
        }
    }
    return 1;
}*/

int ILAnalyzeSchedule::releaseScheduleOptimizeNodeAndVariable(
    ILAnalyzeScheduleNodeVariableRef* variableRef)
{
    const ILAnalyzeScheduleNode* node = variableRef->node;
    ILAnalyzeScheduleVariable* variable = const_cast<ILAnalyzeScheduleVariable*>(variableRef->variable);
    
    // 释放变量
    if(node->nodeType == ILAnalyzeScheduleNode::TypeLocalVariable ||
        (node->nodeType == ILAnalyzeScheduleNode::TypeStatement &&
        node->statement->type == Statement::LocalVariable)) {
        string varName = variable->name;
        std::vector<ILAnalyzeScheduleVariable*>* variableList = &(scheduleVariables[varName]);
        auto iterVariableList = find(variableList->begin(), variableList->end(), variable);
        if(iterVariableList == variableList->end())
            return -ILAnalyzeSchedule::ErrorInternal;
        variableList->erase(iterVariableList);
        variable->release();
        delete variable;
        variable = nullptr;

        if(variableList->empty())
        {
            auto iterScheduleVariables = scheduleVariables.find(varName);
            scheduleVariables.erase(iterScheduleVariables);
        }
    }


    // 释放调度节点，并重新计算id
    int offset = 0;
    for(int i = 0; i < scheduleNodes.size() + offset; i++)
    {
        if(scheduleNodes[i - offset] == node)
        {
            scheduleNodes[i - offset]->release();
            delete scheduleNodes[i - offset];
            scheduleNodes.erase(scheduleNodes.begin() + i - offset);

            offset = 1;
        }
        else if(offset)
        {
            scheduleNodes[i - offset]->id = i - offset;
        }
    }
    return 1;
}

int ILAnalyzeSchedule::releaseScheduleOptimizeNode(const ILAnalyzeScheduleNode* node)
{
    if(node->nodeType == ILAnalyzeScheduleNode::TypeLocalVariable ||
        (node->nodeType == ILAnalyzeScheduleNode::TypeStatement &&
        node->statement->type == Statement::LocalVariable)) {
        string varName = node->variableStored[0]->variable->name;
    }
    // 释放调度节点，并重新计算id
    int offset = 0;
    for(int i = 0; i < scheduleNodes.size() + offset; i++)
    {
        if(scheduleNodes[i - offset] == node)
        {
            scheduleNodes[i - offset]->release();
            delete scheduleNodes[i - offset];
            scheduleNodes.erase(scheduleNodes.begin() + i - offset);

            offset = 1;
        }
        else if(offset)
        {
            scheduleNodes[i - offset]->id = i - offset;
        }
    }
    return 1;
}


int ILAnalyzeSchedule::optimizeFindSingleAssignedNoReadStoredVariable(int findIndex, ILAnalyzeScheduleNodeVariableRef*& retVariableStore) const
{
    for(int i = findIndex; i < scheduleNodes.size(); i++)
    {
        ILAnalyzeScheduleNode* node = scheduleNodes[i];
        // 首先寻找该节点表达式中最左的被赋值变量
        ILAnalyzeScheduleNodeVariableRef* variableRef = optimizeFindAssignedVariableInNode(node);
        // 如果未找到，则找没有默认值的局部变量
        if(!variableRef) {
            if (node->nodeType == ILAnalyzeScheduleNode::TypeLocalVariable &&
                !node->iLLocalVariable->defaultValue ||
                node->nodeType == ILAnalyzeScheduleNode::TypeStatement &&
                node->statement->type == Statement::LocalVariable &&
                !node->statement->getValueExpression())
            {
                variableRef = node->variableStored[0];
            }
            else
            {
                continue;
            }
        }
        if (variableRef->variable->variableType == ILAnalyzeScheduleVariable::TypeGlobalVariable ||
            variableRef->variable->variableType == ILAnalyzeScheduleVariable::TypeOutput)
            continue;

        // 最后判断在该变量被赋值之后是否再被读写过
        if (optimizeFindVariableReadStoredCount(i, variableRef) > 0)
            continue;

        const ILAnalyzeScheduleNode* topLoopNode = getScheduleNodeTopLoop(node);
        if (topLoopNode && optimizeFindVariableReadCountInLoop(variableRef, topLoopNode) > 0)
            continue;

        
        retVariableStore = variableRef;
        return 1;
    }
    return 0;
}

int ILAnalyzeSchedule::optimizeFindSingleAssignedReadVariable(int &findIndex, ILAnalyzeScheduleNodeVariableRef* &retVariableStore, ILAnalyzeScheduleNodeVariableRef* &retVariableRead) const
{
    for(int i = findIndex; i < scheduleNodes.size(); i++)
    {
        // 首先寻找该节点表达式中最左的被赋值变量
        ILAnalyzeScheduleNodeVariableRef* variableRef = optimizeFindAssignedVariableInNode(scheduleNodes[i]);
        if (!variableRef)
            continue;
        // 然后寻找在此之后的该变量的读取
        ILAnalyzeScheduleNodeVariableRef* singleReadNodeVarRef = nullptr;
        int singleReadIndex = -1;
        int res = optimizeFindVariableSingleReadIndex(i, variableRef, singleReadIndex, singleReadNodeVarRef);
        if(res == 0 || singleReadIndex == i)
            continue;

        // 最后判断在读取之前是否被赋值过
        if (optimizeIsVariableBeAssigned(i, singleReadIndex, variableRef))
        {
            continue;
        }
        
        retVariableStore = variableRef;
        retVariableRead = singleReadNodeVarRef;

        findIndex = i;
        return 1;
    }

    return 0;
}

ILAnalyzeScheduleNodeVariableRef* ILAnalyzeSchedule::optimizeFindAssignedVariableInNode(const ILAnalyzeScheduleNode* node) const
{
    // 只优化 TypeLocalVariable
    // 只优化 TypeStatement 中的 LocalVariable 和 ExpressionWithSemicolon
    // 寻找局部变量，在写变量列表中，通过判断变量名获取
    for(int j = 0; node->nodeType == ILAnalyzeScheduleNode::TypeLocalVariable &&
        node->iLLocalVariable->defaultValue && // 局部变量必须有默认值
        j < node->variableStored.size(); 
        j++)
    {
        if(node->variableStored[j]->variable->name == node->iLLocalVariable->name)
        {
            return node->variableStored[j];
        }
    }
     // 寻找语句中的局部变量，在写变量列表中，通过判断变量名获取
    for(int j = 0; node->nodeType == ILAnalyzeScheduleNode::TypeStatement &&
        node->statement->type == Statement::LocalVariable &&
        node->statement->getValueExpression() && // 语句局部变量必须有默认值
        j < node->variableStored.size();
        j++)
    {
        StmtLocalVariable* localVariable = static_cast<StmtLocalVariable*>(node->statement);
        if(node->variableStored[j]->variable->name == localVariable->name)
        {
            return node->variableStored[j];
        }
    }
    // 寻找语句中的最左侧的被赋值的变量
    for(int j = 0; node->nodeType == ILAnalyzeScheduleNode::TypeStatement &&
        node->statement->type == Statement::ExpressionWithSemicolon &&
        j < node->variableStored.size(); j++)
    {
        if(node->variableStored[j]->expression != node->variableStored[j]->expressionFull ||
             node->variableStored[j]->variable->variableType == ILAnalyzeScheduleVariable::TypeInput ||
             node->variableStored[j]->variable->variableType == ILAnalyzeScheduleVariable::TypeOutput ||
             node->variableStored[j]->variable->variableType == ILAnalyzeScheduleVariable::TypeGlobalVariable)
        {
            continue; // 忽略被截断的变量，忽略输入输出参数和全局变量的赋值
        }
        
        int assignedCount = 0;
        const Expression* pLast = node->variableStored[j]->expressionFull;
        Expression* p = node->variableStored[j]->expressionFull->parentExpression;
        while(p) // 寻找最左侧的被赋值的变量
        {
            if(pLast != p->childExpressions[0])
                return nullptr; // 只寻找最左的语句，包括任何表达式符号
            else if(p->type == Token::Assign) {
                assignedCount++;
            }
            pLast = p;
            p = p->parentExpression;
        }
        if(assignedCount == 1)
        {
            return node->variableStored[j];
        }
    }
    if(node->variableStored.size() == 1 &&
        node->nodeType == ILAnalyzeScheduleNode::TypeStatement &&
        node->statement->type == Statement::BatchCalculation)
    {
        return node->variableStored[0];
    }
    return nullptr;
}

int ILAnalyzeSchedule::optimizeFindVariableSingleReadIndex(int startIndex,
    const ILAnalyzeScheduleNodeVariableRef* variableRef, int &retIndex, ILAnalyzeScheduleNodeVariableRef* &retVariableRead) const
{
    int readVarCount = 0;
    int firstReadNodeIndex = -1;
    ILAnalyzeScheduleNodeVariableRef* firstReadNodeVarRef = nullptr;
    for(int j = startIndex; j < scheduleNodes.size(); j++)
    {
        for(int k = 0; k < scheduleNodes[j]->variableRead.size(); k++)
        {
            ILAnalyzeScheduleNodeVariableRef* ref = scheduleNodes[j]->variableRead[k];
            bool isSameVariable = optimizeAreVariableCovered(variableRef, ref) ||
                optimizeAreVariableCovered(ref, variableRef);
            if(isSameVariable && readVarCount == 0)
            {
                firstReadNodeIndex = j;
                firstReadNodeVarRef = ref;
                readVarCount++;
            }
            else if(isSameVariable) //存在额外对该变量的读时，则不优化，直接跳过
            {
                return 0;
            }
        }
        for(int k = 0; j != startIndex && k < scheduleNodes[j]->variableStored.size(); k++)
        {
            ILAnalyzeScheduleNodeVariableRef* ref = scheduleNodes[j]->variableStored[k];
            bool isSameVariable = optimizeAreVariableCovered(variableRef, ref) ||
                optimizeAreVariableCovered(ref, variableRef);
            if(isSameVariable) // 存在任何对该变量的写时，则不优化，直接跳过
            {
                return 0;
            }
        }
    }
    if(readVarCount == 1)
    {
        retIndex = firstReadNodeIndex;
        retVariableRead = firstReadNodeVarRef;
        return 1;
    }
    return 0;
}

int ILAnalyzeSchedule::optimizeFindVariableReadStoredCount(int startIndex, const ILAnalyzeScheduleNodeVariableRef* variableRef) const
{
    int readOrStoredVarCount = 0;
    for(int j = startIndex; j < scheduleNodes.size(); j++)
    {
        for(int k = 0; k < scheduleNodes[j]->variableRead.size(); k++)
        {
            ILAnalyzeScheduleNodeVariableRef* ref = scheduleNodes[j]->variableRead[k];
            if((optimizeAreVariableCovered(variableRef, ref) ||
                optimizeAreVariableCovered(ref, variableRef))  &&
                ref != variableRef)
            {
                readOrStoredVarCount++;
            }
        }
        for(int k = 0; k < scheduleNodes[j]->variableStored.size(); k++)
        {
            ILAnalyzeScheduleNodeVariableRef* ref = scheduleNodes[j]->variableStored[k];
            if((optimizeAreVariableCovered(variableRef, ref) ||
                optimizeAreVariableCovered(ref, variableRef))  &&
                ref != variableRef)
            {
                readOrStoredVarCount++;
            }
        }
    }
    return readOrStoredVarCount;
}

int ILAnalyzeSchedule::optimizeFindVariableReadCountInLoop(const ILAnalyzeScheduleNodeVariableRef* variableRef, const ILAnalyzeScheduleNode* loopNode) const
{
    if(loopNode->children.empty())
        return 0;

    // 通过作用域大小寻找for循环的内容
    int startIndex = loopNode->id;
    int endIndex = scheduleNodes.size();
    int indentDepth = loopNode->domain.size()+1;
    for(int j = startIndex + 1; j < scheduleNodes.size(); j++)
    {
        if(scheduleNodes[j]->domain.size() >= indentDepth)
            endIndex = j;
    }
    
    int readVarCount = 0;
    for(int j = startIndex; j < endIndex; j++)
    {
        for(int k = 0; k < scheduleNodes[j]->variableRead.size(); k++)
        {
            ILAnalyzeScheduleNodeVariableRef* ref = scheduleNodes[j]->variableRead[k];
            if((optimizeAreVariableCovered(variableRef, ref) ||
                optimizeAreVariableCovered(ref, variableRef))  &&
                ref != variableRef)
            {
                readVarCount++;
            }
        }

    }
    return readVarCount;
}

bool ILAnalyzeSchedule::optimizeIsVariableBeAssigned(int startIndex, int endIndex, ILAnalyzeScheduleNodeVariableRef* variableRef) const
{
    for(int j = startIndex; j < scheduleNodes.size() && j <= endIndex; j++)
    {
        for(int k = 0; k < scheduleNodes[j]->variableStored.size(); k++)
        {
            ILAnalyzeScheduleNodeVariableRef* ref = scheduleNodes[j]->variableStored[k];
            if(ref != variableRef &&
                (optimizeAreVariableCovered(ref, variableRef) ||
                optimizeAreVariableCovered(variableRef, ref)))
            {
                return true;
            }
        }
    }
    return false;
}

// 是否variableSrc变量覆盖variableDst，比如a 覆盖 a.b
bool ILAnalyzeSchedule::optimizeAreVariableCovered(const ILAnalyzeScheduleNodeVariableRef* variableSrc,
    const ILAnalyzeScheduleNodeVariableRef* variableDst) const
{
    if(variableSrc->variable == variableDst->variable)
        return true;
    ILAnalyzeScheduleVariable* p = variableDst->variable->parent;
    while(p)
    {
        if(variableSrc->variable == p)
            return true;
        p = p->parent;
    }
    return false;
}

bool ILAnalyzeSchedule::optimizeAreVariableSourceBeAssigned(
    int startIndex, int endIndex,
    const ILAnalyzeScheduleNodeVariableRef* variableRef) const
{
    // 比如int a = b + c;  line: 1
    // 略   ...            line: xx
    // 到  print(a)        line: 9
    // 判断line: 1-9 (含1和9) 之间 b 和 c 是否被赋过值，如果没有，则可以优化
    bool sourceVariableBeAssigned = false;
    for(int i = 0; i < variableRef->node->variableRead.size(); i++)
    {
        if(optimizeIsVariableBeAssigned(startIndex, endIndex, variableRef->node->variableRead[i]))
        {
            sourceVariableBeAssigned = true;
            break;
        }
    }
    for(int i = 0; !sourceVariableBeAssigned && i < variableRef->node->variableStored.size(); i++)
    {
        if(variableRef->node->variableStored[i] == variableRef)
            continue;
        if(optimizeIsVariableBeAssigned(startIndex, endIndex, variableRef->node->variableStored[i]))
        {
            sourceVariableBeAssigned = true;
            break;
        }
    }
    return sourceVariableBeAssigned;
}

bool ILAnalyzeSchedule::optimizeAreNodeVariablesConflictInDomain(const ILAnalyzeScheduleNode* node,
    const std::vector<void*>& domain) const
{
    bool conflictVariableName = false;
    for(int i = 0; i < node->variableRead.size(); i++)
    {
        ILAnalyzeScheduleVariable* var = node->variableRead[i]->variable->getTopParent();
        if(scheduleVariables.find(var->callName) == scheduleVariables.end())
            continue;
        if(getVariableInDomain(&(scheduleVariables.at(var->callName)), domain) != var) {
            conflictVariableName = true;
            break;
        }
    }
    for(int i = 0; i < node->variableStored.size(); i++)
    {
        ILAnalyzeScheduleVariable* var = node->variableStored[i]->variable->getTopParent();
        if(scheduleVariables.find(var->callName) == scheduleVariables.end())
            continue;
        if(getVariableInDomain(&(scheduleVariables.at(var->callName)), domain) != var) {
            conflictVariableName = true;
            break;
        }
    }
    return conflictVariableName;
}

bool ILAnalyzeSchedule::optimizeIsVariableRefAddress(ILAnalyzeScheduleNodeVariableRef* variableRef)
{
    if(variableRef->node->nodeType == ILAnalyzeScheduleNode::TypeLocalVariable) {
        ILLocalVariable* iLLocalVariable = variableRef->node->iLLocalVariable;
        if(iLLocalVariable->isAddress > 0 || !iLLocalVariable->arraySize.empty())
        {
            return true;
        }
    } else if(variableRef->node->nodeType == ILAnalyzeScheduleNode::TypeStatement &&
        variableRef->node->statement->type == Statement::LocalVariable) {
        StmtLocalVariable* stmtLocalVariable = static_cast<StmtLocalVariable*>(variableRef->node->statement);
        if(stmtLocalVariable->isAddress > 0 || !stmtLocalVariable->arraySize.empty())
        {
            return true;
        }
    } else if(variableRef->node->nodeType == ILAnalyzeScheduleNode::TypeStatement &&
        variableRef->node->statement->type == Statement::ExpressionWithSemicolon) {
        ILAnalyzeScheduleVariableType assignedVariableType = getVariableExpressionType(variableRef->expressionFull, variableRef->node->domain);
        if(assignedVariableType.isAddress > 0 || !assignedVariableType.arraySize.empty())
        {
            return true;
        }
    }
    return false;
}

bool ILAnalyzeSchedule::optimizeIsScheduleNodeContainFunction(const ILAnalyzeScheduleNode* node) const
{
   if(node->nodeType == ILAnalyzeScheduleNode::TypeLocalVariable) {
        ILLocalVariable* iLLocalVariable = node->iLLocalVariable;
        return isExpressionContainFunction(iLLocalVariable->defaultValue);
    } else if(node->nodeType == ILAnalyzeScheduleNode::TypeStatement &&
        node->statement->type == Statement::LocalVariable) {
        StmtLocalVariable* stmtLocalVariable = static_cast<StmtLocalVariable*>(node->statement);
        return isExpressionContainFunction(stmtLocalVariable->defaultValue);
    } else if(node->nodeType == ILAnalyzeScheduleNode::TypeStatement &&
        node->statement->type == Statement::ExpressionWithSemicolon) {
        return isExpressionContainFunction(node->statement->getValueExpression());
    }
    return false;
}

std::vector<ILAnalyzeScheduleNode*> ILAnalyzeSchedule::getVariableSourceNodesAll(
    const ILAnalyzeScheduleNode* node) const
{
    vector<ILAnalyzeScheduleNode*> ret = getVariableSourceNodesDirect(node);

    int index = 0;
    while(index < ret.size())
    {
        vector<ILAnalyzeScheduleNode*> tempSourceNodes = getVariableSourceNodesDirect(ret[index]);
        for(int i = 0; i < tempSourceNodes.size(); i++)
        {
            if(find(ret.begin(), ret.end(), tempSourceNodes[i]) == ret.end())
            {
                ret.push_back(tempSourceNodes[i]);
            }
        }
        index++;
    }
    return ret;
}

std::vector<ILAnalyzeScheduleNode*> ILAnalyzeSchedule::getVariableSourceNodesDirect(
    const ILAnalyzeScheduleNode* node) const
{
    vector<ILAnalyzeScheduleNode*> ret;

    vector<ILAnalyzeScheduleNodeVariableRef*> allVariableRead = node->variableRead;

    
    for(int j = node->id - 1; allVariableRead.empty() && j >= 0; j--)
    {
        ILAnalyzeScheduleNode* tempNode = scheduleNodes[j];
        for(int k = 0; allVariableRead.empty() && k < tempNode->variableStored.size(); k++)
        {
            for(int i = 0; i < allVariableRead.size(); i++)
            {
                bool isSameVariable = optimizeAreVariableCovered(tempNode->variableStored[k], allVariableRead[i]) ||
                    optimizeAreVariableCovered(allVariableRead[i], tempNode->variableStored[k]);
                if(isSameVariable && find(ret.begin(), ret.end(), tempNode) == ret.end()) {
                    ret.push_back(tempNode);
                    allVariableRead.erase(allVariableRead.begin() + i);
                    break;
                }
            }
        }
    }

    return ret;
}

bool ILAnalyzeSchedule::isExpressionVarLocalVariable(const string& expression) const
{
    string expStr = getVariableExpRootVarName(expression);

    if (this->scheduleVariables.find(expStr) == this->scheduleVariables.end())
        return false;

    for (int i = 0; i < this->scheduleVariables.at(expStr).size(); i++)
    {
        ILAnalyzeScheduleVariable* variable = this->scheduleVariables.at(expStr)[i];

        if(variable->variableType == ILAnalyzeScheduleVariable::TypeLocalVariable)
            return true;
        if (variable->variableType == ILAnalyzeScheduleVariable::TypeStatementVariable)
            return true;
    }

    return false;
}

bool ILAnalyzeSchedule::isExpressionVarGlobalVariable(const string& expression) const
{
    string expStr = getVariableExpRootVarName(expression);

    if (this->scheduleVariables.find(expStr) == this->scheduleVariables.end())
        return false;

    for (int i = 0; i < this->scheduleVariables.at(expStr).size(); i++)
    {
        ILAnalyzeScheduleVariable* variable = this->scheduleVariables.at(expStr)[i];
        
        if (variable->referedGlobalVariable)
            return true;
    }

    return false;
}

bool ILAnalyzeSchedule::isExpressionVarConstant(const string& expression) const
{
    if(getExpressionVarReferedConstantValue(expression).empty())
        return false;
    return true;
}

bool ILAnalyzeSchedule::isExpressionVarParameter(const string& expression) const
{
    for (int i = 0; i < iLParser->config.parameters.size(); i++)
    {
        if (iLParser->config.parameters[i].name == expression)
            return true;
    }

    return false;
}

std::string ILAnalyzeSchedule::getExpressionVarReferedConstantValue(const string& expression) const
{

    string expStr = getVariableExpRootVarName(expression);
    if (this->scheduleVariables.find(expStr) == this->scheduleVariables.end())
        return "";

    for (int i = 0; i < this->scheduleVariables.at(expStr).size(); i++)
    {
        ILAnalyzeScheduleVariable* variable = this->scheduleVariables.at(expStr)[i];

        if (!variable->referedConstantValue.empty())
            return variable->referedConstantValue;
    }

    return "";
}
