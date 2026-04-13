#include "ILParser.h"

#include <algorithm>
#include <set>

#include "Expression.h"
#include "../Common/Utility.h"

#include "StmtBreak.h"
#include "StmtCase.h"
#include "StmtContinue.h"
#include "StmtDefault.h"
#include "StmtDoWhile.h"
#include "StmtElse.h"
#include "StmtElseIf.h"
#include "StmtExpression.h"
#include "StmtFor.h"
#include "StmtIf.h"
#include "StmtLocalVariable.h"
#include "StmtReturn.h"
#include "StmtSwitch.h"
#include "StmtWhile.h"
#include "StmtBatchCalculation.h"
#include "StmtAssemble.h"

#include "ILExpressionParser.h"

#include "ILHeadFile.h"
#include "ILMacro.h"
#include "ILTypeDefine.h"
#include "ILUtilityFunction.h"
#include "ILStructDefine.h"
#include "ILEnumDefine.h"
#include "ILGlobalVariable.h"
#include "ILFunction.h"
#include "ILInput.h"
#include "ILOutput.h"
#include "ILRef.h"
#include "ILBranch.h"
#include "ILCalculate.h"
#include "ILFile.h"
#include "ILMember.h"
#include "ILEnumValue.h"
#include "ILLocalVariable.h"
#include "ILSchedule.h"
#include "ILExternFile.h"
#include "ILExternVariable.h"

using namespace std;
namespace
{
    const int CONST_NUM_2 = 2;
    const int CONST_NUM_7 = 7;
}



vector<string> ILParser::basicDataTypeList = {
    "void",

    "bool",

    "char",

    "signed char",

    "unsigned char",
    
    "short int",
    "short",
    "signed short int",
    "signed short",
    
    "unsigned short int",
    "unsigned short",
    
    "long long int",
    "long long",
    "signed long long int",
    "signed long long",
    
    "long double",

    "long int",
    "long",
    "signed long int",
    "signed long",
    
    "unsigned long long int",
    "unsigned long long",
    
    "unsigned long int",
    "unsigned long",

    "int",
    "signed int",
    "signed",
    
    "unsigned int",
    "unsigned",

    "float",

    "double",
};

std::map<std::string, int> ILParser::basicDataTypeWidthMap = {
    pair<string, int>("bool", 1),

    pair<string, int>("char", 1),

    pair<string, int>("signed char", 1),

    pair<string, int>("unsigned char", 1),

    pair<string, int>("short int", 2),
    pair<string, int>("short", 2),
    pair<string, int>("signed short int", 2),
    pair<string, int>("signed short", 2),

    pair<string, int>("unsigned short int", 2),
    pair<string, int>("unsigned short", 2),

    pair<string, int>("long long int", 8),
    pair<string, int>("long long", 8),
    pair<string, int>("signed long long int", 8),
    pair<string, int>("signed long long", 8),

    pair<string, int>("long double", 16),

    pair<string, int>("long int", 4),
    pair<string, int>("long", 4),
    pair<string, int>("signed long int", 4),
    pair<string, int>("signed long", 4),

    pair<string, int>("unsigned long long int", 8),
    pair<string, int>("unsigned long long", 8),

    pair<string, int>("unsigned long int", 4),
    pair<string, int>("unsigned long", 4),

    pair<string, int>("int", 4),
    pair<string, int>("signed int", 4),
    pair<string, int>("signed", 4),

    pair<string, int>("unsigned int", 4),
    pair<string, int>("unsigned", 4),

    pair<string, int>("float", 4),

    pair<string, int>("double", 8),
};

std::map<std::string, bool> ILParser::basicDataTypeIntegerMap = {
    pair<string, bool>("bool", true),

    pair<string, bool>("char", true),

    pair<string, bool>("signed char", true),

    pair<string, bool>("unsigned char", true),

    pair<string, bool>("short int", true),
    pair<string, bool>("short", true),
    pair<string, bool>("signed short int", true),
    pair<string, bool>("signed short", true),

    pair<string, bool>("unsigned short int", true),
    pair<string, bool>("unsigned short", true),

    pair<string, bool>("long long int", true),
    pair<string, bool>("long long", true),
    pair<string, bool>("signed long long int", true),
    pair<string, bool>("signed long long", true),

    pair<string, bool>("long double", false),

    pair<string, bool>("long int", true),
    pair<string, bool>("long", true),
    pair<string, bool>("signed long int", true),
    pair<string, bool>("signed long", true),

    pair<string, bool>("unsigned long long int", true),
    pair<string, bool>("unsigned long long", true),

    pair<string, bool>("unsigned long int", true),
    pair<string, bool>("unsigned long", true),

    pair<string, bool>("int", true),
    pair<string, bool>("signed int", true),
    pair<string, bool>("signed", true),

    pair<string, bool>("unsigned int", true),
    pair<string, bool>("unsigned", true),

    pair<string, bool>("float", false),

    pair<string, bool>("double", false),
};

std::map<std::string, std::string> ILParser::basicDataTypeSimpleMap = {
    pair<string, string>("bool", "u8"),

    pair<string, string>("char", "i8"),

    pair<string, string>("signed char", "i8"),

    pair<string, string>("unsigned char", "u8"),

    pair<string, string>("short int", "i16"),
    pair<string, string>("short", "i16"),
    pair<string, string>("signed short int", "i16"),
    pair<string, string>("signed short", "i16"),

    pair<string, string>("unsigned short int", "u16"),
    pair<string, string>("unsigned short", "u16"),

    pair<string, string>("long long int", "i32"),
    pair<string, string>("long long", "i32"),
    pair<string, string>("signed long long int", "i32"),
    pair<string, string>("signed long long", "i32"),

    pair<string, string>("long double", "f128"),

    pair<string, string>("long int", "i32"),
    pair<string, string>("long", "i32"),
    pair<string, string>("signed long int", "i32"),
    pair<string, string>("signed long", "i32"),

    pair<string, string>("unsigned long long int", "u64"),
    pair<string, string>("unsigned long long", "u64"),

    pair<string, string>("unsigned long int", "u32"),
    pair<string, string>("unsigned long", "u32"),

    pair<string, string>("int", "i32"),
    pair<string, string>("signed int", "i32"),
    pair<string, string>("signed", "i32"),

    pair<string, string>("unsigned int", "u32"),
    pair<string, string>("unsigned", "u32"),

    pair<string, string>("float", "f32"),

    pair<string, string>("double", "f64"),
};

map<string, string> ILParser::basicDataTypeMap = {
    pair<string, string>("i32", "int"),
    pair<string, string>("f32", "float"),
    pair<string, string>("i64", "long long"),
    pair<string, string>("f64", "double"),
    pair<string, string>("f128", "long double"),
    pair<string, string>("i8", "char"),
    pair<string, string>("i16", "short"),
    pair<string, string>("u32", "unsigned int"),
    pair<string, string>("u64", "unsigned long long"),
    pair<string, string>("u8", "unsigned char"),
    pair<string, string>("u16", "unsigned short"),
    pair<string, string>("short int", "short"),
    pair<string, string>("sint32", "int"),
};



vector<string> ILParser::simdDataTypeList = {
    "int32x4_t",
    "float32x4_t",
    "__m128i",
    "__m128",
};


vector<ILStructDefine*> ILParser::structTypeList;
vector<ILEnumDefine*> ILParser::enumTypeList;
vector<ILTypeDefine*> ILParser::typeDefineTypeList;

vector<string> ILParser::allDataTypeList = basicDataTypeList;


vector<ILParser::ILVariable> ILParser::variableList;
vector<ILParser::ILVariable> ILParser::variableListRegistered;

std::string ILParser::iLFilePath;

int ILParser::parseIL(string xmlFilePath)
{
    iLFilePath = xmlFilePath;
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError ret = doc.LoadFile(xmlFilePath.c_str());
	if (ret != 0) {
		fprintf(stderr, "Fail to load IL file: %s\n", xmlFilePath.c_str());
        fprintf(stderr, "%s\n", doc.ErrorStr());
		return -ErrorLoadILFileFail;
	}
	tinyxml2::XMLElement* root = doc.RootElement();
	if (root == nullptr) {
		fprintf(stderr, "Fail to get root element of IL file: %s (file may be empty or not valid XML)\n", xmlFilePath.c_str());
		return -ErrorLoadILFileFail;
	}

	int res = parseIL(root);
	if(res < 0)
		return res;
	return 1;
}

int ILParser::parseIL(const tinyxml2::XMLElement* root)
{
	int res = parseConfig(root);
	if(res < 0)
	{
		setCurrentError(res);
		return res;
	}

	res = parseFile(root);
	if(res < 0)
	{
		setCurrentError(res);
		return res;
	}

    res = parseExternFile(root);
	if(res < 0)
	{
		setCurrentError(res);
		return res;
	}
	return 1;
}

std::string ILParser::getCurrentParesdILFilePath()
{
    return iLFilePath;
}

int ILParser::getBasicDataTypeWidth(std::string typeName)
{
    string realType = convertDataType(typeName);
    if(basicDataTypeWidthMap.find(realType) != basicDataTypeWidthMap.end())
        return basicDataTypeWidthMap.at(realType);
    return 0;
}

bool ILParser::getBasicDataTypeIsInteger(std::string typeName)
{
    string realType = convertDataType(typeName);
    if(basicDataTypeIntegerMap.find(realType) != basicDataTypeIntegerMap.end())
        return basicDataTypeIntegerMap.at(realType);
    return false;
}

std::string ILParser::getBasicDataTypeSimple(std::string typeName)
{
    string realType = convertDataType(typeName);
    if(basicDataTypeSimpleMap.find(realType) != basicDataTypeSimpleMap.end())
        return basicDataTypeSimpleMap.at(realType);
    return typeName;
}

ILStructDefine* ILParser::findStructType(string typeName)
{
    ILTypeDefine* typeDef = ILParser::findTypeDefineByValue(typeName);
    set<ILTypeDefine*> foundILTypeDefine;
    while(typeDef && foundILTypeDefine.find(typeDef) == foundILTypeDefine.end())
    {
        foundILTypeDefine.insert(typeDef);
        typeName = typeDef->name;
        typeDef = ILParser::findTypeDefineByValue(typeName);
    }

    if(stringStartsWith(typeName, "struct "))
    {
        typeName = typeName.substr(CONST_NUM_7, typeName.length() - CONST_NUM_7);
    }

	for(int i = 0;i < structTypeList.size();i++)
	{
		if(structTypeList[i]->name == typeName)
			return structTypeList[i];
	}
	return nullptr;
}

ILEnumDefine* ILParser::findEnumType(std::string typeName)
{
	for(int i = 0;i < enumTypeList.size();i++)
	{
		if(enumTypeList[i]->name == typeName)
			return enumTypeList[i];
	}
	return nullptr;
}

ILEnumValue* ILParser::findEnumValue(std::string valueName)
{
    for(int i = 0;i < enumTypeList.size();i++)
	{
        for(int j = 0; j < enumTypeList[i]->enumValues.size(); j++)
        {
            if(enumTypeList[i]->enumValues[j]->name == valueName)
                return enumTypeList[i]->enumValues[j];
        }
	}
	return nullptr;
}

ILTypeDefine* ILParser::findTypeDefineByValue(std::string typeName)
{
	for(int i = 0;i < typeDefineTypeList.size();i++)
	{
		if(typeDefineTypeList[i]->value == typeName)
			return typeDefineTypeList[i];
	}
	return nullptr;
}

ILTypeDefine* ILParser::findTypeDefineByName(std::string typeName)
{
	for(int i = 0;i < typeDefineTypeList.size();i++)
	{
		if(typeDefineTypeList[i]->name == typeName)
			return typeDefineTypeList[i];
	}
	return nullptr;
}

void ILParser::setCurrentError(int errorCode)
{
	this->errorCode = errorCode;
}

ILParser::ILVariable::ILVariable(std::string name, const std::vector<void*>& domain)
{
    this->name = name;
    this->domain = domain;
}


int ILParser::addVariableName(std::string variableName, const std::vector<void*>& domain)
{
    // !(variable->domain.empty() && !domain.empty()) ??????????????????????????
    ILVariable* variable = getVariableInDomain(variableName, domain);
    if(variable && !(variable->domain.empty() && !domain.empty())) {
        return -ErrorVariableConflict;
    }
    ILParser::variableList.push_back(ILVariable(variableName, domain));
    return 1;
}

int ILParser::registerVariableName(std::string variableName, const std::vector<void*>& domain)
{
    if(getVariableInDomain(variableName, domain)) {
        return -ErrorVariableConflict;
    }
     ILParser::variableListRegistered.push_back(ILVariable(variableName, domain));
    return 1;
}

