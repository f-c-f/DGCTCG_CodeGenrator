#pragma once

#include <stack>
#include <unordered_map>
#include <vector>

#include "BranchInfo.h"

class CGTBranch;
class CGTElement
{
public:
    enum ElementType
    {
        TypeBranch,
        TypeActorExe,
        TypeInserter,
        TypeUnknown,
    };

    ElementType type = TypeUnknown;
    
    CGTBranch* parent = nullptr;

    virtual void release();
};

class CGTBranch: public CGTElement
{
public:
    CGTBranch() {type = CGTElement::TypeBranch;}
    ~CGTBranch() {}

    std::vector<BranchPath> realBranchPaths;

    enum CGTBranchType
    {
        TypeIf,
        TypeElseIf,
        TypeElse,
        TypeCombineIf,
        TypeCombineElseIf,
        TypeCombineElse,
        TypeUnknown,
    };

    CGTBranchType branchType = CGTBranchType::TypeUnknown;

    std::vector<CGTElement*> elements;

    
    // 对于类型为TypeCombineElse时(冲突组件组合而来)，存储前面所有分支
    std::vector<CGTBranch*> otherCombinedBranches;

    void release() override;
};

class CGTActorExe: public CGTElement
{
public:
    CGTActorExe() {type = CGTElement::TypeActorExe;}
    ~CGTActorExe() {}
    

    // BranchInfo realBranchInfo; // 在分支路径上真实的分支信息，不是所有的分支信息
    BranchPathDataSrc realBranchPathDataSrc; // 在分支路径上真实的数据源，不是所有的数据源

    MIRScheduleGraphDataflowNode* node = nullptr;
    
    void release() override;
};



class CGTInserter: public CGTElement
{
public:
    CGTInserter() {type = CGTElement::TypeInserter;}
    ~CGTInserter() {}
    
    void release() override;
};

class CodeGenerationTable;




// 同时整理一层的操作，按照影响度排序后，再实施对CGT的更新
class CGTOperation
{
public:
    
    int id = 0;

    //NC指not conflict
    //CF指conflict
    enum OperationType
    {
        TypeNCInsert,
        TypeNCRemoveInserterAndInsert,
        TypeNCRemoveInserterAndCreateBranchAndInsert,
        TypeCFRemoveInserterAndCreateBranchAndInsert,
        TypeNCCreateBranchAndInsert,
        TypeCFCreateBranchAndInsert,
        TypeUnknown,
    };

    OperationType type = TypeUnknown;

    // 仅用于判断和构建分支
    BranchInfo* branchInfo = nullptr;

    // 仅用向Inserter位置批量插入同分支下的组件
    std::vector<BranchInfo*> branchInfoList;
    std::vector<MIRScheduleGraphDataflowNode*> nodeList;

    CodeGenerationTable* CGT;

    void setCGT(CodeGenerationTable* CGT);

    void setBranchInfo(BranchInfo* branchInfo);
    void addDataflowNode(BranchInfo* branchInfo, MIRScheduleGraphDataflowNode* node);

    int execute();

    int executeNCInsert();
    //int executeNCRemoveInserterAndInsert();
    int executeNCRemoveInserterAndCreateBranchAndInsert();
    int executeCFRemoveInserterAndCreateBranchAndInsert();
    int executeNCCreateBranchAndInsert();
    int executeCFCreateBranchAndInsert();

};

