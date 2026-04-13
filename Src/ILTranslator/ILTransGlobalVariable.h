#pragma once
class ILTransFile;
class ILGlobalVariable;
class ILTransGlobalVariable
{
public:
    int translate(const ILGlobalVariable* globalVariable, ILTransFile* file) const;
};
