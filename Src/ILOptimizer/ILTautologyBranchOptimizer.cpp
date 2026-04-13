#include "ILTautologyBranchOptimizer.h"

#include <algorithm>

#include "../Common/Utility.h"

#include "ILOptimizer.h"

#include "../ILBasic/ILSchedule.h"
#include "../ILBasic/ILFile.h"
#include "../ILBasic/ILBranch.h"
#include "../ILBasic/ILCalculate.h"
#include "../ILBasic/ILFunction.h"

#include "../ILBasic/ILSaver.h"
#include "../ILBasic/ILParser.h"

using namespace std;


int ILTautologyBranchOptimizer::optimize(ILParser* parser)
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
int ILTautologyBranchOptimizer::runCBMC() const
{
	string exePath = getExeDirPath();
	string exeParentPath = getParentPath(exePath);
    
	string callCodeGeneratorForCBMC = exeParentPath + "/ILTranslator/ILTranslator.exe";

    //comment = true;
    string callCBMC = exeParentPath + "/CBMC/cbmc.exe";
    //comment = true;
	//string callRunCBMC = exeParentPath + "/CBMC/RunCBMC.bat";
	//string callSetupMSVC = exeParentPath + "/CBMC/SetupMSVC.bat";

	if(!isFileExist(callCodeGeneratorForCBMC))
	{
        callCodeGeneratorForCBMC = exeParentPath + "/ILTranslator/iltranslator.exe";
        if(!isFileExist(callCodeGeneratorForCBMC)) {
            debugLog("ErrorCanNotFindILTranslator", true);
            return 0;
            // comment = true;
            // -ILOptimizer::ErrorCanNotFindCodeGeneratorForCBMC;
        }
	}
	//if(!isFileExist(callRunCBMC))
	//{
    //    debugLog("ErrorCanNotFindRunCBMC", true);
	//    return 0;
    //    // comment = true;
	//    // -ILOptimizer::ErrorCanNotFindRunCBMC;
	//}
	if(!isFileExist(callCBMC))
	{
        debugLog("ErrorCanNotFindCBMC", true);
	    return 0;
        // comment = true;
	    // -ILOptimizer::ErrorCanNotFindCBMC;
	}
    //comment = true;
	//if(!isFileExist(callSetupMSVC))
	//	return -ILOptimizer::ErrorCanNotFindSetupMSVC;

	ILSaver iLSaver;
	iLSaver.save(iLParser,exePath + "/temp/ILTempT.xml");

	string cmd;
	string para = " -t true";
	para += " -l cbmc";
	cmd = "\"\"" + callCodeGeneratorForCBMC + "\"" + para + " -i " + "\"" + exePath + "\"" + "/temp/ILTempT.xml -o " + "\"" + exePath + "/temp" + "\"\""; // 1>nul 2>nul
	system(cmd.c_str());

	setCurrentDirPath(exeParentPath + "/CBMC");
	//cmd = "pwd";
	//system(cmd.c_str());

	//cmd = callRunCBMC + " " + exePath + "/temp/main.c" + " " + exePath + "/temp/res.txt"; // 1>nul 2>nul
	//system(cmd.c_str());

    writeFile(exePath + "/temp/mainT.c", readFile(exePath + "/temp/main.c"));

    string cbmcPara = string("--gcc --arch i386 ") + "\"" + exePath + "/temp/mainT.c" + "\"";
    systemCallWithTimeOut(callCBMC, cbmcPara, CONST_RUNCBMC_TIME, exePath + "/temp/resT.txt");

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

int ILTautologyBranchOptimizer::collectBranch()
{
	for(int i = 0; i < iLParser->files.size(); i++)
	{
		int res = traverseILFile(iLParser->files[i]);
		if(res < 0)
			return res;
	}
	return 1;
}

int ILTautologyBranchOptimizer::traverseILFile(ILFile* file)
{
	for(int i = 0; i < file->functions.size(); i++)
	{
		int res = traverseILFunction(file->functions[i]);
		if(res < 0)
			return res;
	}
	return 1;
}

int ILTautologyBranchOptimizer::traverseILFunction(ILFunction* function)
{
	int res = traverseILSchedule(function->schedule);
	if(res < 0)
		return res;
	return 1;
}

int ILTautologyBranchOptimizer::traverseILSchedule(ILSchedule* schedule)
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

int ILTautologyBranchOptimizer::traverseILCalculate(ILCalculate* calculate)
{
    for(int i = 0; i < calculate->statements.childStatements.size(); i++)
    {
        int res = traverseStatement(calculate->statements.childStatements[i]);
		if(res < 0)
			return res;
    }
    return 1;
}


int ILTautologyBranchOptimizer::traverseILBranch(ILBranch* branch)
{
	if(branch->type == ILBranch::TypeIf)
		branchList.emplace_back(branch);
    
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

int ILTautologyBranchOptimizer::traverseStatement(Statement* statement)
{
	if(statement->type == Statement::If)
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
    const int OPTIMIZEBRANCH_NUM_2 = 2;
}
int ILTautologyBranchOptimizer::optimizeBranch()
{
	string exePath = getExeDirPath();
	string cbmcResPath = exePath + "/temp/resT.txt";
	string cbmcRes = readFile(cbmcResPath);

	vector<string> cbmcBranchResList;
	size_t resPos = cbmcRes.find("** Results:");
	size_t endPos;
	resPos = cbmcRes.find(" assertion ",resPos);
	while(resPos != string::npos)
	{
		string branchRes;
		resPos = cbmcRes.find(":",resPos);
		resPos += OPTIMIZEBRANCH_NUM_2;
		endPos = cbmcRes.find("\n",resPos);
		branchRes = stringTrim(cbmcRes.substr(resPos,endPos-resPos));
		cbmcBranchResList.push_back(branchRes);
		resPos = cbmcRes.find(" assertion ",endPos);
	}
    //VERIFICATION SUCCESSFUL
    if(cbmcBranchResList.empty())
    {
        resPos = cbmcRes.find("VERIFICATION SUCCESSFUL");
        if(resPos != string::npos)
        {
            for(int i = 0; i < branchList.size(); i++)
                cbmcBranchResList.push_back("SUCCESS");
        }
    }
	if(cbmcBranchResList.size() != branchList.size())
		return ILOptimizer::ErrorInternal;

	for(int i = branchList.size() - 1; i >= 0; i--)
	{
		BranchInfo branch = branchList[i];
		if(cbmcBranchResList[i] != "SUCCESS")
		{
			continue;
		}

        if(branch.iLBranch != nullptr)
        {
            vector<ILScheduleNode*>* parentScheduleNodes = nullptr;
			
			if(branch.iLBranch->parent->objType == ILObject::TypeBranch)
			{
				parentScheduleNodes = &(static_cast<ILBranch*>(branch.iLBranch->parent)->scheduleNodes);
			}
			else if(branch.iLBranch->parent->objType == ILObject::TypeSchedule)
			{
				parentScheduleNodes = &(static_cast<ILSchedule*>(branch.iLBranch->parent)->scheduleNodes);
			}
            if(!parentScheduleNodes)
                continue;

			int insertPos = find(parentScheduleNodes->begin(),parentScheduleNodes->end(), branch.iLBranch) - parentScheduleNodes->begin();

			for(int j = 0; j < branch.iLBranch->scheduleNodes.size(); j++)
			{
				parentScheduleNodes->insert(parentScheduleNodes->begin() + insertPos + j, branch.iLBranch->scheduleNodes[j]);
				branch.iLBranch->scheduleNodes[j]->parent = branch.iLBranch->parent;
			}
			branch.iLBranch->scheduleNodes.clear();
			iLParser->releaseILObject(branch.iLBranch);
        }
        else
        {
            vector<Statement*>* parentStatements = nullptr;
            parentStatements = &(branch.statement->parentStatement->childStatements);
            
            if(!parentStatements)
                continue;

			int insertPos = find(parentStatements->begin(),parentStatements->end(), branch.statement) - parentStatements->begin();

			for(int j = 0; j < branch.statement->childStatements.size(); j++)
			{
				parentStatements->insert(parentStatements->begin() + insertPos + j, branch.statement->childStatements[j]);
				branch.statement->childStatements[j]->parentStatement = branch.statement->parentStatement;
			}
			branch.statement->childStatements.clear();
			iLParser->releaseStatement(branch.statement);
            
        }
	}

	return 1;
}
