#pragma once
#include <map>
#include <string>
#include <unordered_map>

#include "../ILBasic/StmtBatchCalculation.h"

class ILBatchCalculationGraph;

class ILBatchInstruction
{
public:

    // 形如 %O_1_1_1 = _mm_mul_ps(%I_1_1_1, %I_1_1_2);
    // 原则上不应该有多行，除非需要数据类型转换
    std::string format;

    // 通过将graph的inputs和outputs带入 %O_1_1_1 = _mm_mul_ps(%I_1_1_1, %I_1_1_2);
    // 获得格式化后的代码
    std::string getFormattedCode(const ILBatchCalculationGraph& graph, const std::map<std::string, std::string> &currentNodeTypeConvertMap) const;

    std::string getFormattedDataLoadCode(std::string inputData) const;
    std::string getFormattedDataLoadCode(std::string inputData, std::string outputData) const;
    std::string getFormattedDataStoreCode(std::string inputData, std::string outputData) const;

    std::string getFormattedDataTypeConvertCode(std::string inputData, std::string outputData) const;

};

class ILBatchInstructionProvider
{
public:
    static bool loadedInstructionConfig;
    static void loadInstructionConfig();
    static void loadInstructionConfigVectorRegisterWidth(std::string context);
    static void loadInstructionConfigHeadFileList(std::string context);
    static void loadInstructionConfigSimdGraphStrToDataTypeMap(std::string context);
    static void loadInstructionConfigSimdGraphStrToCalculationInstructionMap(std::string context);
    static void loadInstructionConfigSimdGraphStrToTypeConvertInstructionMap(std::string context);
    static void loadInstructionConfigSimdGraphStrToDataLoadInstructionMap(std::string context);
    static void loadInstructionConfigSimdGraphStrToDataStoreInstructionMap(std::string context);
    static void loadInstructionConfigSimdGraphStrToCalculationInstructionNormalMap(std::string context);

    static int vectorRegisterWidth;

    static std::vector<std::string> getHeadFileList();

    static int calculateInstructionBatchCount(const ILBatchCalculationGraph& graph);
    
    static std::string getBatchVariableType(std::string dataType, int batchCount);

    static int getBatchLoadInstruction(std::string dataType, int batchCount, int loadCount, ILBatchInstruction& instruction);

    static int getBatchStoreInstruction(std::string dataType, int batchCount, ILBatchInstruction& instruction);

    static int getBatchDataConvertInstruction(std::string dataTypeSrc, std::string dataTypeDst, int batchCount, ILBatchInstruction& instruction);


	static int getInstruction(const ILBatchCalculationGraph& graph, int batchCount, ILBatchInstruction& instruction);
    static int getNormalInstruction(const ILBatchCalculationGraph& graph, int batchCount, ILBatchInstruction& instruction);



private:
    static std::vector<std::string> headFileList;

    static std::map<std::string, std::string> simdGraphStrToDataTypeMap;
    static std::string getSimdDataTypeByGraphStrMap(std::string graphStr);
    static std::map<std::string, std::string> simdGraphStrToCalculationInstructionMap;
    static std::string getSimdCalculationInstructionByGraphStrMap(std::string graphStr);
    static std::map<std::string, std::string> simdGraphStrToTypeConvertInstructionMap;
    static std::string getSimdTypeConvertInstructionByGraphStrMap(std::string graphStr);
    static std::map<std::string, std::string> simdGraphStrToDataLoadInstructionMap;
    static std::string getSimdDataLoadInstructionByGraphStrMap(std::string graphStr);
    static std::map<std::string, std::string> simdGraphStrToDataStoreInstructionMap;
    static std::string getSimdDataStoreInstructionByGraphStrMap(std::string graphStr);


    static std::map<StmtBatchCalculation::OperationType, std::string> operationTypeToTypeStrMap;
    static std::string getOperationStrByOperationTypeMap(StmtBatchCalculation::OperationType type);

    static std::map<StmtBatchCalculation::ModifierType, std::string> modifierTypeToTypeStrMap;
    static std::string getModifierStrByModifierTypeMap(StmtBatchCalculation::ModifierType type);
    
    static std::map<StmtBatchCalculation::ShapeType, std::string> shapeTypeToTypeStrMap;
    static std::string getShapeStrByShapeTypeMap(StmtBatchCalculation::ShapeType type);

    
    static std::map<std::string, std::string> simdGraphStrToCalculationInstructionNormalMap;
    static std::string getSimdCalculationInstructionNormalByGraphStrMap(std::string graphStr);

    
    static std::string translateInstructionStrMapToGraph(ILBatchInstruction& instruction);
    static std::string translateInstructionStrMapToGraph(const ILBatchCalculationGraph& graph, ILBatchInstruction& instruction);
    static void translateInstructionStrMapToGraphCollectVarMap(const ILBatchCalculationGraph& graph, std::map<std::string, bool> &innerVarMap, std::map<std::string, std::string> &outputVarMap, int &outputCount);

    static std::string getSubGraphString(const ILBatchCalculationGraph& graph, int batchCount);

    
    static std::string getMatchSubGraphStrDataType(std::string graphStr);

    static std::string getMatchSubGraphStrLoadInstruction(std::string graphStr);
    static std::string getMatchSubGraphStrStoreInstruction(std::string graphStr);
    static std::string getMatchSubGraphStrDataConvertInstruction(std::string graphStr);
    
    static std::string getMatchSubGraphStrInstruction(std::string graphStr);
    static std::string getMatchSubGraphStrInstructionNormal(std::string graphStr);


};
