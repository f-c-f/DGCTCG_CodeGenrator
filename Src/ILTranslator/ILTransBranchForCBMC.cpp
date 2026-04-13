#include "ILTransBranch.h"

#include "ILTransFile.h"
#include "ILTransInstrumentBranchCoverage.h"

#include "ILTranslator.h"
#include "ILTransScheduleNode.h"

#include "../ILBasic/ILBranch.h"
#include "../ILBasic/Expression.h"
#include "../Common/Utility.h"

using namespace std;

int ILTransBranch::translateForCBMC(const ILBranch* branch, ILTransFile* file) const
{
	string stmtStr;

    string conditionExpression = branch->condition ? branch->condition->expressionStr : "";

	if(ILTranslatorForCBMC::configTautology && 
		branch->type == ILBranch::TypeIf) //For?While??????????????????????????????????�?	
		{
        stmtStr += "char __tautologyTempVariable__v" + to_string(ILTranslatorForCBMC::tautologyTempVariableCount) + " = (" + conditionExpression + ") != 0;\n";
        conditionExpression = "__tautologyTempVariable__v" +
            to_string(ILTranslatorForCBMC::tautologyTempVariableCount);
		stmtStr += "assert(" + conditionExpression + ");//For Tautology Branch Detection\n";
        ILTranslatorForCBMC::tautologyTempVariableCount++;
	}

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

    
    if(ILTranslatorForCBMC::configBranchCover) {
        Instrument_BC::InsertInBlockBranchCoverageCollection(branch, file);
    }

	if(ILTranslatorForCBMC::configUnreachable)
	{
	    file->transCodeFunction += "assert(0);//For Unreachable Branch Detection\n";
	    file->transCodeFunction += "assert(1);//For Unreachable Branch Detection\n";
	}
    
	ILTransScheduleNode iLTransScheduleNode;
	for(int i = 0;i<branch->scheduleNodes.size();i++)
	{
		int res = iLTransScheduleNode.translate(branch->scheduleNodes[i], file, nullptr);
		if(res < 0)
			return res;
	}

	file->transCodeFunction += "}\n";

    if(ILTranslatorForCBMC::configBranchCover) {
        Instrument_BC::InsertPostBlockBranchCoverageCollection(branch, file);
    }
	return 1;
}
