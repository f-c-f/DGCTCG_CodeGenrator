#include "ILHardwareOptimizerForRISCV.h"
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
#include <string>
#include <vector>
using namespace std;

int ILHardwareOptimizerForRISCV::optimize(ILSchedule* schedule, ILParser* parser, MIRModel* mirModel)
{
	this->schedule = schedule;
	this->iLParser = parser;
	this->mirModel = mirModel;
	int res = optimize();
	if (res < 0)
		return res;

	return 1;
}

int ILHardwareOptimizerForRISCV::optimize()
{
	int res;

	traverseILSchedule(this->schedule);
	return 1;
}

int ILHardwareOptimizerForRISCV::traverseILSchedule(ILSchedule* schedule)
{
	// 遍历ScheduleNode寻找可优化的组件
	// 目前考虑的主要是7类组件
	// 对于Simulink未支持的组件类型，目前考虑是构建对应的子系统进行处理
	// 可能不具备通用性，这点之后再说
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
			//else if (calculate->refs[0]->actor == "Product")
			//{
			//	if (calculate->statements.childStatements[0]->type == Statement::BatchCalculation)
			//	{
			//		processBatchMul(calculate);
			//	}
			//}
			else if (calculate->refs[0]->actor == "Abs")
			{
				//if (calculate->statements.childStatements[0]->type == Statement::BatchCalculation)
				//	processBatchABS(calculate);
			}
			else if (calculate->refs[0]->actor == "Sqrt")
			{
				if (calculate->statements.childStatements[0]->type == Statement::BatchCalculation)
				{
					processBatchSQRT(calculate);
				}
			}
		}

	}

	return res;
}

int ILHardwareOptimizerForRISCV::processBatchSum(ILCalculate* calculate)
{
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
					// 判断是否已经添加了UtilityFunction
					bool isContain = false;
					// 需要添加的utility function的名字
					string utilityFunctionName = batchStatement->outputs[0].type == "i32" ? "vsadd_vv" : "vsadd_vv_u";
					for (auto& utilityFunction : file->utilityFunctions)
					{
						if (utilityFunction->name == utilityFunctionName)
						{
							isContain = true;
							break;
						}
					}
					if (!isContain)
					{
						ILUtilityFunction* utilityFunction = new ILUtilityFunction();
						utilityFunction->name = utilityFunctionName;
						utilityFunction->parent = file;
						file->utilityFunctions.push_back(utilityFunction);
					}
				}
				else if (batchStatement->operationType == StmtBatchCalculation::TypeSub)
				{
					batchStatement->operationType = StmtBatchCalculation::TypeSSub;
					// 判断是否已经添加了UtilityFunction
					bool isContain = false;
					// 需要添加的utility function的名字
					string utilityFunctionName = batchStatement->outputs[0].type == "i32" ? "vssub_vv" : "vssub_vv_u";
					for (auto& utilityFunction : file->utilityFunctions)
					{
						if (utilityFunction->name == utilityFunctionName)
						{
							isContain = true;
							break;
						}
					}
					if (!isContain)
					{
						ILUtilityFunction* utilityFunction = new ILUtilityFunction();
						utilityFunction->name = utilityFunctionName;
						utilityFunction->parent = file;
						file->utilityFunctions.push_back(utilityFunction);
					}
				}
			}
		}
	}

	return res;
}

int ILHardwareOptimizerForRISCV::processBatchMul(ILCalculate* calculate)
{
	int res = 1;
	// 需要加个饱和判定
	auto mirActor = getMIRActorByILCalculate(calculate);
	auto saturateParameter = mirActor->getParameterByName("SaturateOnIntegerOverflow");
	if (saturateParameter != nullptr && saturateParameter->value == "on"
		&& calculate->outputs[0]->type == "i32")
	{
		ILFile* file = calculate->getContainerILFile();
		// 添加utilityFunction
		bool isContain = false;

		for (auto& utilityFunction : file->utilityFunctions)
		{
			if (utilityFunction->name == "vsmul_vv")
			{
				isContain = true;
				break;
			}
		}
		if (!isContain)
		{
			ILUtilityFunction* utilityFunction = new ILUtilityFunction();
			utilityFunction->name = "vsmul_vv";
			utilityFunction->parent = file;
			file->utilityFunctions.push_back(utilityFunction);
		}

		for (auto& statement : calculate->statements.childStatements)
		{
			if (statement->type == Statement::BatchCalculation)
			{
				StmtBatchCalculation* batchStatement = static_cast<StmtBatchCalculation*>(statement);
				if (batchStatement->operationType == StmtBatchCalculation::TypeMul)
					batchStatement->operationType = StmtBatchCalculation::TypeSMul;
			}
		}
	}

	return res;
}

