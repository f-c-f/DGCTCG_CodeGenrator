#include "ILTranslatorForTCGStateSearch.h"

#include <fstream>
#include <map>

#include "ILTransFile.h"
#include "ILTransMain.h"

#include "../Common/Utility.h"
#include "../ILBasic/ILParser.h"
#include "../ILBasic/ILFile.h"
#include "../ILBasic/ILRef.h"
#include "../ILBasic/ILExternFile.h"
#include "../ILBasic/ILSchedule.h"
#include "../TCGBasic/TCGConfig.h"

#include "../ILBasic/ILLocalVariable.h"
#include "../ILBasic/ILBranch.h"

#include "../ILBasic/StmtLocalVariable.h"
#include "../ILBasic/StmtIf.h"
#include "../ILBasic/StmtElse.h"
#include "../ILBasic/StmtElseIf.h"
#include "../ILBasic/StmtExpression.h"
#include "../ILBasic/ILStructDefine.h"
#include "../ILBasic/ILMember.h"
#include "../ILBasic/ILGlobalVariable.h"

#include "ILTransExternFile.h"
#include "ILTranslator.h"
#include "ILTranslatorUtility.h"

#include "../ILBasic/ILAnalyzeSchedule.h"
#include "../ILBasic/ILAnalyzeScheduleNode.h"
#include "ILTransInstrumentMCDCCoverage.h"
#include "ILTransMetadataExporter.h"

using namespace std;

bool ILTranslatorForTCGStateSearch::configGenerateComment = true;
int ILTranslatorForTCGStateSearch::configCutOffArray = true;
bool ILTranslatorForTCGStateSearch::configBranchDistanceGuidance = true;
bool ILTranslatorForTCGStateSearch::configArrayDistanceGuidance = true;

ILParser* ILTranslatorForTCGStateSearch::iLParser;
TCGConfig* ILTranslatorForTCGStateSearch::tcgConfig;



std::string ILTranslatorForTCGStateSearch::transCodeBranchRegister;
std::string ILTranslatorForTCGStateSearch::transCodeBranchRelation;
std::string ILTranslatorForTCGStateSearch::transCodeGlobalVariableRegister;
std::string ILTranslatorForTCGStateSearch::transCodeInputRegister;
std::string ILTranslatorForTCGStateSearch::transCodeParameterRegister;
std::string ILTranslatorForTCGStateSearch::transCodeBranchDistanceRegister;

std::map<ILSchedule*, ILAnalyzeSchedule*> ILTranslatorForTCGStateSearch::iLAnalyzeSchedule;

std::map<const void*, int> ILTranslatorForTCGStateSearch::branchDistanceBranchObjectList;
std::map<const void*, std::set<std::string>> ILTranslatorForTCGStateSearch::branchDistanceStmtObjectList;

vector<ILTranslatorForTCGStateSearch::BranchDistanceData> ILTranslatorForTCGStateSearch::branchDistanceDataList;

std::map<std::string, pair<std::string, std::string>> ILTranslatorForTCGStateSearch::arrayDistanceBranchObjectList;

namespace 
{
    const int CONST_NUM_2 = 2;
}

ILTranslatorForTCGStateSearch::ILTranslatorForTCGStateSearch(ILParser* iLParser, TCGConfig* tcgConfig)
{
    ILTranslator::iLParser = iLParser;
    this->iLParser = iLParser;
    this->tcgConfig = tcgConfig;
}

