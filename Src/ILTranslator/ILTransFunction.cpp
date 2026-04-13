#include "ILTransFunction.h"

#include "ILTransFile.h"

#include "ILTranslator.h"
#include "ILTransSchedule.h"

#include "../Common/Utility.h"

#include "../ILBasic/ILFile.h"
#include "../ILBasic/ILFunction.h"
#include "../ILBasic/ILInput.h"
#include "../ILBasic/ILOutput.h"
#include "../ILBasic/ILParser.h"
#include "../ILBasic/ILStructDefine.h"
#include "../ILBasic/ILMember.h"
#include "../ILBasic/ILRef.h"
#include "../ILBasic/ILSchedule.h"
#include "../ILBasic/ILLocalVariable.h"
#include "../ILBasic/Expression.h"
#include "../ILBasic/ILCCodeParser.h"
#include "../ILBasic/ILCalculate.h"

using namespace std;

map<ILFunction::Type, string> ILTransFunction::functionTypeToStrMap = {
    pair<ILFunction::Type, string>(ILFunction::FunctionExec, "Update function"),
    pair<ILFunction::Type, string>(ILFunction::FunctionInit, "Init function"),
    pair<ILFunction::Type, string>(ILFunction::FunctionExit, "Exit function"),
	pair<ILFunction::Type, string>(ILFunction::FunctionNormal, "Normal function"),
    pair<ILFunction::Type, string>(ILFunction::ProgramInit, "Program Init function"),
    pair<ILFunction::Type, string>(ILFunction::ProgramExit, "Program Exit function"),
    pair<ILFunction::Type, string>(ILFunction::IterationStart, "Iteration Start function"),
    pair<ILFunction::Type, string>(ILFunction::IterationEnd, "Iteration End function"),
    pair<ILFunction::Type, string>(ILFunction::FunctionUnknown, "Other function"),
};

int ILTransFunction::translate(const ILFunction* function, ILTransFile* file) const
{
    string commentStr;
    if(ILTranslator::configGenerateComment)
    {
        commentStr = "/* ";
        commentStr += functionTypeToStrMap[function->type] + " of ";
        commentStr += ILTranslator::getRefComment(&(function->refs));
        commentStr += " */\n";
    }

	string functionHeadStr;
    if(function->isStatic)
        functionHeadStr += "static ";
    if(!function->returnType.empty())
        functionHeadStr += function->returnType + " ";
    else
        functionHeadStr += "void ";
    functionHeadStr += function->name + "(";

	if (ILTranslator::translateForC && ILTranslatorForC::configPackageParameter)
	{
		if (storePackageParameter(function, file))
		{
			for (auto& input : function->inputs)
			{
				if (input->name == "self")
				{
					string inputParaStr = translateFunctionInput(input);
					functionHeadStr += inputParaStr;
					break;
				}
				else
					continue;
			}
		}
		else
		{
			for (auto& input : function->inputs)
			{
				string inputParaStr = translateFunctionInput(input);
				functionHeadStr += inputParaStr;
			}
		}
	}
	else
	{

		for (int i = 0; i < function->inputs.size(); i++)
		{
			string inputParaStr = translateFunctionInput(function->inputs[i]);

			if (i != 0)
				functionHeadStr += ", ";
			functionHeadStr += inputParaStr;
		}

		for (int i = 0; i < function->outputs.size(); i++)
		{
			string outputParaStr = translateFunctionOutput(function->outputs[i]);

			if (i != 0 || !function->inputs.empty())
				functionHeadStr += ", ";
			functionHeadStr += outputParaStr;
		}
	}
	functionHeadStr += ")";

	ILTranslator::transCodeFunctionDefine += commentStr + functionHeadStr + ";\n";

	file->transCodeFunction += commentStr + functionHeadStr + " {\n";
	//file->transCodeFunction += "{\n";
    
	if (ILTranslator::translateForC && ILTranslatorForC::configPackageVariable)
	{
		storePackageVariable(function, file);
	}
	ILTransSchedule iLTransSchedule;
	int res = iLTransSchedule.translate(function->schedule, file);
	if(res < 0)
		return res;
	
	////?????????�?	//for(int i = 0; i < function->outputs.size();i++)
	//{
	//	string outputCodeStr = translateFucntionOutputAssign(function->outputs[i]);
	//	file->transCodeFunction += outputCodeStr;
	//}

	file->transCodeFunction += "}\n";

	return 1;
}

