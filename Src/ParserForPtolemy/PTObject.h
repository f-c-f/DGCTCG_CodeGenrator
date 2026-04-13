#pragma once
#include <string>

class PTObject {
public:
    PTObject* parent;

	enum ObjType{
		TypeEntity = 1,
		TypeActor,
		TypeInport,
		TypeOutport,
		TypeRelation,
		TypeLink,
		TypeProperty,
	};

	ObjType objType;


	std::map<std::string, std::string> parameters;
    
	std::vector<PTProperty*> propertys;

	virtual void release(){};
};
