#include <string>
#include "../ILBasic/ILBranch.h"
#include "../ILBasic/Statement.h"
#include "../ILTranslator/ILTransFile.h"

class ILTransInstrumentBranchCoverage
{
public:
    static void InsertInBlockBranchCoverageCollection(const ILBranch* branch, ILTransFile* file);
    static void InsertPostBlockBranchCoverageCollection(const ILBranch* branch, ILTransFile* file);

    static void InsertStatementInBlockBranchCoverageCollection(const Statement* stmt, const std::string branch_desc, std::string& stmtStr);
    static void InsertStatementPostBlockBranchCoverageCollection(const Statement* stmt, std::string& stmtStr);
    static void InsertSwitchDefaultBranchCoverageCollection(const Statement* stmt, const std::string branchDesc, std::string& stmtStr);

    static std::string getBranchCoverNameForCBMC(const ILObject* branch);
    static std::string getBranchCoverNameForCBMC(const Statement* stmt);
    static std::string getBranchCoverNameForCBMC(const ILFunction* function);

    static void getBranchParentAndBrother(int branchCoverId, int& parent, int& brother, bool isCaseOrDefaultStmt = false);

};

typedef ILTransInstrumentBranchCoverage Instrument_BC;
