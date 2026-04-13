#include "ILTransMain.h"

#include "ILTranslator.h"
#include "../Common/Utility.h"
#include "../ILBasic/Expression.h"

#include "../ILBasic/ILFunction.h"
#include "../ILBasic/ILInput.h"
#include "../ILBasic/ILOutput.h"
#include "../ILBasic/ILParser.h"
#include "../ILBasic/ILFile.h"

using namespace std;

int ILTransMain::translate(const ILParser* parser, string& code)
{
    if(ILTranslator::translateForC)
        return translateForC(parser, code);
    else if(ILTranslator::translateForCBMC)
        return translateForCBMC(parser, code);
    else if(ILTranslator::translateForTCGStateSearch)
        return translateForTCGStateSearch(parser, code);
    else if(ILTranslator::translateForLibFuzzer)
        return translateForLibFuzzer(parser, code);
    else if(ILTranslator::translateForTCGHybrid)
        return translateForTCGHybrid(parser, code);
	else if(ILTranslator::translateForCoverage)
		return translateForCoverage(parser, code);
	else if(ILTranslator::translateForSimulator)
		return translateForSimulator(parser, code);
	return 0;
}


std::string ILTransMain::translateMainInitParameter(const ILFunction* mainInitFunction) const
{
    string ret;
	for(int i = 0; i < mainInitFunction->inputs.size(); i++) {
		if(mainInitFunction->inputs[i]->type == iLParser->config.mainCompositeState)
			continue;
        
		ret += translateMainInput(mainInitFunction->inputs[i], "init_");
	}
	for(int i = 0; i < mainInitFunction->outputs.size(); i++) {
		ret += translateMainOutput(mainInitFunction->outputs[i], "init_");
	}
    return ret;
}

std::string ILTransMain::translateMainInitFuncionCall(const ILFunction* mainInitFunction) const
{
    string ret = mainInitFunction->name + "(";
    if(!mainInitFunction->inputs.empty() && mainInitFunction->inputs[0]->type == iLParser->config.mainCompositeState)
        ret += "&" +((ILFile*)(mainInitFunction->parent))->name + "_instance";

	for(int i = 0; i < mainInitFunction->inputs.size(); i++) {
		if(mainInitFunction->inputs[i]->type == iLParser->config.mainCompositeState)
			continue;
        ret += i == 0 ? "" : ", ";
		ret += "init_" + mainInitFunction->inputs[i]->name;
	}
	for(int i = 0; i < mainInitFunction->outputs.size(); i++) {
		ILOutput* iLOutput = mainInitFunction->outputs[i];
        ret += i == 0 && mainInitFunction->inputs.empty() ? "" : ", ";
        if(iLOutput->arraySize.empty())
		    ret += "&init_" + iLOutput->name;
        else
            ret += "init_" + iLOutput->name;
	}
	ret += ");\n";
    return ret;
}

std::string ILTransMain::translateMainExecParameter(const ILFunction* mainExecFunction, int step) const
{
    string ret;
	// 不进行参数打包
	if (!(ILTranslator::translateForC &&
        ILTranslatorForC::configPackageParameter))
	{
        string stepPerfix = (step == 1) ? "" : (to_string(step) + "_");
		for (int i = 0; i < mainExecFunction->inputs.size(); i++) {
			if (mainExecFunction->inputs[i]->type == iLParser->config.mainCompositeState)
				continue;

			ret += translateMainInput(mainExecFunction->inputs[i], "exec_" + stepPerfix);
		}
        
		for (int i = 0; i < mainExecFunction->outputs.size(); i++) {
			ret += translateMainOutput(mainExecFunction->outputs[i], "exec_" + stepPerfix);
		}
        
		if (ILTranslatorForCBMC::configRangeConstraint)
		{
			ret += translateMainFunctionRangeConstraint(step);
		}

        if (ILTranslatorForCBMC::configBranchCover)
        {
            for (int i = 0; i < mainExecFunction->inputs.size(); i++) {
                if (mainExecFunction->inputs[i]->type == iLParser->config.mainCompositeState)
                    continue;
                // string paraName = "exec_" + mainExecFunction->inputs[i]->name;
                // "__CPROVER_input(\"" + paraName + "\", " + paraName + ");\n";
                ret += translateMainFunctionInputCover(mainExecFunction->inputs[i], step);
            }
        }
        
	}
    return ret;
}

