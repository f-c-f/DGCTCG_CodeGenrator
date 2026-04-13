#include "ILTranslatorForLibFuzzer.h"

#include <fstream>

#include "ILTransFile.h"
#include "ILTransMain.h"

#include "../Common/Utility.h"
#include "../ILBasic/ILParser.h"
#include "../ILBasic/ILFile.h"
#include "../ILBasic/ILRef.h"
#include "../ILBasic/ILExternFile.h"
#include "../TCGBasic/TCGConfig.h"
#include "ILTransExternFile.h"
#include "ILTransForLibFuzzerTestCaseConverter.h"
#include "ILTranslator.h"

#include <algorithm>

#include "ILTransInstrumentBitmapHelper.h"
#include "ILTranslatorUtility.h"
#include "ILTransMetadataExporter.h"

using namespace std;

bool ILTranslatorForLibFuzzer::configGenerateComment = true;
int ILTranslatorForLibFuzzer::configCutOffArray = true;
bool ILTranslatorForLibFuzzer::configRangeConstraint = false;
bool ILTranslatorForLibFuzzer::configTargetModelPara = false;
bool ILTranslatorForLibFuzzer::configGenTestCaseConverter = true;
int ILTranslatorForLibFuzzer::configMaxIteratorCount = -1;


ILParser* ILTranslatorForLibFuzzer::iLParser;
TCGConfig* ILTranslatorForLibFuzzer::tcgConfig;

namespace 
{
    const int CONST_NUM_2 = 2;
}

ILTranslatorForLibFuzzer::ILTranslatorForLibFuzzer(ILParser* iLParser, TCGConfig* tcgConfig)
{
    ILTranslator::iLParser = iLParser;
	this->iLParser = iLParser;
	this->tcgConfig = tcgConfig;
}

int ILTranslatorForLibFuzzer::translate(string outputDir)
{
    ILTransMetadataExporter::reset();
    ILTranslatorUtility::buildILFunctionCallGraph(iLParser);

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
    defineCode += "#include \"LibFuzzerUtility.h\"\n";
	//defineCode += getRedefineOutputCode();
	defineCode += ILTranslator::transCodeMacro;
    defineCode += "#define BRANCH_COVERAGE_COUNT " + to_string(ILTransInstrumentBitmapHelper::branchBitmapCount) + "\n";
	defineCode += ILTranslator::transCodeTypeDefine;
	defineCode += ILTranslator::transCodeEnumDefine;
	defineCode += ILTranslator::transCodeStructDefine;
	defineCode += ILTranslator::transCodeGlobalVariable;
    int branchArraySize = ILTransInstrumentBitmapHelper::branchBitmapCount;
    branchArraySize = branchArraySize == 0 ? 1 : branchArraySize;
    defineCode += "extern unsigned char BranchCoverageLibFuzzer[" + to_string(branchArraySize) + "];\n";
    defineCode += "extern long long BranchCoverageStartTime;\n";
    defineCode += ILTranslator::transCodeUtilityFunction;
	defineCode += ILTranslator::transCodeFunctionDefine;
	defineCode += ILTranslator::transCodeFunction;
	defineCode = codeIndentProcess(defineCode);
	defineCode = stringTrim(defineCode);


	if(!defineCode.empty()) {
		defineCode = "#ifndef _DEFINE_H_\n#define _DEFINE_H_\n" + defineCode + "\n#endif\n";
		writeFile(outputDir + "/define.h", defineCode);
	}
    if(!ILTranslator::transCodeUtilityFunctionImplement.empty()) {
        string utilityFunctionImplementCode = codeIndentProcess("#include \"define.h\"\n" + ILTranslator::transCodeUtilityFunctionImplement);
        writeFile(outputDir + "/define.c", utilityFunctionImplementCode);
    }
	for(map<string, string>::iterator iter = fileCodes.begin(); iter!=fileCodes.end();iter++) {
		writeFile(outputDir + "/" + iter->first, iter->second);
	}
    
	string exeFilePath = getExeDirPath();
    //if(ILTranslatorForLibFuzzer::configRangeConstraint)
    {
        if (!isFileExist(exeFilePath + "/ResourceCode/" + "LibFuzzerUtility.h"))
		    return -ILTranslator::ErrorResourceFileNotExist;
	    copyFileOne(exeFilePath + "/ResourceCode/" + "LibFuzzerUtility.h", outputDir + "/" + "LibFuzzerUtility.h");
	    if (!isFileExist(exeFilePath + "/ResourceCode/" + "LibFuzzerUtility.c"))
		    return -ILTranslator::ErrorResourceFileNotExist;
	    copyFileOne(exeFilePath + "/ResourceCode/" + "LibFuzzerUtility.c", outputDir + "/" + "LibFuzzerUtility.c");
    }
	
    createFolder(outputDir + "/" + "In");
    createFolder(outputDir + "/" + "Out");
    createFolder(outputDir + "/" + "OutFuzz");
    createFolder(outputDir + "/" + "TestCaseOutput");
    createFolder(outputDir + "/" + "TestCaseOfFuzzer");

    generateOneKeyBuildScript(outputDir);

    if (ILTranslatorForLibFuzzer::configGenTestCaseConverter) {
        ILTransForLibFuzzerTestCaseConverter transTCC(iLParser);
        res = transTCC.translate(outputDir);
        if (res < 0)
            return res;
    }

    ILTransMetadataExporter::flush(outputDir, iLParser);
	return 1;
}

