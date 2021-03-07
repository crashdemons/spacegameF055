#include "objects_Projectile.h"

Projectile::Projectile()
{
	delete ai;
	ai=(AI*)new AI_Ship(); // should still be cleaned up by ~PhysicalObject();
	ai->type=AI_TYPE_NONE;
	ai->setSequence(AI_SEQ_ATTACK);
	ai->target=-1;

    type=OT_PROJ;
    object_owner=-1;
    object_lastcollision=-1;
    collisStops=true;
    collisDmg=1;
    randmove=true;
    killsProjectiles=false;
    deathspawn=0;
    deathspawn_n=0;
    hitnotify=true;
	targeted=false;
	object_target=-1;
	turnstep=2*DEGRAD;
}
//following two functions only work for object_owner values less than -1
void Projectile::setUnownedView(int view){object_owner=-(view+2);}//hack for unowned projectile that always is always seen hostile
int Projectile::getUnownedView(){return -(object_owner+2);}

void Projectile::die(){//if a projectile dies, it can create multiple projectiles as a `deathspawn`
    PhysicalObject::die();
    Projectile* pj;
    for(int i=0;i<deathspawn_n;i++)
    {
        pj=new Projectile();
        pj->container=container;
        pj->setProjectile(deathspawn);
		pj->velocity[0]=velocity[0]/2;
		pj->velocity[1]=velocity[1]/2;
        ///*
        pj->ii->rotation=frandom(-PI2,PI2);
        pj->setCenterRel(this,deathspawndist);//distance from the projectile's center.
        pj->AccelerateRel(deathspawnveloc);
        pj->object_owner=object_owner;
        container->add(pj);
        //*/
    }
}
void Projectile::setProjectile(Projectile_Type* pt)
{
	if(pt->image!=NULL) LoadImg(container->di,pt->image,pt->transcolor);
    lifespan=pt->lifespan;
	spin=pt->spin;
	randmove=pt->randmove;
	bumpforce=pt->bumpforce;
	stats.health=pt->health;
	collisDmg=pt->collisDmg;
	killsProjectiles=pt->killsProjectiles;
	collisStops=pt->collisStops;
	canmove=pt->canmove;
	deathspawn=(Projectile_Type*)(pt->deathspawn);
	deathspawn_n=pt->deathspawn_n;
	deathspawndist=pt->deathspawndist;
	deathspawnveloc=pt->deathspawnveloc;
	deathexplode=pt->explode;
	hitnotify=pt->hitnotify;
	hasTrail=pt->hasTrail;
	hasGlow=pt->hasGlow;
	maskTrail=pt->maskTrail;
	maskGlow=pt->maskGlow;
	targeted=pt->targeted;
}
void Projectile::setProjectile(Projectile_Type& pt){setProjectile(&pt);}
bool Projectile::collideCheck(PhysicalObject* po)
{
    if(hasdied) return false;
    if(po->visible && !po->hasdied)
    {
        return collideRadius(po);
    }
    return false;
}
void Projectile::collideStep(int i, int j, PhysicalObject* po)
{
	object_lastcollision=j;
	if(hasgravity) stepGravity(po);
	if(po->hasgravity) po->stepGravity(this);
	if(po->solid) bump(po);
	
	damage( po->collisDmg);
	po->damage(collisDmg);

	if( !(po->solid) ) return;

    if(hitnotify) po->hitNotify(j,object_owner,-1);//*** types[object_owner]
    if(container->types[j]==OT_PROJ) ((Projectile*) po)->object_lastcollision=i;

	if(immune) return;
    if(collisStops || po->killsProjectiles) die();
}
bool Projectile::FollowStep(SceneryObject* ship)
{
	bool turn=true;
	int accelerate=0;


	float s,a,a1=getAngleTo(ship);
	

	float dist2,dist=distTo(ship);

	
	ship->VelocityStepRel(20);//planning ahead
	VelocityStepRel(20);

	a=getAngleTo(ship);
	dist2=distTo(ship);

	VelocityStepRel(-20);
	ship->VelocityStepRel(-20);//end planning ahead

	bool goingaway=dist2>dist;

	float targetVeloc=dist2/20;

	bool ccw=TurnGetDirection(a,&s);
	s=fabs(s);



    float sweepMovement=realAngle(movementAngle)-realAngle(a);
            if(sweepMovement<0) sweepMovement=-sweepMovement;
            if(sweepMovement>PI) sweepMovement=PI2 - sweepMovement;


	float vm=VelocAvg();

	if(dist<200){
		if(targetVeloc>vm || (s<( 5*DEGRAD) && sweepMovement>(90*DEGRAD)) ) accelerate=+1;//vm=missile velocity, s=sweep angle between projected coordinates and facing
		if(targetVeloc<vm || (s>(45*DEGRAD) && sweepMovement>(90*DEGRAD)) ) accelerate=-1;
		if(dist<100 && s<(5*DEGRAD) ) accelerate=+1;
	}else{
		if(s<(5*DEGRAD) ) accelerate=+1;
		else accelerate=-1;
	}

	


	if(sweepMovement>(45*DEGRAD) && vm>5.0) accelerate=-1;



	if(ccw) a=realAngle(a);
	else a=realAngle(a);
        
	if(turn) TurnStepToward(a); 
	if(accelerate==+1 && vm<5.0) AccelerateRel((float) -(sweepMovement/3.14));
	if(accelerate==-1) Deccelerate(0.001f,0.95f);
	return (s<=(15*DEGRAD));
}

