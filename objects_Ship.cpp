#include "objects_Ship.h"
#include "objects_Debris.h"
#include "Equipment.h"
#include "sound.h"

void* Ship::sndFire=NULL;

void Ship::die()
{
	PhysicalObject::die();
	fPOINT pt=Dim(ii->DstRect);
	float a=(pt.x) * (pt.y);
	int x= (int) limit( (a/5), 4, 10);
	int n= random(2,x);

	Debris* db;
	for(int i=0;i<n;i++)
	{
		db=new Debris();
		db->lifeGlow=30;
		db->lifespan=100;
		db->scaled=false;
		db->set(container,container->Projectile_Scraps);
		db->velocity[0]=velocity[0]/2;
		db->velocity[1]=velocity[1]/2;
        db->ii->rotation=frandom(-PI2,PI2);
        db->setCenterRel(this,-20);//distance from the projectile's center.
        db->AccelerateRel(3);
        container->add(db);
	}
}

void Ship::FireProjectile(SceneryObject* pj, float speed, float fe, float d, float da)//fe=firing error amount;d=distance from shooter,da=rotation angle offset
{
    if(fe==-1) fe=fireerror;
    float ferror=frandom(-fe,+fe);//((float)random(-1,1))*fe;
    //pj->ii->rotation = realAngle(ii->rotation + da + ferror);
	pj->ii->RotateTo( realAngle(ii->rotation + da + ferror) );

    pj->setCenterRel(this, d);
    pj->velocity[0]=velocity[0]+sin(pj->ii->rotation)*speed;//vx+2
    pj->velocity[1]=velocity[1]-cos(pj->ii->rotation)*speed;//vy+2

	int x1=container->di->viewport.x;
	int y1=container->di->viewport.y;
	int x2=x1+container->di->width/2;
	int y2=y1+container->di->height/2;
	Sound_ClipPlay(container->si,"./Snd/shot.wav",fabs(distTo(x2,y2)),1000);
}

void Ship::doStep(int index){ 
	PhysicalObject::doStep(index);
	//if(sndFire==NULL && container!=NULL) sndFire=Sound_ClipLoad(container->si,"shot.wav");
}
Ship::Ship()
{

    rof=2;
    collisDmg=5;
    type=OT_SHIP;
    turnstep=5*DEGRAD;
    fireerror=5*DEGRAD;
	deathspawn=NULL;

	

	stats.health.set(0,50);
	stats.shields.set(0,0);
	stats.energy.set(0,0);
	stats.cargo.set(0,50);
	stats.quarters.set(0,0);


	//equipment.clear(stats);
	equipment.add(EQUIPMENT(STORAGE,ENERGY));
	equipment.add(EQUIPMENT(REACTOR,FISSION));//released with EquipmentContainer DCTOR or .clear() or .Release()
	
	
	equipment.startup(stats);

}
//Ship::~Ship(){}