int ILHardwareOptimizerForRISCV::processBatchSQRT(ILCalculate* calculate)
{
	// 具体的实现修改ILTranslator的代码
	int res = 1;
	auto mirActor = getMIRActorByILCalculate(calculate);
	auto type = mirActor->getParameterByName("Operator");
	// Tricore仅支持单精度浮点数的开方运算
	if (calculate->outputs[0]->type == "f32")
	{
		// 需要进行饱和运算有关优化
		// 只能处理乘法
		// 不处理除法

		ILFile* file = calculate->getContainerILFile();

		if (type == nullptr)
		{
			// 添加utilityFunction
			bool isContain = false;
			for (auto& utilityFunction : file->utilityFunctions)
			{
				if (utilityFunction->name == "vfsqrt")
				{
					isContain = true;
					break;
				}
			}
			if (!isContain)
			{
				ILUtilityFunction* utilityFunction = new ILUtilityFunction();
				utilityFunction->name = "vfsqrt";
				utilityFunction->parent = file;
				file->utilityFunctions.push_back(utilityFunction);
			}

			for (auto& statement : calculate->statements.childStatements)
			{
				if (statement->type == Statement::BatchCalculation)
				{
					StmtBatchCalculation* batchStatement = static_cast<StmtBatchCalculation*>(statement);
					batchStatement->operationType = StmtBatchCalculation::TypeSqrt;
				}
			}
		}
		else if (type->value == "rSqrt")
		{
			// 添加utilityFunction
			bool isContain = false;
			for (auto& utilityFunction : file->utilityFunctions)
			{
				if (utilityFunction->name == "vfrsqrt")
				{
					isContain = true;
					break;
				}
			}
			if (!isContain)
			{
				ILUtilityFunction* utilityFunction = new ILUtilityFunction();
				utilityFunction->name = "vfrsqrt";
				utilityFunction->parent = file;
				file->utilityFunctions.push_back(utilityFunction);
			}

			for (auto& statement : calculate->statements.childStatements)
			{
				if (statement->type == Statement::BatchCalculation)
				{
					StmtBatchCalculation* batchStatement = static_cast<StmtBatchCalculation*>(statement);
					batchStatement->operationType = StmtBatchCalculation::TypeRSqrt;
				}
			}
		}
	}
	return res;
}


//int ILHardwareOptimizerForRISCV::processBatchABS(ILCalculate* calculate)
//{
//	// 暂时不处理
//	int res = 1;
//	// 不需要MIR
//	// 只需要修改ILTranslator的代码
//
//	// ABS Difference
//	string sourceName = calculate->inputs[0]->sourceStr.substr(0,
//		calculate->inputs[0]->sourceStr.size() - calculate->inputs[0]->name.size() - 2);
//	ILFile* file = calculate->getContainerILFile();
//	for (auto& scheduleNode : schedule->scheduleNodes)
//	{
//		if (scheduleNode->objType == ILScheduleNode::TypeCalculate)
//		{
//			ILCalculate* targetCalculate = static_cast<ILCalculate*>(scheduleNode);
//			if (targetCalculate->name == sourceName && targetCalculate->refs[0]->actor == "Sum")
//			{
//				int size = targetCalculate->statements.childStatements.size();
//				if (targetCalculate->statements.childStatements[size - 1]->type == Statement::BatchCalculation)
//				{
//					// 先不考虑sum输出端口连接多个组件触发的正确性问题
//					// 直接优化
//					StmtBatchCalculation* batchStatement = static_cast<StmtBatchCalculation*>(targetCalculate->statements.childStatements[size - 1]);
//					if (batchStatement->operationType == StmtBatchCalculation::TypeSub)
//					{
//						// 直接优化
//						StmtBatchCalculation* batchABS = static_cast<StmtBatchCalculation*>(calculate->statements.childStatements[0]);
//
//						bool isContain = false;
//						// 需要添加的utility function的名字
//						string utilityFunctionName = batchStatement->outputs[0].type == "i32" ? "vabsdif_vv" : "vabsdif_vv_u";
//						for (auto& utilityFunction : file->utilityFunctions)
//						{
//							if (utilityFunction->name == utilityFunctionName)
//							{
//								isContain = true;
//								break;
//							}
//						}
//						if (!isContain)
//						{
//							ILUtilityFunction* utilityFunction = new ILUtilityFunction();
//							utilityFunction->name = utilityFunctionName;
//							utilityFunction->parent = file;
//							file->utilityFunctions.push_back(utilityFunction);
//						}
//
//
//						batchABS->operationType = StmtBatchCalculation::TypeDAbs;
//						batchABS->inputs[0] = batchStatement->inputs[0];
//						StmtBatchCalculation::StmtBatchCalculationInput input2;
//						input2 = batchStatement->inputs[1];
//						batchABS->inputs.push_back(input2);
//						// 合并之后删除原有减法运算
//						targetCalculate->statements.childStatements.pop_back();
//					}
//				}
//			}
//		}
//	}
//	// ABS Difference
//
//	return res;
//}

MIRActor* ILHardwareOptimizerForRISCV::getMIRActorByILCalculate(ILCalculate* calculate)
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