// // 仅根据分支信息向CGT中插入ActorExe
// class CGTOpNCTypeInsert: public CGTOperation
// {
// public:
//     CGTOpNCTypeInsert() {type = TypeNCInsert;}
//     ~CGTOpNCTypeInsert() {}
// 
//     // 执行条件如下
//     // 2. 分支不存在冲突
//     //   2.2. 公共分支前不存在需要被删除的Inserter
//     //     2.2.2. 可以完全找到对应自身分支信息的Inserter
//     int execute() override;
// };
// 
// // 分支合并时，移除Inserter并插入ActorExe
// class CGTOpNCRemoveInserterAndInsert: public CGTOperation
// {
// public:
//     CGTOpNCRemoveInserterAndInsert() {type = TypeNCRemoveInserterAndInsert;}
//     ~CGTOpNCRemoveInserterAndInsert() {}
// 
//     // 执行条件如下
//     // 2. 分支不存在冲突
//     //   2.1. 公共分支前存在需要被删除的Inserter
//     //     2.1.2. 可以完全找到对应自身分支信息的Inserter
//     int execute() override;
// };
// 
// // 组件遇到新分支时，存在需要清理的前方Inserter，创建分支，之后插入ActorExe
// class CGTOpNCRemoveInserterAndCreateBranchAndInsert: public CGTOperation
// {
// public:
//     CGTOpNCRemoveInserterAndCreateBranchAndInsert() {type = TypeNCRemoveInserterAndCreateBranchAndInsert;}
//     ~CGTOpNCRemoveInserterAndCreateBranchAndInsert() {}
//     
//     // 执行条件如下
//     // 2. 分支不存在冲突
//     //   2.1. 公共分支前存在需要被删除的Inserter
//     //     2.1.1. 找不到(不完全找到)对应自身分支信息的Inserter
//     int execute() override;
// };
// 
// 
// // 组件遇到新分支时，创建分支，之后插入ActorExe
// class CGTOpNCCreateBranchAndInsert: public CGTOperation
// {
// public:
//     CGTOpNCCreateBranchAndInsert() {type = TypeNCCreateBranchAndInsert;}
//     ~CGTOpNCCreateBranchAndInsert() {}
//     
//     // 执行条件如下
//     // 2. 分支不存在冲突
//     //   2.2. 公共分支前不存在需要被删除的Inserter
//     //     2.2.1. 找不到(不完全找到)对应自身分支信息的Inserter
//     int execute() override;
// };
// 
// // 分支冲突时，移除Inserter并创建冲突分支，之后插入ActorExe
// class CGTOpCFRemoveInserterAndCreateBranchAndInsert: public CGTOperation
// {
// public:
//     CGTOpCFRemoveInserterAndCreateBranchAndInsert() {type = TypeCFRemoveInserterAndCreateBranchAndInsert;}
//     ~CGTOpCFRemoveInserterAndCreateBranchAndInsert() {}
//     
//     // 执行条件如下
//     // 1. 分支存在冲突
//     //   1.1. 公共分支前存在需要被删除的Inserter
//     int execute() override;
// };
// 
// 
// // 组件遇到新分支时，且分支冲突，创建分支，之后插入ActorExe
// class CGTOpCFCreateBranchAndInsert: public CGTOperation
// {
// public:
//     CGTOpCFCreateBranchAndInsert() {type = TypeCFCreateBranchAndInsert;}
//     ~CGTOpCFCreateBranchAndInsert() {}
//     
//     // 执行条件如下
//     // 1. 分支存在冲突
//     //   1.2. 公共分支前不存在需要被删除的Inserter
//     int execute() override;
// };


class CGTOperationList
{
public:
    std::vector<CGTOperation*> operations;


    void addOperation(CGTOperation* operation);

    int execute();

    void release();
};

class CodeGenerationTable
{
public:
    

    std::vector<CGTElement*> elements;

    CGTOperationList operationList;

    std::unordered_map<const MIRScheduleGraphDataflowNode*, bool> handledNodes;

    class Iterator
    {
    public:
        Iterator(const CodeGenerationTable& CGT);
        ~Iterator() {};

        const CGTElement* next();
        void init(const CodeGenerationTable& CGT);

    private:

        std::stack<std::pair<const std::vector<CGTElement*>*, int>> nextIterateElements;
    };

    void removeElement(CGTElement* element);
    void removeInserterByBaseInserter(const CGTInserter* inserter);

    void insertActorExe(const CGTInserter* inserter, CGTActorExe* actorExe);
    void insertBranch(const CGTInserter* inserter, CGTBranch* branch);

    void createNCBranch(const CGTInserter* inserter, const BranchPath &baseBranchPath, const BranchInfo &branchInfo);
    
    void release();

    void print();
private:
    void createNCBranchOnePath(const CGTInserter* inserter, const BranchPath &baseBranchPath, const BranchPath &branchPath);
    void print(CGTElement* element, int depth);
};

bool sortCGTOperationByCGTOperationType(const CGTOperation* a, const CGTOperation* b);
