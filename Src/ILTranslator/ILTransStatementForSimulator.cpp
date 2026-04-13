#include "ILTransStatementForSimulator.h"

#include "ILTransFile.h"

#include "ILTranslator.h"
#include "ILTranslatorForSimulator.h"
#include "ILTransUtilityFunction.h"
#include "../Common/Utility.h"

#include "../ILBasic/StmtBreak.h"
#include "../ILBasic/StmtCase.h"
#include "../ILBasic/StmtDefault.h"
#include "../ILBasic/StmtDoWhile.h"
#include "../ILBasic/StmtElse.h"
#include "../ILBasic/StmtElseIf.h"
#include "../ILBasic/StmtExpression.h"
#include "../ILBasic/StmtFor.h"
#include "../ILBasic/StmtIf.h"
#include "../ILBasic/StmtLocalVariable.h"
#include "../ILBasic/StmtReturn.h"
#include "../ILBasic/StmtSwitch.h"
#include "../ILBasic/StmtWhile.h"
#include "../ILBasic/StmtBatchCalculation.h"
#include "../ILBasic/ILOutput.h"
#include "../ILBasic/ILInput.h"

#include "../ILBasic/ILParser.h"
#include "../ILBasic/Expression.h"
#include "../ILBasic/ILUtilityFunction.h"
#include "../ILBasic/ILHeadFile.h"

#include "../ILBasic/BasicTypeCalculator.h"
#include "../ILBasic/ILCalculate.h"
#include "../ILBasic/ILFile.h"
#include "../ILBasic/ILRef.h"
#include "../ILBasic/ILExternFile.h"

#include "../SimulatorBasic/SimulatorConfig.h"

#include <regex>
#include <limits>

using namespace std;


namespace
{
    const int CONST_NUM_2 = 2;
}


int ILTransStatementForSimulator::translate(const ILCalculate* calculate, ILTransFile* file, int count)
{
	iLTransFile = file;
    calculator = calculate;
	Statement* statement = calculator->statements.childStatements[count];
    calculatorType = calculator->refs[0]->actor;
    

	statementStr = "";
	int res = translateStatement(statement);
	if (res < 0)
		return res;

	//file->transCodeFunction += statementStr;
    statementStr = statementStr.substr(0, statementStr.length() - 2);

    if (stmtNeedRecord())
    {
        instrumentRecord();
    }

	return 1;
}

int ILTransStatementForSimulator::translateExpression(Expression* exp)
{
    if (calculatorType == "Sum" || calculatorType == "Product")
    {
        std::string outType = calculator->outputs[0]->type;
        Expression* expOut = exp->childExpressions[0];
        Expression* expRight = exp->childExpressions[1];
        Expression* expLeft = exp->childExpressions[0];

        while (1)
        {
            if (expRight->childExpressions.size() == 2)
            {
                expLeft = expRight->childExpressions[0];
                expRight = expRight->childExpressions[1];
            }
            else if (expLeft->childExpressions.size() == 2)
            {
                expRight = expLeft->childExpressions[1];
                expLeft = expLeft->childExpressions[0];
            }
            else
                break;
        }

        Expression* expParent = expLeft->parentExpression;
        for(int i = 1; expParent->parentExpression; i++)
        {
            expLeft = expParent->childExpressions[0];
            expRight = expParent->childExpressions[1];

            if (!expParent->parentExpression->parentExpression)
                statementStr += expOut->expressionStr + " = ";          
            else
                statementStr += outType + " " + expOut->expressionStr + "_" + to_string(i) + " = ";         

            if (expLeft->childExpressions.size() == 0)
            {
                statementStr += expLeft->expressionStr + " " + expParent->tokenStr + " " + expRight->expressionStr + ";\n";
                instrumentSPDiagnose(i, expParent);
            }
            else
            {
                statementStr += expOut->expressionStr + "_" + to_string(i - 1) + " " + expParent->tokenStr + " " + expRight->expressionStr + ";\n";
                expParent->childExpressions[0]->expressionStr = expOut->expressionStr + "_" + to_string(i - 1);
                instrumentSPDiagnose(i, expParent);
            }

            expParent = expParent->parentExpression;
        }
        
    }
    else if (calculatorType == "Gain")
    {
        statementStr += exp->expressionStr;
        statementStr += ";\n";
        instrumentSPDiagnose(0, exp->childExpressions[1]);
    }
    else
    {
        statementStr += exp->expressionStr;
        statementStr += ";\n";
        if (stmtNeedDiagnose())
            instrumentDiagnose();
    }
    


    return 1;
}

