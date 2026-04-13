#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "ILTranslator.h"
#include "../ILBasic/ILParser.h"
#include "../ILBasic/ILBranch.h"
#include "../ILBasic/Statement.h"
#include "../ILBasic/StmtIf.h"
#include "ILTranslatorForC.h"
#include "ILTranslatorForCBMC.h"
#include "ILTranslatorForTCGStateSearch.h"
#include "ILTranslatorForLibFuzzer.h"
#include "ILTranslatorForTCGHybrid.h"
#include "ILTranslatorForCoverage.h"
#include "ILTranslatorForSimulator.h"

#include "../Common/ArgumentParser.h"
#include "../Common/Utility.h"
#include "../ILBasic/ILCalculate.h"

#include "../ILBasic/ILSaver.h"
#include "../TCGBasic/TCGConfigParser.h"
#include "../SimulatorBasic/SimulatorConfigParser.h"
#include "../ILBasic/ILPreprocessor.h"
#include "../ILBasic/ILSchedule.h"
#include "ILObligationSlicer.h"

using namespace std;

string prepareHelpStr()
{
    string helpStr;
	helpStr += "-help    help information\n";
	helpStr += "-h       help information\n";
    helpStr += "-l=c     ========================   generate c file   ========================\n";
	helpStr += "-i       input file path of IL code file\n";
	helpStr += "-o       output dir of generated code\n";
	helpStr += "-d       target device (Intel, ARMv8, Tricore, RISC-V)\n";
	// ????????????	helpStr += "-c       generate comment. default is \"true\"\n";
	helpStr += "-t       generate test code. default is \"false\"\n";
	helpStr += "-p       generate parameters as static. default is \"false\"\n";
	helpStr += "-v       generate variables as static. default is \"false\"\n";
    helpStr += "-mbc     merge batch calculation in one for loop. default is \"false\"\n";
    helpStr += "-l=cbmc  ======================== generate cbmc file =========================\n";
	helpStr += "-config  config file path of IL code file\n";
	helpStr += "-i       input file path of IL code file\n";
	helpStr += "-o       output dir of generated code\n";
	helpStr += "-f       full parameter assert enable(default is false)\n";
	helpStr += "-u       generate unreachable branch assert(default is false)\n";
	helpStr += "-t       generate tautology branch assert(default is false)\n";
	helpStr += "-b       generate branch cover(default is false)\n";
	helpStr += "-g       generate global variable reset(default is false)\n";
	helpStr += "-r       generate parameter range constraint(default is false)\n";
	helpStr += "-ca      cut off the array size of all array variables(default is 0)\n";
	helpStr += "-c       generate comment. default is \"false\"\n";
	helpStr += "-m       generate multi-entries. default is \"false\"\n";
	helpStr += "-id      instrument IL Else branch (for decision coverage). default is \"false\"\n";
	helpStr += "-ic      instrument boolean expression (for condition coverage). default is \"false\"\n";
    helpStr += "-imcdc   instrument mcdc. default is \"false\"\n";
    helpStr += "-celse   complete else branch. default is \"false\"\n";
    helpStr += "-ms      multi-step. default is \"1\"\n";
    helpStr += "-gsp     generate solving model parameter code. default is \"true\"\n";
    helpStr += "-l=tcg_state_search  ============= generate TCG State Search file ============\n";
	helpStr += "-config  config file path of IL code file\n";
	helpStr += "-i       input file path of IL code file\n";
	helpStr += "-o       output dir of generated code\n";
	helpStr += "-c       generate comment. default is \"true\"\n";
	helpStr += "-ca      cut off the array size of all array variables(default is 0)\n";
	helpStr += "-id      instrument IL Else branch (for decision coverage). default is \"false\"\n";
	helpStr += "-ic      instrument boolean expression (for condition coverage). default is \"false\"\n";
    helpStr += "-imcdc   instrument mcdc. default is \"false\"\n";
    helpStr += "-celse   complete else branch. default is \"false\"\n";
    helpStr += "-bdg     enable branch distance guidance. default is \"true\"\n";
    helpStr += "-adg     enable array distance guidance. default is \"true\"\n";
    helpStr += "-l=libfuzzer  ============= generate LibFuzzer file ============\n";
	helpStr += "-config  config file path of IL code file\n";
	helpStr += "-i       input file path of IL code file\n";
	helpStr += "-o       output dir of generated code\n";
	helpStr += "-c       generate comment. default is \"true\"\n";
	helpStr += "-ca      cut off the array size of all array variables(default is 0)\n";
    helpStr += "-r       generate parameter range constraint(default is false)\n";
	helpStr += "-id      instrument IL Else branch (for decision coverage). default is \"false\"\n";
	helpStr += "-ic      instrument boolean expression (for condition coverage). default is \"false\"\n";
    helpStr += "-tp      targeting only model parameters. default is \"false\"\n";
    helpStr += "-imcdc   instrument mcdc. default is \"false\"\n";
    helpStr += "-celse   complete else branch. default is \"false\"\n";
    helpStr += "-gtcc    generate test case converter. default is \"true\"\n";
    helpStr += "-mic     max model iterator count. default is \"-1\"\n";
    helpStr += "-l=tcg_hybrid  ============= generate TCG Hybrid file ============\n";
    helpStr += "-config  config file path of IL code file\n";
    helpStr += "-i       input file path of IL code file\n";
    helpStr += "-o       output dir of generated code\n";
    helpStr += "-c       generate comment. default is \"true\"\n";
    helpStr += "-ca      cut off the array size of all array variables(default is 0)\n";
    helpStr += "-id      instrument IL Else branch (for decision coverage). default is \"false\"\n";
    helpStr += "-ic      instrument boolean expression (for condition coverage). default is \"false\"\n";
    helpStr += "-imcdc   instrument mcdc. default is \"false\"\n";
    helpStr += "-celse   complete else branch. default is \"false\"\n";
    helpStr += "-slice_bm      optional: obligation slice bitmap list (comma-separated ints, one int, or path to line-based list)\n";
    helpStr += "-slice_request optional: hybrid stall JSON with uncovered_branch_bitmap_indices; if both empty, tries <output>/SliceOut/hybrid_coverage_stall_slice_request.json\n";
    helpStr += "-l=obligation_slice   same pipeline as tcg_hybrid (use with -slice_bm / -slice_request)\n";
	helpStr += "-l=simulator  ============= generate Simulator file ============\n";
	helpStr += "-config  config file path of IL code file\n";
	helpStr += "-i       input file path of IL code file\n";
	helpStr += "-o       output dir of generated code\n";
	helpStr += "-c       generate comment. default is \"true\"\n";
	helpStr += "-t       generate test code. default is \"false\"\n";
	helpStr += "-p       generate parameters as static. default is \"false\"\n";
	helpStr += "-v       generate variables as static. default is \"false\"\n";
	helpStr += "-l=coverage  ============= generate Coverage Collection file ============\n";
	helpStr += "-i       input file path of IL code file\n";
	helpStr += "-it      input file path of test case\n";
	helpStr += "-o       output dir of generated code\n";
	helpStr += "-c       generate comment. default is \"true\"\n";
	helpStr += "-id      instrument IL Else branch (for decision coverage). default is \"false\"\n";
	helpStr += "-ic      instrument boolean expression (for condition coverage). default is \"false\"\n";
    helpStr += "-imcdc   instrument mcdc. default is \"false\"\n";
    helpStr += "-celse   complete else branch. default is \"false\"\n";

    return helpStr;
}

