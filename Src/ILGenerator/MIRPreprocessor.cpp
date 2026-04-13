#include "MIRPreprocessor.h"

#include "MIRScheduleAnalyzer.h"
#include "../Common/Utility.h"

#include "../MIRBasic/MIRFunctionDataflow.h"
#include "../MIRBasic/MIRActor.h"
#include <set>
#include <string>
#include <algorithm>
using namespace std;

string MIRPreprocessor::defaultType = "f32";

int MIRPreprocessor::preprocessMIR(MIRModel* model)
{
    int res;

    this->model = model;

    res = setActorNameByFunctionPort();
    if(res < 0)
        return res;

    res = setDefaultPortName();
    if(res < 0)
        return res;
    
    res = setDefaultPortType();
    if(res < 0)
        return res;
    
    res = preprocessFromGoto();
    if (res < 0)
        return res;

    res = loopBreak();
    if(res < 0)
        return res;

    
    res = preprocessMergeActorAfterIfElseSwitchCaseSubsystem();
    if(res < 0)
        return res;

    res = preprocessDataStoreMemoryReadAndWrite();
    if (res < 0)
        return res;


    res = preprocessFunctionParameter();
    if (res < 0)
        return res;
    

    return 1;
}

void MIRPreprocessor::release()
{
}

string MIRPreprocessor::getErrorStr()
{
    return string();
}

void MIRPreprocessor::setCurrentError(int errorCode)
{
}

int MIRPreprocessor::setDefaultPortName()
{
    int res;
    for(int i = 0; i < model->functions.size(); i++)
    {
        res = setDefaultPortNameOfFunction(model->functions[i]);
        if(res < 0)
            return res;
    }
    return 1;
}

int MIRPreprocessor::setDefaultPortNameOfFunction(MIRFunction* function)
{
    int res;
    for(int i = 0; i < function->inports.size(); i++)
    {
        MIRInport* inport = function->inports[i];
        if(stringStartsWith(inport->name, "#"))
        {
            inport->name = inport->name.substr(1, inport->name.length() - 1);
        }
    }
    for(int i = 0; i < function->outports.size(); i++)
    {
        MIROutport* outport = function->outports[i];
        if(stringStartsWith(outport->name, "#"))
        {
            outport->name = outport->name.substr(1, outport->name.length() - 1);
        }
    }
    for(int i = 0; i < function->actionPorts.size(); i++)
    {
        MIRActionPort* actionPort = function->actionPorts[i];
        if(stringStartsWith(actionPort->name, "#"))
        {
            actionPort->name = actionPort->name.substr(1, actionPort->name.length() - 1);
        }
    }
    if(function->type == MIRFunction::TypeDataflow)
    {
        MIRFunctionDataflow* functionDataflow = static_cast<MIRFunctionDataflow*>(function);
        for(int i = 0; i < functionDataflow->actors.size(); i++)
        {
            res = setDefaultPortNameOfFunctionActor(functionDataflow->actors[i]);
            if(res < 0)
                return res;
        }
    }
    return 1;
}

int MIRPreprocessor::setDefaultPortNameOfFunctionActor(MIRActor* actor)
{
    for(int i = 0; i < actor->inports.size(); i++)
    {
        MIRInport* inport = actor->inports[i];
        if(stringStartsWith(inport->name, "#"))
        {
            inport->name = inport->name.substr(1, inport->name.length() - 1);
            inport->fullName = actor->name + "." + inport->name;
        }
    }
    for(int i = 0; i < actor->outports.size(); i++)
    {
        MIROutport* outport = actor->outports[i];
        if(stringStartsWith(outport->name, "#"))
        {
            outport->name = outport->name.substr(1, outport->name.length() - 1);
            outport->fullName = actor->name + "." + outport->name;
        }
    }
    for(int i = 0; i < actor->actionPorts.size(); i++)
    {
        MIRActionPort* actionPort = actor->actionPorts[i];
        if(stringStartsWith(actionPort->name, "#"))
        {
            actionPort->name = actionPort->name.substr(1, actionPort->name.length() - 1);
            actionPort->fullName = actor->name + "." + actionPort->name;
        }
    }

    return 1;
}

