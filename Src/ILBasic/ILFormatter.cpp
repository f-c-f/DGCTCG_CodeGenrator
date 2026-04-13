#include "ILFormatter.h"

#include <fstream>
#include <algorithm>

#include "Expression.h"
#include "../Common/Utility.h"
#include "ILParser.h"
#include "ILCCodeParser.h"
#include "ILEnumDefine.h"
#include "ILEnumValue.h"
#include "ILFile.h"
#include "ILFunction.h"
#include "ILSchedule.h"
#include "ILBranch.h"
#include "ILCalculate.h"
#include "ILLocalVariable.h"
#include "StmtBatchCalculation.h"
#include "ILHeadFile.h"
#include "ILMacro.h"
#include "ILMember.h"
#include "ILRef.h"
#include "ILSaver.h"
#include "ILScheduleNode.h"
#include "ILStructDefine.h"
#include "ILTypeDefine.h"
using namespace std;

namespace
{
    const int CONST_NUM_2 = 2;
}

int ILFormatter::format(tinyxml2::XMLDocument& doc)
{
	tinyxml2::XMLElement* root = doc.RootElement();
    if(!root)
        return -ErrorLoadILFileFailInner;

	tinyxml2::XMLElement* rootFormat = doc.NewElement("Root");
    if(!rootFormat)
        return -ErrorCreateXMLNodeFail;
    
	doc.InsertEndChild(rootFormat);

	int res = collectStructDefineA(root);
	if(res < 0)
		return res;
    res = collectStructDefineB(root);
	if(res < 0)
		return res;

    res = collectEnumDefine(root);
	if(res < 0)
		return res;

    res = collectTypeDefine(root);
	if(res < 0)
		return res;

	res = traverse(root, rootFormat, &doc);
	if(res < 0)
		return res;

	doc.DeleteChild(root);



	return 1;
}

int ILFormatter::format(string file, string outputFilePath)
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError ret = doc.LoadFile(file.c_str());
	if (ret != 0) {
		fprintf(stderr, "Fail to load IL file: %s\n", file.c_str());
	    setCurrentError(-ILFormatter::ErrorLoadILFileFail);
        fprintf(stderr, "%s\n", doc.ErrorStr());
		return -ILFormatter::ErrorLoadILFileFail;
	}
	int res = format(doc);
	if(res < 0)
	{
	    setCurrentError(res);
		return res;
	}
    doc.SaveFile(outputFilePath.c_str());
	

    ilParser.release();

    res = ilParser.parseIL(outputFilePath);
    if(res < 0)
	{
        fprintf(stderr, "Fail to parse Formatted IL file: %s\n", outputFilePath.c_str());
	    setCurrentError(res);
		return res;
    }

    res = deduplication(&ilParser);
    if(res < 0)
	{
        fprintf(stderr, "Fail to deduplication Formatted IL file: %s\n", outputFilePath.c_str());
	    setCurrentError(res);
		return res;
    }

    res = renameBatchCalculationTempVariable(&ilParser);
    if(res < 0)
	{
        fprintf(stderr, "Fail to Rename BatchCalculation Temp Variable Formatted IL file: %s\n", outputFilePath.c_str());
	    setCurrentError(res);
		return res;
    }

    ILSaver saver;
    res = saver.save(&ilParser, outputFilePath);
    if(res < 0)
	{
        fprintf(stderr, "Fail to save Formatted IL file: %s\n", outputFilePath.c_str());
	    setCurrentError(res);
		return res;
    }

	return 1;
}

int ILFormatter::collectStructDefineA(const tinyxml2::XMLElement* root)
{
	const tinyxml2::XMLElement *c = root->FirstChildElement("file");
	while (c)
	{
		setCurrentReadXMLElement(c);
		const tinyxml2::XMLElement *cc = c->FirstChildElement("record");
		while (cc)
		{
			setCurrentReadXMLElement(cc);
			ILStructDefine* newStructType = new ILStructDefine();
			if(cc->FindAttribute("name"))
				newStructType->name = string(cc->Attribute("name"));
			else
				return releaseILObjectAndReturn(newStructType, -ILFormatter::ErrorStructDefineMissNameAttribute);

            int res = collectStructDefineMemberA(cc, newStructType);
			if(res < 0)
                return releaseILObjectAndReturn(newStructType, res);
            res = collectStructDefineMemberB(cc, newStructType);
			if(res < 0)
                return releaseILObjectAndReturn(newStructType, res);
			ILParser::structTypeList.push_back(newStructType);
			if (std::find(ILParser::allDataTypeList.begin(), ILParser::allDataTypeList.end(),
				"struct " + newStructType->name) != ILParser::allDataTypeList.end())
				return releaseILObjectAndReturn(newStructType, -ErrorStructDefineRedefined);
			ILParser::allDataTypeList.push_back("struct " + newStructType->name);
			cc = cc->NextSiblingElement("record");
		}
		c = c->NextSiblingElement("file");
	}
	return 0;
}

