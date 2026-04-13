#include "CodeGenerationTable.h"

#include <algorithm>

#include "MIRScheduleAnalyzerDataflow.h"
#include "MIRScheduleGraphDataflowNode.h"
#include "MIRScheduleGraphOutport.h"

using namespace std;
void CGTElement::release()
{
}

void CGTBranch::release()
{
    for(int i = 0; i < elements.size(); i++)
    {
        elements[i]->release();
        delete elements[i];
    }
    elements.clear();
}

void CGTActorExe::release()
{
}

void CGTInserter::release()
{

}

void CGTOperation::setCGT(CodeGenerationTable* CGT)
{
    this->CGT = CGT;
}

void CGTOperation::setBranchInfo(BranchInfo* branchInfo)
{
    this->branchInfo = branchInfo;
}

void CGTOperation::addDataflowNode(BranchInfo* branchInfo, MIRScheduleGraphDataflowNode* node)
{
    this->branchInfoList.push_back(branchInfo);
    this->nodeList.push_back(node);
}

int CGTOperation::execute()
{
    bool isBranchInfoConflict = MIRScheduleAnalyzerDataflow::buildCGTIsBranchInfoConflict(*branchInfo);
    bool isCommonBranchNeedRemovedInsert = MIRScheduleAnalyzerDataflow::buildCGTIsCommonBranchNeedRemovedInserter(*branchInfo, *CGT);

    if(isBranchInfoConflict) {
        if(isCommonBranchNeedRemovedInsert) {
            return this->executeCFRemoveInserterAndCreateBranchAndInsert();
        } else {
            return this->executeCFCreateBranchAndInsert();
        }
    } else {
        bool isThereSuitableInserter = MIRScheduleAnalyzerDataflow::buildCGTIsThereSuitableInserter(*branchInfo, *CGT);
        if(isThereSuitableInserter)
        {
            return this->executeNCInsert();
        } else {
            //if(isCommonBranchNeedRemovedInsert) {
            //    return this->executeNCRemoveInserterAndCreateBranchAndInsert();
            //} else {
                return this->executeNCCreateBranchAndInsert();
            //}
        }
    }
    return 0;
}

int CGTOperation::executeNCInsert()
{
    for(int i = 0; i < branchInfo->branchPathWithSrc.size(); i++)
    {
        const BranchPath &branchPath = branchInfo->branchPathWithSrc[i].first;
        const CGTInserter* inserter = MIRScheduleAnalyzerDataflow::buildCGTFindInserterByBranchPath(branchPath, *CGT);

        //CGT->removeInserterByBaseInserter(inserter);

        for(int j = 0; j < nodeList.size(); j++)
        {
            CGTActorExe* tempCGTActorExe = new CGTActorExe();
            tempCGTActorExe->node = nodeList[j];
            tempCGTActorExe->realBranchPathDataSrc = branchInfoList[j]->branchPathWithSrc[i].second;

            CGT->insertActorExe(inserter, tempCGTActorExe);
        }
    }
    return 1;
}

// int CGTOperation::executeNCRemoveInserterAndInsert()
// {
//     BranchPath commonBranchPath = getBranchInfoCommonBranchPath(*branchInfo);
// 
//     const CGTInserter* inserter = buildCGTGetBaseInserterOfCommonBranchPath(commonBranchPath, *CGT);
//     
//     CGT->removeInserterByBaseInserter(inserter);
// 
//     for(int i = 0; i < branchInfo->branchPathWithSrc.size(); i++)
//     {
//         const BranchPath &branchPath = branchInfo->branchPathWithSrc[i].first;
//         const CGTInserter* inserterTemp = buildCGTFindInserterByBranchPath(branchPath, *CGT);
//         
//         for(int j = 0; j < nodeList.size(); j++)
//         {
//             CGTActorExe* tempCGTActorExe = new CGTActorExe();
//             tempCGTActorExe->node = nodeList[j];
//             tempCGTActorExe->realBranchPathDataSrc = branchInfoList[j]->branchPathWithSrc[i].second;
// 
//             CGT->insertActorExe(inserterTemp, tempCGTActorExe);
//         }
//     }
// 
//     return 1;
// }