int ILTranslatorForTCGStateSearch::translate(string outputDir)
{
    int res;

    ILTransMetadataExporter::reset();
    ILTranslatorUtility::buildILFunctionCallGraph(iLParser);

    if(ILTranslatorForTCGStateSearch::configBranchDistanceGuidance)
    {
        res = translateForTCGSSCalculateBranchDistanceRegister();
        if (res < 0)
            return res;
    }

    if (ILTranslatorForTCGStateSearch::configArrayDistanceGuidance)
    {
        res = translateForTCGSSCalculateArrayDistanceRegister(iLParser);
        if (res < 0)
            return res;
    }

	for(int i = 0; i < iLParser->files.size(); i++) {
		string name = iLParser->files[i]->name + (iLParser->files[i]->isHeadFile ? ".h" : ".cpp");
		string code;
		ILTransFile transFile;
		if((res = transFile.translate(iLParser->files[i], code)) < 0)
			return res;

		code = codeIndentProcess(code);
        if(stringEndsWith(name, ".cpp"))
        {
            code = "#include \"TCGStateSearchBasic.h\"\n" + code;
            code = "#include \"BitmapCov.h\"\n" + code;
        }
        code = "#include \"define.h\"\n" + code;
		fileCodes[name] = stringTrim(code);
	}

    for(int i = 0; i < iLParser->externFiles.size(); i++) {
		string name = iLParser->externFiles[i]->name;
		string code;
		ILTransExternFile transExternFile;
		if((res = transExternFile.translate(iLParser->externFiles[i], code)) < 0)
			return res;
        
		fileCodes[name] = stringTrim(code);
	}

	string mainCode;
	ILTransMain iLTransMain;
	if((res = iLTransMain.translate(iLParser, mainCode)) < 0) {
        ILTranslatorUtility::releaseILFunctionCallGraph();
		ILTranslator::setCurrentError(res, "");
		return res;
	}
	mainCode = codeIndentProcess(mainCode);
    mainCode = "#include \"define.h\"\n" + mainCode;
	fileCodes["main.cpp"] = stringTrim(mainCode);
	string defineCode = ILTranslator::transCodeHeadFile;
    //defineCode += getConfigTestRedefineOutputCode();
	defineCode += ILTranslator::transCodeMacro;
	defineCode += ILTranslator::transCodeTypeDefine;
	defineCode += ILTranslator::transCodeEnumDefine;
	defineCode += ILTranslator::transCodeStructDefine;
	defineCode += ILTranslator::transCodeGlobalVariable;
    defineCode += ILTranslator::transCodeUtilityFunction;
	defineCode += ILTranslator::transCodeFunctionDefine;
	defineCode += ILTranslator::transCodeFunction;
	defineCode = codeIndentProcess(defineCode);
	defineCode = stringTrim(defineCode);
	if(!defineCode.empty()) {
		defineCode = "#ifndef _DEFINE_H_\n#define _DEFINE_H_\n" + defineCode + "\n#endif\n";
        defineCode = stringReplaceAll(defineCode, "#define false 0\n", "");
        defineCode = stringReplaceAll(defineCode, "#define true 1\n", "");
        defineCode = stringReplaceAll(defineCode, "#define NULL 0\n", "");
		//cout << " ---- define.h ----\n";
		//cout << defineCode << "\n\n";
		writeFile(outputDir + "/define.h", defineCode);
	}
    if(!ILTranslator::transCodeUtilityFunctionImplement.empty()) {
        string utilityFunctionImplementCode = codeIndentProcess("#include \"define.h\"\n" + ILTranslator::transCodeUtilityFunctionImplement);
        writeFile(outputDir + "/define.c", utilityFunctionImplementCode);
    }
	for(map<string, string>::iterator iter = fileCodes.begin(); iter!=fileCodes.end();iter++) {
		//if(!defineCode.empty())
		//	iter->second = "#include \"define.h\"\n" + iter->second;

		//cout << " ---- " << iter->first << ".cpp ----\n";
		//cout << iter->second << "\n\n";
		writeFile(outputDir + "/" + iter->first, iter->second);
	}
    
    string MCDCFuncRegisterCode = ILTransInstrumentMCDCCoverage::generateMCDCFuncRegister();
    writeFile(outputDir + "/MCDCFuncRegister.cpp", MCDCFuncRegisterCode);
    
    res = copyResourceFiles(outputDir);
    if (res < 0)
        return res;
    if(!isFolderExist(outputDir + "/Temp"))
        createFolder(outputDir + "/Temp");
    writeFile(outputDir + "/TCGStateSearchBasicBranchRegister.cpp", transCodeBranchRegister);
    writeFile(outputDir + "/TCGStateSearchBasicBranchRelation.cpp", transCodeBranchRelation);
    writeFile(outputDir + "/TCGStateSearchBasicGlobalVariableRegister.cpp", transCodeGlobalVariableRegister);
    writeFile(outputDir + "/TCGStateSearchBasicInputRegister.cpp", transCodeInputRegister);
    writeFile(outputDir + "/TCGStateSearchBasicParameterRegister.cpp", transCodeParameterRegister);


    writeFile(outputDir + "/TCGStateSearchBasicDistanceRegister.cpp", transCodeBranchDistanceRegister);

    generateOneKeyBuildScript(outputDir);

    ILTransMetadataExporter::flush(outputDir, iLParser);
    ILTranslatorUtility::releaseILFunctionCallGraph();
	return 1;
}

