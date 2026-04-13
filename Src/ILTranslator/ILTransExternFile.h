#pragma once
#include <map>
#include <set>
#include <string>
#include <vector>

class ILExternFile;
class ILTransFile;
class ILTransExternFile
{
public:
    int translate(const ILExternFile* externFile, std::string& code) const;

private:
    static std::set<std::string> externFileSet;
};
