#pragma once

#include <map>
#include <string>

#include "CGTActorExeTransInport.h"
#include "CGTActorExeTransOutport.h"
#include "CGTActorExeTransSourceOutport.h"
#include "CodeGenerationTable.h"

#include "MIRScheduleAnalyzerDataflow.h"

#include "../ILBasic/ILCalculate.h"

class CGTActorExeTransActionPort;

class CGTActorExeTranslator
{
public:
    enum {
		ErrorLoadActorLibraryFile  = 150001,
		ErrorActorLibraryCanNotFindInitProc,
		ErrorActorLibraryCanNotFindTransProc,
	};

    int transCGTActorExe(const CGTActorExe* actorExe, const MIRModel* model, std::string& retILInformation);//, const MIRScheduleAnalyzer* analyzer, ILFunction* functionInit, ILFunction* functionExec, ILCalculate* retILCalculate, ILFile* retILFile);

    static void releaseActorLibraries();

private:

    int transCGTActorExeToTemplateInput(const CGTActorExe* actorExe, const MIRModel* model, std::string &retStr);

    int transCGTActorExeInport(const CGTActorExe* actorExe, const MIRScheduleGraphInport* inport, CGTActorExeTransInport** retInport);
    int transCGTActorExeOutport(const MIRScheduleGraphOutport* outport, CGTActorExeTransOutport** retOutport);
    int transCGTActorExeActionPort(const CGTActorExe* actorExe, const MIRScheduleGraphActionPort* actionPort, CGTActorExeTransActionPort** retActionPort);
    int transCGTActorExeSourceOutport(const MIRScheduleGraphOutport* srcOutport, CGTActorExeTransSourceOutport** retSourceOutport);

    
    static std::map<std::string, void*> templateLibraries;//HMODULE
    static std::map<std::string, void*> templateInitProc; //FARPROC
    static std::map<std::string, void*> templateTransProc; //FARPROC

    int loadActorLibrary(std::string actorType);

    int transCGTActorExeToILXMLStr(std::string actorType, std::string actorExeTemplateInput, std::string &retILXMLStr);

};