void ILTranslatorForTCGStateSearch::setConfigGenerateComment(bool value)
{
    configGenerateComment = value;
}


void ILTranslatorForTCGStateSearch::setConfigCutOffArray(int value)
{
    configCutOffArray = value;
    if(configCutOffArray == 0)
        configCutOffArray = 1 << 30;
}

void ILTranslatorForTCGStateSearch::setConfigBranchDistanceGuidance(bool value)
{
    configBranchDistanceGuidance = value;
}

void ILTranslatorForTCGStateSearch::setConfigArrayDistanceGuidance(bool value)
{
    configArrayDistanceGuidance = value;
}



void ILTranslatorForTCGStateSearch::release()
{
    auto iter = iLAnalyzeSchedule.begin();
    
    for (; iter != iLAnalyzeSchedule.end(); iter++)
    {
        iter->second->release();
        delete iter->second;
    }
    iLAnalyzeSchedule.clear();
}

string ILTranslatorForTCGStateSearch::getConfigTestRedefineOutputCode() const
{
    //ï¿½ï¿½Ð©ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½0ï¿½æ»»ï¿½ï¿½Îªï¿½Ë±ï¿½Ö¤ï¿½æ»»ï¿½ï¿½Cï¿½ï·¨ï¿½ï¿½ï¿½ï¿½È·ï¿½ï¿½
    string ret;
    //ret += "#include <time.h>\n";
    //ret += "#include <stdio.h>\n";
    //ret += "#define fputc(c, stream) 0\n";
    //ret += "#define fputs(str, stream) 0\n";
    ret += "#define putc(c, stream) 0\n";
    ret += "#define putchar(c) 0\n";
    ret += "#define puts(str) 0\n";
    //ret += "#define vfprintf(stream, format, ...) 0\n";
    ret += "#define printf(format, ...) 0\n";
    //ret += "#define fprintf(stream, format, ...) 0\n";
    //ret += "#define fopen(filename, mode) 0\n";
    //ret += "#define fclose(fp) 0\n";
    //ret += "#define fread(ptr, size, nmemb, stream) 0\n";
    //ret += "#define fwrite(ptr, size, nmemb, stream) 0\n";
    return ret;
}


namespace 
{
    const vector<string> copyFileList = {
        "BitmapCov.cpp",
        "BitmapCov.h",
        "TCGStateSearchBasic.cpp",
        "TCGStateSearchBasic.h",
        "tinyxml2.cpp",
        "tinyxml2.h",
        "Utility.cpp",
        "Utility.h",
        //"BasicExcel.cpp",
        //"BasicExcel.hpp",
        "TCGStateSearchBasicGlobalVariableRegister.h",
        "TCGStateSearchBasicBranchRegister.h",
        "TCGStateSearchBasicBranchRelation.h",
        "TCGStateSearchBasicInputRegister.h",
        "ArgumentParser.h",
        "ArgumentParser.cpp",
        "TCGStateSearchBasicParameterRegister.h",
        "TCGStateSearchBasicDistanceRegister.h",
    };
}
int ILTranslatorForTCGStateSearch::copyResourceFiles(string outputDir)
{
    string exeFilePath = getExeDirPath();
    for(int i = 0; i < copyFileList.size(); i++)
    {
        if(!isFileExist(exeFilePath + "/ResourceCode/" + copyFileList[i]))
            return -ILTranslator::ErrorResourceFileNotExist;
        copyFileOne(exeFilePath + "/ResourceCode/" + copyFileList[i], outputDir + "/" + copyFileList[i]);
    }
    return 1;
}

