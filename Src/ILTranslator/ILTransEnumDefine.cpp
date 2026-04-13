#include "ILTransEnumDefine.h"

#include "ILTransFile.h"

#include "ILTranslator.h"
#include "../ILBasic/ILEnumDefine.h"
#include "../ILBasic/ILEnumValue.h"
#include "../ILBasic/ILFile.h"

#include "../Common/Utility.h"

using namespace std;

//加入全局定义
int ILTransEnumDefine::translate(const ILEnumDefine* enumDefine, ILTransFile* file) const
{
    string code;
    if(ILTranslator::configGenerateComment)
    {
        string commentStr = "/* ";
        commentStr += "Enum define of ";
        commentStr += ILTranslator::getRefComment(&(enumDefine->refs));
        commentStr += " */\n";
        code += commentStr;
    }

	string name = enumDefine->name;
	code += "typedef enum {\n";;
	int res = 0;
	
	for(int i = 0;i<enumDefine->enumValues.size();i++)
	{
        ILEnumValue* enumValue = enumDefine->enumValues[i];
        code += enumValue->name;
        if(!enumValue->value.empty())
        {
            code += " = " + enumValue->value;
        }
        code += ",\n";
	}
	code += "} " + name + ";\n";

    if(ILTranslator::translateForCBMC) {
        ILTranslator::transCodeEnumDefine += code;
    }
    else
    {
        if(file->isIndependent) {
            file->transCodeEnumDefine += code;
        } else {
            ILTranslator::transCodeEnumDefine += code;
        }
    }
    

	return 1;
}

