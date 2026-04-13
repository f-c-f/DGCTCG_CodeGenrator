#pragma once

#include <string>
#include <vector>
#include <map>


class MIRObject
{
public:
	MIRObject* parent;

	enum ObjType{
		TypeModel = 1,
		TypeFunction,
		TypeActor,
		TypeRelation,
		TypeInport,
		TypeOutport,
		TypeActionPort,
		TypeParameter,
	};

	ObjType objType;

	std::map<std::string, std::string> parametersOfXML;

	virtual void release(){};
};
