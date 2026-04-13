#include "ILTranslatorForCoverage.h"

#include <fstream>
#include <map>

#include "ILTransFile.h"
#include "ILTransMain.h"

#include "../Common/Utility.h"
#include "../ILBasic/ILParser.h"
#include "../ILBasic/ILFile.h"
#include "../ILBasic/ILRef.h"
#include "../ILBasic/ILExternFile.h"
#include "../ILBasic/ILSchedule.h"
#include "../TCGBasic/TCGConfig.h"
#include "ILTransExternFile.h"
#include "ILTranslator.h"
#include "ILTranslatorUtility.h"

#include "../ILBasic/ILAnalyzeSchedule.h"
#include "ILTransInstrumentBitmapHelper.h"
#include "ILTransInstrumentMCDCCoverage.h"
#include "ILTransMetadataExporter.h"



using namespace std;

bool ILTranslatorForCoverage::configGenerateComment = true;
int ILTranslatorForCoverage::configCutOffArray = true;

ILParser* ILTranslatorForCoverage::iLParser;
//string ILTranslatorForCoverage::testCaseFileFolder;


std::string ILTranslatorForCoverage::transCodeBranchRegister;
std::string ILTranslatorForCoverage::transCodeBranchRelation;
std::string ILTranslatorForCoverage::transCodeGlobalVariableRegister;
std::string ILTranslatorForCoverage::transCodeInputRegister;

namespace 
{
    const int CONST_NUM_2 = 2;
}

ILTranslatorForCoverage::ILTranslatorForCoverage(ILParser* iLParser)
{
    ILTranslator::iLParser = iLParser;
    this->iLParser = iLParser;

    //this->testCaseFileFolder = testCaseFilePath;
}

int ILTranslatorForCoverage::translate(string outputDir)
{
    ILTransMetadataExporter::reset();
    ILTranslatorUtility::buildILFunctionCallGraph(iLParser);

	int res;
	for(int i = 0; i < iLParser->files.size(); i++) {
		string name = iLParser->files[i]->name + (iLParser->files[i]->isHeadFile ? ".h" : ".cpp");
		string code;
		ILTransFile transFile;
		if((res = transFile.translate(iLParser->files[i], code)) < 0)
			return res;

		code = codeIndentProcess(code);
        if(stringEndsWith(name, ".cpp"))
            code = "#include \"BitmapCov.h\"\n" + code;
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
        ILTranslatorUtility::releaseILFunctionCallGraph();
		ILTranslator::setCurrentError(res, "");
		return res;
	}
	mainCode = codeIndentProcess(mainCode);
    mainCode = "#include \"define.h\"\n" + mainCode;
	fileCodes["main.cpp"] = stringTrim(mainCode);
	string defineCode = ILTranslator::transCodeHeadFile;
    //defineCode += getConfigTestRedefineOutputCode();
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
        defineCode = stringReplaceAll(defineCode, "#define false 0\n", "");
        defineCode = stringReplaceAll(defineCode, "#define true 1\n", "");
        defineCode = stringReplaceAll(defineCode, "#define NULL 0\n", "");
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

    string MCDCFuncRegisterCode = ILTransInstrumentMCDCCoverage::generateMCDCFuncRegister();
    writeFile(outputDir + "/MCDCFuncRegister.cpp", MCDCFuncRegisterCode);
    

    res = copyResourceFiles(outputDir);
    if (res < 0)
        return res;
    //if(!isFolderExist(outputDir + "/Temp"))
    //    createFolder(outputDir + "/Temp");
    //writeFile(outputDir + "/TCGStateSearchBasicBranchRegister.cpp", transCodeBranchRegister);
    //writeFile(outputDir + "/TCGStateSearchBasicBranchRelation.cpp", transCodeBranchRelation);
    //writeFile(outputDir + "/TCGStateSearchBasicGlobalVariableRegister.cpp", transCodeGlobalVariableRegister);
    //writeFile(outputDir + "/TCGStateSearchBasicInputRegister.cpp", transCodeInputRegister);

    ILTransMetadataExporter::flush(outputDir, iLParser);
    ILTranslatorUtility::releaseILFunctionCallGraph();
	return 1;
}

void ILTranslatorForCoverage::setConfigGenerateComment(bool value)
{
    configGenerateComment = value;
}


void ILTranslatorForCoverage::setConfigCutOffArray(int value)
{
    configCutOffArray = value;
    if(configCutOffArray == 0)
        configCutOffArray = 1 << 30;
}


void ILTranslatorForCoverage::release()
{
}

string ILTranslatorForCoverage::getConfigTestRedefineOutputCode() const
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


namespace 
{
    const vector<string> copyFileList = {
        "BitmapCov.cpp",
        "BitmapCov.h",
        "tinyxml2.cpp",
        "tinyxml2.h",
        "Utility.cpp",
        "Utility.h",
        "BasicExcel.cpp",
        "BasicExcel.hpp",
        "ArgumentParser.h",
        "ArgumentParser.cpp",
    };
}
int ILTranslatorForCoverage::copyResourceFiles(string outputDir)
{
    string exeFilePath = getExeDirPath();
    for(int i = 0; i < copyFileList.size(); i++)
    {
        if(!isFileExist(exeFilePath + "/ResourceCode/" + copyFileList[i]))
            return -ILTranslator::ErrorResourceFileNotExist;
        copyFileOne(exeFilePath + "/ResourceCode/" + copyFileList[i], outputDir + "/" + copyFileList[i]);
    }
    return 1;
}