int CGTOperation::executeNCRemoveInserterAndCreateBranchAndInsert()
{
    BranchPath commonBranchPath = MIRScheduleAnalyzerDataflow::getBranchInfoCommonBranchPath(*branchInfo);

    const CGTInserter* inserter = MIRScheduleAnalyzerDataflow::buildCGTGetBaseInserterOfCommonBranchPath(commonBranchPath, *CGT);
    
    //CGT->removeInserterByBaseInserter(inserter);

    CGT->createNCBranch(inserter, commonBranchPath, *branchInfo);

    for(int i = 0; i < branchInfo->branchPathWithSrc.size(); i++)
    {
        const BranchPath &branchPath = branchInfo->branchPathWithSrc[i].first;
        const CGTInserter* inserter = MIRScheduleAnalyzerDataflow::buildCGTFindInserterByBranchPath(branchPath, *CGT);

        for(int j = 0; j < nodeList.size(); j++)
        {
            CGTActorExe* tempCGTActorExe = new CGTActorExe();
            tempCGTActorExe->node = nodeList[j];
            tempCGTActorExe->realBranchPathDataSrc = branchInfoList[j]->branchPathWithSrc[i].second;

            CGT->insertActorExe(inserter, tempCGTActorExe);
        }
    }

    return 1;
}

int CGTOperation::executeCFRemoveInserterAndCreateBranchAndInsert()
{
    BranchPath commonBranchPath = MIRScheduleAnalyzerDataflow::getBranchInfoCommonBranchPath(*branchInfo);

    const CGTInserter* inserter = MIRScheduleAnalyzerDataflow::buildCGTGetBaseInserterOfCommonBranchPath(commonBranchPath, *CGT);
    
    CGT->removeInserterByBaseInserter(inserter);

    // 找出除了基分支路径以外的分支信息
    BranchInfo branchInfoExceptBasePath;
    BranchInfo branchInfoJustBasePath;
    for(int i = 0; i < branchInfo->branchPathWithSrc.size(); i++)
    {
        if(MIRScheduleAnalyzerDataflow::areTwoBranchPathSame(branchInfo->branchPathWithSrc[i].first, commonBranchPath))
        {
            branchInfoJustBasePath.branchPathWithSrc.push_back(branchInfo->branchPathWithSrc[i]);
            continue;
        }
        branchInfoExceptBasePath.branchPathWithSrc.push_back(branchInfo->branchPathWithSrc[i]);
    }

    // 通过排列组合获得可能的分支路径执行情况
    int branchPathCount = branchInfoExceptBasePath.branchPathWithSrc.size();
    vector<vector<int>> combineIds = getCombineNum(branchPathCount);
    vector<BranchInfo> branchInfoCombineList;
    vector<int> branchInfoCombineIdList;

    for(int i = 0; i < combineIds.size(); i++)
    {
        BranchInfo branchInfoTemp;
        for(int j = 0; j < combineIds[i].size(); j++)
        {
            branchInfoTemp.branchPathWithSrc.push_back(branchInfoExceptBasePath.branchPathWithSrc[combineIds[i][j]]);
        }
        // 获取该种路径组合下的所有分支条件
        vector<Branch> conditionBranchList = MIRScheduleAnalyzerDataflow::buildCGTGetBranchInfoConditionBranchList(branchInfoTemp, commonBranchPath);
        // 如果这些分支条件中不存在同一组件的不同分支的情况，则视为这些条件可能同时达成
        if(!MIRScheduleAnalyzerDataflow::buildCGTIsConditionBranchListAlwaysFalse(conditionBranchList))
        {
            branchInfoCombineList.push_back(branchInfoTemp);
            branchInfoCombineIdList.push_back(i);
        }
    }


    for(int i = 0; i < branchInfoCombineList.size() + 1; i++)
    {
        // 额外处理一下在公共路径上的分支路径，也就是其它执行情况的else情况
        if(i ==  branchInfoCombineList.size() && branchInfoJustBasePath.branchPathWithSrc.empty())
            break;
        
        CGTBranch* tempCGTBranch = new CGTBranch();
        tempCGTBranch->parent = inserter->parent;

        if(i == 0)
            tempCGTBranch->branchType = CGTBranch::TypeCombineIf;
        else if(!branchInfoJustBasePath.branchPathWithSrc.empty())
            tempCGTBranch->branchType = CGTBranch::TypeCombineElse;
        else
            tempCGTBranch->branchType = CGTBranch::TypeCombineElseIf;


        if(i < branchInfoCombineList.size())
        {
            for(int j = 0; j < branchInfoCombineList[i].branchPathWithSrc.size(); j++)
            {
                tempCGTBranch->realBranchPaths.push_back(branchInfoCombineList[i].branchPathWithSrc[j].first);
            }
        }
        else
        {
            tempCGTBranch->realBranchPaths.push_back(commonBranchPath);
        }

        // 添加ActorExe到新的组合分支中
        for(int j = 0; j < nodeList.size(); j++)
        {
            CGTActorExe* tempCGTActorExe = new CGTActorExe();
            tempCGTActorExe->node = nodeList[j];

            if(i < branchInfoCombineList.size())
            {
                
                set<MIRScheduleGraphOutport*> srcs;
                vector<int> &combineIdList = combineIds[branchInfoCombineIdList[i]];
                for(int k = 0; k < combineIdList.size(); k++)
                {
                    // 必须保证节点列表中所有节点的分支信息的顺序是一致的，这里+offset是去除了公共路径的那个BranchPath
                    // 可以直接偏移offset，是因为BranchInfo中的BranchPath都已经按照长短排序好了，如果存在公共路径
                    // 那么这个公共路径就一定在最前面
                    int offset = branchInfoJustBasePath.branchPathWithSrc.empty() ? 0 : 1;
                    BranchPathDataSrc &srcsTemp = branchInfoList[j]->branchPathWithSrc[combineIdList[k] + offset].second;
                    for(int m = 0; m < srcsTemp.srcs.size(); m++)
                    {
                        srcs.insert(srcsTemp.srcs[m]);
                    }
                }
                auto iter = srcs.begin();
                for(; iter != srcs.end(); iter++)
                {
                    tempCGTActorExe->realBranchPathDataSrc.srcs.push_back(*iter);
                }
            }
            else
            {
                tempCGTActorExe->realBranchPathDataSrc = branchInfoJustBasePath.branchPathWithSrc[0].second;
            }

            tempCGTBranch->elements.push_back(tempCGTActorExe);
        }

        CGTInserter* tempCGTInserter = new CGTInserter();
        tempCGTInserter->parent = tempCGTBranch;
        tempCGTBranch->elements.push_back(tempCGTInserter);

        CGT->insertBranch(inserter, tempCGTBranch);
    }
    
    return 1;
}

