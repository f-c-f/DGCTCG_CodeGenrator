#include "ILTransInstrumentMCDCCoverage.h"
#include "ILTransInstrumentBitmapHelper.h"
#include "ILTransInstrumentEnv.h"
#include "../ILBasic/ILBranch.h"
#include "../ILBasic/Expression.h"
#include "../ILTranslator/ILTransFile.h"
#include "../Common/Utility.h"
#include "../ILBasic/StmtIf.h"
#include "../ILBasic/StmtElseIf.h"
#include "../ILBasic/StmtSwitch.h"
#include "../ILBasic/StmtCase.h"


using namespace std;

std::string ILTransInstrumentMCDCCoverage::MCDCCoverageFunc;
std::map<const Expression*, std::string> ILTransInstrumentMCDCCoverage::MCDCCoverageSubExpMap;
int ILTransInstrumentMCDCCoverage::MCDCSubExpId;
std::vector<std::pair<const Expression*, std::string>> ILTransInstrumentMCDCCoverage::MCDCCoverageLeafExp;
std::string ILTransInstrumentMCDCCoverage::MCDCCoverageFuncAll;

int ILTransInstrumentMCDCCoverage::HandleDecisionCoverage(const void* condAddr, std::string condStr) {
	/*
	handle condition coverage.
	*/
	// this is for true of condition
	int cbid = ILTransInstrumentBitmapHelper::decBitmapCount;
	ILTransInstrumentBitmapHelper::decBitmapCount++;
	// this is for false of condition
	int fcbid = ILTransInstrumentBitmapHelper::decBitmapCount;
	ILTransInstrumentBitmapHelper::decBitmapCount++;
	std::string cbbStr = std::to_string((long long)condAddr);// static_cast<const void*>(cond_addr)
	ILTransInstrumentBitmapHelper::decIndexMap[cbbStr] = cbid;
	ILTransInstrumentBitmapHelper::decIndexMap[cbbStr + "F"] = fcbid;
	ILTransInstrumentBitmapHelper::decDesc[cbid] = condStr + "==true";
	ILTransInstrumentBitmapHelper::decDesc[fcbid] = condStr + "==false";
	return cbid;
}

int ILTransInstrumentMCDCCoverage::HandleConditionCoverage(const void* condAddr, std::string condStr) {
	/*
	handle condition coverage.
	*/
	// this is for true of condition
	int cbid = ILTransInstrumentBitmapHelper::condBitmapCount;
	ILTransInstrumentBitmapHelper::condBitmapCount++;
	// this is for false of condition
	int fcbid = ILTransInstrumentBitmapHelper::condBitmapCount;
	ILTransInstrumentBitmapHelper::condBitmapCount++;
	std::string cbbStr = std::to_string((long long)condAddr);// static_cast<const void*>(cond_addr)
	ILTransInstrumentBitmapHelper::condIndexMap[cbbStr] = cbid;
	ILTransInstrumentBitmapHelper::condIndexMap[cbbStr+"F"] = fcbid;
	ILTransInstrumentBitmapHelper::condDesc[cbid] = condStr + "==true";
	ILTransInstrumentBitmapHelper::condDesc[fcbid] = condStr + "==false";
	return cbid;
}


