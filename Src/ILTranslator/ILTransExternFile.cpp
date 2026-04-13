#include "ILTransExternFile.h"

#include <fstream>
#include <algorithm>

#include "ILTransFile.h"
#include "ILTransHeadFile.h"

#include "ILTranslator.h"
#include "../Common/Utility.h"
#include "../ILBasic/ILExternFile.h"
#include "../ILBasic/ILParser.h"


using namespace std;

std::set<std::string> ILTransExternFile::externFileSet;

int ILTransExternFile::translate(const ILExternFile* externFile, std::string& code) const
{
	if(externFileSet.find(externFile->name) == externFileSet.end())
	{
        string exePath = getExeDirPath();
        string externFileName = exePath + "/CodeGeneration/C/ExternFile/" + externFile->name;
        if (isFileExist(externFileName))
		{
			code = readFile(externFileName);
            externFileSet.insert(externFile->name);
			return 1;
		}
	    
	    string iLFilePath = ILParser::getCurrentParesdILFilePath();
        string iLFileRealPath = getFileRealPath(iLFilePath);
        string iLFileName = getFileNameWithoutSuffixByPath(iLFilePath);
        string iLFileParentPath = getParentPath(iLFileRealPath);
        string fullExternFilePath = iLFileParentPath + "/" + iLFileName + "_ExternFile/" + externFile->name;
        
		if (isFileExist(fullExternFilePath))
		{
			code = readFile(fullExternFilePath);
            externFileSet.insert(externFile->name);
			return 1;
		}

        return 0;
	}
	return 1;
}