int ILTranslatorForTCGStateSearch::translateForTCGSSCalculateBranchDistanceRegister()
{
    int res;
    // åå»ºä¸ä¸ªå®¹å¨ï¼ç¨äºå­å¨æ¯ä¸ªåæ¯æè¯»åçå¨å±åé

    vector<pair<void*, std::string>> listBranchReadGlobalVar;

    for (int i = 0; i < iLParser->files.size(); i++)
    {
        ILFile* iLFile = iLParser->files[i];
        for (int j = 0; j < iLFile->functions.size(); j++)
        {
            ILFunction* iLFunction = iLFile->functions[j];
            //for (int j = 0; j < iLParser->files[i]->functions.size(); j++)
            {
                ILSchedule* iLSchedule = iLFunction->schedule;
                // å¯¹æ¯ä¸ªå½æ°çè°åº¦ååæ¶éåæ¯è·ç¦»ä¿¡æ¯
                res = translateForTCGSSCalculateBranchDistanceCollectOneSchedule(iLSchedule, iLParser, listBranchReadGlobalVar);
                if (res < 0)
                    return res;
            }
        }
    }

    return 1;
}


int ILTranslatorForTCGStateSearch::translateForTCGSSCalculateBranchDistanceCollectOneSchedule(ILSchedule* iLSchedule, const ILParser* parser, std::vector<std::pair<void*, std::string>>& listBranchReadGlobalVar)
{
    ILAnalyzeSchedule* schedule = new ILAnalyzeSchedule();
    ILTranslatorForTCGStateSearch::iLAnalyzeSchedule[iLSchedule] = schedule;

    int res = schedule->collectSchedule(iLSchedule, const_cast<ILParser*>(parser));
    if (res < 0)
        return res;

    //map<string, string> globalVariablePointerMap; // Key: Ö¸ï¿½ï¿½ï¿½ï¿½ï¿?  Value: È«ï¿½Ö±ï¿½ï¿½ï¿½ï¿½ï¿½(ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½A.bï¿½ï¿½Ê½)

    const std::vector<ILAnalyzeScheduleNode*>* scheduleNodeList = schedule->getScheduleNodeList();

    //translateForTCGSSMainStateSearchBranchDistanceCollectOneScheduleCollectVarMap(iLSchedule, globalVariablePointerMap, scheduleNodeList);

    // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½È½Úµï¿½,ï¿½ï¿½È«ï¿½Ö±ï¿½ï¿½ï¿½ï¿½ï¿½Ð´ï¿½Ð±ï¿½ï¿½Ð¼ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½
    for (int i = 0; i < scheduleNodeList->size(); i++)
    {
        ILAnalyzeScheduleNode* node = (*scheduleNodeList)[i];
        if (node->nodeType == ILAnalyzeScheduleNode::TypeStatement &&
            (node->statement->type == Statement::If ||
                node->statement->type == Statement::Else ||
                node->statement->type == Statement::ElseIf))
        {
            Statement* temp = node->statement;
            while (temp->type == Statement::Else ||
                temp->type == Statement::ElseIf)
            {
                temp = temp->getPreviousStatement();
            }

            StmtIf* stmt = (StmtIf*)temp;
            Expression* cond = stmt->condition;
            if (stringStartsWith(cond->expressionStr, "mcdc_"))
                continue;
        }

        if ((node->nodeType == ILAnalyzeScheduleNode::TypeBranch &&
            (node->iLBranch->type == ILBranch::TypeIf ||
                node->iLBranch->type == ILBranch::TypeElse ||
                node->iLBranch->type == ILBranch::TypeElseIf)) ||
            (node->nodeType == ILAnalyzeScheduleNode::TypeStatement &&
                (node->statement->type == Statement::If ||
                    node->statement->type == Statement::Else ||
                    node->statement->type == Statement::ElseIf ||
                    node->statement->type == Statement::Case ||
                    node->statement->type == Statement::Default)))
        {


            // ----- ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ö§ï¿½ï¿½È¡ï¿½Ä±ï¿½ï¿½ï¿½ ----- 

            VarReadTreeNode root;

            // ï¿½ï¿½È¡ï¿½ï¿½Ç°ï¿½ï¿½Ö§ï¿½ï¿½ï¿½ßµï¿½ï¿½ï¿½ï¿½ï¿½Ç°ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Úµï¿½ï¿½Ð±ï¿½
            vector<ILAnalyzeScheduleNode*> dataflowNodeList = schedule->getScheduleNodeListDataFlowOfOneNode(node);
            //schedule->printNodeList(dataflowNodeList);
            translateForTCGSSCalculateBranchDistanceCollectOneScheduleBranchVarReadTree(dataflowNodeList, node, schedule, &root);

            root.release();
        }
    }

    return 1;
}



