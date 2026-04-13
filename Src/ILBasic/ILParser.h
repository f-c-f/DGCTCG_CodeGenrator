#pragma once
#include <map>
#include <string>
#include <vector>

#include "ILCCodeParser.h"
#include "ILConfig.h"
#include "../Common/tinyxml2.h"
#include "StmtBatchCalculation.h"

class ILExternVariable;
class ILExternFile;
class ILCalculate;
class ILSchedule;
class ILFile;

class ILParser
{
public:
	//ILParser类的主要功能，即：解析中间语言
	int parseIL(std::string xmlFilePath);

	int parseIL(const tinyxml2::XMLElement* root);

    
	static std::string getCurrentParesdILFilePath();

	static std::vector<std::string> allDataTypeList;

	static std::vector<std::string> basicDataTypeList;
    static std::map<std::string, int> basicDataTypeWidthMap;
    static std::map<std::string, bool> basicDataTypeIntegerMap;
    static std::map<std::string, std::string> basicDataTypeSimpleMap;
    static std::map<std::string, std::string> basicDataTypeMap;

    static std::vector<std::string> simdDataTypeList;

	static int getBasicDataTypeWidth(std::string typeName);
	static bool getBasicDataTypeIsInteger(std::string typeName);
	static std::string getBasicDataTypeSimple(std::string typeName);

	static std::vector<ILStructDefine*> structTypeList;
	static ILStructDefine* findStructType(std::string typeName);
	static std::vector<ILEnumDefine*> enumTypeList;
	static ILEnumDefine* findEnumType(std::string typeName);
	static ILEnumValue* findEnumValue(std::string valueName);
	static std::vector<ILTypeDefine*> typeDefineTypeList;
	static ILTypeDefine* findTypeDefineByValue(std::string typeName);
	static ILTypeDefine* findTypeDefineByName(std::string typeName);
    
    static std::string convertDataType(std::string typeName);
    static std::string getRealDataType(std::string typeName);

    static bool isBasicType(std::string typeName);

	ILConfig config;
	std::vector<ILFile*> files;
	std::vector<ILExternFile*> externFiles;

    std::vector<ILFunction*> allFunctions;
    