int ILFormatter::collectStructDefineB(const tinyxml2::XMLElement* root)
{
	const tinyxml2::XMLElement *c = root->FirstChildElement("File");
	while (c)
	{
		setCurrentReadXMLElement(c);
		const tinyxml2::XMLElement *cc = c->FirstChildElement("Struct");
		while (cc)
		{
			setCurrentReadXMLElement(cc);
			ILStructDefine* newStructType = new ILStructDefine();
			if(cc->FindAttribute("Name"))
				newStructType->name = string(cc->Attribute("Name"));
			else
				return releaseILObjectAndReturn(newStructType, -ILFormatter::ErrorStructDefineMissNameAttribute);

            int res = collectStructDefineMemberA(cc, newStructType);
			if(res < 0)
                return releaseILObjectAndReturn(newStructType, res);
            res = collectStructDefineMemberB(cc, newStructType);
			if(res < 0)
                return releaseILObjectAndReturn(newStructType, res);
			ILParser::structTypeList.push_back(newStructType);
			if (std::find(ILParser::allDataTypeList.begin(), ILParser::allDataTypeList.end(),
				"struct " + newStructType->name) != ILParser::allDataTypeList.end())
				return releaseILObjectAndReturn(newStructType, -ErrorStructDefineRedefined);
			ILParser::allDataTypeList.push_back("struct " + newStructType->name);
			cc = cc->NextSiblingElement("Struct");
		}
		c = c->NextSiblingElement("File");
	}
	return 0;
}



int ILFormatter::collectStructDefineMemberA(const tinyxml2::XMLElement* root, ILStructDefine* newStructType)
{
    const tinyxml2::XMLElement *c = root->FirstChildElement("member");
	while (c)
	{
		setCurrentReadXMLElement(c);
		ILMember* newStructMember = new ILMember();
		if(c->FindAttribute("name"))
			newStructMember->name = string(c->Attribute("name"));
		else
			return releaseILObjectAndReturn(newStructMember, -ILFormatter::ErrorStructMemberDefineMissNameAttribute);
		if(c->FindAttribute("type"))
			newStructMember->type = string(c->Attribute("type"));
		else
			return releaseILObjectAndReturn(newStructMember, -ILFormatter::ErrorStructMemberDefineMissTypeAttribute);

		newStructMember->isAddress = 0;
		if(c->FindAttribute("isaddress"))
		{
            string isAddressStr = stringToLower(string(c->Attribute("isaddress")));
		    if(isAddressStr == "true" || isAddressStr == "false")
		    {
		        newStructMember->isAddress = (isAddressStr == "true") ? 1 : 0;
		    }
            else
            {
                newStructMember->isAddress = stringToInt(isAddressStr);
            }
		}
			
			
		if(c->FindAttribute("arraysize"))
		{
			vector<string> arraySizeStrSplit = stringSplit(string(c->Attribute("arraysize")), ",");
			for(int i = 0;i < arraySizeStrSplit.size();i++)
			{
				newStructMember->arraySize.push_back(stringToInt(arraySizeStrSplit[i]));
			}
		}
		newStructType->members.push_back(newStructMember);
		c = c->NextSiblingElement("member");
	}
    return 0;
}

int ILFormatter::collectStructDefineMemberB(const tinyxml2::XMLElement* root, ILStructDefine* newStructType)
{
    const tinyxml2::XMLElement *c = root->FirstChildElement("Member");
	while (c)
	{
		setCurrentReadXMLElement(c);
		ILMember* newStructMember = new ILMember();
		if(c->FindAttribute("Name"))
			newStructMember->name = string(c->Attribute("Name"));
		else
			return  releaseILObjectAndReturn(newStructMember, -ILFormatter::ErrorStructMemberDefineMissNameAttribute);
		if(c->FindAttribute("Type"))
			newStructMember->type = string(c->Attribute("Type"));
		else
			return releaseILObjectAndReturn(newStructMember, -ILFormatter::ErrorStructMemberDefineMissTypeAttribute);

		newStructMember->isAddress = 0;
		if(c->FindAttribute("IsAddress"))
		{
		    string isAddressStr = stringToLower(string(c->Attribute("IsAddress")));
		    if(isAddressStr == "true" || isAddressStr == "false")
		    {
		        newStructMember->isAddress = (isAddressStr == "true") ? 1 : 0;
		    }
            else
            {
                newStructMember->isAddress = stringToInt(isAddressStr);
            }
		}
			
		if(c->FindAttribute("ArraySize"))
		{
			vector<string> arraySizeStrSplit = stringSplit(string(c->Attribute("ArraySize")), ",");
			for(int i = 0;i < arraySizeStrSplit.size();i++)
			{
				newStructMember->arraySize.push_back(stringToInt(arraySizeStrSplit[i]));
			}
		}
		newStructType->members.push_back(newStructMember);
		c = c->NextSiblingElement("Member");
	}
    return 0;
}

