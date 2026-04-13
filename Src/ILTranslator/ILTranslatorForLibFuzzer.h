#pragma once

#include <string>
#include <vector>
#include <map>


class ILRef;
class ILParser;
class TCGConfig;

#define INT8_MIN_VALUE        (-127 - 1)
#define INT16_MIN_VALUE       (-32767 - 1)
#define INT32_MIN_VALUE       (-2147483647 - 1)
#define INT64_MIN_VALUE       (-9223372036854775807 - 1)
#define INT8_MAX_VALUE         127
#define INT16_MAX_VALUE        32767
#define INT32_MAX_VALUE        2147483647
#define INT64_MAX_VALUE        9223372036854775807
#define UINT8_MAX_VALUE        0xffu
#define UINT16_MAX_VALUE       0xffff
#define UINT32_MAX_VALUE       0xffffffff
#define UINT64_MAX_VALUE       0xffffffffffffffff
#define DBL_MAX_VALUE          1.7976931348623158e+308 // max value
#define DBL_MAX_VALUE_10_EXP   308                     // max decimal exponent
#define DBL_MAX_VALUE_EXP      1024                    // max binary exponent
#define DBL_MIN_VALUE         2.2250738585072014e-308 // min positive value
#define DBL_TRUE_MIN_VALUE    4.9406564584124654e-324 // min positive value
#define FLT_MAX_VALUE          3.402823466e+38F        // max value
#define FLT_MIN_VALUE         1.175494351e-38F        // min normalized positive value
#define LDBL_MAX_VALUE         DBL_MAX_VALUE                 // max value
#define LDBL_MAX_VALUE_10_EXP  DBL_MAX_VALUE_10_EXP          // max decimal exponent
#define LDBL_MAX_VALUE_EXP     DBL_MAX_VALUE_EXP             // max binary exponent
#define LDBL_MIN_VALUE        DBL_MIN_VALUE                // min normalized positive value

class ILTranslatorForLibFuzzer
{
public:
	
	static ILParser* iLParser;
    static TCGConfig* tcgConfig;

    ILTranslatorForLibFuzzer(ILParser* iLParser, TCGConfig* tcgConfig);

	int translate(std::string outputDir);

    
    static void setConfigGenerateComment(bool value);
    static bool configGenerateComment;
    
    static void setConfigCutOffArray(int value);
	static int configCutOffArray;

    static void setConfigRangeConstraint(bool value);
    static bool configRangeConstraint;

    static void setConfigTargetModelPara(bool value);
    static bool configTargetModelPara;

    static void setConfigGenTestCaseConverter(bool value);
    static bool configGenTestCaseConverter;

    static void setConfigMaxIteratorCount(int value);
    static int configMaxIteratorCount;
    
	std::map<std::string, std::string> fileCodes;
    
    static std::string getParameterTypeMin(std::string type, std::string min);
    static std::string getParameterTypeMax(std::string type, std::string max);

private:
    std::string getRedefineOutputCode() const;

    int generateOneKeyBuildScript(std::string outputDir) const;

};
