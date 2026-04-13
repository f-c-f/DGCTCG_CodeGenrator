#pragma once

#include "ILActorPattern.h"

class ILSchedule;
class ILScheduleNode;
class ILParser;
class ILBranch;
class ILCalculate;
class Expression;
class MIRModel;

class ILActorPatternIterator : private ILActorPattern
{
public:
	int optimize(ILSchedule* schedule, ILParser* parser, MIRModel* mirModel) override;
private:
	int optimize();
	int traverseILSchedule(ILSchedule* schedule);
	int traverseForIteratorBranch(ILBranch* branch);
	int matchScheduleNode(ILBranch* branch, ILCalculate* calculate);
	int matchAssignment(ILBranch* branch, ILCalculate* calculate, std::string forIteratorPortName);
	
	// optimize函数
	void optimizeSubsequentCalculateOfAssignment(ILCalculate* calculate, std::string forIteratorPortName);
	void optimizeSumCalculate(ILCalculate* calculate, std::string forIteratorPortName);
	void optimizeProductCalculate(ILCalculate* calculate, std::string forIteratorPortName);
	void optimizeSwitchCalculate(ILCalculate* calculate, std::string forIteratorPortName);
	void optimizeOutportCalculate(ILCalculate* calculate, std::string forIteratorPortName);
	void replaceInputAndOutputNameForCalculate(ILCalculate* calculate, std::string srcStr, std::string dstStr);
	void replaceInputAndOutputNameForStatement(Expression* exp, std::string srcStr, std::string dstStr);

	// 判断是否需要循环分裂
	std::pair<int, int> splitLoop;
	bool isSplitLoop = false;
	void splitSubsequentCalculateInsideLoop(ILCalculate* calculate, ILCalculate* container, std::string forIteratorPortName);

	// 插入组件到Iterator前面
	std::vector<std::pair<ILScheduleNode*, ILScheduleNode*>> insertFront;
	// 插入组件到Iterator后面
	// int数据类型定义了位于原来branch的位置
	std::vector<std::tuple<ILScheduleNode*, ILScheduleNode*, int>> insertBack;
};
