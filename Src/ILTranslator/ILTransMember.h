#pragma once


class ILMember;
class ILTransFile;
class ILTransMember
{
public:
    int translate(const ILMember* member, ILTransFile* file) const;
};
