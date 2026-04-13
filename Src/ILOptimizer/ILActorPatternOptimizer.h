#pragma once

//设计思路：为每种不同的组件组合模式定义一个基于ILActorPattern的类，
//比如ILActorPatternDataSelector
//
//优化时，针对每个schedule创建所有不同的ILActorPattern类对象，
//然后把schedule传给各个不同的ILActorPattern类对象，
//由不同的ILActorPattern类对象对schedule进行遍历搜索和中间代码替换

class ILParser;
class ILSchedule;
class MIRModel;
class ILActorPatternOptimizer
{
public:
	int optimize(ILSchedule* schedule, ILParser* parser, MIRModel* mirModel);
	ILSchedule* schedule = nullptr;

private:
	ILParser* iLParser = nullptr;
	MIRModel* mirModel = nullptr;
	int optimize() const;

};

