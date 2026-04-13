#include "ILTransSchedule.h"

#include "ILTransFile.h"
#include "ILTransLocalVariable.h"
#include "ILTransScheduleNode.h"
#include "ILTranslator.h"
#include "ILTranslatorForC.h"
#include  "ILTransSubsystemUtility.h"
#include "../ILBasic/ILSchedule.h"
#include "../ILBasic/ILLocalVariable.h"
#include "../ILBasic/ILCalculate.h"
#include "../ILBasic/ILOutput.h"
#include "../ILBasic/ILRef.h"
#include "../ILBasic/ILCCodeParser.h"
#include "../ILBasic/ILInput.h"
#include "../ILBasic/Statement.h"
#include "../ILBasic/Expression.h"
#include "../ILBasic/StmtExpression.h"
#include "../ILBasic/ILFunction.h"
#include "../ILBasic/ILFile.h"
#include "../ILBasic/ILStructDefine.h"
#include "../ILBasic/ILMember.h"
#include "../ILBasic/ILBranch.h"
#include "../ILBasic/ILParser.h"
#include "../ILBasic/StmtBatchCalculation.h"
#include "../ILBasic/StmtLocalVariable.h"
#include "../ILBasic/StmtIf.h"
#include "../ILBasic/StmtElseIf.h"

#include "../ILBasic/ILAnalyzeSchedule.h"

using namespace std;

int ILTransSchedule::translate(const ILSchedule* schedule, ILTransFile* file) const
{
	int res;
	ILTransLocalVariable iLTransLocalVariable;
	ILTransScheduleNode iLTransScheduleNode;


    
    ILAnalyzeSchedule* iLTranslateSchedule = nullptr;

    if (ILTranslator::translateForC)
    {
        iLTranslateSchedule = new ILAnalyzeSchedule();
        iLTranslateSchedule->collectSchedule(schedule, ILTranslatorForC::iLParser);
    }

	// 不进行变量打包，就生成局部变量
	// 进行变量打包的话，就不调用translate
	// 对于子系统的输出端口，打包参数和打包局部变量可能会冲突 之后需要进行测试
	if (!ILTranslator::translateForC || (!ILTranslatorForC::configPackageParameter && !ILTranslatorForC::configPackageVariable))
	{
		// 测试代码，不进行优化
		for (auto& localVariable : schedule->localVariables)
		{
			res = iLTransLocalVariable.translate(localVariable, file);
			if (res < 0)
				return res;
		}
		for (auto& scheduleNode : schedule->scheduleNodes)
		{
			res = iLTransScheduleNode.translate(scheduleNode, file, iLTranslateSchedule);
			if (res < 0)
				return res;
		}
	}
	else
	{
		// 找出Schedule中的子系统和复杂BatchCalculation
		// 复杂的BatchCalculation同样会被打包成子系统
		ILTransSubsystemUtility subsystemUtil(schedule);
		res = subsystemUtil.findSubsystemInSchedule();
		if (res < 0)
			return res;

		// 获取该函数的self结构体
		// 合理的编码应该是没有找到self结构体就不进行优化
		ILStructDefine* selfStruct = nullptr;
		ILFunction* function = schedule->getContainerILFunction();
		if (function->inputs.size() > 0 && function->inputs[0]->name == "self")
		{
			std::string structName = ILParser::getRealDataType(function->inputs[0]->type);
			for (auto& structDefine : file->file->structDefines)
			{
				if (structDefine->name == structName)
				{
					selfStruct = structDefine;
					break;
				}
			}
		}
		if (ILTranslatorForC::configPackageVariable)
		{
			// 如果开启了参数打包
			// 需要删除 子系统中，为了保存子系统输入而产生的局部变量
			if (ILTranslatorForC::configPackageParameter && selfStruct != nullptr)
			{
				for (auto& subSystemOutput : subsystemUtil.outputNameforLocalVariables)
				{
					for (auto& outputName : subSystemOutput.second)
					{
						for (auto it = selfStruct->members.begin(); it != selfStruct->members.end(); ++it)
						{
							if ((*it)->name == outputName)
							{
								selfStruct->members.erase(it);
								break;
							}
						}
					}
				}
			}
			
			// 如果LocalVariable具有defaulExpression
			// 需要判断是否要更新表达式
			for (auto& localVariable : schedule->localVariables)
			{
				if (localVariable->defaultValue != nullptr)
				{
					updateExpressionForPackage(schedule->getContainerILFunction(), &subsystemUtil, localVariable->defaultValue, selfStruct);
					localVariable->defaultValue->updateExpressionStr();
				}
				res = iLTransLocalVariable.translate(localVariable, file);
				if (res < 0)
					return res;
			}
		}
		else {
			// 没有开启打包变量
			// 如果开启了参数打包，则不对子系统的输出生成local variable
			if (ILTranslatorForC::configPackageParameter)
			{
				for (auto& localVariable : schedule->localVariables)
				{
					if (!subsystemUtil.isSubsystemOutput(localVariable->name))
					{
						// 如果LocalVariable具有defaultExpression
						// 需要判断是否要更新表达式
						// 需要更新表达式
						if (localVariable->defaultValue != nullptr)
						{
							updateExpressionForPackage(schedule->getContainerILFunction(), &subsystemUtil, localVariable->defaultValue, selfStruct);
							localVariable->defaultValue->updateExpressionStr();
						}

						res = iLTransLocalVariable.translate(localVariable, file);
						if (res < 0)
							return res;
					}
				}
			}
			else {
				for (auto& localVariable : schedule->localVariables)
				{
					res = iLTransLocalVariable.translate(localVariable, file);
					if (res < 0)
						return res;
				}
			}
		}


		// 翻译calculate
		for (auto& scheduleNode : schedule->scheduleNodes)
		{
			// 递归寻找Calculate优化
			res = packageCalculate(scheduleNode, &subsystemUtil, selfStruct);
			if (res < 0)
				return res;
			res = iLTransScheduleNode.translate(scheduleNode, file, iLTranslateSchedule);
			if (res < 0)
				return res;
		}
	}
	return 1;
}