int translateForC(ArgumentParser& argumentParser, string inputFilePath, string outputDir)
{
    
	string generateComment = "true";
	string generateTestTime = "false";
	string packageParameter = "false";
	string packageVariable = "false";
	string generateTestCorrectness = "false";
	string targetDevice = "Intel";


	inputFilePath =		        argumentParser.getFlagValue("-i", inputFilePath);
	outputDir =			        argumentParser.getFlagValue("-o", outputDir);
	generateComment =	        stringToLower(argumentParser.getFlagValue("-c", generateComment));
	generateTestTime =	        stringToLower(argumentParser.getFlagValue("-t", generateTestTime));
    packageParameter =          argumentParser.getFlagValue("-p", packageParameter);
    packageVariable =           argumentParser.getFlagValue("-v", packageVariable);
	generateTestCorrectness =   stringToLower(argumentParser.getFlagValue("-tc", generateTestCorrectness));
	targetDevice =              argumentParser.getFlagValue("-d", targetDevice);

    ILTranslator::setConfig(argumentParser);

    cout << argumentParser.flagToString();

    ILTranslatorForC::setConfigGenerateComment(generateComment == "true");
    ILTranslatorForC::setConfigTestTime(generateTestTime == "true");
	ILTranslatorForC::setConfigPackageParameter(packageParameter == "true");
	ILTranslatorForC::setConfigPackageVariable(packageVariable == "true");
    ILTranslatorForC::setConfigTestCorrectness(generateTestCorrectness == "true");
	ILTranslatorForC::setConfigTargetDevice(targetDevice);
    

    ILParser parser;
	int res = parser.parseIL(inputFilePath);
	if(res < 0)
	{
		cout << "ILParser Error: " << res << endl;
		cout << parser.getErrorStr() << endl;
		return res;
	}

	//????????????
	//ILSchedule* sche = parser.allFunctions[0]->schedule;
	//StmtIf* p = &(*((StmtIf*)(*((ILCalculate*)sche->scheduleNodes[0])).statements.childStatements[54]));
	//Statement* s = p->statements[21];
	//vector<Statement*>* stmtList = &p->childStatements;
	//p->childStatements.insert(p->childStatements.begin() + 21, s->clone(p));

	//???????Calculate???
	//ILSchedule* sche = parser.allFunctions[1]->schedule;
	//ILBranch* p = (ILBranch*)sche->scheduleNodes[0];
	//ILCalculate* c = (ILCalculate*)(p->scheduleNodes[3]);
	//sche->scheduleNodes.push_back(c->clone(sche));
	

    ILTranslatorForC iLTranslator;
		
	res = iLTranslator.translate(&parser, outputDir);
	if(res < 0)
	{
		cout << "ILTranslatorForC Error: " << res << endl;
		cout << ILTranslator::getErrorStr() << endl;
		return res;
	}
    std::cout << "Done!\n";

    return 1;
}