	enum {
		ErrorLoadILFileFail = 1,
		ErrorILRootMissFileNode,
		ErrorILFunctionMissScheduleNode,
		ErrorILFileHasNoAttributeName,
		ErrorILHeadFileHasNoAttributeName,
		ErrorILMacroHasNoAttributeName,
		ErrorILMacroHasNoAttributeValue,
		ErrorILTypeDefineHasNoAttributeName,
		ErrorILTypeDefineHasNoAttributeValue,
		ErrorILUtilityFunctionHasNoAttributeName,
		ErrorILStructDefineHasNoAttributeName,
		ErrorILEnumDefineHasNoAttributeName,
		ErrorILGlobalVariableHasNoAttributeName,
		ErrorILGlobalVariableHasNoAttributeDefaultValue,
		ErrorILGlobalVariableHasNoAttributeIsAddress,
		ErrorILGlobalVariableHasNoAttributeType,
		ErrorILGlobalVariableHasNoAttributeArraySize,
		ErrorILFunctionHasNoAttributeName,
		ErrorILFunctionInvalidType,
		ErrorILFunctionHasNoAttributeType,
		ErrorILBranchInvalidType,
		ErrorILBranchHasNoAttributeType,
		ErrorILBranchInvalidParentNode,
		ErrorILLocalVariableHasNoAttributeName,
		ErrorILLocalVariableHasNoAttributeDefaultValue,
		ErrorILLocalVariableHasNoAttributeIsAddress,
		ErrorILLocalVariableHasNoAttributeType,
		ErrorILLocalVariableHasNoAttributeArraySize,
		ErrorILInputHasNoAttributeName,
		ErrorILInputHasNoAttributeDefaultValue,
		ErrorILInputHasNoAttributeIsAddress,
		ErrorILInputHasNoAttributeType,
		ErrorILInputHasNoAttributeArraySize,
		ErrorILInputInvalidParentNode,
		ErrorILOutputHasNoAttributeName,
		ErrorILOutputHasNoAttributeDefaultValue,
		ErrorILOutputHasNoAttributeIsAddress,
		ErrorILOutputHasNoAttributeType,
		ErrorILOutputHasNoAttributeArraySize,
		ErrorILOutputInvalidParentNode,
		ErrorILRefHasNoAttributeActor,
		ErrorILRefHasNoAttributePath,
		ErrorILRefInvalidParentNode,
		ErrorILFunctionHasMultiSchedule,
		ErrorILMemberHasNoAttributeName,
		ErrorILMemberHasNoAttributeIsAddress,
		ErrorILMemberHasNoAttributeType,
		ErrorILMemberHasNoAttributeArraySize,
		ErrorILEnumValueHasNoAttributeName,
		ErrorILCalculateHasNoAttributeName,
		ErrorILCalculateHasNoAttributeIsOptimized,
		ErrorILCalculateInvalidParentNode,
		ErrorStatementInvalidType,
		ErrorStatementIfHasNoAttributeCondition,
		ErrorStatementElseIfHasNoAttributeCondition,
		ErrorStatementWhileHasNoAttributeCondition,
		ErrorStatementDoWhileHasNoAttributeCondition,
		ErrorStatementForHasNoStatementInitial,
		ErrorStatementForHasNoStatementCondition,
		ErrorStatementForHasNoStatementIterator,
		ErrorStatementForConditionHasNoAttributeCondition,
		ErrorStatementSwitchHasNoAttributeCondition,
		ErrorStatementCaseHasNoAttributeValue,
		ErrorStatementLocalVariableHasNoAttributeValue,
		ErrorStatementLocalVariableHasNoAttributeType,
		ErrorStatementBatchCalculationHasNoAttributeOperation,
        ErrorStatementBatchCalculationHasNoAttributeDataType,
		ErrorStatementBatchCalculationHasInvalidOperation,
        ErrorStatementBatchCalculationInputHasNoAttributeName,
        ErrorStatementBatchCalculationInputHasNoAttributeType,
        ErrorStatementBatchCalculationOutputHasNoAttributeName,
        ErrorStatementBatchCalculationOutputHasNoAttributeType,
        ErrorStatementBatchCalculationInputSizeInvalid,
        ErrorStatementAssembleInputHasNoAttributeName,
        ErrorStatementAssembleInputHasNoAttributeType,
        ErrorStatementAssembleOutputHasNoAttributeName,
        ErrorStatementAssembleOutputHasNoAttributeType,
		ErrorILRootMissConfigNode,
		ErrorXMLNodeMissName,
        ErrorTypeRedefined,
        ErrorVariableConflict,
		ErrorILExternFileHasNoAttributeName,
        ErrorILExternVariableHasNoAttributeName,
        ErrorILConfigParameterHasNoAttributeName,
        ErrorILConfigParameterHasNoAttributeType,
	};

