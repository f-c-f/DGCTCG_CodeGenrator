#include "ILActorPatternDataTruncator.h"
#include "../ILBasic/ILParser.h"
#include "../ILBasic/ILSchedule.h"
#include "../ILBasic/ILScheduleNode.h"
#include "../ILBasic/ILBranch.h"
#include "../ILBasic/ILCalculate.h"
#include "../ILBasic/ILRef.h"
#include "../ILBasic/ILFunction.h"
#include "../ILBasic/ILFile.h"
#include "../ILBasic/ILInput.h"
#include "../ILBasic/ILOutput.h"
#include "../ILBasic/Statement.h"
#include "../ILBasic/StmtBatchCalculation.h"
#include "../MIRBasic/MIRModel.h"
#include "../MIRBasic/MIRFunction.h"
#include "../MIRBasic/MIRFunctionDataflow.h"
#include "../ILBasic/ILInput.h"
#include "../ILBasic/ILOutput.h"
#include "../MIRBasic/MIRActor.h"
#include "../MIRBasic/MIRParameter.h"
#include "../MIRBasic/MIRInport.h"
#include "../MIRBasic/MIRRelation.h"
#include "../MIRBasic/MIROutport.h"
#include <string>
#include <vector>

using namespace std;

// 现在设计层面就没有考虑跨Schedule的情况

int ILActorPatternDataTruncator::optimize(ILSchedule* schedule, ILParser* parser, MIRModel* mirModel)
{
	this->schedule = schedule;
	this->iLParser = parser;
	this->mirModel = mirModel;
	if (mirModel == nullptr)
		// 后续加报错
		return 1;

	int res = optimize();
	if (res < 0)
		return res;
	return 1;
}

int ILActorPatternDataTruncator::optimize()
{
	int res = 0;
	res = traverseILSchedule(this->schedule);
	optimizeDataTruncators();
	return res;
}

void ILActorPatternDataTruncator::constructILRelationMap(ILFunction* ilFunction, MIRFunctionDataflow* mirFunction)
{
	this->ilRelations.clear();
	for (const auto& mirRelation : mirFunction->relations)
	{
		// 目前默认考虑MIRRelation的一对多情况
		// 即一个outport对应多个inport
		if (mirRelation->dstObjs[0]->parent->objType == MIRObject::TypeActor)
		{
			MIRActor* dstMIRActor = (MIRActor*)mirRelation->dstObjs[0]->parent;
			ILCalculate* dstILCalculate;
			if (dstMIRActor->type == "UnitDelay")
				dstILCalculate = ilFunction->getCalculateByName(dstMIRActor->name+"_Store");
			else
				dstILCalculate = ilFunction->getCalculateByName(dstMIRActor->name);
			ILInput* dstInput = nullptr;
			for (auto& input : dstILCalculate->inputs)
			{
				if (input->name == ((MIRInport*)mirRelation->dstObjs[0])->name.substr(1))
				{
					dstInput = input;
					break;
				}
			}
			for (const auto& srcObj : mirRelation->srcObjs)
			{
				if (srcObj->parent->objType == MIRObject::TypeActor)
				{
					MIRActor* srcMIRActor = (MIRActor*)srcObj->parent;
					ILCalculate* srcILCalculate;
					if (srcMIRActor->type == "UnitDelay")
					{
						srcILCalculate = ilFunction->getCalculateByName(srcMIRActor->name+"_Read");
					}else
						srcILCalculate = ilFunction->getCalculateByName(srcMIRActor->name);
					for (auto& output : srcILCalculate->outputs)
					{
						if (output->name == ((MIRInport*)srcObj)->name.substr(1))
						{
							ILRelation relation;
							relation.src = output;
							relation.dst = dstInput;
							this->ilRelations[dstInput] = relation;
						}
					}
				}
			}
		}
	}
}

int ILActorPatternDataTruncator::traverseILSchedule(ILSchedule* schedule)
{
	int res = 0;

	for (auto scheduleNode : schedule->scheduleNodes)
	{
		if (scheduleNode->objType == ILScheduleNode::TypeCalculate)
		{
			ILCalculate* calculate = static_cast<ILCalculate*> (scheduleNode);
			if (calculate->refs[0]->actor == "Selector")
			{
				dataTruncators.push_back(calculate);
			}
		}
	}
	return res;
}

