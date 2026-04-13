#pragma once
#include <string>

class ILFile;
class ILTransFile
{
public:
    int translate(ILFile* file, std::string& code);

    ILFile* file = nullptr;

    //Only usd by TranslatorForC
    bool isIndependent = false; // 文件是否独立生成

	std::string transCodeHeadFile;
	std::string transCodeStructDefine;
	std::string transCodeEnumDefine;
	std::string transCodeMacro;
	std::string transCodeTypeDefine;
	std::string transCodeGlobalVariable;
	std::string transCodeUtilityFunction;
	std::string transCodeFunction;
};
