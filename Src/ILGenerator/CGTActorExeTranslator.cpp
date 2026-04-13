#include "CGTActorExeTranslator.h"

#include <windows.h>
#include <string>
#include <algorithm>

#include "CGTActorExeTransActionPort.h"
#include "CodeGenerationTable.h"

#include "MIRScheduleGraphDataflowNode.h"
#include "../MIRBasic/MIRActor.h"
#include "../MIRBasic/MIRParameter.h"

#include "CGTActorExeTransRoot.h"
#include "CGTActorExeTransModelInfo.h"
#include "CGTActorExeTransFunctionInfo.h"
#include "CGTActorExeTransActorInfo.h"
#include "CGTActorExeTransSaver.h"
#include "CGTActorExeTransSourceOutportInfo.h"
#include "MIRScheduleGraphActionPort.h"
#include "MIRScheduleGraphDataflow.h"
#include "MIRScheduleGraphInport.h"
#include "MIRScheduleGraphOutport.h"
#include "../Common/Utility.h"
#include "../ILBasic/ILParser.h"
#include "../ILBasic/ILLocalVariable.h"
#include "../ILBasic/ILSchedule.h"
#include "../ILBasic/ILHeadFile.h"
#include "../ILBasic/ILMacro.h"
#include "../ILBasic/ILTypeDefine.h"
#include "../ILBasic/ILEnumDefine.h"
#include "../ILBasic/ILStructDefine.h"
#include "../ILBasic/ILUtilityFunction.h"
#include "../ILBasic/ILGlobalVariable.h"
#include "../ILBasic/ILFunction.h"
#include "../ILBasic/ILInput.h"
#include "../ILBasic/ILOutput.h"


using namespace std;


typedef int(*TemplateDLLInit)(const char*, int, char*, int);
typedef int(*TemplateDLLTrans)(char*, int, char*, int);

std::map<std::string, void*> CGTActorExeTranslator::templateLibraries;//HMODULE
std::map<std::string, void*> CGTActorExeTranslator::templateInitProc; //FARPROC
std::map<std::string, void*> CGTActorExeTranslator::templateTransProc; //FARPROC


int CGTActorExeTranslator::transCGTActorExe(const CGTActorExe* actorExe, const MIRModel* model, std::string& retILInformation) //, ILFunction* functionInit, ILFunction* functionExec, ILCalculate* retILCalculate, ILFile* retILFile)
{
    int res;
    string actorName = actorExe->node->actor->name;
    string actorType = actorExe->node->actor->type;
    //retILCalculate->name = actorName;

    string templateInputStr;
    res = transCGTActorExeToTemplateInput(actorExe, model, templateInputStr);
    if(res < 0)
    {
        return res;
    }
    
    //cout << templateInputStr << "\n\n";

    res = loadActorLibrary(actorType);
    if(res < 0)
    {
        return res;
    }
    string retILXMLStr;
    res = transCGTActorExeToILXMLStr(actorType, templateInputStr, retILXMLStr);
    if(res < 0)
    {
        debugLog("Template Error: " + actorType + " -> " + actorName + "\n");
        return res;
    }

    retILInformation = retILXMLStr;

    //res = transILXMLStrToIL(retILXMLStr, retILFile, functionInit, functionExec, retILCalculate);
    //if(res < 0)
    //{
    //    return res;
    //}
    //
    //res = transILTypeInference(retILCalculate, const_cast<CGTActorExe*>(actorExe));
    //if(res < 0)
    //{
    //    return res;
    //}

    return 1;
}

