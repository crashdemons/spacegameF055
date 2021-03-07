#include "objects_Debris.h"


void Debris::set(ObjectContainer* cont, Projectile_Type& pt)
{
	container=cont;
    setProjectile(&pt);
	spin*=random(-3,3);
	ii->Rotate(frandom(-PI2,PI2));
	//ii->rotation=frandom(-PI2,PI2);
}
Debris::Debris(void)
{
	setUnownedView(ENEMY);
}

Debris::~Debris(void)
{
}
