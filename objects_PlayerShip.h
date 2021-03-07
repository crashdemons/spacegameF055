/**
@file objects_PlayerShip.h
@brief 

...
@author crashenator (crashenator -at- gmail.com)
*/

#pragma once
#include "objects_Ship.h"
#include "input.h"


class PlayerShip : public Ship
{
    public:
		bool useMouse;
        void doStep(int index);
        PlayerShip();
    
};
