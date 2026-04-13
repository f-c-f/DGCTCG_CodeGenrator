#pragma once

class ILRef;
class ILTransFile;
class ILTransRef
{
public:
    int translate(const ILRef* ref, ILTransFile* file) const;
};
