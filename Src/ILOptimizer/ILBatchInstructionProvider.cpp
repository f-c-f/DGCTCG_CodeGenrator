#include "ILBatchInstructionProvider.h"

#include <set>

#include "ILBatchCalculationGraph.h"
#include "ILOptimizer.h"
#include "../Common/Utility.h"
#include "../ILBasic/ILParser.h"
using namespace std;


namespace
{
    const int CONST_NUM_2  = 2;
    const int CONST_NUM_3  = 3;
    const int CONST_NUM_4  = 4;
    const int CONST_NUM_5  = 5;
    const int CONST_NUM_6  = 6;
    const int CONST_NUM_7  = 7;
    const int CONST_NUM_8  = 8;
    const int CONST_NUM_16  = 16;
    const int CONST_NUM_32  = 32;
    const int CONST_NUM_64  = 64;
    const int CONST_NUM_128  = 128;
}

bool ILBatchInstructionProvider::loadedInstructionConfig = false;

int ILBatchInstructionProvider::vectorRegisterWidth = CONST_NUM_128;

std::vector<std::string> ILBatchInstructionProvider::headFileList = {
    "immintrin", "smmintrin", "emmintrin"
};

// 数据类型定义
// 格式如下：
// type, batchSize
std::map<std::string, std::string> ILBatchInstructionProvider::simdGraphStrToDataTypeMap = {
    pair<string, string>("i32, 4;", "__m128i"),
    pair<string, string>("f32, 4;", "__m128"),
};

// 每一个支持的指令采用唯一的图字符串描述，匹配指令时，也将子图转化为字符串形式，直接匹配字符串
// 单条指令的图字符串的格式为：
// op1, type, modifier, shape, batchSize, I_I1, I_I2, O_O1;\
// op2, type, modifier, shape, batchSize, I_I3, I_O1, O_O2;
// 这里写出全部可能的匹配情况，特别是对于
// A   B
//  \ /
//   C
// 这种，A、B可互换的情况，要写 A;B;C; 和 B;A;C; 两种形式
// 还有对于复合的运算，导致多个输入数据顺序可能不一致的也要都写出来
std::map<std::string, std::string> ILBatchInstructionProvider::simdGraphStrToCalculationInstructionMap = {
    pair<string, string>("Add, i32, N, N, 4, I_I1, I_I2, O_O1;", "%O_1 = _mm_add_epi32 (%I_1, %I_2)"),
    pair<string, string>("Add, f32, N, N, 4, I_I1, I_I2, O_O1;", "%O_1 = _mm_add_ps    (%I_1, %I_2)"),

    pair<string, string>("Sub, i32, N, N, 4, I_I1, I_I2, O_O1;", "%O_1 = _mm_sub_epi32 (%I_1, %I_2)"),
    pair<string, string>("Sub, f32, N, N, 4, I_I1, I_I2, O_O1;", "%O_1 = _mm_sub_ps    (%I_1, %I_2)"),
    
    pair<string, string>("Mul, i32, N, N, 4, I_I1, I_I2, O_O1;", "%O_1 = _mm_mul_epi32 (%I_1, %I_2)"),
    pair<string, string>("Mul, f32, N, N, 4, I_I1, I_I2, O_O1;", "%O_1 = _mm_mul_ps    (%I_1, %I_2)"),
    
    pair<string, string>("Div, f32, N, N, 4, I_I1, I_I2, O_O1;", "%O_1 = _mm_div_ps    (%I_1, %I_2)"),
    
    //由于加法计算不分先后所以有两种表示形式
    pair<string, string>("Mul, f32, N, N, 4, I_I1, I_I2, O_O1;\
                          Add, f32, N, N, 4, I_O1, I_I3, O_O2;", "%O_1 = _mm_fmadd_ps  (%I_1, %I_2, %I_3)"),
    pair<string, string>("Mul, f32, N, N, 4, I_I1, I_I2, O_O1;\
                          Add, f32, N, N, 4, I_I3, I_O1, O_O2;", "%O_1 = _mm_fmadd_ps  (%I_1, %I_2, %I_3)"),
};

// 数据类型转换指令
// 格式如下：
// typeSrc, typeDst, batchSize
std::map<std::string, std::string> ILBatchInstructionProvider::simdGraphStrToTypeConvertInstructionMap = {
    pair<string, string>("i32, f32, 4;", "%O_1 = _mm_cvtepi32_ps (%I_1)"),
    pair<string, string>("f32, i32, 4;", "%O_1 = _mm_cvtps_epi32 (%I_1)"),
};

// 数据加载指令
// 格式如下：
// type, batchSize, loadSize
std::map<std::string, std::string> ILBatchInstructionProvider::simdGraphStrToDataLoadInstructionMap = {
    pair<string, string>("i32, 4, 4;", "%O_1 = _mm_set_epi32   ((%I_1)[3], (%I_1)[2], (%I_1)[1], (%I_1)[0])"),
    pair<string, string>("f32, 4, 4;", "%O_1 = _mm_load_ps      (%I_1)"),
    pair<string, string>("i32, 4, 1;", "%O_1 = _mm_set1_epi32   (%I_1)"),
    pair<string, string>("f32, 4, 1;", "%O_1 = _mm_set1_ps      (%I_1)"),
};