int ILFormatter::collectTypeDefine(const tinyxml2::XMLElement* root)
{
    const tinyxml2::XMLElement *c = root->FirstChildElement("File");
	while (c)
	{
		setCurrentReadXMLElement(c);
		const tinyxml2::XMLElement *cc = c->FirstChildElement("TypeDefine");
		while (cc)
		{
		    setCurrentReadXMLElement(cc);
		    ILTypeDefine* typeDefine = new ILTypeDefine();
		    typeDefine->objType = ILObject::TypeMacro;
		    if(cc->FindAttribute("Name"))
			    typeDefine->name = string(cc->Attribute("Name"));
		    else
			    return releaseILObjectAndReturn(typeDefine, -ErrorILTypeDefineHasNoAttributeName);
		    if(cc->FindAttribute("Value"))
			    typeDefine->value = string(cc->Attribute("Value"));
		    else
			    return releaseILObjectAndReturn(typeDefine, -ErrorILTypeDefineHasNoAttributeValue);
            
            
		    ILParser::typeDefineTypeList.push_back(typeDefine);

            if (std::find(ILParser::allDataTypeList.begin(), ILParser::allDataTypeList.end(),
			    typeDefine->value) != ILParser::allDataTypeList.end())
			    return -ErrorTypeRedefined;
		    
            int index = typeDefine->value.find("[");
            if(index != string::npos) {
                ILParser::allDataTypeList.push_back(typeDefine->value.substr(0, index));
            } else {
                ILParser::allDataTypeList.push_back(typeDefine->value);
            }
			cc = cc->NextSiblingElement("TypeDefine");
		}
		c = c->NextSiblingElement("File");
	}
	return 0;
}


void ILFormatter::setCurrentError(int errorCode)
{
	this->errorCode = errorCode;
}

string ILFormatter::getErrorStr()
{
	string ret = "ILFormatter:\nError Code: " + to_string(errorCode) + "\n";
	if(currentReadXMLElement)
		ret += "XML line: " + to_string(currentReadXMLLineNum);
	else
		ret += "XML line: Unclear.";
	
	ret += "\n";

    ILCCodeParser iLCCodeParserTemp;

	int errorCodeIndex = -errorCode - ILFormatter::ErrorLoadILFileFail;

	if(errorCodeIndex >= 0 && errorCodeIndex < errorStrList.size())
    {
		ret += errorStrList[errorCodeIndex];
    }
    else if((errorCodeIndex = -errorCode - ILParser::ErrorLoadILFileFail) && errorCodeIndex >= 0 && errorCodeIndex < ilParser.errorStrList.size())
    {
		ret += ilParser.getErrorStr();
    }
	else if((errorCodeIndex = -errorCode - ILCCodeParser::ErrorCommentTailNotFound) && errorCodeIndex >= 0 && errorCodeIndex < iLCCodeParserTemp.errorStrList.size())
    {
		ret += iLCCodeParserTemp.getErrorStr(errorCodeIndex);
    }
	else
    {
		ret += "Unknow error code.";
    }

	return ret;
}

void ILFormatter::setCurrentReadXMLElement(const tinyxml2::XMLElement* ele)
{
	currentReadXMLLineNum = ele->GetLineNum();
	currentReadXMLElement = ele;
}


int ILFormatter::collectEnumDefine(const tinyxml2::XMLElement* root)
{
    const tinyxml2::XMLElement *c = root->FirstChildElement("File");
	while (c)
	{
		setCurrentReadXMLElement(c);
        const tinyxml2::XMLElement *cc = root->FirstChildElement("Enum");
	    while (cc)
	    {
		    setCurrentReadXMLElement(cc);
		    ILEnumDefine* enumDefine = new ILEnumDefine();
		    enumDefine->objType = ILObject::TypeEnumDefine;
		    if(cc->FindAttribute("Name"))
			    enumDefine->name = string(cc->Attribute("Name"));
		    else
			    return releaseILObjectAndReturn(enumDefine, -ErrorILEnumDefineHasNoAttributeName);
            
		    int res = collectEnumValue(cc, enumDefine);
		    if(res < 0)
			    return releaseILObjectAndReturn(enumDefine, res);
            
		    ILParser::enumTypeList.push_back(enumDefine);


            if (std::find(ILParser::allDataTypeList.begin(), ILParser::allDataTypeList.end(),
			    "enum " + enumDefine->name) != ILParser::allDataTypeList.end())
			    return -ErrorTypeRedefined;
		    ILParser::allDataTypeList.push_back("enum " + enumDefine->name);

            //由于enum在生成代码时 采用typedef的形式声明，所以也在所有数据类型列表中加入不带enum前缀的类型
            if (std::find(ILParser::allDataTypeList.begin(), ILParser::allDataTypeList.end(),
			    enumDefine->name) != ILParser::allDataTypeList.end())
			    return -ErrorTypeRedefined;
	        ILParser::allDataTypeList.push_back(enumDefine->name);

            
		    cc = cc->NextSiblingElement("Enum");
	    }
		c = c->NextSiblingElement("File");
	}
	return 0;
}

int ILFormatter::collectEnumValue(const tinyxml2::XMLElement* root, ILEnumDefine* enumDefine)
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
		if(c->FindAttribute("Value"))
			enumValue->value = string(c->Attribute("Value"));
        

		enumDefine->enumValues.push_back(enumValue);
		
		c = c->NextSiblingElement("EnumValue");
	}
	return 1;
}

