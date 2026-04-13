#include "ILHardwareOptimizerForTricore.h"
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

int ILHardwareOptimizerForTricore::optimize(ILSchedule* schedule, ILParser* parser, MIRModel* mirModel)
{
	this->schedule = schedule;
	this->iLParser = parser;
	this->mirModel = mirModel;
	int res = optimize();
	if (res < 0)
		return res;

	return 1;
}

int ILHardwareOptimizerForTricore::optimize()
{
	int res;

	traverseILSchedule(this->schedule);
	return 1;
}

int ILHardwareOptimizerForTricore::traverseILSchedule(ILSchedule* schedule)
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
				if (calculate->statements.childStatements[0]->type != Statement::BatchCalculation)
				{
					processSingleSum(calculate);
				}
				else
				{
					processBatchSum(calculate);
				}
			}
			else if (calculate->refs[0]->actor == "Product")
			{
				if (calculate->statements.childStatements[0]->type != Statement::BatchCalculation)
				{
					processSingleMul(calculate);
				}
				else
				{
					processBatchMul(calculate);
				}
			}
			else if (calculate->refs[0]->actor == "Abs")
			{
				if (calculate->statements.childStatements[0]->type == Statement::BatchCalculation)
					processBatchABS(calculate);
			}
			else if (calculate->refs[0]->actor == "Sqrt")
			{
				if (calculate->statements.childStatements[0]->type != Statement::BatchCalculation)
				{
					processSingleSQRT(calculate);
				}
				else
				{
					processBatchSQRT(calculate);
				}
			}
		}

	}

	return res;
}