std::string ILParser::getAvailableVariableName(std::string variableNamePrefix, const std::vector<void*>& domain)
{
    string tryVariableName = variableNamePrefix;
    int tryCount = 1;
    while(getVariableInDomain(tryVariableName, domain))
    {
        tryVariableName = variableNamePrefix + to_string(tryCount);
        tryCount++;
    }
    return tryVariableName;
}

ILParser::ILVariable* ILParser::getVariableInDomain(std::string variableName, const std::vector<void*>& domain)
{
    for(int i = 0; i < variableList.size() + variableListRegistered.size(); i++)
    {
        ILVariable* variable = nullptr;
        if(i < variableList.size())
            variable = &(variableList[i]);
        else
            variable = &(variableListRegistered[i - variableList.size()]);

        if(variable->name != variableName)
            continue;

        //?????????????????????????????????????????
        if(variable->domain.size() > domain.size())
            continue;
        bool containDomain = true;
        for(int i = 0; i < variable->domain.size(); i++) {
            if(domain[i] != variable->domain[i]) {
                containDomain = false;
                break;
            }
        }
        if(!containDomain) {
            continue;
        }

        return variable;
    }
    return nullptr;
}

string ILParser::getErrorStr()
{
	string ret = "ILParser\nError Code: " + to_string(errorCode) + "\n";
	if(currentReadXMLElement)
		ret += "XML line: " + to_string(currentReadXMLLineNum);
	else
		ret += "XML line: Unclear.";
	
	ret += "\n";

	int errorCodeIndex = -errorCode - 1;

	if(errorCodeIndex >= 0 && errorCodeIndex < errorStrList.size())
		ret += errorStrList[errorCodeIndex];
	else
		ret += "Unknow error code.";

	return ret;
}


void ILParser::setCurrentReadXMLElement(const tinyxml2::XMLElement* ele)
{
	currentReadXMLLineNum = ele->GetLineNum();
	currentReadXMLElement = ele;
}

int ILParser::parseConfig(const tinyxml2::XMLElement* root)
{
	config.iterationCount = 1;
	config.mainInitFunction = "";
	config.mainExecFunction = "";
	config.mainCompositeState = "";
	const tinyxml2::XMLElement *c = root->FirstChildElement("Config");
	if(!c)
		return 0;
    // comment = true;
    // -ErrorILRootMissConfigNode;
	setCurrentReadXMLElement(c);
	root = c;
	c = root->FirstChildElement("IterationCount");
	if(c && c->FindAttribute("Value"))
	{
		config.iterationCount = c->IntAttribute("Value");
	}
	c = root->FirstChildElement("MainInitFunction");
	if(c && c->FindAttribute("Name"))
	{
		config.mainInitFunction = string(c->Attribute("Name"));
	}
	c = root->FirstChildElement("MainExecFunction");
	if(c && c->FindAttribute("Name"))
	{
		config.mainExecFunction = string(c->Attribute("Name"));
	}
	c = root->FirstChildElement("MainCompositeState");
	if(c && c->FindAttribute("Name"))
	{
		config.mainCompositeState = string(c->Attribute("Name"));
	}

    int res = parseConfigParameter(root);
    if(res < 0)
        return res;

	return 1;
}

int ILParser::parseConfigParameter(const tinyxml2::XMLElement* root)
{
    const tinyxml2::XMLElement *c = root->FirstChildElement("Parameter");
    while (c)
    {
        setCurrentReadXMLElement(c);
        ILConfigParameter parameter;
        if(c->FindAttribute("Name"))
            parameter.name = string(c->Attribute("Name"));
        else
            return -ErrorILConfigParameterHasNoAttributeName;
        if(c->FindAttribute("Type"))
            parameter.type = string(c->Attribute("Type"));
        else
            return -ErrorILConfigParameterHasNoAttributeType;

        parameter.isAddress = c->FindAttribute("IsAddress") ?
            parseUtilityIsAddress(string(c->Attribute("IsAddress"))) : parameter.isAddress;

        parameter.arraySize = c->FindAttribute("ArraySize") ?
            parseUtilityArraySize(string(c->Attribute("ArraySize"))) : parameter.arraySize;

        if(c->FindAttribute("DefaultValue"))
            parameter.defaultValue = string(c->Attribute("DefaultValue"));


        config.parameters.push_back(parameter);
        c = c->NextSiblingElement("Parameter");
    }
    return 0;
}

void ILParser::parseAllAttribute(ILObject* obj, const tinyxml2::XMLElement* root) const
{
    const tinyxml2::XMLAttribute* attr = root->FirstAttribute();
    while(attr)
    {
        string attrNameStr(attr->Name());
        string attrValueStr(attr->Value());
        obj->allAttributeMap[attrNameStr] = attrValueStr;
        attr = attr->Next();
    }
}

int ILParser::parseFile(const tinyxml2::XMLElement* root)
{
	const tinyxml2::XMLElement *c = root->FirstChildElement("File");
    
	while (c)
	{
		setCurrentReadXMLElement(c);
		ILFile* file = new ILFile(nullptr);
		if(c->FindAttribute("Name"))
			file->name = string(c->Attribute("Name"));
		else
			return releaseILObjectAndReturn(file, -ErrorILFileHasNoAttributeName);

	    file->isIndependent = c->FindAttribute("IsIndependent") ? c->BoolAttribute("IsIndependent") : file->isIndependent;
        
		file->isHeadFile = c->FindAttribute("IsHeadFile") ? c->BoolAttribute("IsHeadFile") : file->isHeadFile;

		int res;
		if((res = parseHeadFile(file, c)) < 0)
			return releaseILObjectAndReturn(file, res);

		if((res = parseMacro(file, c)) < 0)
			return releaseILObjectAndReturn(file, res);

		if((res = parseTypeDefine(file, c)) < 0)
			return releaseILObjectAndReturn(file, res);

		if((res = parseUtilityFunction(file, c)) < 0)
			return releaseILObjectAndReturn(file, res);

		if((res = parseStructDefine(file, c)) < 0)
			return releaseILObjectAndReturn(file, res);

		if((res = parseEnumDefine(file, c)) < 0)
			return releaseILObjectAndReturn(file, res);

		if((res = parseGlobalVariable(file, c)) < 0)
			return releaseILObjectAndReturn(file, res);

        if((res = parseExternVariable(file, c)) < 0)
			return releaseILObjectAndReturn(file, res);

		if((res = parseFunction(file, c)) < 0)
			return releaseILObjectAndReturn(file, res);

		files.push_back(file);
		//objects.push_back(file);
		
        parseAllAttribute(file, c);

		c = c->NextSiblingElement("File");
	}

	return 1;
}

int ILParser::parseExternFile(const tinyxml2::XMLElement* root)
{
    const tinyxml2::XMLElement *c = root->FirstChildElement("ExternFile");
    
	while (c)
	{
		setCurrentReadXMLElement(c);
		ILExternFile* externFile = new ILExternFile(nullptr);
		if(c->FindAttribute("Name"))
			externFile->name = string(c->Attribute("Name"));
		else
			return releaseILObjectAndReturn(externFile, -ErrorILExternFileHasNoAttributeName);
        
		externFiles.push_back(externFile);
		//objects.push_back(file);
		
        parseAllAttribute(externFile, c);

		c = c->NextSiblingElement("ExternFile");
	}

    return 1;
}


int ILParser::parseHeadFileIndependent(const tinyxml2::XMLElement* c, ILHeadFile* retILHeadFile)
{
    ILHeadFile* headFile = retILHeadFile;
    headFile->objType = ILObject::TypeHeadFile;
	if(c->FindAttribute("Name"))
		headFile->name = string(c->Attribute("Name"));
	else
		return releaseILObjectAndReturn(headFile, -ErrorILHeadFileHasNoAttributeName);

	int res = parseRef(headFile, c);
	if(res < 0)
		return releaseILObjectAndReturn(headFile, res);
    

    parseAllAttribute(headFile, c);

    return 1;
}

int ILParser::parseHeadFile(ILFile* file, const tinyxml2::XMLElement* root)
{
    int res;
	const tinyxml2::XMLElement *c = root->FirstChildElement("HeadFile");
	while (c)
	{
		setCurrentReadXMLElement(c);
		ILHeadFile* headFile = new ILHeadFile();
		headFile->parent = file;
		
		file->headFiles.push_back(headFile);
		//objects.push_back(headFile);

        res = parseHeadFileIndependent(c, headFile);
        if(res < 0)
            return res;

		c = c->NextSiblingElement("HeadFile");
	}
	return 1;
}

int ILParser::parseMacroIndependent(const tinyxml2::XMLElement* c, ILMacro* retILMacro)
{
    ILMacro* macro = retILMacro;

    macro->objType = ILObject::TypeMacro;
	if(c->FindAttribute("Name"))
		macro->name = string(c->Attribute("Name"));
	else
		return releaseILObjectAndReturn(macro, -ErrorILMacroHasNoAttributeName);

	macro->value = c->FindAttribute("Value") ? string(c->Attribute("Value")) : macro->value;
	macro->hasIfnodefCheck = c->FindAttribute("HasIfnodefCheck") ? string(c->Attribute("HasIfnodefCheck")) : macro->hasIfnodefCheck;
	// else
	// 	return releaseILObjectAndReturn(macro, -ErrorILMacroHasNoAttributeValue);

	int res = parseRef(macro, c);
	if(res < 0)
		return releaseILObjectAndReturn(macro, res);
	
	
    parseAllAttribute(macro, c);

    return 1;
}


int ILParser::parseMacro(ILFile* file, const tinyxml2::XMLElement* root)
{
    int res;
	const tinyxml2::XMLElement *c = root->FirstChildElement("Macro");
	while (c)
	{
		setCurrentReadXMLElement(c);
		ILMacro* macro = new ILMacro();
		macro->parent = file;
        
		file->macros.push_back(macro);
		//objects.push_back(macro);

		res = parseMacroIndependent(c, macro);
        if(res < 0)
            return res;

		c = c->NextSiblingElement("Macro");
	}
	return 1;
}


int ILParser::parseTypeDefineIndependent(const tinyxml2::XMLElement* c, ILTypeDefine* retILTypeDefine)
{
    ILTypeDefine* typeDefine = retILTypeDefine;

    typeDefine->objType = ILObject::TypeTypeDefine;
	if(c->FindAttribute("Name"))
		typeDefine->name = string(c->Attribute("Name"));
	else
		return releaseILObjectAndReturn(typeDefine, -ErrorILTypeDefineHasNoAttributeName);
	if(c->FindAttribute("Value"))
		typeDefine->value = string(c->Attribute("Value"));
	else
		return releaseILObjectAndReturn(typeDefine, -ErrorILTypeDefineHasNoAttributeValue);

	int res = parseRef(typeDefine, c);

	if(res < 0)
		return releaseILObjectAndReturn(typeDefine, res);
    
    parseAllAttribute(typeDefine, c);
    return 1;
}

int ILParser::parseTypeDefine(ILFile* file, const tinyxml2::XMLElement* root)
{
    int res;
	const tinyxml2::XMLElement *c = root->FirstChildElement("TypeDefine");
	while (c)
	{
		setCurrentReadXMLElement(c);
		ILTypeDefine* typeDefine = new ILTypeDefine();
		typeDefine->parent = file;
        file->typeDefines.push_back(typeDefine);
		//objects.push_back(typeDefine);

        res = parseTypeDefineIndependent(c, typeDefine);
        if(res < 0)
            return res;
		
		this->typeDefineTypeList.push_back(typeDefine);

        if (std::find(ILParser::allDataTypeList.begin(), ILParser::allDataTypeList.end(),
			typeDefine->value) != ILParser::allDataTypeList.end())
			return -ErrorTypeRedefined;
		ILParser::allDataTypeList.push_back(typeDefine->value);
        

		c = c->NextSiblingElement("TypeDefine");
	}
	return 1;
}



int ILParser::parseUtilityFunctionIndependent(const tinyxml2::XMLElement* c, ILUtilityFunction* retILUtilityFunction)
{
    ILUtilityFunction* utilityFunction = retILUtilityFunction;

    utilityFunction->objType = ILObject::TypeUtilityFunction;
	if(c->FindAttribute("Name"))
		utilityFunction->name = string(c->Attribute("Name"));
	else
		return releaseILObjectAndReturn(utilityFunction, -ErrorILUtilityFunctionHasNoAttributeName);
	

	int res = parseRef(utilityFunction, c);
	if(res < 0)
		return releaseILObjectAndReturn(utilityFunction, res);
	
    parseAllAttribute(utilityFunction, c);

    return 1;
}

