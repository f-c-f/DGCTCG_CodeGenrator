#include "ILTransCalculate.h"

#include "ILTransFile.h"
#include "ILTranslator.h"
#include "ILTransStatementForCBMC.h"
#include "../ILBasic/ILCalculate.h"
#include "../ILBasic/ILRef.h"
#include "../ILBasic/ILFile.h"
#include "../ILBasic/ILStructDefine.h"
#include "../ILBasic/StmtExpression.h"
#include "../ILBasic/Expression.h"
#include "../ILBasic/ILFunction.h"
#include "../ILBasic/ILOutput.h"
#include "../ILBasic/ILInput.h"
#include "../Common/Utility.h"
#include <iostream>

#include "ILTransStatementForTCGStateSearch.h"
#include "ILTransInstrumentModelUnitCoverage.h"
#include "ILTransStatementForLibFuzzer.h"
#include "ILTransStatementForTCGHybrid.h"
#include "ILTransStatementForCoverage.h"
#include "ILTransStatementForSimulator.h"

#include "../ILBasic/ILAnalyzeSchedule.h"

using namespace std;

namespace 
{
    const int CONST_NUM_7 = 7;
}

int ILTransCalculate::translate(const ILCalculate* calculate, ILTransFile* file, const ILAnalyzeSchedule* iLTranslateSchedule) const
{
    int res;
    if (ILTranslator::configGenerateComment)
    {
        string commentStr = "/* ";
        commentStr += "Calculate code of ";
        commentStr += ILTranslator::getRefComment(&(calculate->refs));
        commentStr += " */\n";
        file->transCodeFunction += commentStr;
    }

    string calculateStmtStr;

    if (ILTranslator::translateForTCGStateSearch ||
        ILTranslator::translateForTCGHybrid || ILTranslator::translateForCoverage)
    {
        InsertModelUnitCoverageCollection(calculate, file);
    }

    for (int i = 0; i < calculate->statements.childStatements.size(); i++)
    {
        string statementStr;
        if (ILTranslator::translateForCBMC)
        {
            ILTransStatementForCBMC iLTransStatement;
            res = iLTransStatement.translate(calculate->statements.childStatements[i], file, nullptr);
            if (res < 0)
                return res;
            statementStr = iLTransStatement.statementStr;
        }
        else if (ILTranslator::translateForTCGStateSearch)
        {
            ILTransStatementForTCGStateSearch iLTransStatement;
            res = iLTransStatement.translate(calculate->statements.childStatements[i], file, nullptr);
            if (res < 0)
                return res;
            statementStr = iLTransStatement.statementStr;
        }
        else if (ILTranslator::translateForLibFuzzer)
        {
            ILTransStatementForLibFuzzer iLTransStatement;
            res = iLTransStatement.translate(calculate->statements.childStatements[i], file, nullptr);
            if (res < 0)
                return res;
            statementStr = iLTransStatement.statementStr;
        }
        else if (ILTranslator::translateForTCGHybrid)
        {
            ILTransStatementForTCGHybrid iLTransStatement;
            res = iLTransStatement.translate(calculate->statements.childStatements[i], file, nullptr);
            if (res < 0)
                return res;
            statementStr = iLTransStatement.statementStr;
        }
        else if (ILTranslator::translateForCoverage)
        {
            ILTransStatementForCoverage iLTransStatement;
            res = iLTransStatement.translate(calculate->statements.childStatements[i], file, nullptr);
            if (res < 0)
                return res;
            statementStr = iLTransStatement.statementStr;
        }
        else if (ILTranslator::translateForSimulator)
        {
            ILTransStatementForSimulator iLTransStatement;
            res = iLTransStatement.translate(calculate, file, i);
            if (res < 0)
                return res;
            statementStr = iLTransStatement.statementStr;
        }
        else
        {
            ILTransStatement iLTransStatement;
            // configPackageVariable为真，进行变量打包
            res = iLTransStatement.translate(calculate->statements.childStatements[i], file, iLTranslateSchedule);
            if (res < 0)
                return res;
            statementStr = iLTransStatement.statementStr;
        }

        calculateStmtStr += statementStr;
    }

    vector<string> stmtLines = stringSplit(calculateStmtStr, "\n");
    if (stmtLines.size() < CONST_NUM_7 && ILTranslator::translateForC && !ILTranslator::configMergeBatchCalculation)
    {
        res = ILTransStatement::translateStmtBatchCalculationLoopUnrolling(calculateStmtStr);
        if (res < 0)
            return res;
    }

    file->transCodeFunction += calculateStmtStr;
	return 1;
}
//// 修改Expression
//void ILTransCalculate::modifyForPackage(const ILFunction* function, const Statement* statement, const ILTransFile* file) const
//{
//    // 对当前函数的self指针进行参数打包
//    ILStructDefine* temp = nullptr;
//    // function的第一个参数为self表示存在对应的结构体
//    if (function->inputs.size() > 0 && function->inputs[0]->name == "self")
//    {
//        // 寻找对应的Struct
//        std::string structName = function->inputs[0]->type;
//        for (auto& structDeifne : file->file->structDefines)
//        {
//            if (structDeifne->name == structName)
//            {
//                temp = structDeifne;
//                break;
//            }
//        }
//    }
//    else {
//        // 不存在self结构体，无法进行参数打包
//        // 或许考虑引入Error类型
//        return;
//    }
//    if (statement->type == Statement::ExpressionWithSemicolon)
//    {
//        const StmtExpression* stmtTemp = static_cast<const StmtExpression*>(statement);
//        updateExpressionForPackage(function, stmtTemp->expression, temp);
//        stmtTemp->expression->updateExpressionStr();
//    }
//
//}
//
//void ILTransCalculate::updateExpressionForPackage(const ILFunction* function, Expression* expression, ILStructDefine* structDefine) const
//{
//    // 暂时没有考虑变量的作用域，可能会存在问题
//    // 不同作用域的变量可能存在名称重复
//    for (auto& exp : expression->childExpressions)
//    {
//        updateExpressionForPackage(function, exp, structDefine);
//    }
//    if (expression->type == Token::Variable)
//    {
//        if (structDefine->getMemberByName(expression->expressionStr) != nullptr)
//        {
//            int index = 0;
//            for (auto child : expression->parentExpression->childExpressions)
//            {
//                if (child == expression)
//                {
//                    break;
//                }
//                index++;
//            }
//            bool isOutput = false;
//            for (auto& output : function->outputs)
//            {
//                if (output->name == expression->expressionStr)
//                {
//                    isOutput = true;
//                    break;
//                }
//            }
//            Expression* arrow = new Expression;
//            arrow->isExpression = true;
//            arrow->type = Token::Arrow;
//            arrow->tokenStr = "->";
//            arrow->expressionStr = "self -> "+expression->expressionStr;
//            Expression* self = new Expression;
//            self->isExpression = true;
//            self->type = Token::Variable;
//            self->expressionStr = "self";
//            self->tokenStr = self->expressionStr;
//            self->parentExpression = arrow;
//            arrow->childExpressions.push_back(self);
//            arrow->childExpressions.push_back(expression->clone());
//            if (isOutput)
//            {
//                arrow->childExpressions[1]->parentExpression = arrow;
//                Expression* parent = expression->parentExpression;
//                expression->parentExpression = arrow;
//                parent->replace(arrow);
//                delete arrow;
//                delete self;
//                arrow = NULL;
//                self = NULL;
//            }
//            else {
//                arrow->parentExpression = expression->parentExpression;
//                arrow->childExpressions[1]->parentExpression = arrow;
//                arrow->parentExpression->childExpressions[index] = arrow;
//                delete expression;
//                expression = NULL;
//            }
//        }
//    }
//}
