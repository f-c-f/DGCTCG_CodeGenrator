#include "ILTransMain.h"

#include <set>

#include "ILTranslator.h"
#include "ILTranslatorForTCGStateSearch.h"
#include "../Common/Utility.h"
#include "../ILBasic/Expression.h"

#include "../ILBasic/ILFunction.h"
#include "../ILBasic/ILInput.h"
#include "../ILBasic/ILOutput.h"
#include "../ILBasic/ILParser.h"
#include "../ILBasic/ILFile.h"
#include "../ILBasic/ILGlobalVariable.h"
#include "../ILBasic/ILLocalVariable.h"
#include "../ILBasic/ILStructDefine.h"
#include "../ILBasic/ILMember.h"
#include "ILTransInstrumentBitmapHelper.h"
#include "ILTransMetadataExporter.h"
#include "ILTranslatorUtility.h"
#include "../ILBasic/ILAnalyzeSchedule.h"
#include "../ILBasic/ILAnalyzeScheduleNode.h"
#include "../ILBasic/ILBranch.h"
#include "../ILBasic/ILSchedule.h"
#include "../ILBasic/StmtLocalVariable.h"
#include "../ILBasic/StmtIf.h"
#include "../ILBasic/StmtElse.h"
#include "../ILBasic/StmtElseIf.h"
#include "../ILBasic/StmtExpression.h"

using namespace std;

int ILTransMain::translateForTCGStateSearch(const ILParser* parser, string& code)
{
    iLParser = parser;
    string commentStr;
    
    int res = translateForTCGSSHeadFile();
    if(res < 0)
        return res;
    
	res = translateMainInstanceGlobalVariable();
    if(res < 0)
        return res;
    
    string mainCodeStr;
    res = translateForTCGSSMainFunction(parser, mainCodeStr);
    if(res < 0)
        return res;
    transCodeFunction += mainCodeStr;

    string stepCodeStr;
    res = translateForTCGSSStepFunction(parser, stepCodeStr);
    if(res < 0)
        return res;
    transCodeFunction += stepCodeStr;

	string retCode;
	retCode += transCodeHeadFile + transCodeStructDefine;
	retCode += transCodeMacro + transCodeTypeDefine;
	retCode += transCodeGlobalVariable + transCodeUtilityFunction;
	retCode += transCodeFunction;
	code = retCode;
	return 1;
}


int ILTransMain::translateForTCGSSHeadFile()
{
    transCodeHeadFile += "#include <stdio.h>\n";
    transCodeHeadFile += "#include <vector>\n";
    transCodeHeadFile += "#include <set>\n";
    transCodeHeadFile += "#include <string>\n";
    transCodeHeadFile += "#include <string.h>\n";
    transCodeHeadFile += "#include <time.h>\n";
    transCodeHeadFile += "#include \"Utility.h\"\n";
    transCodeHeadFile += "#include \"BitmapCov.h\"\n";
    transCodeHeadFile += "#include \"TCGStateSearchBasic.h\"\n";
    transCodeHeadFile += "#include \"TCGStateSearchBasicBranchRegister.h\"\n";
    transCodeHeadFile += "#include \"TCGStateSearchBasicBranchRelation.h\"\n";
    transCodeHeadFile += "#include \"TCGStateSearchBasicGlobalVariableRegister.h\"\n";
    transCodeHeadFile += "#include \"TCGStateSearchBasicInputRegister.h\"\n";
    transCodeHeadFile += "#include \"TCGStateSearchBasicParameterRegister.h\"\n";
    return 1;
}

string ILTransMain::translateForTCGSSMainStateSearchInit()
{
    string retCode = "// State Search init code\n";
    retCode += "BitmapCov::munitBitmapLength = " + std::to_string(ILTransInstrumentBitmapHelper::munitBitmapCount) + ";\n";
	retCode += "BitmapCov::branchBitmapLength = " + std::to_string(ILTransInstrumentBitmapHelper::branchBitmapCount) + ";\n";
	retCode += "BitmapCov::condBitmapLength = " + std::to_string(ILTransInstrumentBitmapHelper::condBitmapCount) + ";\n";
	retCode += "BitmapCov::decBitmapLength = " + std::to_string(ILTransInstrumentBitmapHelper::decBitmapCount) + ";\n";
	retCode += "BitmapCov::mcdcBitmapLength = " + std::to_string(ILTransInstrumentBitmapHelper::mcdcBitmapCount) + ";\n";
	
	retCode += "static const unsigned char _mcdcMetaInit[] = { ";
	for (int i = 0; i < ILTransInstrumentBitmapHelper::mcdcBitmapCount; i++) {
		std::map<int, int>::iterator itr = ILTransInstrumentBitmapHelper::mcdcConditionNumInDecision.find(i);
		if (itr != ILTransInstrumentBitmapHelper::mcdcConditionNumInDecision.end()) {
			int condNum = itr->second;
			retCode += std::to_string(condNum) + ", ";
		}
		else {
			printf("strange! not found mcdc condition? ");
		}
	}
	retCode += "};\n";
	retCode += "BitmapCov::mcdcMeta = new unsigned char[BitmapCov::mcdcBitmapLength];\n";
	retCode += "memcpy(BitmapCov::mcdcMeta, _mcdcMetaInit, BitmapCov::mcdcBitmapLength);\n";
    retCode += "BitmapCov::init();\n";
    retCode += "StateSearch::initStateSearch(argc, argv);\n";
    // retCode += "StateSearchBasicBranchRegister::loadStateSearchBranchData();\n";
    // retCode += "StateSearch::registStateBranch();\n";
    // 
    // retCode += "vector<pair<int, string>> StateSearch_BranchReadGlobalVar;\n";
    // retCode += "vector<pair<int, string>> StateSearch_BranchWriteGlobalVar;\n";
    // retCode += "StateSearchBasicBranchRelation::loadStateSearchBranchRelation(StateSearch_BranchReadGlobalVar, StateSearch_BranchWriteGlobalVar);\n";
    // retCode += "StateSearch::registStateBranchRelation(StateSearch_BranchReadGlobalVar, StateSearch_BranchWriteGlobalVar);\n";

    
    ILTranslatorForTCGStateSearch::transCodeBranchRegister += translateForTCGSSMainStateSearchRegistBranch();
    ILTranslatorForTCGStateSearch::transCodeBranchRelation += translateForTCGSSMainStateSearchBranchRelation();
    
    ILTranslatorForTCGStateSearch::transCodeBranchDistanceRegister += translateForTCGSSMainStateSearchBranchDistanceRegister();
    

    
    return retCode;
}

