#include "ILHardwareOptimizerForARM.h"
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
#include "../ILBasic/ILUtilityFunction.h"
#include "../Common/Utility.h"
#include <string>
#include <vector>
using namespace std;

int ILHardwareOptimizerForARM::optimize(ILSchedule* schedule, ILParser* parser, MIRModel* mirModel)
{
	this->schedule = schedule;
	this->iLParser = parser;
	this->mirModel = mirModel;
	int res = optimize();
	if (res < 0)
		return res;

	return 1;
}

int ILHardwareOptimizerForARM::optimize()
{
	int res;

	traverseILSchedule(this->schedule);
	return 1;
}

int ILHardwareOptimizerForARM::traverseILSchedule(ILSchedule* schedule)
{
	// 遍历ScheduleNode寻找可优化的组件
	// 目前考虑的主要是7类组件
	// 对于Simulink未支持的组件类型，目前考虑是构建对应的子系统进行处理
	// 可能不具备通用性，这点之后再说

	// ARM的优化主要集中于向量指令
	// ARMv8指令集的普通指令并不附带饱和运算的功能


	int res = 0;
	for (auto& scheduleNode : schedule->scheduleNodes)
	{
		if (scheduleNode->objType == ILScheduleNode::TypeCalculate)
		{
			ILCalculate* calculate = static_cast<ILCalculate*>(scheduleNode);
			if (calculate->refs[0]->actor == "Sum")
			{
				if (calculate->statements.childStatements[0]->type == Statement::BatchCalculation)
				{
					processBatchSum(calculate);
				}
			}
			else if (calculate->refs[0]->actor == "Product")
			{
				if (calculate->statements.childStatements[0]->type == Statement::BatchCalculation)
				{
					processBatchMul(calculate);
				}
			}
			else if (calculate->refs[0]->actor == "Abs")
			{
				if (calculate->statements.childStatements[0]->type == Statement::BatchCalculation)
				{
					processBatchABS(calculate);
				}
			}
		}

	}

	return res;
}

int ILHardwareOptimizerForARM::processBatchSum(ILCalculate* calculate)
{
	// 目前对于加法的优化主要是饱和运算
	int res = 1;
	// 需要加一个饱和判定
	auto mirActor = getMIRActorByILCalculate(calculate);
	auto saturateParameter = mirActor->getParameterByName("SaturateOnIntegerOverflow");
	ILFile* file = calculate->getContainerILFile();
	if (saturateParameter != nullptr && saturateParameter->value == "on" && (calculate->outputs[0]->type == "i32" || calculate->outputs[0]->type == "u32"))
	{
		for (auto& statement : calculate->statements.childStatements)
		{
			if (statement->type == Statement::BatchCalculation)
			{
				StmtBatchCalculation* batchStatement = static_cast<StmtBatchCalculation*>(statement);
				if (batchStatement->operationType == StmtBatchCalculation::TypeAdd)
				{
					batchStatement->operationType = StmtBatchCalculation::TypeSAdd;
				}
				else if (batchStatement->operationType == StmtBatchCalculation::TypeSub)
				{
					batchStatement->operationType = StmtBatchCalculation::TypeSSub;
				}
			}
		}
	}

	return res;
}

// 乘法需要做一部分模式匹配

int ILHardwareOptimizerForARM::processBatchMul(ILCalculate* calculate)
{
	// 目前对于乘法的优化主要是饱和运算
	int res = 1;
	// 需要加个饱和判定

	optimizeSaturatingDoublingLongMultiply(calculate);

	return res;
}

MIRActor* ILHardwareOptimizerForARM::getMIRActorByILCalculate(ILCalculate* calculate)
{
	//cout << calculate->getContainerILFunction()->name << endl;
	ILFunction* function = calculate->getContainerILFunction();
	string functionName = function->refs[0]->path;

	MIRFunction* mirFunction = this->mirModel->getFunctionByName(functionName);
	if (mirFunction == nullptr)
		return nullptr;
	if (mirFunction->type == mirFunction->TypeDataflow)
	{
		MIRFunctionDataflow* mirFunctionDataflow = static_cast<MIRFunctionDataflow*>(mirFunction);
		return mirFunctionDataflow->getActorByName(calculate->name);
	}
	return nullptr;
}

