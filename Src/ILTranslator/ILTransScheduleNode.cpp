#include "ILTransScheduleNode.h"

#include "ILTransFile.h"
#include "ILTransBranch.h"
#include "ILTransCalculate.h"

#include "ILTranslator.h"
#include "ILTranslatorForTCGStateSearch.h"

#include "../ILBasic/ILObject.h"
#include "../ILBasic/ILBranch.h"
#include "../ILBasic/ILCalculate.h"

#include "../ILBasic/ILAnalyzeSchedule.h"

using namespace std;

int ILTransScheduleNode::translate(const ILScheduleNode* scheduleNode, ILTransFile* file, const ILAnalyzeSchedule* iLTranslateSchedule) const
{
	int res;
	if(scheduleNode->objType == ILObject::TypeBranch)
	{
		ILTransBranch iLTransBranch;
		res = iLTransBranch.translate(static_cast<const ILBranch*>(scheduleNode), file, iLTranslateSchedule);
		if(res < 0)
			return res;
	}
	else if(scheduleNode->objType == ILObject::TypeCalculate)
	{
		ILTransCalculate iLTransCalculate;
		res = iLTransCalculate.translate(static_cast<const ILCalculate*>(scheduleNode), file, iLTranslateSchedule);
		if(res < 0)
			return res;
	}
	else
	{
        int res = 0;
        res = -ILTranslator::ErrorILTransLocalVariableShouldNotInILSchedule;

		return res;
	}
	return 1;
}