void ILTranslatorForLibFuzzer::setConfigGenerateComment(bool value)
{
    configGenerateComment = value;
}


void ILTranslatorForLibFuzzer::setConfigCutOffArray(int value)
{
    configCutOffArray = value;
    if(configCutOffArray == 0)
        configCutOffArray = 1 << 30;
}

void ILTranslatorForLibFuzzer::setConfigRangeConstraint(bool value)
{
    configRangeConstraint = value;
}

void ILTranslatorForLibFuzzer::setConfigTargetModelPara(bool value)
{
    configTargetModelPara = value;
}

void ILTranslatorForLibFuzzer::setConfigGenTestCaseConverter(bool value)
{
    configGenTestCaseConverter = value;
}

void ILTranslatorForLibFuzzer::setConfigMaxIteratorCount(int value)
{
    configMaxIteratorCount = value;
}


string ILTranslatorForLibFuzzer::getRedefineOutputCode() const
{
    //??????????0??I?????????I??C?????????
    string ret;
    //ret += "#include <time.h>\n";
    //ret += "#include <stdio.h>\n";
    //ret += "#define fputc(c, stream) 0\n";
    //ret += "#define fputs(str, stream) 0\n";
    ret += "#define putc(c, stream) 0\n";
    ret += "#define putchar(c) 0\n";
    ret += "#define puts(str) 0\n";
    //ret += "#define vfprintf(stream, format, ...) 0\n";
    ret += "#define printf(format, ...) 0\n";
    //ret += "#define fprintf(stream, format, ...) 0\n";
    //ret += "#define fopen(filename, mode) 0\n";
    //ret += "#define fclose(fp) 0\n";
    //ret += "#define fread(ptr, size, nmemb, stream) 0\n";
    //ret += "#define fwrite(ptr, size, nmemb, stream) 0\n";
    return ret;
}

int ILTranslatorForLibFuzzer::generateOneKeyBuildScript(std::string outputDir) const
{
    string scriptStr;
    scriptStr = "clang++ -O2 -fsanitize=fuzzer -Wl,/stack:104857600";
    auto iter = fileCodes.begin();
    for( ;iter != fileCodes.end(); iter++)
    {
        scriptStr += " " + iter->first;
    }
    //if(ILTranslatorForLibFuzzer::configRangeConstraint)
    {
        scriptStr += " LibFuzzerUtility.c";
    }
    scriptStr += " -o Fuzzer.exe\n";
    scriptStr += "del Fuzzer.exp\n";
    scriptStr += "del Fuzzer.lib\n";
    scriptStr += "del Fuzzer.pdb\n";

    writeFile(outputDir + "/Build.bat", scriptStr);

    scriptStr = "Fuzzer.exe -shrink=1 In\n";
    writeFile(outputDir + "/Run.bat", scriptStr);

    return 1;
}


std::string ILTranslatorForLibFuzzer::getParameterTypeMin(std::string type, std::string min)
{
    std::string ret = min;
    if (type[0] == 'u' && min.empty())
    {
        ret = to_string(0);
    }
    else if (type == "i8" && min.empty())
    {
        ret = to_string(INT8_MIN_VALUE);
    }
    else if (type == "i16" && min.empty())
    {
        ret = to_string(INT16_MIN_VALUE);
    }
    else if (type == "i32" && min.empty())
    {
        ret = to_string(INT32_MIN_VALUE);
    }
    else if (type == "i64" && min.empty())
    {
        ret = to_string(INT64_MIN_VALUE);
    }
    else if (type == "f32" && min.empty())
    {
        ret = to_string(FLT_MIN_VALUE);

    }
    else if (type == "f64" && min.empty())
    {
        ret = to_string(DBL_MIN_VALUE);
    }
    return ret;
}

std::string ILTranslatorForLibFuzzer::getParameterTypeMax(std::string type, std::string max)
{
    std::string ret = max;
    if (type == "u8" && max.empty())
    {
        ret = to_string(UINT8_MAX_VALUE);
    }
    else if (type == "u16" && max.empty())
    {
        ret = to_string(UINT16_MAX_VALUE);
    }
    else if (type == "u32" && max.empty())
    {
        ret = to_string(UINT32_MAX_VALUE);
    }
    else if (type == "u64" && max.empty())
    {
        ret = to_string(UINT64_MAX_VALUE);
    }
    else if (type == "i8" && max.empty())
    {
        ret = to_string(INT8_MAX_VALUE);
    }
    else if (type == "i16" && max.empty())
    {
        ret = to_string(INT16_MAX_VALUE);
    }
    else if (type == "i32" && max.empty())
    {
        ret = to_string(INT32_MAX_VALUE);
    }
    else if (type == "i64" && max.empty())
    {
        ret = to_string(INT64_MAX_VALUE);
    }
    else if (type == "f32" && max.empty())
    {
        ret = to_string(FLT_MAX_VALUE);
    }
    else if (type == "f64" && max.empty())
    {
        ret = to_string(DBL_MAX_VALUE);
    }
    return ret;
}
