#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <map>

#include "StmtBatchCalculation.h"
#include "../Common/tinyxml2.h"
#include "../ILBasic/ILParser.h"


class ILCalculate;
class ILBranch;
class ILFile;
class ILMacro;
class ILHeadFile;
class ILObject;
class ILStructDefine;
class StructType;

class ILFormatter
{
public:
	int format(tinyxml2::XMLDocument& doc);
	int format(std::string file, std::string outputFilePath);
	enum {
		ErrorLoadILFileFail = 50001,
		ErrorPaesrFormattedILFileFail,
		ErrorSaveFormattedILFileFail,
		ErrorDeduplicationFormattedILFileFail,
		ErrorStructDefineMissNameAttribute,
		ErrorStructMemberDefineMissNameAttribute,
		ErrorStructMemberDefineMissTypeAttribute,
		ErrorStructDefineRedefined,
        ErrorILTypeDefineHasNoAttributeName,
        ErrorILTypeDefineHasNoAttributeValue,
        ErrorTypeRedefined,
        ErrorILEnumDefineHasNoAttributeName,
        ErrorILEnumValueHasNoAttributeName,
		ErrorPlaceHolderMissCodeAttribute,
		ErrorLoadILFileFailInner,
		ErrorCreateXMLNodeFail,
		ErrorXMLNodeMissName,
	};

	std::vector<std::string> errorStrList = {
		"ErrorLoadILFileFail,					  ",
		"ErrorPaesrFormattedILFileFail,			  ",
		"ErrorSaveFormattedILFileFail,			  ",
		"ErrorDeduplicationFormattedILFileFail,   ",
		"ErrorStructDefineMissNameAttribute,	  ",
		"ErrorStructMemberDefineMissNameAttribute,",
		"ErrorStructMemberDefineMissTypeAttribute,",
		"ErrorStructDefineRedefined,			  ",
        "ErrorILTypeDefineHasNoAttributeName,	  ",
        "ErrorILTypeDefineHasNoAttributeValue,	  ",
        "ErrorTypeRedefined,	                  ",
        "ErrorILEnumDefineHasNoAttributeName,	  ",
        "ErrorILEnumValueHasNoAttributeName,	  ",
		"ErrorPlaceHolderMissCodeAttribute,		  ",
		"ErrorLoadILFileFailInner,                ",
		"ErrorCreateXMLNodeFail,                  ",
		"ErrorXMLNodeMissName,                    ",
	};
	
	std::string getErrorStr();
private:
	int errorCode = 0;
	void setCurrentError(int errorCode);

    ILParser ilParser;

	int currentReadXMLLineNum = -1;
	const tinyxml2::XMLElement* currentReadXMLElement = nullptr;
	void setCurrentReadXMLElement(const tinyxml2::XMLElement* ele);

	//解析中间代码中的结构体定义，到ILParser的StructTypeEnum�?	int collectStructDefineA(const tinyxml2::XMLElement* root);
	int collectStructDefineA(const tinyxml2::XMLElement* root);
	int collectStructDefineB(const tinyxml2::XMLElement* root);
	int collectStructDefineMemberA(const tinyxml2::XMLElement* root, ILStructDefine* newStructType);
    int collectStructDefineMemberB(const tinyxml2::XMLElement* root, ILStructDefine* newStructType);
    
	int collectTypeDefine(const tinyxml2::XMLElement* root);

    int collectEnumDefine(const tinyxml2::XMLElement* root);
    int collectEnumValue(const tinyxml2::XMLElement* root, ILEnumDefine* enumDefine);
    
    int traverseRef(const tinyxml2::XMLElement* root, tinyxml2::XMLElement* rootFormat, tinyxml2::XMLDocument* doc);
	int traverseFuncState(const tinyxml2::XMLElement* root, tinyxml2::XMLElement* rootFormat, tinyxml2::XMLDocument* doc);
	int traverse(const tinyxml2::XMLElement* root, tinyxml2::XMLElement* rootFormat, tinyxml2::XMLDocument* doc);
	int traverseA(const tinyxml2::XMLElement* root, tinyxml2::XMLElement* rootFormat, tinyxml2::XMLDocument* doc, const tinyxml2::XMLElement* c, bool &loopContinue);

    static std::map<std::string, std::string> transNameMap;
    static std::map<std::string, std::string> transAttrMap;
    static std::map<std::string, std::string> transValueMap;
    int translate(const tinyxml2::XMLElement* root, tinyxml2::XMLElement* rootFormat, tinyxml2::XMLDocument* doc) const;
    int translateAttribute(const tinyxml2::XMLAttribute* attribut, std::string name, tinyxml2::XMLElement* rootFormat, tinyxml2::XMLDocument* doc) const;

    int deduplication(const ILParser* parser) const;
    int deduplicationHeadFile(const ILFile* file, const ILParser* parser) const;
    int deduplicationMacro(const ILFile* file, const ILParser* parser) const;

    std::map<std::string, std::string> batchCalculationNameToRealName;
    ILSchedule* renameBatchCalculationCurrentSchedule = nullptr;
    ILCalculate* renameBatchCalculationCurrentCalculate = nullptr;
    int renameBatchCalculationTempVariable(const ILParser* parser);
    int renameBatchCalculationTraverseScheduleNode(ILSchedule* schedule);
    int renameBatchCalculationTraverseBranch(const ILBranch* branch);
    int renameBatchCalculationTraverseCalculate(const ILCalculate* calculate);
    int renameBatchCalculationTraverseStatement(Statement* statement);
    int renameBatchCalculationInputVariable(StmtBatchCalculation::StmtBatchCalculationInput* input, const StmtBatchCalculation* stmt);
    int renameBatchCalculationOutputVariable(StmtBatchCalculation::StmtBatchCalculationOutput* output, const StmtBatchCalculation* stmt);


    int releaseILObjectAndReturn(ILObject* obj, int returnValue) const;
};