// 数据存储指令
// 格式如下：
// type, batchSize
std::map<std::string, std::string> ILBatchInstructionProvider::simdGraphStrToDataStoreInstructionMap = {
    pair<string, string>("i32, 4;", "(%O_1)[0] = ((int*)& %I_1)[0];\
                                     (%O_1)[1] = ((int*)& %I_1)[1];\
                                     (%O_1)[2] = ((int*)& %I_1)[2];\
                                     (%O_1)[3] = ((int*)& %I_1)[3];"),
    pair<string, string>("f32, 4;", "_mm_store_ps    (%O_1, %I_1)"),
};

std::map<std::string, std::string>
ILBatchInstructionProvider::simdGraphStrToCalculationInstructionNormalMap = {
    pair<string, string>("Div, i32, N, N, 4, I_I1, I_I2, O_O1;", "\
        ((int*)& %O_1)[0] = ((int*)& %I_1 )[0] / ((int*)& %I_2 )[0];\
        ((int*)& %O_1)[1] = ((int*)& %I_1 )[1] / ((int*)& %I_2 )[1];\
        ((int*)& %O_1)[2] = ((int*)& %I_1 )[2] / ((int*)& %I_2 )[2];\
        ((int*)& %O_1)[3] = ((int*)& %I_1 )[3] / ((int*)& %I_2 )[3];"),
};




map<StmtBatchCalculation::OperationType, string> ILBatchInstructionProvider::operationTypeToTypeStrMap = {
    pair<StmtBatchCalculation::OperationType, string>(StmtBatchCalculation::TypeAdd,  "Add"),
    pair<StmtBatchCalculation::OperationType, string>(StmtBatchCalculation::TypeSub,  "Sub"),
    pair<StmtBatchCalculation::OperationType, string>(StmtBatchCalculation::TypeMul,  "Mul"),
    pair<StmtBatchCalculation::OperationType, string>(StmtBatchCalculation::TypeDiv,  "Div"),
    pair<StmtBatchCalculation::OperationType, string>(StmtBatchCalculation::TypeLShr, "Shr"),
    pair<StmtBatchCalculation::OperationType, string>(StmtBatchCalculation::TypeAShr, "Shr"),
    pair<StmtBatchCalculation::OperationType, string>(StmtBatchCalculation::TypeShl,  "Shl"),
    pair<StmtBatchCalculation::OperationType, string>(StmtBatchCalculation::TypeAnd,  "And"),
    pair<StmtBatchCalculation::OperationType, string>(StmtBatchCalculation::TypeOr,   "Or"),
    pair<StmtBatchCalculation::OperationType, string>(StmtBatchCalculation::TypeXor,  "Xor"),
};


map<StmtBatchCalculation::ModifierType, string> ILBatchInstructionProvider::modifierTypeToTypeStrMap = {
    pair<StmtBatchCalculation::ModifierType, string>(StmtBatchCalculation::TypeNone,     "N"),
    pair<StmtBatchCalculation::ModifierType, string>(StmtBatchCalculation::TypeSaturate, "S"),
    pair<StmtBatchCalculation::ModifierType, string>(StmtBatchCalculation::TypeHalf,     "H"),
    pair<StmtBatchCalculation::ModifierType, string>(StmtBatchCalculation::TypeDouble,   "D"),
    pair<StmtBatchCalculation::ModifierType, string>(StmtBatchCalculation::TypeRound,    "R"),
};


map<StmtBatchCalculation::ShapeType, string> ILBatchInstructionProvider::shapeTypeToTypeStrMap = {
    pair<StmtBatchCalculation::ShapeType, string>(StmtBatchCalculation::TypeNormal, "N"),
    pair<StmtBatchCalculation::ShapeType, string>(StmtBatchCalculation::TypeLong,   "L"),
    pair<StmtBatchCalculation::ShapeType, string>(StmtBatchCalculation::TypeWide,   "W"),
    pair<StmtBatchCalculation::ShapeType, string>(StmtBatchCalculation::TypeNarrow, "A"),
};

