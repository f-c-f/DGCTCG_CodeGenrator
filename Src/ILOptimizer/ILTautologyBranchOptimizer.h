#pragma once
#include <vector>

class ILSchedule;
class ILFunction;
class ILFile;
class ILParser;
class ILBranch;
class Statement;
class ILCalculate;


class ILTautologyBranchOptimizer
{
public:
	int optimize(ILParser* parser);


private:

    struct BranchInfo
    {
        ILBranch* iLBranch = nullptr;
        Statement* statement = nullptr;
        BranchInfo(ILBranch* iLBranch){this->iLBranch = iLBranch;};
        BranchInfo(Statement* statement){this->statement = statement;};
    };

	ILParser* iLParser = nullptr;

	int runCBMC() const;

	std::vector<BranchInfo> branchList;
	int collectBranch();
	int traverseILFile(ILFile* file);
	int traverseILFunction(ILFunction* function);
	int traverseILSchedule(ILSchedule* schedule);
	int traverseILCalculate(ILCalculate* calculate);
	int traverseILBranch(ILBranch* branch);
	int traverseStatement(Statement* statement);

	int optimizeBranch();
};