int ILParser::parseUtilityFunction(ILFile* file, const tinyxml2::XMLElement* root)
{
    int res;
	const tinyxml2::XMLElement *c = root->FirstChildElement("UtilityFunction");
	while (c)
	{
		setCurrentReadXMLElement(c);
		ILUtilityFunction* utilityFunction = new ILUtilityFunction();
		utilityFunction->parent = file;
        file->utilityFunctions.push_back(utilityFunction);
		//objects.push_back(utilityFunction);

        res = parseUtilityFunctionIndependent(c, utilityFunction);
        if(res < 0)
            return res;

		c = c->NextSiblingElement("UtilityFunction");
	}
	return 1;
}



int ILParser::parseStructDefineIndependent(const tinyxml2::XMLElement* c, ILStructDefine* retILStructDefine)
{
    ILStructDefine* structDefine = retILStructDefine;
    
	structDefine->objType = ILObject::TypeStructDefine;
	if(c->FindAttribute("Name"))
		structDefine->name = string(c->Attribute("Name"));
	else
		return releaseILObjectAndReturn(structDefine, -ErrorILStructDefineHasNoAttributeName);

	int res = parseRef(structDefine, c);
	if(res < 0)
		return releaseILObjectAndReturn(structDefine, res);

	res = parseMember(structDefine, c);
	if(res < 0)
		return releaseILObjectAndReturn(structDefine, res);

    parseAllAttribute(structDefine, c);

	return 1;
}

int ILParser::parseStructDefine(ILFile* file, const tinyxml2::XMLElement* root)
{
    int res;
	const tinyxml2::XMLElement *c = root->FirstChildElement("Struct");
	while (c)
	{
		setCurrentReadXMLElement(c);
		ILStructDefine* structDefine = new ILStructDefine();
		structDefine->parent = file;

		file->structDefines.push_back(structDefine);
		//objects.push_back(structDefine);

        res = parseStructDefineIndependent(c, structDefine);
        if(res < 0)
            return res;

		this->structTypeList.push_back(structDefine);

	    if (std::find(ILParser::allDataTypeList.begin(), ILParser::allDataTypeList.end(),
			"struct " + structDefine->name) != ILParser::allDataTypeList.end())
			return -ErrorTypeRedefined;
		ILParser::allDataTypeList.push_back("struct " + structDefine->name);
        
        
		c = c->NextSiblingElement("Struct");
	}
	return 1;
}


int ILParser::parseEnumDefineIndependent(const tinyxml2::XMLElement* c, ILEnumDefine* retILEnumDefine)
{
    ILEnumDefine* enumDefine = retILEnumDefine;
    
	enumDefine->objType = ILObject::TypeEnumDefine;
	if(c->FindAttribute("Name"))
		enumDefine->name = string(c->Attribute("Name"));
	else
		return releaseILObjectAndReturn(enumDefine, -ErrorILEnumDefineHasNoAttributeName);

	int res = parseRef(enumDefine, c);
	if(res < 0)
		return releaseILObjectAndReturn(enumDefine, res);

	res = parseEnumValue(enumDefine, c);
	if(res < 0)
		return releaseILObjectAndReturn(enumDefine, res);

    parseAllAttribute(enumDefine, c);

    return 1;
}

int ILParser::parseEnumDefine(ILFile* file, const tinyxml2::XMLElement* root)
{
    int res;
	const tinyxml2::XMLElement *c = root->FirstChildElement("Enum");
	while (c)
	{
		setCurrentReadXMLElement(c);
		ILEnumDefine* enumDefine = new ILEnumDefine();
		enumDefine->parent = file;

		file->enumDefines.push_back(enumDefine);
		//objects.push_back(structDefine);
		
		this->enumTypeList.push_back(enumDefine);

        res = parseEnumDefineIndependent(c, enumDefine);
        if(res < 0)
            return res;

        if (std::find(ILParser::allDataTypeList.begin(), ILParser::allDataTypeList.end(),
			"enum " + enumDefine->name) != ILParser::allDataTypeList.end())
			return -ErrorTypeRedefined;
		ILParser::allDataTypeList.push_back("enum " + enumDefine->name);

        //????enum?????????? ????typedef??????????????????????????????????????????enum????????
        if (std::find(ILParser::allDataTypeList.begin(), ILParser::allDataTypeList.end(),
			enumDefine->name) != ILParser::allDataTypeList.end())
			return -ErrorTypeRedefined;
	    ILParser::allDataTypeList.push_back(enumDefine->name);
        
        
        
		c = c->NextSiblingElement("Enum");
	}
	return 1;
}


int ILParser::parseGlobalVariableIndependent(const tinyxml2::XMLElement* c, ILGlobalVariable* retILGlobalVariable)
{
    int res;
    ILGlobalVariable* globalVariable = retILGlobalVariable;

	globalVariable->objType = ILObject::TypeGlobalVariable;
	if(c->FindAttribute("Name"))
		globalVariable->name = string(c->Attribute("Name"));
	else
		return releaseILObjectAndReturn(globalVariable, -ErrorILGlobalVariableHasNoAttributeName);
	if(c->FindAttribute("DefaultValue")) {
		string defaultValueStr = string(c->Attribute("DefaultValue"));
        ILExpressionParser expressionParser;
		if((res = parseExpressionDefaultValue(defaultValueStr, &globalVariable->defaultValue)) < 0)
			return releaseILObjectAndReturn(globalVariable, res);
	}
	//else
	//	return -ErrorILGlobalVariableHasNoAttributeDefaultValue;

	globalVariable->isAddress = c->FindAttribute("IsAddress") ?
        parseUtilityIsAddress(string(c->Attribute("IsAddress"))) : globalVariable->isAddress;
    
	globalVariable->isStatic = c->FindAttribute("IsStatic") ? c->BoolAttribute("IsStatic") : globalVariable->isStatic;
	
	globalVariable->align = c->FindAttribute("Align") ? c->IntAttribute("Align") : globalVariable->align;
	
	globalVariable->isVolatile = c->FindAttribute("IsVolatile") ? c->IntAttribute("IsVolatile") : globalVariable->isVolatile;

    globalVariable->isConst = c->FindAttribute("IsConst") ? c->IntAttribute("IsConst") : globalVariable->isConst;
	
		//	return -ErrorILGlobalVariableHasNoAttributeIsAddress;
	if(c->FindAttribute("Type"))
		globalVariable->type = string(c->Attribute("Type"));
	else
		return releaseILObjectAndReturn(globalVariable, -ErrorILGlobalVariableHasNoAttributeType);

	globalVariable->arraySize = c->FindAttribute("ArraySize") ?
        parseUtilityArraySize(string(c->Attribute("ArraySize"))) : globalVariable->arraySize;

	//else
	//	return -ErrorILGlobalVariableHasNoAttributeArraySize;
    
	if((res = parseRef(globalVariable, c)) < 0)
		return releaseILObjectAndReturn(globalVariable, res);
    else if((res = ILParser::addVariableName(globalVariable->name, globalVariable->getDomain())) < 0)
		return releaseILObjectAndReturn(globalVariable, res);

    
    parseAllAttribute(globalVariable, c);

    return 1;
}

int ILParser::parseExternVariableIndependent(const tinyxml2::XMLElement* c, ILExternVariable* retILExternVariable)
{
    int res;
    ILExternVariable* externVariable = retILExternVariable;

	externVariable->objType = ILObject::TypeExternVariable;
	if(c->FindAttribute("Name"))
		externVariable->name = string(c->Attribute("Name"));
	else
		return releaseILObjectAndReturn(externVariable, -ErrorILExternVariableHasNoAttributeName);
	
	externVariable->isAddress = c->FindAttribute("IsAddress") ?
        parseUtilityIsAddress(string(c->Attribute("IsAddress"))) : externVariable->isAddress;
    
	externVariable->align = c->FindAttribute("Align") ? c->IntAttribute("Align") : externVariable->align;
	
	externVariable->isVolatile = c->FindAttribute("IsVolatile") ? c->IntAttribute("IsVolatile") : externVariable->isVolatile;

    externVariable->isConst = c->FindAttribute("IsConst") ? c->IntAttribute("IsConst") : externVariable->isConst;
	
		//	return -ErrorILGlobalVariableHasNoAttributeIsAddress;
	if(c->FindAttribute("Type"))
		externVariable->type = string(c->Attribute("Type"));
	else
		return releaseILObjectAndReturn(externVariable, -ErrorILGlobalVariableHasNoAttributeType);

	externVariable->arraySize = c->FindAttribute("ArraySize") ?
        parseUtilityArraySize(string(c->Attribute("ArraySize"))) : externVariable->arraySize;

	//else
	//	return -ErrorILGlobalVariableHasNoAttributeArraySize;
    
	if((res = parseRef(externVariable, c)) < 0)
		return releaseILObjectAndReturn(externVariable, res);
    else if((res = ILParser::addVariableName(externVariable->name, externVariable->getDomain())) < 0)
		return releaseILObjectAndReturn(externVariable, res);

    
    parseAllAttribute(externVariable, c);

    return 1;
}

int ILParser::parseGlobalVariable(ILFile* file, const tinyxml2::XMLElement* root)
{
    int res;
	const tinyxml2::XMLElement *c = root->FirstChildElement("GlobalVariable");
	while (c)
	{
		setCurrentReadXMLElement(c);
		ILGlobalVariable* globalVariable = new ILGlobalVariable();
		globalVariable->parent = file;


		file->globalVariables.push_back(globalVariable);
		//objects.push_back(globalVariable);

        res = parseGlobalVariableIndependent(c, globalVariable);
        if(res < 0)
            return res;

		c = c->NextSiblingElement("GlobalVariable");
	}
	return 1;
}

int ILParser::parseExternVariable(ILFile* file, const tinyxml2::XMLElement* root)
{
    int res;
	const tinyxml2::XMLElement *c = root->FirstChildElement("ExternVariable");
	while (c)
	{
		setCurrentReadXMLElement(c);
		ILExternVariable* externVariable = new ILExternVariable();
		externVariable->parent = file;


		file->externVariables.push_back(externVariable);
		//objects.push_back(externVariable);

        res = parseExternVariableIndependent(c, externVariable);
        if(res < 0)
            return res;

		c = c->NextSiblingElement("ExternVariable");
	}
	return 1;
}

int ILParser::parseFunction(ILFile* file, const tinyxml2::XMLElement* root)
{
	const tinyxml2::XMLElement *c = root->FirstChildElement("Function");
	while (c)
	{
		setCurrentReadXMLElement(c);
		ILFunction* function = new ILFunction();
		function->parent = file;
		function->objType = ILObject::TypeFunction;
		function->schedule = nullptr;

		if(c->FindAttribute("Name"))
			function->name = string(c->Attribute("Name"));
		else
			return releaseILObjectAndReturn(function, -ErrorILFunctionHasNoAttributeName);
		if(c->FindAttribute("Type"))
		{
			string typeStr = string(c->Attribute("Type"));
            if(ILFunction::functionTypeStrToTypeMap.find(typeStr) != ILFunction::functionTypeStrToTypeMap.end())
            {
                function->type = ILFunction::functionTypeStrToTypeMap[typeStr];
            }
		    //else{
			//	return releaseILObjectAndReturn(function, -ErrorILFunctionInvalidType);
            //}
		}
		// else{
		// 	return releaseILObjectAndReturn(function, -ErrorILFunctionHasNoAttributeType);
		// }

        function->returnType = c->FindAttribute("ReturnType") ? string(c->Attribute("ReturnType")) : function->returnType;

	    function->isStatic = c->FindAttribute("IsStatic") ? c->BoolAttribute("IsStatic") : function->isStatic;
         
		int res = parseRef(function, c);
		if(res < 0)
			return releaseILObjectAndReturn(function, res);
		res = parseInput(function, c);
		if(res < 0)
			return releaseILObjectAndReturn(function, res);
		res = parseOutput(function, c);
		if(res < 0)
			return releaseILObjectAndReturn(function, res);
		res = parseSchedule(function, c);
		if(res < 0)
			return releaseILObjectAndReturn(function, res);

		file->functions.push_back(function);
		//objects.push_back(function);

        this->allFunctions.push_back(function);

        parseAllAttribute(function, c);

		c = c->NextSiblingElement("Function");
	}
	return 1;
}