int translateForCBMC(ArgumentParser& argumentParser, string inputFilePath, string outputDir)
{
	string configFilePath = "";
	string paraFullEnable = "false";
	string paraUnreachable = "false";
	string paraTautology = "false";
    string paraBranchCover = "false";
    string paraGlobalVariableReset = "false";
	string paraRangeConstraint = "false";
	string generateComment = "true";
	string generateMultiEntries = "false";
	string paraCutOffArray = "0";
    string paraMultiStep = "1";
    string paraGenSolvingPara = "true";
    
    string helpStr = prepareHelpStr();

	argumentParser.setHelpStr(helpStr);

	inputFilePath =		        argumentParser.getFlagValue("-i", inputFilePath);
	outputDir =			        argumentParser.getFlagValue("-o", outputDir);
    configFilePath =            argumentParser.getFlagValue("-config", configFilePath);
	paraFullEnable =            stringToLower(argumentParser.getFlagValue("-f", paraFullEnable));
	paraUnreachable =           stringToLower(argumentParser.getFlagValue("-u", paraUnreachable));
	paraTautology =             stringToLower(argumentParser.getFlagValue("-t", paraTautology));
	paraBranchCover =           stringToLower(argumentParser.getFlagValue("-b", paraBranchCover));
	paraGlobalVariableReset =   stringToLower(argumentParser.getFlagValue("-g", paraGlobalVariableReset));
	paraRangeConstraint =       stringToLower(argumentParser.getFlagValue("-r", paraRangeConstraint));
	generateComment =	        stringToLower(argumentParser.getFlagValue("-c", generateComment));
	generateMultiEntries =	    stringToLower(argumentParser.getFlagValue("-m", generateMultiEntries));
    paraCutOffArray =           argumentParser.getFlagValue("-ca", paraCutOffArray);
    paraMultiStep =             argumentParser.getFlagValue("-ms", paraMultiStep);
    paraGenSolvingPara =        argumentParser.getFlagValue("-gsp", paraGenSolvingPara);

	ILTranslatorForCBMC::setConfigUnreachable((paraUnreachable == "true") || (paraFullEnable == "true"));
	ILTranslatorForCBMC::setConfigTautology((paraTautology == "true") || (paraFullEnable == "true"));
	ILTranslatorForCBMC::setConfigBranchCover(paraBranchCover == "true");
	ILTranslatorForCBMC::setConfigRangeConstraint(paraRangeConstraint == "true");
	ILTranslatorForCBMC::setConfigGlobalVariableReset(paraGlobalVariableReset == "true");
	ILTranslatorForCBMC::setConfigGenerateComment(generateComment == "true");
	ILTranslatorForCBMC::setConfigGenerateMultiEntries(generateMultiEntries == "true");
    ILTranslatorForCBMC::setConfigCutOffArray(stringToInt(paraCutOffArray));
    ILTranslatorForCBMC::setConfigMultiStep(stringToInt(paraMultiStep));
    ILTranslatorForCBMC::setConfigGenSolvingPara(paraGenSolvingPara == "true");

    ILTranslator::setConfig(argumentParser);

	cout << argumentParser.flagToString();
    

    ILParser parser;
	int res = parser.parseIL(inputFilePath);
	if(res < 0)
	{
		cout << "ILParser Error: " << res << endl;
		cout << parser.getErrorStr() << endl;
		return res;
	}
    
	TCGConfig config;
	TCGConfigParser configParser(&config, &parser);
	res = configParser.parseConfig(configFilePath);
	if (res < 0)
	{
		cout << "TCGConfigParser Error: " << res << endl;
		return res;
	}

    ILPreprocessor preprocessor;
    preprocessor.setConfigInstrumentDecision(ILTranslator::configInstrumentDecision);
    preprocessor.setConfigInstrumentCondition(ILTranslator::configInstrumentCondition);
    preprocessor.setConfigInstrumentMCDC(ILTranslator::configInstrumentMCDC);
    preprocessor.setConfigCompleteElse(ILTranslator::configCompleteElse);
    preprocessor.preprocess(&parser);
    
    ILTranslatorForCBMC iLTranslator(&parser, &config);
	if((res = iLTranslator.translate(outputDir)) < 0) {
		cout << "ILTranslatorForCBMC Error: " << res << endl<< ILTranslator::getErrorStr() << endl;
		return res;
	}
	std::cout << "Done!\n";
	return 1;
}