const int ILTransStatementForSimulator::isFunctionExec(const ILCalculate* calculator)
{
    const ILObject* p = calculator->parent;
    while (p->objType != ILObject::TypeFunction)
        p = p->parent;
    ILFunction* func = (ILFunction*)p;

    if (func->type == ILFunction::FunctionExec)
            return 1;

    return 0;
}

const int ILTransStatementForSimulator::stmtNeedRecord()
{
    std::string calculatorPath;

    if (calculatorType == "Outport")
        calculatorPath = calculator->refs[0]->path + "." + calculator->outputs[0]->name;
    else
        calculatorPath = calculator->refs[0]->path;

    auto it = std::find(ILTranslatorForSimulator::simuConfig->actorPathNeedRecord.begin(), ILTranslatorForSimulator::simuConfig->actorPathNeedRecord.end(), calculatorPath);

    if (it != ILTranslatorForSimulator::simuConfig->actorPathNeedRecord.end())
        return 1;

	return 0;
}

const int ILTransStatementForSimulator::stmtNeedDiagnose()
{
	if (calculatorType == "Sum" ||
		calculatorType == "Product" ||
		calculatorType == "Gain" ||
		calculatorType == "Abs" ||
		calculatorType == "Sqrt" ||
		calculatorType == "Math" ||
		calculatorType == "DotProduct" ||
		calculatorType == "Assignment" ||
		calculatorType == "Outport")
		return 1;

	return 0;
}

const void ILTransStatementForSimulator::instrumentRecord()
{
	for (int i = 0; i < calculator->outputs.size(); i++)
	{
        ILOutput* out = calculator->outputs[i];
		string monitorCode;
		monitorCode += "SimulatorMonitor::monitorRecordValue(";
		monitorCode += to_string(ILTranslatorForSimulator::outID);
		ILTranslatorForSimulator::needMonitor[calculator->refs[0]->path + "_" + out->name] = make_pair(ILTranslatorForSimulator::outID, out->type);
		ILTranslatorForSimulator::outID++;

		monitorCode += ", (u8*)(";

		if (calculator->refs[0]->actor == "Outport")
			monitorCode += out->name;
		else
			monitorCode += "&" + calculator->name + "_" + out->name;

		monitorCode += "));\n";
		statementStr += monitorCode;
	}
}

const void ILTransStatementForSimulator::instrumentDiagnose()
{
    string ope = getOperator(statementStr, calculatorType);

    string calculatorPath = stringReplaceAll(calculator->refs[0]->path, ".", "_");
    string output = calculator->name + "_" + calculator->outputs[0]->name;
    string input1 = calculator->inputs[0]->sourceStr;
    string input2;
    

    string outType = calculator->outputs[0]->type;
    string inType1 = calculator->inputs[0]->type;
    string inType2 = "";

    if (calculator->inputs.size() > 1)
    {
        inType2 = calculator->inputs[1]->type;
        input2 = calculator->inputs[1]->sourceStr;
    }

    //生成诊断函数
    if (calculatorType == "Outport")
        statementStr += "diagnoseCalculator_" + calculatorPath + "_" + calculator->outputs[0]->name + "(*" + calculator->outputs[0]->name + ", " + input1 + ");\n";
    else if (calculator->inputs.size() == 1)
        statementStr += "diagnoseCalculator_" + calculatorPath + "(" + output + ", " + input1 + ");\n";
    else if (calculatorType == "Assignment")
        statementStr += "diagnoseCalculator_" + calculatorPath + "(" + output + "[" + calculator->inputs[2]->sourceStr + "], " + input2 + ", " + calculator->inputs[2]->sourceStr + ");\n";
    else
        statementStr += "diagnoseCalculator_" + calculatorPath + "(" + output + ", " + input1 + ", " + input2 + ");\n";

    creatDiagLogic(calculatorPath, ope, outType, inType1, inType2);
}