//int ILParser::parseBranch(ILObject* parent, const tinyxml2::XMLElement* root)
//{
//	const tinyxml2::XMLElement *c = root->FirstChildElement("Branch");
//	while (c)
//	{
//		ILBranch* branch = new ILBranch();
//		branch->objType = ILObject::TypeBranch;
//
//		if(c->FindAttribute("Type"))
//		{
//			string typeStr = string(c->Attribute("Type"));
//			if(typeStr == "If")
//				branch->type = ILBranch::TypeIf;
//			else if(typeStr == "Else")
//				branch->type = ILBranch::TypeElse;
//			else if(typeStr == "ElseIf")
//				branch->type = ILBranch::TypeElseIf;
//			else if(typeStr == "For")
//				branch->type = ILBranch::TypeFor;
//			else if(typeStr == "While")
//				branch->type = ILBranch::TypeWhile;
//			else
//				return -ErrorILBranchInvalidType;
//		}
//		else
//			return -ErrorILBranchHasNoAttributeType;
//		
//		int res;
//		res = parseRef(branch, c);
//		if(res < 0)
//			return res;
//		res = parseScheduleNode(branch, c);
//		if(res < 0)
//			return res;
//		
//		if(parent->objType == ILObject::TypeBranch)
//			((ILBranch*)parent)->scheduleNodes.push_back(branch);
//		else if(parent->objType == ILObject::TypeSchedule)
//			((ILSchedule*)parent)->scheduleNodes.push_back(branch);
//		else
//			return -ErrorILBranchInvalidParentNode;
//
//		objects.push_back(branch);
//	}
//	return 1;
//}

//int ILParser::parseCalculate(ILObject* parent, const tinyxml2::XMLElement* root)
//{
//	return 0;
//}

//int ILParser::parseLocalVariable(ILSchedule* schedule, const tinyxml2::XMLElement* root)
//{
//	const tinyxml2::XMLElement *c = root->FirstChildElement("LocalVariable");
//	while (c)
//	{
//		ILLocalVariable* localVariable = new ILLocalVariable();
//		localVariable->objType = ILObject::TypeLocalVariable;
//		if(c->FindAttribute("Name"))
//			localVariable->name = string(c->Attribute("Name"));
//		else
//			return -ErrorILGlobalVariableHasNoAttributeName;
//		if(c->FindAttribute("DefaultValue"))
//			localVariable->defaultValue = string(c->Attribute("DefaultValue"));
//		else
//			return -ErrorILGlobalVariableHasNoAttributeDefaultValue;
//		if(c->FindAttribute("IsAddress"))
//			localVariable->isAddress = c->BoolAttribute("IsAddress");
//		else
//			return -ErrorILGlobalVariableHasNoAttributeIsAddress;
//		if(c->FindAttribute("Type"))
//			localVariable->dataType = string(c->Attribute("Type"));
//		else
//			return -ErrorILGlobalVariableHasNoAttributeType;
//		if(c->FindAttribute("ArraySize"))
//		{
//			localVariable->arraySize = parseUtilityArraySize(string(c->Attribute("ArraySize")));
//		}
//		else
//			return -ErrorILGlobalVariableHasNoAttributeArraySize;
//
//		int res;
//		res = parseRef(localVariable, c);
//		if(res < 0)
//			return res;
//
//		schedule->localVariables.push_back(localVariable);
//		objects.push_back(localVariable);
//	}
//	return 1;
//}

int ILParser::parseInput(ILObject* parent, const tinyxml2::XMLElement* root)
{
    int res;
	const tinyxml2::XMLElement *c = root->FirstChildElement("Input");
	while (c)
	{
		setCurrentReadXMLElement(c);
		ILInput* input = new ILInput();
		input->parent = parent;
		input->objType = ILObject::TypeInput;
		if(c->FindAttribute("Name"))
			input->name = string(c->Attribute("Name"));
		else
			return releaseILObjectAndReturn(input, -ErrorILInputHasNoAttributeName);
		if(c->FindAttribute("DefaultValue"))
	    {
	        string defaultValueStr = string(c->Attribute("DefaultValue"));
		    if((res = parseExpressionDefaultValue(defaultValueStr, &input->defaultValue)) < 0)
			    return releaseILObjectAndReturn(input, res);
	    }
		//else
		//	return -ErrorILInputHasNoAttributeDefaultValue;

		input->isAddress = c->FindAttribute("IsAddress") ?
            parseUtilityIsAddress(string(c->Attribute("IsAddress"))) : input->isAddress;
		
		//	return -ErrorILInputHasNoAttributeIsAddress;
		if(c->FindAttribute("Type"))
			input->type = string(c->Attribute("Type"));
		else
			return releaseILObjectAndReturn(input, -ErrorILInputHasNoAttributeType);

        input->type = stringStartsWith(input->type, "struct ") ?
            input->type.substr(CONST_NUM_7, input->type.length() - CONST_NUM_7) : input->type;
        
        input->arraySize = c->FindAttribute("ArraySize") ?
            parseUtilityArraySize(string(c->Attribute("ArraySize"))) : input->arraySize;

        input->isConst = c->FindAttribute("IsConst") ? c->BoolAttribute("IsConst") : input->isConst;
		
		//else
		//	return -ErrorILInputHasNoAttributeArraySize;
        
        input->sourceStr = c->FindAttribute("Source") ? string(c->Attribute("Source")) : input->sourceStr;
        
		if((res = parseRef(input, c)) < 0)
			return releaseILObjectAndReturn(input, res);

		if(parent->objType == ILObject::TypeFunction) {
            if((res = ILParser::addVariableName(input->name, input->getDomain())) < 0)
			    return releaseILObjectAndReturn(input, res);
		    static_cast<ILFunction*>(parent)->inputs.push_back(input);
		}
		else if(parent->objType == ILObject::TypeCalculate)
			static_cast<ILCalculate*>(parent)->inputs.push_back(input);
		else
			return releaseILObjectAndReturn(input, -ErrorILInputInvalidParentNode);
		//objects.push_back(input);
		
        parseAllAttribute(input, c);

		c = c->NextSiblingElement("Input");
	}
	return 1;
}

int ILParser::parseOutput(ILObject* parent, const tinyxml2::XMLElement* root)
{
	const tinyxml2::XMLElement *c = root->FirstChildElement("Output");
	while (c) {
		setCurrentReadXMLElement(c);
		ILOutput* output = new ILOutput(parent);
		if(c->FindAttribute("Name"))
			output->name = string(c->Attribute("Name"));
		else {
		    return releaseILObjectAndReturn(output, -ErrorILOutputHasNoAttributeName);
		}
		if(c->FindAttribute("DefaultValue")) {
	        string defaultValueStr = string(c->Attribute("DefaultValue"));
		    int res = parseExpressionDefaultValue(defaultValueStr, &output->defaultValue);
		    if(res < 0)
			    return releaseILObjectAndReturn(output, res);
	    }

	    output->isAddress = c->FindAttribute("IsAddress") ?
            parseUtilityIsAddress(string(c->Attribute("IsAddress"))) : output->isAddress;
		
		if(c->FindAttribute("Type")) {
		    output->type = string(c->Attribute("Type"));
		} else {
		    return releaseILObjectAndReturn(output, -ErrorILOutputHasNoAttributeType);
		}

	    output->arraySize = c->FindAttribute("ArraySize") ?
            parseUtilityArraySize(string(c->Attribute("ArraySize"))) : output->arraySize;
		
	    output->sourceStr = c->FindAttribute("Source") ? string(c->Attribute("Source")) : output->sourceStr;

        output->isConst = c->FindAttribute("IsConst") ? c->BoolAttribute("IsConst") : output->isConst;


		int res = parseRef(output, c);
		if(res < 0) {
		    return releaseILObjectAndReturn(output, res);
		}
		if(parent->objType == ILObject::TypeFunction) {
            if((res = ILParser::addVariableName(output->name, output->getDomain())) < 0)
			    return releaseILObjectAndReturn(output, res);
		    static_cast<ILFunction*>(parent)->outputs.push_back(output);
		}
		else if(parent->objType == ILObject::TypeCalculate)
			static_cast<ILCalculate*>(parent)->outputs.push_back(output);
		else {
		    return releaseILObjectAndReturn(output, -ErrorILOutputInvalidParentNode);
		}
        
        parseAllAttribute(output, c);

		c = c->NextSiblingElement("Output");
	}
	return 1;
}

int ILParser::parseRef(ILObject* parent, const tinyxml2::XMLElement* root)
{
	const tinyxml2::XMLElement *c = root->FirstChildElement("Ref");
	while (c)
	{
		setCurrentReadXMLElement(c);
		ILRef* ref = new ILRef(parent);

		ref->actor = c->FindAttribute("Actor") ? string(c->Attribute("Actor")) : ref->actor;
		//else
		//	return -ErrorILRefHasNoAttributeActor;
		if(c->FindAttribute("Path"))
			ref->path = string(c->Attribute("Path"));
		else
			return releaseILObjectAndReturn(ref, -ErrorILRefHasNoAttributePath);
		
		if(parent->objType == ILObject::TypeFunction)
			static_cast<ILFunction*>(parent)->refs.push_back(ref);
		else if(parent->objType == ILObject::TypeCalculate)
			static_cast<ILCalculate*>(parent)->refs.push_back(ref);
		else if(parent->objType == ILObject::TypeHeadFile)
			static_cast<ILHeadFile*>(parent)->refs.push_back(ref);
		else if(parent->objType == ILObject::TypeMacro)
			static_cast<ILMacro*>(parent)->refs.push_back(ref);
		else if(parent->objType == ILObject::TypeTypeDefine)
			static_cast<ILTypeDefine*>(parent)->refs.push_back(ref);
		else if(parent->objType == ILObject::TypeUtilityFunction)
			static_cast<ILUtilityFunction*>(parent)->refs.push_back(ref);
		else if(parent->objType == ILObject::TypeStructDefine)
			static_cast<ILStructDefine*>(parent)->refs.push_back(ref);
		else if(parent->objType == ILObject::TypeGlobalVariable)
			static_cast<ILGlobalVariable*>(parent)->refs.push_back(ref);
		else if(parent->objType == ILObject::TypeBranch)
			static_cast<ILBranch*>(parent)->refs.push_back(ref);
		else if(parent->objType == ILObject::TypeLocalVariable)
			static_cast<ILLocalVariable*>(parent)->refs.push_back(ref);
		else if(parent->objType == ILObject::TypeInput)
			static_cast<ILInput*>(parent)->refs.push_back(ref);
		else if(parent->objType == ILObject::TypeOutput)
			static_cast<ILOutput*>(parent)->refs.push_back(ref);
		else if(parent->objType == ILObject::TypeEnumDefine)
			static_cast<ILEnumDefine*>(parent)->refs.push_back(ref);
		else if(parent->objType == ILObject::TypeMember)
			static_cast<ILMember*>(parent)->refs.push_back(ref);
        else if(parent->objType == ILObject::TypeExternVariable)
			static_cast<ILExternVariable*>(parent)->refs.push_back(ref);
		else
			return releaseILObjectAndReturn(ref, -ErrorILRefInvalidParentNode);
		//objects.push_back(ref);
        
        parseAllAttribute(ref, c);

		c = c->NextSiblingElement("Ref");
	}
	return 1;
}

//int ILParser::parseRange(ILObject* parent, const tinyxml2::XMLElement* root)
//{
//	const tinyxml2::XMLElement* c = root->FirstChildElement("Range");
//	while (c)
//	{
//		setCurrentReadXMLElement(c);
//		ILRange* range = new ILRange(parent);
//		range->isOptimized = c->FindAttribute("IsOptimized") ?
//			parseUtilityIsOptimized(string(c->Attribute("IsOptimized"))) : range->isOptimized;
//		if (c->FindAttribute("Start"))
//			range->start = stringToInt(string(c->Attribute("Start")));
//		if (c->FindAttribute("Length"))
//			range->length = stringToInt(string(c->Attribute("Length")));
//
//		if (parent->objType == ILObject::TypeInput)
//			static_cast<ILInput*>(parent)->ranges.push_back(range);
//		else if (parent->objType == ILObject::TypeOutput)
//			static_cast<ILOutput*>(parent)->ranges.push_back(range);
//		else
//			return releaseILObjectAndReturn(range, -ErrorILRangeInvalidParentNode);
//		//objects.push_back(ref);
//
//		parseAllAttribute(range, c);
//
//		c = c->NextSiblingElement("Range");
//	}
//	return 1;
//}

int ILParser::parseSchedule(ILFunction* function, const tinyxml2::XMLElement* root)
{
	const tinyxml2::XMLElement *c = root->FirstChildElement("Schedule");
	if(!c)
		return -ErrorILFunctionMissScheduleNode;
	while (c)
	{
		setCurrentReadXMLElement(c);
		ILSchedule* schedule = new ILSchedule();
		schedule->parent = function;
		schedule->objType = ILObject::TypeSchedule;
		
		int res = parseScheduleNode(schedule, c);
		if(res < 0)
			return res;
		
		if(!function->schedule)
			function->schedule = schedule;
		else
			return releaseILObjectAndReturn(schedule, -ErrorILFunctionHasMultiSchedule);
		//objects.push_back(schedule);
		
        parseAllAttribute(schedule, c);

		c = c->NextSiblingElement("Schedule");
	}
	return 1;
}

