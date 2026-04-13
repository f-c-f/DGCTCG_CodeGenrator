#pragma once
class ILTransFile;
class ILExternVariable;
class ILTransExternVariable
{
public:
    int translate(const ILExternVariable* externVariable, ILTransFile* file) const;
};