std::string ILBatchInstruction::getFormattedCode(const ILBatchCalculationGraph& graph, const std::map<std::string, std::string> &currentNodeTypeConvertMap) const
{
    string retCode = this->format;
    size_t posStart = retCode.find("%o_");
    if(posStart == string::npos)
        posStart = retCode.find("%i_");
    if(posStart == string::npos)
        posStart = retCode.find("%p_");
    size_t posEnd = posStart + CONST_NUM_3;
    while(posStart != string::npos) {
        while(retCode[posEnd] >= '0' && retCode[posEnd] <= '9' || retCode[posEnd] == '_') {
            posEnd++;
        }
        string tempVarStr = retCode.substr(posStart, posEnd - posStart);
        vector<string> tempVarStrSp = stringSplit(tempVarStr, "_");
        int layerId = stringToInt(tempVarStrSp[1]);
        int nodeId = stringToInt(tempVarStrSp[CONST_NUM_2]);
        int varId = stringToInt(tempVarStrSp[CONST_NUM_3]);
        ILBatchCalculationGraphNode* node = graph.at(layerId)[nodeId];
        
        string variableName;
        
        if(stringStartsWith(tempVarStr,"%o_"))
            variableName = node->outputs[varId].expression;
        else if(stringStartsWith(tempVarStr,"%i_"))
            variableName = node->inputs[varId].expression;
        else if(stringStartsWith(tempVarStr,"%p_"))
            variableName = node->parameters[varId].expression;

        string variableNameBatch = graph.getNodeIONameMap(variableName);
        if(!stringStartsWith(tempVarStr,"%p_"))
        {
            if(variableNameBatch.empty())
                return "";
            if(currentNodeTypeConvertMap.find(variableName) != currentNodeTypeConvertMap.end())
                variableNameBatch = currentNodeTypeConvertMap.at(variableName);
        } else {
            variableNameBatch = variableName;
        }
        

        retCode = retCode.replace(posStart, posEnd - posStart, variableNameBatch);

        posStart = retCode.find("%o_", posStart + variableNameBatch.length());
        if(posStart == string::npos)
            posStart = retCode.find("%i_");
        if(posStart == string::npos)
            posStart = retCode.find("%p_");
        posEnd = posStart + CONST_NUM_3;
    }
    return retCode;
}

std::string ILBatchInstruction::getFormattedDataLoadCode(std::string inputData) const
{
    string formattedCode = this->format;
    formattedCode = stringReplaceAll(formattedCode, "%o_0_0_1 = ", "");
    formattedCode = stringReplaceAll(formattedCode, "%i_0_0_1", inputData);
    return formattedCode;
}

std::string ILBatchInstruction::getFormattedDataLoadCode(std::string inputData, std::string outputData) const
{
    string formattedCode = this->format;
    formattedCode = stringReplaceAll(formattedCode, "%o_0_0_1", outputData);
    formattedCode = stringReplaceAll(formattedCode, "%i_0_0_1", inputData);
    return formattedCode;
}

std::string ILBatchInstruction::getFormattedDataStoreCode(std::string inputData, std::string outputData) const
{
    string formattedCode = this->format;
    formattedCode = stringReplaceAll(formattedCode, "%o_0_0_1", outputData);
    formattedCode = stringReplaceAll(formattedCode, "%i_0_0_1", inputData);
    return formattedCode;
}

std::string ILBatchInstruction::getFormattedDataTypeConvertCode(std::string inputData, std::string outputData) const
{
    string formattedCode = this->format;
    formattedCode = stringReplaceAll(formattedCode, "%o_0_0_1", outputData);
    formattedCode = stringReplaceAll(formattedCode, "%i_0_0_1", inputData);
    return formattedCode;
}

void ILBatchInstructionProvider::loadInstructionConfig()
{
    if(loadedInstructionConfig)
        return;
    
    loadedInstructionConfig = true;
    string path = getExeDirPath() + "/BatchInstructionProvider.txt";
    string targetDeviceFile = getExeDirPath() + "/BatchInstructionProvider-" + ILOptimizer::configTargetDevice + ".txt";
    if(isFileExist(targetDeviceFile))
        path = targetDeviceFile;

    string configFileContext = readFile(path);
    if(configFileContext.empty())
        return;

    int currentSection = 0;
    vector<string> sectionContextList(CONST_NUM_8);
    vector<string> lineSp = stringSplit(configFileContext, "\n");
    for (int i = 0 ; i < lineSp.size(); i++) {
        lineSp[i] = stringTrim(lineSp[i]);
        if(lineSp[i].empty() || stringStartsWith(lineSp[i], "//"))
            continue;
        //configFileContext += lineSp[i] + "\n";
        if(lineSp[i] == "**vectorRegisterWidth**")
            currentSection = 0;
        else if(lineSp[i] == "**headFileList**")
            currentSection = 1;
        else if(lineSp[i] == "**simdGraphStrToDataTypeMap**")
            currentSection = CONST_NUM_2;
        else if(lineSp[i] == "**simdGraphStrToCalculationInstructionMap**")
            currentSection = CONST_NUM_3;
        else if(lineSp[i] == "**simdGraphStrToTypeConvertInstructionMap**")
            currentSection = CONST_NUM_4;
        else if(lineSp[i] == "**simdGraphStrToDataLoadInstructionMap**")
            currentSection = CONST_NUM_5;
        else if(lineSp[i] == "**simdGraphStrToDataStoreInstructionMap**")
            currentSection = CONST_NUM_6;
        else if(lineSp[i] == "**simdGraphStrToCalculationInstructionNormalMap**")
            currentSection = CONST_NUM_7;
        else
            sectionContextList[currentSection] += lineSp[i] + "\n";
    }

    loadInstructionConfigVectorRegisterWidth(sectionContextList[0]);
    loadInstructionConfigHeadFileList(sectionContextList[1]);
    loadInstructionConfigSimdGraphStrToDataTypeMap(sectionContextList[CONST_NUM_2]);
    loadInstructionConfigSimdGraphStrToCalculationInstructionMap(sectionContextList[CONST_NUM_3]);
    loadInstructionConfigSimdGraphStrToTypeConvertInstructionMap(sectionContextList[CONST_NUM_4]);
    loadInstructionConfigSimdGraphStrToDataLoadInstructionMap(sectionContextList[CONST_NUM_5]);
    loadInstructionConfigSimdGraphStrToDataStoreInstructionMap(sectionContextList[CONST_NUM_6]);
    loadInstructionConfigSimdGraphStrToCalculationInstructionNormalMap(sectionContextList[CONST_NUM_7]);
    
}

