#include "ILTranslatorUtility.h"

#include "../ILBasic/ILParser.h"
#include "../ILBasic/ILBranch.h"

using namespace std;

ILTranslatorUtility::ILFunctionCallGraph ILTranslatorUtility::iLFunctionCallGraph;

ILTranslatorUtility::ILFunctionCallNode* ILTranslatorUtility::ILFunctionCallGraph::findILFunctionCallNodeByName(
    std::string functionName)
{
    for(int i = 0; i < iLFunctionCallGraph.allNodes.size(); i++)
    {
        ILFunctionCallNode* tempNode = iLFunctionCallGraph.allNodes[i];
        if(tempNode->iLFunction->name == functionName)
            return  tempNode;
    }
    return nullptr;
}

void ILTranslatorUtility::ILFunctionCallGraph::release()
{
    for(int i = 0; i < iLFunctionCallGraph.allNodes.size(); i++)
    {
        delete iLFunctionCallGraph.allNodes[i];
    }
}

void ILTranslatorUtility::buildILFunctionCallGraph(ILParser* iLParser)
{
    iLFunctionCallGraph.root = new ILFunctionCallNode();

    // 对有所ILFunction创建调用图节点
    for(int i = 0; i < iLParser->allFunctions.size(); i++)
    {
        ILFunctionCallNode* tempNode = new ILFunctionCallNode();
        iLFunctionCallGraph.allNodes.push_back(tempNode);
        tempNode->iLFunction = iLParser->allFunctions[i];
    }

    // 连接调用图节点
    for(int i = 0; i < iLFunctionCallGraph.allNodes.size(); i++)
    {
        ILFunctionCallNode* tempNode = iLFunctionCallGraph.allNodes[i];
        for(int j = 0; j < tempNode->iLFunction->functionCallInfoList.size(); j++)
        {
            ILFunction::FunctionCallInfo* callInfo = &(tempNode->iLFunction->functionCallInfoList[j]);
            ILFunctionCallNode* targetFunc = iLFunctionCallGraph.findILFunctionCallNodeByName(callInfo->functionName);
            if(!targetFunc)
                continue;
            tempNode->callNodes.push_back(targetFunc);
        }
        if(!iLParser->config.mainInitFunction.empty() && tempNode->iLFunction->name == iLParser->config.mainInitFunction ||
            !iLParser->config.mainExecFunction.empty() && tempNode->iLFunction->name == iLParser->config.mainExecFunction) {
            iLFunctionCallGraph.root->callNodes.push_back(tempNode);
        }
    }
    iLFunctionCallGraph.allNodes.push_back(iLFunctionCallGraph.root);

    // 计算调用节点的最浅深度
    int curDepth = 0;
    vector<ILFunctionCallNode*> curLayer;
    vector<ILFunctionCallNode*> nextLayer;
    curLayer.push_back(iLFunctionCallGraph.root);
    while(!curLayer.empty())
    {
        for(int i = 0; i < curLayer.size(); i++)
        {
            curLayer[i]->callDepth = curDepth;
            if(!curLayer[i]->parentFirst || curLayer[i]->parentFirst == iLFunctionCallGraph.root)
            {
                curLayer[i]->branchDepth = 0;
            }
            else
            {
                int statementDepth = 0;
                ILFunction::FunctionCallInfo* callInfo = curLayer[i]->parentFirst->iLFunction->getFunctionCallInfoMinDepth(curLayer[i]->iLFunction->name);
                if(callInfo)
                {
                    statementDepth = callInfo->statementDepth;
                }
                curLayer[i]->branchDepth = curLayer[i]->parentFirst->branchDepth + statementDepth;
            }

            for(int j = 0; j < curLayer[i]->callNodes.size(); j++)
            {
                if(curLayer[i]->callNodes[j]->callDepth == -1)
                {
                    curLayer[i]->callNodes[j]->parentFirst = curLayer[i];
                    nextLayer.push_back(curLayer[i]->callNodes[j]);
                }
            }
        }
        curLayer = nextLayer;
        nextLayer.clear();
        curDepth++;
    }
    
}

void ILTranslatorUtility::releaseILFunctionCallGraph()
{
    iLFunctionCallGraph.release();
}

int ILTranslatorUtility::getStatementCallDepth(const Statement* stmt)
{
    ILFunction* iLFunction = stmt->getContainerILFunction();
    ILFunctionCallNode* callNode = iLFunctionCallGraph.findILFunctionCallNodeByName(iLFunction->name);
    vector<void*> domain = stmt->getDomain();
    return callNode->branchDepth + domain.size() - 1;
}

int ILTranslatorUtility::getStatementCallDepth(const ILBranch* branch)
{
    ILFunction* iLFunction = branch->getContainerILFunction();
    ILFunctionCallNode* callNode = iLFunctionCallGraph.findILFunctionCallNodeByName(iLFunction->name);
    vector<void*> domain = branch->getDomain();
    return callNode->branchDepth + domain.size() - 1;
}