int ILParser::parseMember(ILStructDefine* structDefine, const tinyxml2::XMLElement* root)
{
	const tinyxml2::XMLElement *c = root->FirstChildElement("Member");
	while (c)
	{
		setCurrentReadXMLElement(c);
		ILMember* member = new ILMember();
		member->parent = structDefine;
		member->objType = ILObject::TypeMember;
		if(c->FindAttribute("Name"))
			member->name = string(c->Attribute("Name"));
		else
			return releaseILObjectAndReturn(member, -ErrorILMemberHasNoAttributeName);

	    member->isAddress = c->FindAttribute("IsAddress") ?
            parseUtilityIsAddress(string(c->Attribute("IsAddress"))) : member->isAddress;
		
			//return -ErrorILMemberHasNoAttributeIsAddress;
		if(c->FindAttribute("Type"))
			member->type = string(c->Attribute("Type"));
		else
			return releaseILObjectAndReturn(member, -ErrorILMemberHasNoAttributeType);

	    member->arraySize = c->FindAttribute("ArraySize") ?
            parseUtilityArraySize(string(c->Attribute("ArraySize"))) : member->arraySize;
		
		//else
		//	return -ErrorILMemberHasNoAttributeArraySize;
        
	    member->align = c->FindAttribute("Align") ? c->IntAttribute("Align") : member->align;
        
		member->isVolatile = c->FindAttribute("IsVolatile") ? c->IntAttribute("IsVolatile") : member->isVolatile;
		
        member->isConst = c->FindAttribute("IsConst") ? c->BoolAttribute("IsConst") : member->isConst;

		structDefine->members.push_back(member);
		//objects.push_back(member);

        int res = parseRef(member, c);
		if(res < 0)
			return releaseILObjectAndReturn(member, res);

        parseAllAttribute(member, c);

		c = c->NextSiblingElement("Member");
	}
	return 1;
}

int ILParser::parseEnumValue(ILEnumDefine* enumDefine, const tinyxml2::XMLElement* root)
{
	const tinyxml2::XMLElement *c = root->FirstChildElement("EnumValue");
	while (c)
	{
		setCurrentReadXMLElement(c);
		ILEnumValue* enumValue = new ILEnumValue();
		enumValue->parent = enumDefine;
		enumValue->objType = ILObject::TypeEnumValue;
		if(c->FindAttribute("Name"))
			enumValue->name = string(c->Attribute("Name"));
		else
			return releaseILObjectAndReturn(enumValue, -ErrorILEnumValueHasNoAttributeName);

	    enumValue->value = c->FindAttribute("Value") ? string(c->Attribute("Value")) : enumValue->value;
        

		enumDefine->enumValues.push_back(enumValue);
		
        parseAllAttribute(enumValue, c);

		c = c->NextSiblingElement("EnumValue");
	}
	return 1;
}

int ILParser::parseScheduleNode(ILObject* parent, const tinyxml2::XMLElement* root)
{
	const tinyxml2::XMLElement *c = root->FirstChildElement();
	while (c)
	{
		setCurrentReadXMLElement(c);
		string nameStr = string((c->Name() == nullptr) ? "": c->Name());
		if(nameStr == "LocalVariable")
		{
            int res = parseLocalVariable(c, parent, root);
            if(res < 0)
                return res;
		}
		else if(nameStr == "Calculate")
		{
            int res = parseCalculate(c, parent, root);
            if(res < 0)
                return res;
		}
		else if(nameStr == "Branch")
		{
            int res = parseBranch(c, parent, root);
            if(res < 0)
                return res;
		}
		
		c = c->NextSiblingElement();
	}
	return 1;
}



int ILParser::parseLocalVariableIndependent(const tinyxml2::XMLElement* c, ILLocalVariable* retILLocalVariable)
{
    int res;
	ILLocalVariable* localVariable = retILLocalVariable;
	localVariable->objType = ILObject::TypeLocalVariable;
	if(c->FindAttribute("Name"))
		localVariable->name = string(c->Attribute("Name"));
	else
		return releaseILObjectAndReturn(localVariable, -ErrorILLocalVariableHasNoAttributeName);
	if(c->FindAttribute("DefaultValue"))
	{
	    string defaultValueStr = string(c->Attribute("DefaultValue"));
		if((res = parseExpressionDefaultValue(defaultValueStr, &localVariable->defaultValue)) < 0)
			return releaseILObjectAndReturn(localVariable, res);
	}
	//else
	//	return -ErrorILLocalVariableHasNoAttributeDefaultValue;
	localVariable->isAddress = c->FindAttribute("IsAddress") ?
        parseUtilityIsAddress(string(c->Attribute("IsAddress"))) : localVariable->isAddress;
	
	//	return -ErrorILLocalVariableHasNoAttributeIsAddress;
	if(c->FindAttribute("Type"))
		localVariable->type = string(c->Attribute("Type"));
	else
		return releaseILObjectAndReturn(localVariable, -ErrorILLocalVariableHasNoAttributeType);
    
	localVariable->arraySize = c->FindAttribute("ArraySize") ?
        parseUtilityArraySize(string(c->Attribute("ArraySize"))) : localVariable->arraySize;
	
	localVariable->align = c->FindAttribute("Align") ? c->IntAttribute("Align") : localVariable->align;
	
	localVariable->isVolatile = c->FindAttribute("IsVolatile") ? c->IntAttribute("IsVolatile") : localVariable->isVolatile;
	
    localVariable->isConst = c->FindAttribute("IsConst") ? c->BoolAttribute("IsConst") : localVariable->isConst;
	//else
	//	return -ErrorILLocalVariableHasNoAttributeArraySize;
    
	if((res = parseRef(localVariable, c)) < 0)
		return releaseILObjectAndReturn(localVariable, res);
    else if((res = ILParser::addVariableName(localVariable->name, localVariable->getDomain())) < 0)
		return releaseILObjectAndReturn(localVariable, res);

    parseAllAttribute(localVariable, c);

    return 1;
}

int ILParser::parseLocalVariable(const tinyxml2::XMLElement* c, ILObject* parent, const tinyxml2::XMLElement* root)
{
    int res;
	ILLocalVariable* localVariable = new ILLocalVariable();
	localVariable->parent = parent;
    static_cast<ILSchedule*>(parent)->localVariables.push_back(localVariable);
	//objects.push_back(localVariable);

    res = parseLocalVariableIndependent(c, localVariable);
    if(res < 0)
        return res;

    return 1;
}


int ILParser::parseCalculateIndependent(const tinyxml2::XMLElement* c, ILCalculate* retILCalculate)
{
    ILCalculate* calculate = retILCalculate;
    calculate->objType = ILObject::TypeCalculate;
	calculate->statements.type = static_cast<Statement::Type>(-1);
	calculate->statements.depth = -1;
	calculate->statements.parentStatement = nullptr;

	if(c->FindAttribute("Name"))
		calculate->name = string(c->Attribute("Name"));
	else
		return releaseILObjectAndReturn(calculate, -ErrorILCalculateHasNoAttributeName);

	calculate->isOptimized = c->FindAttribute("IsOptimized") ? c->BoolAttribute("IsOptimized") : calculate->isOptimized;
	
	int res = parseRef(calculate, c);
	if(res < 0)
		return releaseILObjectAndReturn(calculate, res);
	res = parseInput(calculate, c);
	if(res < 0)
		return releaseILObjectAndReturn(calculate, res);
	res = parseOutput(calculate, c);
	if(res < 0)
		return releaseILObjectAndReturn(calculate, res);

    calculate->statements.parentILObject = calculate;
    res = parseStatement(&calculate->statements, c);
	if(res < 0)
		return releaseILObjectAndReturn(calculate, res);
    
	//objects.push_back(calculate);
    parseAllAttribute(calculate, c);

    return 1;
}

ILFunction* ILParser::findILFunction(std::string name) const
{
	for(int i = 0; i < this->files.size(); i++)
	{
		ILFile* file = this->files[i];
		for(int j = 0;j < file->functions.size(); j++)
		{
			if(file->functions[j]->name == name)
				return file->functions[j];
		}
	}
	return nullptr;
}


int ILParser::parseCalculate(const tinyxml2::XMLElement* c, ILObject* parent, const tinyxml2::XMLElement* root)
{
    int res;
	ILCalculate* calculate = new ILCalculate();
	calculate->parent = parent;

    if(parent->objType == ILObject::TypeBranch)
		static_cast<ILBranch*>(parent)->scheduleNodes.push_back(calculate);
	else if(parent->objType == ILObject::TypeSchedule)
		static_cast<ILSchedule*>(parent)->scheduleNodes.push_back(calculate);
	else
		return releaseILObjectAndReturn(calculate, -ErrorILCalculateInvalidParentNode);
	//objects.push_back(calculate);

    res = parseCalculateIndependent(c, calculate);
    if(res < 0)
        return res;

    return 1;
}

int ILParser::parseBranch(const tinyxml2::XMLElement* c, ILObject* parent, const tinyxml2::XMLElement* root)
{
	ILBranch* branch = new ILBranch();
	branch->parent = parent;
	branch->objType = ILObject::TypeBranch;

	if(c->FindAttribute("Type"))
	{
		string typeStr = string(c->Attribute("Type"));
		if(typeStr == "If")
			branch->type = ILBranch::TypeIf;
		else if(typeStr == "Else")
			branch->type = ILBranch::TypeElse;
		else if(typeStr == "ElseIf")
			branch->type = ILBranch::TypeElseIf;
		else if(typeStr == "For")
			branch->type = ILBranch::TypeFor;
		else if(typeStr == "While")
			branch->type = ILBranch::TypeWhile;
		else
			return releaseILObjectAndReturn(branch, -ErrorILBranchInvalidType);
	}
	else
		return releaseILObjectAndReturn(branch, -ErrorILBranchHasNoAttributeType);

	if(c->FindAttribute("Condition") && branch->type != ILBranch::TypeElse)
	{
		string conditionStr = string(c->Attribute("Condition"));
		ILExpressionParser expressionParser;
        
		int res = expressionParser.parseExpression(conditionStr, &branch->condition);
		if(res < 0)
			return releaseILObjectAndReturn(branch, res);
	}

	int res = parseRef(branch, c);
	if(res < 0)
		return releaseILObjectAndReturn(branch, res);
	res = parseScheduleNode(branch, c);
	if(res < 0)
		return releaseILObjectAndReturn(branch, res);

	if(parent->objType == ILObject::TypeBranch)
		static_cast<ILBranch*>(parent)->scheduleNodes.push_back(branch);
	else if(parent->objType == ILObject::TypeSchedule)
		static_cast<ILSchedule*>(parent)->scheduleNodes.push_back(branch);
	else
		return releaseILObjectAndReturn(branch, -ErrorILBranchInvalidParentNode);
    
    parseAllAttribute(branch, c);
	//objects.push_back(branch);
    return 1;
}

int ILParser::parseStatement(Statement* parent, const tinyxml2::XMLElement* root)
{
	string rootElementName = string((root->Name() == nullptr) ? "": root->Name());
	const tinyxml2::XMLElement *c = root->FirstChildElement();
	while (c) {
		setCurrentReadXMLElement(c);
		string stmtTypeStr = string((c->Name() == nullptr) ? "": c->Name());
		int res = 0;
		if(stmtTypeStr == "If"){
			res = parseStmtIf(parent, c);
		}else if(stmtTypeStr == "Else"){
			res = parseStmtElse(parent, c);
		}else if(stmtTypeStr == "ElseIf"){
			res = parseStmtElseIf(parent, c);
		}else if(stmtTypeStr == "While"){
			res = parseStmtWhile(parent, c);
		}else if(stmtTypeStr == "DoWhile"){
			res = parseStmtDoWhile(parent, c);
		}else if(stmtTypeStr == "For"){
			res = parseStmtFor(parent, c);
		}else if(stmtTypeStr == "Continue"){
			res = parseStmtContinue(parent, c);
		}else if(stmtTypeStr == "Switch"){
			res = parseStmtSwitch(parent, c);
		}else if(stmtTypeStr == "Case"){
			res = parseStmtCase(parent, c);
		}else if(stmtTypeStr == "Default"){
			res = parseStmtDefault(parent, c);
		}else if(stmtTypeStr == "Break"){
			res = parseStmtBreak(parent, c);
		}else if(stmtTypeStr == "Return"){
			res = parseStmtReturn(parent, c);
		//}else if(stmtTypeStr == "BraceStatement"){
		//	res = parseStmtBraceStatement(parent, c);
		}else if(stmtTypeStr == "LocalVariable"){
			res = parseStmtLocalVariable(parent, c);
		}else if(stmtTypeStr == "Expression"){
			res = parseStmtExpression(parent, c);
		}else if(stmtTypeStr == "BatchCalculation"){
			res = parseStmtBatchCalculation(parent, c);
		}else if(stmtTypeStr == "Assemble"){
			res = parseStmtAssemble(parent, c);
		}else if(rootElementName == "For" && (stmtTypeStr =="Initial" || stmtTypeStr =="Condition" || stmtTypeStr =="Iterator")){
			//????For???????????????Initial,Condition,Iterator
		}else if(rootElementName == "Calculate" && (stmtTypeStr =="Ref" || stmtTypeStr =="Input" || stmtTypeStr =="Output")){
			//????Calculate????????????Ref,Input,Output
		}else{
			return -ErrorStatementInvalidType;
		}
		if(res<0)
			return res;

		c = c->NextSiblingElement();
	}
	return 1;
}

