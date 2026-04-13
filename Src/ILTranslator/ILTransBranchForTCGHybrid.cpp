#include "ILTransBranch.h"

#include "ILTransFile.h"

#include "ILTranslator.h"
#include "ILTranslatorForTCGHybrid.h"
#include "ILTransScheduleNode.h"

#include "../ILBasic/ILBranch.h"
#include "../ILBasic/Expression.h"
#include "../Common/Utility.h"

#include "ILTransInstrumentBranchCoverage.h"
#include "ILTransInstrumentMCDCCoverage.h"
#include "ILTransInstrumentExpressionIterator.h"
#include "ILTransHeadFile.h"
#include "..\ILBasic\ILExpressionParser.h"
using namespace std;

int ILTransBranch::translateForTCGHybrid(const ILBranch* branch, ILTransFile* file) const
{
	if(branch->type == ILBranch::TypeIf) {
		string condStr = ILTransInstrumentExpressionIterator::InstrumentRootBoolExpressions(
			branch->condition, branch->getContainerILFunction());
		file->transCodeFunction += "if (" + condStr + ") {\n";
	}
	else if(branch->type == ILBranch::TypeElse) {
		file->transCodeFunction += "else {\n";
	}
	else if(branch->type == ILBranch::TypeElseIf) {
		string condStr = ILTransInstrumentExpressionIterator::InstrumentRootBoolExpressions(
			branch->condition, branch->getContainerILFunction());
		file->transCodeFunction += "else if (" + condStr + ") {\n";
	}
	else if(branch->type == ILBranch::TypeFor) {
		std::string oriCondStr = "i < " + branch->condition->expressionStr;
        //Expression* exp = ILExpressionParser::parseExpression(oriCondStr);
		//string condStr = ILTransInstrumentExpressionIterator::InstrumentRootBoolExpressions(exp);
        //exp->release();
        //delete exp;
		//std::string condStr = "i < " + bcStr;
		//std::string modifiedStr = ILTransInstrumentMCDCCoverage::GenerateMCDCToOneCondExpression(static_cast<const void*>(branch), oriCondStr, condStr);
		file->transCodeFunction += "for (int i = 0; " + oriCondStr + "; i++) {\n";
	}
	else if(branch->type == ILBranch::TypeWhile) {
		string condStr = ILTransInstrumentExpressionIterator::InstrumentRootBoolExpressions(
			branch->condition, branch->getContainerILFunction());
		file->transCodeFunction += "while (" + condStr + ") {\n";
	}
	else {
		return -ILTranslator::ErrorBranchType;
	}

	Instrument_BC::InsertInBlockBranchCoverageCollection(branch, file);
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

	ILTransScheduleNode iLTransScheduleNode;
	for(int i = 0;i<branch->scheduleNodes.size();i++)
	{
		int res = iLTransScheduleNode.translate(branch->scheduleNodes[i], file, nullptr);
		if(res < 0)
			return res;
	}

	file->transCodeFunction += "}\n";
	Instrument_BC::InsertPostBlockBranchCoverageCollection(branch, file);

	return 1;
}