int ILFormatter::traverseRef(const tinyxml2::XMLElement* root, tinyxml2::XMLElement* rootFormat,
                             tinyxml2::XMLDocument* doc)
{
    const tinyxml2::XMLElement *c = root->FirstChildElement("ref");
	while (c)
	{
		setCurrentReadXMLElement(c);
		tinyxml2::XMLElement* rootFormatNew = (*doc).NewElement("Temp");
		rootFormat->InsertEndChild(rootFormatNew);

		int res = traverse(c, rootFormatNew, doc);
		if(res < 0)
			return res;


		c = c->NextSiblingElement("ref");
	}
    c = root->FirstChildElement("Ref");
	while (c)
	{
		setCurrentReadXMLElement(c);
		tinyxml2::XMLElement* rootFormatNew = (*doc).NewElement("Temp");
		rootFormat->InsertEndChild(rootFormatNew);

		int res = traverse(c, rootFormatNew, doc);
		if(res < 0)
			return res;


		c = c->NextSiblingElement("Ref");
	}
    return 0;
}

int ILFormatter::traverseFuncState(const tinyxml2::XMLElement* root, tinyxml2::XMLElement* rootFormat,
    tinyxml2::XMLDocument* doc)
{
    if(!(stringToLower(string((root->Name() == nullptr) ? "": root->Name())) == "function"))
	{
		return 0;
	}
    const tinyxml2::XMLElement *c = root->FirstChildElement("state");
	while (c)
	{
		setCurrentReadXMLElement(c);
		tinyxml2::XMLElement* rootFormatNew = (*doc).NewElement("Temp");
		rootFormat->InsertEndChild(rootFormatNew);

		int res = traverse(c, rootFormatNew, doc);
		if(res < 0)
			return res;


		c = c->NextSiblingElement("state");
	}
    c = root->FirstChildElement("State");
	while (c)
	{
		setCurrentReadXMLElement(c);
		tinyxml2::XMLElement* rootFormatNew = (*doc).NewElement("Temp");
		rootFormat->InsertEndChild(rootFormatNew);

		int res = traverse(c, rootFormatNew, doc);
		if(res < 0)
			return res;


		c = c->NextSiblingElement("State");
	}
    return 0;
}

int ILFormatter::traverse(const tinyxml2::XMLElement* root, tinyxml2::XMLElement* rootFormat, tinyxml2::XMLDocument* doc)
{
	int res = translate(root, rootFormat, doc);
	if(res < 0)
		return res;

	//优先转换ref
	traverseRef(root, rootFormat, doc);

	//对function中的state参数优先处理
	traverseFuncState(root, rootFormat, doc);
	
	const tinyxml2::XMLElement *c = root->FirstChildElement();
	while (c)
	{
        bool loopContinue = false;
        res = traverseA(root, rootFormat, doc, c, loopContinue);
		if(res != 0)
            return res;

		c = c->NextSiblingElement();
	}
	return 1;
}

int ILFormatter::traverseA(const tinyxml2::XMLElement* root, tinyxml2::XMLElement* rootFormat, tinyxml2::XMLDocument* doc, const tinyxml2::XMLElement* c, bool &loopContinue)
{
    setCurrentReadXMLElement(c);
	string name((c->Name() == nullptr) ? "": c->Name());
	if(stringToLower(string((root->Name() == nullptr) ? "": root->Name())) == "function" && stringToLower(name)== "state")
	{
		c = c->NextSiblingElement();
		loopContinue = true;
        return 0;
	}

	if(stringToLower(name) == "ref")
	{
		c = c->NextSiblingElement();
		loopContinue = true;
        return 0;
	}

	if(stringToLower(name) == "placeholder")
	{
		//tinyxml2::XMLElement *c2 = c->FirstChildElement();
		
		tinyxml2::XMLElement* rootFormatNew = (*doc).NewElement("Temp");
		int res = traverse(c, rootFormatNew, doc);
		if(res < 0)
			return res;
		tinyxml2::XMLElement *cc = rootFormatNew->FirstChildElement();
		while (cc)
		{
			setCurrentReadXMLElement(cc);
			tinyxml2::XMLElement *ccOri = cc;
			cc = cc->NextSiblingElement();
			rootFormat->InsertEndChild(ccOri);
		}
		(*doc).DeleteNode(rootFormatNew);

		

		/*while (c2)
		{
			tinyxml2::XMLElement* rootFormatNew = (*doc).NewElement("Temp");

			res = traverse(c2, rootFormatNew, doc);
			if(res < 0)
				return res;

			rootFormat->InsertEndChild(rootFormatNew);

			c2 = c2->NextSiblingElement();
		}*/
	}
	else
	{
		tinyxml2::XMLElement* rootFormatNew = (*doc).NewElement("Temp");
		rootFormat->InsertEndChild(rootFormatNew);

		int res = traverse(c, rootFormatNew, doc);
		if(res < 0)
			return res;

	}
    return 0;
}

