#include "ILActorPatternDataSelector.h"

#include <algorithm>

#include "../Common/Utility.h"
#include "../ILBasic/ILExpressionParser.h"

#include "../ILBasic/ILSchedule.h"
#include "../ILBasic/ILInput.h"
#include "../ILBasic/ILOutput.h"
#include "../ILBasic/ILScheduleNode.h"
#include "../ILBasic/ILBranch.h"
#include "../ILBasic/ILCalculate.h"
#include "../ILBasic/ILRef.h"

#include "../ILBasic/Expression.h"
#include "../ILBasic/Statement.h"
#include "../ILBasic/StmtExpression.h"
#include "../ILBasic/ILParser.h"


using namespace std;
namespace
{
    const int CONST_NUM_2 = 2;
}
int ILActorPatternDataSelector::optimize(ILSchedule* schedule, ILParser* parser, MIRModel* mirModel)
{
	this->schedule = schedule;
	this->iLParser = parser;
	this->mirModel = mirModel;
	int res = optimize();
	if(res < 0)
		return res;
    return 1;
}

int ILActorPatternDataSelector::optimize()
{
	int res = collectActorInputOutputMap();
	if(res < 0)
		return res;

	res = traverseILSchedule(this->schedule);
	if(res < 0)
		return res;

	for(int i = 0;i < patternList.size(); i++)
	{
		res =optimizePattern(&patternList[i]);
		if(res < 0)
			return res;
	}

    return 1;
}

int ILActorPatternDataSelector::traverseILSchedule(ILSchedule* schedule)
{
	int res;
	for(int i = 0; i < schedule->scheduleNodes.size(); i++)
	{
		ILScheduleNode* scheduleNode = schedule->scheduleNodes[i];
		if(scheduleNode->objType == ILScheduleNode::TypeBranch)
		{
			res = matchScheduleNode(scheduleNode);
			if(res < 0)
				return res;
			res = traverseILBranch(static_cast<ILBranch*>(scheduleNode));
			if(res < 0)
				return res;
		}
		else if(scheduleNode->objType == ILScheduleNode::TypeCalculate)
		{
			res = matchScheduleNode(scheduleNode);
			if(res < 0)
				return res;
		}
	}
	return 1;
}

int ILActorPatternDataSelector::traverseILBranch(ILBranch* branch)
{
	int res;
	for(int i = 0; i < branch->scheduleNodes.size(); i++)
	{
		ILScheduleNode* scheduleNode = branch->scheduleNodes[i];
		if(scheduleNode->objType == ILScheduleNode::TypeBranch)
		{
			res = matchScheduleNode(scheduleNode);
			if(res < 0)
				return res;
			res = traverseILBranch(static_cast<ILBranch*>(scheduleNode));
			if(res < 0)
				return res;
		}
		else if(scheduleNode->objType == ILScheduleNode::TypeCalculate)
		{
			res = matchScheduleNode(scheduleNode);
			if(res < 0)
				return res;
		}
	}

	return 1;
}

int ILActorPatternDataSelector::matchScheduleNode(const ILScheduleNode* scheduleNode)
{
	if(scheduleNode->objType != ILObject::TypeCalculate)
		return 0;

	const ILCalculate* condition = static_cast<const ILCalculate*>(scheduleNode);
	string actorType = condition->getRefActorType();
	if(!stringEndsWith(actorType,"Comparator"))
		return 0;

    Pattern p;

    ILOutput* srcOfin1Condition = nullptr;
    ILOutput* srcOfin2Condition = nullptr;
    ILInput* dstOfoutCondition = nullptr;
    
	ILCalculate* selector = nullptr;
    if(matchScheduleNodeA(p, condition, selector, srcOfin1Condition, srcOfin2Condition, dstOfoutCondition) == 0)
        return 0;
    
    ILOutput* srcOfin1Selector = nullptr;
    ILOutput* srcOfin2Selector = nullptr;
    ILInput* dstOfoutSelector = nullptr;

	if(matchScheduleNodeB(p, selector, srcOfin1Selector, srcOfin2Selector, dstOfoutSelector) == 0)
        return 0;
    
	if(srcOfin1Selector == srcOfin1Condition && srcOfin2Selector == srcOfin2Condition){
		p.patternType = Pattern::TypeTrueGetIn1;
	}else if(srcOfin1Selector == srcOfin2Condition && srcOfin2Selector == srcOfin1Condition){
		p.patternType = Pattern::TypeTrueGetIn1;
	}else{
		return 0;
	}

	p.conditionActor = const_cast<ILCalculate*>(condition);
	p.selectorActor = selector;

	p.srcOfin1Condition = srcOfin1Condition;
	p.srcOfin2Condition = srcOfin2Condition;
	p.dstOfoutCondition = dstOfoutCondition;

	p.srcOfin1Selector = srcOfin1Selector;
	p.srcOfin2Selector = srcOfin2Selector;
	p.dstOfoutSelector = dstOfoutSelector;

	patternList.push_back(p);

	return 0;
}