	std::vector<std::string> errorStrList = {
		"ErrorLoadILFileFail = 1,                                ",
		"ErrorILRootMissFileNode,                                ",
		"ErrorILFunctionMissScheduleNode,                        ",
		"ErrorILFileHasNoAttributeName,                          ",
		"ErrorILHeadFileHasNoAttributeName,                      ",
		"ErrorILMacroHasNoAttributeName,                         ",
		"ErrorILMacroHasNoAttributeValue,                        ",
		"ErrorILTypeDefineHasNoAttributeName,                    ",
		"ErrorILTypeDefineHasNoAttributeValue,                   ",
		"ErrorILUtilityFunctionHasNoAttributeName,               ",
		"ErrorILStructDefineHasNoAttributeName,                  ",
		"ErrorILEnumDefineHasNoAttributeName,                    ",
		"ErrorILGlobalVariableHasNoAttributeName,                ",
		"ErrorILGlobalVariableHasNoAttributeDefaultValue,        ",
		"ErrorILGlobalVariableHasNoAttributeIsAddress,           ",
		"ErrorILGlobalVariableHasNoAttributeType,                ",
		"ErrorILGlobalVariableHasNoAttributeArraySize,           ",
		"ErrorILFunctionHasNoAttributeName,                      ",
		"ErrorILFunctionInvalidType,                             ",
		"ErrorILFunctionHasNoAttributeType,                      ",
		"ErrorILBranchInvalidType,                               ",
		"ErrorILBranchHasNoAttributeType,                        ",
		"ErrorILBranchInvalidParentNode,                         ",
		"ErrorILLocalVariableHasNoAttributeName,                 ",
		"ErrorILLocalVariableHasNoAttributeDefaultValue,         ",
		"ErrorILLocalVariableHasNoAttributeIsAddress,            ",
		"ErrorILLocalVariableHasNoAttributeType,                 ",
		"ErrorILLocalVariableHasNoAttributeArraySize,            ",
		"ErrorILInputHasNoAttributeName,                         ",
		"ErrorILInputHasNoAttributeDefaultValue,                 ",
		"ErrorILInputHasNoAttributeIsAddress,                    ",
		"ErrorILInputHasNoAttributeType,                         ",
		"ErrorILInputHasNoAttributeArraySize,                    ",
		"ErrorILInputInvalidParentNode,                          ",
		"ErrorILOutputHasNoAttributeName,                        ",
		"ErrorILOutputHasNoAttributeDefaultValue,                ",
		"ErrorILOutputHasNoAttributeIsAddress,                   ",
		"ErrorILOutputHasNoAttributeType,                        ",
		"ErrorILOutputHasNoAttributeArraySize,                   ",
		"ErrorILOutputInvalidParentNode,                         ",
		"ErrorILRefHasNoAttributeActor,                          ",
		"ErrorILRefHasNoAttributePath,                           ",
		"ErrorILRefInvalidParentNode,                            ",
		"ErrorILFunctionHasMultiSchedule,                        ",
		"ErrorILMemberHasNoAttributeName,                        ",
		"ErrorILMemberHasNoAttributeIsAddress,                   ",
		"ErrorILMemberHasNoAttributeType,                        ",
		"ErrorILMemberHasNoAttributeArraySize,                   ",
		"ErrorILEnumValueHasNoAttributeName,                     ",
		"ErrorILCalculateHasNoAttributeName,                     ",
		"ErrorILCalculateHasNoAttributeIsOptimized,              ",
		"ErrorILCalculateInvalidParentNode,                      ",
		"ErrorStatementInvalidType,                              ",
		"ErrorStatementIfHasNoAttributeCondition,                ",
		"ErrorStatementElseIfHasNoAttributeCondition,            ",
		"ErrorStatementWhileHasNoAttributeCondition,             ",
		"ErrorStatementDoWhileHasNoAttributeCondition,           ",
		"ErrorStatementForHasNoStatementInitial,                 ",
		"ErrorStatementForHasNoStatementCondition,               ",
		"ErrorStatementForHasNoStatementIterator,                ",
		"ErrorStatementForConditionHasNoAttributeCondition,      ",
		"ErrorStatementSwitchHasNoAttributeCondition,            ",
		"ErrorStatementCaseHasNoAttributeValue,                  ",
		"ErrorStatementLocalVariableHasNoAttributeValue,         ",
		"ErrorStatementLocalVariableHasNoAttributeType,          ",
		"ErrorStatementBatchCalculationHasNoAttributeOperation   ",
		"ErrorStatementBatchCalculationHasNoAttributeDataType    ",
        "ErrorStatementBatchCalculationHasInvalidOperation       ",
        "ErrorStatementBatchCalculationInputHasNoAttributeName,  ",
        "ErrorStatementBatchCalculationInputHasNoAttributeType,  ",
        "ErrorStatementBatchCalculationOutputHasNoAttributeName, ",
        "ErrorStatementBatchCalculationOutputHasNoAttributeType, ",
        "ErrorStatementBatchCalculationInputSizeInvalid,         ",
        "ErrorStatementAssembleInputHasNoAttributeName,          ",
        "ErrorStatementAssembleInputHasNoAttributeType,          ",
        "ErrorStatementAssembleOutputHasNoAttributeName,         ",
        "ErrorStatementAssembleOutputHasNoAttributeType,         ",
		"ErrorILRootMissConfigNode,                              ",
		"ErrorXMLNodeMissName,                                   ",
        "ErrorTypeRedefined,                                     ",
        "ErrorVariableConflict,                                  ",
		"ErrorILExternFileHasNoAttributeName,                    ",
        "ErrorILExternVariableHasNoAttributeName,                ",
        "ErrorILConfigParameterHasNoAttributeName,               ",
        "ErrorILConfigParameterHasNoAttributeType,               ",
	};

