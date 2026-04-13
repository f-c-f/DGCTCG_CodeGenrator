#include "ILBranchTypeOptimizer.h"

#include <algorithm>

#include "ILOptimizer.h"
#include "../ILBasic/ILExpressionParser.h"
#include "../ILBasic/ILCCodeParser.h"


#include "../ILBasic/ILSchedule.h"
#include "../ILBasic/ILScheduleNode.h"
#include "../ILBasic/ILBranch.h"
#include "../ILBasic/ILParser.h"
#include "../ILBasic/ILFile.h"
#include "../ILBasic/ILFunction.h"
#include "../ILBasic/Expression.h"


using namespace std;

int ILBranchTypeOptimizer::optimize(ILParser* parser)
{
	this->iLParser = parser;
    
	for(int i = 0; i < iLParser->files.size(); i++)
	{
		int res = traverseILFile(iLParser->files[i]);
		if(res < 0)
			return res;
	}
	
	return 1;
}


int ILBranchTypeOptimizer::traverseILFile(ILFile* file)
{
	for(int i = 0; i < file->functions.size(); i++)
	{
		int res = traverseILFunction(file->functions[i]);
		if(res < 0)
			return res;
	}
	return 1;
}

int ILBranchTypeOptimizer::traverseILFunction(ILFunction* function)
{
	int res = traverseILSchedule(function->schedule);
	if(res < 0)
		return res;
	return 1;
}

int ILBranchTypeOptimizer::traverseILSchedule(ILSchedule* schedule)
{
	for(int i = schedule->scheduleNodes.size() - 1; i >= 0; i--)
	{
		ILScheduleNode* scheduleNode = schedule->scheduleNodes[i];
		if(scheduleNode->objType == ILScheduleNode::TypeBranch)
		{
			int res = traverseILBranch(static_cast<ILBranch*>(scheduleNode));
			if(res < 0)
				return res;
		}
	}
	return 1;
}

int ILBranchTypeOptimizer::traverseILBranch(ILBranch* branch)
{

	int res;
	for(int i = branch->scheduleNodes.size() - 1; i >= 0; i--)
	{
		ILScheduleNode* scheduleNode = branch->scheduleNodes[i];
		if(scheduleNode->objType == ILScheduleNode::TypeBranch)
		{
			res = traverseILBranch(static_cast<ILBranch*>(scheduleNode));
			if(res < 0)
				return res;
		}
	}

    ILBranch* optimiezedBranch = branch;

    if(optimiezedBranch) {
        res = optimizeILBranchElseToIf(optimiezedBranch);
        if(res < 0)
            return res;
    }
    if(optimiezedBranch) {
        res = optimizeILBranchEmpty(optimiezedBranch);
	    if(res < 0)
		    return res;
    }
    if(optimiezedBranch) {
        res = optimizeILBranchElseIf(optimiezedBranch);
	    if(res < 0)
		    return res;
    }
	return 1;
}

int ILBranchTypeOptimizer::optimizeILBranchElseToIf(ILBranch* &branch)
{
    // 因为分支语句块优化是从后向前进行的，所以这里删除的分支是上一轮被优化掉的前面的ILBranch
	set<ILBranch*>::iterator iter = find(removeBranchList.begin(),removeBranchList.end(), branch);
	if(iter != removeBranchList.end()) {
		iLParser->releaseILObject(branch);
        branch = nullptr;
		removeBranchList.erase(iter);
		return 1;
	}
    
	ILObject* preObj = branch->getPreviousSameILObject();
	if(preObj && preObj->objType == ILObject::TypeBranch)
	{
	    ILBranch* branchPre = static_cast<ILBranch*>(preObj);
	    if(branch->type == ILBranch::TypeElse && branchPre->type == ILBranch::TypeIf &&
            branchPre->scheduleNodes.empty() && !branch->scheduleNodes.empty()) {
		
	        if(!branchPre->condition) {
	            return -ILOptimizer::ErrorInternal;
	        }
	        removeBranchList.insert(branchPre);

	        return optimizeILBranchChangeElseToIf(branchPre, branch);
        }
	}
    
	return 0;
}