// Directly preprocessing default data types may cause type inference errors. This function only handles Inport temporarily; others are handled by type inference.
int MIRPreprocessor::setDefaultPortType()
{
    int res;
    for(int i = 0; i < model->functions.size(); i++)
    {
        res = setDefaultPortTypeOfFunctionInport(model->functions[i]);
        if(res < 0)
            return res;
    }
    return 1;
}

int MIRPreprocessor::setDefaultPortTypeOfFunctionInport(MIRFunction* function)
{
    for(int i = 0; i < function->inports.size(); i++)
    {
        MIRInport* inport = function->inports[i];
        if(stringStartsWith(inport->type, "#"))
        {
            inport->type = MIRPreprocessor::defaultType;
        }
    }
    return 1;
}

int MIRPreprocessor::setDefaultPortTypeOfFunctionOutport(MIRFunction* function)
{
    for(int i = 0; i < function->outports.size(); i++)
    {
        MIROutport* outport = function->outports[i];
        if(stringStartsWith(outport->type, "#"))
        {
            outport->type = MIRPreprocessor::defaultType;
        }
    }
    return 1;
}

int MIRPreprocessor::setDefaultPortTypeOfFunctionActionPort(MIRFunction* function)
{
    for(int i = 0; i < function->actionPorts.size(); i++)
    {
        MIRActionPort* actionPort = function->actionPorts[i];
        if(stringStartsWith(actionPort->type, "#"))
        {
            actionPort->type = MIRPreprocessor::defaultType;
        }
    }
    return 1;
}

int MIRPreprocessor::setDefaultPortTypeOfFunctionActor(MIRFunction* function)
{
    int res;
    if(function->type == MIRFunction::TypeDataflow)
    {
        MIRFunctionDataflow* functionDataflow = static_cast<MIRFunctionDataflow*>(function);
        for(int i = 0; i < functionDataflow->actors.size(); i++)
        {
            res = setDefaultPortTypeOfFunctionActorOne(functionDataflow->actors[i]);
            if(res < 0)
                return res;
        }
    }
    return 1;
}

int MIRPreprocessor::setDefaultPortTypeOfFunctionActorOne(MIRActor* actor)
{
    for(int i = 0; i < actor->inports.size(); i++)
    {
        MIRInport* inport = actor->inports[i];
        if(stringStartsWith(inport->type, "#"))
        {
            inport->type = MIRPreprocessor::defaultType;
        }
    }
    for(int i = 0; i < actor->outports.size(); i++)
    {
        MIROutport* outport = actor->outports[i];
        if(stringStartsWith(outport->type, "#"))
        {
            outport->type = MIRPreprocessor::defaultType;
        }
    }
    for(int i = 0; i < actor->actionPorts.size(); i++)
    {
        MIRActionPort* actionPort = actor->actionPorts[i];
        if(stringStartsWith(actionPort->type, "#"))
        {
            actionPort->type = MIRPreprocessor::defaultType;
        }
    }

    return 1;
}

int MIRPreprocessor::loopBreak()
{
    int res;
    for(int i = 0; i < model->functions.size(); i++)
    {
        MIRFunction* function = model->functions[i];
        if(function->type != MIRFunction::TypeDataflow)
            continue;

        res = loopBreakOfFunction(static_cast<MIRFunctionDataflow*>(function));
        if(res < 0)
            return res;
    }
    return 1;
}

namespace{
    map<string, int> delayActorNameCountMap; //??????Delay?????????????????????
}

