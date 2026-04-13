#pragma once

#include <string>
#include <vector>
#include <map>


class ILRef;
class ILParser;


class ILTranslatorForC
{
public:
	
	static ILParser* iLParser;
	int translate(ILParser* iLParser, std::string outputDir);

    static void setConfigGenerateComment(bool value);
    static bool configGenerateComment;
    
    static void setConfigTestTime(bool value);
    static bool configTestTime;

    static void setConfigTestCorrectness(bool value);
    static bool configTestCorrectness;

	static void setConfigPackageParameter(bool value);
	static bool configPackageParameter;

	static void setConfigPackageVariable(bool value);
	static bool configPackageVariable;
    
	static void setConfigTargetDevice(std::string device);
	static std::string configTargetDevice;

	std::map<std::string, std::string> fileCodes;
    
    static std::string getRefComment(const std::vector<ILRef*>* refList);

    

private:


    std::string getConfigTestRedefineOutputCode() const;
};
