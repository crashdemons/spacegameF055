/**
@file objects_Debris.h
@brief 

...
@author crashenator (crashenator -at- gmail.com)
*/

#pragma once
#include "objects_Projectile.h"

class Debris : public Projectile
{
public:
	void set(ObjectContainer* cont, Projectile_Type& pt);
	Debris(void);
	~Debris(void);
};