int CGTOperation::executeNCCreateBranchAndInsert()
{
    
    return  executeNCRemoveInserterAndCreateBranchAndInsert();

    //BranchPath commonBranchPath = getBranchInfoCommonBranchPath(*branchInfo);
    //
    //const CGTInserter* inserter = buildCGTGetBaseInserterOfCommonBranchPath(commonBranchPath, *CGT);
    //
    //CGT->createNCBranch(inserter, commonBranchPath, *branchInfo);
    //
    //for(int i = 0; i < branchInfo->branchPathWithSrc.size(); i++)
    //{
    //    const BranchPath &branchPath = branchInfo->branchPathWithSrc[i].first;
    //    const CGTInserter* inserter = buildCGTFindInserterByBranchPath(branchPath, *CGT);
    //
    //    
    //    for(int j = 0; j < nodeList.size(); j++)
    //    {
    //        CGTActorExe* tempCGTActorExe = new CGTActorExe();
    //        tempCGTActorExe->node = nodeList[j];
    //        tempCGTActorExe->realBranchPathDataSrc = branchInfoList[j]->branchPathWithSrc[i].second;
    //
    //        CGT->insertActorExe(inserter, tempCGTActorExe);
    //    }
    //}
    //
    //return 1;
}

int CGTOperation::executeCFCreateBranchAndInsert()
{
    
    return executeCFRemoveInserterAndCreateBranchAndInsert();
}


