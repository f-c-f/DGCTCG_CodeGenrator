#include "ILActorPatternIterator.h"
#include "../ILBasic/ILSchedule.h"
#include "../ILBasic/ILScheduleNode.h"
#include "../ILBasic/ILBranch.h"
#include "../ILBasic/ILRef.h"
#include "../ILBasic/ILCalculate.h"
#include "../ILBasic/ILInput.h"
#include "../ILBasic/ILOutput.h"
#include "../ILBasic/StmtBatchCalculation.h"
#include "../Common/Utility.h"
#include "../ILBasic/ILCCodeParser.h"
#include "../ILBasic/StmtIf.h"
#include "../ILBasic/ILExpressionParser.h"
#include "../ILBasic/StmtElse.h"
#include "../ILBasic/Expression.h"
#include "../ILBasic/StmtFor.h"
#include "../ILBasic/StmtExpression.h"
#include "../ILBasic/ILExpressionParser.h"
#include "../MIRBasic/MIRModel.h"
#include <algorithm>


int ILActorPatternIterator::optimize(ILSchedule* schedule, ILParser* parser, MIRModel* mirModel)
{
	this->schedule = schedule;
	this->iLParser = parser;
	this->mirModel = mirModel;


	if (isSplitLoop == true)
		isSplitLoop = false;
	int res = optimize();
	if (res < 0)
		return res;
	return 1;
}

int ILActorPatternIterator::optimize()
{
	int res;
	res = traverseILSchedule(this->schedule);
	if (res < 0)
		return res;
	return res;
}

int ILActorPatternIterator::traverseILSchedule(ILSchedule* schedule)
{
	int res;

	for (auto scheduleNode : schedule->scheduleNodes)
	{
		if (scheduleNode->objType == ILScheduleNode::TypeBranch
			&& static_cast<ILBranch*>(scheduleNode)->type == ILBranch::TypeFor)
		{
			ILBranch* branch = static_cast<ILBranch*>(scheduleNode);
			for (auto ref : branch->refs)
			{
				// 当前仅处理For-Iterator
				// 后续会考虑加入While-Iterator之类的
				// 这一步是找到ForIterator表示的Branch
				// 之后需要在该分支中进一步的对For-Iterator Actor和Assignment Actor进行匹配
				if (ref->actor == "ForIterator")
				{
					res = traverseForIteratorBranch(branch);
					break;
				}
			}
		}
	}

	// 处理insertFront
	for (const auto& insertPair : insertFront)
	{
		for (int i = 0; i < schedule->scheduleNodes.size(); i++)
		{
			if (insertPair.first == schedule->scheduleNodes[i])
			{
				schedule->scheduleNodes.insert(schedule->scheduleNodes.begin() + i, insertPair.second);
				break;
			}
		}
	}

	// 处理insertBack
	// insertBack这里需要保证插入的顺序按照原来的顺序
	// 写个简单的拓扑排序
	std::sort(insertBack.begin(), insertBack.end(),
		[](const std::tuple<ILScheduleNode*, ILScheduleNode*, int>& a, const std::tuple<ILScheduleNode*, ILScheduleNode*, int>& b) {
			return std::get<2>(a) > std::get<2>(b); // 按照int值降序排列
		}
	);

	for (const auto& insertPair : insertBack)
	{
		for (int i = 0; i < schedule->scheduleNodes.size(); i++)
		{
			if (std::get<0>(insertPair) == schedule->scheduleNodes[i])
			{
				if (std::get<0>(insertPair)->objType == ILScheduleNode::TypeBranch)
				{
					ILBranch* branch = static_cast<ILBranch*> (std::get<0>(insertPair));
					for (int i = 0; i < branch->scheduleNodes.size(); i++)
					{
						if (branch->scheduleNodes[i] == std::get<1>(insertPair))
						{
							branch->scheduleNodes.erase(branch->scheduleNodes.begin() + i);
							break;
						}
					}
				}
				schedule->scheduleNodes.insert(schedule->scheduleNodes.begin() + i + 1, std::get<1>(insertPair));
				std::get<1>(insertPair)->parent = schedule;
				break;
			}
		}
	}
	return 1;
}

