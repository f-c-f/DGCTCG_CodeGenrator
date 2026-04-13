#include "StmtBatchCalculation.h"

#include "Expression.h"

using namespace std;

map<string, StmtBatchCalculation::OperationType> StmtBatchCalculation::operationTypeStrToTypeMap = {
    pair<string, OperationType>("Add", TypeAdd),
    pair<string, OperationType>("SAdd", TypeSAdd),
    pair<string, OperationType>("Sub", TypeSub),
    pair<string, OperationType>("SSub", TypeSSub),
    pair<string, OperationType>("Mul", TypeMul),
    pair<string, OperationType>("SMul", TypeSMul),
    pair<string, OperationType>("SDMul", TypeSDMul),
    pair<string, OperationType>("Div", TypeDiv),
    pair<string, OperationType>("LShr", TypeLShr),
    pair<string, OperationType>("AShr", TypeAShr),
    pair<string, OperationType>("Shl", TypeShl),
    pair<string, OperationType>("And", TypeAnd),
    pair<string, OperationType>("Or", TypeOr),
    pair<string, OperationType>("Xor", TypeXor),

    pair<string, OperationType>("LogicAnd", TypeLogicAnd),
    pair<string, OperationType>("LogicOr", TypeLogicOr),
    pair<string, OperationType>("LogicNot", TypeLogicNot),
    pair<string, OperationType>("LogicNAnd", TypeLogicNAnd),
    pair<string, OperationType>("LogicNOr", TypeLogicNOr),
    pair<string, OperationType>("LogicXor", TypeLogicXor),
    pair<string, OperationType>("LogicNXor", TypeLogicNXor),
    pair<string, OperationType>("Less", TypeLess),
    pair<string, OperationType>("LessEqual", TypeLessEqual),
    pair<string, OperationType>("Greater", TypeGreater),
    pair<string, OperationType>("GreaterEqual", TypeGreaterEqual),
    pair<string, OperationType>("Equal", TypeEqual),
    pair<string, OperationType>("NotEqual", TypeNotEqual),
    
    pair<string, OperationType>("Square", TypeSquare),
    pair<string, OperationType>("Sqrt", TypeSqrt),
    pair<string, OperationType>("RSqrt", TypeRSqrt),
    pair<string, OperationType>("Exp", TypeExp),
    pair<string, OperationType>("Abs", TypeAbs),
    pair<string, OperationType>("DAbs", TypeDAbs),
    pair<string, OperationType>("Log", TypeLog),
    pair<string, OperationType>("Log10", TypeLog10),
    pair<string, OperationType>("Mod", TypeMod),

    pair<string, OperationType>("Assign", TypeAssign),
    pair<string, OperationType>("ReadData", TypeReadData),
    
    pair<string, OperationType>("DotMul", TypeDotMul),
    pair<string, OperationType>("CrossMul", TypeCrossMul),
    pair<string, OperationType>("FFT", TypeFFT),
    pair<string, OperationType>("IFFT", TypeIFFT),
    pair<string, OperationType>("DCT", TypeDCT),
    pair<string, OperationType>("IDCT", TypeIDCT),
    pair<string, OperationType>("Conv", TypeConv),
    pair<string, OperationType>("Correlation", TypeCorrelation),
};

map<StmtBatchCalculation::OperationType, string> StmtBatchCalculation::operationTypeToTypeStrMap = {
    pair<OperationType, string>(TypeAdd, "Add"),
    pair<OperationType, string>(TypeSAdd, "SAdd"),
    pair<OperationType, string>(TypeSub, "Sub"),
    pair<OperationType, string>(TypeSSub, "SSub"),
    pair<OperationType, string>(TypeMul, "Mul"),
    pair<OperationType, string>(TypeSMul, "SMul"),
    pair<OperationType, string>(TypeSDMul, "SDMul"),
    pair<OperationType, string>(TypeDiv, "Div"),
    pair<OperationType, string>(TypeLShr, "LShr"),
    pair<OperationType, string>(TypeAShr, "AShr"),
    pair<OperationType, string>(TypeShl, "Shl"),
    pair<OperationType, string>(TypeAnd, "And"),
    pair<OperationType, string>(TypeOr, "Or"),
    pair<OperationType, string>(TypeXor, "Xor"),

    pair<OperationType, string>(TypeLogicAnd, "LogicAnd"),
    pair<OperationType, string>(TypeLogicOr, "LogicOr"),
    pair<OperationType, string>(TypeLogicNot, "LogicNot"),
    pair<OperationType, string>(TypeLogicNAnd, "LogicNAnd"),
    pair<OperationType, string>(TypeLogicNOr, "LogicNOr"),
    pair<OperationType, string>(TypeLogicXor, "LogicXor"),
    pair<OperationType, string>(TypeLogicNXor, "LogicNXor"),
    pair<OperationType, string>(TypeLess, "Less"),
    pair<OperationType, string>(TypeLessEqual, "LessEqual"),
    pair<OperationType, string>(TypeGreater, "Greater"),
    pair<OperationType, string>(TypeGreaterEqual, "GreaterEqual"),
    pair<OperationType, string>(TypeEqual, "Equal"),
    pair<OperationType, string>(TypeNotEqual, "NotEqual"),
    
    pair<OperationType, string>(TypeAssign, "Assign"),
    pair<OperationType, string>(TypeReadData, "ReadData"),
    
    pair<OperationType, string>(TypeSquare, "Square"),
    pair<OperationType, string>(TypeSqrt, "Sqrt"),
    pair<OperationType, string>(TypeRSqrt, "RSqrt"),
    pair<OperationType, string>(TypeExp, "Exp"),
    pair<OperationType, string>(TypeAbs, "Abs"),
    pair<OperationType, string>(TypeDAbs, "DAbs"),
    pair<OperationType, string>(TypeLog, "Log"),
    pair<OperationType, string>(TypeLog10, "Log10"),
    pair<OperationType, string>(TypeMod, "Mod"),

    pair<OperationType, string>(TypeDotMul, "DotMul"),
    pair<OperationType, string>(TypeCrossMul, "CrossMul"),
    pair<OperationType, string>(TypeFFT, "FFT"),
    pair<OperationType, string>(TypeIFFT, "IFFT"),
    pair<OperationType, string>(TypeDCT, "DCT"),
    pair<OperationType, string>(TypeIDCT, "IDCT"),
    pair<OperationType, string>(TypeConv, "Conv"),
    pair<OperationType, string>(TypeCorrelation, "Correlation"),

};

