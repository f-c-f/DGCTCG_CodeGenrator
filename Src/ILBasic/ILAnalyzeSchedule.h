#pragma once
#include <map>
#include <string>
#include <vector>

#include "ILAnalyzeScheduleVariable.h"

class ILRef;
class Statement;
class ILBranch;
class ILCalculate;
class Expression;
class ILAnalyzeScheduleNode;
class ILParser;
class ILSchedule;
class ILAnalyzeScheduleNodeVariableRef;


class ILAnalyzeSchedule
{
public:

    enum {
        ErrorInternal = 200001,
        ErrorVariableCanNotFindReference,
    };

    std::vector<std::string> errorStrList = {
        "ErrorInternal,					      ",
        "ErrorVariableCanNotFindReference,	  ",
    };

	int collectSchedule(const ILSchedule* schedule, ILParser* parser);
    
    const std::vector<ILAnalyzeScheduleNode*>* getScheduleNodeList() const;
    std::vector<ILAnalyzeScheduleNode*> getScheduleNodeListDataFlowOfOneNode(ILAnalyzeScheduleNode* node) const;
    std::vector<ILAnalyzeScheduleNode*> getScheduleNodeListContainByOneBranchNode(ILAnalyzeScheduleNode* node, bool exceptInnerBranch) const;

       // 对于折叠后的语句，重新收集表达式中的变量
    int recollectScheduleNodeExpressionVariable(ILAnalyzeScheduleNode* node);
    
    // 通过表达式获取其索引到的变量，遍历表达式的时候如遇到不确定索引，比如var[i]或(*(var + i)).m，则直接返回var，通过passExpression直接返回中断的Expression
    ILAnalyzeScheduleVariable* getVariableByExpressionAndDomain(Expression* expression, const std::vector<void*> &domain, Expression* &passExpression);
    ILAnalyzeScheduleVariable* getVariableMember(ILAnalyzeScheduleVariable* variable, std::string memberName) const;
    bool isSameDomain(const std::vector<void*> &domain1, const std::vector<void*> &domain2) const;
    // 作用域src是否包含作用域dst
    bool isDomainContain(const std::vector<void*> &domainSrc, const std::vector<void*> &domainDst) const;
    // 在重名变量中寻找符合作用域要求的变量
    ILAnalyzeScheduleVariable* getVariableInDomain(const std::vector<ILAnalyzeScheduleVariable*>* variableList, const std::vector<void*> &domain) const;
    // 在重名变量中寻找符合作用域要求的变量
    ILAnalyzeScheduleVariable* getVariableInDomain(std::string variableName, const std::vector<void*> &domain) const;
    // 获取变量表达式的数据类型
    ILAnalyzeScheduleVariableType getVariableExpressionType(const Expression* expression, const std::vector<void*> &domain);
    ILAnalyzeScheduleVariableType getVariableMemberType(ILAnalyzeScheduleVariableType* variableType, std::string memberName) const;
    // 判断表达式是否在函数参数中，即判断表达式上级是否存在函数节点
    bool isExpressionInFunction(const Expression* expression) const;
    // 判断表达式中是否在函数
    bool isExpressionContainFunction(const Expression* expression) const;

