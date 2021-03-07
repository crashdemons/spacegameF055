/**
@file objects_Physical.h
@brief 

...
@author crashenator (crashenator -at- gmail.com)
*/

#pragma once
#include "objects_Scenery.h"
#include "objects_Explosion.h"
#include "Equipment.h"
#include "AI.h"



class PhysicalObject : public SceneryObject /// objects that physically interact with other objects.
{
    public:
		AI* ai;
		ObjectStats stats;
		EquipmentContainer equipment;

		int idxShield;
		SceneryObject soShield;//I know this looks bad, but timed effects didn't work.
		int curShieldMask;
		int newShieldMask;
		int maskAge;

        int collisDmg;//FLOAT?
        bool immune;
        bool solid;
        bool hasdied;
        float bumpforce;
        //float health,healthmax;
        //float shield_max;
        //float shield_rechargestep;
        bool killsProjectiles;
        bool deathexplode;
        float turnstep;

		float gravity;
		bool hasgravity;

		bool FollowStep(SceneryObject* ship,bool& move,bool canstop=true);//follow a ship and orbit it - a very aggressive technique
		bool FollowStep(float x, float y,bool& move);//follow a ship and orbit it - a very aggressive technique


		bool TurnGetDirection(float a, float* s);
        bool TurnGetDirection(SceneryObject* so);
        void TurnStepDirection(bool ccw, float step=0);
        void TurnStepToward(float a);
        void TurnStepToward(SceneryObject* so);

        virtual void die();
        void explode();
		void stepGravity(PhysicalObject* po);
        void bump(PhysicalObject* po);
        void damage(int d);
        void faceCoord(float x, float y);
        void faceObject(SceneryObject* so);
        bool collideRadius(SceneryObject* so);
        void virtual hitNotify(int index, int object_owner,int owner_type);
		void loadEffectShield();//loads the shield (returns if loaded)
		void setEffectShield(int mask);//positions and colors the shield
		void eventShieldColor(int mask);//changes the shield color variables 
        void virtual doStep(int index);
        PhysicalObject();
        virtual ~PhysicalObject();
};