int ILTransSchedule::modifyForPackage(ILCalculate* const calculate, ILTransSubsystemUtility* subsystemUtil, ILStructDefine* const structDefine) const
{
	if (structDefine == nullptr)
	{
		// 没找到self结构体 不进行参数以及变量打包
		return 1;
	}
	int res;
	auto subsystem = subsystemUtil->findCalculateInSubsystems(calculate);
	if (subsystem != subsystemUtil->subsystems.end())
	{
		// 如果开启参数打包，对子系统进行特殊处理
		if (ILTranslatorForC::configPackageParameter || ILTranslatorForC::configPackageVariable)
		{
			res = processSubsystemForPackage(calculate, subsystemUtil, structDefine);
			if (res < 0)
				return res;
		}
	}
	else {
		//for (auto& statement : calculate->statements.childStatements)
		//{
		//	if (statement->type == Statement::ExpressionWithSemicolon)
		//	{
		//		const StmtExpression* stmtTemp = static_cast<const StmtExpression*>(statement);
		//		updateExpressionForPackage(calculate->getContainerILFunction(), subsystemUtil, stmtTemp->expression, structDefine);
		//		stmtTemp->expression->updateExpressionStr();
		//	}
		//	else if (statement->type == Statement::BatchCalculation)
		//	{
		//		// 复杂类型的BatchCalculation
		//		StmtBatchCalculation* stmtTemp = static_cast<StmtBatchCalculation*>(statement);
		//		for (auto& input : stmtTemp->inputs)
		//		{
		//			// 需要判断sourceStr是否属于self结构体
		//			if (structDefine != nullptr && structDefine->getMemberByName(input.name) != nullptr)
		//			{
		//				input.name = "self->" + input.name;
		//			}
		//		}
		//		for (auto& output : stmtTemp->outputs)
		//		{
		//			// 需要判断sourceStr是否属于self结构体
		//			if (structDefine != nullptr && structDefine->getMemberByName(output.name) != nullptr)
		//			{
		//				output.name = "self->" + output.name;
		//			}
		//		}
		//	}
		//}
		processStatementForPackage(calculate->getContainerILFunction(), subsystemUtil, &(calculate->statements), structDefine);
	}
	return 1;
}

