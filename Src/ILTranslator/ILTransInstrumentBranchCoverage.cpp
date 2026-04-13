#include "ILTransInstrumentBitmapHelper.h"
#include "ILTransInstrumentBranchCoverage.h"
#include "../ILBasic/ILRef.h"
#include "../ILBasic/Expression.h"
#include "../ILBasic/StmtIf.h"
#include "../ILBasic/StmtElseIf.h"
#include "../ILBasic/StmtFor.h"
#include "../ILBasic/StmtWhile.h"
#include "../ILBasic/StmtDoWhile.h"
#include <sstream>
#include <assert.h>
#include <algorithm>

#include "ILTranslator.h"
#include "ILTranslatorForTCGStateSearch.h"
#include "ILTranslatorForTCGHybrid.h"
#include "../ILBasic/ILFunction.h"
#include "..\ILTranslator\ILTranslatorUtility.h"

using namespace std;

void ILTransInstrumentBranchCoverage::InsertInBlockBranchCoverageCollection(const ILBranch* branch, ILTransFile* file)
{
	int id = ILTransInstrumentBitmapHelper::branchBitmapCount;
	ILTransInstrumentBitmapHelper::branchBitmapCount++;
	for (ILRef* iref : branch->refs) {
		std::map<std::string, int>::iterator itr = ILTransInstrumentBitmapHelper::branchIndexMap.find(iref->path);
		if (itr == ILTransInstrumentBitmapHelper::branchIndexMap.end()) {
			ILTransInstrumentBitmapHelper::branchIndexMap[iref->path] = id;
		}
	}
	const void* ss = static_cast<const void*>(branch);
	std::stringstream ssStr;
	ssStr << ss;
	std::string bbStr = ssStr.str();
	ILTransInstrumentBitmapHelper::branchIndexMap[bbStr] = id;

	std::string desc = "";
	if (branch->type == ILBranch::BranchType::TypeElse) {
		int index = -1;
		std::vector<ILScheduleNode*>* ilsns = branch->parent->getListOfILScheduleNode();
		std::vector<ILScheduleNode*>::iterator pos = std::find(ilsns->begin(), ilsns->end(), branch);
		if (pos != ilsns->end())
			index = pos - ilsns->begin();
		assert(index > 0);
		desc += "negative";
		for (index = index - 1; index >= 0; index--) {
			ILScheduleNode* ilsn = ilsns->at(index);
			assert(ilsn->objType == ILObject::ObjType::TypeBranch);
			ILBranch* ilb = static_cast<ILBranch*>(ilsn);
			desc += "_" + ilb->condition->expressionStr;
			if (ilb->type == ILBranch::BranchType::TypeIf) {
				break;
			}
		}
	}
	else {
		desc += "positive branch:" + branch->condition->expressionStr;
	}
	
	ILTransInstrumentBitmapHelper::branchDesc[id] = desc;
    ILTransInstrumentBitmapHelper::branchCoverNameForCBMC[id] = Instrument_BC::getBranchCoverNameForCBMC(branch);
    ILTransInstrumentBitmapHelper::branchCoverCallDepth[id] = ILTranslatorUtility::getStatementCallDepth(branch);
    ILTransInstrumentBitmapHelper::branchCoverDomain[id] = branch->getDomain();
    int branchCoverParent = -1;
    int branchCoverBrother = -1;
    getBranchParentAndBrother(id, branchCoverParent, branchCoverBrother);
    ILTransInstrumentBitmapHelper::branchCoverParent[id] = branchCoverParent;
    ILTransInstrumentBitmapHelper::branchCoverBrother[id] = branchCoverBrother;

	ILTransInstrumentBitmapHelper::branchMapId[branch] = id;
	if(ILTranslator::translateForTCGStateSearch || ILTranslator::translateForTCGHybrid) {
	    file->transCodeFunction += "BitmapCov::branchBitmap[" + std::to_string(id) + "] = 1;\n";
        //if (ILTranslatorForTCGStateSearch::branchDistanceBranchObjectList.find(branch) != ILTranslatorForTCGStateSearch::branchDistanceBranchObjectList.end()) {
            ILTranslatorForTCGStateSearch::branchDistanceBranchObjectList[branch] = id;
			ILTranslatorForTCGHybrid::branchDistanceBranchObjectList[branch] = id;
        //}
	} else if (ILTranslator::translateForCBMC) {
		file->transCodeFunction += "__CPROVER_cover(" + std::to_string(id + 1) + ");\n";
	} else if (ILTranslator::translateForLibFuzzer) {
		file->transCodeFunction += "BranchCoverageStatistics(" + std::to_string(id) + ");\n";
	} else if(ILTranslator::translateForTCGHybrid) {
	    file->transCodeFunction += "BitmapCov::branchBitmap[" + std::to_string(id) + "] = 1;\n";
	} else if (ILTranslator::translateForCoverage) {
		file->transCodeFunction += "BitmapCov::branchBitmap[" + std::to_string(id) + "] = 1;\n";
	}
}