std::string ILTransInstrumentMCDCCoverage::HandleLeafConditionExpression(const int bid, const void* condAddr, std::string oriCondStr, std::string condStr, int& decCondId)
{
	int cbid = HandleConditionCoverage(condAddr, condStr);
//	std::string varName = "b_" + std::to_string(bid) + "_" + std::to_string(condId);
	// std::string modifiedExpr = condition->expressionStr;
	// assert(itr != ILTransInstrumentBitmapHelper::mcdcConditionDescInDecision.end());
	std::map<int, std::string>::iterator itr = ILTransInstrumentBitmapHelper::mcdcConditionDescInDecision.find(bid);
	std::string oriDescStr = itr->second + "#" + condStr;
	ILTransInstrumentBitmapHelper::mcdcConditionDescInDecision[bid] = oriDescStr;
//	std::string mcdc_a_vect = "BitmapCov::mcdcBitmap[" + std::to_string(bid) + "]";
//	flatCode += "int last_pos = " + mcdc_a_vect + ".size() - 1;\n";
//	flatCode += mcdc_a_vect + "[last_pos] = " + mcdc_a_vect + "[last_pos] | (((int)" + varName + ") << " + std::to_string(condId) + ");\n";
//	AddCodeToModifyMCDCLast(flatCode, bid, varName, condId);
	std::string fExpr = "BitmapCov::CondAndMCDCRecord(" + condStr + ", " + std::to_string(cbid) + ", " + std::to_string(decCondId) + ", " + std::to_string(bid) + ")";
	decCondId++;
	return fExpr;
}


std::string ILTransInstrumentMCDCCoverage::HandleConditionExpression(const int bid, const Expression* condition, std::map<std::string, std::string>& exprStrMap, int& decCondId)
{
	/*
	comment = true;
	mcdc computation:
	test case1:     0010;F;
	test case2:     0000;T;
	xor:            0010;1;
	Cond1:          0010;

	...
	Cond2:          0001;
	Cond1 or Cond2: 0011 -> Coverage Info.
	Storage: int;
	Number of Conditions < 31;
	*/
	std::string opPre = "";
	std::string opPost = "";
	bool condIsNonLeaf = false;
	bool trimOpPost = true;
	switch (condition->type) {
		case Token::Type::And: {
			opPost = ILTransInstrumentEnv::disableShortCircuit ? "&" : "&&";
			condIsNonLeaf = true;
			break;
		}
		case Token::Type::Or: {
			opPost = ILTransInstrumentEnv::disableShortCircuit ? "|" : "||";
			condIsNonLeaf = true;
			break;
		}
		case Token::Type::Not: {
			opPre = "!";
			condIsNonLeaf = true;
			break;
		}
		case Token::Type::Bracket: {
			opPre = "(";
			opPost = ")";
			trimOpPost = false;
			condIsNonLeaf = true;
			break;
		}
		default:
			break;
	}

	std::string modifiedExpr = "";
	std::string modifiedExprSplit = "";
	if (condIsNonLeaf) {
		std::vector<Expression*> childs = condition->childExpressions;
		for (Expression* child : childs) {
			modifiedExpr += (opPre + HandleConditionExpression(bid, child, exprStrMap, decCondId) + opPost);
            modifiedExprSplit += (opPre + MCDCCoverageSubExpMap.at(child) + opPost);
		}
		if (trimOpPost && opPost != "" && stringEndsWith(modifiedExpr, opPost)) {
			modifiedExpr = modifiedExpr.substr(0, modifiedExpr.length() - opPost.length());
            modifiedExprSplit = modifiedExprSplit.substr(0, modifiedExprSplit.length() - opPost.length());
		}
        MCDCSubExpId++;
        std::string subExpStr = "unsigned char exp_" + std::to_string(MCDCSubExpId) + " = " + modifiedExprSplit;
        MCDCCoverageSubExpMap[condition] = "exp_" + std::to_string(MCDCSubExpId);

        MCDCCoverageFunc += subExpStr + ";\n";
	}
	else {

        MCDCSubExpId++;
        std::string subExpStr = "unsigned char exp_" + std::to_string(MCDCSubExpId) + " = c_" + std::to_string(decCondId);
        MCDCCoverageSubExpMap[condition] = "c_" + std::to_string(decCondId);

        MCDCCoverageFunc += subExpStr + ";\n";
        MCDCCoverageLeafExp.push_back(make_pair(condition, MCDCCoverageSubExpMap[condition]));

		const void* condAddr = static_cast<const void*>(condition);
		std::string cbbStr = std::to_string((long long)condAddr);
		bool exist = exprStrMap.find(cbbStr) != exprStrMap.end();
		std::string rstr = (exist ? exprStrMap[cbbStr] : condition->expressionStr);
		modifiedExpr += HandleLeafConditionExpression(bid, condAddr, condition->expressionStr, rstr, decCondId);

	}

//	int cbid = HandleConditionCoverage(static_cast<const void*>(condition), condition->expressionStr);
//	modifiedExpr = "BitmapCov::CondRecord(" + modifiedExpr + ", " + std::to_string(cbid) + ")";
	return modifiedExpr;
}

