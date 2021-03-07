#include "objects_Physical.h"
#include "drawing.h"
#include "sound.h"

 void PhysicalObject::die(){
	 SceneryObject::die();
	 hasdied=true;
	 if(deathexplode) { explode(); }
 }
        //void die(){die(true);}
bool PhysicalObject::FollowStep(SceneryObject* ship,bool& move,bool canstop)//follow a ship and orbit it - a very aggressive technique
{
		float s,a=getAngleTo(ship);
		bool ccw=TurnGetDirection(a,&s);
		s=fabs(s);
		float dist=distTo(ship);
//		float vavg=VelocAvg();
        
		if(s>(10*DEGRAD)) move=false;//10
		if(s<=(15*DEGRAD) &&  dist<100) move=false;
		if(!move && !canstop) move=true;
        
		if(ccw) a=realAngle(a-9*DEGRAD);
		else a=realAngle(a+9*DEGRAD);
        
		TurnStepToward(a);
		if(move) AccelerateRel((float) -(0.03 + dist/1000));
		else Deccelerate(0.001f,0.95f);
		return (s<=(15*DEGRAD));
}
bool PhysicalObject::FollowStep(float x, float y,bool& move)//follow a ship and orbit it - a very aggressive technique
{
		float s,a=getAngleTo(x,y);
		bool ccw=TurnGetDirection(a,&s);
		s=fabs(s);
		float dist=distTo(x,y);
//		float vavg=VelocAvg();
        
		if(s>=(25*DEGRAD) || dist<100) move=false;
        
		if(ccw) a=realAngle(a-9*DEGRAD);
		else a=realAngle(a+9*DEGRAD);
        
		if(s>=(5*DEGRAD)) TurnStepToward(a);

		if(move) AccelerateRel(-0.05f);//AccelerateRel((float) -(0.03 + dist/1000));
		else Deccelerate(0.001f,0.95f);
		return (s<=(15*DEGRAD));
}


bool PhysicalObject::TurnGetDirection(float a, float* s)
{
    bool ccw;
   // float orot=ii->rotation;
   // ii->rotation=orot-turnstep;
	ii->Rotate(-turnstep);
    float sa1=SweepAngle(a);
   // ii->rotation=orot+turnstep;
	ii->Rotate(turnstep*2.0f);
    float sa2=SweepAngle(a);
	ii->Rotate(-turnstep);//<--------------ERROR 1
    //ii->rotation=orot;
    *s=sa2;//float=float, copy//<--------------ERROR 2
    ccw=(sa1<sa2);
    if(ccw) *s=sa1;
    return ccw;
}
bool PhysicalObject::TurnGetDirection(SceneryObject* so){float s; return TurnGetDirection(getAngleTo(so),&s);}
void PhysicalObject::TurnStepDirection(bool ccw, float step){
    if(step==0) step=turnstep;
    if(ccw) TurnRel(-step);
    else TurnRel(step);
}
void PhysicalObject::TurnStepToward(float a){
    float s;
    bool ccw=TurnGetDirection(a,&s);
    
    if(fabs(fabs(s)-PI)<(10*DEGRAD)) ccw=true;//hack for turning indecision.
    if(s>=turnstep) TurnStepDirection(ccw,turnstep);
}
void PhysicalObject::TurnStepToward(SceneryObject* so){TurnStepToward(getAngleTo(so));}

void PhysicalObject::explode()
{
    Explosion* ex=new Explosion();
    ex->container=container;
    ex->save(ii);
    ex->doStep(-1);
	ex->velocity[0]=velocity[0]/2;
	ex->velocity[1]=velocity[1]/2;
	container->setDepth(container->add(ex),0);
}
void PhysicalObject::stepGravity(PhysicalObject* po)
{
	if(!hasgravity) return;
	float r=realAngle(getAngleTo(po));
    po->AccelerateRel(gravity,r);
}
void PhysicalObject::bump(PhysicalObject* po)
{
    float r1=getAngleTo(po);
    float r2=realAngle(r1+PI);
    if(po->solid) po->AccelerateRel(bumpforce,r2);
    if(solid) AccelerateRel(po->bumpforce,r1);

	if(solid && po->solid){
		if(po->spin!=0) po->spin=-po->spin;
		if(spin!=0) spin=-spin;
	}
}
void PhysicalObject::eventShieldColor(int mask)
{
	maskAge=age;//a tick value from SceneryObject.
	newShieldMask=mask;//light yellow;
}
void PhysicalObject::damage(int d){
    if(immune) return;
	if(d==0) return;
	if(stats.health.empty()) return;//dead
	
	int x1=container->di->viewport.x;
	int y1=container->di->viewport.y;
	int x2=x1+container->di->width/2;
	int y2=y1+container->di->height/2;
	Sound_ClipPlay(container->si,"./Snd/boom.wav",fabs(distTo(x2,y2)),1000);
	

	bool bShield=!stats.health.empty();
	float iShield=stats.shields;

	bool flare=false;

	if(bShield && iShield<=50)
	{
		flare=true;
		stats.shields.decrement(d);

		float dmg=(float) d;
		dmg-=(0.5f)*dmg;
		d=round( ((float)d) - (d * 0.5f) );

	}


	if(bShield && iShield>50)
	{
		flare=true;
		stats.shields.decrement(d);
	}else{
		stats.health.decrement(d);
		if(stats.health.empty()) die();
	}
	if(flare)
	{
		eventShieldColor(0x7FFFFF3F);
		soShield.effectGlow(soShield.ii,10);
	}

}
void PhysicalObject::faceCoord(float x, float y){ii->rotation=getAngleTo(x,y);}
void PhysicalObject::faceObject(SceneryObject* so){ii->rotation=getAngleTo(so);}
bool PhysicalObject::collideRadius(SceneryObject* so)
{
	/*if( ii->circle.collide(so->ii->circle) || ii->rectangle.collide(so->ii->rectangle) ){
		ii->circle.draw(container->di);
		ii->rectangle.draw(container->di);
	}*/
	if( ii->circle.collide(so->ii->circle) ){
		if(ii->rectangle.collide(so->ii->rectangle) ){
			//ii->rectangle.debug();
			//so->ii->rectangle.debug();
			return true;
			//System_Alert("x");
		}else return false;
	}




    float d=distTo(so);
    float d2=ii->radius + so->ii->radius;
    if(d2>=d) return true;
    //if((so->ii->radius)>=d) return true;
    return false;
}
void PhysicalObject::hitNotify(int index, int object_owner,int owner_type){}

