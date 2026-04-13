#include "ILTransHeadFile.h"

#include <fstream>
#include <algorithm>

#include "ILTransFile.h"

#include "ILTranslator.h"
#include "../Common/Utility.h"

#include "../ILBasic/ILHeadFile.h"

using namespace std;

//加入全局定义
int ILTransHeadFile::translateForCBMC(const ILHeadFile* headFile, ILTransFile* file) const
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
            // Use system headers for known C standard library names (no .h in model name)
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
        if(find(ILTransHeadFile::headFileSet.begin(),ILTransHeadFile::headFileSet.end(), headFiles[i]) == ILTransHeadFile::headFileSet.end())
        {
            ILTransHeadFile::headFileSet.insert(headFiles[i]);
            ILTranslator::transCodeHeadFile += commentStr + headFiles[i] + "\n";
        }
    }

	//else
	//{
	//	tempCode = headFileContent[name];
	//}
	
	return 1;
}

