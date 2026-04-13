#pragma once
#include <string>
#include <vector>

class ILBranch;
class Statement;
class ILFunction;
class ILParser;
class ILTranslatorUtility
{
public:
    class ILFunctionCallNode
    {
    public:
        ILFunction* iLFunction = nullptr;
        std::vector<ILFunctionCallNode*> callNodes;
        int callDepth = -1;
        int branchDepth = -1;
        ILFunctionCallNode* parentFirst = nullptr;
    };

    class ILFunctionCallGraph
    {
    public:
        ILFunctionCallNode* root = nullptr;
        std::vector<ILFunctionCallNode*> allNodes;
        ILFunctionCallNode* findILFunctionCallNodeByName(std::string functionName);

        void release();
    };

    static ILFunctionCallGraph iLFunctionCallGraph;
    static void buildILFunctionCallGraph(ILParser* iLParser);
    static void releaseILFunctionCallGraph();

    static int getStatementCallDepth(const Statement* stmt);
    static int getStatementCallDepth(const ILBranch* branch);
};
