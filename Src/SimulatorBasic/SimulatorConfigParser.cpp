
#include "SimulatorConfigParser.h"
#include "../Common/Utility.h"
#include "SimulatorConfig.h"
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


SimulatorConfigParser::SimulatorConfigParser(SimulatorConfig* config, ILParser* ilParser)
{
    this->config = config;
    this->ilParser = ilParser;
}

SimulatorConfigParser::~SimulatorConfigParser()
{
}

int SimulatorConfigParser::parseConfig(const std::string xmlFilePath)
{
    if(xmlFilePath.empty())
        return 1;

	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError ret = doc.LoadFile(xmlFilePath.c_str());
	if (ret != 0) {
		fprintf(stderr, "Fail to load Simualtion Config file: %s\n", xmlFilePath.c_str());
		return -ErrorLoadSimulatorConfigFileFail;
	}
	tinyxml2::XMLElement* root = doc.RootElement();
	if (root == nullptr) {
		return -ErrorLoadSimulatorConfigFileFail;
	}

	int res = parseConfig(root);
	if (res < 0)
		return res;
	return 1;
}

int SimulatorConfigParser::parseConfig(tinyxml2::XMLElement* root)
{

	int res;

    tinyxml2::XMLElement* c = root->FirstChildElement("RecordActor");
	if(c) {
        res = parseRecordActor(config, c);
	    if (res < 0)
		    return res;
    } else {
        return -ErrorSimulatorConfigFileHasNoRecordActor;
    }
    
    c = root->FirstChildElement("DiagnoseOption");
    if (c) {
        res = parseDiagnoseOption(config, c);
        if (res < 0)
            return res;
    }

	return 1;
}


int SimulatorConfigParser::parseRecordActor(SimulatorConfig* parent, tinyxml2::XMLElement* root)
{
	parent->actorPathNeedRecord;

	const tinyxml2::XMLElement *c = nullptr;

    for (c = root->FirstChildElement("RecordActorPath"); c; c = c->NextSiblingElement("RecordActorPath"))
    {
        if (c->FindAttribute("Path"))
        {
            string path = string(c->Attribute("Path"));
            parent->actorPathNeedRecord.push_back(path);
        }
    }

	return 1;
}

int SimulatorConfigParser::parseDiagnoseOption(SimulatorConfig* parent, tinyxml2::XMLElement* root)
{
    const tinyxml2::XMLElement* c = nullptr;

    for (c = root->FirstChildElement("DiagnoseType"); c; c = c->NextSiblingElement("DiagnoseType"))
    {
        const char* typeAttribute = c->Attribute("Type");
        const char* value = c->GetText();

        if (typeAttribute && value)
        {
            std::string type = typeAttribute;
            bool status = (strcmp(value, "on") == 0);

            if (type == "IntegerOverflow")
                parent->diagOption.IntegerOverflow = status;
            else if (type == "IntegerSaturation")
                parent->diagOption.IntegerSaturation = status;
            else if (type == "ParameterDowncast")
                parent->diagOption.ParameterDowncast = status;
            else if (type == "ParameterOverflow")
                parent->diagOption.ParameterOverflow = status;
            else if (type == "ParameterPrecisionLoss")
                parent->diagOption.ParameterPrecisionLoss = status;
            else if (type == "Int32ToFloatConv")
                parent->diagOption.Int32ToFloatConv = status;
            else if (type == "ArrayOutOfBound")
                parent->diagOption.ArrayOutOfBound = status;
            else if (type == "DevideByZero")
                parent->diagOption.DevideByZero = status;
            //else if (type == "option9")
            //    parent->diagOption.option9 = status;
            //else if (type == "option10")
            //    parent->diagOption.option10 = status;
        }
    }

    return 1;
}