std::string ILTransMain::translateForTCGSSMainStateSearchRegistBranch()
{
    // �����ļ������ɷ�֧�б��������֧̫�ർ��main�ļ�����������
    string codeBranchRegister;
    codeBranchRegister += "#include \"TCGStateSearchBasicBranchRegister.h\"\n";
    codeBranchRegister += "map<int, string> StateSearchBasicBranchRegister::StateSearch_Branch;\n";
    codeBranchRegister += "map<int, int> StateSearchBasicBranchRegister::StateSearch_Branch_Depth;\n";
    codeBranchRegister += "map<int, int> StateSearchBasicBranchRegister::StateSearch_Branch_Parent;\n";
    codeBranchRegister += "map<int, int> StateSearchBasicBranchRegister::StateSearch_Branch_Brother;\n";
    codeBranchRegister += "map<int, vector<int>> StateSearchBasicBranchRegister::StateSearch_Branch_Children;\n";
    codeBranchRegister += "void StateSearchBasicBranchRegister::loadStateSearchBranchData()\n";
    codeBranchRegister += "{\n";
    for (int i = 0; i < ILTransInstrumentBitmapHelper::branchBitmapCount; i++)
    {
        codeBranchRegister += "    StateSearch_Branch[" + to_string(i) + "] = \"" + ILTransInstrumentBitmapHelper::branchCoverNameForCBMC[i] + "\";\n";
    }
    for (int i = 0; i < ILTransInstrumentBitmapHelper::branchBitmapCount; i++)
    {
        codeBranchRegister += "    StateSearch_Branch_Depth[" + to_string(i) + "] = " + to_string(ILTransInstrumentBitmapHelper::branchCoverCallDepth[i]) + ";\n";
    }
    codeBranchRegister += "    for(int i = 0; i < StateSearch_Branch.size(); i++)\n";
    codeBranchRegister += "    {\n";
    codeBranchRegister += "        StateSearch_Branch_Parent[i] = -1;\n";
    codeBranchRegister += "        StateSearch_Branch_Brother[i] = -1;\n";
    codeBranchRegister += "    }\n";

    for (int i = 0; i < ILTransInstrumentBitmapHelper::branchBitmapCount; i++)
    {
        int parent = ILTransInstrumentBitmapHelper::branchCoverParent[i];
        int brother = ILTransInstrumentBitmapHelper::branchCoverBrother[i];
        if (parent != -1)
        {
            codeBranchRegister += "    StateSearch_Branch_Parent[" + to_string(i) + "] = " + to_string(parent) + ";\n";
        }
        if (brother != -1)
        {
            codeBranchRegister += "    StateSearch_Branch_Brother[" + to_string(i) + "] = " + to_string(brother) + ";\n";
        }
    }

    codeBranchRegister += "}\n";
    return codeBranchRegister;
}


string ILTransMain::translateForTCGSSMainStateSearchRegistGlobalVariables()
{
    string retCode;
    // retCode += "// State Search Regist Global Variables code\n";
    // retCode += "StateSearchBasicGlobalVariableRegister::registAllGlobalVariables();\n";
    // retCode += "// State Search Regist Parameters code\n";
    // retCode += "StateSearchBasicParameterRegister::registAllModelParameters();\n";
    // retCode += "// State Search Regist Inputs code\n";
    // retCode += "StateSearchBasicInputRegister::registAllModelInputs();\n";


    // �����ļ�������ȫ�ֱ����б�������ȫ�ֱ���̫�ർ��main�ļ�����������
    string codeGlobalVariableRegister;
    codeGlobalVariableRegister += "#include \"TCGStateSearchBasicGlobalVariableRegister.h\"\n";
    codeGlobalVariableRegister += "#include \"define.h\"\n";
    codeGlobalVariableRegister += "#include \"TCGStateSearchBasic.h\"\n";
    codeGlobalVariableRegister += "void StateSearchBasicGlobalVariableRegister::registAllGlobalVariables()\n";
    codeGlobalVariableRegister += "{\n";
    for(int i = 0; i < iLParser->files.size(); i++)
    {
        ILFile* iLFile = iLParser->files[i];
        for(int j = 0; j < iLFile->globalVariables.size(); j++)
        {
            ILGlobalVariable* iLGlobalVariable = iLFile->globalVariables[j];
            string code = translateForTCGSSMainStateSearchRegistGlobalVariableOne(iLGlobalVariable);
            codeGlobalVariableRegister += code;
        }
    }
    codeGlobalVariableRegister += "}\n";
    ILTranslatorForTCGStateSearch::transCodeGlobalVariableRegister += codeGlobalVariableRegister;
    
    return retCode;
}

std::string ILTransMain::translateForTCGSSMainStateSearchRegistGlobalVariableOne(ILGlobalVariable* iLGlobalVariable)
{
    string retCode;
    if(iLGlobalVariable->isAddress)
        return retCode;
    translateForTCGSSMainStateSearchRegistGlobalVariableOne(iLGlobalVariable->name, iLGlobalVariable->type, iLGlobalVariable->arraySize, retCode);
    return retCode;
}

