#pragma once
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include "../Common/tinyxml2.h"
#include <iostream>

#include "../MIRBasic/MIRObject.h"
#include "../MIRBasic/MIRModel.h"
#include "../MIRBasic/MIRFunction.h"
#include "../MIRBasic/MIRActor.h"
#include "../MIRBasic/MIRInport.h"
#include "../MIRBasic/MIROutport.h"
#include "../MIRBasic/MIRActionPort.h "
#include "../MIRBasic/MIRRelation.h"
#include "../MIRBasic/MIRSaver.h"

#include "../Common/Utility.h"


class MIRPreprocessor
{
public:
	int preprocessMIR(MIRModel* model);

	MIRModel* model;

	void release();
	
	std::string getErrorStr();
    
    static std::string defaultType;

private:
	int errorCode = 0;
	void setCurrentError(int errorCode);

    int setDefaultPortName();
    int setDefaultPortNameOfFunction(MIRFunction* function);
    int setDefaultPortNameOfFunctionActor(MIRActor* actor);
    
    int setDefaultPortType();
    int setDefaultPortTypeOfFunctionInport(MIRFunction* function);
    int setDefaultPortTypeOfFunctionOutport(MIRFunction* function);
    int setDefaultPortTypeOfFunctionActionPort(MIRFunction* function);
    int setDefaultPortTypeOfFunctionActor(MIRFunction* function);
    int setDefaultPortTypeOfFunctionActorOne(MIRActor* actor);
    
    int loopBreak();
    int loopBreakOfFunction(MIRFunctionDataflow* function);
    int loopBreakOfFunctionRelation(MIRFunctionDataflow* function, std::string delayActorName);

    // 由于复合组件的端口名在Simulink中可能被保存为了“DefaultInport1”这样的
    // 所以需要将这些端口改名，以保证后续类型推导过程
    int setActorNameByFunctionPort();
    int setActorNameByFunctionPort(MIRFunction* function);
    int setActorNameByFunctionPortActor(MIRActor* actor, MIRFunction* refFunction);

    // 为了保证Simulink的If-Else子系统之后的Merge组件的代码能正常放到if-else语句里面，
    // 所以需要将Merge组件换成If-Else子系统的属性，然后在Function组件中生成赋值代码
    // 同时Merge组件也应该增加一个属性，表示该组件已经在If-Else子系统生成代码时生成了，
    // 就不需额外生成Merge组件的代码了
    int preprocessMergeActorAfterIfElseSwitchCaseSubsystem();
    int preprocessMergeActorAfterIfElseSwitchCaseSubsystem(MIRFunction* function);
    int preprocessMergeActorAfterIfElseSwitchCaseSubsystemActor(MIRActor* actor);

    
    int preprocessFromGoto();
    int preprocessFromGoto(MIRFunction* function);
    std::vector<std::string> preprocessFromGotoCollectGotoTag(MIRFunctionDataflow* function);
    int preprocessFromGotoCreateNewRelation(MIRFunctionDataflow* functionDataflow, std::string gotoTag, std::vector<MIROutport*>& portsOfFromActors, std::vector<MIRInport*>& portsOfGotoActors);
    int preprocessFromGotoDeleteOldRelations(MIRFunctionDataflow* functionDataflow, std::vector<MIROutport*>& portsOfFromActors, std::vector<MIRInport*>& portsOfGotoActors);
    int preprocessFromGotoDeleteFromGotoActors(MIRFunctionDataflow* functionDataflow, std::vector<MIROutport*>& portsOfFromActors, std::vector<MIRInport*>& portsOfGotoActors);

    
    int preprocessDataStoreMemoryReadAndWrite();
    int preprocessDataStoreMemoryReadAndWrite(MIRFunctionDataflow* functionDataflow);
    int preprocessDataStoreMemoryReadAndWriteRename(MIRFunctionDataflow* functionDataflow, std::string oldName, std::string newName, std::string dimensions, std::string outDataTypeStr);

    int preprocessFunctionParameter();
    int preprocessFunctionParameter(MIRFunctionDataflow* functionDataflow);
    // 直接将子系统参数的值替换到其它组件的参数值上，避免数组使用时无法解析具体数值
    int preprocessFunctionParameterRewrite(MIRFunctionDataflow* functionDataflow, std::string oldName, std::string newValue);



    std::unordered_map<std::string, MIRActor*> dataStoreMemories;
};

