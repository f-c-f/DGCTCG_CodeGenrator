#include "ILTransMain.h"

#include <set>

#include "ILTranslator.h"
#include "ILTranslatorForTCGHybrid.h"
#include "../Common/Utility.h"
#include "../ILBasic/Expression.h"

#include "../ILBasic/ILFunction.h"
#include "../ILBasic/ILInput.h"
#include "../ILBasic/ILOutput.h"
#include "../ILBasic/ILParser.h"
#include "../ILBasic/ILFile.h"
#include "../ILBasic/ILGlobalVariable.h"
#include "../ILBasic/ILStructDefine.h"
#include "../ILBasic/ILMember.h"
#include "ILTransInstrumentBitmapHelper.h"
#include "ILTranslatorUtility.h"
#include "../ILBasic/ILAnalyzeSchedule.h"
#include "../ILBasic/ILAnalyzeScheduleNode.h"
#include "../ILBasic/ILBranch.h"
#include "../ILBasic/ILSchedule.h"
#include "../ILBasic/StmtLocalVariable.h"

using namespace std;



int ILTransMain::translateForTCGHybrid(const ILParser* parser, string& code)
{
    iLParser = parser;
    string commentStr;
    
    int res = translateForTCGHybridHeadFile();
    if(res < 0)
        return res;
    
	res = translateMainInstanceGlobalVariable();
    if(res < 0)
        return res;
    
    string mainCodeStr;
    res = translateForTCGHybridMainFunction(parser, mainCodeStr);
    if(res < 0)
        return res;
    transCodeFunction += mainCodeStr;

    string stepCodeStr;
    res = translateForTCGHybridStepFunction(parser, stepCodeStr);
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


int ILTransMain::translateForTCGHybridHeadFile()
{
    transCodeHeadFile += "#include <stdio.h>\n";
    transCodeHeadFile += "#include <vector>\n";
    transCodeHeadFile += "#include <set>\n";
    transCodeHeadFile += "#include <string>\n";
    transCodeHeadFile += "#include <string.h>\n";
    transCodeHeadFile += "#include <time.h>\n";
    transCodeHeadFile += "#include \"Utility.h\"\n";
    transCodeHeadFile += "#include \"BitmapCov.h\"\n";
    transCodeHeadFile += "#include \"TCGHybridBasic.h\"\n";
    //transCodeHeadFile += "#include \"TCGHybridBasicBranchRegister.h\"\n";
    //transCodeHeadFile += "#include \"TCGHybridBasicGlobalVariableRegister.h\"\n";
    //transCodeHeadFile += "#include \"TCGHybridBasicInputRegister.h\"\n";
    //transCodeHeadFile += "#include \"TCGHybridBasicParameterRegister.h\"\n";
    return 1;
}

string ILTransMain::translateForTCGHybridMainHybridInit()
{
    string retCode = "// TCG Hybrid init code\n";
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
    retCode += "TCGHybrid::initTCGHybrid(argc, argv);\n";
    //retCode += "TCGHybridBasicBranchRegister::loadTCGHybridBranchData();\n";
    
    ILTranslatorForTCGHybrid::transCodeBranchRegister += translateForTCGHybridMainHybridRegistBranch();
    //ILTranslatorForTCGHybrid::transCodeBranchRelation += translateForTCGHybridMainHybridBranchRelation();
    ILTranslatorForTCGHybrid::transCodeBranchDistanceRegister += translateForHybridMainBranchDistanceRegister();
    
    
    return retCode;
}

std::string ILTransMain::translateForTCGHybridMainHybridRegistBranch()
{
    // 在新文件中生成分支列表，避免分支太多导致main文件看起来难以阅读   
    string codeBranchRegister;
    codeBranchRegister += "#include \"TCGHybridBasicBranchRegister.h\"\n";
    codeBranchRegister += "map<int, string> TCGHybridBasicBranchRegister::TCGHybrid_Branch;\n";
    codeBranchRegister += "map<int, int> TCGHybridBasicBranchRegister::TCGHybrid_Branch_Depth;\n";
    codeBranchRegister += "void TCGHybridBasicBranchRegister::loadTCGHybridBranchData()\n";
    codeBranchRegister += "{\n";
    for (int i = 0; i < ILTransInstrumentBitmapHelper::branchBitmapCount; i++)
    {
        codeBranchRegister += "    TCGHybrid_Branch[" + to_string(i) + "] = \"" + ILTransInstrumentBitmapHelper::branchCoverNameForCBMC[i] + "\";\n";
    }
    for (int i = 0; i < ILTransInstrumentBitmapHelper::branchBitmapCount; i++)
    {
        codeBranchRegister += "    TCGHybrid_Branch_Depth[" + to_string(i) + "] = " + to_string(ILTransInstrumentBitmapHelper::branchCoverCallDepth[i]) + ";\n";
    }
    codeBranchRegister += "}\n";
    return codeBranchRegister;
}


string ILTransMain::translateForTCGHybridMainHybridRegistGlobalVariables()
{
    string retCode = "TCGHybrid::initTCGHybrid2();\n";
    // string retCode = "// TCG Hybrid Regist Global Variables code\n";
    // retCode += "TCGHybridBasicGlobalVariableRegister::registAllGlobalVariables();\n";
    // retCode += "// TCG Hybrid Regist Parameters code\n";
    // retCode += "TCGHybridBasicParameterRegister::registAllParameters();\n";
    // retCode += "// TCG Hybrid Regist Inputs code\n";
    // retCode += "TCGHybridBasicInputRegister::registAllInputs();\n";



    // 在新文件中生成全局变量列表，避免全局变量太多导致main文件看起来难�?   
    string codeGlobalVariableRegister;
    codeGlobalVariableRegister += "#include \"TCGHybridBasicGlobalVariableRegister.h\"\n";
    codeGlobalVariableRegister += "#include \"define.h\"\n";
    codeGlobalVariableRegister += "#include \"TCGHybridBasic.h\"\n";
    codeGlobalVariableRegister += "void TCGHybridBasicGlobalVariableRegister::registAllGlobalVariables()\n";
    codeGlobalVariableRegister += "{\n";
    for(int i = 0; i < iLParser->files.size(); i++)
    {
        ILFile* iLFile = iLParser->files[i];
        for(int j = 0; j < iLFile->globalVariables.size(); j++)
        {
            ILGlobalVariable* iLGlobalVariable = iLFile->globalVariables[j];
            string code = translateForTCGHybridMainHybridRegistGlobalVariableOne(iLGlobalVariable);
            codeGlobalVariableRegister += code;
        }
    }
    codeGlobalVariableRegister += "}\n";
    ILTranslatorForTCGHybrid::transCodeGlobalVariableRegister += codeGlobalVariableRegister;
    
    return retCode;
}

std::string ILTransMain::translateForTCGHybridMainHybridRegistGlobalVariableOne(ILGlobalVariable* iLGlobalVariable)
{
    string retCode;
    if(iLGlobalVariable->isAddress)
        return retCode;
    translateForTCGHybridMainHybridRegistGlobalVariableOne(iLGlobalVariable->name, iLGlobalVariable->type, iLGlobalVariable->arraySize, retCode);
    return retCode;
}

void ILTransMain::translateForTCGHybridMainHybridRegistGlobalVariableOne(std::string varName, std::string type,
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
            //if(ILTranslatorForTCGHybrid::configCutOffArray < arraySize[0])
            //    arrayLen = ILTranslatorForTCGHybrid::configCutOffArray;
            //else
            arrayLen = arraySize[0];
            tempCode += to_string(arrayLen);
            //}
            tempCode += "};\n";
        } else {
            tempCode += ";\n";
        }
        tempCode += "    TCGHybrid::registGlobalVariables(\"" + varName + "\", ";
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
        for(int i = 0; i < arraySize[0] && i < ILTranslatorForTCGHybrid::configCutOffArray; i++)
        {
            string varNameNew = varName + "[" + to_string(i) + "]";
            translateForTCGHybridMainHybridRegistGlobalVariableOne(varNameNew, type, arraySizeNew, retCode);
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
        translateForTCGHybridMainHybridRegistGlobalVariableOne(varNameNew, typeNew, arraySizeNew, retCode);
    }

}