int ILActorPatternDataSelector::matchScheduleNodeA(Pattern &p, const ILCalculate* condition, 
    ILCalculate* &selector, ILOutput* &srcOfin1Condition, ILOutput* &srcOfin2Condition, 
    ILInput* &dstOfoutCondition)
{
	ILInput* in1Condition = nullptr;
	ILInput* in2Condition = nullptr;
	ILOutput* outCondition = nullptr;
	for(int i = 0;i < condition->inputs.size(); i++)
	{
		ILInput* input = condition->inputs[i];
		if(stringEndsWith(input->name, "_left"))
			in1Condition = input;
		if(stringEndsWith(input->name, "_right"))
			in2Condition = input;
	}
	for(int i = 0;i < condition->outputs.size(); i++)
	{
		ILOutput* output = condition->outputs[i];
		if(stringEndsWith(output->name, "_out"))
			outCondition = output;
	}
	if(!in1Condition || !in2Condition || !outCondition)
		return 0;

    srcOfin1Condition = actorInputToOutputMap[in1Condition];
    srcOfin2Condition = actorInputToOutputMap[in2Condition];
    dstOfoutCondition = actorOutputToInputMap[outCondition];
	if(!srcOfin1Condition || !srcOfin2Condition || !dstOfoutCondition)
		return 0;

	if(!stringEndsWith(dstOfoutCondition->name, "_control"))
		return 0;
	if(!dstOfoutCondition->parent || dstOfoutCondition->parent->objType != ILObject::TypeCalculate)
		return 0;
	selector = static_cast<ILCalculate*>(dstOfoutCondition->parent);
    
	p.in1Condition = in1Condition;
	p.in2Condition = in2Condition;
	p.outCondition = outCondition;

    return 1;
}

int ILActorPatternDataSelector::matchScheduleNodeB(Pattern &p, const ILCalculate* selector, 
    ILOutput* &srcOfin1Selector, ILOutput* &srcOfin2Selector, ILInput* &dstOfoutSelector)
{
	ILInput* in1Selector = nullptr;
	ILInput* in2Selector = nullptr;
	ILOutput* outSelector = nullptr;
	for(int i = 0;i < selector->inputs.size(); i++)
	{
		ILInput* input = selector->inputs[i];
		if(stringEndsWith(input->name, "_trueInput"))
			in1Selector = input;
		if(stringEndsWith(input->name, "_falseInput"))
			in2Selector = input;
	}
	for(int i = 0;i < selector->outputs.size(); i++)
	{
		ILOutput* output = selector->outputs[i];
		if(stringEndsWith(output->name, "_output"))
			outSelector = output;
	}
	if(!in1Selector || !in2Selector || !outSelector)
		return 0;
    srcOfin1Selector = actorInputToOutputMap[in1Selector];
    srcOfin2Selector = actorInputToOutputMap[in2Selector];
    dstOfoutSelector = actorOutputToInputMap[outSelector];
	if(!srcOfin1Selector || !srcOfin2Selector || !dstOfoutSelector)
		return 0;
    
	p.in1Selector = in1Selector;
	p.in2Selector = in2Selector;
	p.outSelector = outSelector;

    return 1;
}