int ILTransSchedule::processSubsystemForPackage(ILCalculate* const calculate, ILTransSubsystemUtility* subsystemUtil, ILStructDefine* const structDefine) const
{
	// 这里负责处理变量打包的情况
	// 对于参数打包的情形，如果开启了参数打包了
	// 这里会对需要赋值的参数生成赋值语句
	int res;
	if (ILTranslatorForC::configPackageVariable)
	{
		// bug here
		// func
		processStatementForPackage(calculate->getContainerILFunction(), subsystemUtil, &(calculate->statements), structDefine);
	}
	if (ILTranslatorForC::configPackageParameter)
	{
		ILCCodeParser codeParser;
		std::string inputStr;
		auto subsystem = subsystemUtil->findCalculateInSubsystems(calculate);
		auto ref = std::get<1>(*subsystem);
		std::string path = ref->path;

		// 先获取到参数打包的目标函数
		ILFunction* targetFunction = nullptr;
		for (auto& file : ILTranslator::iLParser->files)
		{
			if (file->name == path)
			{
				for (auto& function : file->functions)
				{
					if (function->type == ILFunction::FunctionExec)
					{
						targetFunction = function;
						break;
					}
				}
			}
		}

		// 找到目标函数才能继续打包
		if (targetFunction != nullptr)
		{
			//  对所有的input生成statement
			for (auto& input : targetFunction->inputs)
			{
				// 需要判断sourceStr是否属于self结构体
				if (structDefine != nullptr && structDefine->getMemberByName(input->sourceStr) != nullptr)
				{
					for (auto& calculateInput : calculate->inputs)
					{
						if (calculateInput->name == input->name)
						{
							inputStr += std::get<2>(*subsystem) + "." + input->name + " = " + "self->" + calculateInput->sourceStr + ";";
							break;
						}
					}
				}
				else {
					for (auto& calculateInput : calculate->inputs)
					{
						if (calculateInput->name == input->name)
						{
							inputStr += std::get<2>(*subsystem) + "." + input->name + " = " + calculateInput->sourceStr + ";";
							break;
						}
					}
				}
			}
			Statement* statement = new Statement;
			res = codeParser.parseCCode(inputStr, statement);
			if (res < 0)
				return res;
			// 避免Statement多嵌套一层
			// 处理Statement
			calculate->statements.childStatements.insert(calculate->statements.childStatements.begin(), statement->childStatements.begin(), statement->childStatements.end());
			for (int i = 0; i < statement->childStatements.size(); i++)
			{
				calculate->statements.childStatements[i]->parentStatement = &calculate->statements;
			}

			statement->childStatements.clear();
			statement->release();
			delete statement;
		}
	}
	return 1;
}

int ILTransSchedule::packageCalculate(ILScheduleNode* const scheduleNode, ILTransSubsystemUtility* subsystemUtil, ILStructDefine* const structDefine) const
{
	// 递归遍历schedule内部节点
	// 对Calculate进行优化
	int res;
	if (scheduleNode->objType == ILObject::TypeCalculate)
	{
		//如果是Calculate，并且是子系统，需要进行处理
		ILCalculate* calculate = static_cast<ILCalculate*>(scheduleNode);
		res = modifyForPackage(calculate, subsystemUtil, structDefine);
		if (res < 0)
			return res;
	}
	else if (scheduleNode->objType == ILObject::TypeBranch)
	{
		ILBranch* branch = static_cast<ILBranch*>(scheduleNode);
		if (branch->condition != nullptr)
		{
			updateExpressionForPackage(branch->getContainerILFunction(), subsystemUtil, branch->condition, structDefine);
			branch->condition->updateExpressionStr();
		}
		for (auto& node : branch->scheduleNodes)
		{
			res = packageCalculate(node, subsystemUtil, structDefine);
			if (res < 0)
				return res;
		}
	}
	return 1;
}

