/**
@file ObjectContainer.h
@brief Provides multiple-ingame-object control, filtering, and interaction

...
@author crashenator (crashenator -at- gmail.com)
*/

#pragma once
#include "drawing.h"
#include "input.h"
#include "factions.h"
#include "object_sets.h"

#define OBJECTS_MAX 512//256
#define OBJECTS_LAST OBJECTS_MAX-1
#define OBJECT_TYPES 8
enum OBJECT_TYPE
{
    OT_NONE,
    OT_SCEN,
    OT_EXPL,
    OT_PHYS,
    OT_PROJ,
    OT_SHIP,
    OT_PSHP,
    OT_NSHP
};
enum OBJECT_FLAG
{
	OF_NONE=0,
	OF_NODELETE=1
};
enum OBJECT_DEPTH
{
	OD_TOP=0,
	OD_DEFAULT=3,
	OD_BOTTOM=6,
	OD_NONE=255
};
#define OD_LAST OD_TOP
#define OD_FIRST OD_BOTTOM


const char* const  object_typenames[OBJECT_TYPES]=
{
	"Nothing",
	"Decoration",
	"Decoration",
	"Some object",
	"Projectile",
	"Ship",
	"Your Ship",
	"Ship"
};



class ObjectFilter
{
    public:
    bool a,n;
    bool omatch[OBJECT_TYPES];
    bool getMatch(int type);
    void setMatch(int type,bool match=true,bool all=false,bool none=false);
    void setMatchOnly(int type,bool match=true);
    ObjectFilter();
};

class ObjectContainer
{
	private:
        int next;//used to select the next likely empty index.
		int depthmin;
		int depthmax;
    public:
        Drawing_Info* di;
        void* in;//Input_Info
		void* si;//Sound_Info

		int selectedObject;
        

		Gradient grShields;


    	Projectile_Type Projectile_Plasma;
        Projectile_Type Projectile_PlasmaBomb;
        Projectile_Type Projectile_Missile;
        Projectile_Type Projectile_Mine;
        Projectile_Type Projectile_MiningLaser;
        Projectile_Type Projectile_Asteroid;
        Projectile_Type Projectile_Scraps;
        Projectile_Type Projectile_TractorBeam;


        Ship_Type Ship_Shuttle;
        Ship_Type Ship_Cargo;
        Ship_Type Ship_Fighter;
        Ship_Type Ship_Heavy;
        Ship_Type Ship_Cruiser;
        Ship_Type Ship_Battle;
        
        ObjectContainer();
        ~ObjectContainer();

        void* objects[OBJECTS_MAX];
        int types[OBJECTS_MAX];
		unsigned char depth[OBJECTS_MAX];
		OBJECT_FLAG flags[OBJECTS_MAX];


		int count;
		int selectFromCursor(void* pCursor);
		string getSelectedInfo();

        void release(int i);
        void release(void* p,int type=OT_NONE);
        void step(int i);
        void step();
        void stepType(int type);
        int add(void* so, unsigned char depthv=OD_DEFAULT, OBJECT_FLAG flagsv=OF_NONE);
		void setDepth(int i, unsigned char depthv=OD_DEFAULT);
		void setFlag(int i, OBJECT_FLAG of, bool enable=true);
		bool checkFlag(int i, OBJECT_FLAG of);

        int getOwner(int i);
		int getFactionView(int i, FACTION_TYPE fact=FNONE);
        int getView(int i, int j);
        int getOwnerView(int i, int j);
		vector<int> getFiltered(ObjectFilter* of,ReputationFilter* rf, int max_results=-1);///retrieves objects matching an Object and Reputation Filter (relative to Faction None)
		vector<int> ObjectContainer::getRelativeFiltered(int j,ObjectFilter* of,ReputationFilter* rf,Range* rn,bool excludej,int max_results=-1);///retrieves objects matching an Object and Reputation Filter (relative to a viewer object#)
        vector<pair<int,float>> ObjectContainer::getRelativeFilteredEx(int j,ObjectFilter* of,ReputationFilter* rf,Range* rn,bool excludej,int max_results=-1);///retrieves objects matching an Object and Reputation Filter (relative to a viewer object#)
		int getClosestFiltered(int j,ObjectFilter* of,ReputationFilter* rf,Range* rn,bool excludej=true);///retrieves the closest object matching an Object and Reputation Filter (relative to a viewer object#)
        int findempty(int i);
        void remove(int i,bool free);
        void clear(bool free);
        //int count();
		int countScreen();
        void draw(Drawing_Info* di);
		void draw_debug(Drawing_Info* di,int debug_mode);
        void unloadimages(Drawing_Info* di);
        void reloadimages(Drawing_Info* di);
};
 void clearContainerInfo(void* so);