void ILTranslatorForTCGStateSearch::translateForTCGSSCalculateBranchDistanceCollectOneScheduleBranchVarReadTree(
    const std::vector<ILAnalyzeScheduleNode*>& scheduleNodeDataflowList,
    ILAnalyzeScheduleNode* node,
    ILAnalyzeSchedule* analyzeSchedule,
    VarReadTreeNode* varReadTreeRoot)
{

    //std::set<const ILAnalyzeScheduleVariable*> variableReadSet;
    //std::vector<const ILAnalyzeScheduleVariable*> variableReadList;

    //translateForTCGSSMainStateSearchBranchDistanceCollectOneScheduleBranchVarReadGetDefaultList(node, variableReadList);
    //variableReadList.insert(variableReadList.end(), tempNode->variableRead.begin(), tempNode->variableRead.end());
    //if (variableReadList.empty())
    //    return;

    if (node->nodeType == ILAnalyzeScheduleNode::TypeBranch && node->iLBranch &&
        (node->iLBranch->type == ILBranch::TypeElse || node->iLBranch->type == ILBranch::TypeFor))
    {
        return;
    }
    if (node->nodeType == ILAnalyzeScheduleNode::TypeStatement && node->statement &&
        (node->statement->type == Statement::Else || node->statement->type == Statement::For))
    {
        return;
    }

    vector<Expression*> innerExpressions = node->getNodeExpressions();

    for (int i = 0; i < innerExpressions.size(); i++)
    {
        translateForTCGSSCalculateBranchDistanceCollectOneScheduleBranchVarReadTreeTraverseExp(innerExpressions[i], varReadTreeRoot, analyzeSchedule, scheduleNodeDataflowList, scheduleNodeDataflowList.size() - 1, node, node);
    }


}

namespace {
    map<Token::Type, Token::Type> invertCompareOpMap = {
        make_pair(Token::Greater, Token::Less),
        make_pair(Token::GreaterEqual, Token::LessEqual),
        make_pair(Token::Less, Token::Greater),
        make_pair(Token::LessEqual, Token::GreaterEqual),
    };
}