int ILHardwareOptimizerForTricore::processSingleSum(ILCalculate* calculate)
{
	int res = 1;
	auto mirActor = getMIRActorByILCalculate(calculate);
	auto saturateParameter = mirActor->getParameterByName("SaturateOnIntegerOverflow");
	if (saturateParameter->value == "on" && (calculate->outputs[0]->type == "i32" || calculate->outputs[0]->type == "u32"))
	{
		// 需要进行饱和运算有关优化
		// 先处理加法 再处理减法
		// 目前不对Simulink的求和组件处理
		string operators;
		if (mirActor->getParameterByName("Inputs") == nullptr)
			operators = "++";
		else
			operators = mirActor->getParameterByName("Inputs")->value;
		vector<int> addPositions;
		vector<int> subPositions;
		for (int i = 0; i < operators.size(); i++)
		{
			if (operators[i] == '+')
				addPositions.push_back(i);
			else if (operators[i] == '-')
				subPositions.push_back(i);
		}
		int addPosition = 0;
		int subPosition = 0;

		// 先默认都是单个元素的情况
		// 不考虑数组
		string expression = "";

		// 如果是数组再实现对应的BatchCalculation吧

		bool isI32 = calculate->outputs[0]->type == "i32" ? true : false;

		// 添加UtilityFunction
		if (isI32)
		{
			ILFile* ilFile = calculate->getContainerILFile();
			ILUtilityFunction* utilityFunction_add = new ILUtilityFunction();
			utilityFunction_add->name = "adds_i";
			utilityFunction_add->parent = ilFile;
			ilFile->utilityFunctions.push_back(utilityFunction_add);

			ILUtilityFunction* utilityFunction_sub = new ILUtilityFunction();
			utilityFunction_sub->name = "subs_i";
			utilityFunction_sub->parent = ilFile;
			ilFile->utilityFunctions.push_back(utilityFunction_sub);
		}
		else
		{
			ILFile* ilFile = calculate->getContainerILFile();
			ILUtilityFunction* utilityFunction_add = new ILUtilityFunction();
			utilityFunction_add->name = "adds_u";
			utilityFunction_add->parent = ilFile;
			ilFile->utilityFunctions.push_back(utilityFunction_add);

			ILUtilityFunction* utilityFunction_sub = new ILUtilityFunction();
			utilityFunction_sub->name = "subs_u";
			utilityFunction_sub->parent = ilFile;
			ilFile->utilityFunctions.push_back(utilityFunction_sub);
		}
		// 第一次需要处理两个符号，之后只需要处理一个符号
		// 两个符号的情况 只有 ++ 或者 +-
		if (addPositions.size() >= 2)
		{
			//string expression = "";
			if (isI32)
			{
				expression += "adds_i (&" + calculate->inputs[addPositions[0]]->sourceStr
					+ ", &" + calculate->inputs[addPositions[1]]->sourceStr
					+ ", &" + calculate->name + "_" + calculate->outputs[0]->name + ");";
			}
			else
			{
				expression += "adds_u (&" + calculate->inputs[addPositions[0]]->sourceStr
					+ ", &" + calculate->inputs[addPositions[1]]->sourceStr
					+ ", &" + calculate->name + "_" + calculate->outputs[0]->name + ");";
			}
			addPosition = 2;
		}
		else
		{
			string expression = "";
			if (isI32)
			{
				expression += "subs_i (&" + calculate->inputs[addPositions[0]]->sourceStr
					+ ", &" + calculate->inputs[subPositions[0]]->sourceStr
					+ ", &" + calculate->name + "_" + calculate->outputs[0]->name + ");";
			}
			else
			{
				expression += "subs_u (&" + calculate->inputs[addPositions[0]]->sourceStr
					+ ", &" + calculate->inputs[subPositions[0]]->sourceStr
					+ ", &" + calculate->name + "_" + calculate->outputs[0]->name + ");";
			}
			addPosition = 1;
			subPosition = 1;
		}

		// 处理所有的加法
		while (addPosition < addPositions.size())
		{
			string expression = "";
			if (isI32)
			{
				expression += "adds_i (&" + calculate->name + "_" + calculate->outputs[0]->name
					+ ", &" + calculate->inputs[addPositions[addPosition]]->sourceStr
					+ ", &" + calculate->name + "_" + calculate->outputs[0]->name + ");";
			}
			else
			{
				expression += "adds_u (&" + calculate->name + "_" + calculate->outputs[0]->name
					+ ", &" + calculate->inputs[addPositions[addPosition]]->sourceStr
					+ ", &" + calculate->name + "_" + calculate->outputs[0]->name + ");";
			}
			addPosition++;
		}

		// 处理所有的减法
		while (subPosition < subPositions.size())
		{
			string expression = "";
			if (isI32)
			{
				expression += "subs_i (&" + calculate->name + "_" + calculate->outputs[0]->name
					+ ", &" + calculate->inputs[subPositions[subPosition]]->sourceStr
					+ ", &" + calculate->name + "_" + calculate->outputs[0]->name + ");";
			}
			else
			{
				expression += "subs_u (&" + calculate->name + "_" + calculate->outputs[0]->name
					+ ", &" + calculate->inputs[subPositions[subPosition]]->sourceStr
					+ ", &" + calculate->name + "_" + calculate->outputs[0]->name + ");";
			}
			subPosition++;
		}

		// 调用CCodeParser生成对应的C代码
		ILCCodeParser codeParser;
		calculate->statements.release();
		codeParser.parseCCode(expression, &calculate->statements);
	}
	return res;
}

int ILHardwareOptimizerForTricore::processBatchSum(ILCalculate* calculate)
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
					string utilityFunctionName = batchStatement->outputs[0].type == "i32" ? "adds_i" : "adds_u";
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
					string utilityFunctionName = batchStatement->outputs[0].type == "i32" ? "subs_i" : "subs_u";
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

int ILHardwareOptimizerForTricore::processSingleMul(ILCalculate* calculate)
{
	int res = 1;
	auto mirActor = getMIRActorByILCalculate(calculate);
	auto saturateParameter = mirActor->getParameterByName("SaturateOnIntegerOverflow");
	auto operators = mirActor->getParameterByName("Inputs")->value;
	bool isOptimized = true;
	for (int i = 0; i < operators.size(); i++)
	{
		if (operators[i] == '/')
		{
			isOptimized = false;
			break;
		}
	}

	if (isOptimized && saturateParameter->value == "on"
		&& calculate->outputs[0]->type == "i32")
	{
		// 需要进行饱和运算有关优化
		// 只能处理乘法
		// 不处理除法
		string expression = "";
		expression += "muls_i (&" + calculate->inputs[0]->sourceStr
			+ ", &" + calculate->inputs[1]->sourceStr
			+ ", &" + calculate->name + "_" + calculate->outputs[0]->name + ");";
		for (int i = 2; i < operators.size(); i++)
		{
			expression += "muls_i (&" + calculate->name + "_" + calculate->outputs[0]->name
				+ ", &" + calculate->inputs[i]->sourceStr
				+ ", &" + calculate->name + "_" + calculate->outputs[0]->name + ");";
		}
		// 调用CCodeParser生成对应的C代码
		ILCCodeParser codeParser;
		calculate->statements.release();
		codeParser.parseCCode(expression, &calculate->statements);
	}
	return res;
}

