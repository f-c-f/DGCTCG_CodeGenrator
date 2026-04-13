#pragma once
#include <map>
#include <set>
#include <string>
#include <vector>

class ILUtilityFunction;
class ILTransFile;
class ILTransUtilityFunction
{
public:
    int translate(const ILUtilityFunction* utilityFunction, ILTransFile* file) const;
	static std::set<std::string> utilityFunctionSet;

private:
	static std::map<std::string, std::string> utilityFunctionContent;
    std::vector<std::string> getHeadFileList(std::string code) const;
    std::string getFunctionImplement(std::string code) const;
    std::string getFunctionDefine(std::string code) const;
};