void ILTranslatorForTCGStateSearch::translateForTCGSSCalculateBranchDistanceCollectOneScheduleBranchVarReadTreeTraverseExp(
    const Expression* exp,
    VarReadTreeNode* parent,
    ILAnalyzeSchedule* analyzeSchedule,
    const std::vector<ILAnalyzeScheduleNode*>& scheduleNodeDataFlowList,
    int nodeIndex,
    ILAnalyzeScheduleNode* curNode,
    ILAnalyzeScheduleNode* curBranchNode)
{

    VarReadTreeNode* newNode = new VarReadTreeNode();
    newNode->varName = exp->expressionStr;
    newNode->opType = exp->type;
    newNode->parent = parent;
    parent->children.push_back(newNode);

    if (exp->type == Token::Arrow ||
        exp->type == Token::Point ||
        exp->type == Token::Subscript)
    {
        if (analyzeSchedule->isExpressionVarGlobalVariable(exp->expressionStr))
        {
            newNode->isGlobalRelated = true;
        }
        return;
    }

    if (exp->isConstantExpression())
    {
        newNode->isConst = true;
        newNode->referedConstValue = exp->expressionStr;
        return;
    }

    if (exp->type == Token::Variable)
    {
        string referedConstValue = analyzeSchedule->getExpressionVarReferedConstantValue(exp->expressionStr);
        if (!referedConstValue.empty())
        {
            newNode->isConst = true;
            newNode->referedConstValue = referedConstValue;
            return;
        }
        if (analyzeSchedule->isExpressionVarParameter(exp->expressionStr))
        {
            newNode->isConst = true;
            return;
        }
        if (analyzeSchedule->isExpressionVarGlobalVariable(exp->expressionStr))
        {
            newNode->isGlobalRelated = true;
            return;
        }

        // ï¿½Ã±ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ç³ï¿½ï¿½ï¿½ï¿½Ä»ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Õ¹VarReadTree

        // ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Òµï¿½ï¿½Ã±ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½ï¿½Ô´
        string varName = exp->expressionStr;
        for (int i = nodeIndex - 1; i >= 0; i--)
        {
            ILAnalyzeScheduleNode* dataflowNode = scheduleNodeDataFlowList[i];

            if (dataflowNode->nodeType == ILAnalyzeScheduleNode::TypeLocalVariable)
            {
                ILLocalVariable* localVar = (ILLocalVariable*)dataflowNode->iLLocalVariable;
                if (localVar->defaultValue == nullptr)
                    continue;
                if (localVar->name != varName)
                    continue;

                translateForTCGSSCalculateBranchDistanceCollectOneScheduleBranchVarReadTreeTraverseExp(localVar->defaultValue, newNode, analyzeSchedule, scheduleNodeDataFlowList, i, dataflowNode, curBranchNode);
                if (newNode->children[0]->isGlobalRelated) {
                    newNode->isGlobalRelated = true;
                }
                if (newNode->children[0]->isConst) {
                    newNode->isConst = true;
                }

                break;
            }
            else if (dataflowNode->nodeType == ILAnalyzeScheduleNode::TypeStatement)
            {
                Statement* stmt = dataflowNode->statement;

                if (stmt->type == Statement::LocalVariable)
                {
                    StmtLocalVariable* localVar = (StmtLocalVariable*)stmt;
                    if (localVar->defaultValue == nullptr)
                        continue;
                    if (localVar->name != varName)
                        continue;

                    translateForTCGSSCalculateBranchDistanceCollectOneScheduleBranchVarReadTreeTraverseExp(localVar->defaultValue, newNode, analyzeSchedule, scheduleNodeDataFlowList, i, dataflowNode, curBranchNode);
                    if (newNode->children[0]->isGlobalRelated) {
                        newNode->isGlobalRelated = true;
                    }
                    if (newNode->children[0]->isConst) {
                        newNode->isConst = true;
                    }

                    break;
                }
                else if (stmt->type == Statement::ExpressionWithSemicolon)
                {
                    StmtExpression* stmtExp = (StmtExpression*)stmt;
                    Expression* exp = stmtExp->expression;
                    if (exp->type != Token::Assign)
                        continue;
                    string assignedVar = getVariableExpRootVarName(exp->childExpressions[0]->expressionStr);
                    if (assignedVar != varName)
                        continue;

                    translateForTCGSSCalculateBranchDistanceCollectOneScheduleBranchVarReadTreeTraverseExp(exp->childExpressions[1], newNode, analyzeSchedule, scheduleNodeDataFlowList, i, dataflowNode, curBranchNode);
                    if (newNode->children[0]->isGlobalRelated) {
                        newNode->isGlobalRelated = true;
                    }
                    if (newNode->children[0]->isConst) {
                        newNode->isConst = true;
                    }

                    break;
                }
            }

        }

        return;
    }

    for (int i = 0; i < exp->childExpressions.size(); i++)
    {
        translateForTCGSSCalculateBranchDistanceCollectOneScheduleBranchVarReadTreeTraverseExp(exp->childExpressions[i], newNode, analyzeSchedule, scheduleNodeDataFlowList, nodeIndex, curNode, curBranchNode);
        if (newNode->children[i]->isGlobalRelated) {
            newNode->isGlobalRelated = true;
        }
        if (newNode->children[i]->isConst && exp->childExpressions.size() == 1) {
            newNode->isConst = true;
        }
    }


    if (newNode->opType >= Token::Less && newNode->opType <= Token::GreaterEqual &&
        newNode->children.size() == 2)
    {
        Token::Type opType = Token::Unknow;
        BranchDistanceData branchDistanceData;
        if (newNode->children[0]->isConst && newNode->children[1]->isGlobalRelated)
        {
            branchDistanceData.varName = newNode->children[1]->varName;
            branchDistanceData.targetValue = newNode->children[0]->referedConstValue;
            opType = invertCompareOpMap[newNode->opType];

            cout << "Distance: " << newNode->children[0]->referedConstValue << " " << Token::getTokenStr(newNode->opType) << " " << newNode->children[1]->varName << endl;
        }
        else if (newNode->children[0]->isGlobalRelated && newNode->children[1]->isConst)
        {
            branchDistanceData.varName = newNode->children[0]->varName;
            branchDistanceData.targetValue = newNode->children[1]->referedConstValue;
            opType = newNode->opType;

            cout << "Distance: " << newNode->children[0]->varName << " " << Token::getTokenStr(newNode->opType) << " " << newNode->children[1]->referedConstValue << endl;
        }

        if (opType != Token::Unknow)
        {
            if (curBranchNode->nodeType == ILAnalyzeScheduleNode::TypeBranch)
                branchDistanceData.branchObject = curBranchNode->iLBranch;
            else if (curBranchNode->nodeType == ILAnalyzeScheduleNode::TypeStatement)
                branchDistanceData.branchObject = curBranchNode->statement;
            branchDistanceData.distanceType = 1;
            branchDistanceData.distanceOpType = (opType == Token::Less || opType == Token::LessEqual) ? '<' : '>';
            branchDistanceDataList.push_back(branchDistanceData);

            if (curNode->nodeType == ILAnalyzeScheduleNode::TypeBranch)
            {
                branchDistanceStmtObjectList[curNode->iLBranch].insert(branchDistanceData.varName);
            }
            else if (curNode->nodeType == ILAnalyzeScheduleNode::TypeStatement)
            {
                Statement* stmt = curNode->statement;
                while (stmt->type == Statement::Else || stmt->type == Statement::ElseIf)
                    stmt = stmt->getPreviousStatement();

                branchDistanceStmtObjectList[stmt].insert(branchDistanceData.varName);
            }
            
        }
    }

}