std::string ILTransMain::translateMainExecFuncionCall(const ILFunction* mainExecFunction, int step) const
{
    string ret = mainExecFunction->name + "(";
    if(!mainExecFunction->inputs.empty() && mainExecFunction->inputs[0]->type == iLParser->config.mainCompositeState)
        ret += "&" + ((ILFile*)(mainExecFunction->parent))->name + "_instance";
	// 不进行参数打包
	if (!(ILTranslator::translateForC &&
        ILTranslatorForC::configPackageParameter))
	{
        string stepPerfix = (step == 1) ? "" : (to_string(step) + "_");
		for (int i = 0; i < mainExecFunction->inputs.size(); i++) {
			if (mainExecFunction->inputs[i]->type == iLParser->config.mainCompositeState)
				continue;
			ret += i == 0 ? "" : ", ";
			ret += "exec_" + stepPerfix + mainExecFunction->inputs[i]->name;
		}
		for (int i = 0; i < mainExecFunction->outputs.size(); i++) {
			ILOutput* iLOutput = mainExecFunction->outputs[i];
			ret += i == 0 && mainExecFunction->inputs.empty() ? "" : ", ";
			if (iLOutput->arraySize.empty())
				ret += "&exec_" + stepPerfix + iLOutput->name;
			else
				ret += "exec_" + stepPerfix + iLOutput->name;
		}
	}
	ret += ");\n";
    return ret;
}

std::string ILTransMain::translateMainExecOutputPrint(const ILFunction* mainExecFunction)
{
    string ret;
    if(transCodeHeadFile.find("<stdio.h>") == string::npos)
        transCodeHeadFile += "#include <stdio.h>\n";

    for(int i = 0; i < mainExecFunction->outputs.size(); i++) {
		ILOutput* iLOutput = mainExecFunction->outputs[i];
		string name = iLOutput->name;
		string type = ILParser::convertDataType(iLOutput->type);

		if(type == "int" || type == "short")
			ret += "printf(\"" + name + ": %d\\n\", ";
		else if(type == "char")
			ret += "printf(\"" + name + ": %c\\n\", ";
		else if(type == "float")
			ret += "printf(\"" + name + ": %f\\n\", ";
		else if(type == "long long")
			ret += "printf(\"" + name + ": %ld\\n\", ";
		else if(type == "double")
			ret += "printf(\"" + name + ": %lf\\n\", ";
        else
            continue;
		if (ILTranslator::translateForC && ILTranslatorForC::configPackageParameter)
		{
			ret += ((ILFile*)(mainExecFunction->parent))->name + "_instance." + name;
		}else
			ret += "exec_" + name;
        for(int j = 0; j < iLOutput->arraySize.size(); j++)
        {
            ret += "[0]";
        }
        ret +=  ");\n";
	}
    return ret;
}

std::string ILTransMain::translateMainInput(const ILInput* input, std::string perfix) const
{
    string variableStr;
    string type = ILParser::convertDataType(input->type);
    variableStr += type;
	if(input->isAddress > 0)
		variableStr += " " + stringRepeat("*", input->isAddress);
	variableStr += " " + perfix + input->name;
	if(!input->arraySize.empty())
	{
		for(int i = 0;i < input->arraySize.size(); i++)
		{
			variableStr += "[" + to_string(input->arraySize[i]) + "]";
		}
	}
	if(input->defaultValue)
	{
		variableStr += " = " + input->defaultValue->expressionStr;
	}
	else if (ILTranslator::translateForSimulator)
	{
		int width = ILParser::getBasicDataTypeWidth(type);
		if(width <= 0)
			width = 8;
		variableStr += " = *(" + type + "*)SimulatorTestCase::takeTestCase(";
		variableStr += to_string(width) + ")";
	}
    else if (!ILParser::isBasicType(type) && !ILTranslator::translateForCBMC)
    {
        variableStr += " = {}";
    }
	variableStr += ";\n";
    return variableStr;
}

std::string ILTransMain::translateMainOutput(const ILOutput* output, std::string perfix) const
{
    string variableStr = ILParser::convertDataType(output->type);

	if(output->isAddress > 0)
		variableStr += " " + stringRepeat("*", output->isAddress);
	variableStr += " " + perfix + output->name;
	if(!output->arraySize.empty())
	{
		for(int i = 0;i < output->arraySize.size(); i++)
		{
			variableStr += "[" + to_string(output->arraySize[i]) + "]";
		}
	}
	if(output->defaultValue)
	{
		variableStr += " = " + output->defaultValue->expressionStr;
	}
	variableStr += ";\n";
    return variableStr;
}

