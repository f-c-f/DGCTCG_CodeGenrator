#include "ILTransTypeDefine.h"

#include "ILTransFile.h"

#include "ILTranslator.h"
#include "../ILBasic/ILParser.h"

#include "../ILBasic/ILTypeDefine.h"
#include "../Common/Utility.h"

using namespace std;

//加入全局定义
int ILTransTypeDefine::translate(ILTypeDefine* typeDefine, ILTransFile* file) const
{
    string code;
    string commentStr;
    if(ILTranslator::configGenerateComment)
    {
        string refCommentStr = ILTranslator::getRefComment(&(typeDefine->refs));
        if(!stringTrim(refCommentStr).empty())
        {
            commentStr = "/* ";
            commentStr += "Type define of ";
            commentStr += refCommentStr;
            commentStr += " */\n";
        }
    }

	string name = typeDefine->name;
	string value = typeDefine->value;

    if(ILParser::findStructType(name))
        name = "struct " + name;
    else if(ILParser::findEnumType(name))
        name = "enum " + name;
    else
        name = ILParser::convertDataType(name);
	code += commentStr + "typedef " + name + " " + value + ";\n";

    if(ILTranslator::translateForCBMC)
    {
        ILTranslator::transCodeTypeDefine += code;
    }
    else
    {
        if(!file->isIndependent)
	        ILTranslator::transCodeTypeDefine += code;
        else
            file->transCodeTypeDefine += code;
    }



	return 1;
}
