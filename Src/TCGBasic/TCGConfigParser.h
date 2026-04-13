#pragma once
#include<string>
#include<vector>
#include<map>

#include "TCGConfig.h"
#include "../Common/tinyxml2.h"
class TCGConfig;
class ILParser;
class TCGConfigParser {
public:
	enum {
		ErrorLoadTestCaseGenerationConfigFileFail = 180001,
        ErrorTCGConfigFileHasNoModelInfoNode,
        ErrorTCGConfigFileHasNoInputInfoNode,
        ErrorTCGInputHasNoAttributeName,
        ErrorTCGInputHasNoAttributeType,
        ErrorTCGInputParameterHasNoAttributeExpression,
        ErrorTCGInputParameterHasNoAttributeType,
	};
	TCGConfigParser(TCGConfig* config, ILParser* ilParser);
	~TCGConfigParser();
	int parseConfig(std::string xmlFilePath);
	TCGConfig* config = nullptr;
	ILParser* ilParser = nullptr;
    
private:
	int parseConfig(tinyxml2::XMLElement* root);
	static int parseModelInfo(TCGConfig* parent, tinyxml2::XMLElement* root);
    static int parseInputInfo(TCGConfig* parent, tinyxml2::XMLElement* root);
    static int parseInputParameterInfo(TCGConfig::TCGInput* parent, tinyxml2::XMLElement* root);

    static int parseUtilityIsAddress(std::string str);
    static std::vector<int> parseUtilityArraySize(std::string str);
};