void ILBatchInstructionProvider::loadInstructionConfigVectorRegisterWidth(std::string context)
{
    vectorRegisterWidth = 0;
    size_t pos = context.find("value");
    if(pos == string::npos) {
        return;
    }
    pos = context.find("\"", pos);
    if(pos == string::npos) {
        return;
    }
    size_t pos2 = context.find("\"", pos + 1);
    if(pos2 == string::npos) {
        return;
    }
    string value = context.substr(pos + 1, pos2 - pos - 1);
    vectorRegisterWidth = stringToInt(value);
}

void ILBatchInstructionProvider::loadInstructionConfigHeadFileList(std::string context)
{
    headFileList.clear();
    size_t pos = context.find("\"");
    while(pos != string::npos)
    {
        size_t pos2 = context.find("\"", pos + 1);
        if(pos2 == string::npos) {
            return;
        }
        string value = context.substr(pos + 1, pos2 - pos - 1);

        headFileList.push_back(value);

        pos = context.find("\"", pos2 + 1);
    }
}

void ILBatchInstructionProvider::loadInstructionConfigSimdGraphStrToDataTypeMap(std::string context)
{
    simdGraphStrToDataTypeMap.clear();
    size_t pos = context.find("key");
    while(pos != string::npos)
    {
        pos = context.find("\"", pos);
        if(pos == string::npos) {
            return;
        }
        size_t pos2 = context.find("\"", pos + 1);
        if(pos2 == string::npos) {
            return;
        }
        string key = context.substr(pos + 1, pos2 - pos - 1);
        key = stringReplaceAll(key, "\\", "");
        key = stringReplaceAll(key, "\n", "");
        pos = context.find("\"", pos2 + 1);
        if(pos == string::npos) {
            return;
        }
        pos2 = context.find("\"", pos + 1);
        if(pos2 == string::npos) {
            return;
        }
        string value = context.substr(pos + 1, pos2 - pos - 1);
        value = stringReplaceAll(value, "\\", "");
        value = stringReplaceAll(value, "\n", "");

        simdGraphStrToDataTypeMap[key] = value;

        pos = context.find("\"", pos2 + 1);
    }
}

void ILBatchInstructionProvider::loadInstructionConfigSimdGraphStrToCalculationInstructionMap(std::string context)
{
    simdGraphStrToCalculationInstructionMap.clear();
    size_t pos = context.find("key");
    while(pos != string::npos)
    {
        pos = context.find("\"", pos);
        if(pos == string::npos) {
            return;
        }
        size_t pos2 = context.find("\"", pos + 1);
        if(pos2 == string::npos) {
            return;
        }
        string key = context.substr(pos + 1, pos2 - pos - 1);
        key = stringReplaceAll(key, "\\", "");
        key = stringReplaceAll(key, "\n", "");

        pos = context.find("\"", pos2 + 1);
        if(pos == string::npos) {
            return;
        }
        pos2 = context.find("\"", pos + 1);
        if(pos2 == string::npos) {
            return;
        }
        string value = context.substr(pos + 1, pos2 - pos - 1);
        value = stringReplaceAll(value, "\\", "");
        value = stringReplaceAll(value, "\n", "");

        simdGraphStrToCalculationInstructionMap[key] = value;

        pos = context.find("\"", pos2 + 1);
    }
}

void ILBatchInstructionProvider::loadInstructionConfigSimdGraphStrToTypeConvertInstructionMap(std::string context)
{
    simdGraphStrToTypeConvertInstructionMap.clear();
    size_t pos = context.find("key");
    while(pos != string::npos)
    {
        pos = context.find("\"", pos);
        if(pos == string::npos) {
            return;
        }
        size_t pos2 = context.find("\"", pos + 1);
        if(pos2 == string::npos) {
            return;
        }
        string key = context.substr(pos + 1, pos2 - pos - 1);
        key = stringReplaceAll(key, "\\", "");
        key = stringReplaceAll(key, "\n", "");

        pos = context.find("\"", pos2 + 1);
        if(pos == string::npos) {
            return;
        }
        pos2 = context.find("\"", pos + 1);
        if(pos2 == string::npos) {
            return;
        }
        string value = context.substr(pos + 1, pos2 - pos - 1);
        value = stringReplaceAll(value, "\\", "");
        value = stringReplaceAll(value, "\n", "");

        simdGraphStrToTypeConvertInstructionMap[key] = value;

        pos = context.find("\"", pos2 + 1);
    }
}