//Sum和Product组件的诊断插桩 
const void ILTransStatementForSimulator::instrumentSPDiagnose(int count, Expression* parent)
{
    string diagnoseLogic;
    string diagnoseLogicH;

    string ope = parent->tokenStr;
    string leftV = parent->childExpressions[0]->expressionStr;
    string rightV = parent->childExpressions[1]->expressionStr;
    string outV, isDiag, calculatorPath;
    string outType, inType1, inType2;

    outType = calculator->outputs[0]->type;
    inType2 = calculator->inputs[count]->type;
    if (calculatorType == "Gain")
        inType1 = "f64";
    else if (parent->childExpressions[0]->childExpressions.size() == 0)
        inType1 = calculator->inputs[0]->type;
    else
        inType1 = outType;

    if (parent->parentExpression->parentExpression)
    {
        outV = calculator->name + "_" + calculator->outputs[0]->name + "_" + to_string(count);
        calculatorPath = stringReplaceAll(calculator->refs[0]->path, ".", "_") + "_" + to_string(count);
    } 
    else
    {
        outV = calculator->name + "_" + calculator->outputs[0]->name;
        calculatorPath = stringReplaceAll(calculator->refs[0]->path, ".", "_");
    }

    statementStr += "diagnoseCalculator_" + calculatorPath + "(";
    statementStr += outV + ", " + leftV + ", " + rightV + ");\n";

    creatDiagLogic(calculatorPath, ope, outType, inType1, inType2);
}