void PhysicalObject::loadEffectShield()
{
	if(container->di==NULL || soShield.ii!=NULL) return;
	System_DebugOutput("Load Shield\n");
	soShield.relative=ii->viewrelative;
	soShield.scaled=true;
	soShield.LoadImg(container->di,"./Img/shield.bmp",0,0);
	soShield.ii->mask=Drawing_GetColor(0,0xFF,0xFF,0xFF);//0x7FFF007F+
	soShield.container=container;

	idxShield=container->add(&soShield,OD_TOP,OF_NODELETE);//prevents ObjectContainer for automatically releasing this object.  since it is on the stack, ~PhysicalObject() will release the shield object.
	

}
void PhysicalObject::setEffectShield(int mask)
{
	if(soShield.ii==NULL) return;
	//OutputDebugString("Set Shield\n");
	soShield.ii->rotation=ii->rotation;
	soShield.ii->DstRect=ii->DstRect;
	soShield.ii->DstRect.left-=10;
	soShield.ii->DstRect.top-=10;
	soShield.ii->DstRect.bottom+=10;
	soShield.ii->DstRect.right+=10;

	soShield.ii->RecalculateCircle();
	soShield.ii->mask= mask;//Drawing_GetColor(opacity,0xFF,0xFF,0xFF);//0x7FFF007F+
	soShield.setCenterRel(this);
	soShield.canmove=false;
	soShield.idxAttachedTo=object_index;
}

int stepMask(int from, int tocl, char step)
{
	int fr[4];
	int to[4];
	Drawing_GetColorComponents(from,fr[0],fr[1],fr[2],fr[3]);
	Drawing_GetColorComponents(tocl,to[0],to[1],to[2],to[3]);
	for(int i=0;i<4;i++)
	{
		if(fr[i]<to[i]){
			fr[i]+=step;
			if(fr[i]>to[i]) fr[i]=to[i];
		}else if(fr[i]>to[i]){
			fr[i]-=step;
			if(fr[i]<to[i]) fr[i]=to[i];
		}
	}
	return Drawing_GetColor(fr[0],fr[1],fr[2],fr[3]);
}

void  PhysicalObject::doStep(int index)
{
	ai->step();
	equipment.step(stats);

    SceneryObject::doStep(index);

	if(!stats.shields.empty()){
		loadEffectShield();//only loads the first time.
		if( (age-maskAge)>5 ){
			newShieldMask=container->grShields.colors[(int) round(stats.shields.percent()*255.0)];
			//if(stats.shields.percent()>0.99) newShieldMask=0x7FFFFFFF;
			//if(stats.shields.percent()<=0.99)newShieldMask=0x7F7F7FFF;
			//if(stats.shields.percent()<0.75) newShieldMask=0x7F00FF00;
			//if(stats.shields.percent()<0.50) newShieldMask=0x7FFF7F00;//orange?
			//if(stats.shields.percent()<0.25) newShieldMask=0x7FFF0000;
		}

	}else newShieldMask=0;
	curShieldMask=stepMask(curShieldMask,newShieldMask,30);
	setEffectShield(curShieldMask);

}
PhysicalObject::PhysicalObject()
{
	ai=new AI();
	ai->type=AI_TYPE_NONE;

	requires_boundaries=true;

	idxShield=-1;
	curShieldMask=0;
	newShieldMask=0;
	maskAge=0;
	relative=true;

    type=OT_PHYS;
    immune=false;
    solid=true;
    hasdied=false;
    physical=true;
    collisDmg=1;
    bumpforce=0.1f;
	stats.shields.set(0,0);
	stats.health.set(0,50);
	stats.energy.set(0,0);
	stats.cargo.set(0,0);
	stats.fuel.set(0,0);

//           shield_max=100;
//            shield_rechargestep=0.2f;
    killsProjectiles=false;
    deathexplode=true;
//            health=100;healthmax=100;
	hasgravity=false;
	gravity=0.1f;
	if(ii) ii->requires_boundaries=true;
}
 PhysicalObject::~PhysicalObject()
{
	delete ai;
	container->remove(idxShield,false);
	equipment.clear(stats);
}