int ILParser::parseStmtIf(Statement* parent, const tinyxml2::XMLElement* root)
{
	StmtIf* statement = new StmtIf(parent);
	statement->depth = parent->depth + 1;

	int res;
	if(root->FindAttribute("Condition"))
	{
		string conditionStr = string(root->Attribute("Condition"));
		res = parseExpression(statement, conditionStr);
		if(res < 0)
			return releaseStatementAndReturn(statement, res);
	}
	else
		return releaseStatementAndReturn(statement, -ErrorStatementIfHasNoAttributeCondition);

	res = parseStatement(statement, root);
	if(res < 0)
		return releaseStatementAndReturn(statement, res);
	
	statement->condition = statement->innerExpressions[0];
	statement->statements = statement->childStatements;

	parent->childStatements.push_back(statement);
	//statements.push_back(statement);
	return 1;
}

int ILParser::parseStmtElse(Statement* parent, const tinyxml2::XMLElement* root)
{
	StmtElse* statement = new StmtElse(parent);
	statement->depth = parent->depth + 1;

	int res = parseStatement(statement, root);
	if(res < 0)
		return releaseStatementAndReturn(statement, res);
	
	statement->statements = statement->childStatements;

	parent->childStatements.push_back(statement);
	//statements.push_back(statement);
	return 1;
}

int ILParser::parseStmtElseIf(Statement* parent, const tinyxml2::XMLElement* root)
{
	StmtElseIf* statement = new StmtElseIf(parent);
	statement->depth = parent->depth + 1;

	int res;
	if(root->FindAttribute("Condition"))
	{
		string conditionStr = string(root->Attribute("Condition"));
		res = parseExpression(statement, conditionStr);
		if(res < 0)
			return releaseStatementAndReturn(statement, res);
	}
	else
		return releaseStatementAndReturn(statement, -ErrorStatementElseIfHasNoAttributeCondition);

	res = parseStatement(statement, root);
	if(res < 0)
		return releaseStatementAndReturn(statement, res);
	
	statement->condition = statement->innerExpressions[0];
	statement->statements = statement->childStatements;

	parent->childStatements.push_back(statement);
	//statements.push_back(statement);
	return 1;
}

int ILParser::parseStmtWhile(Statement* parent, const tinyxml2::XMLElement* root)
{
	StmtWhile* statement = new StmtWhile(parent);
	statement->depth = parent->depth + 1;

	int res;
	if(root->FindAttribute("Condition"))
	{
		string conditionStr = string(root->Attribute("Condition"));
		res = parseExpression(statement, conditionStr);
		if(res < 0)
			return releaseStatementAndReturn(statement, res);
	}
	else
		return releaseStatementAndReturn(statement, -ErrorStatementWhileHasNoAttributeCondition);

	res = parseStatement(statement, root);
	if(res < 0)
		return releaseStatementAndReturn(statement, res);
	
	statement->condition = statement->innerExpressions[0];
	statement->statements = statement->childStatements;

	parent->childStatements.push_back(statement);
	//statements.push_back(statement);
	return 1;
}

int ILParser::parseStmtDoWhile(Statement* parent, const tinyxml2::XMLElement* root)
{
	StmtDoWhile* statement = new StmtDoWhile(parent);
	statement->depth = parent->depth + 1;

	int res;
	if(root->FindAttribute("Condition"))
	{
		string conditionStr = string(root->Attribute("Condition"));
		res = parseExpression(statement, conditionStr);
		if(res < 0)
			return releaseStatementAndReturn(statement, res);
	}
	else
		return releaseStatementAndReturn(statement, -ErrorStatementDoWhileHasNoAttributeCondition);

	res = parseStatement(statement, root);
	if(res < 0)
		return releaseStatementAndReturn(statement, res);
	
	statement->condition = statement->innerExpressions[0];
	statement->statements = statement->childStatements;

	parent->childStatements.push_back(statement);
	//statements.push_back(statement);
	return 1;
}

int ILParser::parseStmtFor(Statement* parent, const tinyxml2::XMLElement* root)
{
	//???????????????????Statement??????????
	//For???????????????Initial????????Condition?????????Iterator????????For????????
	StmtFor* statement = new StmtFor(parent);
	statement->depth = parent->depth + 1;
    

	const tinyxml2::XMLElement* elementInitial = root->FirstChildElement("Initial");
	
	Statement* statementInitial = new Statement(Statement::ExpressionList, parent->depth + CONST_NUM_2, statement);
	statement->childStatements.push_back(statementInitial);
	if(elementInitial)
	{
        setCurrentReadXMLElement(elementInitial);
		int res = parseStatement(statementInitial, elementInitial);
		if(res < 0)
			return releaseStatementAndReturn(statement, res);
	}

	StmtExpression* statementCondition = new StmtExpression(statement);
    statementCondition->depth = parent->depth + CONST_NUM_2;
	statement->childStatements.push_back(statementCondition);
	if(root->FindAttribute("Condition"))
	{
		string conditionStr = string(root->Attribute("Condition"));
		int res = parseExpression(statementCondition, conditionStr);
		if(res < 0)
			return releaseStatementAndReturn(statement, res);
	}


	const tinyxml2::XMLElement* elementIterator = root->FirstChildElement("Iterator");
	
	Statement* statementIterator = new Statement(Statement::ExpressionList, parent->depth + CONST_NUM_2, statement);
	statement->childStatements.push_back(statementIterator);
	if(elementIterator)
	{
        setCurrentReadXMLElement(elementIterator);
		int res = parseStatement(statementIterator, elementIterator);
		if(res < 0)
			return releaseStatementAndReturn(statement, res);
	}

	int res = parseStatement(statement, root);
	if(res < 0)
		return releaseStatementAndReturn(statement, res);

	if(statement->childStatements.size() < 3)
		return releaseStatementAndReturn(statement, -ErrorStatementForHasNoStatementIterator);
	statement->initial = statement->childStatements[0]->childStatements;
	if(statement->childStatements[1]->innerExpressions.size() > 0 && statement->childStatements[1]->innerExpressions[0])
		statement->condition = statement->childStatements[1]->innerExpressions[0];
	statement->iterator = statement->childStatements[CONST_NUM_2]->childStatements;
	for(int i = 3; i < statement->childStatements.size();i++)
	{
		statement->statements.push_back(statement->childStatements[i]);
	}

	parent->childStatements.push_back(statement);
	//statements.push_back(statement);
	return 1;
}

int ILParser::parseStmtContinue(Statement* parent, const tinyxml2::XMLElement* root) const
{
	StmtContinue* statement = new StmtContinue(parent);
	statement->depth = parent->depth + 1;

	parent->childStatements.push_back(statement);
	//statements.push_back(statement);
	return 1;
}

int ILParser::parseStmtSwitch(Statement* parent, const tinyxml2::XMLElement* root)
{
	StmtSwitch* statement = new StmtSwitch(parent);
	statement->depth = parent->depth + 1;

	int res;
	if(root->FindAttribute("Condition"))
	{
		string conditionStr = string(root->Attribute("Condition"));
		res = parseExpression(statement, conditionStr);
		if(res < 0)
			return releaseStatementAndReturn(statement, res);
	}
	else
		return releaseStatementAndReturn(statement, -ErrorStatementSwitchHasNoAttributeCondition);

	res = parseStatement(statement, root);
	if(res < 0)
		return releaseStatementAndReturn(statement, res);
	
	statement->condition = statement->innerExpressions[0];
	statement->statements = statement->childStatements;

	parent->childStatements.push_back(statement);
	//statements.push_back(statement);
	return 1;
}

int ILParser::parseStmtCase(Statement* parent, const tinyxml2::XMLElement* root) const
{
	StmtCase* statement = new StmtCase(parent);
	statement->depth = parent->depth + 1;

    
	if(root->FindAttribute("Value"))
	{
		string valueStr = string(root->Attribute("Value"));
		int res = parseExpression(statement, valueStr);
		if(res < 0)
			return releaseStatementAndReturn(statement, res);
	}
	else
		return releaseStatementAndReturn(statement, -ErrorStatementCaseHasNoAttributeValue);
	
	statement->value = statement->innerExpressions[0];
	
	parent->childStatements.push_back(statement);
	//statements.push_back(statement);
	return 1;
}

int ILParser::parseStmtDefault(Statement* parent, const tinyxml2::XMLElement* root) const
{
	StmtDefault* statement = new StmtDefault(parent);
	statement->depth = parent->depth + 1;

	parent->childStatements.push_back(statement);
	//statements.push_back(statement);
	return 1;
}

int ILParser::parseStmtBreak(Statement* parent, const tinyxml2::XMLElement* root) const
{
	StmtBreak* statement = new StmtBreak(parent);
	statement->depth = parent->depth + 1;

	parent->childStatements.push_back(statement);
	//statements.push_back(statement);
	return 1;
}

int ILParser::parseStmtReturn(Statement* parent, const tinyxml2::XMLElement* root) const
{
	StmtReturn* statement = new StmtReturn(parent);
	statement->depth = parent->depth + 1;

	int res = 0;
	if(root->FindAttribute("Expression"))
	{
		string valueStr = string(root->Attribute("Expression"));
		res = parseExpression(statement, valueStr);
		if(res < 0)
			return releaseStatementAndReturn(statement, res);

		statement->expression = statement->innerExpressions[0];
	}
	else
	{
		statement->expression = nullptr;
	}
	
	
	parent->childStatements.push_back(statement);
	//statements.push_back(statement);
	return 1;
}

//int ILParser::parseStmtBraceStatement(Statement* parent, const tinyxml2::XMLElement* root)
//{
//	return 0;
//}


int ILParser::parseStmtLocalVariable(Statement* parent, const tinyxml2::XMLElement* root) const
{
	StmtLocalVariable* statement = new StmtLocalVariable(parent);
	statement->depth = parent->depth + 1;

	int res = 0;
	if(root->FindAttribute("DefaultValue")) {
		string valueStr = string(root->Attribute("DefaultValue"));
        Expression* defaultValueExp = nullptr;
		res = parseExpressionDefaultValue(valueStr, &defaultValueExp);
		if(res < 0)
			return releaseStatementAndReturn(statement, res);
        statement->innerExpressions.push_back(defaultValueExp);
		statement->defaultValue = defaultValueExp;
	}

	if(root->FindAttribute("Name")) {
		string nameStr = string(root->Attribute("Name"));
		statement->name = nameStr;
	} else {
	    return releaseStatementAndReturn(statement, -ErrorStatementLocalVariableHasNoAttributeValue);
	}

	if(root->FindAttribute("Type")) {
		string typeStr = string(root->Attribute("Type"));
		statement->dataType = typeStr;
	} else {
	    return releaseStatementAndReturn(statement, -ErrorStatementLocalVariableHasNoAttributeType);
	}

    statement->isAddress = root->FindAttribute("IsAddress") ?
        parseUtilityIsAddress(string(root->Attribute("IsAddress"))) : statement->isAddress;
	
	//return -ErrorStatementLocalVariableHasNoAttributeType;
    
	statement->arraySize = root->FindAttribute("ArraySize") ?
        parseUtilityArraySize(string(root->Attribute("ArraySize"))) : statement->arraySize;
	
    
	statement->align = root->FindAttribute("Align") ? root->IntAttribute("Align") : statement->align;
    
	statement->isVolatile = root->FindAttribute("IsVolatile") ? root->IntAttribute("IsVolatile") : statement->isVolatile;
	
    statement->isConst = root->FindAttribute("IsConst") ? root->BoolAttribute("IsConst") : statement->isConst;

    if((res = ILParser::addVariableName(statement->name, statement->getDomain())) < 0)
		return releaseStatementAndReturn(statement, res);

	parent->childStatements.push_back(statement);
	//statements.push_back(statement);
	return 1;
}