int ILBranchTypeOptimizer::optimizeILBranchEmpty(ILBranch* &branch) const
{
    if (branch->type == ILBranch::TypeElse && branch->scheduleNodes.empty())
    {
        iLParser->releaseILObject(branch);
        branch = nullptr;
        return 1;
    }

    ILObject* nextObj = branch->getNextSameILObject();
	if(nextObj && nextObj->objType == ILObject::TypeBranch)
	{
        ILBranch* branchNext = static_cast<ILBranch*>(nextObj);
        if(branch->type == ILBranch::TypeIf && branch->scheduleNodes.empty() &&
            branchNext->type == ILBranch::TypeIf)
        {
            iLParser->releaseILObject(branch);
            branch = nullptr;
            return 1;
        }
    }
    else
    {
        if(branch->type == ILBranch::TypeIf && branch->scheduleNodes.empty())
        {
            iLParser->releaseILObject(branch);
            branch = nullptr;
            return 1;
        }
    }
    return 0;
}

int ILBranchTypeOptimizer::optimizeILBranchElseIf(ILBranch* &branch) const
{
    if (branch->type != ILBranch::TypeElse || branch->scheduleNodes.empty()) {
        return 0;
    }
    for(int i = 0; i < branch->scheduleNodes.size(); i++) {
        if(branch->scheduleNodes[i]->objType != ILObject::TypeBranch) {
            return 0;
        }
    }
    ILBranch* firstChild = static_cast<ILBranch*>(branch->scheduleNodes[0]);
    if(firstChild->type != ILBranch::TypeIf) {
        return 0;
    }
    ILBranch::BranchType lastBranchType = ILBranch::TypeIf;
    for(int i = 1; i < branch->scheduleNodes.size(); i++) {
        ILBranch* child = static_cast<ILBranch*>(branch->scheduleNodes[i]);
        if(child->type != ILBranch::TypeElseIf && child->type != ILBranch::TypeElse) {
            return 0;
        } else if(child->type == ILBranch::TypeElseIf && lastBranchType != ILBranch::TypeIf && lastBranchType != ILBranch::TypeElseIf) {
            return 0;
        } else if(child->type == ILBranch::TypeElse && lastBranchType != ILBranch::TypeIf && lastBranchType != ILBranch::TypeElseIf) {
            return 0;
        }
        lastBranchType = child->type;
    }
    firstChild->type = ILBranch::TypeElseIf;
    
    vector<ILScheduleNode*>* nodeList = nullptr;
    if(branch->parent->objType == ILObject::TypeSchedule) {
        nodeList = &(static_cast<ILSchedule*>(branch->parent)->scheduleNodes);
    } else if(branch->parent->objType == ILObject::TypeBranch) {
        nodeList = &(static_cast<ILBranch*>(branch->parent)->scheduleNodes);
    }
    if(!nodeList)
        return 0;

    auto iter = find(nodeList->begin(), nodeList->end(), branch);
    if(iter == nodeList->end())
        return 0;

    for(int i = 0; i < branch->scheduleNodes.size(); i++) {
        nodeList->insert(iter + i + 1, branch->scheduleNodes[i]);
        branch->scheduleNodes[i]->parent = branch->parent;
    }
    branch->scheduleNodes.clear();
    iLParser->releaseILObject(branch);
    branch = nullptr;
    return 1;
}

int ILBranchTypeOptimizer::optimizeILBranchChangeElseToIf(ILBranch* branchIf, ILBranch* branchElse) const
{
    branchElse->type = ILBranch::TypeIf;
    Expression* exp = nullptr;
	if(branchIf->condition->type == Token::Not) {
		if(branchIf->condition->childExpressions[0]->type == Token::Bracket) {
			exp = branchIf->condition->childExpressions[0]->childExpressions[0]->clone();
		} else {
			exp = branchIf->condition->childExpressions[0]->clone();
		}
	} else {
        exp = new Expression(true, Token::Not, "!", nullptr);
        Expression* condExp = branchIf->condition->clone();
		int opcodePrecedence1 = ILCCodeParser::getOperatorPrecedence(Token::Not);
		int opcodePrecedence2 = ILCCodeParser::getOperatorPrecedence(branchIf->condition->type);
		if(opcodePrecedence1 > opcodePrecedence2) {
            condExp->parentExpression = exp;
			exp->childExpressions.push_back(condExp);
		} else {
			Expression* expBracket = new Expression(true, Token::Bracket, "(", nullptr);
			expBracket->childExpressions.push_back(condExp);
            condExp->parentExpression = expBracket;
			exp->childExpressions.push_back(expBracket);
			expBracket->parentExpression = exp;
		}
	}
	exp->updateExpressionStr();
	branchElse->condition = exp;
    return 1;
}
