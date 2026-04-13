#pragma once

class ILInput;
class ILTransFile;
class ILTransInput
{
public:
    int translate(const ILInput* input, ILTransFile* file) const;
};