void ILTransInstrumentBranchCoverage::InsertPostBlockBranchCoverageCollection(const ILBranch* branch, ILTransFile* file)
{
	bool toInst = false;
	if (branch->type == ILBranch::BranchType::TypeFor || branch->type == ILBranch::BranchType::TypeWhile) {
		toInst = true;
	}
	if (branch->type == ILBranch::BranchType::TypeIf || branch->type == ILBranch::BranchType::TypeElseIf) {
		ILObject* obj = branch->getNextSameILObject();
		if (obj != nullptr) {
			if (obj->objType == ILObject::ObjType::TypeBranch) {
				ILBranch* bb = static_cast<ILBranch*>(obj);
				// bb->type == ILBranch::BranchType::TypeIf || 
				if (bb->type == ILBranch::BranchType::TypeElse 
					|| bb->type == ILBranch::BranchType::TypeElseIf) {
					;
				}
				else {
					toInst = true;
				}
			}
			else {
				toInst = true;
			}
		}
		else {
			toInst = true;
		}
	}
	if (toInst) {
		std::string branchDesc = "negative";
		const ILBranch* obj = nullptr;
		do {
			if (obj == nullptr) {
				obj = branch;
			}
			else {
				obj = static_cast<ILBranch*>(obj->getPreviousSameILObject());
			}
			branchDesc += "_" + obj->condition->expressionStr;
		} while (obj->type != ILBranch::BranchType::TypeIf && obj->type != ILBranch::BranchType::TypeFor && obj->type != ILBranch::BranchType::TypeWhile);

		int id = ILTransInstrumentBitmapHelper::branchBitmapCount;
		ILTransInstrumentBitmapHelper::branchBitmapCount++;
		const void* ss = static_cast<const void*>(branch);
		std::stringstream ssStr;
		ssStr << ss;
		std::string bbStr = ssStr.str() + "_POST";
		std::map<std::string, int>::iterator itr = ILTransInstrumentBitmapHelper::branchIndexMap.find(bbStr);
		// assert(itr == ILTransInstrumentBitmapHelper::branchIndexMap.end());
		// int id = itr->second;
		ILTransInstrumentBitmapHelper::branchIndexMap[bbStr] = id;
		ILTransInstrumentBitmapHelper::branchDesc[id] = branchDesc;
        ILTransInstrumentBitmapHelper::branchCoverNameForCBMC[id] = Instrument_BC::getBranchCoverNameForCBMC(branch);
        ILTransInstrumentBitmapHelper::branchCoverCallDepth[id] = ILTranslatorUtility::getStatementCallDepth(branch) - 1;
        ILTransInstrumentBitmapHelper::branchCoverDomain[id] = branch->getDomain();
        ILTransInstrumentBitmapHelper::branchCoverDomain[id].pop_back();
        int branchCoverParent = -1;
        int branchCoverBrother = -1;
        getBranchParentAndBrother(id, branchCoverParent, branchCoverBrother);
        ILTransInstrumentBitmapHelper::branchCoverParent[id] = branchCoverParent;
        ILTransInstrumentBitmapHelper::branchCoverBrother[id] = branchCoverBrother;
		ILTransInstrumentBitmapHelper::branchMapId[branch] = id;
		if (ILTranslator::translateForTCGStateSearch) {
			file->transCodeFunction += "BitmapCov::branchBitmap[" + std::to_string(id) + "] = 1;\n";
		} else if (ILTranslator::translateForCBMC) {
			file->transCodeFunction += "__CPROVER_cover(" + std::to_string(id + 1) + ");\n";
		} else if (ILTranslator::translateForLibFuzzer) {
			file->transCodeFunction += "BranchCoverageStatistics(" + std::to_string(id) + ");\n";
		} else if (ILTranslator::translateForTCGHybrid) {
			file->transCodeFunction += "BitmapCov::branchBitmap[" + std::to_string(id) + "] = 1;\n";
		} else if (ILTranslator::translateForCoverage) {
			file->transCodeFunction += "BitmapCov::branchBitmap[" + std::to_string(id) + "] = 1;\n";
		}
	}
}