//std::string ILTranslatorForTCGStateSearch::repalceBranchDistancePlaceholder(std::string transCodeBranchDistanceRegister)
//{
//    for (auto item : branchDistanceBranchObjectList)
//    {
//        const void* branchDistanceObject = item.first;
//        int branchId = item.second;
//        string branchDistanceObjectStr = "<<BranchDistancePlaceholder_" + to_string(branchId) + ">>";
//        transCodeBranchDistanceRegister = stringReplaceAll(transCodeBranchDistanceRegister, branchDistanceObjectStr, to_string(branchId));
//    }
//    return transCodeBranchDistanceRegister;
//}
namespace {
    int arrayDistanceCount = 0;
}

int ILTranslatorForTCGStateSearch::translateForTCGSSCalculateArrayDistanceRegister(const ILParser* parser)
{
    arrayDistanceCount = 0;
    for (int i = 0; i < iLParser->files.size(); i++)
    {
        ILFile* iLFile = iLParser->files[i];
        for (int j = 0; j < iLFile->globalVariables.size(); j++)
        {
            ILGlobalVariable* iLGlobalVariable = iLFile->globalVariables[j];
            translateForTCGSSCalculateArrayDistanceRegisterOne(iLGlobalVariable);
        }
    }
    return 1;
}

void ILTranslatorForTCGStateSearch::translateForTCGSSCalculateArrayDistanceRegisterOne(ILGlobalVariable* iLGlobalVariable)
{
    if (iLGlobalVariable->isAddress)
        return;
    translateForTCGSSCalculateArrayDistanceRegisterOne(iLGlobalVariable->name, iLGlobalVariable->type, iLGlobalVariable->arraySize);
}