int translateForTCGStateSearch(ArgumentParser& argumentParser, string inputFilePath, string outputDir)
{
	string configFilePath = "";
	string generateComment = "true";
	string paraCutOffArray = "0";
    string paraBranchDistanceGuidance = "true";
    string paraArrayDistanceGuidance = "true";
    
    
	inputFilePath =		    argumentParser.getFlagValue("-i", inputFilePath);
	outputDir =			    argumentParser.getFlagValue("-o", outputDir);
	configFilePath =	    argumentParser.getFlagValue("-config", configFilePath);
	generateComment =	    stringToLower(argumentParser.getFlagValue("-c", generateComment));
    paraCutOffArray =       argumentParser.getFlagValue("-ca", paraCutOffArray);
    paraBranchDistanceGuidance = stringToLower(argumentParser.getFlagValue("-bdg", paraBranchDistanceGuidance));
    paraArrayDistanceGuidance = stringToLower(argumentParser.getFlagValue("-adg", paraArrayDistanceGuidance));
    
    
    ILTranslatorForTCGStateSearch::setConfigGenerateComment(generateComment == "true");
    ILTranslatorForTCGStateSearch::setConfigCutOffArray(stringToInt(paraCutOffArray));
    ILTranslatorForTCGStateSearch::setConfigBranchDistanceGuidance(paraBranchDistanceGuidance == "true");
    ILTranslatorForTCGStateSearch::setConfigArrayDistanceGuidance(paraArrayDistanceGuidance == "true");

    ILTranslator::setConfig(argumentParser);

    cout << argumentParser.flagToString();



    ILParser parser;
	int res = parser.parseIL(inputFilePath);
	if(res < 0)
	{
		cout << "ILParser Error: " << res << endl;
		cout << parser.getErrorStr() << endl;
		return res;
	}

    TCGConfig config;
    TCGConfigParser configParser(&config, &parser);
    res = configParser.parseConfig(configFilePath);
    if(res < 0)
	{
		cout << "TCGConfigParser Error: " << res << endl;
		return res;
	}
    
    
    ILPreprocessor preprocessor;
    preprocessor.setConfigInstrumentDecision(ILTranslator::configInstrumentDecision);
    preprocessor.setConfigInstrumentCondition(ILTranslator::configInstrumentCondition);
    preprocessor.setConfigInstrumentMCDC(ILTranslator::configInstrumentMCDC);
    preprocessor.setConfigCompleteElse(ILTranslator::configCompleteElse);
    //preprocessor.setConfigAddSwitchMCDC(true);
    preprocessor.preprocess(&parser);

    ILTranslatorForTCGStateSearch iLTranslator(&parser, &config);
	res = iLTranslator.translate(outputDir);
	iLTranslator.release();
	if(res < 0)
	{
		cout << "ILTranslatorForTCGStateSearch Error: " << res << endl;
		cout << ILTranslator::getErrorStr() << endl;
		return res;
	}
    std::cout << "Done!\n";

    return 1;
}

