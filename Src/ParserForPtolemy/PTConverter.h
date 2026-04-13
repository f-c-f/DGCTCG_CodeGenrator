#pragma once

#include "ParserForPtolemy.h"

#include <vector>
#include <string>


#include "PTInport.h"
#include "PTOutport.h"
#include "PTProperty.h"
#include "PTRelation.h"
#include "PTLink.h"
#include "PTObject.h"
#include "PTEntity.h"
#include "PTActor.h"
#include "ParserForPtolemy.h"


#include "../MIRBasic/MIRObject.h"
#include "../MIRBasic/MIRModel.h"
#include "../MIRBasic/MIRFunction.h"
#include "../MIRBasic/MIRActor.h"
#include "../MIRBasic/MIRInport.h"
#include "../MIRBasic/MIRActionPort.h "
#include "../MIRBasic/MIROutport.h"
#include "../MIRBasic/MIRRelation.h"
#include "../MIRBasic/MIRFunctionDataflow.h"




class PTConverter
{
public:
	ParserForPtolemy* parserForPtolemy;
	MIRModel* mIRModel;

	enum {
		ErrorLineSrcDstStrInvalid = 80001,

	};

	int convert(ParserForPtolemy* parserForPtolemy, MIRModel* mIRModel);

private:
	int convert(MIRModel* mIRModel);
	int convertEntity(PTEntity* entity, MIRModel* mIRModel);
	int convertActor(PTActor* actor, MIRFunctionDataflow* mIRFunction);
	int convertLink(PTEntity* entity, MIRFunctionDataflow* mIRFunction);
	
	int convertEntityInport(PTInport* inport, MIRFunction* mIRFunction);
	int convertEntityOutport(PTOutport* outport, MIRFunction* mIRFunction);
	
	int convertActorInport(PTInport* inport, MIRActor* mIRActor);
	int convertActorOutport(PTOutport* outport, MIRActor* mIRActor);
	
	int convertParameter(PTObject* object, MIRObject* mIRObject);

	int convertLineSrcDstStr(std::string str, std::string &ret);

	int trimNameStr(std::string str, std::string &ret);
	int trimTypeStr(std::string str, std::string &ret);


    class MapParameter
    {
    public:
        std::string name;
        std::string value;
    };
    class MapInport
    {
    public:
        std::string name;
        std::string value;
    };
    class MapOutport
    {
    public:
        std::string name;
        std::string value;
    };
    class MapActor
    {
    public:
        std::string name;
        std::string value;
        std::vector<MapParameter> mapParameters;
        std::vector<MapInport> mapInports;
        std::vector<MapOutport> mapOutports;
    };

    std::vector<MapActor> mapActorList;
    MapActor* findMapActor(std::string name);
    MapParameter* findMapParameter(MapActor* mapActor, std::string name);
    MapInport* findMapInport(MapActor* mapActor, std::string name);
    MapOutport* findMapOutport(MapActor* mapActor, std::string name);

    int mapActors();
    int loadActorMapRule();

    int mapActorsTraverseFunction(MIRModel* model);
    int mapActorsTraverseActor(MIRFunctionDataflow* function);
    int mapActorsTraverseParameter(MIRActor* actor, MapActor* mapActor);
    int mapActorsTraverseInport(MIRActor* actor, MapActor* mapActor);
    int mapActorsTraverseOutport(MIRActor* actor, MapActor* mapActor);
    int mapActorsUpdateLinkPort(MIRFunctionDataflow* function);

    bool charIsWord(char c) const;
    int mapActorsTraverseActorHandleExpression(MIRActor* actor);


};
