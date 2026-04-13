#pragma once


class ILStructDefine;
class ILTransFile;
class ILTransStructDefine
{
public:
    int translate(ILStructDefine* structDefine, ILTransFile* file) const;
	int translateDeclaration(ILStructDefine* structDefine, ILTransFile* file) const;
};
