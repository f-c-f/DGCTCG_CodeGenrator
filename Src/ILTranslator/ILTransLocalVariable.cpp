#include "ILTransLocalVariable.h"

#include "ILTransFile.h"

#include "ILTranslator.h"
#include "../ILBasic/Expression.h"

#include "../ILBasic/ILParser.h"
#include "../ILBasic/ILLocalVariable.h"
#include "../Common/Utility.h"

#include "ILTranslatorForTCGStateSearch.h"
#include "ILTranslatorForTCGHybrid.h"

using namespace std;

int ILTransLocalVariable::translate(const ILLocalVariable* localVariable, ILTransFile* file) const
{
    string commentStr;
    if(ILTranslator::configGenerateComment)
    {
        commentStr = "/* ";
        commentStr += "Local Variable define of ";
        commentStr += ILTranslator::getRefComment(&(localVariable->refs));
        commentStr += " */\n";
    }

    string variableStr;

    if(localVariable->align > 0)
    {
        variableStr += "__declspec(aligned(" + to_string(localVariable->align) + ")) ";
    }
    
    if(localVariable->isConst)
        variableStr += "const ";

    variableStr += ILParser::convertDataType(localVariable->type);

	if(localVariable->isAddress > 0)
		variableStr += " " + stringRepeat("*", localVariable->isAddress);
	variableStr += " " + localVariable->name;
	if(!localVariable->arraySize.empty())
	{
		for(int i = 0;i < localVariable->arraySize.size(); i++)
		{
            int arrayLen = localVariable->arraySize[i];
            if(ILTranslator::translateForCBMC && arrayLen > ILTranslatorForCBMC::configCutOffArray)
                arrayLen = ILTranslatorForCBMC::configCutOffArray;
			variableStr += "[" + to_string(arrayLen) + "]";
		}
	}
	if(localVariable->defaultValue)
	{
        if(ILTranslator::translateForCBMC && localVariable->defaultValue->type == Token::Compound) {
            variableStr += ILTranslatorForCBMC::getCutOffArrayDefaultValueExpStr(localVariable->name, localVariable->type, localVariable->arraySize, localVariable->defaultValue);
        }
	    else {
		    variableStr += " = " + localVariable->defaultValue->expressionStr;
        }
	}
    else
    {
        variableStr += " = ";
        if(ILParser::isBasicType(localVariable->type) && localVariable->arraySize.empty())
        {
            variableStr += "0";
        }
        else
        {
            variableStr += "{0}";
        }
    }
	variableStr += ";\n";

    if (ILTranslator::translateForC && ILTranslatorForC::configPackageVariable)
    {
        if(localVariable->defaultValue!=nullptr)
            file->transCodeFunction += commentStr + "self->" + localVariable->name + " = " + localVariable->defaultValue->expressionStr + ";\n";
    }
    else
    {
        file->transCodeFunction += commentStr + variableStr;
    }


    // STCG分支距离引导模式下，若局部变量存在默认值，则生成对应的距离值统计代码   
    if (ILTranslatorForTCGStateSearch::configBranchDistanceGuidance && 
        ILTranslator::translateForTCGStateSearch && localVariable->defaultValue)
    {
        std::vector<ILTranslatorForTCGStateSearch::BranchDistanceData>* branchDistanceDataList = &ILTranslatorForTCGStateSearch::branchDistanceDataList;
        for (int i = 0; i < branchDistanceDataList->size(); i++)
        {
            if (branchDistanceDataList->at(i).varName == localVariable->name)
            {
                // StateSearch::updateDistanceValue("ComplexParaTest2_DW->A", ComplexParaTest2_DW->A);
                file->transCodeFunction += "StateSearch::updateBranchDistanceValue(\"" + localVariable->name + "\", " + localVariable->name + ");\n";
                break;
            }
        }
    } 
    else if (ILTranslator::translateForTCGHybrid && localVariable->defaultValue)
    {
        std::vector<ILTranslatorForTCGHybrid::BranchDistanceData>* branchDistanceDataList = &ILTranslatorForTCGHybrid::branchDistanceDataList;
        for (int i = 0; i < branchDistanceDataList->size(); i++)
        {
            if (branchDistanceDataList->at(i).varName == localVariable->name)
            {
                // Hybrid::updateDistanceValue("ComplexParaTest2_DW->A", ComplexParaTest2_DW->A);
                file->transCodeFunction += "TCGHybrid::updateBranchDistanceValue(\"" + localVariable->name + "\", " + localVariable->name + ");\n";
                break;
            }
        }
    }
 

	return 1;
}