    Expression* addTypeConvertToExpression(Expression* expression, std::string type, const std::vector<void*> &domain);
    std::string getExpressionBasicType(Expression* expression, const std::vector<void*> &domain, bool &isAddress);
    
    
    // 一次优化后释放scheduleNodes中被优化掉的ILScheduleOptimizeNode
    // 同时如果是局部变量类型的节点，则释放scheduleVariables中对应的变量
    // comment = true;
    // int releaseScheduleOptimizeNodeAndVariable(ILOptimizeScheduleNodeVariableRef* variableStore, ILOptimizeScheduleNodeVariableRef* variableRead);
    int releaseScheduleOptimizeNodeAndVariable(ILAnalyzeScheduleNodeVariableRef* variableRef);
    int releaseScheduleOptimizeNode(const ILAnalyzeScheduleNode* node);

    
    // 寻找被单次赋值并之后未读写的变量(节点包含在ILScheduleOptimizeNodeVariableRef retVariableStore中)
    int optimizeFindSingleAssignedNoReadStoredVariable(int findIndex, ILAnalyzeScheduleNodeVariableRef* &retVariableStore) const;
    // 寻找被单次赋值并单次读的变量(节点包含在ILScheduleOptimizeNodeVariableRef retVariableStore中)
    int optimizeFindSingleAssignedReadVariable(int &findIndex, ILAnalyzeScheduleNodeVariableRef* &retVariableStore, ILAnalyzeScheduleNodeVariableRef* &retVariableRead) const;
    // 寻找该节点表达式中最左的被赋值变量
    ILAnalyzeScheduleNodeVariableRef* optimizeFindAssignedVariableInNode(const ILAnalyzeScheduleNode* node) const;
    // 从start节点开始寻找某变量被读入的节点，而且保证start节点之后仅被读取一次
    int optimizeFindVariableSingleReadIndex(int startIndex, const ILAnalyzeScheduleNodeVariableRef* variableRef, int &retIndex, ILAnalyzeScheduleNodeVariableRef* &retVariableRead) const;
    // 从start节点开始统计某变量被读入写出的次数
    int optimizeFindVariableReadStoredCount(int startIndex, const ILAnalyzeScheduleNodeVariableRef* variableRef) const;
    // 从循环中统计某变量被读入的次数
    int optimizeFindVariableReadCountInLoop(const ILAnalyzeScheduleNodeVariableRef* variableRef, const ILAnalyzeScheduleNode* loopNode) const;
    // 在start节点到end节点之间，某变量是否被赋值过
    bool optimizeIsVariableBeAssigned(int startIndex, int endIndex, ILAnalyzeScheduleNodeVariableRef* variableRef) const;
    // 是否variableSrc变量覆盖variableDst，比如a 覆盖 a.b
    bool optimizeAreVariableCovered(const ILAnalyzeScheduleNodeVariableRef* variableSrc, const ILAnalyzeScheduleNodeVariableRef* variableDst) const;
    // 判断在找到的当前节点 到 变量单次读取节点之间 ，被赋值变量的数据源的所有变量是否被赋过值
    bool optimizeAreVariableSourceBeAssigned(int startIndex, int endIndex, const ILAnalyzeScheduleNodeVariableRef* variableRef) const;
    // 判断一个节点中的变量在另一个domain中是否存在其它同名变量
    bool optimizeAreNodeVariablesConflictInDomain(const ILAnalyzeScheduleNode* node, const std::vector<void*> &domain) const;
    // 判断变量引用是否是一个数组或地址
    bool optimizeIsVariableRefAddress(ILAnalyzeScheduleNodeVariableRef* variableRef);
    // 判断节点内是否存在函数
    bool optimizeIsScheduleNodeContainFunction(const ILAnalyzeScheduleNode* node) const;

    // 获取一个节点的具有数据依赖(前驱)的全部节点
    std::vector<ILAnalyzeScheduleNode*> getVariableSourceNodesAll(const ILAnalyzeScheduleNode* node) const;
    // 获取一个节点的具有数据依赖(前驱)的直接引用节点
    std::vector<ILAnalyzeScheduleNode*> getVariableSourceNodesDirect(const ILAnalyzeScheduleNode* node) const;

    bool isExpressionVarLocalVariable(const std::string& expression) const;
    bool isExpressionVarGlobalVariable(const std::string& expression) const;
    bool isExpressionVarConstant(const std::string& expression) const;
    bool isExpressionVarParameter(const std::string& expression) const;

    std::string getExpressionVarReferedConstantValue(const std::string& expression) const;

	int release();
    
    void print();
    void printNodeList(std::vector<ILAnalyzeScheduleNode*>& nodeList);

private:
    
	const ILSchedule* schedule = nullptr;
	ILParser* iLParser = nullptr;
    