int ILActorPatternIterator::traverseForIteratorBranch(ILBranch* branch)
{
	int res;

	for (auto& scheduleNode : branch->scheduleNodes)
	{
		if (scheduleNode->objType == ILScheduleNode::TypeCalculate)
		{
			res = matchScheduleNode(branch, static_cast<ILCalculate*>(scheduleNode));
			if (res < 0)
				return res;
		}
	}

	return 1;
}

int ILActorPatternIterator::matchScheduleNode(ILBranch* branch, ILCalculate* calculate)
{
	if (calculate->refs[0]->actor == "Assignment")
	{
		// 判断是否需要循环分裂
		if (calculate->inputs.size() == 3)
		{
			int forIterationCount = stringToInt(branch->condition->expressionStr);
			int assignmentIterationCount =calculate->inputs[0]->arraySize[0];
			if (assignmentIterationCount > forIterationCount)
			{
				splitLoop = { forIterationCount, assignmentIterationCount };
			}
			else {
				// 暂时不处理异常情况
			}
		}
		// 判断Assignment是否连接了For Iterator
		std::string sourceStr;
		if (calculate->inputs.size() == 3)
		{
			sourceStr = calculate->inputs[2]->sourceStr;
		}
		else if (calculate->inputs.size() == 2)
		{
			sourceStr = calculate->inputs[1]->sourceStr;
		}

		// 这个判断由于For-Iterator直接同Assignment，因此不需要用dataflowAnalysis
		// 目前不考虑 Assignment组件同For-Iterator的连接中间还存在其它组件的情况
		if (!sourceStr.empty())
		{
			for (auto& scheduleNode : branch->scheduleNodes)
			{
				if (scheduleNode->objType == ILScheduleNode::TypeCalculate)
				{
					ILCalculate* cal = static_cast<ILCalculate*>(scheduleNode);
					if (stringStartsWith(sourceStr, cal->name) && cal->refs[0]->actor == "ForIterator")
					{
						// 开始优化
						matchAssignment(branch, calculate, sourceStr);
						break;
					}
				}
			}
		}
	}

	return 1;
}

