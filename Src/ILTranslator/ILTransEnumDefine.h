#pragma once


class ILEnumDefine;
class ILTransFile;
class ILTransEnumDefine
{
public:
    int translate(const ILEnumDefine* enumDefine, ILTransFile* file) const;
};