int translateForLibFuzzer(ArgumentParser& argumentParser, string inputFilePath, string outputDir)
{
    
	string configFilePath = "";
	string generateComment = "true";
	string paraCutOffArray = "0";
    string paraRangeConstraint = "false";
    string paraTargetModelPara = "false";
    string paraGenTestCaseConverter = "true";
    string paraMaxIteratorCount = "-1";
    
	inputFilePath =		        argumentParser.getFlagValue("-i", inputFilePath);
	outputDir =			        argumentParser.getFlagValue("-o", outputDir);
	configFilePath =	        argumentParser.getFlagValue("-config", configFilePath);
	generateComment =	        stringToLower(argumentParser.getFlagValue("-c", generateComment));
    paraCutOffArray =           argumentParser.getFlagValue("-ca", paraCutOffArray);
    paraRangeConstraint =       stringToLower(argumentParser.getFlagValue("-r", paraRangeConstraint));
    paraTargetModelPara =       stringToLower(argumentParser.getFlagValue("-tp", paraTargetModelPara));
    paraGenTestCaseConverter =  stringToLower(argumentParser.getFlagValue("-gtcc", paraGenTestCaseConverter));
    paraMaxIteratorCount =      argumentParser.getFlagValue("-mic", paraMaxIteratorCount);
    
    ILTranslatorForLibFuzzer::setConfigGenerateComment(generateComment == "true");
    ILTranslatorForLibFuzzer::setConfigCutOffArray(stringToInt(paraCutOffArray));
    ILTranslatorForLibFuzzer::setConfigRangeConstraint(paraRangeConstraint == "true");
    ILTranslatorForLibFuzzer::setConfigTargetModelPara(paraTargetModelPara == "true");
    ILTranslatorForLibFuzzer::setConfigGenTestCaseConverter(paraGenTestCaseConverter == "true");
    ILTranslatorForLibFuzzer::setConfigMaxIteratorCount(stringToInt(paraMaxIteratorCount));

    ILTranslator::setConfig(argumentParser);

    cout << argumentParser.flagToString();
    


    ILParser parser;
	int res = parser.parseIL(inputFilePath);
	if(res < 0)
	{
		cout << "ILParser Error: " << res << endl;
		cout << parser.getErrorStr() << endl;
		return res;
	}

    TCGConfig config;
    TCGConfigParser configParser(&config, &parser);
    res = configParser.parseConfig(configFilePath);
    if(res < 0)
	{
		cout << "TCGConfigParser Error: " << res << endl;
		return res;
	}
    
    ILPreprocessor preprocessor;
    preprocessor.setConfigInstrumentDecision(ILTranslator::configInstrumentDecision);
    preprocessor.setConfigInstrumentCondition(ILTranslator::configInstrumentCondition);
    preprocessor.setConfigInstrumentMCDC(ILTranslator::configInstrumentMCDC);
    preprocessor.setConfigCompleteElse(ILTranslator::configCompleteElse);
    preprocessor.preprocess(&parser);

    ILTranslatorForLibFuzzer iLTranslator(&parser, &config);
	res = iLTranslator.translate(outputDir);
	if(res < 0)
	{
		cout << "ILTranslatorForLibFuzzer Error: " << res << endl;
		cout << ILTranslator::getErrorStr() << endl;
		return res;
	}
    std::cout << "Done!\n";

    return 1;
}