int ILHardwareOptimizerForARM::processBatchABS(ILCalculate* calculate)
{
	// 暂时不处理
	int res = 1;
	// 不需要MIR
	// 只需要修改ILTranslator的代码

	// ABS Difference
	string sourceName = calculate->inputs[0]->sourceStr.substr(0, 
		calculate->inputs[0]->sourceStr.size() - calculate->inputs[0]->name.size() - 2);

	for (auto& scheduleNode : schedule->scheduleNodes)
	{
		if (scheduleNode->objType == ILScheduleNode::TypeCalculate)
		{
			ILCalculate* targetCalculate = static_cast<ILCalculate*>(scheduleNode);
			if (targetCalculate->name == sourceName && targetCalculate->refs[0]->actor == "Sum")
			{
				int size = targetCalculate->statements.childStatements.size();
				if (targetCalculate->statements.childStatements[size - 1]->type == Statement::BatchCalculation)
				{
					// 先不考虑sum输出端口连接多个组件触发的正确性问题
					// 直接优化
					StmtBatchCalculation* batchStatement = static_cast<StmtBatchCalculation*>(targetCalculate->statements.childStatements[size - 1]);
					if (batchStatement->operationType == StmtBatchCalculation::TypeSub)
					{
						// 直接优化
						StmtBatchCalculation* batchABS = static_cast<StmtBatchCalculation*>(calculate->statements.childStatements[0]);
						batchABS->operationType = StmtBatchCalculation::TypeDAbs;
						batchABS->inputs[0] = batchStatement->inputs[0];
						StmtBatchCalculation::StmtBatchCalculationInput input2;
						input2 = batchStatement->inputs[1];
						batchABS->inputs.push_back(input2);
						// 合并之后删除原有减法运算
						targetCalculate->statements.childStatements.pop_back();
					}
				}
			}
		}
	}
	// ABS Difference

	return res;
}

