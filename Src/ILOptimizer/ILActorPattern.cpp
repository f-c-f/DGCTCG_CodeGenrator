#include "ILActorPattern.h"

#include "../ILBasic/ILSchedule.h"
#include "../ILBasic/ILInput.h"
#include "../ILBasic/ILOutput.h"
#include "../ILBasic/ILScheduleNode.h"
#include "../ILBasic/ILBranch.h"
#include "../ILBasic/ILCalculate.h"
#include "../Common/Utility.h"
#include "../MIRBasic/MIRModel.h"

using namespace std;


int ILActorPattern::patternCount = 0;


ILActorPattern::ILActorPattern()
{
}

ILActorPattern::~ILActorPattern()
{
}

int ILActorPattern::optimize(ILSchedule* schedule, ILParser* parser, MIRModel* mirModel)
{
	return 0;
}

int ILActorPattern::collectActorInputOutputMap()
{
	int res = traverseCollectILSchedule(schedule);
	if (res < 0)
		return res;

	map<string, ILInput*>::iterator iter = nameToInputMap.begin();
	for (; iter != nameToInputMap.end(); iter++)
	{
		ILInput* input = iter->second;
		string source = input->sourceStr;
		map<string, ILOutput*>::iterator outputIter = nameToOutputMap.find(source);
		if (outputIter == nameToOutputMap.end())
			continue;
		ILOutput* output = outputIter->second;
		actorInputToOutputMap[input] = output;
		actorOutputToInputMap[output] = input;
	}
	return 1;
}

int ILActorPattern::traverseCollectILSchedule(ILSchedule* schedule)
{
	int res;
	for (int i = 0; i < schedule->scheduleNodes.size(); i++)
	{
		ILScheduleNode* scheduleNode = schedule->scheduleNodes[i];
		if (scheduleNode->objType == ILScheduleNode::TypeBranch)
		{
			res = traverseCollectILBranch(static_cast<ILBranch*>(scheduleNode));
			if (res < 0)
				return res;
		}
		else if (scheduleNode->objType == ILScheduleNode::TypeCalculate)
		{
			res = collectScheduleNode(scheduleNode);
			if (res < 0)
				return res;
		}
	}
	return 1;
}

int ILActorPattern::traverseCollectILBranch(ILBranch* branch)
{
	int res;
	for (int i = 0; i < branch->scheduleNodes.size(); i++)
	{
		ILScheduleNode* scheduleNode = branch->scheduleNodes[i];
		if (scheduleNode->objType == ILScheduleNode::TypeBranch)
		{
			res = traverseCollectILBranch(static_cast<ILBranch*>(scheduleNode));
			if (res < 0)
				return res;
		}
		else if (scheduleNode->objType == ILScheduleNode::TypeCalculate)
		{
			res = collectScheduleNode(scheduleNode);
			if (res < 0)
				return res;
		}
	}

	return 1;
}

int ILActorPattern::collectScheduleNode(ILScheduleNode* scheduleNode)
{
	if (scheduleNode->objType != ILObject::TypeCalculate)
		return 0;

	ILCalculate* calculate = static_cast<ILCalculate*>(scheduleNode);
	for (int i = 0; i < calculate->inputs.size(); i++)
	{
		ILInput* input = calculate->inputs[i];
		nameToInputMap[input->name] = input;
	}
	for (int i = 0; i < calculate->outputs.size(); i++)
	{
		ILOutput* output = calculate->outputs[i];
		nameToOutputMap[output->name] = output;
	}
	return 1;
}

std::vector<std::vector<ILRelation*>> ILActorPattern::dataflowAnalyze(ILCalculate* src, ILCalculate* dst)
{
	dataflowPaths.clear();
	lists = {};
	// 当前实现的数据流分析不具备通用性
	// 仅能分析在同一branch下的情况
	// 如果不在同一个Branch下，则不进行数据流分析
	if (src->getContainerILBranch() != dst->getContainerILBranch())
	{
		return {};
	}
	getRelationMap(src->getContainerILBranch());

	searchRelationForConnectPath(src, dst);

	return std::vector<std::vector<ILRelation*>>(dataflowPaths);
}

