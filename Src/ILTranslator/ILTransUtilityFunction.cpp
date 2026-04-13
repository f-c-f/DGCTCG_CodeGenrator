#include "ILTransUtilityFunction.h"

#include <fstream>
#include <algorithm>

#include "ILTransFile.h"
#include "ILTransHeadFile.h"

#include "ILTranslator.h"
#include "ILTranslatorForTCGStateSearch.h"
#include "../Common/Utility.h"

#include "../ILBasic/ILUtilityFunction.h"

using namespace std;

set<string> ILTransUtilityFunction::utilityFunctionSet;
map<string, string> ILTransUtilityFunction::utilityFunctionContent;

//加入全局定义
int ILTransUtilityFunction::translate(const ILUtilityFunction* utilityFunction, ILTransFile* file) const
{
    string commentStr;
    if(ILTranslator::configGenerateComment)
    {
        commentStr = "/* Utility Function define of ";
        commentStr += ILTranslator::getRefComment(&(utilityFunction->refs));
        commentStr += " */\n";
    }

	string name = utilityFunction->name;
	if(utilityFunctionContent.find(name) == utilityFunctionContent.end())
	{
		string exePath = getExeDirPath();
        string fileName = exePath + "/CodeGeneration/C/UtilityFunction/" + name + ".txt";

		if (!isFileExist(fileName))
		{
			string msg = "Can not read: " + exePath + "/CodeGeneration/C/UtilityFunction/" + name + ".txt";
            int res = 0;
            res = -ILTranslator::ErrorUtilityFunctionTemplateCanNotFind;
            ILTranslator::setCurrentError(res, msg);
			return res;
		}

        string code = readFile(fileName);

        vector<string> headFiles = getHeadFileList(code);
        for(int i = 0; i < headFiles.size(); i++)
        {
            if(find(ILTransHeadFile::headFileSet.begin(),ILTransHeadFile::headFileSet.end(), headFiles[i]) == ILTransHeadFile::headFileSet.end())
			{
                ILTransHeadFile::headFileSet.insert(headFiles[i]);
			    ILTranslator::transCodeHeadFile += commentStr + headFiles[i] + "\n";
			}
        }

        string functionDefine = getFunctionDefine(code);
        ILTranslator::transCodeUtilityFunction += commentStr + functionDefine + "\n";

        string functionImplement = getFunctionImplement(code);
        ILTranslator::transCodeUtilityFunctionImplement += commentStr + functionImplement + "\n";

        utilityFunctionContent[name] = functionImplement;
        
	}
	return 1;
}

std::vector<std::string> ILTransUtilityFunction::getHeadFileList(std::string code) const
{
    vector<string> ret;
    vector<string> codeLines = stringSplit(code, "\n");
    
    for(int i = 0; i < codeLines.size(); i++)
    {
        string lineStr = codeLines[i];
        lineStr = stringTrim(lineStr);
        if(lineStr.empty())
            continue;
        if (stringStartsWith(lineStr, "#include"))
        {
            ret.push_back(lineStr);
        }
    }
    return ret;
}

std::string ILTransUtilityFunction::getFunctionImplement(std::string code) const
{
    string ret;
    vector<string> codeLines = stringSplit(code, "\n");
    
    for(int i = 0; i < codeLines.size(); i++)
    {
        string lineStr = codeLines[i];
        lineStr = stringTrim(lineStr);
        if(lineStr.empty())
            continue;
        if (stringStartsWith(lineStr, "#include"))
            continue;
        ret += lineStr + ((i == codeLines.size()-1) ? "" : "\n");
    }

    return ret;
}

std::string ILTransUtilityFunction::getFunctionDefine(std::string code) const
{
    string retCode;
    code = getFunctionImplement(code);
    code = codeRemoveComment(code);
    int codeLength = code.length();
    int i = 0;
    bool isInString = false;
    int curIndent = 0;
    while(i < codeLength)
    {
        if (code[i] == '\"' && (!(i > 0 && code[i-1] == '\\')) && isInString == false) {
	        isInString = true;
        } else if (code[i] == '\"' && (!(i > 0 && code[i-1] == '\\')) && isInString == true) {
	        isInString = false;
        }
        
        if(code[i] == '}' && !isInString)
            curIndent--;
        else if(code[i] == '{' && !isInString)
        {
            if(curIndent == 0)
                retCode += ";";
            curIndent++;
        }

        if(curIndent == 0 && code[i] != '}')
            retCode += code[i];

        i++;
    }
    return retCode;
}