	void release();
	void releaseStatement(Statement* stmt) const;
	void releaseILObject(ILObject* object) const;
    static void clearVariableAndDataTypeRecord();

    class ILVariable
    {
    public:
        ILVariable() {};
        ILVariable(std::string name, const std::vector<void*>& domain);
        ~ILVariable() {};
        std::string name;
        std::vector<void*> domain;
    };
    static std::vector<ILVariable> variableList;
    static std::vector<ILVariable> variableListRegistered;
    static int addVariableName(std::string variableName, const std::vector<void*>& domain);
    static int registerVariableName(std::string variableName, const std::vector<void*>& domain);
    static std::string getAvailableVariableName(std::string variableNamePrefix, const std::vector<void*>& domain);
    static ILVariable* getVariableInDomain(std::string variableName, const std::vector<void*>& domain);


	std::string getErrorStr();

    int parseHeadFileIndependent(const tinyxml2::XMLElement* c, ILHeadFile* retILHeadFile);
    int parseMacroIndependent(const tinyxml2::XMLElement* c, ILMacro* retILMacro);
    int parseTypeDefineIndependent(const tinyxml2::XMLElement* c, ILTypeDefine* retILTypeDefine);
    int parseEnumDefineIndependent(const tinyxml2::XMLElement* c, ILEnumDefine* retILEnumDefine);
    int parseStructDefineIndependent(const tinyxml2::XMLElement* c, ILStructDefine* retILStructDefine);
    int parseUtilityFunctionIndependent(const tinyxml2::XMLElement* c, ILUtilityFunction* retILUtilityFunction);
    int parseGlobalVariableIndependent(const tinyxml2::XMLElement* c, ILGlobalVariable* retILGlobalVariable);
    int parseExternVariableIndependent(const tinyxml2::XMLElement* c, ILExternVariable* retILExternVariable);
    int parseLocalVariableIndependent(const tinyxml2::XMLElement* c, ILLocalVariable* retILLocalVariable);
    int parseCalculateIndependent(const tinyxml2::XMLElement* c, ILCalculate* retILCalculate);

	ILFunction* findILFunction(std::string name) const;


private:
	int errorCode = 0;
	void setCurrentError(int errorCode);

	int currentReadXMLLineNum = -1;
	const tinyxml2::XMLElement* currentReadXMLElement = nullptr;
	void setCurrentReadXMLElement(const tinyxml2::XMLElement* ele);

    static std::string iLFilePath;

	int parseConfig(const tinyxml2::XMLElement* root);
    int parseConfigParameter(const tinyxml2::XMLElement* root);

    void parseAllAttribute(ILObject* obj, const tinyxml2::XMLElement* root) const;

	int parseFile(const tinyxml2::XMLElement* root);
	int parseExternFile(const tinyxml2::XMLElement* root);

	int parseHeadFile(ILFile* file, const tinyxml2::XMLElement* root);
	int parseMacro(ILFile* file, const tinyxml2::XMLElement* root);
	int parseTypeDefine(ILFile* file, const tinyxml2::XMLElement* root);
	int parseStructDefine(ILFile* file, const tinyxml2::XMLElement* root);
	int parseEnumDefine(ILFile* file, const tinyxml2::XMLElement* root);
	int parseGlobalVariable(ILFile* file, const tinyxml2::XMLElement* root);
	int parseExternVariable(ILFile* file, const tinyxml2::XMLElement* root);
	int parseUtilityFunction(ILFile* file, const tinyxml2::XMLElement* root);
	int parseFunction(ILFile* file, const tinyxml2::XMLElement* root);

    //comment = true;
	//int parseBranch(ILObject* parent, tinyxml2::XMLElement* root);
	//int parseCalculate(ILObject* parent, tinyxml2::XMLElement* root);
	//int parseLocalVariable(ILSchedule* schedule, tinyxml2::XMLElement* root);

