#pragma once

#include <string>
#include <vector>
#include <map>




class CGTActorExeTransObject
{
public:
	CGTActorExeTransObject* parent = nullptr;

	enum ObjType {
		TypeRoot = 1,
		TypeModelInfo,
		TypeFunctionInfo,
		TypeActorInfo,
		TypeInport,
		TypeOutport,
		TypeActionPort,
		TypeSourceOutportInfo,
		TypeSourceOutport,
		TypeParameter,
	};

	ObjType objType;

	virtual void release() {};
};