std::string ILTransMain::translateForHybridMainBranchDistanceRegister()
{

    string codeDistanceRegister;
    codeDistanceRegister += "#include \"TCGHybridBasicDistanceRegister.h\"\n";
    codeDistanceRegister += "#include \"define.h\"\n";
    codeDistanceRegister += "#include \"TCGHybridBasic.h\"\n";
    codeDistanceRegister += "void TCGHybridBasicDistanceRegister::registAllDistance()\n";
    codeDistanceRegister += "{\n";
    //codeDistanceRegister += "    //int branchId = 0; // ��������Ӧ��branchId\n";
    //��ȥ��//codeDistanceRegister += "    //int distanceType = 0; // 1 ��ֵ����   2 �������\n";
    //codeDistanceRegister += "    //string varName;\n";
    //codeDistanceRegister += "    //char distanceOpType = 0; // ֻ����'>'��'<'\n";
    //codeDistanceRegister += "    //double targetValue = 0; // ��ֵ����ʱ��Ŀ��ֵ�� �������ʱ��Ŀ�����鳤��\n";
    // StateSearch::branchDistanceDataMap[0] = StateSearch::BranchDistanceData(34, 1, "ComplexParaTest2_DW->A", '>', 0);


        for (int i = 0; i < ILTranslatorForTCGHybrid::branchDistanceDataList.size(); i++)
        {
            //ILTranslatorForTCGStateSearch::branchDistanceBranchObjectList[BranchDistanceDataList[i].branchObject] = -1;

            codeDistanceRegister += "    TCGHybrid::branchDistanceDataMap[" + to_string(i) + "] = ";
            codeDistanceRegister += "TCGHybrid::BranchDistanceData(";
            codeDistanceRegister += to_string(ILTranslatorForTCGHybrid::branchDistanceBranchObjectList[ILTranslatorForTCGHybrid::branchDistanceDataList[i].branchObject]) + ", ";
            //codeDistanceRegister += "<<BranchDistancePlaceholder_" + to_string(i) + ">>" + ", ";
            //codeDistanceRegister += to_string(ILTranslatorForTCGHybrid::branchDistanceDataList[i].distanceType) + ", ";
            codeDistanceRegister += "\"" + ILTranslatorForTCGHybrid::branchDistanceDataList[i].varName + "\", ";
            codeDistanceRegister += "'" + ILTranslatorForTCGHybrid::branchDistanceDataList[i].distanceOpType + "', ";
            codeDistanceRegister += ILTranslatorForTCGHybrid::branchDistanceDataList[i].targetValue + ");\n";
        }



        int i = 0;
        for (auto item : ILTranslatorForTCGHybrid::arrayDistanceBranchObjectList)
        {
            // TCGHybrid::arrayDistanceDataMap[0] = TCGHybrid::ArrayDistanceData("LANSwitch_DW_instance.PortPool", (unsigned char*)LANSwitch_DW_instance.PortPool, 256 * sizeof(int32_T));
            codeDistanceRegister += "    TCGHybrid::arrayDistanceDataMap[" + to_string(i) + "] = ";
            codeDistanceRegister += "TCGHybrid::ArrayDistanceData(\"" + item.first + "\", ";
            codeDistanceRegister += "(unsigned char*)" + item.first + ", ";
            codeDistanceRegister += item.second.first + " * sizeof(" + item.second.second + ")" + ");\n";

            i++;
        }
    

    codeDistanceRegister += "}\n";
    return codeDistanceRegister;
}
std::string ILTransMain::translateForTCGHybridMainInit()
{
    string retCode = "// init code\n";
    if(!iLParser->config.mainInitFunction.empty()) {
	    ILFunction* mainInitFunction = iLParser->findILFunction(iLParser->config.mainInitFunction);
        if(!mainInitFunction)
            return ""; //-ILTranslator::ErrorCanNotFindMainInitFunction;

        // init parameter declarations�?       
        std::string initVars = translateMainInitParameter(mainInitFunction);
        if (initVars.empty() && !iLParser->config.mainExecFunction.empty()) {
            ILFunction* mainExecFunction = iLParser->findILFunction(iLParser->config.mainExecFunction);
            if (mainExecFunction) {
                for (int i = 0; i < mainExecFunction->inputs.size(); i++) {
                    if (mainExecFunction->inputs[i]->type == iLParser->config.mainCompositeState)
                        continue;
                    initVars += translateMainInput(mainExecFunction->inputs[i], "init_");
                }
                for (int i = 0; i < mainExecFunction->outputs.size(); i++) {
                    initVars += translateMainOutput(mainExecFunction->outputs[i], "init_");
                }
            }
        }
        retCode += initVars;

        retCode += translateMainInitFuncionCall(mainInitFunction);
	}
    return retCode;
}

