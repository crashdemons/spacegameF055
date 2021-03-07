/**
@file objects_Ship.h
@brief 

...
@author crashenator (crashenator -at- gmail.com)
*/

#pragma once
#include "objects_Physical.h"
#include "objects_Projectile.h"

class Ship : public PhysicalObject // physical objects that have propulsion and/or reputation.
{
    public:
		static void* sndFire;


        float fireerror;
        int rof;

		Faction reputation;//faction and repuation information.

		Projectile_Type* deathspawn;//for Debris


		virtual void die();


        void FireProjectile(SceneryObject* pj, float speed, float fe=-1, float d=0, float da=0);//fe=firing error amount;d=distance from shooter,da=rotation angle offset
        virtual void doStep(int index);
        Ship();
        //virtual ~Ship();
};
