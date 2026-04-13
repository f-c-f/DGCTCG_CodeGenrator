#include "ILTransStructDefine.h"

#include "ILTransFile.h"

#include "ILTranslator.h"
#include "ILTransMember.h"
#include "../ILBasic/ILStructDefine.h"
#include "../Common/Utility.h"

using namespace std;

//加入全局定义
int ILTransStructDefine::translate(ILStructDefine* structDefine, ILTransFile* file) const
{
    string *code = nullptr;
    if(ILTranslator::translateForCBMC)
    {
        code = &ILTranslator::transCodeStructDefine;
    }
    else
    {
        if(!file->isIndependent)
            code = &ILTranslator::transCodeStructDefine;
        else
            code = &file->transCodeStructDefine;
    }
    
    string commentStr;
    if(ILTranslator::configGenerateComment)
    {
        string refCommentStr = ILTranslator::getRefComment(&(structDefine->refs));
        if(!stringTrim(refCommentStr).empty())
        {
            commentStr = "/* ";
            commentStr += "Struct define of ";
            commentStr += refCommentStr;
            commentStr += " */\n";
        }
    }

	string name = structDefine->name;
	*code += commentStr + "struct " + name + " {\n";
	//ILTranslator::transCodeStructDefine += "{\n";
    //comment = true;
	int res = 0;
	ILTransMember iLTransMember;
	for(int i = 0;i<structDefine->members.size();i++)
	{
		res = iLTransMember.translate(structDefine->members[i], file);
		if(res < 0)
			return res;
	}
    *code += "};\n";

	return 1;
}

int ILTransStructDefine::translateDeclaration(ILStructDefine* structDefine, ILTransFile* file) const
{
    string code;
	string name = structDefine->name;
	code += "struct " + name + ";\n";

    if(ILTranslator::translateForCBMC)
    {
        ILTranslator::transCodeStructDefine += code;
    }
    else
    {
        if(!file->isIndependent)
	        ILTranslator::transCodeStructDefine += code;
        else
            file->transCodeStructDefine += code;
    }
    
	return 1;
}