// int CGTOpNCTypeInsert::execute()
// {
//     for(int i = 0; i < branchInfo->branchPathWithSrc.size(); i++)
//     {
//         const BranchPath &branchPath = branchInfo->branchPathWithSrc[i].first;
//         const CGTInserter* inserter = buildCGTFindInserterByBranchPath(branchPath, *CGT);
// 
//         CGTActorExe* tempCGTActorExe = new CGTActorExe();
//         tempCGTActorExe->node = node;
//         tempCGTActorExe->realBranchPathDataSrc = branchInfo->branchPathWithSrc[i].second;
// 
//         CGT->insertActorExe(inserter, tempCGTActorExe);
//         
//     }
// 
//     return 1;
// }
// 
// // void CGTOpRemoveInserterAndInsert::setRemovedBaseInserter(CGTInserter* removedBaseInserter)
// // {
// //     this->removedBaseInserter = removedBaseInserter;
// // }
// 
// int CGTOpNCRemoveInserterAndInsert::execute()
// {
//     BranchPath commonBranchPath = getBranchInfoCommonBranchPath(*branchInfo);
// 
//     const CGTInserter* inserter = buildCGTGetBaseInserterOfCommonBranchPath(commonBranchPath, *CGT);
//     
//     CGT->removeInserterByBaseInserter(inserter);
// 
//     for(int i = 0; i < branchInfo->branchPathWithSrc.size(); i++)
//     {
//         const BranchPath &branchPath = branchInfo->branchPathWithSrc[i].first;
//         const CGTInserter* inserter = buildCGTFindInserterByBranchPath(branchPath, *CGT);
// 
//         CGTActorExe* tempCGTActorExe = new CGTActorExe();
//         tempCGTActorExe->node = node;
//         tempCGTActorExe->realBranchPathDataSrc = branchInfo->branchPathWithSrc[i].second;
// 
//         CGT->insertActorExe(inserter, tempCGTActorExe);
//     }
// 
//     return 1;
// }
// 
// int CGTOpNCRemoveInserterAndCreateBranchAndInsert::execute()
// {
//     BranchPath commonBranchPath = getBranchInfoCommonBranchPath(*branchInfo);
// 
//     const CGTInserter* inserter = buildCGTGetBaseInserterOfCommonBranchPath(commonBranchPath, *CGT);
//     
//     CGT->removeInserterByBaseInserter(inserter);
// 
// 
// 
// 
//     return 1;
// }
// 
// int CGTOpNCCreateBranchAndInsert::execute()
// {
//     
//     return 1;
// }
// 
// int CGTOpCFRemoveInserterAndCreateBranchAndInsert::execute()
// {
// 
//     return 1;
// }
// 
// int CGTOpCFCreateBranchAndInsert::execute()
// {
// 
//     return 1;
// }

void CGTOperationList::addOperation(CGTOperation* operation)
{
    operation->id = operations.size();
    operations.push_back(operation);
}

int CGTOperationList::execute()
{
    int res;

    std::sort(operations.begin(), operations.end(), sortCGTOperationByCGTOperationType);

    for(int i = 0; i < operations.size(); i++)
    {
        res = operations[i]->execute();
        if(res < 0)
            return res;
    }
    return 1;
}

void CGTOperationList::release()
{
    for(int i = 0; i < operations.size(); i++)
    {
        delete operations[i];
    }
    operations.clear();
}

CodeGenerationTable::Iterator::Iterator(const CodeGenerationTable& CGT)
{
    // 不采用递归的方式遍历CGT，直接模拟栈存储进行CGT树的遍历
    // 栈中存储树的每一层中接下来要遍历的一个元素，用于继续遍历
    nextIterateElements.push(pair<const vector<CGTElement*>*, int>(&CGT.elements, 0));
    
}

const CGTElement* CodeGenerationTable::Iterator::next()
{
    if(nextIterateElements.empty())
    {
        return nullptr;
    }
    const vector<CGTElement*>* list = nextIterateElements.top().first;
    int id = nextIterateElements.top().second;
    nextIterateElements.pop();

    const CGTElement* element = (*list)[id];
    if(element->type == CGTElement::TypeBranch)
    {
        if(id + 1 < list->size())
        {
            nextIterateElements.push(pair<const vector<CGTElement*>*, int>(list, id + 1));
        }
        const CGTBranch* branch = static_cast<const CGTBranch*>(element);
        if(branch->elements.size() > 0)
        {
            nextIterateElements.push(pair<const vector<CGTElement*>*, int>(&branch->elements, 0));
        }
        return element;
    }
    if(id + 1 < list->size())
    {
        nextIterateElements.push(pair<const vector<CGTElement*>*, int>(list, id + 1));
    }
    return element;
}

void CodeGenerationTable::Iterator::init(const CodeGenerationTable& CGT)
{
    while(!nextIterateElements.empty())
    {
        nextIterateElements.pop();
    }
    nextIterateElements.push(pair<const vector<CGTElement*>*, int>(&CGT.elements, 0));
    
}


