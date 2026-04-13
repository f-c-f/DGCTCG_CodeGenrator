#pragma once

#include <string>
#include <vector>
#include <map>


class ILRef;
class ILParser;
class SimulatorConfig;

class ILTranslatorForSimulator
{
public:

    static ILParser* iLParser;
    static SimulatorConfig* simuConfig;

    ILTranslatorForSimulator(ILParser* iLParser, SimulatorConfig* simuConfig);

    int translate(std::string outputDir);

    static void setConfigGenerateComment(bool value);
    static bool configGenerateComment;

    static void setConfigTest(bool value);
    static bool configTest;

    static void setConfigPackageParameter(bool value);
    static bool configPackageParameter;

    static void setConfigPackageVariable(bool value);
    static bool configPackageVariable;

    //Ãû×Ö£¬ID£¬ÀàÐÍ
    static std::map < std::string, std::pair<int, std::string>>needMonitor;
    static int outID;

    //path, type
    //static std::map<std::string, std::string> needDiagnose;

    std::map<std::string, std::string> fileCodes;

    static std::string getRefComment(const std::vector<ILRef*>* refList);

    static std::string diagnoseFile;
    static std::string diagnoseHFile;

    void release();

private:

    std::string getConfigTestRedefineOutputCode() const;

    int copyResourceFiles(std::string outputDir);
};