int translateForTCGHybrid(ArgumentParser& argumentParser, string inputFilePath, string outputDir)
{
    
	string configFilePath = "";
	string generateComment = "true";
	string paraCutOffArray = "0";
    
    
	inputFilePath =		    argumentParser.getFlagValue("-i", inputFilePath);
	outputDir =			    argumentParser.getFlagValue("-o", outputDir);
	configFilePath =	    argumentParser.getFlagValue("-config", configFilePath);
	generateComment =	    stringToLower(argumentParser.getFlagValue("-c", generateComment));
    paraCutOffArray =       argumentParser.getFlagValue("-ca", paraCutOffArray);
    
    
    ILTranslatorForTCGHybrid::setConfigGenerateComment(generateComment == "true");
    ILTranslatorForTCGHybrid::setConfigCutOffArray(stringToInt(paraCutOffArray));

    ILTranslator::setConfig(argumentParser);

    cout << argumentParser.flagToString();
    


    ILParser parser;
	int res = parser.parseIL(inputFilePath);
	if(res < 0)
	{
		cout << "ILParser Error: " << res << endl;
		cout << parser.getErrorStr() << endl;
		return res;
	}

    TCGConfig config;
    TCGConfigParser configParser(&config, &parser);
    res = configParser.parseConfig(configFilePath);
    if(res < 0)
	{
		cout << "TCGConfigParser Error: " << res << endl;
		return res;
	}

    
    ILPreprocessor preprocessor;
    preprocessor.setConfigInstrumentDecision(ILTranslator::configInstrumentDecision);
    preprocessor.setConfigInstrumentCondition(ILTranslator::configInstrumentCondition);
    preprocessor.setConfigInstrumentMCDC(ILTranslator::configInstrumentMCDC);
    preprocessor.setConfigCompleteElse(ILTranslator::configCompleteElse);
    //preprocessor.setConfigAddSwitchMCDC(true);
    preprocessor.preprocess(&parser);

    ILTranslatorForTCGHybrid iLTranslator(&parser, &config);
	res = iLTranslator.translate(outputDir);
	iLTranslator.release();
	if(res < 0)
	{
		cout << "ILTranslatorForTCGHybrid Error: " << res << endl;
		cout << ILTranslator::getErrorStr() << endl;
		return res;
	}

	vector<int> sliceIndices;
	const string sliceBm = stringTrim(argumentParser.getFlagValue("-slice_bm", ""));
	const string sliceRequest = stringTrim(argumentParser.getFlagValue("-slice_request", ""));
	if (!sliceBm.empty()) {
		if (!ILObligationSlicer::parseBitmapIndexListArg(sliceBm, sliceIndices))
			cout << "ILObligationSlicer: -slice_bm produced no indices\n";
	} else {
		string reqPath = sliceRequest;
		if (reqPath.empty())
			reqPath = outputDir + "/SliceOut/hybrid_coverage_stall_slice_request.json";
		if (!ILObligationSlicer::parseHybridSliceRequestFile(reqPath, sliceIndices) && !sliceRequest.empty())
			cout << "ILObligationSlicer: could not read indices from -slice_request: " << reqPath << endl;
	}
	if (!sliceIndices.empty()) {
		const string oblPath = outputDir + "/obligation_map.json";
		const string brPath = outputDir + "/branch_relation.json";
		if (!ILObligationSlicer::emitSlicesForBitmapIndices(&parser, sliceIndices, oblPath, brPath, outputDir))
			cout << "ILObligationSlicer: emitSlicesForBitmapIndices failed\n";
		else
			cout << "ILObligationSlicer: wrote slices under " << outputDir << "/Slices\n";
	}

    std::cout << "Done!\n";

    return 1;
}