void CodeGenerationTable::removeElement(CGTElement* element)
{
    std::vector<CGTElement*>* elementsList;
    if(element->parent)
    {
        elementsList = &element->parent->elements;
    }
    else
    {
        elementsList = &elements;
    }

    for(int i = 0; i < elementsList->size(); i++)
    {
        if((*elementsList)[i] != element)
        {
            continue;
        }
        elementsList->erase(elementsList->begin() + i);
        break;
    }
}

void CodeGenerationTable::removeInserterByBaseInserter(const CGTInserter* inserter)
{
    vector<const CGTInserter*> inserterNeedRemoved;

    bool startSearch = inserter->parent ? false : true;
    CodeGenerationTable::Iterator iter(*this);
    const CGTElement* element = iter.next();
    while(element)
    {
        if(element == inserter->parent)
        {
            startSearch = true;
            element = (const CGTElement*)iter.next();
            continue;
        }

        if(startSearch && element->type == CGTElement::TypeInserter)
        {
            if(element != inserter)
            {
                inserterNeedRemoved.push_back(static_cast<const CGTInserter*>(element));
            }
            else
            {
                break;
            }
        }

        element = (const CGTElement*)iter.next();
    }

    for(int i = 0; i < inserterNeedRemoved.size(); i++)
    {
        removeElement(const_cast<CGTInserter*>(inserterNeedRemoved[i]));
    }
}

void CodeGenerationTable::insertActorExe(const CGTInserter* inserter,
    CGTActorExe* actorExe)
{
    std::vector<CGTElement*>* elementsList;
    if(inserter->parent)
    {
        elementsList = &inserter->parent->elements;
    }
    else
    {
        elementsList = &elements;
    }

    for(int i = 0; i < elementsList->size(); i++)
    {
        if((*elementsList)[i] != inserter)
        {
            continue;
        }
        elementsList->insert(elementsList->begin() + i, actorExe);
        break;
    }
    
}

void CodeGenerationTable::insertBranch(const CGTInserter* inserter, CGTBranch* branch)
{
    std::vector<CGTElement*>* elementsList;
    if(inserter->parent)
    {
        elementsList = &inserter->parent->elements;
    }
    else
    {
        elementsList = &elements;
    }
    
    for(int i = 0; i < elementsList->size(); i++)
    {
        if((*elementsList)[i] != inserter)
        {
            continue;
        }
        elementsList->insert(elementsList->begin() + i, branch);
        break;
    }
}

void CodeGenerationTable::createNCBranch(const CGTInserter* inserter,
    const BranchPath& baseBranchPath, const BranchInfo& branchInfo)
{
    for(int i = 0; i < branchInfo.branchPathWithSrc.size(); i++)
    {
        const BranchPath &branchPath = branchInfo.branchPathWithSrc[i].first;

        createNCBranchOnePath(inserter, baseBranchPath, branchPath);
    }
    
}


void CodeGenerationTable::release()
{
    for(int i = 0; i < elements.size(); i++)
    {
        elements[i]->release();
        delete elements[i];
    }
    elements.clear();
}