string ILTransMain::translateForTCGHybridMainLoop()
{
    string retCode = "";
    string commentStr;
    if(ILTranslator::configGenerateComment)
    {
        commentStr = "/* Main Loop of model */\n";
    }
    retCode += commentStr;
    // retCode += "BitmapCov::step();\n";
    // retCode += "BitmapCov::recordTotal();\n";
    // //retCode += "if(argc >= 2 && isFolderExist(argv[1])){\n";
    // //retCode += "    TCGHybrid::runExistTestCaseBinary(argv[1]);\n";
    // //retCode += "}\n";
    // retCode += "clock_t testEndTime = clock();\n";
    // 
    // retCode += "TCGHybrid::callFuzzer();\n";
    // retCode += "TCGHybrid::callHSTCG();\n";
    // retCode += "cout << \"Sleep 3s\" << endl;\n";
    // retCode += "_sleep(3000);\n";

    retCode += "int i;\n";
	retCode += "for(i = 0; testEndTime - testStartTime < TCGHybrid::paraMaxRunTime; i++)\n";
	retCode += "{\n";

    retCode += "    if (!TCGHybrid::stepTCGHybrid())\n";
    retCode += "    {\n";
    retCode += "        break;\n";
    retCode += "    }\n";
    // retCode += "    vector<TCGHybrid::TestCaseBinary> testCasesBinary;\n";
    // retCode += "    bool res = TCGHybrid::minimizeTestCasesBinary(exePath + \"/Fuzz/In\", testCasesBinary);\n";
    // retCode += "    if(res)\n";
    // retCode += "    {\n";
    // retCode += "        TCGHybrid::callSTCGForSolve();\n";
    // retCode += "    }\n";
    // retCode += "    else\n";
    // retCode += "    {\n";
    // retCode += "        cout << \"testCasesBinary is empty.\" << endl;\n";
    // retCode += "    }\n";
    //retCode += "    cout << \"Sleep 3s\" << endl;\n";
    //retCode += "    _sleep(3000);\n";
    retCode += "    testEndTime = clock();\n";


	retCode += "}\n";
    retCode += "cout << \"Achieved: \" << BitmapCov::branchCov << \"/\" << BitmapCov::branchBitmapLength;\n";
    //retCode += "string tempPlainPath = exePath + \"/Temp/StateTraceTreeViz_svg_final.svg\";\n";
    //retCode += "TCGHybrid::printStateTraceTree(tempPlainPath);\n";
    
    return retCode;
}

