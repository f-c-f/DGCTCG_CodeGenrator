#pragma once
#include "ILActorPattern.h"
#include <unordered_map>

class ILParser;
class ILSchedule;
class MIRModel;
class MIRActor;
class ILCalculate;
class MIRFunctionDataflow;
class ILFunction;

class PortRange 
{
public:
	// 属性
	std::string name;
	int start;
	int length;
	std::vector<int> index;
};


class ILActorPatternDataTruncator : private ILActorPattern 
{
public:
	int optimize(ILSchedule* schedule, ILParser* parser, MIRModel* mirModel) override;
private:
	int optimize();
	int traverseILSchedule(ILSchedule* schedule);
	int optimizeDataTruncators();
	int processSelector(ILCalculate* ilSelector, MIRActor* mirSelector);
	int processCalculate(ILCalculate* calculate);

	// 调用该方法之后
	// 构建的数据流图保存在ilRelations中
	void constructILRelationMap(ILFunction* ilFunction, MIRFunctionDataflow* mirFunction);


	// Relation分为起始组件和终止组件两个部分
	// 由于该优化是从后往前推导
	// 因此，先确认终止组件的计算范围，再确认起始组件的计算范围
	int mappingDerivation(ILOutput* srcOuput, ILInput* dstInput);

	// 储存需要修改的组件
	// 组件级映射关系
	// 目前的实现没有考虑复合组件的情况
	// 即 input不一定属于ILCalculate
	// 也可能属于复合组件（ILFunction）
	std::unordered_map<ILCalculate*, std::vector<PortRange>> actorToRange;
	std::vector<ILCalculate*> dataTruncators;
	std::unordered_map<ILInput*, ILRelation> ilRelations;

};