void CodeGenerationTable::createNCBranchOnePath(const CGTInserter* inserter,
                                                                             const BranchPath& baseBranchPath, const BranchPath& branchPath)
{

    BranchPath curBranchPath = baseBranchPath;
    const CGTInserter* curInserter = inserter;
    
    BranchPath nextBranchPath = baseBranchPath;
    const CGTInserter* nextInserter = inserter;

    //构建基分支路径到目标分支路径之间的分支
    for(int i = baseBranchPath.branchs.size(); i < branchPath.branchs.size(); i++)
    {
        const Branch &branch = branchPath.branchs[i];

        BranchPath curBranchPathSearch = curBranchPath;
        curBranchPathSearch.branchs.push_back(branch);

        const CGTInserter* foundInserter = MIRScheduleAnalyzerDataflow::buildCGTFindInserterByBranchPath(curBranchPathSearch, *this);
        if(foundInserter)
        {
            curBranchPath = curBranchPathSearch;
            curInserter = foundInserter;
            continue;
        }

        int branchCount = MIRScheduleAnalyzerDataflow::getDataflowNodeBranchCount(branch.branchNode);
        for(int j = 0; j < branchCount; j++)
        {

            CGTBranch* tempCGTBranch = new CGTBranch();
            tempCGTBranch->parent = curInserter->parent;

            if(j == 0)
                tempCGTBranch->branchType = CGTBranch::TypeIf;
            else if(j == branchCount - 1)
                tempCGTBranch->branchType = CGTBranch::TypeElse;
            else
                tempCGTBranch->branchType = CGTBranch::TypeElseIf;

            Branch tempBranch = branch;
            tempBranch.branchId = j;
            BranchPath tempBranchPath = curBranchPath;
            tempBranchPath.branchs.push_back(tempBranch);
            tempCGTBranch->realBranchPaths.push_back(tempBranchPath);

            CGTInserter* tempCGTInserter = new CGTInserter();
            tempCGTInserter->parent = tempCGTBranch;
            tempCGTBranch->elements.push_back(tempCGTInserter);

            this->insertBranch(curInserter, tempCGTBranch);

            if(j == branch.branchId)
            {
                nextBranchPath = tempBranchPath;
                nextInserter = tempCGTInserter;
            }
        }

        curBranchPath = nextBranchPath;
        curInserter = nextInserter;
    }
}


void CodeGenerationTable::print()
{
    for(int i = 0; i < elements.size(); i++)
    {
        print(elements[i], 0);
    }
    cout << "\n";
}


void CodeGenerationTable::print(CGTElement* element, int depth)
{
    cout << stringGetBlanks(depth * 4);
    if(element->type == CGTElement::TypeActorExe)
    {
        //cout << "\033[1;33m";
        CGTActorExe* actorExe = static_cast<CGTActorExe*>(element);
        cout << actorExe->node->actor->name;
        cout << "(";
        for(int i = 0; i < actorExe->realBranchPathDataSrc.srcs.size(); i++)
        {
            if(i != 0)
            {
                cout << ", ";
            }
            
            cout << "\033[1;36m";
            MIRScheduleGraphOutport* src = actorExe->realBranchPathDataSrc.srcs[i];
            if(src->containerNode)
            {
                cout << src->containerNode->actor->name << ".";
            }
            if(src->outportFromActor)
            {
                cout << src->outportFromActor->name;
            }
            else if(src->inportFromFunction)
            {
                cout << src->inportFromFunction->name;
            }
            
            cout << "\033[0m";
        }
        cout << ")\n";
        
        //cout << "\033[0m";
    }
    else if(element->type == CGTElement::TypeInserter)
    {
        
        cout << "\033[1;31m";
        cout << "Inserter\n";
        cout << "\033[0m";
    }
    else if(element->type == CGTElement::TypeBranch)
    {
        cout << "\033[1;32m";
        CGTBranch* branch = static_cast<CGTBranch*>(element);
        
        if(branch->branchType == CGTBranch::TypeIf)
            cout << "If\n";
        else if(branch->branchType == CGTBranch::TypeElseIf)
            cout << "ElseIf\n";
        else if(branch->branchType == CGTBranch::TypeElseIf)
            cout << "Else\n";
        else if(branch->branchType == CGTBranch::TypeCombineIf)
            cout << "CombineIf\n";
        else if(branch->branchType == CGTBranch::TypeCombineElseIf)
            cout << "CombineElseIf\n";
        else if(branch->branchType == CGTBranch::TypeCombineElse)
            cout << "CombineElse\n";

        
        cout << "\033[1;33m";
        for(int i = 0; i < branch->realBranchPaths.size(); i++)
        {
            cout << "  * ";
            const BranchPath &branchPath = branch->realBranchPaths[i];
            for(int j = 0; j < branchPath.branchs.size(); j++)
            {
                const Branch &branch = branchPath.branchs[j];
                if(j != 0)
                {
                    cout << " -> ";
                }
                if(branch.branchNode)
                {
                    cout << branch.branchNode->actor->name << ".";
                }
                cout << branch.branchId;
            }
            cout << "\n";
        }

        
        cout << "\033[0m";
        for(int i = 0; i < branch->elements.size(); i++)
        {
            print(branch->elements[i], depth + 1);
        }
    }
}



bool sortCGTOperationByCGTOperationType(const CGTOperation* a,
                                        const CGTOperation* b)
{
    if(a->type == b->type)
        return a->id < b->id;
    return a->type < b->type;
}