int ILTransMain::translateForTCGHybridMainFunction(const ILParser* parser, std::string& code)
{
    int res = 0;
    string retCode;
    string commentStr;
    if(ILTranslator::configGenerateComment) {
        commentStr = "/* Main Function of model */\n";
    }


	retCode += commentStr + "int main(int argc, char *argv[]) {\n";
    //retCode += "string exePath = getExeDirPath();\n";
    retCode += "clock_t testStartTime = clock();\n";
    retCode += "clock_t testEndTime = clock();\n";

    string codeHybridInit = translateForTCGHybridMainHybridInit();
    retCode += codeHybridInit;
    
    string codeInit = translateForTCGHybridMainInit();
    retCode += codeInit;

    string codeHybridRegistGlobalVariables = translateForTCGHybridMainHybridRegistGlobalVariables();
    retCode += codeHybridRegistGlobalVariables;
    
    res = translateForTCGHybridMainHybridRegistInputs(parser);
    if(res < 0)
        return res;

    res = translateForTCGHybridMainHybridRegistParameters(parser);
    if (res < 0)
        return res;

    string codeLoop = translateForTCGHybridMainLoop();
    retCode += codeLoop;

    retCode += "testEndTime = clock();\n";
    retCode += "long long testCostTime = (testEndTime - testStartTime);\n";
    retCode += "cout << \" run \" << i << \" times, cost : \" << testCostTime << \" ms\\n\";\n";
    retCode += "TCGHybrid::terminateTCGHybrid();\n";
    retCode += "return 0;\n";
	//retCode += commentStr + "for (int i = 0; i < " + to_string(config->iterationCount) + "; i++) {\n";
    
	retCode += "}\n";

    code = retCode;
    return 1;
}


