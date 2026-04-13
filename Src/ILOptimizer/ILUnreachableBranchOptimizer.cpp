#include "ILUnreachableBranchOptimizer.h"
#include "../Common/Utility.h"

#include "ILOptimizer.h"

#include "../ILBasic/ILSchedule.h"
#include "../ILBasic/ILBranch.h"
#include "../ILBasic/ILCalculate.h"
#include "../ILBasic/ILFile.h"

#include "../ILBasic/ILSaver.h"
#include "../ILBasic/ILParser.h"
#include "../ILBasic/ILFunction.h"

using namespace std;

int ILUnreachableBranchOptimizer::optimize(ILParser* parser)
{
	this->iLParser = parser;

	int res = collectBranch();
	if(res < 0)
		return res;

	if(branchList.empty())
		return 0;

	res = runCBMC();
	if(res <= 0)
		return res;


	res = optimizeBranch();
	if(res < 0)
		return res;
	

	return 1;
}

namespace
{
    const int CONST_RUNCBMC_TIME = 12000;
}
int ILUnreachableBranchOptimizer::runCBMC() const
{
	string exePath = getExeDirPath();
	string exeParentPath = getParentPath(exePath);

	string callCodeGeneratorForCBMC = exeParentPath + "/ILTranslator/ILTranslator.exe";

    //comment = true;
    string callCBMC = exeParentPath + "/CBMC/cbmc.exe";
    //comment = true;
	//string callRunCBMC = exeParentPath + "/CBMC/RunCBMC.bat";
    //string callSetupMSVC = exeParentPath + "/CBMC/SetupMSVC.bat";

	if(!isFileExist(callCodeGeneratorForCBMC)) {
        callCodeGeneratorForCBMC = exeParentPath + "/ILTranslator/iltranslator.exe";
        if(!isFileExist(callCodeGeneratorForCBMC)) {
            debugLog("ErrorCanNotFindILTranslator", true);
            return 0;
            // comment = true;
            // -ILOptimizer::ErrorCanNotFindCodeGeneratorForCBMC;
        }
	}
	//if(!isFileExist(callRunCBMC)) {
    //    debugLog("ErrorCanNotFindRunCBMC", true);
	//    return 0;
    //    // comment = true;
	//    // -ILOptimizer::ErrorCanNotFindRunCBMC;
	//}
	if(!isFileExist(callCBMC)) {
        debugLog("ErrorCanNotFindCBMC", true);
	    return 0;
        // comment = true;
	    // -ILOptimizer::ErrorCanNotFindCBMC;
	}
    //comment = true;
	//if(!isFileExist(callSetupMSVC))
	//	return -ILOptimizer::ErrorCanNotFindSetupMSVC;



	ILSaver iLSaver;
	iLSaver.save(iLParser,exePath + "/temp/ILTempU.xml");

	string cmd;
	string para = " -u true";
	para += " -l cbmc";
	cmd = "\"\"" + callCodeGeneratorForCBMC + "\"" + para + " -i " + "\"" + exePath + "\"" + "/temp/ILTempU.xml -o " + "\"" + exePath + "/temp" + "\"\""; // 1>nul 2>nul
	system(cmd.c_str());

	setCurrentDirPath(exeParentPath + "/CBMC");
	//cmd = "pwd";
	//system(cmd.c_str());

	//cmd = callRunCBMC + " " + exePath + "/temp/main.c" + " " + exePath + "/temp/res.txt"; // 1>nul 2>nul
	//system(cmd.c_str());
    
    writeFile(exePath + "/temp/mainU.c", readFile(exePath + "/temp/main.c"));
    
    string cbmcPara = string("--gcc --arch i386 ") + "\"" + exePath + "/temp/mainU.c" + "\"";
    systemCallWithTimeOut(callCBMC, cbmcPara, CONST_RUNCBMC_TIME, exePath + "/temp/resU.txt");

	setCurrentDirPath(exePath);

	//cmd = "call \"" + callSetupMSVC + "\"";
	//system(cmd.c_str());
	//
	//cmd = "cd .";
	//system(cmd.c_str());
	//
	//cmd = callCBMC + " temp/main.c > temp/res.txt";
	//system(cmd.c_str());
    
	return 1;
}

int ILUnreachableBranchOptimizer::collectBranch()
{
	for(int i = 0; i < iLParser->files.size(); i++)
	{
		int res = traverseILFile(iLParser->files[i]);
		if(res < 0)
			return res;
	}
	return 1;
}

int ILUnreachableBranchOptimizer::traverseILFile(ILFile* file)
{
	for(int i = 0; i < file->functions.size(); i++)
	{
		int res = traverseILFunction(file->functions[i]);
		if(res < 0)
			return res;
	}
	return 1;
}

