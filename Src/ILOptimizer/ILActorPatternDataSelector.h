#pragma once
#include <vector>

#include "ILActorPattern.h"

class ILCalculate;
class ILActorPatternDataSelector :
	private ILActorPattern
{
public:
	int optimize(ILSchedule* schedule, ILParser* parser, MIRModel* mirModel) override;

private:

	int optimize();

	int traverseILSchedule(ILSchedule* schedule);
	int traverseILBranch(ILBranch* branch);

	


	struct Pattern{
		ILCalculate* conditionActor = nullptr;
		
		const ILInput* in1Condition = nullptr;
		const ILInput* in2Condition = nullptr;
		const ILOutput* outCondition = nullptr;
		
		const ILOutput* srcOfin1Condition = nullptr;
		const ILOutput* srcOfin2Condition = nullptr;
		const ILInput* dstOfoutCondition = nullptr;
		

		ILCalculate* selectorActor = nullptr;

		const ILInput* in1Selector = nullptr;
		const ILInput* in2Selector = nullptr;
		const ILOutput* outSelector = nullptr;

		const ILOutput* srcOfin1Selector = nullptr;
		const ILOutput* srcOfin2Selector = nullptr;
		const ILInput* dstOfoutSelector = nullptr;


		
        enum PatternType{
            TypeTrueGetIn1 = 1,
            TypeTrueGetIn2,
            TypeUnknown,
        };
        PatternType patternType = TypeUnknown; // 1 true分支取in1   2 true分支取in2
	};

    int matchScheduleNode(const ILScheduleNode* scheduleNode);
	int matchScheduleNodeA(Pattern &p, const ILCalculate* condition, ILCalculate* &selector, ILOutput* &srcOfin1Condition, ILOutput* &srcOfin2Condition, ILInput* &dstOfoutCondition);
	int matchScheduleNodeB(Pattern &p, const ILCalculate* selector, ILOutput* &srcOfin1Selector, ILOutput* &srcOfin2Selector, ILInput* &dstOfoutSelector);


	std::vector<ILActorPatternDataSelector::Pattern> patternList;

	int optimizePattern(ILActorPatternDataSelector::Pattern* pattern) const;
	int createPatternActor(ILCalculate* &patternActor, const ILActorPatternDataSelector::Pattern* pattern, std::string conditionStr, std::vector<ILScheduleNode*>* containList, int insertPos) const;
};