int ILHardwareOptimizerForTricore::processBatchMul(ILCalculate* calculate)
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
			if (utilityFunction->name == "muls_i")
			{
				isContain = true;
				break;
			}
		}
		if (!isContain)
		{
			ILUtilityFunction* utilityFunction = new ILUtilityFunction();
			utilityFunction->name = "muls_i";
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

int ILHardwareOptimizerForTricore::processBatchSQRT(ILCalculate* calculate)
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
				if (utilityFunction->name == "rqseedf")
				{
					isContain = true;
					break;
				}
			}
			if (!isContain)
			{
				ILUtilityFunction* utilityFunction = new ILUtilityFunction();
				utilityFunction->name = "rqseedf";
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
				if (utilityFunction->name == "qseedf")
				{
					isContain = true;
					break;
				}
			}
			if (!isContain)
			{
				ILUtilityFunction* utilityFunction = new ILUtilityFunction();
				utilityFunction->name = "qseedf";
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

int ILHardwareOptimizerForTricore::processSingleSQRT(ILCalculate* calculate)
{
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

		string expression = "";
		if (type->value == "rSqrt")
		{
			// 添加utilityFunction
			bool isContain = false;
			for (auto& utilityFunction : file->utilityFunctions)
			{
				if (utilityFunction->name == "qseedf")
				{
					isContain = true;
					break;
				}
			}
			if (!isContain)
			{
				ILUtilityFunction* utilityFunction = new ILUtilityFunction();
				utilityFunction->name = "qseedf";
				utilityFunction->parent = file;
				file->utilityFunctions.push_back(utilityFunction);
			}
			expression += "qseed_f (&" + calculate->inputs[0]->sourceStr
				+ ", &" + calculate->name + "_" + calculate->outputs[0]->name + ");";
		}
		else if (!type)
		{
			// 添加utilityFunction
			bool isContain = false;
			for (auto& utilityFunction : file->utilityFunctions)
			{
				if (utilityFunction->name == "rqseedf")
				{
					isContain = true;
					break;
				}
			}
			if (!isContain)
			{
				ILUtilityFunction* utilityFunction = new ILUtilityFunction();
				utilityFunction->name = "rqseedf";
				utilityFunction->parent = file;
				file->utilityFunctions.push_back(utilityFunction);
			}
			// 根据mir的结果来看 type == nullptr表示正常的SQRT运算
			expression += "rqseed_f (&" + calculate->inputs[0]->sourceStr
				+ ", &" + calculate->name + "_" + calculate->outputs[0]->name + ");";
		}
		
		// 调用CCodeParser生成对应的C代码
		ILCCodeParser codeParser;
		calculate->statements.release();
		codeParser.parseCCode(expression, &calculate->statements);
	}
	return res;
}

int ILHardwareOptimizerForTricore::processBatchABS(ILCalculate* calculate)
{
	// 暂时不处理
	int res = 1;
	// 不需要MIR
	// 只需要修改ILTranslator的代码

	// ABS Difference
	string sourceName = calculate->inputs[0]->sourceStr.substr(0,
		calculate->inputs[0]->sourceStr.size() - calculate->inputs[0]->name.size() - 2);
	ILFile* file = calculate->getContainerILFile();
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
						
						bool isContain = false;
						// 需要添加的utility function的名字
						string utilityFunctionName = batchStatement->outputs[0].type == "i32" ? "absdif_i" : "absdif_u";
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

MIRActor* ILHardwareOptimizerForTricore::getMIRActorByILCalculate(ILCalculate* calculate)
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