int MIRPreprocessor::loopBreakOfFunction(MIRFunctionDataflow* function)
{
    int res;

    for(int i = 0; i < function->actors.size(); i++)
    {
        MIRActor* actor = function->actors[i];
        if(actor->type != "UnitDelay" && actor->type != "Delay") {
            continue;
        }

        string delayActorName = actor->name;

        if(delayActorNameCountMap.find(delayActorName) == delayActorNameCountMap.end())
            delayActorNameCountMap[delayActorName] = 1;
        else
            delayActorNameCountMap[delayActorName]++;



        MIRActor* delayStore = actor;
        MIRActor* delayRead = new MIRActor();
        function->actors.push_back(delayRead);
        delayRead->objType = MIRObject::TypeActor;
        delayRead->outports = delayStore->outports;
        delayRead->name = delayActorName + "_Read";
        delayRead->type = "DelayRead";
        //delayRead->parameters = delayStore->parameters;
        for(int j = 0; j < delayStore->parameters.size(); j++)
        {
            delayRead->parameters.push_back(delayStore->parameters[j]->clone());
        }

        delayStore->outports.clear();
        delayStore->name = delayActorName + "_Store";
        delayStore->type = "DelayStore";

        for(int j = 0; j < delayStore->inports.size(); j++)
        {
            delayStore->inports[j]->fullName = delayStore->name + "." + delayStore->inports[j]->name;
        }

        for(int j = 0; j < delayRead->outports.size(); j++)
        {
            delayRead->outports[j]->fullName = delayRead->name + "." + delayRead->outports[j]->name;
            delayRead->outports[j]->parent = delayRead;
        }

        if(delayActorNameCountMap.at(delayActorName) == 1) {
            delayRead->addParameter("StateVariableName", delayActorName + "_state");
            delayStore->addParameter("StateVariableName", delayActorName + "_state");
        } else {
            delayRead->addParameter("StateVariableName", delayActorName + "_state" + to_string(delayActorNameCountMap.at(delayActorName) - 1));
            delayStore->addParameter("StateVariableName", delayActorName + "_state" + to_string(delayActorNameCountMap.at(delayActorName) - 1));
        }

        res = loopBreakOfFunctionRelation(function, delayActorName);
        if(res < 0)
            return res;

    }

    return 1;
}

int MIRPreprocessor::loopBreakOfFunctionRelation(MIRFunctionDataflow* function, std::string delayActorName)
{
    for(int i = 0; i < function->relations.size(); i++)
    {
        MIRRelation* relation = function->relations[i];
        for(int j = 0; j < relation->srcStrs.size(); j++)
        {
            if(!stringStartsWith(relation->srcStrs[j], delayActorName + "."))
            {
                continue;
            }
            vector<string> portNameSplit = stringSplit(relation->srcStrs[j], ".");
            relation->srcStrs[j] = portNameSplit[0] + "_Store." + portNameSplit[1];
        }
        for(int j = 0; j < relation->dstStrs.size(); j++)
        {
            if(!stringStartsWith(relation->dstStrs[j], delayActorName + "."))
            {
                continue;
            }
            vector<string> portNameSplit = stringSplit(relation->dstStrs[j], ".");
            relation->dstStrs[j] = portNameSplit[0] + "_Read." + portNameSplit[1];
        }
    }

    return 1;
}

int MIRPreprocessor::setActorNameByFunctionPort()
{
    int res;
    for(int i = 0; i < model->functions.size(); i++)
    {
        res = setActorNameByFunctionPort(model->functions[i]);
        if(res < 0)
            return res;
    }
    return 1;
}

int MIRPreprocessor::setActorNameByFunctionPort(MIRFunction* function)
{
    int res;
    if(function->type == MIRFunction::TypeDataflow)
    {
        MIRFunctionDataflow* functionDataflow = static_cast<MIRFunctionDataflow*>(function);
        for(int i = 0; i < functionDataflow->actors.size(); i++)
        {
            MIRActor* actor = functionDataflow->actors[i];

            if(actor->type != "Function")
                continue;
            
            string functionName = actor->getParameterByName("FunctionRef")->value;
            MIRFunction* refFunction = model->getFunctionByName(functionName);
            if(!refFunction)
                return -MIRScheduleAnalyzer::ErrorUnknownFunctionName;

            res = setActorNameByFunctionPortActor(actor, refFunction);
            if(res < 0)
                return res;
        }
    }
    return 1;
}

