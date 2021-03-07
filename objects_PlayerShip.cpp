#include "objects_PlayerShip.h"
#include "sound.h"

#ifndef INPUT_MACROS
	#define INPUT_MACROS
	#define KEYSTATE(y,z) (Input_GetKeyState(container->in,z)&y)==y
	#define KEYSTATEEX(x,y,z) (Input_GetExplicitKeyState(container->in,z,x)&y)==y
	#define MOUSE_DOWN(y) KEYSTATEEX(INPUT_KEYS_MOUSE,INPUT_KEY_DOWN,y)
	#define MOUSE_UP(y) KEYSTATEEX(INPUT_KEYS_MOUSE,INPUT_KEY_UP,y)
	#define MOUSE_PRESSED(y) KEYSTATEEX(INPUT_KEYS_MOUSE,INPUT_KEY_PRESSED,y)
	#define MOUSE_RELEASED(y) KEYSTATEEX(INPUT_KEYS_MOUSE,INPUT_KEY_RELEASED,y)

	#define KEY_DOWN(y) KEYSTATE(INPUT_KEY_DOWN,y)
	#define KEY_UP(y) KEYSTATE(INPUT_KEY_UP,y)
	#define KEY_PRESSED(y) KEYSTATE(INPUT_KEY_PRESSED,y)
	#define KEY_RELEASED(y) KEYSTATE(INPUT_KEY_RELEASED,y)
#endif




void PlayerShip::doStep(int index)
{
    Ship::doStep(index);

    bool fired=false;
    float da=0;
    int lsteps=abs(istep-jstep);


	bool deccelkey=KEY_DOWN(SDLK_SPACE);
	bool move=!deccelkey;//this is passed byref below;
//---------------------------------
	if(useMouse){
		POINT3 pt=Input_GetMousePos(container->in);
		fPOINT fpt;
		fpt.x=(float)pt.x;
		fpt.y=(float)pt.y;
		fpt.x+=container->di->viewport.x;
		fpt.y+=container->di->viewport.y;
		fRECT fr;
		fr.left=0; fr.right=fpt.x;
		fr.top=0; fr.bottom=fpt.y;
		

		FollowStep(fpt.x,fpt.y,move);
	}
	hasTrail=move || VelocAvg()>1.5;
	hasGlow=move;

	if(KEY_PRESSED('8') || KEY_PRESSED('m')){ useMouse=!useMouse; Deccelerate(0.001f,0.50f); } 
	if(KEY_PRESSED('g')){ stats.health.set(0,9999); } 
	
//---------------------------------


    if(KEY_DOWN(SDLK_RSHIFT) || KEY_DOWN(SDLK_LSHIFT)) da=PI;
	if(stats.energy>5)
    {
        if(istep%rof==0)
        {
            if(KEY_DOWN(VK_RETURN) || KEY_DOWN(VK_LBUTTON))
            {
				stats.energy.decrement(5);
                fired=true;
                Projectile* pj=new Projectile();
                pj->container=container;
                pj->setProjectile(container->Projectile_Plasma);
                pj->object_owner=index;
                FireProjectile(pj, 5,-1,0,da);
				container->setDepth(container->add(pj),0);
            }
        }
    }
    if(stats.energy>1)//0.5
    {
        if(KEY_DOWN('3'))
        {
			stats.energy.decrement(1);//0.5;
            fired=true;
            Projectile* pj=new Projectile();
            pj->container=container;
            pj->setProjectile(container->Projectile_MiningLaser);
            pj->object_owner=index;
            FireProjectile(pj, 5,2*DEGRAD,48,da);
            container->add(pj);
        }
    }
    if(stats.energy>50 && lsteps>50)
    {
        if(KEY_DOWN('4'))
        {
			stats.energy.decrement(50);
            fired=true;
            Projectile* pj=new Projectile();
            pj->container=container;
            pj->setProjectile(container->Projectile_PlasmaBomb);
            pj->object_owner=index;
            FireProjectile(pj, 5,-1,0,da);
            container->add(pj);
        }
		if(KEY_DOWN('b'))
        {
			stats.energy.decrement(50);
            fired=true;
            Projectile* pj=new Projectile();
            pj->container=container;
            pj->setProjectile(container->Projectile_Missile);
			pj->object_target=container->selectedObject;
            pj->object_owner=index;
            FireProjectile(pj, 1,-1,0,da);
            container->add(pj);
        }
    }
	if(stats.energy>30 && lsteps>50)
    {
        if(KEY_DOWN('5'))
        {
			stats.energy.decrement(30);
            fired=true;
            Projectile* pj=new Projectile();
            pj->container=container;
            pj->setProjectile(container->Projectile_Mine);
            pj->object_owner=index;
            FireProjectile(pj, 5,-1,0,da);
            container->add(pj);
        }
    }
	if(stats.energy>=1)//0.5
    {
        if(KEY_DOWN('6'))
        {
			if(istep%2==0) stats.energy.decrement(1);
            fired=true;
            Projectile* pj=new Projectile();
            pj->container=container;
            pj->setProjectile(container->Projectile_TractorBeam);
            pj->object_owner=index;
            FireProjectile(pj, 5,0,75,da);
            container->add(pj);
        }
    }
    if(KEY_DOWN('9')) damage(20);
    
    if(fired) jstep=istep;
    
	if(KEY_DOWN(SDLK_UP))    AccelerateRel(-0.05f);
    if(KEY_DOWN(SDLK_DOWN))  AccelerateRel(+0.05f);
    if(deccelkey) Deccelerate(0.001f,0.95f);
    if(KEY_DOWN(SDLK_LEFT))  TurnStepDirection(true);
    if(KEY_DOWN(SDLK_RIGHT)) TurnStepDirection(false);
    istep++;
	
}
PlayerShip::PlayerShip(){
	useMouse=true;
    type=OT_PSHP;
    //rep=new Reputation();
}