int translateForCoverage(ArgumentParser& argumentParser, string inputFilePath, string outputDir)
{
	string generateComment = "true";

	
	
	inputFilePath = argumentParser.getFlagValue("-i", inputFilePath);
	outputDir = argumentParser.getFlagValue("-o", outputDir);
	generateComment = stringToLower(argumentParser.getFlagValue("-c", generateComment));


	ILTranslatorForCoverage::setConfigGenerateComment(generateComment == "true");

    ILTranslator::setConfig(argumentParser);

    cout << argumentParser.flagToString();



	ILParser parser;
	int res = parser.parseIL(inputFilePath);
	if (res < 0)
	{
		cout << "ILParser Error: " << res << endl;
		cout << parser.getErrorStr() << endl;
		return res;
	}


	ILPreprocessor preprocessor;
	preprocessor.setConfigInstrumentDecision(ILTranslator::configInstrumentDecision);
	preprocessor.setConfigInstrumentCondition(ILTranslator::configInstrumentCondition);
    preprocessor.setConfigInstrumentMCDC(ILTranslator::configInstrumentMCDC);
    preprocessor.setConfigCompleteElse(ILTranslator::configCompleteElse);
    preprocessor.setConfigAddSwitchMCDC(true);
	preprocessor.preprocess(&parser);

	//if (inputTestCaseFilePath.empty())
	//{
	//	cout << "ILTranslatorForCoverage Error: " << ILTranslator::ErrorNotProvideTestCase << endl;
	//	ILTranslator::setCurrentError(ILTranslator::ErrorNotProvideTestCase, "");
	//	cout << ILTranslator::getErrorStr() << endl;
	//	return res;
	//}

	ILTranslatorForCoverage iLTranslator(&parser);
	res = iLTranslator.translate(outputDir);
	iLTranslator.release();
	if (res < 0)
	{
		cout << "ILTranslatorForCoverage Error: " << res << endl;
		cout << ILTranslator::getErrorStr() << endl;
		return res;
	}
	std::cout << "Done!\n";

	return 1;
}