/*void GetRootVarName(const void* block, std::string& rootVarName)
{
	std::string bbStr = std::to_string((long long)(static_cast<const void*>(block));

	std::map<std::string, int>::iterator itr = ILTransInstrumentBitmapHelper::mcdcIndexMap.find(bbStr);
	// assert(itr != ILTransInstrumentBitmapHelper::mcdcIndexMap.end());
	int bid = itr->second;
	rootVarName += "b_" + std::to_string(bid) + "_" + std::to_string(0);
}*/

// const void* block, 
std::string ILTransInstrumentMCDCCoverage::GenerateMCDCToRootExpression(const Expression* condition, std::map<std::string, std::string>& exprStrMap)
{
	if (condition == nullptr) {
		return "";
	}
    MCDCCoverageLeafExp.clear();
    MCDCCoverageSubExpMap.clear();
    MCDCCoverageFunc = "";
    MCDCSubExpId = 0;

	int bid = ILTransInstrumentBitmapHelper::mcdcBitmapCount;
	ILTransInstrumentBitmapHelper::mcdcBitmapCount++;
	ILTransInstrumentBitmapHelper::mcdcDecisionExpressionStr[bid] = condition->expressionStr;
	ILTransInstrumentBitmapHelper::recordMcdcDecisionSite(bid);

//	AddCodeToCreateNewMCDCLast(flatCode, bid);

	std::string bbStr = std::to_string((long long)condition);// block

	ILTransInstrumentBitmapHelper::mcdcIndexMap[bbStr] = bid;
	ILTransInstrumentBitmapHelper::mcdcConditionDescInDecision[bid] = "";
	int condId = 1;
	std::string modifiedExpr = HandleConditionExpression(bid, condition, exprStrMap, condId);
	int condNum = condId - 1;
	ILTransInstrumentBitmapHelper::mcdcConditionNumInDecision[bid] = condNum;
//	std::string rootVarName = "";
//	GetRootVarName(block, rootVarName);
//	flatCode += "bool " + rootVarName + " = " + modifiedExpr + ";\n";
//	flatCode += "BitmapCov::mcdcBitmap[" + std::to_string(bid) + "] |= (((int)" + rootVarName + ") << 0);\n";
//	AddCodeToModifyMCDCLast(flatCode, bid, rootVarName, 0);

    if(!MCDCCoverageLeafExp.empty())
    {
        string func;
        func += generateMCDCFuncHead(bid);
        func += "{\n";
        func += generateMCDCFuncExpSplit();
        func += generateMCDCFuncMaskingCond();
        func += generateMCDCFuncRes();
        func += "}\n";
    
        func = codeIndentProcess(func);
    
        MCDCCoverageFuncAll += func + "\n\n";
    }
    

	int decid = HandleDecisionCoverage(condition, condition->expressionStr);
	std::string fExpr = "BitmapCov::CondAndMCDCRecord(" + modifiedExpr + ", -" + std::to_string(decid+1) + ", 0, " + std::to_string(bid) + ")";
	
    return fExpr;
}

