#pragma once


class ILTypeDefine;
class ILTransFile;
class ILTransTypeDefine
{
public:
    int translate(ILTypeDefine* typeDefine, ILTransFile* file) const;
};
