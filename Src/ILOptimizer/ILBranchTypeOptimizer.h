#pragma once
#include <set>

class ILSchedule;
class ILFunction;
class ILFile;
class ILParser;
class ILBranch;
class ILBranchTypeOptimizer
{
public:
	int optimize(ILParser* parser);

private:
	
	ILParser* iLParser = nullptr;

	int traverseILFile(ILFile* file);
	int traverseILFunction(ILFunction* function);
	int traverseILSchedule(ILSchedule* schedule);
	int traverseILBranch(ILBranch* branch);
	
	//由于遍历Branch是自底向上进行的，对于需要移除的前面的分支结构，需要用这个list存储
	std::set<ILBranch*> removeBranchList;
	int optimizeILBranchElseToIf(ILBranch* &branch);

    //移除空的分支语句块
	int optimizeILBranchEmpty(ILBranch* &branch) const;

    //将else语句块在内部仅含有if-else或if-else if-else时，转为else if
	int optimizeILBranchElseIf(ILBranch* &branch) const;

    int optimizeILBranchChangeElseToIf(ILBranch* branchIf, ILBranch* branchElse) const;
};