/*
std::string ILTransInstrumentMCDCCoverage::GenerateMCDCToOneCondExpression(const void* block, const std::string oriCondStr, const std::string condStr)
{
	int bid = ILTransInstrumentBitmapHelper::mcdcBitmapCount;
	ILTransInstrumentBitmapHelper::mcdcBitmapCount++;

	//	AddCodeToCreateNewMCDCLast(flatCode, bid);

	std::string bbStr = std::to_string((long long)block);

	ILTransInstrumentBitmapHelper::mcdcIndexMap[bbStr] = bid;
	ILTransInstrumentBitmapHelper::mcdcConditionDescInDecision[bid] = "";
	int condId = 1;
	std::string modifiedExpr = HandleLeafConditionExpression(bid, block, oriCondStr, condStr, condId);
	int condNum = condId - 1;
	ILTransInstrumentBitmapHelper::mcdcConditionNumInDecision[bid] = condNum;
	//	std::string rootVarName = "";
	//	GetRootVarName(block, rootVarName);
	//	flatCode += "bool " + rootVarName + " = " + modifiedExpr + ";\n";
	//	flatCode += "BitmapCov::mcdcBitmap[" + std::to_string(bid) + "] |= (((int)" + rootVarName + ") << 0);\n";
	//	AddCodeToModifyMCDCLast(flatCode, bid, rootVarName, 0);
	int decid = HandleDecisionCoverage(block, condStr);
	std::string fExpr = "BitmapCov::CondAndMCDCRecord(" + modifiedExpr + ", -" + std::to_string(decid+1) + ", 0, " + std::to_string(bid) + ")";
	return fExpr;
}
*/

/*
void ILTransInstrumentMCDCCoverage::AddCodeToModifyMCDCLast(std::string& flatCode, const int bid, const std::string varName, int condId) {
//	std::string mcdcVect = "BitmapCov::mcdcBitmap[" + std::to_string(bid) + "]";
//	std::string lastPos = mcdcVect + ".size() - 1";// "int last_pos = " + 
//	flatCode += mcdcVect + "[" + lastPos + "] |= (((unsigned int)" + varName + ") << " + std::to_string(condId) + ");\n";

    //[" + std::to_string(bid) + "]
	flatCode += "BitmapCov::mcdcDecTemp |= (((unsigned int)" + varName + ") << " + std::to_string(condId) + ");\n";
}
*/
/*
void ILTransInstrumentMCDCCoverage::AddCodeToPutMCDCLastToMCDCBitmapAndRefresh(std::string& flatCode, const int bid) {
	flatCode += "{\n";
	flatCode += "MCDCFunc mcdcFunc = BitmapCov::mcdcFuncs[" + std::to_string(bid) + "];\n";
    flatCode += "mcdcFunc(BitmapCov::mcdcDecTemp, BitmapCov::mcdcResultBitmap + BitmapCov::mcdcResultBitmapOffset[" + std::to_string(bid) + "]);\n";
    
    //flatCode += "std::set<unsigned int>* vect = BitmapCov::mcdcBitmap->at(" + std::to_string(bid) + ");\n";
	//flatCode += "vect->insert(BitmapCov::mcdcDecTemp[" + std::to_string(bid) + "]);\n";
	//flatCode += "BitmapCov::mcdcDecTemp[" + std::to_string(bid) + "] = 0;\n";
	flatCode += "}\n";
	//	std::string mcdcVect = "BitmapCov::mcdcBitmap[" + std::to_string(bid) + "]";
	//	flatCode += mcdcVect + ".push_back(0);\n";
}
*/
std::string ILTransInstrumentMCDCCoverage::generateMCDCFuncHead(int bid)
{
    return "void MCDCCovStatistic_" + std::to_string(bid) + "(int condValue, unsigned char* res)";
}

std::string ILTransInstrumentMCDCCoverage::generateMCDCFuncExpSplit()
{
    string func;
    for (int i = 0; i < MCDCCoverageLeafExp.size(); i++) {
        func += "unsigned char " + MCDCCoverageLeafExp[i].second + " = ((1 << " + to_string(i+1) + ") & condValue) != 0;\n";
    }
    func += MCDCCoverageFunc;
    return func;
}