std::vector<ILRelation*> ILActorPattern::getRelationsInsideBranch(ILBranch* branch)
{
	// obtainRelations目前实现不具备通用性
	// 目前仅考虑了branch内部的情况
	// 没有考虑Schedule里面嵌套Branch的情况
	std::vector<ILRelation*> relations;
	for (const auto& scheduleNode : branch->scheduleNodes)
	{
		if (scheduleNode->objType == ILScheduleNode::TypeCalculate)
		{
			ILCalculate* calculate = static_cast<ILCalculate*>(scheduleNode);
			if (calculate->inputs.size() > 0)
			{
				for (auto& input : calculate->inputs)
				{
					ILOutput* output = findSourceOutport(branch, input->sourceStr);
					ILRelation* relation = new ILRelation;
					relation->dst = input;
					relation->src = output;
					// key为被连接的inport
					// 判断是否连通 采用的是从后往前遍历的方式
					// 如果 inport 被calculate的outport的连接 则连通
					// 并且由于Simulink是一对多的连接方式，因此，一个outport可以连接多个inport
					// 反之，一个inport只能跟一个outport相连
					relations.push_back(relation);
				}
			}
		}
	}

	return relations;
}

void ILActorPattern::searchRelationForConnectPath(ILCalculate* cur, ILCalculate* tar)
{
	if (!lists.empty() && lists.top()->src->parent == tar)
	{
		// 找到了一条Relation链
		vector<ILRelation*> relationLink;

		while (!lists.empty())
		{
			relationLink.push_back(lists.top());
			lists.pop();
		}

		// 把ILRelation压回栈                                                                                                                  

		for (int i = relationLink.size() - 1; i >= 0; i--)
		{
			lists.push(relationLink[i]);
		}
		dataflowPaths.push_back(relationLink);
		return;
	}

	// 递归寻找Relation
	for (auto& input : cur->inputs)
	{
		if (relations.find(input) != relations.end())
		{
			if (relations[input]->src->parent->objType == ILScheduleNode::TypeCalculate)
			{
				lists.push(relations[input]);
				searchRelationForConnectPath(
					static_cast<ILCalculate*>(relations[input]->src->parent), tar);
				lists.pop();
			}
		}
	}

	return;
}

void ILActorPattern::getRelationMap(ILBranch* branch)
{
	// obtainRelations目前实现不具备通用性
	// 目前仅考虑了branch内部的情况
	// 没有考虑Schedule里面嵌套Branch的情况
	for (const auto& scheduleNode : branch->scheduleNodes)
	{
		if (scheduleNode->objType == ILScheduleNode::TypeCalculate)
		{
			ILCalculate* calculate = static_cast<ILCalculate*>(scheduleNode);
			if (calculate->inputs.size() > 0)
			{
				for (auto& input : calculate->inputs)
				{
					ILOutput* output = findSourceOutport(branch, input->sourceStr);
					ILRelation* relation = new ILRelation;
					relation->dst = input;
					relation->src = output;
					// key为被连接的inport
					// 判断是否连通 采用的是从后往前遍历的方式
					// 如果 inport 被calculate的outport的连接 则连通
					// 并且由于Simulink是一对多的连接方式，因此，一个outport可以连接多个inport
					// 反之，一个inport只能跟一个outport相连
					relations.insert({ input,relation });
				}
			}
		}
	}
}


ILOutput* ILActorPattern::findSourceOutport(ILBranch* branch, string sourceOutport)
{

	for (const auto& scheduleNode : branch->scheduleNodes)
	{
		if (scheduleNode->objType == ILScheduleNode::TypeCalculate)
		{
			ILCalculate* calculate = static_cast<ILCalculate*>(scheduleNode);
			if (stringStartsWith(sourceOutport, calculate->name + "_"))
			{
				for (auto& output : static_cast<ILCalculate*>(scheduleNode)->outputs)
				{
					if (calculate->name + "_" + output->name == sourceOutport)
					{
						// 找到了SourceOutport
						return output;
					}
				}
			}
		}
	}

	return nullptr;
}
