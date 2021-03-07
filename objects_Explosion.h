/**
@file objects_Explosion.h
@brief 

...
@author crashenator (crashenator -at- gmail.com)
*/

#pragma once

#include "objects_Scenery.h"
class Explosion : public SceneryObject
{
    public:
        fRECT svDst;
        fPOINT svCenter;
        float svRotation;
        Explosion();
        void save(Image_Info* ix);
        void restore();
		void update();
        void doStep(int index);
};