const void ILTransStatementForSimulator::creatDiagLogic(std::string calculatorPath, std::string ope, std::string outType, std::string inType1, std::string inType2)
{
    string diagnoseLogic;
    string diagnoseLogicH;

    if (calculatorType == "Outport")
    {
        diagnoseLogic += "void diagnoseCalculator_" + calculatorPath + "_" + calculator->outputs[0]->name + "(" + outType + " output, " + inType1 + " input1)\n";
        diagnoseLogicH += "void diagnoseCalculator_" + calculatorPath + "_" + calculator->outputs[0]->name + "(" + outType + " output, " + inType1 + " input1);\n";
        //diagnoseLogicH += "bool Flag_" + calculatorPath + " = false;\n";
    }
    else if (calculatorType == "Assignment")
    {
        diagnoseLogic += "int isDiag_" + calculatorPath + " = 0;\n";
        diagnoseLogic += "void diagnoseCalculator_" + calculatorPath + "(" + outType + " output, " + inType2 + " input1, " + calculator->inputs[2]->type + " input2)\n";
        diagnoseLogicH += "void diagnoseCalculator_" + calculatorPath + "(" + outType + " output, " + inType2 + " input1, " + calculator->inputs[2]->type + " input2);\n";
        //diagnoseLogicH += "int isDiag_" + calculatorPath + " = 0;\n";
    }
    else if (inType2 == "")
    {
        diagnoseLogic += "int isDiag_" + calculatorPath + " = 0;\n";
        diagnoseLogic += "void diagnoseCalculator_" + calculatorPath + "(" + outType + " output, " + inType1 + " input1)\n";
        diagnoseLogicH += "void diagnoseCalculator_" + calculatorPath + "(" + outType + " output, " + inType1 + " input1);\n";
        //diagnoseLogicH += "int isDiag_" + calculatorPath + " = 0;\n";
    }
    else
    {
        diagnoseLogic += "int isDiag_" + calculatorPath + " = 0;\n";
        diagnoseLogic += "void diagnoseCalculator_" + calculatorPath + "(" + outType + " output, " + inType1 + " input1, " + inType2 + " input2)\n";
        diagnoseLogicH += "void diagnoseCalculator_" + calculatorPath + "(" + outType + " output, " + inType1 + " input1, " + inType2 + " input2);\n";
        //diagnoseLogicH += "int isDiag_" + calculatorPath + " = 0;\n";
    }

    diagnoseLogic += "{\n";

    //溢出时绕回
    if (needIntegerOverflow())
    {
        if (ope == "+")
        {
            diagnoseLogic += "  if ((input1 > 0 && input2 > 0 && output < 0) || (input1 < 0 && input2 < 0 && output > 0)) {\n";
            diagnoseLogic += "    if (isDiag_" + calculatorPath + " == 0) {\n";
            diagnoseLogic += "    isDiag_" + calculatorPath + " = 1;\n";
            diagnoseLogic += "    clock_t detect_time = clock();\n";
            diagnoseLogic += "    double time_used = ((double) (detect_time - Start_Time)) / CLOCKS_PER_SEC;\n";
            diagnoseLogic += "    printf(\"WARRING: Wrap on overflow occur on " + calculatorPath + "!\\n\");\n";
            diagnoseLogic += "    printf(\"Detect at %.3f seconds of simulation\\n\", time_used);\n    }\n  }\n";
        }
        if (ope == "-")
        {
            diagnoseLogic += "  if ((input1 > 0 && input2 < 0 && output < 0) || (input1 < 0 && input2 > 0 && output > 0)) {\n";
            diagnoseLogic += "    if (isDiag_" + calculatorPath + " == 0) {\n";
            diagnoseLogic += "    isDiag_" + calculatorPath + " = 1;\n";
            diagnoseLogic += "    clock_t detect_time = clock();\n";
            diagnoseLogic += "    double time_used = ((double) (detect_time - Start_Time)) / CLOCKS_PER_SEC;\n";
            diagnoseLogic += "    printf(\"WARRING: Wrap on overflow occur on " + calculatorPath + "!\\n\");\n";
            diagnoseLogic += "    printf(\"Detect at %.3f seconds of simulation\\n\", time_used);\n    }\n  }\n";
        }
        if (ope == "*")
        {
            diagnoseLogic += "  if (input1 != 0 && input2 != 0 && output/input1 != input2) {\n";
            diagnoseLogic += "    if (isDiag_" + calculatorPath + " == 0) {\n";
            diagnoseLogic += "    isDiag_" + calculatorPath + " = 1;\n";
            diagnoseLogic += "    clock_t detect_time = clock();\n";
            diagnoseLogic += "    double time_used = ((double) (detect_time - Start_Time)) / CLOCKS_PER_SEC;\n";
            diagnoseLogic += "    printf(\"WARRING: Wrap on overflow occur on " + calculatorPath + "!\\n\");\n";
            diagnoseLogic += "    printf(\"Detect at %.3f seconds of simulation\\n\", time_used);\n    }\n  }\n";
        }
        /*else if (ope == "*Gain")
        {
            double para = extractNumberInGainActor(calculatorStmtStr);
            if (std::fabs(para) < std::numeric_limits<double>::epsilon())
            {
                diagnoseLogic += "  if (output != 0) {\n";
                diagnoseLogic += "    clock_t detect_time = clock();\n";
                diagnoseLogic += "    double time_used = ((double) (detect_time - Start_Time)) / CLOCKS_PER_SEC;\n";
                diagnoseLogic += "    printf(\"WARRING: Wrap on overflow occur on " + calculator->refs[0]->path + "!\\n\");\n";
                diagnoseLogic += "    printf(\"Detect at %.3f seconds of simulation\\n\", time_used);\n  }\n";
            }
            else
            {
                diagnoseLogic += "  if (input1 != 0 && std::fabs(output / input1 - " + to_string(para) + ") < std::numeric_limits<double>::epsilon()) {\n";
                diagnoseLogic += "    clock_t detect_time = clock();\n";
                diagnoseLogic += "    double time_used = ((double) (detect_time - Start_Time)) / CLOCKS_PER_SEC;\n";
                diagnoseLogic += "    printf(\"WARRING: Wrap on overflow occur on " + calculator->refs[0]->path + "!\\n\");\n";
                diagnoseLogic += "    printf(\"Detect at %.3f seconds of simulation\\n\", time_used);\n  }\n";
            }
        }*/
        else if (ope == "*Math")
        {
            diagnoseLogic += "  if (input1 != 0  && output/input1 != input1) {\n";
            diagnoseLogic += "    if (isDiag_" + calculatorPath + " == 0) {\n";
            diagnoseLogic += "    isDiag_" + calculatorPath + " = 1;\n";
            diagnoseLogic += "    clock_t detect_time = clock();\n";
            diagnoseLogic += "    double time_used = ((double) (detect_time - Start_Time)) / CLOCKS_PER_SEC;\n";
            diagnoseLogic += "    printf(\"WARRING: Wrap on overflow occur on " + calculatorPath + "!\\n\");\n";
            diagnoseLogic += "    printf(\"Detect at %.3f seconds of simulation\\n\", time_used);\n    }\n  }\n";
        }
        if (ope == "exp")
        {
            if (outType == "f32")
            {
                diagnoseLogic += "  if (input1 > 87.5f) {\n";
                diagnoseLogic += "    if (isDiag_" + calculatorPath + " == 0) {\n";
                diagnoseLogic += "    isDiag_" + calculatorPath + " = 1;\n";
                diagnoseLogic += "    clock_t detect_time = clock();\n";
                diagnoseLogic += "    double time_used = ((double) (detect_time - Start_Time)) / CLOCKS_PER_SEC;\n";
                diagnoseLogic += "    printf(\"WARRING: Wrap on overflow occur on " + calculatorPath + "!\\n\");\n";
                diagnoseLogic += "    printf(\"Detect at %.3f seconds of simulation\\n\", time_used);\n    }\n  }\n";
            }
            if (outType == "f64")
            {
                diagnoseLogic += "  if (input1 > 709.78) {\n";
                diagnoseLogic += "    if (isDiag_" + calculatorPath + " == 0) {\n";
                diagnoseLogic += "    isDiag_" + calculatorPath + " = 1;\n";
                diagnoseLogic += "    clock_t detect_time = clock();\n";
                diagnoseLogic += "    double time_used = ((double) (detect_time - Start_Time)) / CLOCKS_PER_SEC;\n";
                diagnoseLogic += "    printf(\"WARRING: Wrap on overflow occur on " + calculatorPath + "!\\n\");\n";
                diagnoseLogic += "    printf(\"Detect at %.3f seconds of simulation\\n\", time_used);\n    }\n  }\n";
            }
        }
    }

    //溢出时饱和
    if (needIntegerSaturation())
    {

    }

    //参数向下转换
    if (needParameterDowncast())
    {
        if (ope != "log10" || ope != "log" || calculator->refs[0]->actor == "Assignment")
        {
            if (calculator->inputs.size() == 1 || calculator->refs[0]->actor == "Assignment")
            {
                diagnoseLogic += "  if(sizeof(output) < sizeof(input1)) {\n";
                diagnoseLogic += "    clock_t detect_time = clock();\n";
                diagnoseLogic += "    double time_used = ((double) (detect_time - Start_Time)) / CLOCKS_PER_SEC;\n";
                diagnoseLogic += "    printf(\"WARRING: Downcast may exist on " + calculatorPath + "!\\n\");\n";
                diagnoseLogic += "    printf(\"Detect at %.3f seconds of simulation\\n\", time_used);\n  }\n";
            }
            else
            {
                diagnoseLogic += "  if(sizeof(output) < sizeof(input1) || sizeof(output) < sizeof(input2)) {\n";
                diagnoseLogic += "    clock_t detect_time = clock();\n";
                diagnoseLogic += "    double time_used = ((double) (detect_time - Start_Time)) / CLOCKS_PER_SEC;\n";
                diagnoseLogic += "    printf(\"WARRING: Downcast may exist on " + calculatorPath + "!\\n\");\n";
                diagnoseLogic += "    printf(\"Detect at %.3f seconds of simulation\\n\", time_used);\n  }\n";
            }
        }
    }

    //参数溢出
    if (needParameterOverflow())
    {

    }

    //精度损失
    if (needParameterPrecisionLoss())
    {
        /*if (ope == "/" || ope == "sqrt" || ope == "exp" || ope == "log10" || ope == "log")
        {
            if(!(outType == "f32" || outType == "f64"))
                diagnoseLogic += "  printf(\"WARRING: Precision Lose on " + calculatorPath + "!\\n\");\n";
        }*/
    }

    //int32转换float
    if (needInt32ToFloatConv())
    {
        if (ope == "+" || ope == "-" || ope == "*" || ope == "/" || ope == "abs")
        {
            if (outType == "f32")
            {
                if (inType1 == "i32")
                {
                    diagnoseLogic += "  if(input1 < -16777216 || input1 > 16777216) {n";
                    diagnoseLogic += "    clock_t detect_time = clock();\n";
                    diagnoseLogic += "    double time_used = ((double) (detect_time - Start_Time)) / CLOCKS_PER_SEC;\n";
                    diagnoseLogic += "    printf(\"ERROR: int32 transfer to float overflow on " + calculatorPath + "!\\n\");\n";
                    diagnoseLogic += "    printf(\"Detect at %.3f seconds of simulation\\n\", time_used);\n    }\n";
                }
                if (calculator->inputs.size() == 2)
                {
                    if (inType1 == "i32")
                    {
                        diagnoseLogic += "  if(input2 < -16777216 || input2 > 16777216) {\n";
                        diagnoseLogic += "    clock_t detect_time = clock();\n";
                        diagnoseLogic += "    double time_used = ((double) (detect_time - Start_Time)) / CLOCKS_PER_SEC;\n";
                        diagnoseLogic += "    printf(\"ERROR: int32 transfer to float overflow on " + calculatorPath + "!\\n\");\n";
                        diagnoseLogic += "    printf(\"Detect at %.3f seconds of simulation\\n\", time_used);\n    }\n";
                    }
                }
            }
        }
    }

    //数组越界
    if (needArrayOutOfBound())
    {
        if (calculator->refs[0]->actor == "Assignment")
        {
            diagnoseLogic += "  if(input2 > " + std::to_string(calculator->inputs[0]->arraySize[0] - 1) + " || input2 < 0) {\n";
            diagnoseLogic += "    if (isDiag_" + calculatorPath + " == 0) {\n";
            diagnoseLogic += "    isDiag_" + calculatorPath + " = 1;\n";
            diagnoseLogic += "    clock_t detect_time = clock();\n";
            diagnoseLogic += "    double time_used = ((double) (detect_time - Start_Time)) / CLOCKS_PER_SEC;\n";
            diagnoseLogic += "    printf(\"ERROR: Array out of Bound on " + calculatorPath + "!\\n\");\n";
            diagnoseLogic += "    printf(\"Detect at %.3f seconds of simulation\\n\", time_used);\n     }\n  }\n";
        }
    }

    //除零异常
    if (needDevideByZero())
    {
        if (ope == "/")
        {
            diagnoseLogic += "  if(input2 == 0) {\n";
            diagnoseLogic += "    clock_t detect_time = clock();\n";
            diagnoseLogic += "    double time_used = ((double) (detect_time - Start_Time)) / CLOCKS_PER_SEC;\n";
            diagnoseLogic += "    printf(\"ERROR: Devided by Zero on " + calculatorPath + "!\\n\");\n";
            diagnoseLogic += "    printf(\"Detect at %.3f seconds of simulation\\n\", time_used);\n  }\n";
        }
        if (ope == "sqrt")
        {
            diagnoseLogic += "  if(input1 < 0) {\n";
            diagnoseLogic += "    if (isDiag_" + calculatorPath + " == 0) {\n";
            diagnoseLogic += "    isDiag_" + calculatorPath + " = 1;\n";
            diagnoseLogic += "    clock_t detect_time = clock();\n";
            diagnoseLogic += "    double time_used = ((double) (detect_time - Start_Time)) / CLOCKS_PER_SEC;\n";
            diagnoseLogic += "    printf(\"ERROR: Parameter of sqrt must not be less than zero on " + calculatorPath + "!\\n\");\n";
            diagnoseLogic += "    printf(\"Detect at %.3f seconds of simulation\\n\", time_used);\n    }\n  }\n";
        }
        if (ope == "log" || ope == "log10")
        {
            diagnoseLogic += "  if(input1 <= 0) {\n";
            diagnoseLogic += "    if (isDiag_" + calculatorPath + " == 0) {\n";
            diagnoseLogic += "    isDiag_" + calculatorPath + " = 1;\n";
            diagnoseLogic += "    clock_t detect_time = clock();\n";
            diagnoseLogic += "    double time_used = ((double) (detect_time - Start_Time)) / CLOCKS_PER_SEC;\n";
            diagnoseLogic += "    printf(\"ERROR: Parameter of log must be lager than zero on " + calculatorPath + "!\\n\");\n";
            diagnoseLogic += "    printf(\"Detect at %.3f seconds of simulation\\n\", time_used);\n    }\n  }\n";
        }
    }

    diagnoseLogic += "}\n\n";

    ILTranslatorForSimulator::diagnoseFile += diagnoseLogic;
    ILTranslatorForSimulator::diagnoseHFile += diagnoseLogicH;
}


