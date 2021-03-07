#include "objects_NPCShip.h"



void NPCShip::doStep(int index)
{
    /* DONE (crash#1#): NPCShip::doStep - prioritize ships over projectiles, 
                        or don't allow turning to handle projectiles */
    if(automatic_targetting)
    {
        bool target=true;
        bool follow=false;
        
        Range rn;
        rn.set(0,200,true,false);
        ReputationFilter rf;
        ObjectFilter of;
        rf.setMatchOnly(ENEMY);
        of.setMatchOnly(OT_SHIP);
        of.setMatch(OT_PSHP);
        of.setMatch(OT_NSHP);
        int closest_ship=container->getClosestFiltered(index,&of,&rf,&rn);
        rn.set(0,200,false,false);
        of.setMatchOnly(OT_PROJ);
        int closest_projectile=container->getClosestFiltered(index,&of,&rf,&rn);
        
        if(closest_ship==-1)
        {
            object_target=closest_ship;
            if(closest_projectile!=-1) object_target=closest_projectile;
        }else object_target=closest_ship;
        
        
        if(target) if(object_target!=-1) if(container->objects[object_target]!=NULL)
        {
            if(container->types[object_target]==OT_PSHP || container->types[object_target]==OT_NSHP) follow=true;
			if(FollowStep((SceneryObject*)container->objects[object_target],follow)){
				if(stats.energy>5){
                    if(istep%rof==0){//
						stats.energy.decrement(5);
                        Projectile* pj=new Projectile();
                        pj->container=container;
                        pj->setProjectile(container->Projectile_Plasma);
                        pj->object_owner=index;
                        FireProjectile(pj, 5,-1,0,0);
                        container->add(pj);
                    }
                }
			}
			if(follow) hasTrail=true;
        }
        istep++;
    }
    Ship::doStep(index);
}
void NPCShip::hitNotify(int index, int object_owner, int type)
{
    if(object_owner<=-1 || container->objects[object_owner]==NULL) return;
    SceneryObject* so=(SceneryObject*) container->objects[object_owner];
    if(type==-1) type=so->type;
    if(type==OT_PSHP || type==OT_NSHP){
        Ship* ship2=(Ship*) so;
		ship2->reputation.setReputationWith( reputation.getFaction(), ENEMY );
    }
}
void NPCShip::init(Faction* fac)
{
	//rep=new Faction(FNONE);
    type=OT_NSHP;
    if(fac!=NULL) reputation=*fac;
    object_target=-1;
    object_target_view=NEUTRAL;
    automatic_targetting=true;
    attack_criminals=false;//easier for trade ships.
}
NPCShip::NPCShip(){init(NULL);}
NPCShip::NPCShip(Faction* fac){init(fac);}
NPCShip::~NPCShip(){/*delete rep;*/}