	int parseInput(ILObject* parent, const tinyxml2::XMLElement* root);
	int parseOutput(ILObject* parent, const tinyxml2::XMLElement* root);
	//int parseRange(ILObject* parent, const tinyxml2::XMLElement* root);
	int parseRef(ILObject* parent, const tinyxml2::XMLElement* root);
	int parseSchedule(ILFunction* function, const tinyxml2::XMLElement* root);

	int parseMember(ILStructDefine* structDefine, const tinyxml2::XMLElement* root);
	int parseEnumValue(ILEnumDefine* enumDefine, const tinyxml2::XMLElement* root);

	int parseScheduleNode(ILObject* parent, const tinyxml2::XMLElement* root);
    int parseLocalVariable(const tinyxml2::XMLElement* c, ILObject* parent, const tinyxml2::XMLElement* root);
    int parseCalculate(const tinyxml2::XMLElement* c, ILObject* parent, const tinyxml2::XMLElement* root);
    int parseBranch(const tinyxml2::XMLElement* c, ILObject* parent, const tinyxml2::XMLElement* root);
    
	int parseStatement(Statement* parent, const tinyxml2::XMLElement* root);
	
	int parseStmtIf(Statement* parent, const tinyxml2::XMLElement* root);
	int parseStmtElse(Statement* parent, const tinyxml2::XMLElement* root);
	int parseStmtElseIf(Statement* parent, const tinyxml2::XMLElement* root);
	int parseStmtWhile(Statement* parent, const tinyxml2::XMLElement* root);
	int parseStmtDoWhile(Statement* parent, const tinyxml2::XMLElement* root);
	int parseStmtFor(Statement* parent, const tinyxml2::XMLElement* root);
	int parseStmtContinue(Statement* parent, const tinyxml2::XMLElement* root) const;
	int parseStmtSwitch(Statement* parent, const tinyxml2::XMLElement* root);
	int parseStmtCase(Statement* parent, const tinyxml2::XMLElement* root) const;
	int parseStmtDefault(Statement* parent, const tinyxml2::XMLElement* root) const;
	int parseStmtBreak(Statement* parent, const tinyxml2::XMLElement* root) const;
	int parseStmtReturn(Statement* parent, const tinyxml2::XMLElement* root) const;
	//int parseStmtBraceStatement(Statement* parent, const tinyxml2::XMLElement* root) const;
	int parseStmtLocalVariable(Statement* parent, const tinyxml2::XMLElement* root) const;
	int parseStmtExpression(Statement* parent, const tinyxml2::XMLElement* root) const;
	int parseStmtBatchCalculation(Statement* parent, const tinyxml2::XMLElement* root);
	int parseStmtBatchCalculationInput(Statement* parent, const tinyxml2::XMLElement* root);
	int parseStmtBatchCalculationOutput(Statement* parent, const tinyxml2::XMLElement* root);
	int parseStmtBatchCalculationRange(StmtBatchCalculation::StmtBatchCalculationInput* parent, const tinyxml2::XMLElement* root);
	int parseStmtBatchCalculationRange(StmtBatchCalculation::StmtBatchCalculationOutput* parent, const tinyxml2::XMLElement* root);
	int parseStmtAssemble(Statement* parent, const tinyxml2::XMLElement* root);
	int parseStmtAssembleInput(Statement* parent, const tinyxml2::XMLElement* root);
	int parseStmtAssembleOutput(Statement* parent, const tinyxml2::XMLElement* root);
	

	int parseExpression(Statement* parent, std::string expressionStr) const;
    int parseExpressionDefaultValue(std::string expressionStr, Expression** retExpression) const;

    int releaseILObjectAndReturn(ILObject* object, int retCode) const;
    int releaseStatementAndReturn(Statement* stmt, int retCode) const;

    int parseUtilityIsAddress(std::string str) const;
	bool parseUtilityIsOptimized(std::string str) const;
    std::vector<int> parseUtilityArraySize(std::string str) const;
	std::vector<int> parseUtilityIndex(std::string str) const;
    //comment = true;
	//std::vector<ILObject*> objects;
	//std::vector<Statement*> statements;

    int collectFunctionCall(Statement* parent, Expression* exp) const;
};