int ILHardwareOptimizerForARM::optimizeSaturatingDoublingLongMultiply(ILCalculate* calculate)
{
	// 暂时不处理
	int res = 1;
	// 不需要MIR
	// 只需要修改ILTranslator的代码

	// Saturating Doubling Long Multiply
	auto mirActor = getMIRActorByILCalculate(calculate);
	// 两种情况
	// 第一种情况是直接一个product组件，其中一个端口连接了值为2的const组件
	// 并且开启了饱和运算
	// 第二种情况是两个product组件，其中一个product连接了值为2的const组件，并且开启了饱和运算
	// 搭模型需要考虑第二种情况
	auto inputsParameter = mirActor->getParameterByName("Inputs");
	if (inputsParameter != nullptr && inputsParameter->value == "***")
	{
		// 第一种情况
		// 需要判断是否开启了饱和运算
		auto saturateParameter = mirActor->getParameterByName("SaturateOnIntegerOverflow");
		if (saturateParameter != nullptr && saturateParameter->value == "on" && calculate->outputs[0]->type == "i32")
		{
			for (int i = 0; i < calculate->inputs.size(); i++)
			{
				vector<string> sourceStrs = stringSplit(calculate->inputs[i]->sourceStr, "_");
				// 遍历ILSchedule
				ILCalculate* inputCalculate = getILCalculateByName(sourceStrs[0]);
				if (inputCalculate != nullptr)
				{
					if (inputCalculate->refs[0]->actor == "Constant")
					{
						MIRActor* inputMIRActor = getMIRActorByILCalculate(inputCalculate);
						auto valueParameter = inputMIRActor->getParameterByName("Value");
						if (valueParameter != nullptr && valueParameter->value == "2")
						{
							// 可优化
							StmtBatchCalculation* batchCalculation = new StmtBatchCalculation;
							batchCalculation->operationType = StmtBatchCalculation::TypeSDMul;
							batchCalculation->dataType = "i32";
							StmtBatchCalculation::StmtBatchCalculationOutput output;
							output.name = calculate->name + "_" + calculate->outputs[0]->name;
							output.type = "i32";
							output.arraySize = calculate->outputs[0]->arraySize;
							output.defaultValue = calculate->outputs[0]->defaultValue;
							output.isAddress = calculate->outputs[0]->isAddress;
							batchCalculation->outputs.push_back(output);

							for (int j = 0; j != i && j < calculate->inputs.size(); j++)
							{
								StmtBatchCalculation::StmtBatchCalculationInput input;
								input.name = calculate->inputs[j]->sourceStr;
								input.type = calculate->inputs[j]->type;
								input.arraySize = calculate->inputs[j]->arraySize;
								input.defaultValue = calculate->inputs[j]->defaultValue;
								input.isAddress = calculate->inputs[j]->isAddress;
								batchCalculation->inputs.push_back(input);
							}
							calculate->statements.childStatements.clear();
							calculate->statements.childStatements.push_back(batchCalculation);
							return res;
						}
					}
				}
			}
		}
	}

	// 第二种情况
	if (inputsParameter == nullptr || inputsParameter->value == "**")
	{
		auto saturateParameter = mirActor->getParameterByName("SaturateOnIntegerOverflow");
		if (saturateParameter != nullptr && saturateParameter->value == "on" && calculate->outputs[0]->type == "i32")
		{
		    vector<string> input1Strs = stringSplit(calculate->inputs[0]->sourceStr, "_");
			vector<string> input2Strs = stringSplit(calculate->inputs[1]->sourceStr, "_");
			ILCalculate* input1Calculate = getILCalculateByName(input1Strs[0]);
			ILCalculate* input2Calculate = getILCalculateByName(input2Strs[0]);
			if (input1Calculate != nullptr && input2Calculate != nullptr)
			{
				if (input1Calculate->refs[0]->actor == "Product" && input2Calculate->refs[0]->actor == "Constant")
				{
					MIRActor* input1MIRActor = getMIRActorByILCalculate(input1Calculate);
					MIRActor* input2MIRActor = getMIRActorByILCalculate(input2Calculate);
					auto valueParameter = input2MIRActor->getParameterByName("Value");
					if (valueParameter != nullptr && valueParameter->value == "2")
					{
						auto input2MIRActorParameter = input1MIRActor->getParameterByName("Inputs");
						if (input2MIRActorParameter == nullptr || input2MIRActorParameter->value == "**")
						{
							// 可优化
							StmtBatchCalculation* batchCalculation = new StmtBatchCalculation;
							batchCalculation->operationType = StmtBatchCalculation::TypeSDMul;
							batchCalculation->dataType = "i32";
							StmtBatchCalculation::StmtBatchCalculationOutput output;
							output.name = calculate->name + "_" + calculate->outputs[0]->name;
							output.type = "i32";
							output.arraySize = calculate->outputs[0]->arraySize;
							output.defaultValue = calculate->outputs[0]->defaultValue;
							output.isAddress = calculate->outputs[0]->isAddress;
							batchCalculation->outputs.push_back(output);

							StmtBatchCalculation::StmtBatchCalculationInput input1 = static_cast<StmtBatchCalculation*>
								(input1Calculate->statements.childStatements[0])->inputs[0];
							StmtBatchCalculation::StmtBatchCalculationInput input2 = static_cast<StmtBatchCalculation*>
								(input1Calculate->statements.childStatements[0])->inputs[1];
							batchCalculation->inputs.push_back(input1);
							batchCalculation->inputs.push_back(input2);

							calculate->statements.childStatements.clear();
							calculate->statements.childStatements.push_back(batchCalculation);
							input1Calculate->statements.childStatements.clear();
							return res;
						}
					}
				}
				else if (input1Calculate->refs[0]->actor == "Constant" && input2Calculate->refs[0]->actor == "Product")
				{
					MIRActor* input1MIRActor = getMIRActorByILCalculate(input1Calculate);
					MIRActor* input2MIRActor = getMIRActorByILCalculate(input2Calculate);
					auto valueParameter = input1MIRActor->getParameterByName("Value");
					if (valueParameter != nullptr && valueParameter->value == "2")
					{
						auto input1MIRActorParameter = input2MIRActor->getParameterByName("Inputs");
						if (input1MIRActorParameter == nullptr || input1MIRActorParameter->value == "**")
						{
							// 可优化
							StmtBatchCalculation* batchCalculation = new StmtBatchCalculation;
							batchCalculation->operationType = StmtBatchCalculation::TypeSDMul;
							batchCalculation->dataType = "i32";
							StmtBatchCalculation::StmtBatchCalculationOutput output;
							output.name = calculate->name + "_" + calculate->outputs[0]->name;
							output.type = "i32";
							output.arraySize = calculate->outputs[0]->arraySize;
							output.defaultValue = calculate->outputs[0]->defaultValue;
							output.isAddress = calculate->outputs[0]->isAddress;
							batchCalculation->outputs.push_back(output);

							StmtBatchCalculation::StmtBatchCalculationInput input1 = static_cast<StmtBatchCalculation*>
								(input2Calculate->statements.childStatements[0])->inputs[0];
							StmtBatchCalculation::StmtBatchCalculationInput input2 = static_cast<StmtBatchCalculation*>
								(input2Calculate->statements.childStatements[0])->inputs[1];
							batchCalculation->inputs.push_back(input1);
							batchCalculation->inputs.push_back(input2);

							calculate->statements.childStatements.clear();
							calculate->statements.childStatements.push_back(batchCalculation);
							input2Calculate->statements.childStatements.clear();
							return res;
						}
					}
					
				}
			}
		}
	}
	// Saturating Doubling Long Multiply

	return res;
}


ILCalculate* ILHardwareOptimizerForARM::getILCalculateByName(string name)
{
	for (auto& scheduleNode : this->schedule->scheduleNodes)
	{
		if (scheduleNode->objType == ILScheduleNode::TypeCalculate)
		{
			ILCalculate* calculate = static_cast<ILCalculate*>(scheduleNode);
			if (calculate->name == name)
				return calculate;
		}
	}
	return nullptr;
}
