#pragma once

#include <string>
#include <vector>
#include <map>


class ILSchedule;
class ILRef;
class ILParser;
class TCGConfig;
class ILAnalyzeSchedule;

class ILTranslatorForCoverage
{
public:
	
	static ILParser* iLParser;
    //static std::string testCaseFileFolder;
    //static TCGConfig* tcgConfig;

    ILTranslatorForCoverage(ILParser* iLParser);

	int translate(std::string outputDir);

    static void setConfigGenerateComment(bool value);
    static bool configGenerateComment;
    
    static void setConfigCutOffArray(int value);
	static int configCutOffArray;

	std::map<std::string, std::string> fileCodes;
    
	static std::string transCodeBranchRegister;
    static std::string transCodeBranchRelation;
	static std::string transCodeGlobalVariableRegister;
    static std::string transCodeInputRegister;

    void release();

private:

    std::string getConfigTestRedefineOutputCode() const;

    int copyResourceFiles(std::string outputDir);
};
