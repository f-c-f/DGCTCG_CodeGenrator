#include "ILTransMacro.h"
#include "ILTransFile.h"

#include "ILTranslator.h"
#include "../Common/Utility.h"

#include "../ILBasic/ILMacro.h"

using namespace std;

//加入全局定义
int ILTransMacro::translate(const ILMacro* macro, ILTransFile* file) const
{
    string code;
    string commentStr;

    if (!(ILTranslator::translateForCBMC || ILTranslator::translateForC))
    {
        if (stringToLower(macro->name) == "bool")
            return 0;
    }

    if(ILTranslator::configGenerateComment)
    {
        string refCommentStr = ILTranslator::getRefComment(&(macro->refs));
        if(!stringTrim(refCommentStr).empty())
        {
            commentStr = "/* ";
            commentStr += "Macro define of ";
            commentStr += refCommentStr;
            commentStr += " */\n";
        }
    }
    

	string name = macro->name;
	string value = macro->value;

    code = commentStr + "#define " + name;
    if(!value.empty())
        code += " " + value;
    code += "\n";

    if(ILTranslator::translateForCBMC)
    {
        ILTranslator::transCodeMacro += code;
    }
    else
    {
        if(!file->isIndependent)
            ILTranslator::transCodeMacro += code;
        else
            file->transCodeMacro +=code;
    }

	return 1;
}