void ILTranslatorForTCGStateSearch::translateForTCGSSCalculateArrayDistanceRegisterOne(std::string varName, std::string type,
    std::vector<int> arraySize)
{
    string simpleType = ILParser::getRealDataType(type);
    simpleType = ILParser::getBasicDataTypeSimple(simpleType);

    //string tempCode;
    if (ILParser::isBasicType(simpleType)) {
        
        if (arraySize.empty())
            return;
        // StateSearch::arrayDistanceDataMap[0] = StateSearch::ArrayDistanceData("LANSwitch_DW_instance.PortPool", (unsigned char*)LANSwitch_DW_instance.PortPool, 256 * sizeof(int32_T));
        //tempCode += "StateSearch::arrayDistanceDataMap[" + to_string(arrayDistanceCount) + "]";
        //tempCode += " = StateSearch::ArrayDistanceData(\"" + varName + "\", (unsigned char*)" + varName + ", ";
        
        string arraySizeStr;
        for (int i = 0; i < arraySize.size(); i++)
        {
            if(i > 0)
                arraySizeStr += " * ";
            arraySizeStr += to_string(arraySize[i]);
        }
        
        arrayDistanceBranchObjectList[varName] = make_pair(arraySizeStr, type);

        //tempCode += " * sizeof(" + type + "));\n";

        cout << "Array Distance: " << varName << "  " << arraySizeStr << endl;

        //retCode += tempCode;
        return;
    }

    if (!arraySize.empty()) {
        vector<int> arraySizeNew;
        arraySizeNew.insert(arraySizeNew.begin(), arraySize.begin() + 1, arraySize.end());
        for (int i = 0; i < arraySize[0] && i < ILTranslatorForCBMC::configCutOffArray; i++)
        {
            string varNameNew = varName + "[" + to_string(i) + "]";
            translateForTCGSSCalculateArrayDistanceRegisterOne(varNameNew, type, arraySizeNew);
        }
        return;
    }

    ILStructDefine* structDefine = ILParser::findStructType(type);
    for (int i = 0; i < structDefine->members.size(); i++)
    {
        ILMember* iLMember = structDefine->members[i];
        if (iLMember->isAddress)
            continue;
        string varNameNew = varName + "." + iLMember->name;
        string typeNew = iLMember->type;
        vector<int> arraySizeNew = iLMember->arraySize;
        translateForTCGSSCalculateArrayDistanceRegisterOne(varNameNew, typeNew, arraySizeNew);
    }
}


int ILTranslatorForTCGStateSearch::generateOneKeyBuildScript(std::string outputDir) const
{
    string scriptStr;
    scriptStr = "g++ -O2 *.cpp -Wl,--stack=104857600 -o HSTCG.exe\n";
    writeFile(outputDir + "/Build.bat", scriptStr);

    scriptStr = "HSTCG.exe\n";
    writeFile(outputDir + "/Run.bat", scriptStr);

    return 1;
}