void ILTransInstrumentBranchCoverage::InsertStatementInBlockBranchCoverageCollection(const Statement* stmt, const std::string branchDesc, std::string& stmtStr)
{
	const void* ss = static_cast<const void*>(stmt);
	std::stringstream ssStr;
	ssStr << ss;
	std::string bbStr = ssStr.str();
	int id = ILTransInstrumentBitmapHelper::branchBitmapCount;
	ILTransInstrumentBitmapHelper::branchBitmapCount++;
	ILTransInstrumentBitmapHelper::branchIndexMap[bbStr] = id;
	ILTransInstrumentBitmapHelper::branchDesc[id] = "positive branch:" + branchDesc;
    ILTransInstrumentBitmapHelper::branchCoverNameForCBMC[id] = Instrument_BC::getBranchCoverNameForCBMC(stmt);
    ILTransInstrumentBitmapHelper::branchCoverCallDepth[id] = ILTranslatorUtility::getStatementCallDepth(stmt);
    ILTransInstrumentBitmapHelper::branchCoverDomain[id] = stmt->getDomain();
    //if(stmt->type == Statement::Switch || stmt->type == Statement::Case)
    //    ILTransInstrumentBitmapHelper::branchCoverDomain[id].pop_back();
    int branchCoverParent = -1;
    int branchCoverBrother = -1;
    getBranchParentAndBrother(id, branchCoverParent, branchCoverBrother, stmt->type == Statement::Switch || stmt->type == Statement::Case);
    ILTransInstrumentBitmapHelper::branchCoverParent[id] = branchCoverParent;
    ILTransInstrumentBitmapHelper::branchCoverBrother[id] = branchCoverBrother;
	ILTransInstrumentBitmapHelper::branchMapId[stmt] = id;
	if (ILTranslator::translateForTCGStateSearch || ILTranslator::translateForTCGHybrid) {
		stmtStr += "BitmapCov::branchBitmap[" + std::to_string(id) + "] = 1;\n";
        //if (ILTranslatorForTCGStateSearch::branchDistanceBranchObjectList.find(stmt) != ILTranslatorForTCGStateSearch::branchDistanceBranchObjectList.end()) {
            ILTranslatorForTCGStateSearch::branchDistanceBranchObjectList[stmt] = id;
				ILTranslatorForTCGHybrid::branchDistanceBranchObjectList[stmt] = id;
        //}
	} else if (ILTranslator::translateForCBMC) {
		stmtStr += "__CPROVER_cover(" + std::to_string(id + 1) + ");\n";
	} else if (ILTranslator::translateForLibFuzzer) {
		stmtStr += "BranchCoverageStatistics(" + std::to_string(id) + ");\n";
	} else if (ILTranslator::translateForTCGHybrid) {
		stmtStr += "BitmapCov::branchBitmap[" + std::to_string(id) + "] = 1;\n";
	} else if (ILTranslator::translateForCoverage) {
		stmtStr += "BitmapCov::branchBitmap[" + std::to_string(id) + "] = 1;\n";
	}
}