int ILParser::parseStmtExpression(Statement* parent, const tinyxml2::XMLElement* root) const
{
	StmtExpression* statement = new StmtExpression(parent);
	statement->depth = parent->depth + 1;

	int res = 0;
	if(root->FindAttribute("Expression"))
	{
		string valueStr = string(root->Attribute("Expression"));
		res = parseExpression(statement, valueStr);
		if(res < 0)
			return releaseStatementAndReturn(statement, res);
	}

	if(statement->innerExpressions.empty())
		return releaseStatementAndReturn(statement, -ErrorStatementInvalidType);
	statement->expression = statement->innerExpressions[0];

	parent->childStatements.push_back(statement);
	//statements.push_back(statement);
	return 1;
}

int ILParser::parseStmtBatchCalculation(Statement* parent, const tinyxml2::XMLElement* root)
{
    StmtBatchCalculation* statement = new StmtBatchCalculation(parent);
	statement->depth = parent->depth + 1;

	int res;
	if(root->FindAttribute("Operation")) {
		string valueStr = string(root->Attribute("Operation"));
        if(StmtBatchCalculation::operationTypeStrToTypeMap.find(valueStr) != StmtBatchCalculation::operationTypeStrToTypeMap.end())
		    statement->operationType = StmtBatchCalculation::operationTypeStrToTypeMap.at(valueStr);
        else
             return releaseStatementAndReturn(statement, -ErrorStatementBatchCalculationHasNoAttributeOperation);
	} else {
        return releaseStatementAndReturn(statement, -ErrorStatementBatchCalculationHasNoAttributeOperation);
    }

    if(root->FindAttribute("DataType")) {
		string valueStr = string(root->Attribute("DataType"));
        statement->dataType = valueStr;
	} else {
        return releaseStatementAndReturn(statement, -ErrorStatementBatchCalculationHasNoAttributeDataType);
    }

    if(root->FindAttribute("Modifier")) {
		string valueStr = string(root->Attribute("Modifier"));
        if(StmtBatchCalculation::modifierTypeStrToTypeMap.find(valueStr) != StmtBatchCalculation::modifierTypeStrToTypeMap.end())
		    statement->modifierType = StmtBatchCalculation::modifierTypeStrToTypeMap.at(valueStr);

	}

    if(root->FindAttribute("Shape")) {
		string valueStr = string(root->Attribute("Shape"));
        if(StmtBatchCalculation::shapeTypeStrToTypeMap.find(valueStr) != StmtBatchCalculation::shapeTypeStrToTypeMap.end())
		    statement->shapeType = StmtBatchCalculation::shapeTypeStrToTypeMap.at(valueStr);

	}

    
    if((res = parseStmtBatchCalculationInput(statement, root)) < 0)
        return releaseStatementAndReturn(statement, res);
    
    if((res = parseStmtBatchCalculationOutput(statement, root)) < 0)
        return releaseStatementAndReturn(statement, res);
    

	parent->childStatements.push_back(statement);

	return 1;
}

int ILParser::parseStmtBatchCalculationInput(Statement* parent, const tinyxml2::XMLElement* root)
{

    StmtBatchCalculation* stmtParent = static_cast<StmtBatchCalculation*>(parent);

    //vector<int> arraySize;

    const tinyxml2::XMLElement* c = root->FirstChildElement("Input");
    while(c){
        setCurrentReadXMLElement(c);

        auto input = StmtBatchCalculation::StmtBatchCalculationInput();
        

	    if(c->FindAttribute("Name"))
		    input.name = string(c->Attribute("Name"));
	    else
		    return -ErrorStatementBatchCalculationInputHasNoAttributeName;

	    if(c->FindAttribute("DefaultValue")) {
	        string defaultValueStr = string(c->Attribute("DefaultValue"));
		    int res = parseExpressionDefaultValue(defaultValueStr, &input.defaultValue);
		    if(res < 0)
			    return res;
	    }
        
        input.isAddress = c->FindAttribute("IsAddress") ?
            parseUtilityIsAddress(string(c->Attribute("IsAddress"))) : input.isAddress;

	    if(c->FindAttribute("Type"))
		    input.type = string(c->Attribute("Type"));
	    else
		    return -ErrorStatementBatchCalculationInputHasNoAttributeType;
        
        input.type = stringStartsWith(input.type, "struct ") ? input.type.substr(CONST_NUM_7, input.type.length() - CONST_NUM_7) : input.type;
        
        input.arraySize = c->FindAttribute("ArraySize") ?
            parseUtilityArraySize(string(c->Attribute("ArraySize"))) : input.arraySize;

        //if(arraySize.empty())
        //{
        //    arraySize = input.arraySize;
        //}
        //else if(!input.arraySize.empty())
        //{
        //    if(!areTwoIntVectorSame(arraySize, input.arraySize))
        //    {
        //        return -ErrorStatementBatchCalculationInputSizeInvalid;
        //    }
        //}
		parseStmtBatchCalculationRange(&input, c);
        stmtParent->inputs.push_back(input);
        
        c = c->NextSiblingElement("Input");
    }
    //if(arraySize.empty())
    //{
    //    return -ErrorStatementBatchCalculationInputSizeInvalid;
    //}
    return 1;
}

int ILParser::parseStmtBatchCalculationOutput(Statement* parent, const tinyxml2::XMLElement* root)
{
    StmtBatchCalculation* stmtParent = static_cast<StmtBatchCalculation*>(parent);

    const tinyxml2::XMLElement* c = root->FirstChildElement("Output");
    while(c){
        setCurrentReadXMLElement(c);
        auto output = StmtBatchCalculation::StmtBatchCalculationOutput();
        
	    if(c->FindAttribute("Name"))
		    output.name = string(c->Attribute("Name"));
	    else
		    return -ErrorStatementBatchCalculationOutputHasNoAttributeName;

	    if(c->FindAttribute("DefaultValue")) {
	        string defaultValueStr = string(c->Attribute("DefaultValue"));
		    int res = parseExpressionDefaultValue(defaultValueStr, &output.defaultValue);
		    if(res < 0)
			    return res;
	    }
        
        output.isAddress = c->FindAttribute("IsAddress") ?
            parseUtilityIsAddress(string(c->Attribute("IsAddress"))) : output.isAddress;

	    if(c->FindAttribute("Type"))
		    output.type = string(c->Attribute("Type"));
	    else
		    return -ErrorStatementBatchCalculationOutputHasNoAttributeType;

        output.type = stringStartsWith(output.type, "struct ") ? output.type.substr(CONST_NUM_7, output.type.length() - CONST_NUM_7) : output.type;
        
        output.arraySize = c->FindAttribute("ArraySize") ?
            parseUtilityArraySize(string(c->Attribute("ArraySize"))) : output.arraySize;
        
		parseStmtBatchCalculationRange(&output, c);
        stmtParent->outputs.push_back(output);
        c = c->NextSiblingElement("Output");
    }
    return 1;
}

int ILParser::parseStmtBatchCalculationRange(StmtBatchCalculation::StmtBatchCalculationInput* parent, const tinyxml2::XMLElement* root)
{
	const tinyxml2::XMLElement* c = root->FirstChildElement("Range");
	if (c) {
		setCurrentReadXMLElement(c);
		auto range = StmtBatchCalculation::StmtBatchCalculationRange();

		if (c->FindAttribute("Start"))
			range.start = stringToInt(c->Attribute("Start"));

		if (c->FindAttribute("Length"))
			range.length = stringToInt(c->Attribute("Length"));

		range.index = c->FindAttribute("Index") ?
			parseUtilityIndex(string(c->Attribute("Index"))) : range.index;
		parent->range = range;
	}
	return 1;
}

int ILParser::parseStmtBatchCalculationRange(StmtBatchCalculation::StmtBatchCalculationOutput* parent, const tinyxml2::XMLElement* root)
{
	const tinyxml2::XMLElement* c = root->FirstChildElement("Range");
	if (c) {
		setCurrentReadXMLElement(c);
		auto range = StmtBatchCalculation::StmtBatchCalculationRange();

		if (c->FindAttribute("Start"))
			range.start = stringToInt(c->Attribute("Start"));

		if (c->FindAttribute("Length"))
			range.length = stringToInt(c->Attribute("Length"));

		range.index = c->FindAttribute("Index") ?
			parseUtilityIndex(string(c->Attribute("Index"))) : range.index;
		parent->range = range;
	}
	return 1;
}

int ILParser::parseStmtAssemble(Statement* parent, const tinyxml2::XMLElement* root)
{
    StmtAssemble* statement = new StmtAssemble(parent);
	statement->depth = parent->depth + 1;

	int res;
    
    if((res = parseStmtBatchCalculationInput(statement, root)) < 0)
        return releaseStatementAndReturn(statement, res);
    
    if((res = parseStmtBatchCalculationOutput(statement, root)) < 0)
        return releaseStatementAndReturn(statement, res);

    const tinyxml2::XMLElement* c = root->FirstChildElement("ASM");
    while(c){
        setCurrentReadXMLElement(c);

        string valueStr = stringTrim(string(c->Attribute("Expression")));
        if(!valueStr.empty())
        {
            statement->expressions.push_back(valueStr);
        }
        c = c->NextSiblingElement("ASM");
    }

	parent->childStatements.push_back(statement);

	return 1;
}

int ILParser::parseStmtAssembleInput(Statement* parent, const tinyxml2::XMLElement* root)
{
    StmtAssemble* stmtParent = static_cast<StmtAssemble*>(parent);
    
    const tinyxml2::XMLElement* c = root->FirstChildElement("Input");
    while(c){
        setCurrentReadXMLElement(c);

        auto input = StmtAssemble::StmtAssembleInput();
        

	    if(c->FindAttribute("Name"))
		    input.name = string(c->Attribute("Name"));
	    else
		    return -ErrorStatementBatchCalculationInputHasNoAttributeName;

	    if(c->FindAttribute("DefaultValue")) {
	        string defaultValueStr = string(c->Attribute("DefaultValue"));
		    int res = parseExpressionDefaultValue(defaultValueStr, &input.defaultValue);
		    if(res < 0)
			    return res;
	    }
        
        input.isAddress = c->FindAttribute("IsAddress") ?
            parseUtilityIsAddress(string(c->Attribute("IsAddress"))) : input.isAddress;

	    if(c->FindAttribute("Type"))
		    input.type = string(c->Attribute("Type"));
	    else
		    return -ErrorStatementBatchCalculationInputHasNoAttributeType;
        
        input.type = stringStartsWith(input.type, "struct ") ? input.type.substr(CONST_NUM_7, input.type.length() - CONST_NUM_7) : input.type;
        
        input.arraySize = c->FindAttribute("ArraySize") ?
            parseUtilityArraySize(string(c->Attribute("ArraySize"))) : input.arraySize;
        
        stmtParent->inputs.push_back(input);

        
        c = c->NextSiblingElement("Input");
    }
    return 1;
}

int ILParser::parseStmtAssembleOutput(Statement* parent, const tinyxml2::XMLElement* root)
{
    StmtAssemble* stmtParent = static_cast<StmtAssemble*>(parent);
    const tinyxml2::XMLElement* c = root->FirstChildElement("Output");
    while(c){
        setCurrentReadXMLElement(c);
        auto output = StmtAssemble::StmtAssembleOutput();
        
	    if(c->FindAttribute("Name"))
		    output.name = string(c->Attribute("Name"));
	    else
		    return -ErrorStatementBatchCalculationOutputHasNoAttributeName;

	    if(c->FindAttribute("DefaultValue")) {
	        string defaultValueStr = string(c->Attribute("DefaultValue"));
		    int res = parseExpressionDefaultValue(defaultValueStr, &output.defaultValue);
		    if(res < 0)
			    return res;
	    }
        
        output.isAddress = c->FindAttribute("IsAddress") ?
            parseUtilityIsAddress(string(c->Attribute("IsAddress"))) : output.isAddress;

	    if(c->FindAttribute("Type"))
		    output.type = string(c->Attribute("Type"));
	    else
		    return -ErrorStatementBatchCalculationOutputHasNoAttributeType;

        output.type = stringStartsWith(output.type, "struct ") ? output.type.substr(CONST_NUM_7, output.type.length() - CONST_NUM_7) : output.type;
        
        output.arraySize = c->FindAttribute("ArraySize") ?
            parseUtilityArraySize(string(c->Attribute("ArraySize"))) : output.arraySize;
        
        stmtParent->outputs.push_back(output);

        c = c->NextSiblingElement("Output");
    }
    return 1;
}