void ILTransMain::translateForTCGSSMainStateSearchRegistGlobalVariableOne(std::string varName, std::string type,
    std::vector<int> arraySize, std::string& retCode)
{
    string simpleType = ILParser::getRealDataType(type);
    simpleType = ILParser::getBasicDataTypeSimple(simpleType);

    string tempCode;
    if(ILParser::isBasicType(simpleType) && arraySize.size() <= 1) {
        string varNameCode = "arraySizeOf_" + stringReplaceAll(varName, ".", "_");
        varNameCode = stringReplaceAll(varNameCode, "[", "_");
        varNameCode = stringReplaceAll(varNameCode, "]", "_");

        tempCode = "    vector<int> " + varNameCode;
        int arrayLen = 0;
        if(!arraySize.empty()) {
            tempCode += " = {";
            //for(int i = 0; i < arraySize.size(); i++) {
                //if(i != 0)
                //    tempCode += ", ";
            //if(ILTranslatorForTCGStateSearch::configCutOffArray < arraySize[0])
            //    arrayLen = ILTranslatorForTCGStateSearch::configCutOffArray;
            //else
            arrayLen = arraySize[0];
            tempCode += to_string(arrayLen);
            //}
            tempCode += "};\n";
        } else {
            tempCode += ";\n";
        }
        tempCode += "    StateSearch::registGlobalVariables(\"" + varName + "\", ";
        tempCode += "\"" + simpleType + "\", ";
        tempCode += varNameCode + ", ";
        tempCode += "(char*)(&(" + varName + ")), ";
        tempCode += "sizeof(" + ILParser::convertDataType(simpleType) + ")";
        tempCode += arrayLen == 0 ? "" : " * " + to_string(arrayLen);
        tempCode += ");\n";
        retCode += tempCode;
        return;
    }

    if(!arraySize.empty()) {
        vector<int> arraySizeNew;
        arraySizeNew.insert(arraySizeNew.begin(), arraySize.begin() + 1, arraySize.end());
        for(int i = 0; i < arraySize[0] && i < ILTranslatorForTCGStateSearch::configCutOffArray; i++)
        {
            string varNameNew = varName + "[" + to_string(i) + "]";
            translateForTCGSSMainStateSearchRegistGlobalVariableOne(varNameNew, type, arraySizeNew, retCode);
        }
        return;
    }

    ILStructDefine* structDefine = ILParser::findStructType(type);
    for(int i = 0; i < structDefine->members.size(); i++)
    {
        ILMember* iLMember = structDefine->members[i];
        if(iLMember->isAddress)
            continue;
        string varNameNew = varName + "." + iLMember->name;
        string typeNew = iLMember->type;
        vector<int> arraySizeNew = iLMember->arraySize;
        translateForTCGSSMainStateSearchRegistGlobalVariableOne(varNameNew, typeNew, arraySizeNew, retCode);
    }

}


std::string ILTransMain::translateForTCGSSMainStateSearchBranchRelation()
{

    int res;
    // ����һ������ÿ����֧��д����Щȫ�ֱ���(ֻͳ��һ�������ģ��༶�����ķ�֧����)
    // ����һ������ÿ����֧������������Щȫ�ֱ���

    // ֮������������ű�����������ͼ������ͼ��?/ ������Բ�����ͼ��ֱ�����õڶ���������Ϣ���һЩ��֧�����ȼ�����
    // �ĸ���֧���������ͻ��ڵ�ǰ����״̬�����ȷ�����Щ��֧

    vector<pair<void*, std::string>> listBranchWriteGlobalVar;
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
                res = translateForTCGSSMainStateSearchRegistBranchRelationsCollectOneSchedule(iLSchedule, iLParser, listBranchWriteGlobalVar, listBranchReadGlobalVar);
                if (res < 0)
                    return "";
            }
        }
    }

    ILTransMetadataExporter::recordBranchRelationPairs(listBranchReadGlobalVar, listBranchWriteGlobalVar);

    string codeBranchRelation;
    codeBranchRelation += "#include \"TCGStateSearchBasicBranchRelation.h\"\n";
    codeBranchRelation += "void StateSearchBasicBranchRelation::loadStateSearchBranchRelation(vector<pair<int, string>>& StateSearch_BranchReadGlobalVar, vector<pair<int, string>>& StateSearch_BranchWriteGlobalVar)\n";
    codeBranchRelation += "{\n";
    for (int i = 0; i < listBranchReadGlobalVar.size(); i++)
    {
        if(ILTransInstrumentBitmapHelper::branchMapId.find(listBranchReadGlobalVar[i].first) == ILTransInstrumentBitmapHelper::branchMapId.end())
            continue;
        int branchId = ILTransInstrumentBitmapHelper::branchMapId.at(listBranchReadGlobalVar[i].first);
        codeBranchRelation += "    StateSearch_BranchReadGlobalVar.push_back(make_pair(" + to_string(branchId) + ", \"" + listBranchReadGlobalVar[i].second + "\"));\n";
    }
    for (int i = 0; i < listBranchWriteGlobalVar.size(); i++)
    {
        if (ILTransInstrumentBitmapHelper::branchMapId.find(listBranchWriteGlobalVar[i].first) == ILTransInstrumentBitmapHelper::branchMapId.end())
            continue;
        int branchId = ILTransInstrumentBitmapHelper::branchMapId.at(listBranchWriteGlobalVar[i].first);
        codeBranchRelation += "    StateSearch_BranchWriteGlobalVar.push_back(make_pair(" + to_string(branchId) + ", \"" + listBranchWriteGlobalVar[i].second + "\"));\n";
    }
    codeBranchRelation += "}\n";
    return codeBranchRelation;
}