int CGTActorExeTranslator::transCGTActorExeToTemplateInput(const CGTActorExe* actorExe, const MIRModel* model, std::string &retStr)
{
    int res;
    CGTActorExeTransRoot root;
    root.modelInfo = new CGTActorExeTransModelInfo();
    root.modelInfo->name = model->name;
    root.functionInfo = new CGTActorExeTransFunctionInfo();
    root.functionInfo->name = actorExe->node->graph->name;
    root.actorInfo = new CGTActorExeTransActorInfo();
    root.actorInfo->name = actorExe->node->actor->name;
    root.actorInfo->type = actorExe->node->actor->type;


    for(int i = 0; i < actorExe->node->inports.size(); i++)
    {
        CGTActorExeTransInport* tempCGTActorExeTransInport = nullptr;
        res = transCGTActorExeInport(actorExe, actorExe->node->inports[i], &tempCGTActorExeTransInport);
        if(res < 0)
        {
            root.release();
            return res;
        }
        if(!tempCGTActorExeTransInport)
            continue;
        root.actorInfo->inports.push_back(tempCGTActorExeTransInport);
    }

    for(int i = 0; i < actorExe->node->outports.size(); i++)
    {
        CGTActorExeTransOutport* tempCGTActorExeTransOutport = nullptr;
        res = transCGTActorExeOutport(actorExe->node->outports[i], &tempCGTActorExeTransOutport);
        if(res < 0)
        {
            root.release();
            return res;
        }
        root.actorInfo->outports.push_back(tempCGTActorExeTransOutport);
    }

    for(int i = 0; i < actorExe->node->actionPorts.size(); i++)
    {
        CGTActorExeTransActionPort* tempCGTActorExeTransActionPort = nullptr;
        res = transCGTActorExeActionPort(actorExe, actorExe->node->actionPorts[i], &tempCGTActorExeTransActionPort);
        if(res < 0)
        {
            root.release();
            return res;
        }
        if (tempCGTActorExeTransActionPort)
            root.actorInfo->actionPorts.push_back(tempCGTActorExeTransActionPort);
    }

    for(auto iter = actorExe->node->actor->parameters.begin(); iter != actorExe->node->actor->parameters.end(); iter++)
    {
        CGTActorExeTransParameter* tempCGTActorExeTransParameter = new CGTActorExeTransParameter();
        tempCGTActorExeTransParameter->name = (*iter)->name;
        tempCGTActorExeTransParameter->value = (*iter)->value;
        root.actorInfo->parameters.push_back(tempCGTActorExeTransParameter);
    }
    
    root.sourceOutportInfo = new CGTActorExeTransSourceOutportInfo();
    for(int j = 0; j < actorExe->realBranchPathDataSrc.srcs.size(); j++)
    {
        const MIRScheduleGraphOutport* srcOutport = actorExe->realBranchPathDataSrc.srcs[j];
        CGTActorExeTransSourceOutport* tempCGTActorExeTransSourceOutport = nullptr;
        res = transCGTActorExeSourceOutport(srcOutport, &tempCGTActorExeTransSourceOutport);
        if(res < 0)
        {
            root.release();
            return res;
        }
        root.sourceOutportInfo->sourceOutports.push_back(tempCGTActorExeTransSourceOutport);
    }

    CGTActorExeTransSaver saver;
    string ret;
    res = saver.save(&root, ret);
    root.release();
    if(res < 0)
    {
        return res;
    }
    retStr = ret;
    
    return 1;
}

int CGTActorExeTranslator::transCGTActorExeInport(const CGTActorExe* actorExe, const MIRScheduleGraphInport* inport, CGTActorExeTransInport** retInport)
{
    vector<MIRScheduleGraphOutport*> srcOutport = inport->getRealSrcOutport(actorExe);
    if(srcOutport.empty())
    {
        *retInport = nullptr;
        return 0;
    }
    
    CGTActorExeTransInport* tempCGTActorExeTransInport = new CGTActorExeTransInport();
    tempCGTActorExeTransInport->name = inport->inportFromActor->name;
    tempCGTActorExeTransInport->type = inport->inportFromActor->type;
    tempCGTActorExeTransInport->isAddress = inport->inportFromActor->isAddress;
    tempCGTActorExeTransInport->arraySize = inport->inportFromActor->arraySize;
    for(int i = 0; i < srcOutport.size(); i++)
    {
        string srcStr;
        if(srcOutport[i]->outportFromActor)
            srcStr = srcOutport[i]->outportFromActor->fullName;
        else
            srcStr = srcOutport[i]->inportFromFunction->name;
        if(i > 0)
            srcStr = "," + srcStr;
        tempCGTActorExeTransInport->sourceOutport += srcStr;
    }
    
    *retInport = tempCGTActorExeTransInport;
    return 1;
}

int CGTActorExeTranslator::transCGTActorExeOutport(const MIRScheduleGraphOutport* outport,
    CGTActorExeTransOutport** retOutport)
{
    CGTActorExeTransOutport* tempCGTActorExeTransOutport = new CGTActorExeTransOutport();
    tempCGTActorExeTransOutport->name = outport->outportFromActor->name;
    tempCGTActorExeTransOutport->type = outport->outportFromActor->type;
    tempCGTActorExeTransOutport->isAddress = outport->outportFromActor->isAddress;
    tempCGTActorExeTransOutport->arraySize = outport->outportFromActor->arraySize;
    *retOutport = tempCGTActorExeTransOutport;

    return 1;
}

int CGTActorExeTranslator::transCGTActorExeActionPort(const CGTActorExe* actorExe, const MIRScheduleGraphActionPort* actionPort,
    CGTActorExeTransActionPort** retActionPort)
{
    MIRScheduleGraphOutport* srcOutport = actionPort->getRealSrcOutport(actorExe);
    if(!srcOutport)
    {
        *retActionPort = nullptr;
        return 0;
    }

    CGTActorExeTransActionPort* tempCGTActorExeTransActionPort = new CGTActorExeTransActionPort();
    tempCGTActorExeTransActionPort->name = actionPort->actionPort->name;
    tempCGTActorExeTransActionPort->type = actionPort->actionPort->type;

    if(srcOutport->outportFromActor)
        tempCGTActorExeTransActionPort->sourceOutport = srcOutport->outportFromActor->fullName;
    else
        tempCGTActorExeTransActionPort->sourceOutport = srcOutport->inportFromFunction->name;

    *retActionPort = tempCGTActorExeTransActionPort;

    return 1;
}