int MIRPreprocessor::setActorNameByFunctionPortActor(MIRActor* actor, MIRFunction* refFunction)
{
    for(int i = 0; i < actor->inports.size(); i++)
    {
        actor->inports[i]->name = refFunction->inports[i]->name;
        actor->inports[i]->fullName = actor->name + "." + refFunction->inports[i]->name;
    }
    for(int i = 0; i < actor->outports.size(); i++)
    {
        actor->outports[i]->name = refFunction->outports[i]->name;
        actor->outports[i]->fullName = actor->name + "." + refFunction->outports[i]->name;
    }

    MIRFunctionDataflow* parentFunction = (MIRFunctionDataflow*)actor->parent;

    for(int i = 0; i < parentFunction->relations.size(); i++)
    {
        MIRRelation* relation = parentFunction->relations[i];
        for(int j = 0; j < relation->dstObjs.size(); j++)
        {
            if(relation->dstObjs[j]->parent == actor)
            {
                MIRInport* dstPort = (MIRInport*)relation->dstObjs[j];
                relation->dstStrs[j] = actor->name + "." + dstPort->name;
            }
        }
        for(int j = 0; j < relation->srcObjs.size(); j++)
        {
            if(relation->srcObjs[j]->parent == actor)
            {
                MIROutport* srcPort = (MIROutport*)relation->srcObjs[j];
                relation->srcStrs[j] = actor->name + "." + srcPort->name;
            }
        } 
    }

    return 1;
}

int MIRPreprocessor::preprocessMergeActorAfterIfElseSwitchCaseSubsystem()
{
    int res;
    for(int i = 0; i < model->functions.size(); i++)
    {
        res = preprocessMergeActorAfterIfElseSwitchCaseSubsystem(model->functions[i]);
        if(res < 0)
            return res;
    }
    return 1;
}

int MIRPreprocessor::preprocessMergeActorAfterIfElseSwitchCaseSubsystem(MIRFunction* function)
{
    int res;
    if(function->type != MIRFunction::TypeDataflow)
    {
        return 0;
    }
    MIRFunctionDataflow* functionDataflow = static_cast<MIRFunctionDataflow*>(function);
    for(int i = 0; i < functionDataflow->actors.size(); i++)
    {
        MIRActor* actor = functionDataflow->actors[i];

        if(actor->type != "Merge")
            continue;
        
        res = preprocessMergeActorAfterIfElseSwitchCaseSubsystemActor(actor);
        if(res < 0)
            return res;
    }
    return 1;
}