int ILActorPatternIterator::matchAssignment(ILBranch* branch, ILCalculate* calculate, std::string forIteratorPortName)
{
	// 判断Assignment是否连接了Output
	std::string outputName;
	std::string assignmentName;

	// 更改BatchAssign的位置
	for (int i = 0; i < calculate->statements.childStatements.size(); i++)
	{
		if (calculate->statements.childStatements[i]->type == Statement::BatchCalculation)
		{
			StmtBatchCalculation* batchAssign = static_cast<StmtBatchCalculation*>(calculate->statements.childStatements[i]);
			// batchAssign->outputs[0].name = outputName;

			ILCalculate* assignCalculate = new ILCalculate;
			for (auto& ref : calculate->refs)
			{
				assignCalculate->refs.push_back(ref->clone(assignCalculate));
			}

			assignCalculate->name = calculate->name + "_Assign_Optimized";
			assignCalculate->statements.childStatements.push_back(batchAssign);
			assignCalculate->parent = branch;
			assignCalculate->statements.parentILObject = assignCalculate;

			calculate->statements.childStatements[i]->parentStatement = &(assignCalculate->statements);
			calculate->statements.childStatements[i]->parentILObject = assignCalculate;

			calculate->statements.childStatements.erase(calculate->statements.childStatements.begin() + i);

			
			insertFront.push_back({ branch, assignCalculate });
			
			//// 获取上层包含的schedule
			//ILSchedule* schedule = calculate->getContainerILSchedule();
			//// 插入新生成的ILCalculate
			//for (int j = 0; j < schedule->scheduleNodes.size(); j++)
			//{
			//	if (branch == schedule->scheduleNodes[j])
			//	{
			//		schedule->scheduleNodes.insert(schedule->scheduleNodes.begin() + j, assignCalculate);
			//		break;
			//	}

			//	// 没有找到对应的 branch 则不插入
			//}
			//break;
		}
	}


	// 对后续节点进行修改
	optimizeSubsequentCalculateOfAssignment(calculate, forIteratorPortName);

	// 分裂循环
	if (isSplitLoop)
	{
		// 创建循环分离的容器
		ILCalculate* container = new ILCalculate(calculate->parent);
		container->name = "SplitContainer";
		ILRef* ref = new ILRef(container);
		ref->actor = "SplitContainer";
		std::vector<std::string> paths = stringSplit(calculate->refs[0]->path, ".");
		for (int i = 0; i < paths.size()-1; i++)
		{
			ref->path += paths[i];
		}
		
		// insertBack.push_back({ branch, container });

		//// 插入分离容器
		//// 获取上层包含的schedule
		//ILSchedule* schedule = calculate->getContainerILSchedule();
		//// 插入新生成的ILCalculate
		//for (int j = 0; j < schedule->scheduleNodes.size(); j++)
		//{
		//	if (branch == schedule->scheduleNodes[j])
		//	{
		//		schedule->scheduleNodes.insert(schedule->scheduleNodes.begin() + j + 1, container);

		//		break;
		//	}

		//	// 没有找到对应的 branch 则不插入
		//}

		// 对内部进行操作，尝试分离
		splitSubsequentCalculateInsideLoop(calculate, container, forIteratorPortName);
	}

	// 寻找output端口
	for (auto& scheduleNode : branch->scheduleNodes)
	{
		if (scheduleNode->objType == ILScheduleNode::TypeCalculate)
		{
			ILCalculate* output = static_cast<ILCalculate*>(scheduleNode);

			// Assignment的输出连接到了ForIterator子系统的输出端口

			if ((output->refs[0]->actor == "Outport" || output->refs[0]->actor == "DataStoreWrite")
				&& stringStartsWith(output->inputs[0]->sourceStr, calculate->name + "_"))
			{
				// optimize
				if (output->refs[0]->actor == "Outport")
					outputName = output->outputs[0]->name;
				else
					outputName = output->name + "_state";
				assignmentName = calculate->name + "_" + output->inputs[0]->name;

				// 修改Output名字
				replaceInputAndOutputNameForCalculate(calculate, assignmentName, outputName);

				for (int i = 0; i < branch->scheduleNodes.size(); i++)
				{
					if (output == branch->scheduleNodes[i])
					{
						branch->scheduleNodes.erase(branch->scheduleNodes.begin() + i);
						break;
					}
				}
				break;


			}
		}
	}

	// 寻找需要修改的Calculate
	if (!outputName.empty() && !assignmentName.empty())
	{
		for (auto& scheduleNode : branch->scheduleNodes)
		{
			if (scheduleNode->objType == ILScheduleNode::TypeCalculate)
			{
				ILCalculate* cal = static_cast<ILCalculate*>(scheduleNode);

				for (auto& input : calculate->inputs)
				{
					// 将Assignment的变量名修改为Output的变量名
					if (input->sourceStr == assignmentName)
					{
						replaceInputAndOutputNameForCalculate(cal, assignmentName, outputName);
						break;
					}
				}
			}
		}

	}

	return 1;
}