const std::string ILTransStatementForSimulator::getOperator(std::string statementStr, std::string calculatorType)
{
    /*if (calculatorType == "Sum")
    {
        if (statementStr.find("+") != std::string::npos)
            return "+";
        else if (statementStr.find("-") != std::string::npos)
            return "-";
    }
    else if (calculatorType == "Product")
    {
        if (statementStr.find("*") != std::string::npos)
            return "*";
        else if (statementStr.find("/") != std::string::npos)
            return "/";
    }
    else if (calculatorType == "Gain")
    {
        return "*Gain";
    }*/
    if (calculatorType == "Abs")
    {
        return "abs";
    }
    else if (calculatorType == "Sqrt")
    {
        return "sqrt";
    }
    else if (calculatorType == "Math")
    {
        if (statementStr.find("exp") != std::string::npos)
            return "exp";
        else if (statementStr.find("log10") != std::string::npos)
            return "log10";
        else if (statementStr.find("log") != std::string::npos)
            return "log";
        else if (statementStr.find("*") != std::string::npos)
            return "*Math";
    }
    else if (calculatorType == "DotProduct")
    {
        return "*";
    }
    return " ";
}

const bool ILTransStatementForSimulator::needIntegerOverflow()
{
    if (ILTranslatorForSimulator::simuConfig->diagOption.IntegerOverflow)
        return true;
    return false;
}