int ILActorPatternDataTruncator::processSelector(ILCalculate* ilSelector, MIRActor* mirSelector)
{
	int res = 0;
	MIRParameter* indexOption = mirSelector->getParameterByName("IndexOptions");
	// Truncation Actor 现在是通过指针来确定的
	// 前面一定是Batch
	if (indexOption->value == "Starting index (dialog)" || indexOption->value == "Starting index (port)")
	{
		MIRParameter* indices = mirSelector->getParameterByName("Indices");
		// 暂时不考虑多维数组的情况
		int length = std::stoi(mirSelector->getParameterByName("OutputSizes")->value);
		std::string indexMode = mirSelector->getParameterByName("IndexMode")->value;
		int start;
		if(indexOption->value == "Starting index (dialog)")
			start = std::stoi(indices->value.substr(1, indices->value.size() - 2));
		else
		{
			MIRInport* inport = mirSelector->inports[1];
			MIRFunctionDataflow* functionContainer = (MIRFunctionDataflow*)mirSelector->parent;
			vector<MIRRelation*> relations = functionContainer->getRelationByPort(inport);
			MIRActor* srcActor = nullptr;
			if (!relations.empty())
			{
				srcActor = (MIRActor*)relations[0]->srcObjs[0]->parent;
			}
			start = srcActor == nullptr ? -1 : stoi(srcActor->getParameterByName("Value")->value);
		}
		start += indexMode == "Zero-based" ? 0 : -1;
		PortRange selectorRange;
		selectorRange.name = ilSelector->inputs[0]->name;
		selectorRange.start = start;
		selectorRange.length = length;
		actorToRange.insert({ ilSelector, {selectorRange} });
	}
	else
	{
		// 暂时不实现
	}

	return res;
}


int ILActorPatternDataTruncator::optimizeDataTruncators()
{
	int res = 0;

	// 目前仅考虑单个Branch内部的递推情况
	for (auto& dataTruncator : dataTruncators)
	{
		// Function区分Update与Init
		// File部分，因此这里直接用File的name查询
		MIRFunction* mirFunction = mirModel->getFunctionByName(dataTruncator->getContainerILFile()->name);

		// find target actor
		// 这里根据MIR找到Selector的属性
		if (mirFunction->type == mirFunction->TypeDataflow)
		{
			MIRFunctionDataflow* functionDataflow = static_cast<MIRFunctionDataflow*> (mirFunction);
			MIRActor* mirTruncator = functionDataflow->getActorByName(dataTruncator->name);
			
			// 重建IL DataflowGraph
			constructILRelationMap(dataTruncator->getContainerILFunction(), functionDataflow);

			if (dataTruncator->refs[0]->actor == "Selector")
			{
				// 暂时默认为Selector
				// dataTruncator为ILSelector
				// mirTruncator为对应的MIRActor
				processSelector(dataTruncator, mirTruncator);
				// MIRInport* inport = mirTruncator->inports[0];
				// vector<MIRRelation*> relations = functionDataflow->getRelationByPort(inport);
			}
			mappingDerivation(ilRelations[dataTruncator->inputs[0]].src, dataTruncator->inputs[0]);
		}
	}

	return res;
}

int ILActorPatternDataTruncator::processCalculate(ILCalculate* calculate)
{
	int res = 0;
	// 判断Statement是不是BatchCalculation
	// 如果是BatchCalculation，再判断是不是ArraySize
	// 然后，再对BatchCalculation加对应的Range
	PortRange range = actorToRange[calculate][0];
	for (auto& statement : calculate->statements.childStatements)
	{
		if (statement->type == Statement::BatchCalculation)
		{
			StmtBatchCalculation* batchStatement = static_cast<StmtBatchCalculation*>(statement);
			for (auto& batchInput : batchStatement->inputs)
			{
				if (!batchInput.arraySize.empty())
				{
					StmtBatchCalculation::StmtBatchCalculationRange inputRange;
					inputRange.start = range.start;
					inputRange.length = range.length;
					inputRange.index = range.index;
					batchInput.range = inputRange;
				}
			}
			for (auto& batchOutput : batchStatement->outputs)
			{
				if (!batchOutput.arraySize.empty())
				{
					StmtBatchCalculation::StmtBatchCalculationRange outputRange;
					outputRange.start = range.start;
					outputRange.length = range.length;
					outputRange.index = range.index;
					batchOutput.range = outputRange;
				}
			}
		}
	}

	return res;
}

int ILActorPatternDataTruncator::mappingDerivation(ILOutput* srcOuput, ILInput* dstInput)
{
	// 目前版本仅考虑
	// PortRange一一对应的关系
	// 即，输入 <---> 输出的数据范围是一一对应的
	int res = 0;
	ILCalculate* srcCalculate = (ILCalculate*)srcOuput->parent;
	ILCalculate* dstCalculate = (ILCalculate*)dstInput->parent;
	PortRange target;
	// 获取到目标range，以便优化
	for (const auto& range : actorToRange[dstCalculate])
	{
		if (range.name == dstInput->name)
		{
			target = range;
			break;
		}
	}
	
	// 为源Calculate设置PortRange
	target.name = srcOuput->name;
	actorToRange[srcCalculate].push_back(target);

	// 处理组件的输入输出映射
	// 现在默认一对一
	for (const auto& input : srcCalculate->inputs)
	{
		if (!input->arraySize.empty() && ilRelations.find(input) != ilRelations.end())
		{
			target.name = input->name;
			actorToRange[srcCalculate].push_back(target);
			mappingDerivation(ilRelations[input].src, input);
		}
	}

	// 递归完之后对Calculate进行处理
	// 为BatchCalculation添加Range
	processCalculate(srcCalculate);
	return res;
}