int MIRPreprocessor::preprocessMergeActorAfterIfElseSwitchCaseSubsystemActor(MIRActor* actor)
{
    MIRFunctionDataflow* functionContainer = (MIRFunctionDataflow*)actor->parent;

    // ??Merge????????????IfSubsystem
    for(int i = 0; i < actor->inports.size(); i++)
    {
        MIRInport* inport = actor->inports[i];
        vector<MIRRelation*> relation = functionContainer->getRelationByPort(inport);
        if (!relation.empty())
        {
            if (relation[0]->dstObjs.size() != 1 || relation[0]->srcObjs.size() != 1)
                return 0;
            if (relation[0]->srcObjs[0]->objType != MIRObject::TypeOutport)
                return 0;
            MIROutport* srcOutport = (MIROutport*)relation[0]->srcObjs[0];
            if (srcOutport->parent->objType != MIRObject::TypeActor)
                return 0;
            MIRActor* srcActor = (MIRActor*)srcOutport->parent;
            if (srcActor->type != "Function" || srcActor->actionPorts.size() != 1 || srcActor->actionPorts[0]->type != "IfAction")
                return 0;
        }
    }
    if(actor->outports.size() != 1)
        return 0;

    MIROutport* outport = actor->outports[0];
    for(int i = 0; i < actor->inports.size(); i++)
    {
        MIRInport* inport = actor->inports[i];
        vector<MIRRelation*> relation = functionContainer->getRelationByPort(inport);
        if (!relation.empty())
        {
            MIROutport* srcOutport = (MIROutport*)relation[0]->srcObjs[0];
            MIRActor* srcActor = (MIRActor*)srcOutport->parent;

            MIRParameter* mergeOutputPara = srcActor->getParameterByName("OutputRedefine");
            if (!mergeOutputPara)
            {
                mergeOutputPara = new MIRParameter();
                srcActor->parameters.push_back(mergeOutputPara);
                mergeOutputPara->parent = srcActor;

                mergeOutputPara->name = "OutputRedefine";
                mergeOutputPara->value = "";
            }
            mergeOutputPara->value += srcOutport->name + "=>" + actor->name + "." + outport->name + ";";
        }
    }

    MIRParameter* mergePara = new MIRParameter();
    actor->parameters.push_back(mergePara);
    mergePara->parent = actor;
    mergePara->name = "JustGenerateVariable";
    mergePara->value = "true";
    return 1;
}


int MIRPreprocessor::preprocessFromGoto()
{
    
    int res;
    for(int i = 0; i < model->functions.size(); i++)
    {
        res = preprocessFromGoto(model->functions[i]);
        if(res < 0)
            return res;
    }
    return 1;
}

int MIRPreprocessor::preprocessFromGoto(MIRFunction* function)
{
    int res;
    if(function->type != MIRFunction::TypeDataflow)
    {
        return 0;
    }
    MIRFunctionDataflow* functionDataflow = static_cast<MIRFunctionDataflow*>(function);
    
    vector<string> gotoTagSet = preprocessFromGotoCollectGotoTag(functionDataflow);

    for(int i = 0; i < gotoTagSet.size(); i++)
    {
        string gotoTag = gotoTagSet[i];
        vector<MIROutport*> portsOfFromActors;
        vector<MIRInport*> portsOfGotoActors;

        preprocessFromGotoCreateNewRelation(functionDataflow, gotoTag, portsOfFromActors, portsOfGotoActors);
        
        preprocessFromGotoDeleteOldRelations(functionDataflow, portsOfFromActors, portsOfGotoActors);

        preprocessFromGotoDeleteFromGotoActors(functionDataflow, portsOfFromActors, portsOfGotoActors);

    }


    return 1;
}

vector<std::string> MIRPreprocessor::preprocessFromGotoCollectGotoTag(MIRFunctionDataflow* function)
{
    set<string> gotoTagSet;

    for(int i = 0; i < function->actors.size(); i++)
    {
        MIRActor* actor = function->actors[i];
        if(actor->type != "Goto" && actor->type != "From")
            continue;
        MIRParameter* tag = actor->getParameterByName("GotoTag");
        if(!tag)
        {
            tag = actor->addParameter("GotoTag", "A");
        }

        gotoTagSet.insert(tag->value);
    }

    vector<string> ret(gotoTagSet.begin(), gotoTagSet.end());

    return ret;
}

