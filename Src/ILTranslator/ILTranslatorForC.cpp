#include "ILTranslatorForC.h"

#include <fstream>

#include "ILTransFile.h"
#include "ILTransMain.h"

#include "../Common/Utility.h"
#include "../ILBasic/ILParser.h"
#include "../ILBasic/ILFile.h"
#include "../ILBasic/ILRef.h"
#include "../ILBasic/ILExternFile.h"
#include "ILTransExternFile.h"
#include "ILTranslator.h"


using namespace std;

bool ILTranslatorForC::configGenerateComment = true;
bool ILTranslatorForC::configTestTime = false;
bool ILTranslatorForC::configPackageVariable = true;
bool ILTranslatorForC::configPackageParameter = true;
bool ILTranslatorForC::configTestCorrectness = false;
std::string ILTranslatorForC::configTargetDevice = "Intel";

ILParser* ILTranslatorForC::iLParser;

namespace 
{
    const int CONST_NUM_2 = 2;
}

int ILTranslatorForC::translate(ILParser* iLParser, string outputDir)
{
    ILTranslator::iLParser = iLParser;
	this->iLParser = iLParser;
	int res;
	for(int i = 0; i < iLParser->files.size(); i++) {
		string name = iLParser->files[i]->name + (iLParser->files[i]->isHeadFile ? ".h" : ".c");
		string code;
		ILTransFile transFile;
		if((res = transFile.translate(iLParser->files[i], code)) < 0)
			return res;

		code = codeIndentProcess(code);
        code = "#include \"define.h\"\n" + code;
		fileCodes[name] = stringTrim(code);
	}

    for(int i = 0; i < iLParser->externFiles.size(); i++) {
		string name = iLParser->externFiles[i]->name;
		string code;
		ILTransExternFile transExternFile;
		if((res = transExternFile.translate(iLParser->externFiles[i], code)) < 0)
			return res;
        
		fileCodes[name] = stringTrim(code);
	}

	string mainCode;
	ILTransMain iLTransMain;
	if((res = iLTransMain.translate(iLParser, mainCode)) < 0) {
		ILTranslator::setCurrentError(res, "");
		return res;
	}
	mainCode = codeIndentProcess(mainCode);
    mainCode = "#include \"define.h\"\n" + mainCode;
	fileCodes["main.c"] = stringTrim(mainCode);
	string defineCode = ILTranslator::transCodeHeadFile;
	defineCode += configTestTime ? getConfigTestRedefineOutputCode() : "";
	defineCode += ILTranslator::transCodeMacro;
	defineCode += ILTranslator::transCodeTypeDefine;
	defineCode += ILTranslator::transCodeEnumDefine;
	defineCode += ILTranslator::transCodeStructDefine;
	defineCode += ILTranslator::transCodeGlobalVariable;
	defineCode += ILTranslator::transCodeUtilityFunction;
	defineCode += ILTranslator::transCodeFunctionDefine;
	defineCode += ILTranslator::transCodeFunction;
	defineCode = codeIndentProcess(defineCode);
	defineCode = stringTrim(defineCode);
	if(!defineCode.empty()) {
		defineCode = "#ifndef _DEFINE_H_\n#define _DEFINE_H_\n" + defineCode + "\n#endif\n";
		//cout << " ---- define.h ----\n";
		//cout << defineCode << "\n\n";
		writeFile(outputDir + "/define.h", defineCode);
	}
    if(!ILTranslator::transCodeUtilityFunctionImplement.empty()) {
        string utilityFunctionImplementCode = codeIndentProcess("#include \"define.h\"\n" + ILTranslator::transCodeUtilityFunctionImplement);
        writeFile(outputDir + "/define.c", utilityFunctionImplementCode);
    }
	for(map<string, string>::iterator iter = fileCodes.begin(); iter!=fileCodes.end();iter++) {
		//if(!defineCode.empty())
		//	iter->second = "#include \"define.h\"\n" + iter->second;

		//cout << " ---- " << iter->first << ".cpp ----\n";
		//cout << iter->second << "\n\n";
		writeFile(outputDir + "/" + iter->first, iter->second);
	}
	return 1;
}

void ILTranslatorForC::setConfigGenerateComment(bool value)
{
    configGenerateComment = value;
}

void ILTranslatorForC::setConfigTestTime(bool value)
{
    configTestTime = value;
}

void ILTranslatorForC::setConfigTestCorrectness(bool value)
{
    configTestCorrectness = value;
}

void ILTranslatorForC::setConfigPackageVariable(bool value)
{
	configPackageVariable = value;
}

void ILTranslatorForC::setConfigTargetDevice(std::string device)
{
    configTargetDevice = device;
}

void ILTranslatorForC::setConfigPackageParameter(bool value)
{
	configPackageParameter = value;
}

std::string ILTranslatorForC::getRefComment(const std::vector<ILRef*>* refList)
{
    if(!refList)
        return "";
    string commentStr;
    for(int i = 0; i < refList->size(); i++)
    {
        ILRef* ref = (*refList)[i];
        vector<string> actorStrSp = stringSplit(ref->actor, ".");
        string actorStr = "";
        if(!actorStrSp.empty())
            actorStr = actorStrSp.back();
        if(i != 0)
            commentStr += ",\n";
        if (!actorStr.empty() && !ref->path.empty())
            commentStr += actorStr + " named \"" + ref->path + "\"";
        else if(!actorStr.empty())
            commentStr += actorStr;
        else if(!ref->path.empty())
            commentStr += "\"" + ref->path + "\"";
        else if(i != 0)
            commentStr = commentStr.substr(0, commentStr.length() - CONST_NUM_2);
    }
    if(stringTrim(commentStr).empty())
        return "";
    if(stringSplit(commentStr, "\n").size() > 1)
        commentStr = "\n" + commentStr;
    return commentStr;
}

string ILTranslatorForC::getConfigTestRedefineOutputCode() const
{
    //这些函数用0替换是为了保证替换后C语法的正确性
    string ret;
    ret += "#include <time.h>\n";
    ret += "#include <stdio.h>\n";
    ret += "#define fputc(c, stream) 0\n";
    ret += "#define fputs(str, stream) 0\n";
    ret += "#define putc(c, stream) 0\n";
    ret += "#define putchar(c) 0\n";
    ret += "#define puts(str) 0\n";
    ret += "#define vfprintf(stream, format, ...) 0\n";
    ret += "#define printf(format, ...) 0\n";
    ret += "#define fprintf(stream, format, ...) 0\n";
    ret += "#define fopen(filename, mode) 0\n";
    ret += "#define fclose(fp) 0\n";
    ret += "#define fread(ptr, size, nmemb, stream) 0\n";
    ret += "#define fwrite(ptr, size, nmemb, stream) 0\n";
    return ret;
}
