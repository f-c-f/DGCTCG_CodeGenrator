#pragma once
#include <map>
#include <string>
#include <vector>
#include <stack>
#include <unordered_map>
class ILOutput;
class ILInput;
class ILParser;
class ILSchedule;
class ILBranch;
class ILScheduleNode;
class ILCalculate;
class MIRModel;

class ILRelation {
public:
	ILOutput* src;
	ILInput* dst;
};

class ILActorPattern
{
public:
    ILActorPattern();
    virtual ~ILActorPattern();

	static int patternCount;

	virtual int optimize(ILSchedule* schedule, ILParser* parser, MIRModel* mirModel);
	ILSchedule* schedule = nullptr;
	ILParser* iLParser = nullptr;
	MIRModel* mirModel = nullptr;

	// 暴露的数据流分析接口
	// 组件级别的数据流分析
	std::vector<std::vector<ILRelation*>> dataflowAnalyze(ILCalculate* src, ILCalculate* dst);
	std::vector<ILRelation*> getRelationsInsideBranch(ILBranch* branch);

	std::map<ILInput*, ILOutput*> actorInputToOutputMap;
	std::map<ILOutput*, ILInput*> actorOutputToInputMap;
	int collectActorInputOutputMap();
private:
	std::map<std::string, ILInput*> nameToInputMap;
	std::map<std::string, ILOutput*> nameToOutputMap;
	int traverseCollectILSchedule(ILSchedule* schedule);
	int traverseCollectILBranch(ILBranch* branch);
	int collectScheduleNode(ILScheduleNode* scheduleNode);
	void getRelationMap(ILBranch* branch);
	void searchRelationForConnectPath(ILCalculate* cur, ILCalculate* tar);
	ILOutput* findSourceOutport(ILBranch* branch, std::string sourceOutport);
	// 两节点之间的数据流路径
	std::vector<std::vector<ILRelation*>> dataflowPaths;
	// 数据流分析得出的当前路径
	std::stack<ILRelation*> lists;
	// 当前Branch下的关联关系
	std::unordered_map<ILInput*, ILRelation*> relations;
};

