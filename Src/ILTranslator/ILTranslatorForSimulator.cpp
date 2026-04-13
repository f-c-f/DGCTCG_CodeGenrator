#include "ILTranslatorForSimulator.h"

#include <fstream>

#include "ILTransFile.h"
#include "ILTransMain.h"

#include "../Common/Utility.h"
#include "../ILBasic/ILParser.h"
#include "../ILBasic/ILFile.h"
#include "../ILBasic/ILRef.h"
#include "../ILBasic/ILExternFile.h"
#include "../SimulatorBasic/SimulatorConfig.h"
#include "ILTransExternFile.h"
#include "ILTranslator.h"
#include "ILTransMetadataExporter.h"


using namespace std;

bool ILTranslatorForSimulator::configGenerateComment = true;
bool ILTranslatorForSimulator::configTest = false;
bool ILTranslatorForSimulator::configPackageVariable = true;
bool ILTranslatorForSimulator::configPackageParameter = true;

ILParser* ILTranslatorForSimulator::iLParser;
SimulatorConfig* ILTranslatorForSimulator::simuConfig;

std::map<std::string, pair<int, std::string>> ILTranslatorForSimulator::needMonitor;
int ILTranslatorForSimulator::outID = 0;
//std::map<std::string, std::string> ILTranslatorForSimulator::needDiagnose;
std::string ILTranslatorForSimulator::diagnoseFile;
std::string ILTranslatorForSimulator::diagnoseHFile;

namespace
{
    const int CONST_NUM_2 = 2;
}

ILTranslatorForSimulator::ILTranslatorForSimulator(ILParser* iLParser, SimulatorConfig* simuConfig)
{
    ILTranslator::iLParser = iLParser;
    this->iLParser = iLParser;
    this->simuConfig = simuConfig;
}

int ILTranslatorForSimulator::translate(string outputDir)
{
    int res;

    ILTransMetadataExporter::reset();
    diagnoseHFile = "#ifndef _DIAGNOSE_H_\n";
    diagnoseHFile += "#define _DIAGNOSE_H_\n";
    diagnoseHFile += "#include <stdio.h>\n";
    diagnoseHFile += "#include <string>\n";
    diagnoseHFile += "#include <cfenv>\n";
    diagnoseHFile += "#include <cmath>\n";
    diagnoseHFile += "#include <limits>\n";
    diagnoseHFile += "#include <time.h>\n\n";
    diagnoseHFile += "static clock_t Start_Time;\n\n";
    diagnoseHFile += "typedef int					i32;\n";
    diagnoseHFile += "typedef float				f32;\n";
    diagnoseHFile += "typedef long long			i64;\n";
    diagnoseHFile += "typedef double				f64;\n";
    diagnoseHFile += "typedef long double			f128;\n";
    diagnoseHFile += "typedef char				i8;\n";
    diagnoseHFile += "typedef short				i16;\n";
    diagnoseHFile += "typedef unsigned int		u32;\n";
    diagnoseHFile += "typedef unsigned long long	u64;\n";
    diagnoseHFile += "typedef unsigned char		u8;\n";
    diagnoseHFile += "typedef unsigned short		u16;\n";
    diagnoseHFile += "\n";


    diagnoseFile += "#include \"SimulatorDiagnose.h\"\n\n";

    for (int i = 0; i < iLParser->files.size(); i++) {
        string name = iLParser->files[i]->name + (iLParser->files[i]->isHeadFile ? ".h" : ".cpp");
        string code;
        ILTransFile transFile;
        if ((res = transFile.translate(iLParser->files[i], code)) < 0)
            return res;

        code = codeIndentProcess(code);
        code = "#include \"define.h\"\n" + code;
        fileCodes[name] = stringTrim(code);
    }

    for (int i = 0; i < iLParser->externFiles.size(); i++) {
        string name = iLParser->externFiles[i]->name;
        string code;
        ILTransExternFile transExternFile;
        if ((res = transExternFile.translate(iLParser->externFiles[i], code)) < 0)
            return res;

        fileCodes[name] = stringTrim(code);
    }

    string mainCode;
    ILTransMain iLTransMain;
    if ((res = iLTransMain.translate(iLParser, mainCode)) < 0) {
        ILTranslator::setCurrentError(res, "");
        return res;
    }
    mainCode = codeIndentProcess(mainCode);
    mainCode = "#include \"define.h\"\n" + mainCode;
    fileCodes["main.cpp"] = stringTrim(mainCode);
    string defineCode = ILTranslator::transCodeHeadFile;
    defineCode += "#include \"SimulatorBasic.h\"\n";
    defineCode += "#include \"SimulatorMonitor.h\"\n";
    defineCode += "#include \"SimulatorTestCase.h\"\n";
    defineCode += "#include \"SimulatorDiagnose.h\"\n";
    defineCode += configTest ? getConfigTestRedefineOutputCode() : "";
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

    if (!defineCode.empty()) {
        defineCode = "#ifndef _DEFINE_H_\n#define _DEFINE_H_\n" + defineCode + "\n#endif\n";
        //cout << " ---- define.h ----\n";
        //cout << defineCode << "\n\n";
        writeFile(outputDir + "/define.h", defineCode);
    }
    if (!ILTranslator::transCodeUtilityFunctionImplement.empty()) {
        string utilityFunctionImplementCode = codeIndentProcess("#include \"define.h\"\n" + ILTranslator::transCodeUtilityFunctionImplement);
        writeFile(outputDir + "/define.c", utilityFunctionImplementCode);
    }
    for (map<string, string>::iterator iter = fileCodes.begin(); iter != fileCodes.end(); iter++) {
        //if(!defineCode.empty())
        //	iter->second = "#include \"define.h\"\n" + iter->second;

        //cout << " ---- " << iter->first << ".cpp ----\n";
        //cout << iter->second << "\n\n";
        writeFile(outputDir + "/" + iter->first, iter->second);
    }
    

    diagnoseHFile += "#endif // _DIAGNOSE_H_\n";
    writeFile(outputDir + "/SimulatorDiagnose.cpp", diagnoseFile);
    writeFile(outputDir + "/SimulatorDiagnose.h", diagnoseHFile);

    res = copyResourceFiles(outputDir);
    if (res < 0)
        return res;

    ILTransMetadataExporter::flush(outputDir, iLParser);
    return 1;
}