void ILActorPatternIterator::optimizeSubsequentCalculateOfAssignment(ILCalculate* calculate, std::string forIteratorPortName)
{

	//  TypeAdd,
	//  TypeSub,
	//	TypeMul,
	//	TypeDiv,
	if (!calculate)
		return;

	// 递归
	if (calculate->refs[0]->actor != "Assignment")
	{
		ILBranch* branch = calculate->getContainerILBranch();
		int position = 0;
		for (; position < branch->scheduleNodes.size(); position++)
		{
			if (branch->scheduleNodes[position] == calculate)
				break;
		}

		bool isContain = false;
		for (auto& back : insertBack)
		{
			if (std::get<1>(back) == calculate)
			{
				isContain = true;
				break;
			}
		}
		if(!isContain)
			insertBack.push_back({ branch, calculate, position });
	}

	//if (calculate->refs[0]->actor == "Sum")
	//{
	//	optimizeSumCalculate(calculate, forIteratorPortName);
	//}
	//else if (calculate->refs[0]->actor == "Product")
	//{
	//	optimizeProductCalculate(calculate, forIteratorPortName);
	//}
	//else if (calculate->refs[0]->actor == "Switch")
	//{
	//	optimizeSwitchCalculate(calculate, forIteratorPortName);
	//}
	//else if (calculate->refs[0]->actor == "Outport")
	//{
	//	optimizeOutportCalculate(calculate, forIteratorPortName);
	//}



	for (auto& relation : getRelationsInsideBranch(calculate->getContainerILBranch()))
	{
		// 如果连接到了Calculate的输出端口
		// 并且类型为TypeCalculate
		if (relation->src != nullptr && relation->dst != nullptr)
		{
			if (relation->src->parent == calculate &&
				relation->dst->parent->objType == ILScheduleNode::TypeCalculate)
			{
				optimizeSubsequentCalculateOfAssignment(static_cast<ILCalculate*>(relation->dst->parent), forIteratorPortName);
			}
		}
	}
}

void ILActorPatternIterator::optimizeSumCalculate(ILCalculate* calculate, std::string forIteratorPortName)
{
	int res;
	for (int i = 0; i < calculate->statements.childStatements.size(); i++)
	{
		if (calculate->statements.childStatements[i]->type == Statement::BatchCalculation)
		{
			StmtBatchCalculation* batchStatement = static_cast<StmtBatchCalculation*>(calculate->statements.childStatements[i]);
			std::string expression;
			if (batchStatement->operationType == StmtBatchCalculation::TypeAdd)
			{
				expression = batchStatement->outputs[0].name + "[" + forIteratorPortName + "] = ";

				if (!batchStatement->inputs[0].arraySize.empty())
					expression += batchStatement->inputs[0].name + "[" + forIteratorPortName + "] + ";
				else
					expression += batchStatement->inputs[0].name + " + ";

				if (!batchStatement->inputs[1].arraySize.empty())
					expression += batchStatement->inputs[1].name + "[" + forIteratorPortName + "];";
				else
					expression += batchStatement->inputs[1].name + ";";
			}
			else
			{
				if (!batchStatement->inputs[0].arraySize.empty())
					expression += batchStatement->inputs[0].name + "[" + forIteratorPortName + "] - ";
				else
					expression += batchStatement->inputs[0].name + " - ";

				if (!batchStatement->inputs[1].arraySize.empty())
					expression += batchStatement->inputs[1].name + "[" + forIteratorPortName + "];";
				else
					expression += batchStatement->inputs[1].name + ";";
			}

			ILCCodeParser codeParser;
			Statement* statement = new Statement;
			res = codeParser.parseCCode(expression, statement);
			if (res < 0)
				return;
			calculate->statements.childStatements[i]->release();
			calculate->statements.childStatements[i] = statement->childStatements[0];

			statement->childStatements.clear();
			statement->release();
		}
	}
}