const bool ILTransStatementForSimulator::needIntegerSaturation()
{
    if (ILTranslatorForSimulator::simuConfig->diagOption.IntegerSaturation)
        return true;
    return false;
}

const bool ILTransStatementForSimulator::needParameterDowncast()
{
    if (ILTranslatorForSimulator::simuConfig->diagOption.ParameterDowncast)
        return true;
    return false;
}

const bool ILTransStatementForSimulator::needParameterOverflow()
{
    if (ILTranslatorForSimulator::simuConfig->diagOption.ParameterOverflow)
        return true;
    return false;
}

const bool ILTransStatementForSimulator::needParameterPrecisionLoss()
{
    if (ILTranslatorForSimulator::simuConfig->diagOption.ParameterPrecisionLoss)
        return true;
    return false;
}

const bool ILTransStatementForSimulator::needInt32ToFloatConv()
{
    if (ILTranslatorForSimulator::simuConfig->diagOption.Int32ToFloatConv)
        return true;
    return false;
}

const bool ILTransStatementForSimulator::needArrayOutOfBound()
{
    if (ILTranslatorForSimulator::simuConfig->diagOption.ArrayOutOfBound)
        return true;
    return false;
}

const bool ILTransStatementForSimulator::needDevideByZero()
{
    if (ILTranslatorForSimulator::simuConfig->diagOption.DevideByZero)
        return true;
    return false;
}
