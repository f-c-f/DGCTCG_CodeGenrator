#pragma once
class Expression;
class ILParser;
class ILBranch;
class ILBranchOptimizer
{
public:
	int optimize(ILBranch* branch, ILParser* parser);
	ILBranch* branch = nullptr;
    
private:
	
	ILParser* iLParser = nullptr;
    
	int release() const;
};

