#include "ILTransHeadFile.h"

#include <fstream>
#include <algorithm>

#include "ILTransFile.h"

#include "ILTranslator.h"
#include "../Common/Utility.h"

#include "../ILBasic/ILHeadFile.h"


using namespace std;

set<string> ILTransHeadFile::headFileSet;
map<string, string> ILTransHeadFile::headFileContent;

//加入全局定义
int ILTransHeadFile::translate(const ILHeadFile* headFile, ILTransFile* file) const
{
    if(ILTranslator::translateForCBMC)
        return translateForCBMC(headFile, file);
    else
        return translateBasic(headFile, file);
	return 0;
}


std::vector<std::string> ILTransHeadFile::getHeadFileList(std::string code) const
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

//加入全局定义
int ILTransHeadFile::translateBasic(const ILHeadFile* headFile, ILTransFile* file) const
{
    string commentStr;
    if(ILTranslator::configGenerateComment)
    {
        commentStr = "/* ";
        commentStr += "Head File define of ";
        commentStr += ILTranslator::getRefComment(&(headFile->refs));
        commentStr += " */\n";
    }

	string name = headFile->name;
	string tempCode = "";
	if(headFileContent.find(name) == headFileContent.end())
	{
		string exePath = getExeDirPath();
        string fileName = exePath + "/CodeGeneration/C/HeadFile/" + name + ".txt";
        string code;
        if(!isFileExist(fileName))
        {
            if(name == "math" || name == "float" || name == "stdio" || name == "stdlib" || name == "string")
                code = "#include <" + name + ".h>";
            else
                code = "#include \"" + name + "\"";
        }
        else
        {
            code = readFile(fileName);
        }
        
		headFileContent[name] = code;
	}
    
    vector<string> headFiles = getHeadFileList(headFileContent[name]);
    for(int i = 0; i < headFiles.size(); i++)
    {
        if(!file->isIndependent)
        {
            if(find(ILTransHeadFile::headFileSet.begin(),ILTransHeadFile::headFileSet.end(), headFiles[i]) == ILTransHeadFile::headFileSet.end())
            {
                ILTransHeadFile::headFileSet.insert(headFiles[i]);
                ILTranslator::transCodeHeadFile += commentStr + headFiles[i] + "\n";
            }
        }
        else
        {
            file->transCodeHeadFile += commentStr + headFiles[i] + "\n";
        }
    }

	//else
	//{
	//	tempCode = headFileContent[name];
	//}
	
	return 1;
}