std::string ILTransInstrumentMCDCCoverage::generateMCDCFuncMaskingCond()
{
    string func;
    auto it = MCDCCoverageLeafExp.begin();
    for (; it != MCDCCoverageLeafExp.end(); it++) {
        func += "unsigned char " + it->second + "_masked = ";

        std::string cond = "";
        const Expression* p = it->first->parentExpression;
        const Expression* c = it->first;
        while(p)
        {
            if(p->type == Token::Type::And || p->type == Token::Type::Or)
            {
                const Expression* other = p->childExpressions[0] == c ? p->childExpressions[1] : p->childExpressions[0];
                cond += MCDCCoverageSubExpMap.at(other) + " == " + ((p->type == Token::Type::And) ? "0" : "1") + " || ";
            }
            c = p;
            p = p->parentExpression;
        }

        if(cond.empty())
            func += "0;\n";
        else
            func += cond.substr(0, cond.length() - 4) + ";\n";
    }
    return func;
}

std::string ILTransInstrumentMCDCCoverage::generateMCDCFuncRes()
{
    string func;
    for (int i = 0; i < MCDCCoverageLeafExp.size(); i++) {
        func += "res[" + to_string(i * 2) + "] |= " + MCDCCoverageLeafExp[i].second + " && !" + MCDCCoverageLeafExp[i].second + "_masked;\n";
        func += "res[" + to_string(i * 2 + 1) + "] |= !" + MCDCCoverageLeafExp[i].second + " && !" + MCDCCoverageLeafExp[i].second + "_masked;\n";
    }
    return func;
}
/*
void ILTransInstrumentMCDCCoverage::InsertMCDCToSwitch(const StmtSwitch* stmt, std::string& wholeCode)
{
	int bid = ILTransInstrumentBitmapHelper::mcdcBitmapCount;
	ILTransInstrumentBitmapHelper::mcdcBitmapCount++;

//	AddCodeToCreateNewMCDCLast(wholeCode, bid);

	std::string bbStr = std::to_string((long long)stmt);
	ILTransInstrumentBitmapHelper::mcdcIndexMap[bbStr] = bid;
	
	std::string inExpr = "";
	std::string dftExpr = "true";
	std::string dftExprShow = "true";
	int idx = 0;
	std::string allVar = "bool_" + std::to_string(bid) + "_" + std::to_string(idx);
	for (int i = 0; i < stmt->childStatements.size(); i++)
	{
		Statement* oneStmt = stmt->childStatements[i];
		if (oneStmt->type == Statement::Type::Case) {
			idx++;
			StmtCase* stmtTemp = static_cast<StmtCase*>(oneStmt);

			std::string exprStr = stmt->condition->expressionStr + " != " + stmtTemp->value->expressionStr;
			std::string idxVar = "bool_" + std::to_string(bid) + "_" + std::to_string(idx);
			
			inExpr += "bool " + idxVar + " = " + exprStr + ";\n";
			AddCodeToModifyMCDCLast(inExpr, bid, idxVar, idx);

			dftExpr += " && (" + idxVar + ")";
			dftExprShow += " && (" + exprStr + ")";
		}
	}
	inExpr += "bool " + allVar + " = " + dftExpr + ";\n";
	AddCodeToModifyMCDCLast(inExpr, bid, allVar, 0);

	ILTransInstrumentBitmapHelper::mcdcConditionNumInDecision[bid] = idx;

	wholeCode += inExpr;

	AddCodeToPutMCDCLastToMCDCBitmapAndRefresh(wholeCode, bid);

	ILTransInstrumentBitmapHelper::mcdcConditionDescInDecision[bid] = dftExprShow;
}
*/

std::string ILTransInstrumentMCDCCoverage::generateMCDCFuncRegister()
{
    string ret;

    ret += "#include \"BitmapCov.h\"\n\n";

    ret += MCDCCoverageFuncAll;

    ret += "void registerMCDCFunctions()";
    ret += "{\n";
    ret += "    BitmapCov::mcdcFuncs.clear();\n";
    for (int i = 0; i < ILTransInstrumentBitmapHelper::mcdcBitmapCount; i++) {
        ret += "    BitmapCov::mcdcFuncs.push_back(MCDCCovStatistic_" + std::to_string(i) + ");\n";
    }
    ret += "}\n";

    return ret;
}





