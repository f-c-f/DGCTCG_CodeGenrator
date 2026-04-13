#include <string>

#include <sstream>
#include <vector>
#include <map>

class Expression;
class ILBranch;
class ILTransFile;
class StmtSwitch;

/*enum IfHandleType {
	ILIfBranch,
	StatementIfBranch, 
};*/

class ILTransInstrumentMCDCCoverage
{
public:

    // void GetRootVarName(const void* block, std::string& rootVarName);
    // void InsertMCDCToILFor(const ILBranch* block, ILTransFile* file);
    // void InsertMCDCToStatementFor(const void* block, const Expression* cond, ILTransFile* file, std::string forIni);
    // void InsertMCDCToForPost(const void* block, ILTransFile* file, std::string forUpd);
    // void InsertMCDCToWhile(const void* block, const Expression* cond, ILTransFile* file, bool isDoWhile);
    static std::string GenerateMCDCToRootExpression(const Expression* condition, std::map<std::string, std::string>& exprStrMap);
    //static std::string GenerateMCDCToOneCondExpression(const void* block, const std::string oriCondStr, const std::string condStr);
    // void InsertMCDCToIf(const IfHandleType htp, const void* rblock, const Expression* cond, ILTransFile* file);
    //static void InsertMCDCToSwitch(const StmtSwitch* stmt, std::string& wholeCode);

    
    static std::string generateMCDCFuncRegister();

private:
    static std::string HandleLeafConditionExpression(const int bid, const void* condAddr, std::string oriCondStr, std::string condStr, int& decCondId);
    
    static int HandleDecisionCoverage(const void* condAddr, std::string condStr);
    static int HandleConditionCoverage(const void* condAddr, std::string condStr);
    static std::string HandleConditionExpression(const int bid, const Expression* condition, std::map<std::string, std::string>& exprStrMap, int& decCondId);
    
    static std::vector<std::pair<const Expression*, std::string>> MCDCCoverageLeafExp;
    static std::map<const Expression*, std::string> MCDCCoverageSubExpMap;
    static std::string MCDCCoverageFunc;
    static int MCDCSubExpId;
    static std::string MCDCCoverageFuncAll;
    
    static std::string generateMCDCFuncHead(int bid);
    static std::string generateMCDCFuncExpSplit();
    static std::string generateMCDCFuncMaskingCond();
    static std::string generateMCDCFuncRes();

};