int ILUnreachableBranchOptimizer::traverseILFunction(ILFunction* function)
{
	int res = traverseILSchedule(function->schedule);
	if(res < 0)
		return res;
	return 1;
}

int ILUnreachableBranchOptimizer::traverseILSchedule(ILSchedule* schedule)
{
	for(int i = 0; i < schedule->scheduleNodes.size(); i++)
	{
		ILScheduleNode* scheduleNode = schedule->scheduleNodes[i];
		if(scheduleNode->objType == ILScheduleNode::TypeBranch)
		{
			int res = traverseILBranch(static_cast<ILBranch*>(scheduleNode));
			if(res < 0)
				return res;
		}
        else if(scheduleNode->objType == ILScheduleNode::TypeCalculate)
        {
            int res = traverseILCalculate(static_cast<ILCalculate*>(scheduleNode));
			if(res < 0)
				return res;
        }
	}
	return 1;
}


int ILUnreachableBranchOptimizer::traverseILCalculate(ILCalculate* calculate)
{
    for(int i = 0; i < calculate->statements.childStatements.size(); i++)
    {
        int res = traverseStatement(calculate->statements.childStatements[i]);
		if(res < 0)
			return res;
    }
    return 1;
}

int ILUnreachableBranchOptimizer::traverseILBranch(ILBranch* branch)
{
	branchList.push_back(branch);
    
	for(int i = 0; i < branch->scheduleNodes.size(); i++)
	{
		ILScheduleNode* scheduleNode = branch->scheduleNodes[i];
		if(scheduleNode->objType == ILScheduleNode::TypeBranch)
		{
			int res = traverseILBranch(static_cast<ILBranch*>(scheduleNode));
			if(res < 0)
				return res;
		}
	    else if(scheduleNode->objType == ILScheduleNode::TypeCalculate)
        {
            int res = traverseILCalculate(static_cast<ILCalculate*>(scheduleNode));
			if(res < 0)
				return res;
        }
	}

	return 1;
}

int ILUnreachableBranchOptimizer::traverseStatement(Statement* statement)
{
	if(statement->type == Statement::If || 
        statement->type == Statement::ElseIf || 
        statement->type == Statement::Else)
		branchList.emplace_back(statement);

    for(int i = 0; i < statement->childStatements.size(); i++)
    {
        int res = traverseStatement(statement->childStatements[i]);
		if(res < 0)
			return res;
    }
    return 1;
}


namespace
{
    const int OPTIMIZEBRANCH_NUM_13 = 13;
    const int OPTIMIZEBRANCH_NUM_2 = 2;
}
int ILUnreachableBranchOptimizer::optimizeBranch()
{
	string exePath = getExeDirPath();
	string cbmcResPath = exePath + "/temp/resU.txt";
	string cbmcRes = readFile(cbmcResPath);

	vector<string> cbmcBranchResList;
	size_t resPos = cbmcRes.find("** Results:");
    size_t resPos2;
	size_t endPos;
	resPos = cbmcRes.find("assertion 0:",resPos);
	while(resPos != string::npos)
	{
		string branchRes;
		resPos += OPTIMIZEBRANCH_NUM_13;
		endPos = cbmcRes.find("\n",resPos);
		branchRes = stringTrim(cbmcRes.substr(resPos,endPos-resPos));
		cbmcBranchResList.push_back(branchRes);

        resPos = cbmcRes.find("assertion 1:",endPos);
        resPos += OPTIMIZEBRANCH_NUM_13;
		endPos = cbmcRes.find("\n",resPos);
		branchRes = stringTrim(cbmcRes.substr(resPos,endPos-resPos));
		cbmcBranchResList.push_back(branchRes);

		resPos = cbmcRes.find("assertion 0:",endPos);
	}


	if(cbmcBranchResList.size() != branchList.size() * OPTIMIZEBRANCH_NUM_2)
		return ILOptimizer::ErrorInternal;

	for(int i = branchList.size() - 1; i >= 0; i--)
	{
		BranchInfo branch = branchList[i];
		if(!(cbmcBranchResList[i*2] == "SUCCESS" && cbmcBranchResList[i*2 + 1] == "SUCCESS"))
		{
            continue;
		}

        if(branch.iLBranch != nullptr)
        {
            for(int j = branch.iLBranch->scheduleNodes.size() - 1; j >= 0; j--)
            {
                iLParser->releaseILObject(branch.iLBranch->scheduleNodes[j]);
            }
        }
        else
        {
            for(int j = branch.statement->childStatements.size() - 1; j >= 0; j--)
            {
                iLParser->releaseStatement(branch.statement->childStatements[j]);
            }
        }

	}

	return 1;
}