namespace 
{
    int execInputCopyId = 0;
    int inputsDataLengthTotal = 0;

    int parameterCopyId = 0;
    int parasDataLengthTotal = 0;
}

int ILTransMain::translateForTCGHybridStepFunction(const ILParser* parser, std::string& code)
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
    retCode += "if(data && length >= TCGHybrid::inputsDataLengthTotal) {\n";
    retCode += "    enoughData = true;\n";
    //retCode += "    //if(length != StateSearch::inputsDataLengthTotal) {\n";
    //retCode += "    //    cout << \"TestCase Length Not Match: \" << length << \"!=\" << StateSearch::inputsDataLengthTotal << \"\\n\";\n";
    //retCode += "    //}\n";
    retCode += "}\n";

    //�����������Ĳ�������
    for (int i = 0; i < mainExecFunction->inputs.size(); i++) {
        if (mainExecFunction->inputs[i]->type == iLParser->config.mainCompositeState)
            continue;
        retCode += "    " + translateMainInput(mainExecFunction->inputs[i], "exec_");
    }
    for (int i = 0; i < mainExecFunction->outputs.size(); i++) {
        retCode += "    " + translateMainOutput(mainExecFunction->outputs[i], "exec_");
    }
    retCode += translateForTCGHybridMainExecInputCopy(mainExecFunction);
	//���������������������?	
    retCode += translateMainExecFuncionCall(mainExecFunction);
    //�������������ֵ�Ĵ�ӡ����?    //retCode += translateMainExecOutputPrint(mainExecFunction);

    retCode += "    return true;\n";
    retCode += "}\n";

    code = retCode;
    return 1;
    
}

std::string ILTransMain::translateForTCGHybridMainExecInputCopy(const ILFunction* mainExecFunction) const
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
        ret += translateForTCGHybridMainExecInputCopyTraverse(paraName, input->type, input->arraySize, input->isAddress);
	}
    ret += "}\n";
    return ret;
}


std::string ILTransMain::translateForTCGHybridMainExecInputCopyTraverse(std::string name, std::string type,
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
        for(int i = 0; i < count && i < ILTranslatorForTCGHybrid::configCutOffArray; i++)
        {
            ret += translateForTCGHybridMainExecInputCopyTraverse(name + "[" + to_string(i) + "]", type, arraySize, isAddress);
        }
        return ret;
    }

    ILStructDefine* structDefine = ILParser::findStructType(type);
    for(int i = 0; i < structDefine->members.size(); i++)
    {
        ILMember* member = structDefine->members[i];
        ret += translateForTCGHybridMainExecInputCopyTraverse(name + "." + member->name, member->type, member->arraySize, member->isAddress);
    }
    return ret;
}

int ILTransMain::translateForTCGHybridMainHybridRegistInputs(const ILParser* parser)
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
    codeInputRegister += "#include \"TCGHybridBasicInputRegister.h\"\n";
    codeInputRegister += "#include \"define.h\"\n";
    codeInputRegister += "#include \"TCGHybridBasic.h\"\n";
    codeInputRegister += "void TCGHybridBasicInputRegister::registAllInputs()\n";
    codeInputRegister += "{\n";
    for(int i = 0; i < mainExecFunction->inputs.size(); i++) {
        ILInput* input = mainExecFunction->inputs[i];
		if(mainExecFunction->inputs[i]->type == iLParser->config.mainCompositeState) {
		    continue;
		}
        string paraName = "exec_" + input->name;
        codeInputRegister += translateForTCGHybridMainHybridRegistInputOne(paraName, input->type, input->arraySize, input->isAddress);
	}
    codeInputRegister += "}\n";
    ILTranslatorForTCGHybrid::transCodeInputRegister += codeInputRegister;
    return 1;
}

