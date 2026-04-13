#include "ILTransFile.h"

#include "ILTransEnumDefine.h"
#include "ILTransHeadFile.h"
#include "ILTransMacro.h"
#include "ILTransTypeDefine.h"
#include "ILTransUtilityFunction.h"
#include "ILTransStructDefine.h"
#include "ILTransGlobalVariable.h"
#include "ILTransFunction.h"
#include "ILTransExternVariable.h"
#include "ILTranslator.h"

#include "../ILBasic/ILFile.h"
#include "../ILBasic/ILStructDefine.h"

using namespace std;

int ILTransFile::translate(ILFile* file, string& code)
{
    this->file = file;
    if(!ILTranslator::translateForCBMC)
    {
        this->isIndependent = file->isIndependent;
		
		/*
		* 或许之后会用到
		// 添加结构体in，out，local分别用来储存inport，outport和localvariable
		if (ILTranslatorForC::configPackageVariable)
		{
			ILStructDefine* in = new ILStructDefine(file);
			file->structDefines.push_back(in);
			ILStructDefine* out = new ILStructDefine(file);
			file->structDefines.push_back(out);
			ILStructDefine* local = new ILStructDefine(file);
			file->structDefines.push_back(local);
			in->name = file->name + "_in";
			out->name = file->name + "_out";
			local->name = file->name + "_local";
		}
		*/
	
	}
	int res = 0;
	

	ILTransFunction iLTransFunction;
	for(int i = 0; res >= 0 && i<file->functions.size();i++)
	{
		res = iLTransFunction.translate(file->functions[i], this);
	}
    ILTransHeadFile iLTransHeadFile;
	for(int i = 0; res >= 0 && i<file->headFiles.size();i++)
	{
		res = iLTransHeadFile.translate(file->headFiles[i], this);
	}
	ILTransStructDefine iLTransStructDefine;
	for(int i = 0; res >= 0 && i<file->structDefines.size();i++)
	{
		res = iLTransStructDefine.translateDeclaration(file->structDefines[i], this);
	}
	for(int i = 0; res >= 0 && i<file->structDefines.size();i++)
	{
		res = iLTransStructDefine.translate(file->structDefines[i], this);
	}
    ILTransEnumDefine iLTransEnumDefine;
	for(int i = 0; res >= 0 && i< file->enumDefines.size();i++)
	{
		res = iLTransEnumDefine.translate(file->enumDefines[i], this);
	}
	ILTransMacro iLTransMacro;
	for(int i = 0; res >= 0 && i<file->macros.size();i++)
	{
		res = iLTransMacro.translate(file->macros[i], this);
	}
	ILTransTypeDefine iLTransTypeDefine;
	for(int i = 0; res >= 0 && i<file->typeDefines.size();i++)
	{
		res = iLTransTypeDefine.translate(file->typeDefines[i], this);
	}
	ILTransGlobalVariable iLTransGlobalVariable;
	for(int i = 0; res >= 0 && i<file->globalVariables.size();i++)
	{
		res = iLTransGlobalVariable.translate(file->globalVariables[i], this);
	}
	ILTransUtilityFunction iLTransUtilityFunction;
	for(int i = 0; res >= 0 && i<file->utilityFunctions.size();i++)
	{
		res = iLTransUtilityFunction.translate(file->utilityFunctions[i], this);
	}
    ILTransExternVariable iLTransExternVariable;
	for(int i = 0; res >= 0 && i<file->externVariables.size();i++)
	{
		res = iLTransExternVariable.translate(file->externVariables[i], this);
	}
	if(res < 0)
		return res;

	string retCode;
    
    if(ILTranslator::configGenerateComment)
    {
        retCode += "/* Code of " + file->name + " */\n";
    }

	retCode += transCodeHeadFile + transCodeMacro;
	retCode += transCodeEnumDefine + transCodeStructDefine;
	retCode += transCodeTypeDefine + transCodeGlobalVariable;
	retCode += transCodeUtilityFunction + transCodeFunction;
    

	code = retCode;

	return 1;
}