int ILTransMain::translateMainInstanceGlobalVariable()
{
    const ILConfig* config = &(iLParser->config);
	if(!config->mainCompositeState.empty()) {
	    ILFunction* mainInitFunction = iLParser->findILFunction(config->mainInitFunction);
        if(!mainInitFunction)
            return -ILTranslator::ErrorCanNotFindMainInitFunction;
        if (mainInitFunction->inputs.empty() || mainInitFunction->inputs[0]->type != iLParser->config.mainCompositeState)
            return -ILTranslator::ErrorCanNotFindMainStateParameterOfInitFunction;

	    string name = ((ILFile*)(mainInitFunction->parent))->name + "_instance";
        vector<void*> domain;
        ILParser::ILVariable* var = ILParser::getVariableInDomain(name, domain);
	    if(!var)
	    {
            string commentStr;
	        if(ILTranslator::configGenerateComment)
	        {
	            commentStr = "/* Main State variable define */\n";
	        }
	        transCodeFunction += commentStr;
	        string mainType = mainInitFunction->inputs[0]->type;
	        if(ILParser::findTypeDefineByValue(mainType))
	            transCodeFunction += mainType + " " + name + ";\n";
	        else
	            transCodeFunction += "struct " + mainType + " " + name + ";\n";
	    }
	}
    return 1;
}


namespace{
    map<string, string> placeholderMap = {
        pair<string, string>("i32", "d"),
        pair<string, string>("f32", "f"),
        pair<string, string>("i64", "lld"),
        pair<string, string>("f64", "lf"),
        pair<string, string>("i8", "d"),
        pair<string, string>("i16", "d"),
        pair<string, string>("u32", "u"),
        pair<string, string>("u64", "llu"),
        pair<string, string>("u8", "d"),
        pair<string, string>("u16", "d"),
        pair<string, string>("short int", "hd"),
        pair<string, string>("sint32", "d"),
    };
}

std::string ILTransMain::translateMainExecParameterAssignment(const ILFunction* mainExecFunction, int step) const
{
    string ret = "";

    string stepPerfix = (step == 1) ? "" : (to_string(step) + "_");

    for (int i = 0; i < mainExecFunction->inputs.size(); i++) {
		if (mainExecFunction->inputs[i]->type == iLParser->config.mainCompositeState)
			continue;

        ILInput* input = mainExecFunction->inputs[i];

        string type = ILParser::getBasicDataTypeSimple(input->type);

        if(type == "i8" || type == "u8" || type == "i16" || type == "u16")
        {
            ret += "int exec_" + stepPerfix + "temp_" + input->name + ";\n";
        }
	}



    int varCount = 1;
    ret += "int result = fscanf(file, \"";

    string placeholderStr = "%f";
    string inportVariableStr = "&step";
	for (int i = 0; i < mainExecFunction->inputs.size(); i++) {
		if (mainExecFunction->inputs[i]->type == iLParser->config.mainCompositeState)
			continue;

        placeholderStr += ",";
        placeholderStr += "%";
        string type = ILParser::getBasicDataTypeSimple(mainExecFunction->inputs[i]->type);
        if(placeholderMap.find(type) != placeholderMap.end())
            placeholderStr += placeholderMap.at(type);
        else
            placeholderStr += "d";

		inportVariableStr += ", ";


        if(type == "i8" || type == "u8" || type == "i16" || type == "u16") {
            inportVariableStr += "&exec_" + stepPerfix + "temp_" + mainExecFunction->inputs[i]->name;
        } else {
            inportVariableStr += "&exec_" + stepPerfix + mainExecFunction->inputs[i]->name;
        }

        varCount++;
	}
    ret += placeholderStr + "\", " + inportVariableStr + ");\n";

    for (int i = 0; i < mainExecFunction->inputs.size(); i++) {
		if (mainExecFunction->inputs[i]->type == iLParser->config.mainCompositeState)
			continue;

        ILInput* input = mainExecFunction->inputs[i];

        string type = ILParser::getBasicDataTypeSimple(input->type);

        if(type == "i8" || type == "u8" || type == "i16" || type == "u16")
        {
            ret += "exec_" + stepPerfix + input->name + " = exec_" + stepPerfix + "temp_" + input->name + ";\n";
        }
        if(type == "f32" || type == "f64")
        {
            ret += "if(exec_" + stepPerfix + input->name + " == -0.0){\n";
            ret += "    exec_" + stepPerfix + input->name + " = 0.0;\n";
            ret += "}\n";
        }
	}


    ret += "if (result == EOF || result != " + to_string(varCount) + ") {\n";
    ret += "    break; // Stop if the end of file is reached\n";
    ret += "}\n";

    ret += "printf(\"Step: %f\\n\", step);\n";

    return ret;
}
