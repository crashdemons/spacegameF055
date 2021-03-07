/**
@file objects_NPCShip.h
@brief 

...
@author crashenator (crashenator -at- gmail.com)
*/

#pragma once
#include "objects_Ship.h"
#include "objects_PlayerShip.h"

class NPCShip : public Ship/// NPC-logic controlled Ship.
{
    public:
        bool automatic_targetting;
        int object_target;//Objects[] index of the target ship
        int object_target_view;
        bool attack_criminals;//consider implementing  REPUTATION_VALUE minAttackReputation   (where the equivalent is <=CRIMINAL)
        //Faction* rep;
        void doStep(int index);
        void hitNotify(int index, int object_owner, int type);
        void init(Faction* fac);
        NPCShip();
        NPCShip(Faction* fac);
        ~NPCShip();
};

