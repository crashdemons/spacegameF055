/**
@file objects_Projectile.h
@brief 

...
@author crashenator (crashenator -at- gmail.com)
*/

#pragma once
#include "objects_Physical.h"

enum CollisionType /// NOT IMPLEMENTED
{
	COLLISION_LINEAR,
	COLLISION_RADIUS,
	COLLISION_BOX
};

class Projectile : public PhysicalObject /// Physical objects that are fired, use interobject and view collision logic
{
    public:
		CollisionType collisionType;

        Projectile_Type* deathspawn;
        int deathspawn_n;
        float deathspawndist;
        float deathspawnveloc;

        bool targeted;
		int object_target;

        int object_lastcollision;
        int object_owner;//Objects[] index of the firing ship
        bool collisStops;
        bool hitnotify;
        Projectile();

        //following two functions only work for object_owner values less than -1
        void setUnownedView(int view);//hack for unowned projectile that always is always seen hostile
        int getUnownedView();
        
        void die();
        void setProjectile(Projectile_Type* pt);
		void setProjectile(Projectile_Type& pt);
        bool collideCheck(PhysicalObject* po);
        void collideStep(int i, int j, PhysicalObject* po);
		bool Projectile::FollowStep(SceneryObject* ship);
        void doStep(int index);
};