int MIRPreprocessor::preprocessFromGotoCreateNewRelation(MIRFunctionDataflow* functionDataflow,
    string gotoTag,
    vector<MIROutport*>& portsOfFromActors,
    vector<MIRInport*>& portsOfGotoActors)
{
    for(int j = 0; j < functionDataflow->actors.size(); j++)
    {
        MIRActor* actor = functionDataflow->actors[j];
        if(actor->type == "From")
        {
            MIRParameter* tag = actor->getParameterByName("GotoTag");
            if(tag->value == gotoTag)
            {
                portsOfFromActors.push_back(actor->outports[0]);
            }
        }
        else if(actor->type == "Goto")
        {
            MIRParameter* tag = actor->getParameterByName("GotoTag");
            if(tag->value == gotoTag)
            {
                portsOfGotoActors.push_back(actor->inports[0]);
            }
        }
    }

    set<MIROutport*> linkedOutports;
    for(int j = 0; j < portsOfGotoActors.size(); j++)
    {
        MIRInport* inport = portsOfGotoActors[j];
        vector<MIROutport*> linkedOutportsOfOutport = functionDataflow->getLinkedOutportsByInport(inport);
        for(int k = 0; k < linkedOutportsOfOutport.size(); k++)
        {
            linkedOutports.insert(linkedOutportsOfOutport[k]);
        }
    }

    set<MIRInport*> linkedInports;
    for(int j = 0; j < portsOfFromActors.size(); j++)
    {
        MIROutport* outport = portsOfFromActors[j];
        vector<MIRInport*> linkedInportsOfOutport = functionDataflow->getLinkedInportsByOutport(outport);
        for(int k = 0; k < linkedInportsOfOutport.size(); k++)
        {
            linkedInports.insert(linkedInportsOfOutport[k]);
        }
    }

    MIRRelation* relation = new MIRRelation();

    for (std::set<MIROutport*>::iterator it = linkedOutports.begin(); it != linkedOutports.end(); ++it) {
        relation->srcObjs.push_back(*it);
        relation->srcStrs.push_back((*it)->fullName);
    }
    for (std::set<MIRInport*>::iterator it = linkedInports.begin(); it != linkedInports.end(); ++it) {
        relation->dstObjs.push_back(*it);
        relation->dstStrs.push_back((*it)->fullName);
    }

    functionDataflow->relations.push_back(relation);

    return 1;
}

int MIRPreprocessor::preprocessFromGotoDeleteOldRelations(MIRFunctionDataflow* functionDataflow, std::vector<MIROutport*>& portsOfFromActors, std::vector<MIRInport*>& portsOfGotoActors)
{
    for(int i = 0; i < portsOfFromActors.size(); i++)
    {
        MIROutport* outport = portsOfFromActors[i];
        vector<MIRRelation*> relations = functionDataflow->getRelationByPort(outport);
        for(int j = relations.size() - 1; j >= 0; j--)
        {
            MIRRelation* relation = relations[j];
            if(relation->srcObjs.size() == 1)
            {
                functionDataflow->deleteRelation(relation);
            }
            else
            {
                relation->deleteSrc(outport);
            }
        }
    }

    for (int i = 0; i < portsOfGotoActors.size(); i++)
    {
        MIRInport* inport = portsOfGotoActors[i];
        vector<MIRRelation*> relations = functionDataflow->getRelationByPort(inport);
        for(int j = relations.size() - 1; j >= 0; j--)
        {
            MIRRelation* relation = relations[j];
            if (relation->dstObjs.size() == 1)
            {
                functionDataflow->deleteRelation(relation);
            }
            else
            {
                relation->deleteDst(inport);
            }
        }
    }

    return 1;
}

int MIRPreprocessor::preprocessFromGotoDeleteFromGotoActors(MIRFunctionDataflow* functionDataflow, std::vector<MIROutport*>& portsOfFromActors, std::vector<MIRInport*>& portsOfGotoActors)
{
    for(int i = 0; i < portsOfFromActors.size(); i++)
    {
        MIROutport* outport = portsOfFromActors[i];
        MIRActor* actor = (MIRActor*)outport->parent;
        functionDataflow->deleteActor(actor);
    }

    for (int i = 0; i < portsOfGotoActors.size(); i++)
    {
        MIRInport* inport = portsOfGotoActors[i];
        MIRActor* actor = (MIRActor*)inport->parent;
        functionDataflow->deleteActor(actor);
    }

    return 1;
}