void Projectile::doStep(int index)
{
    SceneryObject* so;
    PhysicalObject* po;
	bool move=true;

    PhysicalObject::doStep(index);

	int tmp_target=object_target;
	if(targeted){
		//if(container!=NULL){
		//	ai->step();
		//}
		//-------------------------

		return;
		if(tmp_target==-1 && object_owner>-1){
			if(container->types[object_owner]==OT_PSHP) tmp_target=container->selectedObject;
		}
		if(tmp_target!=-1) if(container->objects[tmp_target]==NULL) tmp_target=-1;
		if(tmp_target==-1){

			Range rn;
			rn.set(0,400,true,false);
			ReputationFilter rf;
			ObjectFilter of;
			rf.setMatchOnly(ENEMY);
			rf.setMatch(CRIMINAL);
			of.setMatchOnly(OT_NSHP);
			of.setMatch(OT_PSHP);
			of.setMatch(OT_PROJ);
			tmp_target=container->getClosestFiltered(index,&of,&rf,&rn);
			//object_target=tmp_target;//permanently stuck to it.
		}
		if(tmp_target!=-1)
		{
			idxDebugLine=tmp_target;
			so=(SceneryObject*) container->objects[tmp_target];
			if(so!=NULL) FollowStep(so);
		}
	}

    //Projectile* pj;
    bool collided=false;
    for(int j=0;j<OBJECTS_MAX;j++)
    {
        if(container->objects[j]==NULL) continue;
        if(j==object_owner || j==index || j==object_lastcollision) continue;
        so=(SceneryObject*) container->objects[j];
        if(so->physical)//CONSIDER USING types[] or implement an ObjectContainer method - so->physical || 
        {
			if(object_owner>0 && so->type==OT_PROJ)
				if( ((Projectile*) so)->object_owner==object_owner) continue;

            int view=container->getOwnerView(j,object_owner);
            if(view>FRIENDLY) continue;
            po=(PhysicalObject*) so;
            if(collideCheck(po)){

                collided=true;
				collideStep(index,j,po);
				/*
				object_lastcollision=j;

				if(hasgravity) stepGravity(po);
				if(po->hasgravity) po->stepGravity(this);
				
				if(po->solid){
					bump(po);
					collideStep(po);
				}
                damage( po->collisDmg);//CONSIDER FLOAT
                if(hitnotify) po->hitNotify(j,object_owner,-1);//*** types[object_owner]
                if(container->types[j]==OT_PROJ) ((Projectile*) po)->object_lastcollision=index;
				*/
                
                break;
            }
        }
    }
    if(collided)
    {
        istep=0;
    }else
    {
        istep++;
        if(istep==60){
            object_lastcollision=-1;
            istep=0;
        }
    }
}