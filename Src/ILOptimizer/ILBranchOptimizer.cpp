#include "ILBranchOptimizer.h"
#include "ILCalculateOptimizer.h"
#include "ILOptimizer.h"
#include "../ILBasic/Expression.h"


#include "../ILBasic/ILSchedule.h"
#include "../ILBasic/ILScheduleNode.h"
#include "../ILBasic/ILBranch.h"
#include "../ILBasic/ILCalculate.h"
#include "../ILBasic/ILParser.h"

using namespace std;

//comment = true;
//unordered_map<ILBranch*, bool> ILBranchOptimizer::isILBranchOptimized;

int ILBranchOptimizer::optimize(ILBranch* branch, ILParser* parser)
{
	//if(isILBranchOptimized.find(branch) != isILBranchOptimized.end())
	//	return 0;
	//isILBranchOptimized[branch] = true;

    this->iLParser = parser;
	this->branch = branch;
	int res;
	for(int i = branch->scheduleNodes.size() - 1;i >= 0; i--)
	{
		ILScheduleNode* iLScheduleNode = branch->scheduleNodes[i];
		if(iLScheduleNode->objType == ILObject::TypeCalculate)
		{
			ILCalculateOptimizer iLCalculateOptimizer;
			res = iLCalculateOptimizer.optimize(static_cast<ILCalculate*>(iLScheduleNode), iLParser);
			if(res < 0)
				return res;
		}
		else if(iLScheduleNode->objType == ILObject::TypeBranch)
		{
			ILBranchOptimizer iLBranchOptimizer;
			res = iLBranchOptimizer.optimize(static_cast<ILBranch*>(iLScheduleNode), iLParser);
			if(res < 0)
				return res;
		}
	}

	if(branch->type == ILBranch::TypeIf || branch->type == ILBranch::TypeElseIf)
	{
		ILObject* nextILObject = branch->getNextSameILObject();
		//vector<ILBranch*> iLObjectElesIfElseList;
		//while(nextILObject->objType == ILObject::TypeBranch)
		//{
		//	ILBranch* tempBranch = (ILBranch*)nextILObject;
		//	if(tempBranch->type == ILBranch::TypeElse || tempBranch->type == ILBranch::TypeElseIf)
		//	{
		//		iLObjectElesIfElseList.push_back(tempBranch);
		//		nextILObject = nextILObject->getNextSameILObject();
		//	}
		//	else
		//	{
		//		break;
		//	}
		//}
		//for(int i = iLObjectElesIfElseList.size() - 1; i >= 0; i--)
		//{
		//	ILBranchOptimizer iLBranchOptimizer;
		//	res = iLBranchOptimizer.optimize(iLObjectElesIfElseList[i], iLParser);
		//	if(res < 0)
		//		return res;
		//}
		//nextILObject = branch->getNextSameILObject();
		if(!branch->condition->isContainAssign() &&
            branch->scheduleNodes.empty() &&
			(nextILObject == nullptr ||
			nextILObject->objType != ILObject::TypeBranch ||
			nextILObject->objType == ILObject::TypeBranch && static_cast<ILBranch*>(nextILObject)->type != ILBranch::TypeElse &&  static_cast<ILBranch*>(nextILObject)->type != ILBranch::TypeElseIf))
		{
			iLParser->releaseILObject(branch);
		}
	}
	else if(branch->type == ILBranch::TypeElse || branch->type == ILBranch::TypeFor)
	{
		if(branch->scheduleNodes.empty())
		{
			iLParser->releaseILObject(branch);
		}
	}
    else if(branch->type == ILBranch::TypeWhile)
	{
		if(branch->scheduleNodes.empty() && !branch->condition->isContainAssign())
		{
			iLParser->releaseILObject(branch);
		}
	}
	else
	{
		return -ILOptimizer::ErrorInvalidILBranchType;
	}

	return 1;
}

//void ILBranchOptimizer::clear()
//{
//	isILBranchOptimized.clear();
//}



int ILBranchOptimizer::release() const
{
	return 1;
}
