#pragma once
#include<string>
#include<vector>
#include<map>

#include "SimulatorConfig.h"
#include "../Common/tinyxml2.h"
class SimulatorConfig;
class ILParser;
class SimulatorConfigParser {
public:
	enum {
		ErrorLoadSimulatorConfigFileFail = 190001,
        ErrorSimulatorConfigFileHasNoRecordActor,
	};
    SimulatorConfigParser(SimulatorConfig* config, ILParser* ilParser);
	~SimulatorConfigParser();
	int parseConfig(std::string xmlFilePath);
    SimulatorConfig* config = nullptr;
	ILParser* ilParser = nullptr;
    
private:
	int parseConfig(tinyxml2::XMLElement* root);
	static int parseRecordActor(SimulatorConfig* parent, tinyxml2::XMLElement* root);
    static int parseDiagnoseOption(SimulatorConfig* parent, tinyxml2::XMLElement* root);
};

// 
