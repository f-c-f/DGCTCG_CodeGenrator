#pragma once

#include <string>
#include <vector>
#include <map>

class ILInput;
class ILFunction;
class ILParser;
class ILConfig;
class ILConfigParameter;

class ILTransForLibFuzzerTestCaseConverter
{
public:
    ILTransForLibFuzzerTestCaseConverter(const ILParser* parser);

	int translate(std::string outputDir);


private:
    
    std::string transCodeHeadFile;
    std::string transCodeFunction;

    const ILParser* parser = nullptr;

    int translateMainFile(std::string outputDir);

    int copyResourceFiles(std::string outputDir) const;

    int generateOneKeyBuildScript(std::string outputDir) const;

    int translateMainFileHeadFile();
    int translateForLibFuzzerMainFunction(std::string& code);

    std::string translateForLibFuzzerMainExecInputCopy(const ILFunction* mainExecFunction) const;
    std::string translateForLibFuzzerMainExecInputCopyTraverse(std::string name, std::string type, std::vector<int> arraySize, bool isAddress, bool outOfArrayCut) const;
    std::string translateMainExecInput(const ILFunction* mainExecFunction) const;
    std::string translateMainInput(const ILInput* input, std::string perfix) const;

    std::string translateForLibFuzzerMainParameterCopy(const ILConfig* config) const;
    std::string translateForLibFuzzerMainParameterCopyTraverse(std::string name, std::string type, std::vector<int> arraySize, bool isAddress, bool outOfArrayCut) const;
    std::string translateMainParameter(const ILConfig* config) const;
    std::string translateMainParameter(const ILConfigParameter* para, std::string perfix) const;
};