/*
* è¿”å›žå€¼ä¸ºboolç±»åž‹
* è¡¨ç¤ºä¼˜åŒ–æ²¡æœ‰å®Œæˆ�ï¼Œä¹Ÿåº”è¯¥ç»§ç»­æ‰§è¡Œä»£ç �ç”Ÿæˆ�
*/
bool ILTransFunction::storePackageParameter(const ILFunction* function, ILTransFile* file) const
{
	// æ‰¾åˆ°å¯¹åº”çš„StructDefine
	ILStructDefine* temp = nullptr;
	if (function->inputs.size() > 0 && function->inputs[0]->name == "self")
	{
		// ?????Struct (real type after TypeDefine)
		std::string structName = ILParser::getRealDataType(function->inputs[0]->type);
		// ???TypeDefine??????�?		
		// std::string structName = ILParser::getRealDataType(function->inputs[0]->type);
		for (auto structDeifne : file->file->structDefines)
		{
			if (structDeifne->name == structName)
				temp = structDeifne;
			else
				continue;
		}
	}
	else {
		// ä¸�å­˜åœ¨selfç»“æž„ä½“ï¼Œæ— æ³•è¿›è¡Œå�‚æ•°æ‰“åŒ…
		// æˆ–è®¸è€ƒè™‘å¼•å…¥Errorç±»åž‹
		return false;
	}

	// ??input
	for (auto& input : function->inputs)
	{
		if (input->name == "self")
			continue;
		if(!storePackageInput(input, temp))
			return false;
	}

	// ??Output
	for (auto& output : function->outputs)
	{
		if(!storePackageOutput(output, temp))
			return false;
	}
	return true;
}

bool ILTransFunction::storePackageVariable(const ILFunction* function, ILTransFile* file) const
{
	// ?????StructDefine
	ILStructDefine* temp = nullptr;
	if (function->inputs.size() > 0 && function->inputs[0]->name == "self")
	{
		// ?????Struct
		std::string structName = ILParser::getRealDataType(function->inputs[0]->type);
		for (auto structDeifne : file->file->structDefines)
		{
			if (structDeifne->name == structName)
				temp = structDeifne;
			else
				continue;
		}
	}
	else {
		// ???self????????????
		// ??????Error??
		return false;
	}

	// ??????LocalVariable?????Calculate
	// ??Calculate????Schedule????
	for (auto& variable : function->schedule->localVariables)
	{
		if (!storePackageLocalVariable(variable, temp))
			return false;
	}
	return true;
}

bool ILTransFunction::storePackageInput(const ILInput* input, ILStructDefine* structDefine) const
{
	if (input == nullptr || structDefine == nullptr)
		return false;
	ILMember* member = new ILMember(structDefine);
	structDefine->members.push_back(member);
	member->name = input->name;
	member->type = input->type;
	member->isAddress = input->isAddress;
	member->isConst = input->isConst;
	for (auto size : input->arraySize)
	{
		member->arraySize.push_back(size);
	}
	for (auto ref : input->refs)
	{
		member->refs.push_back(ref->clone(member));
	}
	return true;
}