int ILTransMain::translateForTCGSSMainStateSearchRegistBranchRelationsCollectOneSchedule(
    ILSchedule* iLSchedule, const ILParser* parser,
    vector<pair<void*, std::string>>& listBranchWriteGlobalVar,
    vector<pair<void*, std::string>>& listBranchReadGlobalVar)
{
    ILAnalyzeSchedule* analyzeSchedule = new ILAnalyzeSchedule();
    ILTranslatorForTCGStateSearch::iLAnalyzeSchedule[iLSchedule] = analyzeSchedule;

    int res = analyzeSchedule->collectSchedule(iLSchedule, const_cast<ILParser*>(parser));
    if (res < 0)
        return res;

    map<string, string> globalVariablePointerMap; // Key: ָ�����?  Value: ȫ�ֱ�����(������A.b��ʽ)

    const std::vector<ILAnalyzeScheduleNode*>* scheduleNodeList = analyzeSchedule->getScheduleNodeList();

    translateForTCGSSMainStateSearchRegistBranchRelationsCollectOneScheduleCollectVarMap(iLSchedule, globalVariablePointerMap, scheduleNodeList, analyzeSchedule);

    // �������Ƚڵ�,��ȫ�ֱ�����д�б��м�������
    for(int i = 0; i < scheduleNodeList->size(); i++)
    {
        ILAnalyzeScheduleNode* node = (*scheduleNodeList)[i];
        // if (node->nodeType == ILAnalyzeScheduleNode::TypeStatement &&
        //     (node->statement->type == Statement::If ||
        //         node->statement->type == Statement::Else ||
        //         node->statement->type == Statement::ElseIf))
        // {
        //     Statement* temp = node->statement;
        //     while (temp->type == Statement::Else ||
        //         temp->type == Statement::ElseIf)
        //     {
        //         temp = temp->getPreviousStatement();
        //     }
        // 
        //     StmtIf* stmt = (StmtIf*)temp;
        //     Expression* cond = stmt->condition;
        //     if (stringStartsWith(cond->expressionStr, "mcdc_"))
        //         continue;
        // }

        if((node->nodeType == ILAnalyzeScheduleNode::TypeBranch && 
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
            // ----- ������֧��ȡ�ı��� ----- 

            // ��ȡ��ǰ��֧���ߵ�����ǰ���������ڵ��б�
            vector<ILAnalyzeScheduleNode*> dataflowNodeList = analyzeSchedule->getScheduleNodeListDataFlowOfOneNode(node);
            //schedule->printNodeList(dataflowNodeList);
            translateForTCGSSMainStateSearchRegistBranchRelationsCollectOneScheduleBranchVarRead(dataflowNodeList, listBranchReadGlobalVar, node, globalVariablePointerMap);
            

            // ----- ������֧д���ı��� ----- 
            // ��ȡ��ǰ��֧�ڲ����������ڵ��б�
            vector<ILAnalyzeScheduleNode*> dataflowInnerNodeList = analyzeSchedule->getScheduleNodeListContainByOneBranchNode(node, true);
            //schedule->printNodeList(dataflowInnerNodeList);
            translateForTCGSSMainStateSearchRegistBranchRelationsCollectOneScheduleBranchVarWrite(dataflowInnerNodeList, listBranchWriteGlobalVar, node, globalVariablePointerMap);
        }
    }



    return 1;
}

void ILTransMain::translateForTCGSSMainStateSearchRegistBranchRelationsCollectOneScheduleCollectVarMap(
    ILSchedule* iLSchedule,
    map<string, string>& globalVariablePointerMap,
    const std::vector<ILAnalyzeScheduleNode*>* scheduleNodeList,
    ILAnalyzeSchedule* analyzeSchedule)
{

    // 收集所有全局变量的指针，用于后续的变量跟踪   
    ILFunction* iLFunction = (ILFunction*)iLSchedule->parent;
    if (!iLFunction->inputs.empty())
        globalVariablePointerMap[iLFunction->inputs[0]->name] = iLFunction->inputs[0]->name;

    for (int i = 0; i < scheduleNodeList->size(); i++)
    {
        ILAnalyzeScheduleNode* node = (*scheduleNodeList)[i];

        if (node->variableRead.empty())
            continue;
        //if (node->variableRead[0]->variable->variableType != ILAnalyzeScheduleVariable::TypeInput)
        //    continue;
        //if (iLFunction->inputs.empty())
        //    continue;

        string localVarName;
        if (node->nodeType == ILAnalyzeScheduleNode::TypeStatement && node->statement->type == Statement::LocalVariable)
        {
            localVarName = ((StmtLocalVariable*)node->statement)->name;
        }
        else if(node->nodeType == ILAnalyzeScheduleNode::TypeLocalVariable)
        {
            localVarName = ((ILLocalVariable*)node->iLLocalVariable)->name;
        }
        else if (node->nodeType == ILAnalyzeScheduleNode::TypeStatement && node->statement->type == Statement::ExpressionWithSemicolon)
        {
            StmtExpression* stmt = (StmtExpression*)node->statement;
            if (stmt->expression->type == Token::Assign)
            {
                Expression* exp = (Expression*)stmt->expression->childExpressions[0];
                localVarName = exp->expressionStr;
                
            }
        }

        if(localVarName.empty())
            continue;

        if (!(analyzeSchedule->isExpressionVarLocalVariable(localVarName) && analyzeSchedule->isExpressionVarGlobalVariable(localVarName)))
        {
            continue;
        }

        const ILAnalyzeScheduleVariable* p = node->variableRead.back()->variable;
        while (p && p->parent)
        {
            p = p->parent;
        }
        //if (p->name != iLFunction->inputs[0]->name)
        //   continue;

        globalVariablePointerMap[localVarName] = node->variableRead.back()->variable->callName;
    }
}

void ILTransMain::translateForTCGSSMainStateSearchRegistBranchRelationsCollectOneScheduleBranchVarRead(
    const std::vector<ILAnalyzeScheduleNode*>& scheduleNodeDataflowList,
    vector<pair<void*, std::string>>& listBranchReadGlobalVar,
    ILAnalyzeScheduleNode* node,
    std::map<std::string, std::string>& globalVariablePointerMap)
{

    std::set<const ILAnalyzeScheduleVariable*> variableReadSet;
    std::vector<const ILAnalyzeScheduleVariable*> variableReadList;
    translateForTCGSSMainStateSearchRegistBranchRelationsCollectOneScheduleBranchVarReadGetDefaultList(node, variableReadList);
    //variableReadList.insert(variableReadList.end(), tempNode->variableRead.begin(), tempNode->variableRead.end());
    if (variableReadList.empty())
        return;

    for(int i = 0; i < variableReadList.size(); i++)
    {
        translateForTCGSSMainStateSearchRegistBranchRelationsCollectOneScheduleBranchVarReadFindSource(scheduleNodeDataflowList, scheduleNodeDataflowList.size() - 1, variableReadList[i], variableReadSet);
    }

    for (auto ele : variableReadSet)
    {
        string globalVarName;
        if (ele->variableType == ILAnalyzeScheduleVariable::TypeGlobalVariable)
        {
            globalVarName = ele->callName;
        }
        else if(ele->variableType == ILAnalyzeScheduleVariable::TypeStatementVariable)
        {
            const ILAnalyzeScheduleVariable* p = ele;
            bool find = false;
            while (p)
            {
                if (globalVariablePointerMap.find(p->callName) != globalVariablePointerMap.end())
                {
                    find = true;
                    break;
                }
                p = p->parent;
            }
            if (find)
            {
                globalVarName = globalVariablePointerMap.at(p->callName) + ele->callName.substr(p->callName.length(), ele->callName.length() - p->callName.length());
            }
        }
        else if (ele->variableType == ILAnalyzeScheduleVariable::TypeInput)
        {
            const ILAnalyzeScheduleVariable* p = ele;
            if (globalVariablePointerMap.find(p->callName) != globalVariablePointerMap.end())
            {
                globalVarName = globalVariablePointerMap.at(p->callName) + ele->callName.substr(p->callName.length(), ele->callName.length() - p->callName.length());
            }
            
        }
        if(globalVarName.empty())
        {
            continue;
        }
        bool repeat = false;
        for(int i = 0; i < listBranchReadGlobalVar.size(); i++)
        {
            if (listBranchReadGlobalVar[i].first == node->statement && listBranchReadGlobalVar[i].second == globalVarName)
            {
                repeat = true;
                break;
            }
        }
        if(!repeat)
        {
            listBranchReadGlobalVar.push_back(make_pair(node->statement, globalVarName));
        }
    }
}

void ILTransMain::translateForTCGSSMainStateSearchRegistBranchRelationsCollectOneScheduleBranchVarReadGetDefaultList(
    ILAnalyzeScheduleNode* node, std::vector<const ILAnalyzeScheduleVariable*>& variableReadList)
{
    ILAnalyzeScheduleNode* tempNode = node;
    if(node->nodeType == ILAnalyzeScheduleNode::TypeStatement)
    {
        if (node->statement->type == Statement::Else)
        {
            tempNode = node->prevNode;
        }
        // ����ElseIf���Read�ı���
        while (tempNode->statement->type == Statement::ElseIf)
        {
            for (auto ele : tempNode->variableRead)
            {
                variableReadList.push_back(ele->variable);
            }
            tempNode = tempNode->prevNode;
        }
        // ����If���Read�ı���
        for (auto ele : tempNode->variableRead)
        {
            variableReadList.push_back(ele->variable);
        }
    }
    else if(node->nodeType == ILAnalyzeScheduleNode::TypeBranch)
    {
        if (node->iLBranch->type == ILBranch::TypeElse)
        {
            tempNode = node->prevNode;
        }
        // ����ElseIf���Read�ı���
        while (node->iLBranch->type == ILBranch::TypeElseIf)
        {
            for (auto ele : tempNode->variableRead)
            {
                variableReadList.push_back(ele->variable);
            }
            tempNode = tempNode->prevNode;
        }
        // ����If���Read�ı���
        for (auto ele : tempNode->variableRead)
        {
            variableReadList.push_back(ele->variable);
        }
    }
}

void ILTransMain::translateForTCGSSMainStateSearchRegistBranchRelationsCollectOneScheduleBranchVarReadFindSource(
    const std::vector<ILAnalyzeScheduleNode*>& scheduleNodeDataFlowList, int nodeIndex, const ILAnalyzeScheduleVariable* var, std::set<const ILAnalyzeScheduleVariable*>& variableReadSet)
{
    variableReadSet.insert(var);
    bool needAddReadVar = false;
    ILAnalyzeScheduleNode* dataflowNode = nullptr;
    int i;
    for(i = nodeIndex - 1; i >= 0; i--)
    {
        dataflowNode = scheduleNodeDataFlowList[i];
        for(int j = 0; j < dataflowNode->variableStored.size(); j++)
        {
            if (dataflowNode->variableStored[j]->variable == var)
            {
                needAddReadVar = true;
                break;
            }
        }
        if (needAddReadVar)
        {
            break;
        }
    }
    if (needAddReadVar)
    {
        for (auto ele : dataflowNode->variableRead)
        {
            translateForTCGSSMainStateSearchRegistBranchRelationsCollectOneScheduleBranchVarReadFindSource(scheduleNodeDataFlowList, i, ele->variable, variableReadSet);
        }
    }
}

void ILTransMain::translateForTCGSSMainStateSearchRegistBranchRelationsCollectOneScheduleBranchVarWrite(
    const std::vector<ILAnalyzeScheduleNode*>& scheduleNodeDataFlowList,
    std::vector<std::pair<void*, std::string>>& listBranchWriteGlobalVar, ILAnalyzeScheduleNode* node,
    std::map<std::string, std::string>& globalVariablePointerMap)
{
    for (auto innerNode : scheduleNodeDataFlowList)
    {
        for (auto eleRef : innerNode->variableStored)
        {
            string globalVarName;
            const ILAnalyzeScheduleVariable* ele = eleRef->variable;
            if (ele->variableType == ILAnalyzeScheduleVariable::TypeGlobalVariable)
            {
                globalVarName = ele->callName;
            }
            else if (ele->variableType == ILAnalyzeScheduleVariable::TypeStatementVariable)
            {
                const ILAnalyzeScheduleVariable* p = ele;
                bool find = false;
                while (p)
                {
                    if (globalVariablePointerMap.find(p->callName) != globalVariablePointerMap.end())
                    {
                        find = true;
                        break;
                    }
                    p = p->parent;
                }
                if (find)
                {
                    globalVarName = globalVariablePointerMap.at(p->callName) + ele->callName.substr(p->callName.length(), ele->callName.length() - p->callName.length());
                }

            }
            else if (ele->variableType == ILAnalyzeScheduleVariable::TypeInput)
            {
                const ILAnalyzeScheduleVariable* p = ele;
                if (globalVariablePointerMap.find(p->callName) != globalVariablePointerMap.end())
                {
                    globalVarName = globalVariablePointerMap.at(p->callName) + ele->callName.substr(p->callName.length(), ele->callName.length() - p->callName.length());
                }
            }
            if (globalVarName.empty())
            {
                continue;
            }
            bool repeat = false;
            for (int i = 0; i < listBranchWriteGlobalVar.size(); i++)
            {
                if (listBranchWriteGlobalVar[i].first == node->statement && listBranchWriteGlobalVar[i].second == globalVarName)
                {
                    repeat = true;
                    break;
                }
            }
            if (!repeat)
            {
                listBranchWriteGlobalVar.push_back(make_pair(node->statement, globalVarName));
            }
        }
    }
}

std::string ILTransMain::translateForTCGSSMainStateSearchBranchDistanceRegister()
{

    string codeDistanceRegister;
    codeDistanceRegister += "#include \"TCGStateSearchBasicDistanceRegister.h\"\n";
    codeDistanceRegister += "#include \"define.h\"\n";
    codeDistanceRegister += "#include \"TCGStateSearchBasic.h\"\n";
    codeDistanceRegister += "void TCGStateSearchBasicDistanceRegister::registAllDistance()\n";
    codeDistanceRegister += "{\n";
    //codeDistanceRegister += "    //int branchId = 0; // ��������Ӧ��branchId\n";
    //��ȥ��//codeDistanceRegister += "    //int distanceType = 0; // 1 ��ֵ����   2 �������\n";
    //codeDistanceRegister += "    //string varName;\n";
    //codeDistanceRegister += "    //char distanceOpType = 0; // ֻ����'>'��'<'\n";
    //codeDistanceRegister += "    //double targetValue = 0; // ��ֵ����ʱ��Ŀ��ֵ�� �������ʱ��Ŀ�����鳤��\n";
    // StateSearch::branchDistanceDataMap[0] = StateSearch::BranchDistanceData(34, 1, "ComplexParaTest2_DW->A", '>', 0);

    if (ILTranslatorForTCGStateSearch::configBranchDistanceGuidance) {
        for (int i = 0; i < ILTranslatorForTCGStateSearch::branchDistanceDataList.size(); i++)
        {
            //ILTranslatorForTCGStateSearch::branchDistanceBranchObjectList[BranchDistanceDataList[i].branchObject] = -1;

            codeDistanceRegister += "    StateSearch::branchDistanceDataMap[" + to_string(i) + "] = ";
            codeDistanceRegister += "StateSearch::BranchDistanceData(";
            codeDistanceRegister += to_string(ILTranslatorForTCGStateSearch::branchDistanceBranchObjectList[ILTranslatorForTCGStateSearch::branchDistanceDataList[i].branchObject]) + ", ";
            //codeDistanceRegister += "<<BranchDistancePlaceholder_" + to_string(i) + ">>" + ", ";
            //codeDistanceRegister += to_string(ILTranslatorForTCGStateSearch::branchDistanceDataList[i].distanceType) + ", ";
            codeDistanceRegister += "\"" + ILTranslatorForTCGStateSearch::branchDistanceDataList[i].varName + "\", ";
            codeDistanceRegister += "'" + ILTranslatorForTCGStateSearch::branchDistanceDataList[i].distanceOpType + "', ";
            codeDistanceRegister += ILTranslatorForTCGStateSearch::branchDistanceDataList[i].targetValue + ");\n";
        }
    }

    if (ILTranslatorForTCGStateSearch::configArrayDistanceGuidance) {
        int i = 0;
        for (auto item : ILTranslatorForTCGStateSearch::arrayDistanceBranchObjectList)
        {
            // StateSearch::arrayDistanceDataMap[0] = StateSearch::ArrayDistanceData("LANSwitch_DW_instance.PortPool", (unsigned char*)LANSwitch_DW_instance.PortPool, 256 * sizeof(int32_T));
            codeDistanceRegister += "    StateSearch::arrayDistanceDataMap[" + to_string(i) + "] = ";
            codeDistanceRegister += "StateSearch::ArrayDistanceData(\"" + item.first + "\", ";
            codeDistanceRegister += "(unsigned char*)" + item.first + ", ";
            codeDistanceRegister += item.second.first + " * sizeof(" + item.second.second + ")" + ");\n";

            i++;
        }
    }

    codeDistanceRegister += "}\n";
    return codeDistanceRegister;
}

std::string ILTransMain::translateForTCGSSMainInit()
{
    string retCode = "// init code\n";
    if(!iLParser->config.mainInitFunction.empty()) {
	    ILFunction* mainInitFunction = iLParser->findILFunction(iLParser->config.mainInitFunction);
        if(!mainInitFunction)
            return ""; //-ILTranslator::ErrorCanNotFindMainInitFunction;

        // generate init function parameters
        retCode += translateMainInitParameter(mainInitFunction);

        // generate init function call
        retCode += translateMainInitFuncionCall(mainInitFunction);
	}
    return retCode;
}

string ILTransMain::translateForTCGSSMainLoop()
{
    string retCode = "";
    string commentStr;
    if(ILTranslator::configGenerateComment)
    {
        commentStr = "/* Main Loop of model */\n";
    }
    //retCode += "BitmapCov::step();\n";
    retCode += "StateSearch::initStateSearch2();\n";
    retCode += "int achived = 0;\n";
    //retCode += "if(argc >= 2 && isFolderExist(argv[1])){\n";
    //retCode += "    StateSearch::runExistTestCaseBinary(argv[1]);\n";
    //retCode += "}\n";
    retCode += "int i;\n";
    retCode += "int printId = 0;\n";
    retCode += "bool timeOut = false;\n";
    retCode += "StateSearch::testStartTime = clock();\n";

    retCode += commentStr;
    retCode += "clock_t testEndTime = clock();\n";
	retCode += "for(i = 0; i < StateSearch::paraMaxRunIterator && !timeOut; i++)\n";
	retCode += "{\n";
    retCode += "    if(StateSearch::finish > 0 || isFileExist(\"finish.txt\"))\n";
    retCode += "    {\n";
    retCode += "        break;\n";
    retCode += "    }\n";
    retCode += "    StateSearch::handlePause(); //�ж��Ƿ���Ҫ��ͣ��Ȼ�������ͣ\n";
    //retCode += "    //string printIdStr = to_string(printId++);\n";
    //retCode += "    //printIdStr = stringRepeat(\"0\",4-printIdStr.length()) + printIdStr;\n";
    //retCode += "    //string tempPlainPath = exePath + \"/Temp/StateTraceTreeViz_svg\" + printIdStr + \".svg\";\n";
    //retCode += "    //StateSearch::printStateTraceTree(tempPlainPath);\n";
    retCode += "    achived = StateSearch::stateSearch(i, timeOut);\n";
    retCode += "    if(BitmapCov::branchCov == BitmapCov::branchBitmapLength || achived == -1)\n";
    retCode += "    {\n";
    retCode += "        break;\n";
    retCode += "    }\n";
    retCode += "    testEndTime = clock();\n";
    retCode += "    if(testEndTime - StateSearch::testStartTime > StateSearch::paraMaxRunTime)\n";
    retCode += "    {\n";
    retCode += "        timeOut = true;\n";
    retCode += "    }\n";
	retCode += "}\n";
    retCode += "cout << \"Achieved: \" << BitmapCov::branchCov << \"/\" << BitmapCov::branchBitmapLength;\n";
    //retCode += "string tempPlainPath = exePath + \"/Temp/StateTraceTreeViz_svg_final.svg\";\n";
    //retCode += "StateSearch::printStateTraceTree(tempPlainPath);\n";
    
    return retCode;
}

int ILTransMain::translateForTCGSSMainFunction(const ILParser* parser, std::string& code)
{
    int res = 0;
    string retCode;
    string commentStr;
    if(ILTranslator::configGenerateComment) {
        commentStr = "/* Main Function of model */\n";
    }


	retCode += commentStr + "int main(int argc, char *argv[]) {\n";
    retCode += "StateSearch::unwindCount = " + to_string(ILTranslatorForTCGStateSearch::configCutOffArray) + ";\n";
    retCode += "string exePath = getExeDirPath();\n";

    string codeStateSearchInit = translateForTCGSSMainStateSearchInit();
    retCode += codeStateSearchInit;
    
    string codeInit = translateForTCGSSMainInit();
    retCode += codeInit;

    string codeStateSearchRegistGlobalVariables = translateForTCGSSMainStateSearchRegistGlobalVariables();
    retCode += codeStateSearchRegistGlobalVariables;
    
    res = translateForTCGSSMainStateSearchRegistInputs(parser);
    if(res < 0)
        return res;

    res = translateForTCGSSMainStateSearchRegistParameters(parser);
    if (res < 0)
        return res;

    string codeLoop = translateForTCGSSMainLoop();
    retCode += codeLoop;

    retCode += "testEndTime = clock();\n";
    retCode += "long long testCostTime = (testEndTime - StateSearch::testStartTime);\n";
    retCode += "cout << \" run \" << i << \" times, cost : \" << testCostTime << \" ms\\n\";\n";
    retCode += "BitmapCov::printCoverage();\n";
    retCode += "StateSearch::terminateStateSearch();\n";
    retCode += "if(StateSearch::finish > 0) {\n";
    retCode += "    return StateSearch::finish;\n";
    retCode += "}\n";

    retCode += "return 0;\n";
	//retCode += commentStr + "for (int i = 0; i < " + to_string(config->iterationCount) + "; i++) {\n";
    
	retCode += "}\n";

    code = retCode;
    return 1;
}

int ILTransMain::translateForTCGSSStepFunction(const ILParser* parser, std::string& code)
{
    string retCode;

    ILFunction* mainExecFunction = nullptr;
    if(!iLParser->config.mainExecFunction.empty()) {
		mainExecFunction = iLParser->findILFunction(iLParser->config.mainExecFunction);
        if(!mainExecFunction)
            return -ILTranslator::ErrorCanNotFindMainExecFunction;
    } else {
        return -ILTranslator::ErrorCanNotFindMainExecFunction;
    }

    
    retCode += "bool tryStateBranch(char* data, int length)\n";
    retCode += "{\n";
    retCode += "// exec code\n";
    //retCode += "//int needDataLen = 43;\n";
    retCode += "bool enoughData = false;\n";
    retCode += "if(data && length >= StateSearch::inputsDataLengthTotal) {\n";
    retCode += "    enoughData = true;\n";
    //retCode += "    //if(length != StateSearch::inputsDataLengthTotal) {\n";
    //retCode += "    //    cout << \"TestCase Length Not Match: \" << length << \"!=\" << StateSearch::inputsDataLengthTotal << \"\\n\";\n";
    //retCode += "    //}\n";
    retCode += "}\n";

    // 复制执行函数的参�?    
    retCode += translateMainExecParameter(mainExecFunction);
    retCode += translateForTCGSSMainExecParameterCopy(mainExecFunction);
	//���������������������?	
    retCode += translateMainExecFuncionCall(mainExecFunction);
    //�������������ֵ�Ĵ�ӡ����?    //retCode += translateMainExecOutputPrint(mainExecFunction);

    retCode += "    return true;\n";
    retCode += "}\n";

    code = retCode;
    return 1;
}

namespace 
{
    int execInputCopyId = 0;
    int inputsDataLengthTotal = 0;
}
std::string ILTransMain::translateForTCGSSMainExecParameterCopy(const ILFunction* mainExecFunction) const
{
    string ret;
    execInputCopyId = 0;
    ret += "if(enoughData){\n";
    for(int i = 0; i < mainExecFunction->inputs.size(); i++) {
        ILInput* input = mainExecFunction->inputs[i];
		if(mainExecFunction->inputs[i]->type == iLParser->config.mainCompositeState) {
		    continue;
		}
        string paraName = "exec_" + input->name;
        ret += translateForTCGSSMainExecParameterCopyTraverse(paraName, input->type, input->arraySize, input->isAddress);
	}
    ret += "}\n";
    return ret;
}


std::string ILTransMain::translateForTCGSSMainExecParameterCopyTraverse(std::string name, std::string type,
    std::vector<int> arraySize, bool isAddress) const
{
    string ret;
    if(isAddress)
        return ret;
    type = ILParser::getRealDataType(type);
    type = ILParser::getBasicDataTypeSimple(type);
    if(ILParser::isBasicType(type) && arraySize.empty())
    {
        int typeWidth = ILParser::getBasicDataTypeWidth(type);
		ret += "memcpy(&" + name + ", ";
        ret += "data + ";
        ret += to_string(inputsDataLengthTotal);
        ret += ", " ;
        //ret += "stateTraceNode->inputVariablesOneStep->allInputVariable[" + to_string(execInputCopyId) + "].data, ";
        //ret += "sizeof(" + name + "));\n";
        ret += to_string(typeWidth) + ");\n";
        inputsDataLengthTotal += typeWidth;
        execInputCopyId++;
        return ret;
    }
    if(!arraySize.empty())
    {
        int count = arraySize[0];
        arraySize.erase(arraySize.begin());
        for(int i = 0; i < count && i < ILTranslatorForTCGStateSearch::configCutOffArray; i++)
        {
            ret += translateForTCGSSMainExecParameterCopyTraverse(name + "[" + to_string(i) + "]", type, arraySize, isAddress);
        }
        return ret;
    }

    ILStructDefine* structDefine = ILParser::findStructType(type);
    for(int i = 0; i < structDefine->members.size(); i++)
    {
        ILMember* member = structDefine->members[i];
        ret += translateForTCGSSMainExecParameterCopyTraverse(name + "." + member->name, member->type, member->arraySize, member->isAddress);
    }
    return ret;
}

int ILTransMain::translateForTCGSSMainStateSearchRegistInputs(const ILParser* parser)
{
    ILFunction* mainExecFunction = nullptr;
    if(!iLParser->config.mainExecFunction.empty()) {
		mainExecFunction = iLParser->findILFunction(iLParser->config.mainExecFunction);
        if(!mainExecFunction)
            return -ILTranslator::ErrorCanNotFindMainExecFunction;
    } else {
        return -ILTranslator::ErrorCanNotFindMainExecFunction;
    }

    string codeInputRegister;
    codeInputRegister += "#include \"TCGStateSearchBasicInputRegister.h\"\n";
    codeInputRegister += "#include \"define.h\"\n";
    codeInputRegister += "#include \"TCGStateSearchBasic.h\"\n";
    codeInputRegister += "void StateSearchBasicInputRegister::registAllModelInputs()\n";
    codeInputRegister += "{\n";
    for(int i = 0; i < mainExecFunction->inputs.size(); i++) {
        ILInput* input = mainExecFunction->inputs[i];
		if(mainExecFunction->inputs[i]->type == iLParser->config.mainCompositeState) {
		    continue;
		}
        string paraName = "exec_" + input->name;
        codeInputRegister += translateForTCGSSMainStateSearchRegistInputOne(paraName, input->type, input->arraySize, input->isAddress);
	}
    codeInputRegister += "}\n";
    ILTranslatorForTCGStateSearch::transCodeInputRegister += codeInputRegister;
    return 1;
}

std::string ILTransMain::translateForTCGSSMainStateSearchRegistInputOne(std::string name, std::string type,
    std::vector<int> arraySize, bool isAddress) const
{
    string ret;
    if(isAddress)
        return ret;
    type = ILParser::getRealDataType(type);
    type = ILParser::getBasicDataTypeSimple(type);
    if(ILParser::isBasicType(type) && arraySize.empty())
    {
        int typeWidth = ILParser::getBasicDataTypeWidth(type);
		ret += "    StateSearch::registModelInputs(\"" + name + "\", \"";
        ret += type;
        ret += "\", " ;
        ret += to_string(typeWidth) + ");\n";
        return ret;
    }
    if(!arraySize.empty())
    {
        int count = arraySize[0];
        arraySize.erase(arraySize.begin());
        for(int i = 0; i < count && i < ILTranslatorForTCGStateSearch::configCutOffArray; i++)
        {
            ret += translateForTCGSSMainStateSearchRegistInputOne(name + "[" + to_string(i) + "]", type, arraySize, isAddress);
        }
        return ret;
    }

    ILStructDefine* structDefine = ILParser::findStructType(type);
    for(int i = 0; i < structDefine->members.size(); i++)
    {
        ILMember* member = structDefine->members[i];
        ret += translateForTCGSSMainStateSearchRegistInputOne(name + "." + member->name, member->type, member->arraySize, member->isAddress);
    }
    return ret;
}

int ILTransMain::translateForTCGSSMainStateSearchRegistParameters(const ILParser* parser)
{
    string codeParameterRegister;
    codeParameterRegister += "#include \"TCGStateSearchBasicParameterRegister.h\"\n";
    codeParameterRegister += "#include \"define.h\"\n";
    codeParameterRegister += "#include \"TCGStateSearchBasic.h\"\n";
    codeParameterRegister += "void StateSearchBasicParameterRegister::registAllModelParameters()\n";
    codeParameterRegister += "{\n";
    for (int i = 0; i < parser->config.parameters.size(); i++) {
        const ILConfigParameter* para = &parser->config.parameters[i];
        string paraName = para->name;
        codeParameterRegister += translateForTCGSSMainStateSearchRegistParameterOne(paraName, para->type, para->arraySize, para->isAddress);
    }
    codeParameterRegister += "}\n";
    ILTranslatorForTCGStateSearch::transCodeParameterRegister += codeParameterRegister;
    return 1;
}

std::string ILTransMain::translateForTCGSSMainStateSearchRegistParameterOne(std::string name, std::string type,
    std::vector<int> arraySize, bool isAddress) const
{
    string ret;
    if (isAddress)
        return ret;
    type = ILParser::getRealDataType(type);
    type = ILParser::getBasicDataTypeSimple(type);
    if (ILParser::isBasicType(type) && arraySize.empty())
    {
        int typeWidth = ILParser::getBasicDataTypeWidth(type);
        ret += "    StateSearch::registModelParameters(\"" + name + "\", \"";
        ret += type;
        ret += "\", ";
        ret += "(char*)(&(";
        ret += name;
        ret += ")), ";
        ret += to_string(typeWidth) + ");\n";
        return ret;
    }
    if (!arraySize.empty())
    {
        int count = arraySize[0];
        arraySize.erase(arraySize.begin());
        for (int i = 0; i < count && i < ILTranslatorForTCGStateSearch::configCutOffArray; i++)
        {
            ret += translateForTCGSSMainStateSearchRegistParameterOne(name + "[" + to_string(i) + "]", type, arraySize, isAddress);
        }
        return ret;
    }

    ILStructDefine* structDefine = ILParser::findStructType(type);
    for (int i = 0; i < structDefine->members.size(); i++)
    {
        ILMember* member = structDefine->members[i];
        ret += translateForTCGSSMainStateSearchRegistParameterOne(name + "." + member->name, member->type, member->arraySize, member->isAddress);
    }
    return ret;
}
