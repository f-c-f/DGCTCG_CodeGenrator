#include "ILTransMember.h"

#include "ILTransFile.h"

#include "ILTranslator.h"
#include "../Common/Utility.h"

#include "../ILBasic/ILMember.h"
#include "../ILBasic/ILParser.h"

using namespace std;

//加入全局定义
int ILTransMember::translate(const ILMember* member, ILTransFile* file) const
{
    string code;
    
    if(member->align > 0) {
        code += "__declspec(aligned(" + to_string(member->align) + ")) ";
    }
    if(member->isConst) {
        code += "const ";
    }

    code += ILParser::convertDataType(member->type);
    
	if(member->isAddress > 0)
		code += " " + stringRepeat("*", member->isAddress);

	code += " " + member->name;
	if(!member->arraySize.empty())
	{
		for(int i = 0;i < member->arraySize.size(); i++)
		{
            int arrayLen = member->arraySize[i];
            if(ILTranslator::translateForCBMC && arrayLen > ILTranslatorForCBMC::configCutOffArray)
                arrayLen = ILTranslatorForCBMC::configCutOffArray;
			code += "[" + to_string(arrayLen) + "]";
		}
	}
	code += ";\n";

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