std::map<std::string, std::string> ILFormatter::transNameMap = {
    pair<string, string>("IRRoot",    "Root"),          pair<string, string>("file",     "File"), 
    pair<string, string>("headfile",  "HeadFile"),      pair<string, string>("macro",    "Macro"), 
    pair<string, string>("typedef",   "TypeDefine"),    pair<string, string>("record",   "Struct"), 
    pair<string, string>("ref",       "Ref"),           pair<string, string>("inport",   "Input"), 
    pair<string, string>("outport",   "Output"),        pair<string, string>("schedule", "Schedule"), 
    pair<string, string>("calculate", "Calculate"),     pair<string, string>("if",       "If"), 
    pair<string, string>("else",      "Else"),          pair<string, string>("elseif",   "ElseIf"),
    pair<string, string>("while",     "While"),         pair<string, string>("dowhile",  "DoWhile"),
    pair<string, string>("for",       "For"),           pair<string, string>("continue", "Continue"),
    pair<string, string>("switch",    "Switch"),        pair<string, string>("case",     "Case"),
    pair<string, string>("default",   "Default"),       pair<string, string>("break",    "Break"),
    pair<string, string>("initial",   "Initial"),       pair<string, string>("iterator", "Iterator"),
    pair<string, string>("config",    "Config"),        pair<string, string>("function", "Function"),  
    pair<string, string>("return",    "Return"),        pair<string, string>("branch",   "Branch"), 
    pair<string, string>("member",    "Member"),        pair<string, string>("state",    "Input"),
    pair<string, string>("expression",          "Expression"),
    pair<string, string>("localvariable",       "LocalVariable"),
    pair<string, string>("localvariable",       "LocalVariable"),
    pair<string, string>("bracestatement",      "BraceStatement"),  
    pair<string, string>("iterationcount",      "IterationCount"),
    pair<string, string>("globalvariable",      "GlobalVariable"), 
    pair<string, string>("utilityfunction",     "UtilityFunction"),
    pair<string, string>("maininitfunction",    "MainInitFunction"),
    pair<string, string>("mainexecfunction",    "MainExecFunction"),
    pair<string, string>("maincompositestate",  "MainCompositeState"),
    
    pair<string, string>("Root",      "Root"),          pair<string, string>("File",     "File"), 
    pair<string, string>("HeadFile",  "HeadFile"),      pair<string, string>("Macro",    "Macro"), 
    pair<string, string>("TypeDefine","TypeDefine"),    pair<string, string>("Struct",   "Struct"), 
    pair<string, string>("Typedef",   "TypeDefine"),
    pair<string, string>("Ref",       "Ref"),           pair<string, string>("Input",    "Input"), 
    pair<string, string>("Output",    "Output"),        pair<string, string>("Schedule", "Schedule"), 
    pair<string, string>("Calculate", "Calculate"),     pair<string, string>("If",       "If"), 
    pair<string, string>("Else",      "Else"),          pair<string, string>("ElseIf",   "ElseIf"),
    pair<string, string>("While",     "While"),         pair<string, string>("DoWhile",  "DoWhile"),
    pair<string, string>("For",       "For"),           pair<string, string>("Continue", "Continue"),
    pair<string, string>("Switch",    "Switch"),        pair<string, string>("Case",     "Case"),
    pair<string, string>("Default",   "Default"),       pair<string, string>("Break",    "Break"),
    pair<string, string>("Initial",   "Initial"),       pair<string, string>("Iterator", "Iterator"),
    pair<string, string>("Config",    "Config"),        pair<string, string>("Function", "Function"),  
    pair<string, string>("Return",    "Return"),        pair<string, string>("Branch",   "Branch"), 
    pair<string, string>("Member",    "Member"),        pair<string, string>("Input",    "Input"),
    pair<string, string>("State",    "Input"),
    pair<string, string>("Expression",          "Expression"),
    pair<string, string>("LocalVariable",       "LocalVariable"),
    pair<string, string>("LocalVariable",       "LocalVariable"),
    pair<string, string>("BraceStatement",      "BraceStatement"),  
    pair<string, string>("IterationCount",      "IterationCount"),
    pair<string, string>("GlobalVariable",      "GlobalVariable"), 
    pair<string, string>("UtilityFunction",     "UtilityFunction"),
    pair<string, string>("MainInitFunction",    "MainInitFunction"),
    pair<string, string>("MainExecFunction",    "MainExecFunction"),
    pair<string, string>("MainCompositeState",  "MainCompositeState"),
    pair<string, string>("Iterationcount",      "IterationCount"),
    pair<string, string>("Maininitfunction",    "MainInitFunction"),
    pair<string, string>("Mainexecfunction",    "MainExecFunction"),
    pair<string, string>("Maincompositestate",  "MainCompositeState"),
    pair<string, string>("Enum",                "Enum"),
    pair<string, string>("EnumValue",           "EnumValue"),
    pair<string, string>("BatchCalculation",    "BatchCalculation"),
    pair<string, string>("Assemble",            "Assemble"),
    pair<string, string>("ASM",                 "ASM"),
};
std::map<std::string, std::string> ILFormatter::transAttrMap = {
    pair<string, string>("name",           "Name"),        pair<string, string>("actor",     "Actor"),
    pair<string, string>("value",          "Value"),       pair<string, string>("type",      "Type"), 
    pair<string, string>("path",           "Path"),        pair<string, string>("isaddress", "IsAddress"), 
    pair<string, string>("arraysize",      "ArraySize"),   pair<string, string>("source",    "Source"), 
    pair<string, string>("expression",     "Expression"),  pair<string, string>("condition", "Condition"), 
    pair<string, string>("vondition",      "Condition"),   pair<string, string>("alias",     "Value"), 
    pair<string, string>("isoptimized",    "IsOptimized"), pair<string, string>("datatype",  "DataType"), 
    pair<string, string>("defaultvalue",   "DefaultValue"), 
    pair<string, string>("isinitialstate", "IsInitialState"),
    pair<string, string>("isfinalstate",   "IsFinalState"),
    
    pair<string, string>("Name",           "Name"),        pair<string, string>("Actor",     "Actor"),
    pair<string, string>("Value",          "Value"),       pair<string, string>("Type",      "Type"), 
    pair<string, string>("Path",           "Path"),        pair<string, string>("IsAddress", "IsAddress"), 
    pair<string, string>("ArraySize",      "ArraySize"),   pair<string, string>("Source",    "Source"), 
    pair<string, string>("Expression",     "Expression"),  pair<string, string>("Condition", "Condition"), 
    pair<string, string>("Condition",      "Condition"),   pair<string, string>("Value",     "Value"), 
    pair<string, string>("IsOptimized",    "IsOptimized"), pair<string, string>("DataType",  "DataType"), 
    pair<string, string>("DefaultValue",   "DefaultValue"),pair<string, string>("Alias",     "Value"), 
    pair<string, string>("IsInitialState", "IsInitialState"),
    pair<string, string>("IsFinalState",   "IsFinalState"), 
    pair<string, string>("IsStatic",       "IsStatic"), 
    pair<string, string>("isStatic",       "IsStatic"), 
    pair<string, string>("isstatic",       "IsStatic"), 
    pair<string, string>("retType",        "ReturnType"), 
    pair<string, string>("ReturnType",     "ReturnType"), 
    pair<string, string>("returnType",     "ReturnType"), 
    pair<string, string>("returntype",     "ReturnType"), 
    pair<string, string>("IsIndependent",  "IsIndependent"), 
    pair<string, string>("isIndependent",  "IsIndependent"), 
    pair<string, string>("isindependent",  "IsIndependent"), 
    pair<string, string>("isHeadFile",     "IsHeadFile"), 
    pair<string, string>("isheadFile",     "IsHeadFile"), 
    pair<string, string>("isheadfile",     "IsHeadFile"), 
    pair<string, string>("external",       "IsExternal"), 
    pair<string, string>("isexternal",     "IsExternal"), 
    pair<string, string>("isExternal",     "IsExternal"), 
    pair<string, string>("IsExternal",     "IsExternal"), 
    pair<string, string>("Operation",      "Operation"), 
    pair<string, string>("DataType",       "DataType"), 
    pair<string, string>("Modifier",       "Modifier"), 
    pair<string, string>("Shape",          "Shape"), 
    pair<string, string>("operation",      "Operation"), 
    pair<string, string>("dataType",       "DataType"), 
    pair<string, string>("datatype",       "DataType"), 
    pair<string, string>("modifier",       "Modifier"), 
    pair<string, string>("shape",          "Shape"), 
    pair<string, string>("xmlns:xsi",      "xmlns:xsi"), 
    pair<string, string>("xsi:noNamespaceSchemaLocation",      "xsi:noNamespaceSchemaLocation"), 
    pair<string, string>("Align",          "Align"), 
    pair<string, string>("align",          "Align"), 
    pair<string, string>("isVolatile",          "IsVolatile"), 
    pair<string, string>("IsVolatile",          "IsVolatile"), 
};
std::map<std::string, std::string> ILFormatter::transValueMap = {
        pair<string, string>("if",      "If"),      pair<string, string>("else",    "Else"),
        pair<string, string>("elseif",  "ElseIf"),  pair<string, string>("for",     "For"),
        pair<string, string>("while",   "While"),
    
        pair<string, string>("If",      "If"),      pair<string, string>("Else",    "Else"),
        pair<string, string>("ElseIf",  "ElseIf"),  pair<string, string>("For",     "For"),
        pair<string, string>("While",   "While"),

        pair<string, string>("FunctionTrans",      "FunctionExec"),
        pair<string, string>("FunctionStep",    "FunctionExec"),
};
int ILFormatter::translate(const tinyxml2::XMLElement* root, tinyxml2::XMLElement* rootFormat, tinyxml2::XMLDocument* doc) const
{
    int res;
	string name((root->Name() == nullptr) ? "": root->Name());
    if(ILFormatter::transNameMap.find(name) != ILFormatter::transNameMap.end()){
        name = ILFormatter::transNameMap[name];
        if(name == "Struct" &&
            (root->FindAttribute("Kind") && string(root->Attribute("Kind")) == "enum" ||
            root->FindAttribute("kind") && string(root->Attribute("kind")) == "enum")) {
            name = "Enum";
        }
        else if(name == "Member")
        {
            tinyxml2::XMLElement* p = static_cast<tinyxml2::XMLElement*>(rootFormat->Parent());
            name = (p && string(p->Name()) == "Enum") ? "EnumValue" : name;
        }
        else if(name == "Function")
        {
            if(!root->FirstChildElement("Schedule")) {
                tinyxml2::XMLElement* rootFormatNew = (*doc).NewElement("Schedule");
		        rootFormat->InsertEndChild(rootFormatNew);
            }
        }

    }else if(name == "placeholder" || name == "Placeholder") {
		ILCCodeParser codeParser;
		Statement retStatement;
		string code;
		if(root->FindAttribute("code")){
			code = string(root->Attribute("code"));
		}else{
			return -ILFormatter::ErrorPlaceHolderMissCodeAttribute;
        }
		if ((res = codeParser.parseCCode(code, &retStatement)) < 0) {
			cout << "Parse C Code Error: " << res << "\n";
			cout << "Code: " << code << "\n";
			return res;
		}
		ILSaver iLSaver;
		for(int i = 0; i <retStatement.childStatements.size(); i++) {
			if ((res = iLSaver.saveStatement(retStatement.childStatements[i],rootFormat, doc)) < 0) {
				cout << "Save C Code To XML Error: " << res << "\n";
				return res;
			}
		}
	} else {
		//name = "UnFormated" + name;
	}
	rootFormat->SetName(name.c_str());
	const tinyxml2::XMLAttribute* attribut = root->FirstAttribute();
	while(attribut){
        if((res = translateAttribute(attribut, name, rootFormat, doc)) < 0)
            return res;
		attribut = attribut->Next();
	}
	return 1;
}