bool ILTransFunction::storePackageOutput(const ILOutput* output, ILStructDefine* structDefine) const
{
	if (output == nullptr || structDefine == nullptr)
		return false;
	ILMember* member = new ILMember(structDefine);
	structDefine->members.push_back(member);
	member->name = output->name;
	member->type = output->type;
	member->isAddress = output->isAddress;
	member->isConst = output->isConst;
	for (auto size : output->arraySize)
	{
		member->arraySize.push_back(size);
	}
	for (auto ref : output->refs)
	{
		member->refs.push_back(ref->clone(member));
	}
	return true;
}

bool ILTransFunction::storePackageLocalVariable(const ILLocalVariable* variable, ILStructDefine* structDefine) const
{
	if (variable == nullptr || structDefine == nullptr)
		return false;
	// ?????localvariable??????
	//if (variable->defaultValue != nullptr)
	//{
	//	// localVariable???�?	//	std::string defaultValue = variable->name + variable->defaultValue->expressionStr;
	//	ILCalculate* calculate = new ILCalculate(variable->parent);
	//	ILCCodeParser codeParser;
	//	codeParser.parseCCode(defaultValue, &calculate->statements);
	//	calculate->name = variable->name;
	//	for (auto ref : variable->refs)
	//	{
	//		calculate->refs.push_back(ref->clone(calculate));
	//	}
	//	insertList.push_back(calculate);
	//}
	ILMember* member = new ILMember(structDefine);
	structDefine->members.push_back(member);
	member->name = variable->name;
	member->type = variable->type;
	member->isAddress = variable->isAddress;
	member->isConst = variable->isConst;
	member->align = variable->align;
	member->isVolatile = variable->isVolatile;
	for (auto size : variable->arraySize)
	{
		member->arraySize.push_back(size);
	}
	for (auto ref : variable->refs)
	{
		member->refs.push_back(ref->clone(member));
	}
	return true;
}

std::string ILTransFunction::translateFunctionInput(const ILInput* input) const
{
    string inputParaStr;
    if(input->isConst)
        inputParaStr += "const ";

    inputParaStr += ILParser::convertDataType(input->type);

    if(input->isAddress > 0)
	    inputParaStr += stringRepeat("*", input->isAddress);
    inputParaStr += " " + input->name;
    if(!input->arraySize.empty())
    {
	    for(int j = 0;j < input->arraySize.size(); j++)
	    {
            if(input->arraySize[j] == -1)
		        inputParaStr += "[]";
            else
                inputParaStr += "[" + to_string(input->arraySize[j]) + "]";
	    }
    }
    return inputParaStr;
}

std::string ILTransFunction::translateFunctionOutput(const ILOutput* output) const
{
    string outputParaStr;
        
    if(output->isConst)
        outputParaStr += "const ";
        
    outputParaStr += ILParser::convertDataType(output->type);
    
    if(output->arraySize.empty())
	    outputParaStr += "*";
	if(output->isAddress > 0)
	    outputParaStr += stringRepeat("*", output->isAddress);
	outputParaStr += " " + output->name;
	if(!output->arraySize.empty())
	{
		for(int j = 0;j < output->arraySize.size(); j++)
		{
            if(output->arraySize[j] == -1)
			    outputParaStr += "[]";
            else
                outputParaStr += "[" + to_string(output->arraySize[j]) + "]";
		}
	}
    return outputParaStr;
}


std::string ILTransFunction::translateFunctionOutputAssign(const ILOutput* output) const
{
    string outputCodeStr;
	if(!output->arraySize.empty())
	{
		vector<vector<int>> indexList;
		getFullPermutation(output->arraySize, indexList);
		for(int j = 0;j < indexList.size(); j++)
		{
			string indexStr;
			for(int k = 0; k < indexList[j].size(); k++)
			{
				indexStr +=  "[" + to_string(indexList[j][k]) + "]";
			}
			outputCodeStr += output->name + indexStr + " = " + output->sourceStr + indexStr + ";\n";
		}
	}
	else
	{
		outputCodeStr += "*" + output->name + " = " + output->sourceStr + ";\n";
	}
    return outputCodeStr;
}