void ILBatchInstructionProvider::loadInstructionConfigSimdGraphStrToDataLoadInstructionMap(std::string context)
{
    simdGraphStrToDataLoadInstructionMap.clear();
    size_t pos = context.find("key");
    while(pos != string::npos)
    {
        pos = context.find("\"", pos);
        if(pos == string::npos) {
            return;
        }
        size_t pos2 = context.find("\"", pos + 1);
        if(pos2 == string::npos) {
            return;
        }
        string key = context.substr(pos + 1, pos2 - pos - 1);
        key = stringReplaceAll(key, "\\", "");
        key = stringReplaceAll(key, "\n", "");

        pos = context.find("\"", pos2 + 1);
        if(pos == string::npos) {
            return;
        }
        pos2 = context.find("\"", pos + 1);
        if(pos2 == string::npos) {
            return;
        }
        string value = context.substr(pos + 1, pos2 - pos - 1);
        value = stringReplaceAll(value, "\\", "");
        value = stringReplaceAll(value, "\n", "");

        simdGraphStrToDataLoadInstructionMap[key] = value;

        pos = context.find("\"", pos2 + 1);
    }
}

void ILBatchInstructionProvider::loadInstructionConfigSimdGraphStrToDataStoreInstructionMap(std::string context)
{
    simdGraphStrToDataStoreInstructionMap.clear();
    size_t pos = context.find("key");
    while(pos != string::npos)
    {
        pos = context.find("\"", pos);
        if(pos == string::npos) {
            return;
        }
        size_t pos2 = context.find("\"", pos + 1);
        if(pos2 == string::npos) {
            return;
        }
        string key = context.substr(pos + 1, pos2 - pos - 1);
        key = stringReplaceAll(key, "\\", "");
        key = stringReplaceAll(key, "\n", "");

        pos = context.find("\"", pos2 + 1);
        if(pos == string::npos) {
            return;
        }
        pos2 = context.find("\"", pos + 1);
        if(pos2 == string::npos) {
            return;
        }
        string value = context.substr(pos + 1, pos2 - pos - 1);
        value = stringReplaceAll(value, "\\", "");
        value = stringReplaceAll(value, "\n", "");

        simdGraphStrToDataStoreInstructionMap[key] = value;

        pos = context.find("\"", pos2 + 1);
    }
}

void ILBatchInstructionProvider::loadInstructionConfigSimdGraphStrToCalculationInstructionNormalMap(std::string context)
{
    simdGraphStrToCalculationInstructionNormalMap.clear();
    size_t pos = context.find("key");
    while(pos != string::npos)
    {
        pos = context.find("\"", pos);
        if(pos == string::npos) {
            return;
        }
        size_t pos2 = context.find("\"", pos + 1);
        if(pos2 == string::npos) {
            return;
        }
        string key = context.substr(pos + 1, pos2 - pos - 1);
        key = stringReplaceAll(key, "\\", "");
        key = stringReplaceAll(key, "\n", "");

        pos = context.find("\"", pos2 + 1);
        if(pos == string::npos) {
            return;
        }
        pos2 = context.find("\"", pos + 1);
        if(pos2 == string::npos) {
            return;
        }
        string value = context.substr(pos + 1, pos2 - pos - 1);
        value = stringReplaceAll(value, "\\", "");
        value = stringReplaceAll(value, "\n", "");

        simdGraphStrToCalculationInstructionNormalMap[key] = value;

        pos = context.find("\"", pos2 + 1);
    }
}

std::string ILBatchInstructionProvider::getSimdDataTypeByGraphStrMap(std::string graphStr)
{
    if(simdGraphStrToCalculationInstructionMap.find(graphStr) != simdGraphStrToCalculationInstructionMap.end())
        return simdGraphStrToCalculationInstructionMap.at(graphStr);
    return "";
}

std::string ILBatchInstructionProvider::getSimdCalculationInstructionByGraphStrMap(std::string graphStr)
{
    if(simdGraphStrToCalculationInstructionMap.find(graphStr) != simdGraphStrToCalculationInstructionMap.end())
        return simdGraphStrToCalculationInstructionMap.at(graphStr);
    return "";
}

std::string ILBatchInstructionProvider::getSimdTypeConvertInstructionByGraphStrMap(std::string graphStr)
{
    if(simdGraphStrToTypeConvertInstructionMap.find(graphStr) != simdGraphStrToTypeConvertInstructionMap.end())
        return simdGraphStrToTypeConvertInstructionMap.at(graphStr);
    return "";
}

std::string ILBatchInstructionProvider::getSimdDataLoadInstructionByGraphStrMap(std::string graphStr)
{
    if(simdGraphStrToDataLoadInstructionMap.find(graphStr) != simdGraphStrToDataLoadInstructionMap.end())
        return simdGraphStrToDataLoadInstructionMap.at(graphStr);
    return "";
}

std::string ILBatchInstructionProvider::getSimdDataStoreInstructionByGraphStrMap(std::string graphStr)
{
    if(simdGraphStrToDataStoreInstructionMap.find(graphStr) != simdGraphStrToDataStoreInstructionMap.end())
        return simdGraphStrToDataStoreInstructionMap.at(graphStr);
    return "";
}

