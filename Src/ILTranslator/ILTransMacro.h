#pragma once


class ILMacro;
class ILTransFile;
class ILTransMacro
{
public:
    int translate(const ILMacro* macro, ILTransFile* file) const;
};