int MIRPreprocessor::preprocessDataStoreMemoryReadAndWrite()
{
    int res;
    for(int i = 0; i < model->functions.size(); i++)
    {
        if(model->functions[i]->name == model->mainFunction)
        {
            return preprocessDataStoreMemoryReadAndWrite((MIRFunctionDataflow*)(model->functions[i]));
        }
    }
    return 0;
}

namespace{
    map<string, int> dataStoreMemoryNameUsed;
    map<MIRObject*, string> dataStoreMemoryNameOri;
}

int MIRPreprocessor::preprocessDataStoreMemoryReadAndWrite(MIRFunctionDataflow* functionDataflow)
{
    for (int i = 0; i < functionDataflow->actors.size(); i++)
    {
        MIRActor* actor = functionDataflow->actors[i];
        if(actor->type != "DataStoreMemory") {
            continue;
        }

        MIRParameter* dataStoreName = actor->getParameterByName("DataStoreName");
        if(!dataStoreName)
            dataStoreName = actor->addParameter("DataStoreName", "A");

        string dataStoreNameStr = dataStoreName->value;
        string newdataStoreNameStr;
        if(dataStoreMemoryNameUsed.find(dataStoreNameStr) == dataStoreMemoryNameUsed.end()) {
            newdataStoreNameStr = actor->name + "_" + dataStoreNameStr;
            dataStoreMemoryNameUsed[dataStoreNameStr] = 1;
        } else {
            newdataStoreNameStr = actor->name + "_" + dataStoreNameStr + "_" + to_string(dataStoreMemoryNameUsed[dataStoreNameStr]);
            dataStoreMemoryNameUsed[dataStoreNameStr]++;
        }
        dataStoreName->value = newdataStoreNameStr;

        MIRParameter* dataStoreDimensions = actor->getParameterByName("Dimensions");
        if(!dataStoreDimensions)
            dataStoreDimensions = actor->addParameter("Dimensions", "");
        else if(dataStoreDimensions->value == "1")
            dataStoreDimensions->value = "";
        MIRParameter* dataStoreOutDataTypeStr = actor->getParameterByName("OutDataTypeStr");
        if(!dataStoreOutDataTypeStr)
            dataStoreOutDataTypeStr = actor->addParameter("OutDataTypeStr", "f32");
        
        preprocessDataStoreMemoryReadAndWriteRename(functionDataflow, dataStoreNameStr, newdataStoreNameStr, dataStoreDimensions->value, dataStoreOutDataTypeStr->value);
    }

    for (int i = 0; i < functionDataflow->actors.size(); i++)
    {
        MIRActor* actor = functionDataflow->actors[i];
        if(actor->type != "Function") {
            continue;
        }

        MIRParameter* refFunctionName = actor->getParameterByName("FunctionRef");

        MIRFunctionDataflow* refFunction = (MIRFunctionDataflow*)(model->getFunctionByName(refFunctionName->value));

        preprocessDataStoreMemoryReadAndWrite(refFunction);
    }

    return 1;
}

int MIRPreprocessor::preprocessDataStoreMemoryReadAndWriteRename(MIRFunctionDataflow* functionDataflow, std::string oldName, std::string newName, string dimensions, string outDataTypeStr)
{
    for (int i = 0; i < functionDataflow->actors.size(); i++)
    {
        MIRActor* actor = functionDataflow->actors[i];
        if(actor->type != "DataStoreRead" && actor->type != "DataStoreWrite") {
            continue;
        }

        MIRParameter* dataStoreName = actor->getParameterByName("DataStoreName");
        if(!dataStoreName)
            dataStoreName = actor->addParameter("DataStoreName", "A");

        if(dataStoreMemoryNameOri.find(actor) == dataStoreMemoryNameOri.end())
        {
            dataStoreMemoryNameOri[actor] = dataStoreName->value;
        }

        if(dataStoreMemoryNameOri.at(actor) == oldName)
        {
            dataStoreName->value = newName;
            MIRParameter* dataStoreDimensions = actor->addParameter("Dimensions", dimensions);
            MIRParameter* dataStoreOutDataTypeStr = actor->addParameter("OutDataTypeStr", outDataTypeStr);
        }
    }

    for (int i = 0; i < functionDataflow->actors.size(); i++)
    {
        MIRActor* actor = functionDataflow->actors[i];
        if(actor->type != "Function") {
            continue;
        }

        MIRParameter* refFunctionName = actor->getParameterByName("FunctionRef");

        MIRFunctionDataflow* refFunction = (MIRFunctionDataflow*)(model->getFunctionByName(refFunctionName->value));

        preprocessDataStoreMemoryReadAndWriteRename(refFunction, oldName, newName, dimensions, outDataTypeStr);
    }


    return 1;
}


