#include "ILTransBranch.h"
#include "ILTransFile.h"

#include "ILTranslator.h"
#include "ILTransScheduleNode.h"

#include "../ILBasic/ILBranch.h"
#include "../ILBasic/Expression.h"
#include "../Common/Utility.h"

#include "../ILBasic/ILAnalyzeSchedule.h"

using namespace std;

int ILTransBranch::translate(const ILBranch* branch, ILTransFile* file, const ILAnalyzeSchedule* iLTranslateSchedule) const
{
	if (ILTranslator::translateForCBMC)
		return translateForCBMC(branch, file);
	else if (ILTranslator::translateForTCGStateSearch)
		return translateForTCGStateSearch(branch, file);
	else if (ILTranslator::translateForTCGHybrid)
		return translateForTCGHybrid(branch, file);
	else if (ILTranslator::translateForCoverage)
		return translateForCoverage(branch, file);
    else
        return translateBasic(branch, file, iLTranslateSchedule);
	return 0;
}


int ILTransBranch::translateBasic(const ILBranch* branch, ILTransFile* file, const ILAnalyzeSchedule* iLTranslateSchedule) const
{
	string stmtStr;

    string conditionExpression = branch->condition ? branch->condition->expressionStr : "";
	if(branch->type == ILBranch::TypeIf)
		stmtStr += "if (" + conditionExpression + ")";
	else if(branch->type == ILBranch::TypeElse)
		stmtStr += "else";
	else if(branch->type == ILBranch::TypeElseIf)
		stmtStr += "else if (" + conditionExpression + ")";
	else if(branch->type == ILBranch::TypeFor)
		stmtStr += "for (int i = 0; i < " + conditionExpression + "; i++)";
	else if(branch->type == ILBranch::TypeWhile)
		stmtStr += "while (" + conditionExpression + ")";
	else 
		return -ILTranslator::ErrorBranchType;

    if((branch->type == ILBranch::TypeElse || branch->type == ILBranch::TypeElseIf) &&
        stringEndsWith(file->transCodeFunction, "\n")) {
        file->transCodeFunction[file->transCodeFunction.length() - 1] = ' ';
    } else if (ILTranslator::configGenerateComment){
        string commentStr = "/* ";
        commentStr += "Branch of ";
        commentStr += ILTranslator::getRefComment(&(branch->refs));
        commentStr += " */\n";
        file->transCodeFunction += commentStr;
    }


	file->transCodeFunction += stmtStr + " {\n";
	//file->transCodeFunction += "{\n";
    
	ILTransScheduleNode iLTransScheduleNode;
	for(int i = 0;i<branch->scheduleNodes.size();i++)
	{
		int res = iLTransScheduleNode.translate(branch->scheduleNodes[i], file, iLTranslateSchedule);
		if(res < 0)
			return res;
	}

	file->transCodeFunction += "}\n";

	return 1;
}