int ILActorPatternDataSelector::optimizePattern(ILActorPatternDataSelector::Pattern* pattern) const
{
	vector<ILScheduleNode*>* containList = nullptr;
    if(pattern->conditionActor->parent != pattern->selectorActor->parent || 
        !pattern->conditionActor->parent){
        return 0;
	}
	if(pattern->conditionActor->parent->objType == ILObject::TypeBranch){
		ILBranch* parent = static_cast<ILBranch*>(pattern->conditionActor->parent);
		containList = &(parent->scheduleNodes);
	}else if(pattern->conditionActor->parent->objType == ILObject::TypeSchedule){
		ILSchedule* parent = static_cast<ILSchedule*>(pattern->conditionActor->parent);
		containList = &(parent->scheduleNodes);
	}
	if(!containList) {
	    return 0;
	}
    auto insertPos1 = find(containList->begin(),containList->end(), pattern->conditionActor);
    auto insertPos2 = find(containList->begin(),containList->end(), pattern->selectorActor);
	if(insertPos1 == containList->end() || insertPos2 == containList->end()){
		return 0;
    }
	vector<ILScheduleNode*>::iterator insertPos = insertPos2;
    if(insertPos1 < insertPos2){
		insertPos = insertPos1;
	}
	string conditionStr;
	if(stringEndsWith(pattern->conditionActor->getRefActorType(),"Comparator") && 
        !(pattern->conditionActor->statements.childStatements.empty())){
		Statement* stmt = pattern->conditionActor->statements.childStatements[0];
		if(!stmt || stmt->type != Statement::ExpressionWithSemicolon) {
		    return 0;
		}
		StmtExpression* stmtExp = static_cast<StmtExpression*>(stmt);
		Expression* exp = stmtExp->expression;
		if(!exp || exp->childExpressions.size() != CONST_NUM_2 || 
            exp->type != Token::Assign || !exp->childExpressions[1]) {
		    return 0;
		}
		exp = exp->childExpressions[1];
		if(exp->type == Token::Greater || exp->type == Token::GreaterEqual || exp->type == Token::Less ||
            exp->type == Token::LessEqual || exp->type == Token::Equal || exp->type == Token::NotEqual) {
		    conditionStr = Token::tokenStrList[exp->type - Token::SelfAdd];
		} else {
			return 0;
		}
	} else {
		return 0;
	}
    ILCalculate* patternActor = nullptr;
    return createPatternActor(patternActor, pattern, conditionStr, containList, insertPos - containList->begin());
}

int ILActorPatternDataSelector::createPatternActor(ILCalculate* &patternActor, 
    const ILActorPatternDataSelector::Pattern* pattern, string conditionStr, 
    vector<ILScheduleNode*>* containList, int insertPos) const
{
    ILActorPattern::patternCount++;
	patternActor = new ILCalculate();
    patternActor->parent = pattern->conditionActor->parent;
	patternActor->objType = ILObject::TypeCalculate;
	patternActor->name = "PatternActor" + to_string(ILActorPattern::patternCount);
	patternActor->isOptimized = false;
	for(int i = 0;i < pattern->conditionActor->refs.size();i++){
		patternActor->refs.push_back(pattern->conditionActor->refs[i]->clone(patternActor));
    }
	for(int i = 0;i < pattern->selectorActor->refs.size();i++){
		patternActor->refs.push_back(pattern->selectorActor->refs[i]->clone(patternActor));
    }
	ILInput* in1 = pattern->in1Condition->clone(patternActor);
	patternActor->inputs.push_back(in1);
	ILInput* in2 = pattern->in2Condition->clone(patternActor);
	patternActor->inputs.push_back(in2);
	ILOutput* out = pattern->outSelector->clone(patternActor);
	patternActor->outputs.push_back(out);
	string expressionStr = out->name + "=" + pattern->in1Condition->sourceStr + conditionStr + pattern->in2Condition->sourceStr + "?";
	if(pattern->patternType == Pattern::TypeTrueGetIn1) {
		expressionStr += pattern->in1Condition->sourceStr + ":" + pattern->in2Condition->sourceStr;
	} else {
		expressionStr += pattern->in2Condition->sourceStr + ":" + pattern->in1Condition->sourceStr;
	}
	StmtExpression* statement = new StmtExpression();
	statement->type = Statement::ExpressionWithSemicolon;
	statement->depth = 1;
	statement->parentStatement = &patternActor->statements;
	ILExpressionParser expressionParser;
	Expression* expRet = nullptr;
	int res = ILExpressionParser::parseExpression(expressionStr, &expRet);
	if(res < 0){
        patternActor->release();
        delete patternActor;
        patternActor = nullptr;
        statement->release();
        delete statement;
		return res;
    }
	statement->innerExpressions.push_back(expRet);
	statement->expression = expRet;
	patternActor->statements.childStatements.push_back(statement);
	patternActor->statements.type = static_cast<Statement::Type>(-1);
    
	containList->insert(containList->begin() + insertPos, patternActor);
	iLParser->releaseILObject(pattern->conditionActor);
	iLParser->releaseILObject(pattern->selectorActor);
	return 1;
}