std::string ILBatchInstructionProvider::getOperationStrByOperationTypeMap(
    StmtBatchCalculation::OperationType type)
{
    if(operationTypeToTypeStrMap.find(type) != operationTypeToTypeStrMap.end())
        return operationTypeToTypeStrMap.at(type);
    return "";
}

std::string ILBatchInstructionProvider::getModifierStrByModifierTypeMap(
    StmtBatchCalculation::ModifierType type)
{
    if(modifierTypeToTypeStrMap.find(type) != modifierTypeToTypeStrMap.end())
        return modifierTypeToTypeStrMap.at(type);
    return "";
}

std::string ILBatchInstructionProvider::getShapeStrByShapeTypeMap(
    StmtBatchCalculation::ShapeType type)
{
    if(shapeTypeToTypeStrMap.find(type) != shapeTypeToTypeStrMap.end())
        return shapeTypeToTypeStrMap.at(type);
    return "";
}

std::string ILBatchInstructionProvider::getSimdCalculationInstructionNormalByGraphStrMap(
    std::string graphStr)
{
    if(simdGraphStrToCalculationInstructionNormalMap.find(graphStr) != simdGraphStrToCalculationInstructionNormalMap.end())
        return simdGraphStrToCalculationInstructionNormalMap.at(graphStr);
    return "";
}

std::string ILBatchInstructionProvider::translateInstructionStrMapToGraph(
    const ILBatchCalculationGraph& graph,
    ILBatchInstruction& instruction)
{
    string instructionStr = instruction.format;
    
    map<string, bool> innerVarMap;
    map<string, string> outputVarMap;
    int outputCount = 0;
    translateInstructionStrMapToGraphCollectVarMap(graph, innerVarMap, outputVarMap, outputCount);

    int inputCount = 0;
    outputCount = 0;
    int paraCount = 0;
    for(int i = 0; i < graph.size(); i++) {
        for(int j = 0; !graph.isLayerEmpty(i) && j < graph.at(i).size(); j++) {
            ILBatchCalculationGraphNode* tempNode = graph.at(i)[j];
            int layerId = 0;
            int arrayId = 0;
            graph.getRootGraph()->findNode(tempNode, layerId, arrayId);
            string varFormat = "_" + to_string(layerId) + "_" + to_string(arrayId) + "_";
            

            for(int k = 0; k < tempNode->inputs.size(); k++) {
                string varName = tempNode->inputs[k].expression;
                if(innerVarMap.find(varName) != innerVarMap.end())
                    continue;
                inputCount++;
                instructionStr = stringReplaceAll(instructionStr, "%I_" + to_string(inputCount), "%i" + varFormat + to_string(k));
            }
            for(int k = 0; k < tempNode->parameters.size(); k++) {
                string paraName = tempNode->parameters[k].expression;
                paraCount++;
                instructionStr = stringReplaceAll(instructionStr, "%P_" + to_string(paraCount), "%p" + varFormat + to_string(k));
            }
            for(int k = 0; k < tempNode->outputs.size(); k++) {
                string varName = tempNode->outputs[k].expression;
                if(innerVarMap.find(varName) != innerVarMap.end())
                    continue;
                outputCount++;
                instructionStr = stringReplaceAll(instructionStr, "%O_" + to_string(outputCount), "%o" + varFormat + to_string(k));
            }
        }
    }

    return instructionStr;
}

std::string ILBatchInstructionProvider::translateInstructionStrMapToGraph(
    ILBatchInstruction& instruction)
{
    string instructionStr = instruction.format;

    string inStr = "%I_";
    int inCount = 0;
    string inDstStr = "%i_0_0_";
    size_t pos = instructionStr.find(inStr);
    while(pos != string::npos)
    {
        string temp = inStr;// + to_string(inCount + 1);
        instructionStr.replace(pos, temp.length(), inDstStr); // + to_string(inCount)
        pos = instructionStr.find(inStr, pos + 1);
        inCount++;
    }
    string outStr = "%O_";
    int outCount = 0;
    string outDstStr = "%o_0_0_";
    pos = instructionStr.find(outStr);
    while(pos != string::npos)
    {
        string temp = outStr; // + to_string(outCount + 1)
        instructionStr.replace(pos, temp.length(), outDstStr); // + to_string(outCount)
        pos = instructionStr.find(outStr, pos + 1);
        outCount++;
    }
    return instructionStr;
}

void ILBatchInstructionProvider::translateInstructionStrMapToGraphCollectVarMap(const ILBatchCalculationGraph& graph,
    map<string, bool> &innerVarMap, map<string, string> &outputVarMap, int &outputCount)
{
    for(int i = 0; i < graph.size(); i++) {
        for(int j = 0; !graph.isLayerEmpty(i) && j < graph.at(i).size(); j++) {
            ILBatchCalculationGraphNode* tempNode = graph.at(i)[j];

            for(int k = 0; k < tempNode->inputs.size(); k++) {
                string varName = tempNode->inputs[k].expression;
                if(outputVarMap.find(varName) != outputVarMap.end()) {
                    innerVarMap[varName] = true;
                }
            }
            for(int k = 0; k < tempNode->outputs.size(); k++) {
                string varNameTemp = to_string(++outputCount);
                outputVarMap[tempNode->outputs[k].expression] = varNameTemp;
            }
        }
    }
}