void ILActorPatternIterator::optimizeProductCalculate(ILCalculate* calculate, std::string forIteratorPortName)
{
	int res;
	for (int i = 0; i < calculate->statements.childStatements.size(); i++)
	{
		if (calculate->statements.childStatements[i]->type == Statement::BatchCalculation)
		{
			StmtBatchCalculation* batchStatement = static_cast<StmtBatchCalculation*>(calculate->statements.childStatements[i]);
			std::string expression;
			if (batchStatement->operationType == StmtBatchCalculation::TypeMul)
			{
				expression = batchStatement->outputs[0].name + "[" + forIteratorPortName + "] = ";

				if (!batchStatement->inputs[0].arraySize.empty())
					expression += batchStatement->inputs[0].name + "[" + forIteratorPortName + "] * ";
				else
					expression += batchStatement->inputs[0].name + " * ";

				if (!batchStatement->inputs[1].arraySize.empty())
					expression += batchStatement->inputs[1].name + "[" + forIteratorPortName + "];";
				else
					expression += batchStatement->inputs[1].name + ";";
			}
			else
			{
				if (!batchStatement->inputs[0].arraySize.empty())
					expression += batchStatement->inputs[0].name + "[" + forIteratorPortName + "] / ";
				else
					expression += batchStatement->inputs[0].name + " / ";

				if (!batchStatement->inputs[1].arraySize.empty())
					expression += batchStatement->inputs[1].name + "[" + forIteratorPortName + "];";
				else
					expression += batchStatement->inputs[1].name + ";";
			}

			ILCCodeParser codeParser;
			Statement* statement = new Statement;
			res = codeParser.parseCCode(expression, statement);
			if (res < 0)
				return;
			calculate->statements.childStatements[i]->release();
			calculate->statements.childStatements[i] = statement->childStatements[0];

			statement->childStatements.clear();
			statement->release();
		}
	}
}

void ILActorPatternIterator::optimizeSwitchCalculate(ILCalculate* calculate, std::string forIteratorPortName)
{
	int res;

	// 如果可以优化
	// 第一条Statement应该为For
	if (calculate->statements.childStatements[0]->type == Statement::For)
	{
		StmtFor* stmtFor = static_cast<StmtFor*>(calculate->statements.childStatements[0]);
		std::string statementStr = "";

		// StmtFor的第一条语句为StmtIf
		// StmtFor的第二条语句为StmtElese
		if (stmtFor->statements[0]->type == Statement::If)
		{
			statementStr += "if (";
			StmtIf* stmtIf = static_cast<StmtIf*>(stmtFor->statements[0]);
			// 生成Switch的语句
			// 然后通过Parser转为对应的Statement
			statementStr += calculate->inputs[1]->sourceStr + "[" + forIteratorPortName + "]" 
				+ stmtIf->condition->tokenStr + stmtIf->condition->childExpressions[1]->expressionStr + ") {\n";
			statementStr += calculate->name + "_" + calculate->outputs[0]->name + "[" + forIteratorPortName + "]";
			statementStr += " = ";
			if (!calculate->inputs[0]->arraySize.empty())
			{
				statementStr += calculate->inputs[0]->sourceStr + "[" + forIteratorPortName + "];\n";
			}
			else
			{
				statementStr += calculate->inputs[0]->sourceStr + "];\n";
			}
			statementStr += "}\n";
		}
		if (stmtFor->statements[1]->type == Statement::Else)
		{
			statementStr += "else {\n";
			statementStr += calculate->name + "_" + calculate->outputs[0]->name + "[" + forIteratorPortName + "]";
			statementStr += " = ";
			if (!calculate->inputs[2]->arraySize.empty())
			{
				statementStr += calculate->inputs[2]->sourceStr + "[" + forIteratorPortName + "];\n";
			}
			else
			{
				statementStr += calculate->inputs[2]->sourceStr + ";\n";
			}
			statementStr += "}\n";
		}

		ILCCodeParser codeParser;
		calculate->statements.release();
		res = codeParser.parseCCode(statementStr, &calculate->statements);
		if (res < 0)
			return;
		
	}

	return;
}

void ILActorPatternIterator::optimizeOutportCalculate(ILCalculate* calculate, std::string forIteratorPortName)
{
	int res;
	for (int i = 0; i < calculate->statements.childStatements.size(); i++)
	{
		if (calculate->statements.childStatements[i]->type == Statement::BatchCalculation)
		{
			StmtBatchCalculation* batchStatement = static_cast<StmtBatchCalculation*>(calculate->statements.childStatements[i]);
			std::string expression;
			if (batchStatement->operationType == StmtBatchCalculation::TypeAssign)
			{
				if (!batchStatement->inputs[0].arraySize.empty())
					expression = batchStatement->outputs[0].name + "[" + forIteratorPortName + "] = " +
					batchStatement->inputs[0].name + "[" + forIteratorPortName + "];";
				else
					expression = batchStatement->outputs[0].name + "[" + forIteratorPortName + "] = " +
					batchStatement->inputs[0].name + ";";
				ILCCodeParser codeParser;
				Statement* statement = new Statement;
				res = codeParser.parseCCode(expression, statement);
				if (res < 0)
					return;
				calculate->statements.childStatements[i]->release();
				calculate->statements.childStatements[i] = statement->childStatements[0];

				statement->childStatements.clear();
				statement->release();
			}
		}
	}
}