std::string ILTransMain::translateForTCGHybridMainHybridRegistInputOne(std::string name, std::string type,
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
		ret += "    TCGHybrid::registInputs(\"" + name + "\", \"";
        ret += type;
        ret += "\", " ;
        ret += to_string(typeWidth) + ");\n";
        return ret;
    }
    if(!arraySize.empty())
    {
        int count = arraySize[0];
        arraySize.erase(arraySize.begin());
        for(int i = 0; i < count && i < ILTranslatorForTCGHybrid::configCutOffArray; i++)
        {
            ret += translateForTCGHybridMainHybridRegistInputOne(name + "[" + to_string(i) + "]", type, arraySize, isAddress);
        }
        return ret;
    }

    ILStructDefine* structDefine = ILParser::findStructType(type);
    for(int i = 0; i < structDefine->members.size(); i++)
    {
        ILMember* member = structDefine->members[i];
        ret += translateForTCGHybridMainHybridRegistInputOne(name + "." + member->name, member->type, member->arraySize, member->isAddress);
    }
    return ret;
}


std::string ILTransMain::translateForTCGHybridMainParameterCopy(const ILConfig* config) const
{
    string ret;
    parameterCopyId = 0;
    parasDataLengthTotal = 0;
    for (int i = 0; i < config->parameters.size(); i++) {
        const ILConfigParameter* para = &config->parameters[i];

        string paraName = para->name;
        ret += translateForTCGHybridMainParameterCopyTraverse(paraName, para->type, para->arraySize, para->isAddress);
    }
    return ret;
}


std::string ILTransMain::translateForTCGHybridMainParameterCopyTraverse(std::string name, std::string type,
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
        ret += "memcpy(&" + name + ", ";
        ret += "dataBuf + ";
        ret += to_string(parasDataLengthTotal);
        ret += ", ";
        ret += to_string(typeWidth) + ");\n";
        parasDataLengthTotal += typeWidth;
        parameterCopyId++;
        return ret;
    }
    if (!arraySize.empty())
    {
        int count = arraySize[0];
        arraySize.erase(arraySize.begin());
        for (int i = 0; i < count && i < ILTranslatorForTCGHybrid::configCutOffArray; i++)
        {
            ret += translateForTCGHybridMainParameterCopyTraverse(name + "[" + to_string(i) + "]", type, arraySize, isAddress);
        }
        return ret;
    }

    ILStructDefine* structDefine = ILParser::findStructType(type);
    for (int i = 0; i < structDefine->members.size(); i++)
    {
        ILMember* member = structDefine->members[i];
        ret += translateForTCGHybridMainParameterCopyTraverse(name + "." + member->name, member->type, member->arraySize, member->isAddress);
    }
    return ret;
}

int ILTransMain::translateForTCGHybridMainHybridRegistParameters(const ILParser* parser)
{
    string codeParameterRegister;
    codeParameterRegister += "#include \"TCGHybridBasicParameterRegister.h\"\n";
    codeParameterRegister += "#include \"define.h\"\n";
    codeParameterRegister += "#include \"TCGHybridBasic.h\"\n";
    codeParameterRegister += "void TCGHybridBasicParameterRegister::registAllParameters()\n";
    codeParameterRegister += "{\n";
    for (int i = 0; i < parser->config.parameters.size(); i++) {
        const ILConfigParameter* para = &parser->config.parameters[i];
        string paraName = para->name;
        codeParameterRegister += translateForTCGHybridMainHybridRegistParameterOne(paraName, para->type, para->arraySize, para->isAddress);
    }
    codeParameterRegister += "}\n";
    ILTranslatorForTCGHybrid::transCodeParameterRegister += codeParameterRegister;
    return 1;
}

std::string ILTransMain::translateForTCGHybridMainHybridRegistParameterOne(std::string name, std::string type,
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
        ret += "    TCGHybrid::registParameters(\"" + name + "\", \"";
        ret += type;
        ret += "\", ";
        ret += to_string(typeWidth) + ");\n";
        return ret;
    }
    if (!arraySize.empty())
    {
        int count = arraySize[0];
        arraySize.erase(arraySize.begin());
        for (int i = 0; i < count && i < ILTranslatorForTCGHybrid::configCutOffArray; i++)
        {
            ret += translateForTCGHybridMainHybridRegistParameterOne(name + "[" + to_string(i) + "]", type, arraySize, isAddress);
        }
        return ret;
    }

    ILStructDefine* structDefine = ILParser::findStructType(type);
    for (int i = 0; i < structDefine->members.size(); i++)
    {
        ILMember* member = structDefine->members[i];
        ret += translateForTCGHybridMainHybridRegistParameterOne(name + "." + member->name, member->type, member->arraySize, member->isAddress);
    }
    return ret;
}
