#pragma once

#include <vector>
#include <string>
#include <map>



#include "TBInport.h"
#include "TBOutport.h"
#include "TBProperty.h"
#include "TBRelation.h"
#include "TBLinkNode.h"
#include "TBObject.h"
#include "TBModelEntirety.h"
#include "TBProgramModelPage.h"
#include "TBActor.h"
#include "ParserForTBLink.h"


#include "../MIRBasic/MIRObject.h"
#include "../MIRBasic/MIRModel.h"
#include "../MIRBasic/MIRFunction.h"
#include "../MIRBasic/MIRActor.h"
#include "../MIRBasic/MIRInport.h"
#include "../MIRBasic/MIRActionPort.h"
#include "../MIRBasic/MIROutport.h"
#include "../MIRBasic/MIRParameter.h"
#include "../MIRBasic/MIRRelation.h"
#include "../MIRBasic/MIRFunctionDataflow.h"

using namespace std;


class TBConverter
{
public:
	ParserForTBLink* parserForTBLink;
	MIRModel* mIRModel;

	enum {
		ErrorLineSrcDstStrInvalid = 80001,

	};

	int convert(ParserForTBLink* parserForTBLink, MIRModel* mIRModel);

private:
	int convert(MIRModel* mIRModel);
	int convertProgramModelPage(TBProgramModelPage* programModelPage, MIRModel* mIRModel);
    int convertProgramModelPageProperty(TBProperty* property, MIRFunction* mIRFunction);
	int convertActor(TBActor* actor, MIRFunctionDataflow* mIRFunction);
	int convertRelation(TBProgramModelPage* programModelPage, MIRFunctionDataflow* mIRFunction);
	
	int convertActorInport(TBInport* inport, MIRActor* mIRActor);
	int convertActorOutport(TBOutport* outport, MIRActor* mIRActor);
    int convertActorOutport(TBBranch* branch, MIRActor* mIRActor);
    int convertActorProperty(TBProperty* property, MIRActor* mIRActor);
	
	int convertParameter(TBObject* object, MIRObject* mIRObject);

	int convertLineSrcDstStr(std::string str, std::string &ret);

	int trimNameStr(std::string str, std::string &ret);
	int trimTypeStr(std::string str, std::string &ret);

    map<long long, long long> linkInterfaceColor;
    void calculateLinkNodeRelatedAllLinkInterfaces(TBProgramModelPage* page);
    
    // 并查集相关函数
    long long findRoot(map<long long, long long>& parent, long long x);
    void unionSets(map<long long, long long>& parent, map<long long, long long>& rank, long long x, long long y);


    class MapParameter
    {
    public:
        std::string name;
        std::string value;
        bool isNew;
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