std::vector<std::string> ILBatchInstructionProvider::getHeadFileList()
{
    return headFileList;
}

int ILBatchInstructionProvider::calculateInstructionBatchCount(const ILBatchCalculationGraph& graph)
{
    ILBatchCalculationGraphNode* firstNode = graph.getFirstNode();
    if(!firstNode)
        return 0;
    int batchCount = firstNode->calculateSize.back();
    if(batchCount < CONST_NUM_2)
        return batchCount;

    
    int dataTypeWidth = ILParser::getBasicDataTypeWidth(firstNode->dataType) * CONST_NUM_8;
    

    for(int i = 0; i < graph.size(); i++) {
        for(int j = 0; !graph.isLayerEmpty(i) && j < graph.at(i).size(); j++) {
            ILBatchCalculationGraphNode* tempNode = graph.at(i)[j];

            int dataTypeWidthTemp = ILParser::getBasicDataTypeWidth(tempNode->dataType) * CONST_NUM_8;
            dataTypeWidth = dataTypeWidth > dataTypeWidthTemp ? dataTypeWidth : dataTypeWidthTemp;

            for(int k = 0; k < tempNode->inputs.size(); k++)
            {
                dataTypeWidthTemp = ILParser::getBasicDataTypeWidth(tempNode->inputs[k].type) * CONST_NUM_8;
                dataTypeWidth = dataTypeWidth > dataTypeWidthTemp ? dataTypeWidth : dataTypeWidthTemp;
            }

            for(int k = 0; k < tempNode->outputs.size(); k++)
            {
                dataTypeWidthTemp = ILParser::getBasicDataTypeWidth(tempNode->outputs[k].type) * CONST_NUM_8;
                dataTypeWidth = dataTypeWidth > dataTypeWidthTemp ? dataTypeWidth : dataTypeWidthTemp;
            }
        }
    }

    if(dataTypeWidth == 0)
        return 0;

    return vectorRegisterWidth / dataTypeWidth;
}

std::string ILBatchInstructionProvider::getBatchVariableType(std::string dataType, int batchCount)
{
    string graphStr = ILParser::getBasicDataTypeSimple(dataType) + "," + to_string(batchCount) + ";";

    string typeStr = getMatchSubGraphStrDataType(graphStr);

    return typeStr;
}

int ILBatchInstructionProvider::getBatchLoadInstruction(std::string dataType, int batchCount, int loadCount, ILBatchInstruction& instruction)
{
    string graphStr = ILParser::getBasicDataTypeSimple(dataType) + ",";
    graphStr += to_string(batchCount) + "," + to_string(loadCount) + ";";
    string instructionStr = getMatchSubGraphStrLoadInstruction(graphStr);
    if(instructionStr.empty()) {
        instruction.format = "";
        return 0;
    }
    instruction.format = instructionStr;
    instruction.format = translateInstructionStrMapToGraph(instruction);
    return 1;
}

int ILBatchInstructionProvider::getBatchStoreInstruction(std::string dataType, int batchCount, ILBatchInstruction& instruction)
{
    string graphStr = ILParser::getBasicDataTypeSimple(dataType) + "," + to_string(batchCount) + ";";
    string instructionStr = getMatchSubGraphStrStoreInstruction(graphStr);
    if(instructionStr.empty()) {
        instruction.format = "";
        return 0;
    }
    instruction.format = instructionStr;
    instruction.format = translateInstructionStrMapToGraph(instruction);
    return 1;
}

int ILBatchInstructionProvider::getBatchDataConvertInstruction(std::string dataTypeSrc, std::string dataTypeDst,
    int batchCount, ILBatchInstruction& instruction)
{
    string graphStr = ILParser::getBasicDataTypeSimple(dataTypeSrc) + ",";
    graphStr += ILParser::getBasicDataTypeSimple(dataTypeDst) + ",";
    graphStr += to_string(batchCount) + ";";
    string instructionStr = getMatchSubGraphStrDataConvertInstruction(graphStr);
    if(instructionStr.empty()) {
        instruction.format = "";
        return 0;
    }
    instruction.format = instructionStr;
    instruction.format = translateInstructionStrMapToGraph(instruction);
    return 1;
}

int ILBatchInstructionProvider::getInstruction(const ILBatchCalculationGraph& graph, int batchCount, ILBatchInstruction& instruction)
{
    string graphStr = getSubGraphString(graph, batchCount);

    string instructionStr = getMatchSubGraphStrInstruction(graphStr);
    if(instructionStr.empty()) {
        instruction.format = "";
        return 0;
    }
    instruction.format = instructionStr;
    instruction.format = translateInstructionStrMapToGraph(graph, instruction);

    return 1;
}

int ILBatchInstructionProvider::getNormalInstruction(const ILBatchCalculationGraph& graph, int batchCount, ILBatchInstruction& instruction)
{
    string graphStr = getSubGraphString(graph, batchCount);

    string instructionStr = getMatchSubGraphStrInstructionNormal(graphStr);
    if(instructionStr.empty()) {
        instruction.format = "";
        return 0;
    }
    instruction.format = instructionStr;
    instruction.format = translateInstructionStrMapToGraph(graph, instruction);
    
    return 1;
}