    // 从LocalVariable开始优化，忽略前面的GlobalVariable、Input和Output
    // 在优化过程中动态调整该列表
    std::vector<ILAnalyzeScheduleNode*> scheduleNodes;
    std::vector<ILAnalyzeScheduleNode*> scheduleNodeTree;
    
    // key: 顶级变量名，不考虑a.b的直接索引，因为可能出现不同数据类型的重名问题
    // value: 对应的变量
    // 考虑到变量重名的问题，同一个callName对应多个变量，由变量的作用域细分
    std::map<std::string, std::vector<ILAnalyzeScheduleVariable*>> scheduleVariables;


    int collectGlobalVariable();
    int collectInputVariable();
    int collectOutputVariable();
    int collectLocalVariable();
    
    int collectScheduleNode();
    int collectScheduleNodeTraverseBranch(ILBranch* iLBranch, ILAnalyzeScheduleNode* parent);
    int collectScheduleNodeTraverseCalculate(ILCalculate* iLCalculate, ILAnalyzeScheduleNode* parent);

    int calculateLocalVarIsReferToGlobalVar(ILAnalyzeScheduleNode* node);
    int calculateLocalVarIsConstant(ILAnalyzeScheduleNode* node);

     ILCalculate* currentTraverseILCalculate = nullptr;
    int collectScheduleNodeTraverseStatement(Statement* statement, ILCalculate* iLCalculate, ILAnalyzeScheduleNode* parent);
    int collectScheduleNodeTraverseStatementBranch(Statement* statement, ILCalculate* iLCalculate, ILAnalyzeScheduleNode* parent);
    int collectScheduleNodeTraverseStatementDoWhile(Statement* statement, ILCalculate* iLCalculate, ILAnalyzeScheduleNode* parent);
    int collectScheduleNodeTraverseStatementFor(Statement* statement, ILCalculate* iLCalculate, ILAnalyzeScheduleNode* parent);
    int collectScheduleNodeTraverseStatementLocalVariable(Statement* statement, ILCalculate* iLCalculate, ILAnalyzeScheduleNode* parent);
    int collectScheduleNodeTraverseStatementBatchCalculation(Statement* statement, ILCalculate* iLCalculate, ILAnalyzeScheduleNode* parent);

    // 收集表达式中的变量
    int collectScheduleNodeExpressionVariable(Expression* expression, ILAnalyzeScheduleNode* node, bool isAssigned = false);
    // 由于collectScheduleNodeExpressionVariable忽略了[]或(var + index)或func(...)里面的内容，所以要继续遍历索引
    int collectScheduleNodeExpressionVariableInner(Expression* expression, ILAnalyzeScheduleNode* node, bool isAssigned = false);

    // 根据得到的NodePath，获取完整的影响数据流的Node列表
    void getScheduleNodeListDataFlowOfOneNode(std::vector<ILAnalyzeScheduleNode*>& treePath, std::vector<ILAnalyzeScheduleNode*>& ret, std::vector<ILAnalyzeScheduleNode*>& mainStreamBranchNodeList) const;

    void getScheduleNodeListContainByOneBranchNode(ILAnalyzeScheduleNode* node, std::vector<ILAnalyzeScheduleNode*>& ret, bool exceptInnerBranch) const;

    void printVariables();
    void printNode(const ILAnalyzeScheduleNode* node);
    void printNodeLocalVariable(const ILAnalyzeScheduleNode* node);
    void printNodeBranch(const ILAnalyzeScheduleNode* node);
    void printNodeStatement(const ILAnalyzeScheduleNode* node);
    void printNodeStatementLocalVariable(const ILAnalyzeScheduleNode* node) const;
    void printNodeStatementBatchCalculation(const ILAnalyzeScheduleNode* node) const;
    void printNodeStatementStoreRead(const ILAnalyzeScheduleNode* node);
    void printNodeVariableExp(const ILAnalyzeScheduleNodeVariableRef* variableRef);

    const ILAnalyzeScheduleNode* getScheduleNodeTopLoop(const ILAnalyzeScheduleNode* node) const;

};
