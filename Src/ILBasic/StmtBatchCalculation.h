#pragma once
#include <map>
#include "Statement.h"



class StmtBatchCalculation:
	public Statement
{
public:
    StmtBatchCalculation();
    explicit StmtBatchCalculation(const Statement* parent);
    ~StmtBatchCalculation() override;

    class StmtBatchCalculationRange
    {
    public:
        int start = -1;
        int length = -1;
        std::vector<int> index;
    };

    class StmtBatchCalculationInput
    {
    public:
	    std::string name;
	    std::string type;
	    int isAddress = 0;
	    std::vector<int> arraySize;
        Expression* defaultValue = nullptr;
        StmtBatchCalculationRange range;
    };

    class StmtBatchCalculationOutput
    {
    public:
	    std::string name;
	    std::string type;
	    int isAddress = 0;
	    std::vector<int> arraySize;
        Expression* defaultValue = nullptr;
        StmtBatchCalculationRange range;
    };

    enum OperationType
    {
        TypeAdd,
        TypeSAdd,
        TypeSub,
        TypeSSub,
        TypeMul,
        TypeSMul,
        TypeSDMul,
        TypeDiv,
        TypeLShr,
        TypeAShr,
        TypeShl,
        TypeAnd,
        TypeOr,
        TypeXor,
        TypeLogicAnd,
        TypeLogicOr,
        TypeLogicNot,
        TypeLogicNAnd,
        TypeLogicNOr,
        TypeLogicXor,
        TypeLogicNXor,
        TypeLess,
        TypeLessEqual,
        TypeGreater,
        TypeGreaterEqual,
        TypeEqual,
        TypeNotEqual,

        TypeAssign,
        TypeReadData,

        TypeSquare,
        TypeSqrt,
        TypeRSqrt,
        TypeExp,
        TypeAbs,
        TypeDAbs,
        TypeLog,
        TypeLog10,
        TypeMod,

        TypeDotMul,
        TypeCrossMul,
        TypeFFT,
        TypeIFFT,
        TypeDCT,
        TypeIDCT,
        TypeConv,
        TypeCorrelation,

        TypeUnknown,
    };
    static std::map<std::string, OperationType> operationTypeStrToTypeMap;
    static std::map<OperationType, std::string> operationTypeToTypeStrMap;



    enum ModifierType
    {
        TypeNone,
        TypeSaturate,
        TypeHalf,
        TypeDouble,
        TypeRound,
    };
    static std::map<std::string, ModifierType> modifierTypeStrToTypeMap;
    static std::map<ModifierType, std::string> modifierTypeToTypeStrMap;


    enum ShapeType
    {
        TypeNormal,
        TypeLong,
        TypeWide,
        TypeNarrow,
    };
    static std::map<std::string, ShapeType> shapeTypeStrToTypeMap;
    static std::map<ShapeType, std::string> shapeTypeToTypeStrMap;

    
    OperationType operationType = TypeUnknown;
    std::string dataType;
    std::vector<StmtBatchCalculationInput> inputs;
    std::vector<StmtBatchCalculationOutput> outputs;

    ShapeType shapeType = TypeNormal;
    ModifierType modifierType = TypeNone;

	void release() override;

    virtual Statement* clone(Statement* parent);
};