int ILFormatter::translateAttribute(const tinyxml2::XMLAttribute* attribut, string name, tinyxml2::XMLElement* rootFormat,
    tinyxml2::XMLDocument* doc) const
{
    string attributeName = string(attribut->Name());
    if((attributeName == "type" || attributeName == "Type") && name == "TypeDefine"){
		attributeName = "Name";
    }
	if((attributeName == "value" || attributeName == "Value") &&
        (name == "GlobalVariable" || name == "Input" || name == "Output" ||
        name == "LocalVariable")){
		attributeName = "DefaultValue";
    }else if(ILFormatter::transAttrMap.find(attributeName) != ILFormatter::transAttrMap.end()){
        attributeName = ILFormatter::transAttrMap[attributeName];
    }else if((attributeName == "kind" || attributeName == "Kind") && (name == "Function" || name == "Branch")){
		attributeName = "Type";
    }else if((attributeName == "kind" || attributeName == "Kind")){
        attributeName = "Kind_Unused";
    } else {
		//attributeName = "UnFormated" + attributeName;
    }
	string value = string(attribut->Value());
	if(attributeName == "Type" && ILFormatter::transValueMap.find(value) != ILFormatter::transValueMap.end()) {
        value = ILFormatter::transValueMap[value];
	}
    if(name == "File" && attributeName == "Name" && stringEndsWith(value, ".h"))
    {
        rootFormat->SetAttribute("IsIndependent", "true");
        rootFormat->SetAttribute("IsHeadFile", "true");
        value = value.substr(0, value.length() - CONST_NUM_2);
    }
	rootFormat->SetAttribute(attributeName.c_str(), value.c_str());
    
    return 1;
}

