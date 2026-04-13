
#include "TCGConfigParser.h"
#include "../Common/Utility.h"
#include "TCGConfig.h"
#include <assert.h>
#include "../ILBasic/ILParser.h"
#include "../ILBasic/ILFile.h"
#include "../ILBasic/ILFunction.h"
#include "../ILBasic/ILRef.h"
#include "../ILBasic/ILInput.h"
#include "../ILBasic/ILOutput.h"

using namespace std;
namespace
{
    const int CONST_NUM_2 = 2;
    const int CONST_NUM_7 = 7;
}


TCGConfigParser::TCGConfigParser(TCGConfig* config, ILParser* ilParser)
{
    this->config = config;
    this->ilParser = ilParser;
}

TCGConfigParser::~TCGConfigParser()
{
}

int TCGConfigParser::parseConfig(const std::string xmlFilePath)
{
    if(xmlFilePath.empty())
    {
        config->iterationCount = ilParser->config.iterationCount;
        config->mainInitFunction = ilParser->config.mainInitFunction;
        config->mainExecFunction = ilParser->config.mainExecFunction;
        config->mainCompositeState = ilParser->config.mainCompositeState;
        return 1;
    }
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError ret = doc.LoadFile(xmlFilePath.c_str());
	if (ret != 0) {
		fprintf(stderr, "Fail to load Test Config file: %s\n", xmlFilePath.c_str());
		return -ErrorLoadTestCaseGenerationConfigFileFail;
	}
	tinyxml2::XMLElement* root = doc.RootElement();
	if (root == nullptr) {
		return -ErrorLoadTestCaseGenerationConfigFileFail;
	}

	int res = parseConfig(root);
	if (res < 0)
		return res;
	return 1;
}

int TCGConfigParser::parseConfig(tinyxml2::XMLElement* root)
{

	int res;

    tinyxml2::XMLElement* c = root->FirstChildElement("ModelInfo");
	if(c) {
        res = parseModelInfo(config, c);
	    if (res < 0)
		    return res;
    } else {
        return -ErrorTCGConfigFileHasNoModelInfoNode;
    }
    
    c = root->FirstChildElement("InputInfo");
	if(c) {
        res = parseInputInfo(config, c);
	    if (res < 0)
		    return res;
    }
    

	return 1;
}



int TCGConfigParser::parseModelInfo(TCGConfig* parent, tinyxml2::XMLElement* root)
{
	parent->iterationCount = 1;
	parent->mainInitFunction = "";
	parent->mainExecFunction = "";
	parent->mainCompositeState = "";
	const tinyxml2::XMLElement *c = nullptr;

	c = root->FirstChildElement("IterationCount");
	if(c && c->FindAttribute("Value"))
	{
		parent->iterationCount = c->IntAttribute("Value");
	}
	c = root->FirstChildElement("MainInitFunction");
	if(c && c->FindAttribute("Name"))
	{
		parent->mainInitFunction = string(c->Attribute("Name"));
	}
	c = root->FirstChildElement("MainExecFunction");
	if(c && c->FindAttribute("Name"))
	{
		parent->mainExecFunction = string(c->Attribute("Name"));
	}
	c = root->FirstChildElement("MainCompositeState");
	if(c && c->FindAttribute("Name"))
	{
		parent->mainCompositeState = string(c->Attribute("Name"));
	}
	return 1;
}



int TCGConfigParser::parseInputInfo(TCGConfig* parent, tinyxml2::XMLElement* root)
{
    int res;
	tinyxml2::XMLElement *c = root->FirstChildElement("Input");
	while (c)
	{
		TCGConfig::TCGInput input;
		if(c->FindAttribute("Name"))
			input.name = string(c->Attribute("Name"));
		else
			return -ErrorTCGInputHasNoAttributeName;
		
		input.isAddress = c->FindAttribute("IsAddress") ?
            parseUtilityIsAddress(string(c->Attribute("IsAddress"))) : input.isAddress;
		
		if(c->FindAttribute("Type"))
			input.type = string(c->Attribute("Type"));
		else
			return -ErrorTCGInputHasNoAttributeType;

        input.type = stringStartsWith(input.type, "struct ") ?
            input.type.substr(CONST_NUM_7, input.type.length() - CONST_NUM_7) : input.type;
        
        input.arraySize = c->FindAttribute("ArraySize") ?
            parseUtilityArraySize(string(c->Attribute("ArraySize"))) : input.arraySize;

        res = parseInputParameterInfo(&input, c);

        parent->inputs.push_back(input);
        
		c = c->NextSiblingElement("Input");
	}
	return 1;
}

int TCGConfigParser::parseInputParameterInfo(TCGConfig::TCGInput* parent, tinyxml2::XMLElement* root)
{
    int res;
	tinyxml2::XMLElement *c = root->FirstChildElement("Parameter");
	while (c)
	{
		TCGConfig::TCGInput::TCGParameter parameter;
		if(c->FindAttribute("Expression"))
			parameter.expression = string(c->Attribute("Expression"));
		else
			return -ErrorTCGInputParameterHasNoAttributeExpression;
		
		if(c->FindAttribute("Type"))
			parameter.type = string(c->Attribute("Type"));
		else
			return -ErrorTCGInputParameterHasNoAttributeType;

        if(c->FindAttribute("Min"))
			parameter.min = string(c->Attribute("Min"));
        
        if(c->FindAttribute("Max"))
			parameter.max = string(c->Attribute("Max"));
        
        if(c->FindAttribute("Value"))
			parameter.value = string(c->Attribute("Value"));

        parent->parameters.push_back(parameter);
        
		c = c->NextSiblingElement("Parameter");
	}
	return 1;
}

int TCGConfigParser::parseUtilityIsAddress(std::string str)
{
    string isAddressStr = stringToLower(str);
	if(isAddressStr == "true" || isAddressStr == "false")
	{
		return (isAddressStr == "true") ? 1 : 0;
	}
    else
    {
        return stringToInt(isAddressStr);
    }
}

vector<int> TCGConfigParser::parseUtilityArraySize(std::string str)
{
    vector<int> ret;
	vector<string> arraySizeStrSplit = stringSplit(str, ",");
	for(int i = 0;i < arraySizeStrSplit.size();i++)
	{
		ret.push_back(stringToInt(arraySizeStrSplit[i]));
	}
    return ret;
}