int translateForSimulator(ArgumentParser& argumentParser, string inputFilePath, string outputDir)
{
	string configFilePath = "";
	string generateComment = "true";
	string generateTest = "false";
	string packageParameter = "false";
	string packageVariable = "false";


	inputFilePath = argumentParser.getFlagValue("-i", inputFilePath);
	outputDir = argumentParser.getFlagValue("-o", outputDir);
    configFilePath = argumentParser.getFlagValue("-config", configFilePath);
	generateComment = stringToLower(argumentParser.getFlagValue("-c", generateComment));
	generateTest = stringToLower(argumentParser.getFlagValue("-t", generateTest));
	packageParameter = argumentParser.getFlagValue("-p", packageParameter);
    packageVariable = argumentParser.getFlagValue("-v", packageVariable);


    ILTranslator::setConfig(argumentParser);

	cout << argumentParser.flagToString();

	ILTranslatorForSimulator::setConfigGenerateComment(generateComment == "true");
	ILTranslatorForSimulator::setConfigTest(generateTest == "true");
	ILTranslatorForSimulator::setConfigPackageParameter(packageParameter == "true");
	ILTranslatorForSimulator::setConfigPackageVariable(packageVariable == "true");


	ILParser parser;
	int res = parser.parseIL(inputFilePath);
	if (res < 0)
	{
		cout << "ILParser Error: " << res << endl;
		cout << parser.getErrorStr() << endl;
		return res;
	}

	SimulatorConfig config;
	SimulatorConfigParser configParser(&config, &parser);
	res = configParser.parseConfig(configFilePath);
	if (res < 0)
	{
		cout << "SimulatorConfigParser Error: " << res << endl;
		return res;
	}

	ILTranslatorForSimulator iLTranslator(&parser, &config);
	res = iLTranslator.translate(outputDir);
    iLTranslator.release();

	if (res < 0)
	{
		cout << "ILTranslatorForSimulator Error: " << res << endl;
		cout << ILTranslator::getErrorStr() << endl;
		return res;
	}
	std::cout << "Done!\n";

	return 1;
}


int main(int argc, char** argv)
{
    string helpStr = prepareHelpStr();
    ArgumentParser argumentParser;
	// ../../test/subsystem_MIR_IL.xml
	// D:\\BehATG1_IR_Formatted.xml
    string inputFilePath =	 "../../test/Simpson_MIR_IL_Test.xml";
    string outputDir =		 "../../test/CodeOutput/temp";
    
    argumentParser.setDefaultValue("-i", inputFilePath);
    argumentParser.setDefaultValue("-o", outputDir);

    argumentParser.setHelpStr(helpStr);
    string language = "c";
	argumentParser.setDefaultValue("-l", language);
    if(argumentParser.parseArguments(argc, argv) == 0)
		return 0;
    language = argumentParser.getFlagValue("-l");
    inputFilePath = argumentParser.getFlagValue("-i");
    outputDir = argumentParser.getFlagValue("-o");

    int res = 0;
    
	cout << "-l: " << language << endl;
    if(language == "c")
    {
	    res = translateForC(argumentParser, inputFilePath, outputDir);
    }
	else if(language == "cbmc")
    {
        res = translateForCBMC(argumentParser, inputFilePath, outputDir);
    }
    else if(language == "tcg_state_search")
    {
        res = translateForTCGStateSearch(argumentParser, inputFilePath, outputDir);
    }
    else if(language == "libfuzzer")
    {
        res = translateForLibFuzzer(argumentParser, inputFilePath, outputDir);
    }
    else if(language == "tcg_hybrid" || language == "obligation_slice")
    {
        res = translateForTCGHybrid(argumentParser, inputFilePath, outputDir);
    }
	else if (language == "coverage")
	{
		//string inputTestCaseFilePath;
		//inputTestCaseFilePath= argumentParser.getFlagValue("-it");
		res = translateForCoverage(argumentParser, inputFilePath, outputDir);
	}
	else if (language == "simulator")
	{
		res = translateForSimulator(argumentParser, inputFilePath, outputDir);
	}
	return res;
}