int CGTActorExeTranslator::transCGTActorExeSourceOutport(const MIRScheduleGraphOutport* srcOutport,
                                                         CGTActorExeTransSourceOutport** retSourceOutport)
{
    CGTActorExeTransSourceOutport* tempCGTActorExeTransSourceOutport = new CGTActorExeTransSourceOutport();
    if(srcOutport->outportFromActor)
    {
        MIRActor* outportParent = (MIRActor*)srcOutport->outportFromActor->parent;
        tempCGTActorExeTransSourceOutport->actorName = outportParent->name;
        tempCGTActorExeTransSourceOutport->actorType = outportParent->type;
        tempCGTActorExeTransSourceOutport->name = srcOutport->outportFromActor->name;
        tempCGTActorExeTransSourceOutport->type = srcOutport->outportFromActor->type;
        tempCGTActorExeTransSourceOutport->isAddress = srcOutport->outportFromActor->isAddress;
        tempCGTActorExeTransSourceOutport->arraySize = srcOutport->outportFromActor->arraySize;
    }
    else
    {
        tempCGTActorExeTransSourceOutport->actorName = "";
        tempCGTActorExeTransSourceOutport->actorType = "CompositeActor";
        tempCGTActorExeTransSourceOutport->name = srcOutport->inportFromFunction->name;
        tempCGTActorExeTransSourceOutport->type = srcOutport->inportFromFunction->type;
        tempCGTActorExeTransSourceOutport->isAddress = srcOutport->inportFromFunction->isAddress;
        tempCGTActorExeTransSourceOutport->arraySize = srcOutport->inportFromFunction->arraySize;
    }
    *retSourceOutport = tempCGTActorExeTransSourceOutport;
    return 1;
}

int CGTActorExeTranslator::loadActorLibrary(std::string actorType)
{
    HINSTANCE hInst = NULL;
	if (templateLibraries.find(actorType) == templateLibraries.end())
	{
        string exePath = getExeDirPath();
		string actorLibraryPath = exePath + "\\ILTemplateDataflow\\" + actorType + "\\" + actorType + ".dll";
		hInst = LoadLibraryA(actorLibraryPath.c_str());
        int err = GetLastError();
		templateLibraries[actorType] = (void*)hInst;
	}
	else
	{
		hInst = (HINSTANCE)templateLibraries.at(actorType);
	}
	if (!hInst)
	{
        cout << "Error: " << actorType << " Actor Template Not Found.\n";
		return -ErrorLoadActorLibraryFile;
	}

    TemplateDLLInit templateDLLInit;
    if (hInst && templateInitProc.find(actorType) == templateInitProc.end())
	{
		templateDLLInit = (TemplateDLLInit)GetProcAddress(hInst, "init");
		if(templateDLLInit)
			templateInitProc[actorType] = (void*)templateDLLInit;
	}
	else
	{
		templateDLLInit = (TemplateDLLInit)templateInitProc.at(actorType);
	}
	if (!templateDLLInit)
	{
		return -ErrorActorLibraryCanNotFindInitProc;
	}

    TemplateDLLTrans templateDLLTrans;
    if (hInst && templateTransProc.find(actorType) == templateTransProc.end())
	{
		templateDLLTrans = (TemplateDLLTrans)GetProcAddress(hInst, "trans");
		if(templateDLLTrans)
			templateTransProc[actorType] = (void*)templateDLLTrans;
	}
	else
	{
		templateDLLTrans = (TemplateDLLTrans)templateTransProc.at(actorType);
	}
	if (!templateDLLTrans)
	{
		return -ErrorActorLibraryCanNotFindTransProc;
	}

    return 1;
}

namespace
{
    const int TRANS_CGTACTOREXE_TO_IL_BUF_LEN = 65536;
}
int CGTActorExeTranslator::transCGTActorExeToILXMLStr(std::string actorType, std::string actorExeTemplateInput, std::string &retILXMLStr)
{
    TemplateDLLInit templateDLLInit = (TemplateDLLInit)templateInitProc.at(actorType);
    TemplateDLLTrans templateDLLTrans = (TemplateDLLTrans)templateTransProc.at(actorType);


    char buf[TRANS_CGTACTOREXE_TO_IL_BUF_LEN];
    char error[TRANS_CGTACTOREXE_TO_IL_BUF_LEN];
    int res = templateDLLInit(actorExeTemplateInput.c_str(), actorExeTemplateInput.length(), error, TRANS_CGTACTOREXE_TO_IL_BUF_LEN);
    if(res < 0)
        return res;

    res = templateDLLTrans(buf, TRANS_CGTACTOREXE_TO_IL_BUF_LEN, error, TRANS_CGTACTOREXE_TO_IL_BUF_LEN);
    if(res < 0)
        return res;

    retILXMLStr = string(buf);

    return 1;
}


void CGTActorExeTranslator::releaseActorLibraries()
{
    auto iter = templateLibraries.begin();
    for(; iter != templateLibraries.end(); iter++)
    {
        FreeLibrary((HINSTANCE)iter->second);
    }
    templateLibraries.clear();
    templateInitProc.clear();
    templateTransProc.clear();
}