void ILTransInstrumentBranchCoverage::InsertStatementPostBlockBranchCoverageCollection(const Statement* stmt, std::string& stmtStr)
{
	bool toInst = false;
	if (stmt->type == Statement::Type::For || stmt->type == Statement::Type::While 
		|| stmt->type == Statement::Type::DoWhile) {
		toInst = true;
	}

	// if (stmt->type == Statement::Type::Case || stmt->type == Statement::Type::Default) {
	//	to_inst = true;
	// }

	if (stmt->type == Statement::Type::If || stmt->type == Statement::Type::ElseIf) {
		Statement* nextStmt = stmt->getNextStatement();
		if (nextStmt != nullptr) {
			// nextStmt->type == Statement::Type::If || 
			if (nextStmt->type == Statement::Type::Else
				|| nextStmt->type == Statement::Type::ElseIf) {
				;
			}
			else {
				toInst = true;
			}
		}
		else {
			toInst = true;
		}
	}

	if (toInst) {
		std::string branchDesc = "negative";
		const Statement* descStmt = nullptr;
		do {
			if (descStmt == nullptr) {
				descStmt = stmt;
			}
			else {
				descStmt = descStmt->getPreviousStatement();
			}

			if (descStmt->type == Statement::Type::If) {
				const StmtIf* stmtTemp = static_cast<const StmtIf*>(stmt);
				branchDesc += "_" + stmtTemp->condition->expressionStr;
			}
			else if (descStmt->type == Statement::Type::ElseIf) {
				const StmtElseIf* stmtTemp = static_cast<const StmtElseIf*>(stmt);
				branchDesc += "_" + stmtTemp->condition->expressionStr;
			}
			else if (descStmt->type == Statement::Type::For) {
				const StmtFor* stmtTemp = static_cast<const StmtFor*>(stmt);
				std::string for_cond_str = ILTransInstrumentBitmapHelper::AlwaysTrueCondDesc;
				if (stmtTemp->condition != nullptr) {
					for_cond_str = stmtTemp->condition->expressionStr;
				}
				branchDesc += "_" + for_cond_str;
			}
			else if (descStmt->type == Statement::Type::While) {
				const StmtWhile* stmtTemp = static_cast<const StmtWhile*>(stmt);
				branchDesc += "_" + stmtTemp->condition->expressionStr;
			}
			else if (descStmt->type == Statement::Type::DoWhile) {
				const StmtDoWhile* stmtTemp = static_cast<const StmtDoWhile*>(stmt);
				branchDesc += "_" + stmtTemp->condition->expressionStr;
			}
			else {
				assert(false);
			}
		} while (descStmt->type != Statement::Type::If && descStmt->type != Statement::Type::For && descStmt->type != Statement::Type::While && descStmt->type != Statement::Type::DoWhile);
		
		int id = ILTransInstrumentBitmapHelper::branchBitmapCount;
		ILTransInstrumentBitmapHelper::branchBitmapCount++;
		const void* ss = static_cast<const void*>(stmt);
		std::stringstream ssStr;
		ssStr << ss;
		std::string bbStr = ssStr.str() + "_POST";
		std::map<std::string, int>::iterator itr = ILTransInstrumentBitmapHelper::branchIndexMap.find(bbStr);
		assert(itr == ILTransInstrumentBitmapHelper::branchIndexMap.end());
		// int id = itr->second;
		ILTransInstrumentBitmapHelper::branchIndexMap[bbStr] = id;
		ILTransInstrumentBitmapHelper::branchDesc[id] = branchDesc;
        ILTransInstrumentBitmapHelper::branchCoverNameForCBMC[id] = Instrument_BC::getBranchCoverNameForCBMC(stmt);
        ILTransInstrumentBitmapHelper::branchCoverCallDepth[id] = ILTranslatorUtility::getStatementCallDepth(stmt) - 1;
        ILTransInstrumentBitmapHelper::branchCoverDomain[id] = stmt->getDomain();
        ILTransInstrumentBitmapHelper::branchCoverDomain[id].pop_back();
        int branchCoverParent = -1;
        int branchCoverBrother = -1;
        getBranchParentAndBrother(id, branchCoverParent, branchCoverBrother);
        ILTransInstrumentBitmapHelper::branchCoverParent[id] = branchCoverParent;
        ILTransInstrumentBitmapHelper::branchCoverBrother[id] = branchCoverBrother;
		ILTransInstrumentBitmapHelper::branchMapId[stmt] = id;
		if (ILTranslator::translateForTCGStateSearch) {
			stmtStr += "BitmapCov::branchBitmap[" + std::to_string(id) + "] = 1;\n";
		} else if (ILTranslator::translateForCBMC) {
			stmtStr += "__CPROVER_cover(" + std::to_string(id + 1) + ");\n";
		} else if (ILTranslator::translateForLibFuzzer) {
			stmtStr += "BranchCoverageStatistics(" + std::to_string(id) + ");\n";
		} else if (ILTranslator::translateForTCGHybrid) {
			stmtStr += "BitmapCov::branchBitmap[" + std::to_string(id) + "] = 1;\n";
		} else if (ILTranslator::translateForCoverage) {
			stmtStr += "BitmapCov::branchBitmap[" + std::to_string(id) + "] = 1;\n";
		}
	}
}

