#include "ILTransExternVariable.h"

#include "ILTransFile.h"
#include "ILTranslator.h"
#include "../Common/Utility.h"
#include "../ILBasic/Expression.h"
#include "../ILBasic/ILFile.h"

#include "../ILBasic/ILExternVariable.h"
#include "../ILBasic/ILParser.h"

using namespace std;

int ILTransExternVariable::translate(const ILExternVariable* externVariable, ILTransFile* file) const
{
    string commentStr;
    if(ILTranslator::configGenerateComment)
    {
        commentStr = "/* ";
        commentStr += "Extern Variable define of ";
        commentStr += ILTranslator::getRefComment(&(externVariable->refs));
        commentStr += " */\n";
    }

	string externVariableStr;

    if(externVariable->align > 0)
    {
        externVariableStr += "__declspec(aligned(" + to_string(externVariable->align) + ")) ";
    }
    
    //if(globalVariable->isStatic)
    //    globalVariableStr += "static ";
    if(externVariable->isConst)
        externVariableStr += "const ";

    externVariableStr += ILParser::convertDataType(externVariable->type);

	
	if(externVariable->isAddress > 0)
		externVariableStr += " " + stringRepeat("*", externVariable->isAddress);
	externVariableStr += " " + externVariable->name;
	if(!externVariable->arraySize.empty())
	{
		for(int i = 0;i < externVariable->arraySize.size(); i++)
		{
            int arrayLen = externVariable->arraySize[i];
            if(ILTranslator::translateForCBMC && arrayLen > ILTranslatorForCBMC::configCutOffArray)
                arrayLen = ILTranslatorForCBMC::configCutOffArray;
			externVariableStr += "[" + to_string(arrayLen) + "]";
		}
	}

    if(ILTranslator::translateForCBMC)
    {
        ILTranslator::transCodeStructDefine += commentStr + "extern " + externVariableStr + ";\n";
    }
    else
    {
        if(!file->isIndependent) //!globalVariable->isStatic && 
            ILTranslator::transCodeGlobalVariable += commentStr + "extern " + externVariableStr + ";\n";
        else
            file->transCodeGlobalVariable += commentStr + "extern " + externVariableStr + ";\n";
    }
	return 1;
}