void ILTransSchedule::updateExpressionForPackage(const ILFunction* function, ILTransSubsystemUtility* subsystemUtil, Expression* expression, ILStructDefine* structDefine) const
{
	// 暂时没有考虑变量的作用域，可能会存在问题
	// 不同作用域的变量可能存在名称重复
	for (auto exp : expression->childExpressions)
	{
		updateExpressionForPackage(function, subsystemUtil, exp, structDefine);
	}
	if (expression->type == Token::Variable)
	{
		if (subsystemUtil->isSubsystemOutput(expression->expressionStr) && ILTranslatorForC::configPackageParameter)
		{
			int index = 0;
			for (auto& child : expression->parentExpression->childExpressions)
			{
				if (child == expression)
				{
					break;
				}
				index++;
			}
			std::string subsystemName = subsystemUtil->getSubsystemNameByOutputName(expression->expressionStr);
			auto subsystem = subsystemUtil->getCalculateInSubsystemsByName(subsystemName);
			std::string outputName = expression->expressionStr.substr(subsystemName.size() + 1);
			Expression* point = new Expression;
			point->isExpression = true;
			point->type = Token::Point;
			point->tokenStr = ".";
			point->expressionStr = std::get<2>(*subsystem) + "." + outputName;
			Expression* self = new Expression;
			self->isExpression = true;
			self->type = Token::Variable;
			self->expressionStr = std::get<2>(*subsystem);
			self->tokenStr = self->expressionStr;
			self->parentExpression = point;
			expression->expressionStr = outputName;
			expression->tokenStr = outputName;
			point->childExpressions.push_back(self);
			point->childExpressions.push_back(expression->clone());
			point->parentExpression = expression->parentExpression;
			point->childExpressions[1]->parentExpression = point;
			point->parentExpression->childExpressions[index] = point;
			delete expression;
			expression = NULL;
		}
		else if (structDefine->getMemberByName(expression->expressionStr) != nullptr)
		{
			bool isOutput = false;
			for (auto& output : function->outputs)
			{
				if (output->name == expression->expressionStr)
				{
					isOutput = true;
					break;
				}
			}

			//Expression* arrow = new Expression;
			//arrow->isExpression = true;
			//arrow->type = Token::Arrow;
			//arrow->tokenStr = "->";
			//arrow->expressionStr = "self -> " + expression->expressionStr;
			Expression* originalExp = expression->clone();
			expression->type = Token::Arrow;
			expression->tokenStr = "->";
			expression->expressionStr = "self ->" + expression->expressionStr;
			for (int i = 0; i < expression->childExpressions.size(); i++)
			{
				delete expression->childExpressions[i];
			}
			expression->childExpressions.clear();

			Expression* self = new Expression;
			self->isExpression = true;
			self->type = Token::Variable;
			self->expressionStr = "self";
			self->tokenStr = self->expressionStr;
			self->parentExpression = expression;
			expression->childExpressions.push_back(self);
			expression->childExpressions.push_back(originalExp);
			expression->childExpressions[1]->parentExpression = expression;
			if (isOutput)
			{
				// 由于生成代码中，输出的值是通过引用传递
				// 因此处理输出端口（outport）的时候
				// 需要将*符号去掉
				if (expression->parentExpression != nullptr)
				{
					Expression* parent = expression->parentExpression;
					Expression* exp = expression->clone();
					parent->replace(exp);
					exp->release();
					delete exp;
					exp = NULL;
				}
			}
		}
	}
}

int ILTransSchedule::processStatementForPackage(const ILFunction* function, ILTransSubsystemUtility* subsystemUtil, Statement* const statement, ILStructDefine* const structDefine) const
{
	if (statement == nullptr)
		return 0;
	for (auto& childStatement : statement->childStatements)
	{
		processStatementForPackage(function, subsystemUtil, childStatement, structDefine);
	}
	if (statement->type == Statement::ExpressionWithSemicolon && !statement->innerExpressions.empty())
	{
		for (auto& expression : statement->innerExpressions)
		{
			updateExpressionForPackage(function, subsystemUtil, expression, structDefine);
			expression->updateExpressionStr();
		}
		//？？？
		// what are you doing?
		// fix bug
		// fine, go on!
		// god bless you
		// ... 886
	}
	else if (statement->type == Statement::BatchCalculation)
	{
		// 复杂类型的BatchCalculation
		StmtBatchCalculation* stmtTemp = static_cast<StmtBatchCalculation*>(statement);
		for (auto& input : stmtTemp->inputs)
		{
			// 需要判断sourceStr是否属于self结构体
			if (structDefine != nullptr && structDefine->getMemberByName(input.name) != nullptr)
			{
				input.name = "self->" + input.name;
			}
		}
		for (auto& output : stmtTemp->outputs)
		{
			// 需要判断sourceStr是否属于self结构体
			if (structDefine != nullptr && structDefine->getMemberByName(output.name) != nullptr)
			{
				output.name = "self->" + output.name;
			}
		}
	}
	else if (statement->type == Statement::LocalVariable)
	{
		StmtLocalVariable* stmtTemp = static_cast<StmtLocalVariable*>(statement);
		if (stmtTemp->defaultValue != nullptr)
		{
			updateExpressionForPackage(function, subsystemUtil, stmtTemp->defaultValue, structDefine);
			stmtTemp->defaultValue->updateExpressionStr();
		}
	}
	else if (statement->type == Statement::If)
	{
		StmtIf* stmtIf = static_cast<StmtIf*>(statement);
		updateExpressionForPackage(function, subsystemUtil, stmtIf->condition, structDefine);
		stmtIf->condition->updateExpressionStr();
	}
	else if (statement->type == Statement::ElseIf)
	{
		StmtElseIf* stmtElseIf = static_cast<StmtElseIf*>(statement);
		updateExpressionForPackage(function, subsystemUtil, stmtElseIf->condition, structDefine);
		stmtElseIf->condition->updateExpressionStr();
	}
	return 0;
}
