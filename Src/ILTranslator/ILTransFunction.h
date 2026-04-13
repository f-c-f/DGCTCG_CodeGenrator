#pragma once
#include <map>
#include <string>
#include <vector>
#include "../ILBasic/ILFunction.h"
class ILOutput;
class ILInput;
class ILFunction;
class ILFile;
class ILTransFile;
class ILLocalVariable;
class ILCalculate;


class ILTransFunction
{
public:
    int translate(const ILFunction* function, ILTransFile* file) const;
private:
    std::string translateFunctionInput(const ILInput* input) const;
    std::string translateFunctionOutput(const ILOutput* output) const;
    std::string translateFunctionOutputAssign(const ILOutput* output) const;

    bool storePackageParameter(const ILFunction* function, ILTransFile* file) const;
    bool storePackageVariable(const ILFunction* function, ILTransFile* file) const;
    bool storePackageInput(const ILInput* input, ILStructDefine* structDefine) const;
    bool storePackageOutput(const ILOutput * input, ILStructDefine* structDefine) const;
    bool storePackageLocalVariable(const ILLocalVariable* variable, ILStructDefine* structDefine) const;

    static std::map<ILFunction::Type, std::string> functionTypeToStrMap;
};