int ILParser::parseExpression(Statement* parent, string expressionStr) const
{
	ILExpressionParser expressionParser;
	Expression* expRet = nullptr;
	int res = expressionParser.parseExpression(expressionStr, &expRet);
	if(res < 0)
		return res;
	parent->innerExpressions.push_back(expRet);

    res = collectFunctionCall(parent, expRet);
    if(res < 0)
		return res;

	return 1;
}

int ILParser::parseExpressionDefaultValue(std::string expressionStr, Expression** retExpression) const
{
    expressionStr = "v=" + expressionStr;
    Expression* tempExp = nullptr;
    ILExpressionParser expressionParser;
	int res = expressionParser.parseExpression(expressionStr, &tempExp);
	if(res < 0)
		return res;
    *retExpression = tempExp->childExpressions[1]->clone();
    tempExp->release();
    delete tempExp;
    return 1;
}

int ILParser::releaseILObjectAndReturn(ILObject* object, int retCode) const
{
    object->release();
    delete object;
    return retCode;
}

int ILParser::releaseStatementAndReturn(Statement* stmt, int retCode) const
{
    stmt->release();
    delete stmt;
    return retCode;
}

int ILParser::parseUtilityIsAddress(std::string str) const
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

bool ILParser::parseUtilityIsOptimized(std::string str) const
{
	string isAddressStr = stringToLower(str);
	if (isAddressStr == "true")
		return true;
	else
		return false;
}

vector<int> ILParser::parseUtilityArraySize(std::string str) const
{
    vector<int> ret;
	vector<string> arraySizeStrSplit = stringSplit(str, ",");
	for(int i = 0;i < arraySizeStrSplit.size();i++)
	{
		ret.push_back(stringToInt(arraySizeStrSplit[i]));
	}
    return ret;
}

vector<int> ILParser::parseUtilityIndex(std::string str) const
{
	vector<int> ret;
	vector<string> indexStrSplit = stringSplit(str, ",");
	for (int i = 0; i < indexStrSplit.size(); i++)
	{
		ret.push_back(stringToInt(indexStrSplit[i]));
	}
	return ret;
}

int ILParser::collectFunctionCall(Statement* parent, Expression* exp) const
{
    int res = 1;
    if(exp->type == Token::Function)
    {
        ILFunction* iLFunction = parent->getContainerILFunction();
        if(iLFunction)
        {
            ILFunction::FunctionCallInfo callInfo;
            callInfo.statement = parent;
            callInfo.statementDepth = parent->getDomain().size() - 1;
            callInfo.functionName = exp->tokenStr;
            iLFunction->functionCallInfoList.push_back(callInfo);
        }
    }

    for(int i = 0; i < exp->childExpressions.size(); i++)
    {
        res = collectFunctionCall(parent, exp->childExpressions[i]);
        if(res < 0)
            return res;
    }
    return res;
}

void ILParser::release()
{
	//objects.clear();
	//statements.clear();
	for(int i = 0;i < files.size();i++)
	{
		files[i]->release();
        delete files[i];
	}
	files.clear();

    allDataTypeList.clear();
    allDataTypeList = basicDataTypeList;
    structTypeList.clear();
    enumTypeList.clear();
    typeDefineTypeList.clear();
    variableList.clear();

    errorCode = 0;
    currentReadXMLLineNum = -1;
    currentReadXMLElement = nullptr;
}

void ILParser::releaseStatement(Statement* stmt) const
{
	Statement* parent = stmt->parentStatement;
	if(!parent){
        stmt->release();
        delete stmt;
	    return;
    }
	parent->childStatements.erase(find(parent->childStatements.begin(), parent->childStatements.end(),stmt));
    vector<Statement*>* childStmtList = nullptr;
    if(parent->type == Statement::If) {
        childStmtList = &(static_cast<StmtIf*>(parent)->statements);
	} else if(parent->type == Statement::Else) {
        childStmtList = &(static_cast<StmtElse*>(parent)->statements);
	} else if(parent->type == Statement::ElseIf) {
        childStmtList = &(static_cast<StmtElseIf*>(parent)->statements);
	} else if(parent->type == Statement::While) {
        childStmtList = &(static_cast<StmtWhile*>(parent)->statements);
	} else if(parent->type == Statement::DoWhile) {
        childStmtList = &(static_cast<StmtDoWhile*>(parent)->statements);
	} else if(parent->type == Statement::Switch) {
        childStmtList = &(static_cast<StmtSwitch*>(parent)->statements);
	} else if(parent->type == Statement::For) {
		StmtFor* stmtTemp = static_cast<StmtFor*>(parent);
        auto iter1 = find(stmtTemp->initial.begin(), stmtTemp->initial.end(), stmt);
		auto iter2 = find(stmtTemp->statements.begin(), stmtTemp->statements.end(), stmt);
		auto iter3 = find(stmtTemp->iterator.begin(), stmtTemp->iterator.end(), stmt);
		if(iter1 != stmtTemp->initial.end()) {
			stmtTemp->initial.erase(iter1);
		} else if(iter2 != stmtTemp->statements.end()) {
			stmtTemp->statements.erase(iter2);
		} else if(iter3 != stmtTemp->iterator.end()) {
			stmtTemp->iterator.erase(iter3);
		}
        stmt->release();
        delete stmt;
        return;
	} else {
        stmt->release();
        delete stmt;
        return;
    }
    auto iter = find(childStmtList->begin(), childStmtList->end(), stmt);
	if(iter != childStmtList->end()){
		childStmtList->erase(iter);
    }
    stmt->release();
    delete stmt;
}

void ILParser::releaseILObject(ILObject* object) const
{
	ILObject* parent = object->parent;
	if(!parent) {
	    object->release();
        delete object;
		return;
	}
    vector<ILHeadFile*>* listILHeadFile;
    vector<ILMacro*>* listILMacro;
    vector<ILStructDefine*>* listILStructDefine;
    vector<ILGlobalVariable*>* listILGlobalVariable;
    vector<ILUtilityFunction*>* listILUtilityFunction;
    vector<ILFunction*>* listILFunction;
    vector<ILRef*>* listILRef;
    vector<ILMember*>* listILMember;
    vector<ILInput*>* listILInput;
    vector<ILOutput*>* listILOutput;
    vector<ILScheduleNode*>* listILScheduleNode;
    vector<ILLocalVariable*>* listILLocalVariable;
    vector<ILTypeDefine*>* listILTypeDefine;
    vector<ILEnumDefine*>* listILEnumDefine;
    vector<ILEnumValue*>* listILEnumValue;
    if(object->objType == ILObject::TypeHeadFile && (listILHeadFile = parent->getListOfILHeadFile())) {
        listILHeadFile->erase(find(listILHeadFile->begin(),listILHeadFile->end(), object));
    }
    else if(object->objType == ILObject::TypeMacro && (listILMacro = parent->getListOfILMacro())) {
        listILMacro->erase(find(listILMacro->begin(),listILMacro->end(), object));
    }
    else if(object->objType == ILObject::TypeStructDefine && (listILStructDefine = parent->getListOfILStructDefine())) {
        listILStructDefine->erase(find(listILStructDefine->begin(),listILStructDefine->end(), object));
    }
    else if(object->objType == ILObject::TypeGlobalVariable && (listILGlobalVariable = parent->getListOfILGlobalVariable())) {
        listILGlobalVariable->erase(find(listILGlobalVariable->begin(),listILGlobalVariable->end(), object));
    }
    else if(object->objType == ILObject::TypeUtilityFunction && (listILUtilityFunction = parent->getListOfILUtilityFunction())) {
        listILUtilityFunction->erase(find(listILUtilityFunction->begin(),listILUtilityFunction->end(), object));
    }
    else if(object->objType == ILObject::TypeFunction && (listILFunction = parent->getListOfILFunction())) {
        listILFunction->erase(find(listILFunction->begin(),listILFunction->end(), object));
    }
    else if(object->objType == ILObject::TypeRef && (listILRef = parent->getListOfILRef())) {
        listILRef->erase(find(listILRef->begin(),listILRef->end(), object));
    }
    else if(object->objType == ILObject::TypeMember && (listILMember = parent->getListOfILMember())) {
        listILMember->erase(find(listILMember->begin(),listILMember->end(), object));
    }
    else if(object->objType == ILObject::TypeInput && (listILInput = parent->getListOfILInput())) {
        listILInput->erase(find(listILInput->begin(),listILInput->end(), object));
    }
    else if(object->objType == ILObject::TypeOutput && (listILOutput = parent->getListOfILOutput())) {
        listILOutput->erase(find(listILOutput->begin(),listILOutput->end(), object));
    }
    else if(object->objType == ILObject::TypeBranch && (listILScheduleNode = parent->getListOfILScheduleNode())) {
        listILScheduleNode->erase(find(listILScheduleNode->begin(),listILScheduleNode->end(), object));
    }
    else if(object->objType == ILObject::TypeCalculate && (listILScheduleNode = parent->getListOfILScheduleNode())) {
        listILScheduleNode->erase(find(listILScheduleNode->begin(),listILScheduleNode->end(), object));
    }
    else if(object->objType == ILObject::TypeLocalVariable && (listILLocalVariable = parent->getListOfILLocalVariable())) {
        listILLocalVariable->erase(find(listILLocalVariable->begin(),listILLocalVariable->end(), object));
    }
    else if(object->objType == ILObject::TypeTypeDefine && (listILTypeDefine = parent->getListOfILTypeDefine())) {
        listILTypeDefine->erase(find(listILTypeDefine->begin(),listILTypeDefine->end(), object));
    }
    else if(object->objType == ILObject::TypeEnumDefine && (listILEnumDefine = parent->getListOfILEnumDefine())) {
        listILEnumDefine->erase(find(listILEnumDefine->begin(),listILEnumDefine->end(), object));
    }
    else if(object->objType == ILObject::TypeEnumValue && (listILEnumValue = parent->getListOfILEnumValue())) {
        listILEnumValue->erase(find(listILEnumValue->begin(),listILEnumValue->end(), object));
    }
	object->release();
    delete object;
}

void ILParser::clearVariableAndDataTypeRecord()
{
    allDataTypeList.clear();
    allDataTypeList = basicDataTypeList;
    structTypeList.clear();
    enumTypeList.clear();
    typeDefineTypeList.clear();
    variableList.clear();
}

string ILParser::convertDataType(string typeName)
{
    if(basicDataTypeMap.find(typeName) != ILParser::basicDataTypeMap.end())
    {
        return basicDataTypeMap[typeName];
    }

    string findTypeName = typeName;
    if(stringStartsWith(findTypeName, "struct "))
    {
        findTypeName = findTypeName.substr(CONST_NUM_7, findTypeName.length() - CONST_NUM_7);
    }

	for(int i = 0;i < structTypeList.size();i++)
	{
		if(structTypeList[i]->name == findTypeName)
			return "struct " + structTypeList[i]->name;
	}
    
    //ILEnumDefine* iLEnumDefine = findEnumType(typeName);
    //if(iLEnumDefine)
    //{
    //    return "enum " + iLEnumDefine->name;
    //}
	return typeName;
}

std::string ILParser::getRealDataType(std::string typeName)
{
    ILTypeDefine* iLTypeDefine = findTypeDefineByValue(typeName);
    while(iLTypeDefine)
    {
        if(basicDataTypeMap.find(typeName) != ILParser::basicDataTypeMap.end())
        {
            return basicDataTypeMap[typeName];
        }
        ILStructDefine* iLStructDefine = findStructType(typeName);
        if(iLStructDefine)
        {
            return iLStructDefine->name;
        }
        typeName = iLTypeDefine->name;
        iLTypeDefine = findTypeDefineByValue(typeName);
    }
    if(basicDataTypeMap.find(typeName) != ILParser::basicDataTypeMap.end())
    {
        return basicDataTypeMap[typeName];
    }
    ILStructDefine* iLStructDefine = findStructType(typeName);
    if(iLStructDefine)
    {
        return iLStructDefine->name;
    }
    //ILEnumDefine* iLEnumDefine = findEnumType(typeName);
    //if(iLEnumDefine)
    //{
    //    return "enum " + iLEnumDefine->name;
    //}
	return typeName;
}


bool ILParser::isBasicType(std::string typeName)
{
    if(basicDataTypeMap.find(typeName) != basicDataTypeMap.end())
        return true;
    if(find(basicDataTypeList.begin(), basicDataTypeList.end(),typeName) != basicDataTypeList.end())
        return true;
    return false;
}