int MIRPreprocessor::preprocessFunctionParameter()
{
    int res;
    for(int i = 0; i < model->functions.size(); i++)
    {
        if(model->functions[i]->name == model->mainFunction)
        {
            return preprocessFunctionParameter((MIRFunctionDataflow*)(model->functions[i]));
        }
    }
    return 0;
}

namespace{

    vector<string> parameterNoNeedToGenerate = {
        "FunctionRef",
        "OutputRedefine",
        "TreatAsAtomicUnit",
    };
}

int MIRPreprocessor::preprocessFunctionParameter(MIRFunctionDataflow* functionDataflow)
{
    for (int i = 0; i < functionDataflow->actors.size(); i++)
    {
        MIRActor* actor = functionDataflow->actors[i];
        if(actor->type != "Function") {
            continue;
        }

        string functionName = actor->getParameterByName("FunctionRef")->value;

        MIRFunctionDataflow* refFunction = (MIRFunctionDataflow*)(model->getFunctionByName(functionName));

        for(int j = 0; j < actor->parameters.size(); j++)
        {
            MIRParameter* parameter = actor->parameters[j];
            if(find(parameterNoNeedToGenerate.begin(), parameterNoNeedToGenerate.end(), parameter->name) != parameterNoNeedToGenerate.end())
                continue;

            preprocessFunctionParameterRewrite(refFunction, parameter->name, parameter->value);
        }
        
        
    }

    for (int i = 0; i < functionDataflow->actors.size(); i++)
    {
        MIRActor* actor = functionDataflow->actors[i];
        if(actor->type != "Function") {
            continue;
        }

        MIRParameter* refFunctionName = actor->getParameterByName("FunctionRef");

        MIRFunctionDataflow* refFunction = (MIRFunctionDataflow*)(model->getFunctionByName(refFunctionName->value));

        preprocessFunctionParameter(refFunction);
    }

    return 1;
}

int MIRPreprocessor::preprocessFunctionParameterRewrite(MIRFunctionDataflow* functionDataflow, std::string oldName, std::string newValue)
{
    for (int i = 0; i < functionDataflow->actors.size(); i++)
    {
        MIRActor* actor = functionDataflow->actors[i];
        
        for (int j = 0; j < actor->parameters.size(); j++)
        {
            MIRParameter* parameter = actor->parameters[j];
            if(actor->type == "Function" && (parameter->name == "FunctionRef" || parameter->name == oldName))
                continue;

            if(parameter->value == oldName)
            {
                parameter->value = newValue;
            }
        }

    }

    for (int i = 0; i < functionDataflow->actors.size(); i++)
    {
        MIRActor* actor = functionDataflow->actors[i];
        if(actor->type != "Function") {
            continue;
        }

        MIRParameter* refFunctionName = actor->getParameterByName("FunctionRef");

        
        if(actor->getParameterByName(oldName)) {
            continue;
        }

        MIRFunctionDataflow* refFunction = (MIRFunctionDataflow*)(model->getFunctionByName(refFunctionName->value));

        preprocessFunctionParameterRewrite(refFunction, oldName, newValue);
    }

    return 1;
}