int ILFormatter::deduplication(const ILParser* parser) const
{
    int res;
    for(int i = 0; i < parser->files.size(); i++)
    {
        res = deduplicationHeadFile(parser->files[i], parser);
        if(res < 0)
            return res;
        res = deduplicationMacro(parser->files[i], parser);
        if(res < 0)
            return res;
    }
    return 1;
}

int ILFormatter::deduplicationHeadFile(const ILFile* file, const ILParser* parser) const
{
    for(int j = file->headFiles.size() - 1; j >= 0; j--)
    {
        for(int k = j - 1; k >= 0; k--)
        {
            if(file->headFiles[j]->name == file->headFiles[k]->name)
            {
                parser->releaseILObject(file->headFiles[j]);
                break;
            }
        }
    }
    return 1;
}

int ILFormatter::deduplicationMacro(const ILFile* file, const ILParser* parser) const
{
    for(int j = file->macros.size() - 1; j >= 0; j--)
    {
        for(int k = j - 1; k >= 0; k--)
        {
            if(file->macros[j]->name == file->macros[k]->name)
            {
                parser->releaseILObject(file->macros[j]);
                break;
            }
        }
    }
    return 1;
}

int ILFormatter::renameBatchCalculationTempVariable(const ILParser* parser)
{
    for(int i = 0; i < parser->files.size(); i++)
    {
        for(int j = 0; j < parser->files[i]->functions.size(); j++)
        {
            ILSchedule* iLSchedule = parser->files[i]->functions[j]->schedule;
            int res = renameBatchCalculationTraverseScheduleNode(iLSchedule);
            if(res < 0) {
                return res;
            }
        }
    }
    return 1;
}

int ILFormatter::renameBatchCalculationTraverseScheduleNode(ILSchedule* schedule)
{
    renameBatchCalculationCurrentSchedule = schedule;
    for(int i = 0; i < schedule->scheduleNodes.size(); i++)
    {
        ILScheduleNode* scheduleNode = schedule->scheduleNodes[i];
        if(scheduleNode->objType == ILObject::TypeBranch)
        {
            int res = renameBatchCalculationTraverseBranch(reinterpret_cast<ILBranch*>(scheduleNode));
            if(res < 0)
                return res;
        }
        else
        {
            int res = renameBatchCalculationTraverseCalculate(reinterpret_cast<ILCalculate*>(scheduleNode));
            if(res < 0)
                return res;
        }
    }
    return 1;
}

