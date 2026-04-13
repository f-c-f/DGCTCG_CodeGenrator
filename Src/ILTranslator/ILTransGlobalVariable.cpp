#include "ILTransGlobalVariable.h"

#include "ILTransFile.h"
#include "ILTranslator.h"
#include "../Common/Utility.h"
#include "../ILBasic/Expression.h"
#include "../ILBasic/ILFile.h"

#include "../ILBasic/ILGlobalVariable.h"
#include "../ILBasic/ILParser.h"

using namespace std;



int ILTransGlobalVariable::translate(const ILGlobalVariable* globalVariable, ILTransFile* file) const
{
    string commentStr;
    if(ILTranslator::configGenerateComment)
    {
        commentStr = "/* ";
        commentStr += "Global Variable define of ";
        commentStr += ILTranslator::getRefComment(&(globalVariable->refs));
        commentStr += " */\n";
    }

	string globalVariableStr;

    if(globalVariable->align > 0)
    {
        globalVariableStr += "__declspec(aligned(" + to_string(globalVariable->align) + ")) ";
    }
    
    // 全局变量被设置为static后，会导致TCGStateSearch的代码无法访问
    if(globalVariable->isStatic && 
        !ILTranslator::translateForTCGStateSearch && 
        !ILTranslator::translateForTCGHybrid)
        globalVariableStr += "static ";
    if(globalVariable->isConst)
        globalVariableStr += "const ";

    globalVariableStr += ILParser::convertDataType(globalVariable->type);

	
	if(globalVariable->isAddress > 0)
		globalVariableStr += " " + stringRepeat("*", globalVariable->isAddress);
	globalVariableStr += " " + globalVariable->name;
	if(!globalVariable->arraySize.empty())
	{
		for(int i = 0;i < globalVariable->arraySize.size(); i++)
		{
            int arrayLen = globalVariable->arraySize[i];
            if(ILTranslator::translateForCBMC && arrayLen > ILTranslatorForCBMC::configCutOffArray)
                arrayLen = ILTranslatorForCBMC::configCutOffArray;
			globalVariableStr += "[" + to_string(arrayLen) + "]";
		}
	}

    if(ILTranslator::translateForCBMC) {
        ILTranslator::transCodeGlobalVariable += commentStr + globalVariableStr;

        if((ILTranslatorForCBMC::configTautology || ILTranslatorForCBMC::configUnreachable) &&
            ILParser::isBasicType(globalVariable->type) && !globalVariable->isAddress && globalVariable->arraySize.empty()) {
            string simplifyType = ILParser::getBasicDataTypeSimple(globalVariable->type);
            ILTranslator::transCodeGlobalVariable += " = " + ILTranslator::cbmcTypeMap.at(simplifyType);

        } else if(globalVariable->defaultValue) {
            if(ILTranslator::translateForCBMC && globalVariable->defaultValue->type == Token::Compound) {
                ILTranslator::transCodeGlobalVariable += " = " + ILTranslatorForCBMC::getCutOffArrayDefaultValueExpStr(globalVariable->name, globalVariable->type, globalVariable->arraySize, globalVariable->defaultValue);
            }
	        else {
		        ILTranslator::transCodeGlobalVariable += " = " + globalVariable->defaultValue->expressionStr;
            }
	    }
	    ILTranslator::transCodeGlobalVariable += ";\n";
    }
    else
    {
        // 全局变量被设置为static后，会导致TCGStateSearch的代码无法访问
        if((!globalVariable->isStatic || 
            ILTranslator::translateForTCGStateSearch || 
            ILTranslator::translateForTCGHybrid
            ) && !file->isIndependent)
	        ILTranslator::transCodeStructDefine += commentStr + "extern " + globalVariableStr + ";\n";
    
	    file->transCodeGlobalVariable += commentStr + globalVariableStr;
	    if(globalVariable->defaultValue)
	    {
		    file->transCodeGlobalVariable += " = ";
            if(globalVariable->arraySize.empty())
                file->transCodeGlobalVariable += globalVariable->defaultValue->expressionStr;
            else
                file->transCodeGlobalVariable += "{" + globalVariable->defaultValue->expressionStr + "}";
	    }
	    
	    file->transCodeGlobalVariable += ";\n";
    }

	return 1;
}