void ILTransInstrumentBranchCoverage::InsertSwitchDefaultBranchCoverageCollection(const Statement* stmt, const std::string branchDesc, std::string & stmtStr)
{
	const void* ss = static_cast<const void*>(stmt);
	std::stringstream ssStr;
	ssStr << ss;
	std::string bbStr = ssStr.str();
	int id = ILTransInstrumentBitmapHelper::branchBitmapCount;
	ILTransInstrumentBitmapHelper::branchBitmapCount++;
	ILTransInstrumentBitmapHelper::branchIndexMap[bbStr] = id;
	ILTransInstrumentBitmapHelper::branchDesc[id] = "switch branch:" + branchDesc;
    ILTransInstrumentBitmapHelper::branchCoverNameForCBMC[id] = Instrument_BC::getBranchCoverNameForCBMC(stmt);
    ILTransInstrumentBitmapHelper::branchCoverCallDepth[id] = ILTranslatorUtility::getStatementCallDepth(stmt);
    ILTransInstrumentBitmapHelper::branchCoverDomain[id] = stmt->getDomain();
    //ILTransInstrumentBitmapHelper::branchCoverDomain[id].pop_back();
    int branchCoverParent = -1;
    int branchCoverBrother = -1;
    getBranchParentAndBrother(id, branchCoverParent, branchCoverBrother, true);
    ILTransInstrumentBitmapHelper::branchCoverParent[id] = branchCoverParent;
    ILTransInstrumentBitmapHelper::branchCoverBrother[id] = branchCoverBrother;
	ILTransInstrumentBitmapHelper::branchMapId[stmt] = id;
	if (ILTranslator::translateForTCGStateSearch) {
		stmtStr += "BitmapCov::branchBitmap[" + std::to_string(id) + "] = 1;\n";
	} else if (ILTranslator::translateForCBMC) {
		stmtStr += "__CPROVER_cover(" + std::to_string(id + 1) + ");\n";
	} else if (ILTranslator::translateForLibFuzzer) {
		stmtStr += "BranchCoverageStatistics(" + std::to_string(id) + ");\n";
	} else if (ILTranslator::translateForTCGHybrid) {
		stmtStr += "BitmapCov::branchBitmap[" + std::to_string(id) + "] = 1;\n";
	} else if (ILTranslator::translateForCoverage) {
		stmtStr += "BitmapCov::branchBitmap[" + std::to_string(id) + "] = 1;\n";
	}
}

std::string ILTransInstrumentBranchCoverage::getBranchCoverNameForCBMC(const ILObject* branch)
{
    ILFunction* containFunction = branch->getContainerILFunction();
    return getBranchCoverNameForCBMC(containFunction);
}

std::string ILTransInstrumentBranchCoverage::getBranchCoverNameForCBMC(const Statement* stmt)
{
    ILFunction* containFunction = stmt->getContainerILFunction();
    return getBranchCoverNameForCBMC(containFunction);
}
std::string ILTransInstrumentBranchCoverage::getBranchCoverNameForCBMC(const ILFunction* function)
{
    string functionName;
    if(function)
        functionName = function->name;
    if(ILTransInstrumentBitmapHelper::branchCoverFunctionCoverCount.find(functionName) == 
        ILTransInstrumentBitmapHelper::branchCoverFunctionCoverCount.end()) {
        ILTransInstrumentBitmapHelper::branchCoverFunctionCoverCount[functionName] = 0;
    }
    ILTransInstrumentBitmapHelper::branchCoverFunctionCoverCount[functionName]++;
    int branchCoverIdForCBMC = ILTransInstrumentBitmapHelper::branchCoverFunctionCoverCount[functionName];

    return functionName + ".coverage." + to_string(branchCoverIdForCBMC);
}

void ILTransInstrumentBranchCoverage::getBranchParentAndBrother(int branchCoverId, int& parent, int& brother, bool isCaseOrDefaultStmt)
{
    parent = -1;
    brother = -1;
    vector<void*> domain = ILTransInstrumentBitmapHelper::branchCoverDomain[branchCoverId];
    if(isCaseOrDefaultStmt)
        domain.pop_back();
    for(int i = branchCoverId - 1; i >= 0; i--)
    {
        vector<void*>& domainTemp = ILTransInstrumentBitmapHelper::branchCoverDomain[i];
        if(domainTemp.size() > domain.size())
        {
            continue;
        }
        else if(domainTemp.size() == domain.size())
        {
            if(domainTemp[domainTemp.size() - 1] == domain[domain.size() - 1])
            {
                brother = i;
                return;
            }
        }
        else
        {
            if(domainTemp[domainTemp.size() - 1] == domain[domain.size() - 2])
            {
                parent = i;
                return;
            }
        }
    }
}