int ILFormatter::renameBatchCalculationTraverseBranch(const ILBranch* branch)
{
    for(int i = 0; i < branch->scheduleNodes.size(); i++)
    {
        ILScheduleNode* scheduleNode = branch->scheduleNodes[i];
        if(scheduleNode->objType == ILObject::TypeBranch)
        {
            int res = renameBatchCalculationTraverseBranch(reinterpret_cast<ILBranch*>(scheduleNode));
            if(res < 0)
                return res;
        }
        else
        {
            int res = renameBatchCalculationTraverseCalculate(reinterpret_cast<ILCalculate*>(scheduleNode));
            if(res < 0)
                return res;
        }
    }
    return 1;
}

int ILFormatter::renameBatchCalculationTraverseCalculate(const ILCalculate* calculate)
{
    renameBatchCalculationCurrentCalculate = const_cast<ILCalculate*>(calculate);
    batchCalculationNameToRealName.clear();
    for(int i = 0 ; i < calculate->statements.childStatements.size(); i++)
    {
        int res = renameBatchCalculationTraverseStatement(calculate->statements.childStatements[i]);
        if(res < 0)
            return res;
    }
    return 1;
}

int ILFormatter::renameBatchCalculationTraverseStatement(Statement* statement)
{
    if(statement->type == Statement::BatchCalculation)
    {
        StmtBatchCalculation* stmt = static_cast<StmtBatchCalculation*>(statement);
        for(int i = 0; i < stmt->inputs.size(); i++)
        {
            int res = renameBatchCalculationInputVariable(&(stmt->inputs[i]), stmt);
            if(res < 0)
                return res;
        }
        for(int i = 0; i < stmt->outputs.size(); i++)
        {
            int res = renameBatchCalculationOutputVariable(&(stmt->outputs[i]), stmt);
            if(res < 0)
                return res;
        }
        return 1;
    }
    for(int i = 0 ; i < statement->childStatements.size(); i++)
    {
        int res = renameBatchCalculationTraverseStatement(statement->childStatements[i]);
        if(res < 0)
            return res;
    }
    return 1;
}

int ILFormatter::renameBatchCalculationInputVariable(
    StmtBatchCalculation::StmtBatchCalculationInput* input,
    const StmtBatchCalculation* stmt)
{
    if(!(input->name[0] >= '0' && input->name[0] <= '9'))
    {
        return 0;
    }
    if(batchCalculationNameToRealName.find(input->name) != batchCalculationNameToRealName.end())
    {
        input->name = batchCalculationNameToRealName.at(input->name);
        return 0;
    }

    string tempVarName = ILParser::getAvailableVariableName("batchTempVar", stmt->getDomain());
    ILParser::registerVariableName(tempVarName, stmt->getDomain());
    ILLocalVariable* tempILVar = new ILLocalVariable();
    tempILVar->name = tempVarName;
    tempILVar->arraySize = input->arraySize;
    if(input->defaultValue)
        tempILVar->defaultValue = input->defaultValue->clone();
    tempILVar->isAddress = input->isAddress;
    tempILVar->type = input->type;
    tempILVar->parent = renameBatchCalculationCurrentSchedule;
    for(int j = 0; j < renameBatchCalculationCurrentCalculate->refs.size(); j++)
    {
        tempILVar->refs.push_back(renameBatchCalculationCurrentCalculate->refs[j]->clone(tempILVar));
    }
    renameBatchCalculationCurrentSchedule->localVariables.push_back(tempILVar);
    batchCalculationNameToRealName[input->name] = tempVarName;
    input->name = tempVarName;
    return 1;
}

int ILFormatter::renameBatchCalculationOutputVariable(
    StmtBatchCalculation::StmtBatchCalculationOutput* output,
    const StmtBatchCalculation* stmt)
{
    if(!(output->name[0] >= '0' && output->name[0] <= '9'))
    {
        return 0;
    }
    if(batchCalculationNameToRealName.find(output->name) != batchCalculationNameToRealName.end())
    {
        output->name = batchCalculationNameToRealName.at(output->name);
        return 0;
    }

    string tempVarName = ILParser::getAvailableVariableName("batchTempVar", stmt->getDomain());
    ILParser::registerVariableName(tempVarName, stmt->getDomain());
    ILLocalVariable* tempILVar = new ILLocalVariable();
    tempILVar->name = tempVarName;
    tempILVar->arraySize = output->arraySize;
    if(output->defaultValue)
        tempILVar->defaultValue = output->defaultValue->clone();
    tempILVar->isAddress = output->isAddress;
    tempILVar->type = output->type;
    tempILVar->parent = renameBatchCalculationCurrentSchedule;
    for(int j = 0; j < renameBatchCalculationCurrentCalculate->refs.size(); j++)
    {
        tempILVar->refs.push_back(renameBatchCalculationCurrentCalculate->refs[j]->clone(tempILVar));
    }
    renameBatchCalculationCurrentSchedule->localVariables.push_back(tempILVar);
    batchCalculationNameToRealName[output->name] = tempVarName;
    output->name = tempVarName;
    return 1;
}

int ILFormatter::releaseILObjectAndReturn(ILObject* obj, int returnValue) const
{
    obj->release();
    delete obj;
    return returnValue;
}
