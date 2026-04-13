#pragma once

class ILLocalVariable;
class ILTransFile;
class ILTransLocalVariable
{
public:
    int translate(const ILLocalVariable* localVariable, ILTransFile* file) const;
};
