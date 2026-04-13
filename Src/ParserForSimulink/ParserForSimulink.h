#pragma once

#include <map>
#include <string>
#include <vector>

#include "../Common/tinyxml2.h"
#include "../Common/Utility.h"




class SLSubsystem;
class SLBlock;
class SLLine;
class SLObject;
class ParserForSimulink
{
public:
	int parseSL(std::string filePath);

	enum {
		ErrorLoadSLModelFail = 70001,
		ErrorLoadSLInnerModelFail,
		ErrorSLBlockHasNoAttributeBlockType,
		ErrorSLBlockHasNoAttributeName,
		ErrorSLBlockHasNoAttributeSID,
		ErrorSLInportHasNoAttributeName,
		ErrorSLInportHasNoAttributeSID,
		ErrorSLOutportHasNoAttributeName,
		ErrorSLOutportHasNoAttributeSID,
		ErrorSLActionPortHasNoAttributeName,
		ErrorSLActionPortHasNoAttributeSID,
		ErrorSLObjectHasNoAttributeName,
		ErrorSLBlockSubSystemHasNoRef,
        ErrorSLFunctionHasNoAttributeMaskParameterName,
        ErrorSLFunctionHasNoAttributeMaskParameterValue,
	};

	std::vector<SLSubsystem*> subsystems;

	std::vector<SLObject*> objectList;

	void release();
private:

	int parseSubsystem(tinyxml2::XMLElement* root, std::string name);
	int parseSubsystemPort(SLSubsystem* subsystem, tinyxml2::XMLElement* root);
	int parseSubsystemPortId(SLObject* port);
    std::vector<int> parseSubsystemPortArraySize(SLObject* port);
    std::string parseSubsystemPortType(SLObject* port);

	int parseBlock(SLSubsystem* subsystem, tinyxml2::XMLElement* root);
	int parseBlockPort(SLBlock* block, tinyxml2::XMLElement* root);
    int parseSLBlockMaskParameter(SLBlock* block, tinyxml2::XMLElement* root);
	int parseLine(SLSubsystem* subsystem, tinyxml2::XMLElement* root);

	int parseParameter(SLObject* object, tinyxml2::XMLElement* root);


    std::map<std::string, std::string> basicTypeNameMap = {
        
	    std::pair<std::string, std::string>("uint8", "u8"),
	    std::pair<std::string, std::string>("int8", "i8"),
	    std::pair<std::string, std::string>("uint16", "u16"),
	    std::pair<std::string, std::string>("int16", "i16"),
	    std::pair<std::string, std::string>("uint32", "u32"),
	    std::pair<std::string, std::string>("int32", "i32"),
	    std::pair<std::string, std::string>("uint64", "u64"),
	    std::pair<std::string, std::string>("int64", "i64"),
	    std::pair<std::string, std::string>("single", "f32"),
	    std::pair<std::string, std::string>("double", "f64"),
	    std::pair<std::string, std::string>("float32", "f32"),
	    std::pair<std::string, std::string>("float64", "f64"),
	    std::pair<std::string, std::string>("boolean", "bool"),
    };
};
