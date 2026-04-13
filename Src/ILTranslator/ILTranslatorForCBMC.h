#pragma once

#include <string>
#include <vector>
#include <map>


class Expression;
class ILRef;
class ILParser;
class TCGConfig;

class ILTranslatorForCBMC
{
public:
	
	static ILParser* iLParser;
    static TCGConfig* tcgConfig;
    ILTranslatorForCBMC(ILParser* iLParser, TCGConfig* tcgConfig);

	int translate(std::string outputDir);

    static void setConfigGenerateMultiEntries(bool value);
    static bool configGenerateMultiEntries;

    static void setConfigGenerateComment(bool value);
    static bool configGenerateComment;
    
    static void setConfigUnreachable(bool value);
	static bool configUnreachable;
    
    static void setConfigTautology(bool value);
	static bool configTautology;
    static int tautologyTempVariableCount;
    
    static void setConfigBranchCover(bool value);
	static bool configBranchCover;

    static void setConfigRangeConstraint(bool value);
    static bool configRangeConstraint;

    static void setConfigGlobalVariableReset(bool value);
	static bool configGlobalVariableReset;
    
    static void setConfigCutOffArray(int value);
	static int configCutOffArray;



    static void setConfigMultiStep(int value);
    static int configMultiStep;

    static void setConfigGenSolvingPara(bool value);
    static bool configGenSolvingPara;
    

	std::map<std::string, std::string> fileCodes;
    
    static std::string getCutOffArrayDefaultValueExpStr(std::string varName, std::string type, const std::vector<int>& arraySize, Expression* exp);
    static void generateDefaultValueStructList(std::string name, std::string type, const std::vector<int>& arraySize);
    //static Expression* getCompoundInnerExp(Expression* exp, std::vector<int> id);
    //static void matchExpToDefaultValueStructList(Expression* exp);

private:

    std::string removeRelativeHeadFileInCode(std::string fileName);
};
