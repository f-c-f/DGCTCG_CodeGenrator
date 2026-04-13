#pragma once

#include <string>
#include <vector>
#include <map>




class SLObject
{
public:
	SLObject* parent;

	enum ObjType{
		TypeSubsystem = 1,
		TypeBlock,
		TypeLine,
		TypeInport,
		TypeOutport,
		TypeActionPort,
	};

	ObjType objType;

	int sid;

	std::map<std::string, std::string> parameters;

	virtual void release(){};
};
