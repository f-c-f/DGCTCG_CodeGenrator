#pragma once
#include <string>
#include <vector>
#include <map>

class TBProperty;

class TBObject {
public:
    TBObject* parent;

	enum ObjType{
		TypeModelEntirety,
		TypeProgramModelPage,
		TypeStateMachineModelPage,
		TypeStructModelPage,
		TypeGlobalVariableModelPage,

		TypeActor,
		TypeInport,
		TypeOutport,
		TypeProperty,
		TypeFunction,
		TypeReturnValue,
		TypeLinkNode,
		TypeTransitionNode,
		TypeRelation,
		TypeBranch,
		TypeState,
		TypeTransition,
		TypeLinkInterface,


		TypeTypeMember,
		TypeTypeDefine,

		TypeGlobalVariable,
	};

	ObjType objType;

	long long id;


	std::map<std::string, std::string> parameters;

	virtual void release(){};
};