map<string, StmtBatchCalculation::ModifierType> StmtBatchCalculation::modifierTypeStrToTypeMap = {
    pair<string,ModifierType>("None", TypeNone),
    pair<string,ModifierType>("Saturate", TypeSaturate),
    pair<string,ModifierType>("Half", TypeHalf),
    pair<string,ModifierType>("Double", TypeDouble),
    pair<string,ModifierType>("Round", TypeRound),
};

map<StmtBatchCalculation::ModifierType, string> StmtBatchCalculation::modifierTypeToTypeStrMap = {
    pair<ModifierType, string>(TypeNone, "None"),
    pair<ModifierType, string>(TypeSaturate, "Saturate"),
    pair<ModifierType, string>(TypeHalf, "Half"),
    pair<ModifierType, string>(TypeDouble, "Double"),
    pair<ModifierType, string>(TypeRound, "Round"),
};

map<string, StmtBatchCalculation::ShapeType> StmtBatchCalculation::shapeTypeStrToTypeMap = {
    pair<string,ShapeType>("Normal", TypeNormal),
    pair<string,ShapeType>("Long", TypeLong),
    pair<string,ShapeType>("Wide", TypeWide),
    pair<string,ShapeType>("Narrow", TypeNarrow),
};

map<StmtBatchCalculation::ShapeType, string> StmtBatchCalculation::shapeTypeToTypeStrMap = {
    pair<ShapeType, string>(TypeNormal, "Normal"),
    pair<ShapeType, string>(TypeLong, "Long"),
    pair<ShapeType, string>(TypeWide, "Wide"),
    pair<ShapeType, string>(TypeNarrow, "Narrow"),
};

StmtBatchCalculation::StmtBatchCalculation()
{
    this->type = Statement::BatchCalculation;
}

StmtBatchCalculation::StmtBatchCalculation(const Statement* parent)
{
    this->type = Statement::BatchCalculation;
    this->parentStatement = const_cast<Statement*>(parent);
}

StmtBatchCalculation::~StmtBatchCalculation()
{
}

void StmtBatchCalculation::release()
{
    for(int i = 0; i < this->inputs.size(); i++)
    {
        if(this->inputs[i].defaultValue)
        {
            this->inputs[i].defaultValue->release();
            delete this->inputs[i].defaultValue;
        }
    }
    for(int i = 0; i < this->outputs.size(); i++)
    {
        if(this->outputs[i].defaultValue)
        {
            this->outputs[i].defaultValue->release();
            delete this->outputs[i].defaultValue;
        }
    }
    Statement::release();
}

Statement* StmtBatchCalculation::clone(Statement* parent)
{
    StmtBatchCalculation* ret = new StmtBatchCalculation;
    ret->parentStatement = parent;
    ret->type = this->type;
    ret->depth = parent->depth + 1;
    for (int i = 0; i < this->childStatements.size(); i++)
    {
        Statement* child = this->childStatements[i]->clone(ret);
        ret->childStatements.push_back(child);
    }
    for (int i = 0; i < this->innerExpressions.size(); i++)
    {
        Expression* exp = this->innerExpressions[i]->clone();
        ret->innerExpressions.push_back(exp);
    }

    ret->operationType = this->operationType;
    ret->dataType = this->dataType;
    ret->inputs = this->inputs;
    ret->outputs = this->outputs;
    ret->shapeType = this->shapeType;
    ret->modifierType = this->modifierType;

    return ret;
}