void ILTranslatorForSimulator::setConfigGenerateComment(bool value)
{
    configGenerateComment = value;
}

void ILTranslatorForSimulator::setConfigTest(bool value)
{
    configTest = value;
}

void ILTranslatorForSimulator::setConfigPackageVariable(bool value)
{
    configPackageVariable = value;
}

void ILTranslatorForSimulator::setConfigPackageParameter(bool value)
{
    configPackageParameter = value;
}

std::string ILTranslatorForSimulator::getRefComment(const std::vector<ILRef*>* refList)
{
    if (!refList)
        return "";
    string commentStr;
    for (int i = 0; i < refList->size(); i++)
    {
        ILRef* ref = (*refList)[i];
        vector<string> actorStrSp = stringSplit(ref->actor, ".");
        string actorStr = "";
        if (!actorStrSp.empty())
            actorStr = actorStrSp.back();
        if (i != 0)
            commentStr += ",\n";
        if (!actorStr.empty() && !ref->path.empty())
            commentStr += actorStr + " named \"" + ref->path + "\"";
        else if (!actorStr.empty())
            commentStr += actorStr;
        else if (!ref->path.empty())
            commentStr += "\"" + ref->path + "\"";
        else if (i != 0)
            commentStr = commentStr.substr(0, commentStr.length() - CONST_NUM_2);
    }
    if (stringTrim(commentStr).empty())
        return "";
    if (stringSplit(commentStr, "\n").size() > 1)
        commentStr = "\n" + commentStr;
    return commentStr;
}

void ILTranslatorForSimulator::release()
{
}


string ILTranslatorForSimulator::getConfigTestRedefineOutputCode() const
{
    //??????????0??I?????????I??C?????????
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


namespace
{
    const vector<string> copyFileList = {        
        "ArgumentParser.h",
        "ArgumentParser.cpp",
        //"Utility.h",
        //"Utility.cpp",
        "SimulatorMonitor.h",
        "SimulatorMonitor.cpp",
        "SimulatorTestCase.h",
        "SimulatorTestCase.cpp",
        "SimulatorBasic.h",
        "SimulatorBasic.cpp",
    };
}
int ILTranslatorForSimulator::copyResourceFiles(string outputDir)
{
    string exeFilePath = getExeDirPath();
    for (int i = 0; i < copyFileList.size(); i++)
    {
        if (!isFileExist(exeFilePath + "/ResourceCode/" + copyFileList[i]))
            return -ILTranslator::ErrorResourceFileNotExist;
        copyFileOne(exeFilePath + "/ResourceCode/" + copyFileList[i], outputDir + "/" + copyFileList[i]);
    }
    return 1;
}