void ILActorPatternIterator::replaceInputAndOutputNameForCalculate(ILCalculate* calculate, std::string srcStr, std::string dstStr)
{
	for (auto statement : calculate->statements.childStatements)
	{
		if (statement->type == Statement::ExpressionWithSemicolon)
		{
			for (auto& expression : statement->innerExpressions)
			{
				// 将Assignment替代为Output
				replaceInputAndOutputNameForStatement(expression, srcStr, dstStr);
				expression->updateExpressionStr();
			}
		}
	}
}

void ILActorPatternIterator::replaceInputAndOutputNameForStatement(Expression* exp, std::string srcStr, std::string dstStr)
{
	if (exp != nullptr)
	{
		for (auto expression : exp->childExpressions)
		{
			replaceInputAndOutputNameForStatement(expression, srcStr, dstStr);
		}

		if (exp->type == Token::Variable)
		{
			if (exp->expressionStr == srcStr)
			{
				exp->expressionStr = dstStr;
				exp->tokenStr = dstStr;
			}
		}
	}
}

void ILActorPatternIterator::splitSubsequentCalculateInsideLoop(ILCalculate* calculate, ILCalculate* container, std::string forIteratorPortName)
{
	if (container->statements.childStatements.size() == 0)
	{
		// 需要对Container初始化
		StmtFor* stmtFor = new StmtFor(&container->statements);
		container->statements.childStatements.push_back(stmtFor);

		StmtExpression* stmtInit = new StmtExpression(stmtFor);
		stmtFor->initial.push_back(stmtInit);
		stmtFor->childStatements.push_back(stmtInit);

		stmtInit->expression = ILExpressionParser::parseExpression(forIteratorPortName + "=" + std::to_string(splitLoop.first));
		stmtInit->innerExpressions.push_back(stmtInit->expression);
		
		std::string conditionStr = forIteratorPortName + "<" + std::to_string(splitLoop.second);
		stmtFor->condition = ILExpressionParser::parseExpression(conditionStr);
		stmtFor->innerExpressions.push_back(stmtFor->condition);

		StmtExpression* stmtIter = new StmtExpression(stmtFor);
		std::string iterStr = forIteratorPortName + "++";
		stmtFor->iterator.push_back(stmtIter);
		stmtFor->childStatements.push_back(stmtIter);
		stmtIter->expression = ILExpressionParser::parseExpression(iterStr);
		stmtIter->innerExpressions.push_back(stmtIter->expression);
	}
	else if (calculate != nullptr) {
		StmtFor* stmtFor = static_cast<StmtFor*>(container->statements.childStatements[0]);
		stmtFor->childStatements.insert(stmtFor->childStatements.end(), calculate->statements.childStatements.begin(), calculate->statements.childStatements.end());
		stmtFor->statements.insert(stmtFor->statements.end(), calculate->statements.childStatements.begin(), calculate->statements.childStatements.end());
	}

	if (calculate != nullptr) {
		for (auto& relation : getRelationsInsideBranch(calculate->getContainerILBranch()))
		{
			// 如果连接到了Calculate的输出端口
			// 并且类型为TypeCalculate
			if (relation->src->parent == calculate &&
				relation->dst->parent->objType == ILScheduleNode::TypeCalculate)
			{
				// 递归
				splitSubsequentCalculateInsideLoop(static_cast<ILCalculate*>(relation->dst->parent), container, forIteratorPortName);
			}
		}
	}
}
