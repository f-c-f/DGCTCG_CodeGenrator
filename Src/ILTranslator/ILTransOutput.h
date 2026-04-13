#pragma once

class ILOutput;
class ILTransFile;
class ILTransOutput
{
public:
    int translate(const ILOutput* output, ILTransFile* file) const;
};
