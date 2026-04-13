#pragma once
#include <vector>
#include <map>
#include <string>

class StmtDoWhile;
class StmtElseIf;
class StmtWhile;
class StmtElse;
class StmtIf;
class Expression;
class Statement;
class ILCalculate;
class ILBranch;
class ILSchedule;
class ILFunction;
class ILParser;
class ILFile;



class ILPreprocessor
{
public:
    int preprocess(ILParser* iLParser);
    void setConfigInstrumentDecision(bool value);
    void setConfigInstrumentCondition(bool value);
    void setConfigAddSwitchMCDC(bool value);
    void setConfigInstrumentMCDC(bool value);
    void setConfigCompleteElse(bool value);
    
    static std::vector<Statement*> extraStatementInstrumented;


    enum {
		ErrorILPreprocessorInnerError = 210001,
    };

    static std::vector<std::string> errorStrList;
    
	static std::string getErrorStr();
	static void setCurrentError(int errorCode, std::string msg);
    
private:
	static int errorCode;
	static std::string errorStr;

private:


    static bool configInstrumentDecision;
    static bool configInstrumentCondition;
    static bool configAddSwitchMCDC;
    static bool configInstrumentMCDC;
    static bool configCompeleteElse;

    // 采用两种不同顺序遍历数组，是因为有时对元素的修改是会对前面造成影响、
    // 正序遍历相应元素中的数组元素，向元素后面插入元素的逻辑应该采用正序，比如if后面多加else
    int preprocessILParser(ILParser* iLParser);
    int preprocessILFile(ILFile* iLFile);
    int preprocessILFunction(ILFunction* iLFunction);
    int preprocessILSchedule(ILSchedule* iLSchedule);
    int preprocessILBranch(ILBranch* iLBranch);
    int preprocessILCalculate(ILCalculate* iLCalculate);
    int preprocessILStatement(Statement* statement);
    // 倒序遍历相应元素中的数组元素，向元素前面插入元素的逻辑应该采用逆序，比如bool表达式前增加条件判断
    int preprocessILParserReverseOrder(ILParser* iLParser);
    int preprocessILFileReverseOrder(ILFile* iLFile);
    int preprocessILFunctionReverseOrder(ILFunction* iLFunction);
    int preprocessILScheduleReverseOrder(ILSchedule* iLSchedule);
    int preprocessILBranchReverseOrder(ILBranch* iLBranch);
    int preprocessILCalculateReverseOrder(ILCalculate* iLCalculate);
    int preprocessILStatementReverseOrder(Statement* statement);


    void addElseBranchBehindIf(ILBranch* iLBranch);
    void addElseStmtBehindIf(Statement* statement);

    //std::vector<Expression*> addedIfExpression;
    //void collectBooleanExpression(Expression* exp, std::vector<Expression*>& boolExpList);
    //void addBooleanExpressionIfElseBeforeStatement(Statement* statement, Expression* boolExp);
    
    void addSwitchMCDCStatement(Statement* statement);

    // 在 &&、|| 、<、!=、==等表达式前面加上MCDC的if-else判断
    void addMCDCBranchBeforeStatement(Statement* statement);
    // statement 是当前要插入的位置，realStatement是当statement是if时的else语句
    void addMCDCBranchBeforeStatementRecursiveExp(Expression* exp, Statement* statement, Statement* realStatement);
    // 以下是依赖的变量和函数
    static std::vector<std::pair<const Expression*, std::string>> MCDCCoverageLeafExp;
    static std::map<const Expression*, std::string> MCDCCoverageSubExpMap;
    static std::string MCDCCoverageFunc;
    static int MCDCSubExpId;
    static std::string MCDCCoverageFuncAll;
    static int mcdcCount;
    static std::string GenerateMCDCToRootStatements(const Expression* condition, const Statement* statement);
    static std::string HandleConditionExpression(const int bid, const Expression* condition, int& decCondId);
    static void generateBranchGateJudgement(const Statement* statement, bool& needGenCondition, bool& needGenDecision, bool& needGenMCDC);
    static std::string generateMCDCFuncExpSplit(bool needGenCondition, bool needGenDecision, bool needGenMCDC);
    static std::string generateMCDCFuncMaskingCond(int mcdcId, bool needGenCondition, bool needGenDecision, bool needGenMCDC);
    static std::string generateMCDCFuncRes(int mcdcId, const Statement* statement, bool needGenCondition, bool needGenDecision, bool needGenMCDC);

};
