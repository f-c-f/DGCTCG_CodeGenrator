#ifndef _TESTCASE_H_
#define _TESTCASE_H_

#include <string>
#include <vector>

typedef unsigned char	u8;

using namespace std;

class SimulatorTestCase
{
public:
	static string binaryTestCaseFilePath;
	static char* testCaseData;
	static int offset;

public:
	static int initTestCase();
	static u8* takeTestCase(int datatypesize);
	static void releaseTestCase();

};

#endif // _TESTCASE_H_