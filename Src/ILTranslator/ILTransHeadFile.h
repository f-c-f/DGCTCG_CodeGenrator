#pragma once
#include <map>
#include <set>
#include <string>
#include <vector>

class ILHeadFile;
class ILTransFile;
class ILTransHeadFile
{
public:
    int translate(const ILHeadFile* headFile, ILTransFile* file) const;
    //int translateForC(const ILHeadFile* headFile, ILTransFile* file) const;
    int translateForCBMC(const ILHeadFile* headFile, ILTransFile* file) const;
    //int translateForTCGStateSearch(const ILHeadFile* headFile, ILTransFile* file) const;

    int translateBasic(const ILHeadFile* headFile, ILTransFile* file) const;

	static std::set<std::string> headFileSet;

private:
	static std::map<std::string, std::string> headFileContent;

    std::vector<std::string> getHeadFileList(std::string code) const;
};