std::string ILBatchInstructionProvider::getSubGraphString(const ILBatchCalculationGraph& graph, int batchCount)
{
    string ret;
    map<string, string> outputVarMap;
    int inputCount = 0;
    int outputCount = 0;
    int paraCount = 0;

    for(int i = 0; i < graph.size(); i++) {
        for(int j = 0; !graph.isLayerEmpty(i) && j < graph.at(i).size(); j++) {
            ILBatchCalculationGraphNode* tempNode = graph.at(i)[j];

            ret += getOperationStrByOperationTypeMap(tempNode->operationType) + ",";
            ret += ILParser::getBasicDataTypeSimple(tempNode->dataType) + ",";
            ret += getModifierStrByModifierTypeMap(tempNode->modifierType) + ",";
            ret += getShapeStrByShapeTypeMap(tempNode->shapeType) + ",";
            ret += to_string(batchCount) + ",";
            for(int k = 0; k < tempNode->inputs.size(); k++)
            {
                string varName = tempNode->inputs[k].expression;
                ret += "I_" + (outputVarMap.find(varName) == outputVarMap.end() ?
                    "I" + to_string(++inputCount) : outputVarMap.at(varName));
                ret += ",";
            }
            for(int k = 0; k < tempNode->parameters.size(); k++)
            {
                string paraName = tempNode->parameters[k].expression;
                ret += "P_P" + to_string(++paraCount);
                ret += ",";
            }
            for(int k = 0; k < tempNode->outputs.size(); k++)
            {
                string varNameTemp = "O" + to_string(++outputCount);
                ret += "O_" + varNameTemp;
                outputVarMap[tempNode->outputs[k].expression] = varNameTemp;
                ret += ",";
            }
            ret = ret.substr(0, ret.length() - 1);
            ret += ";";
        }
    }
    return ret;
}

std::string ILBatchInstructionProvider::getMatchSubGraphStrDataType(std::string graphStr)
{
    graphStr = stringReplaceAll(graphStr, " ", "");
    for(auto iter = simdGraphStrToDataTypeMap.begin(); iter != simdGraphStrToDataTypeMap.end(); iter++)
    {
        string key = iter->first;
        string value = iter->second;

        key = stringReplaceAll(key, " ", "");

        if(graphStr == key)
            return value;
    }
    return "";
}

std::string ILBatchInstructionProvider::getMatchSubGraphStrLoadInstruction(std::string graphStr)
{
    graphStr = stringReplaceAll(graphStr, " ", "");
    for(auto iter = simdGraphStrToDataLoadInstructionMap.begin(); iter != simdGraphStrToDataLoadInstructionMap.end(); iter++)
    {
        string key = iter->first;
        string value = iter->second;

        key = stringReplaceAll(key, " ", "");

        if(graphStr == key)
            return value;
    }
    return "";
}

std::string ILBatchInstructionProvider::getMatchSubGraphStrStoreInstruction(std::string graphStr)
{
    graphStr = stringReplaceAll(graphStr, " ", "");
    for(auto iter = simdGraphStrToDataStoreInstructionMap.begin(); iter != simdGraphStrToDataStoreInstructionMap.end(); iter++)
    {
        string key = iter->first;
        string value = iter->second;

        key = stringReplaceAll(key, " ", "");

        if(graphStr == key)
            return value;
    }
    return "";
}

std::string ILBatchInstructionProvider::getMatchSubGraphStrDataConvertInstruction(std::string graphStr)
{
    graphStr = stringReplaceAll(graphStr, " ", "");
    for(auto iter = simdGraphStrToTypeConvertInstructionMap.begin(); iter != simdGraphStrToTypeConvertInstructionMap.end(); iter++)
    {
        string key = iter->first;
        string value = iter->second;

        key = stringReplaceAll(key, " ", "");

        if(graphStr == key)
            return value;
    }
    return "";
}

std::string ILBatchInstructionProvider::getMatchSubGraphStrInstruction(std::string graphStr)
{
    graphStr = stringReplaceAll(graphStr, " ", "");
    for(auto iter = simdGraphStrToCalculationInstructionMap.begin(); iter != simdGraphStrToCalculationInstructionMap.end(); iter++)
    {
        string key = iter->first;
        string value = iter->second;

        key = stringReplaceAll(key, " ", "");

        if(graphStr == key)
            return value;
    }
    return "";
}

std::string ILBatchInstructionProvider::getMatchSubGraphStrInstructionNormal(std::string graphStr)
{
    graphStr = stringReplaceAll(graphStr, " ", "");
    for(auto iter = simdGraphStrToCalculationInstructionNormalMap.begin(); iter != simdGraphStrToCalculationInstructionNormalMap.end(); iter++)
    {
        string key = iter->first;
        string value = iter->second;

        key = stringReplaceAll(key, " ", "");

        if(graphStr == key)
            return value;
    }
    return "";
}
