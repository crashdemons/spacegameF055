/**
@file objects_Scenery.h
@brief Base class for all in-game objects as a form of sprite with extended movement and effect functions

...
@author crashenator (crashenator -at- gmail.com)
*/

#pragma once
#include "ObjectContainer.h"

class SceneryObject /// Base class for all in-game objects as a form of sprite with extended movement and effect functions
{
	private:
		void reattach();//for re-centering with idxAttachedTo
    public:
		bool inscreen,inworld;
		bool visible,randmove,physical,canmove,required;
        Image_Info* ii;
		bool relative,centered,scaled;//is this sprite drawn relative to screen viewport? does it center the viewport? is the image scaled?
		Boundary boundary;
		bool requires_boundaries;



        ObjectContainer* container;//just for external object referencing really - ugly implementation
        int type;//enum OBJECT_TYPE Object child-type, for workaround reasons and ease of implementation.
		int object_index;


        int istep,jstep;//internal step values for various child-class uses; used for tick-measuring
        float fstep;

        long int lifespan,age;//age is a tick-counter that increments every Step (update), if lifespan is passed die() is called.
        
        float velocity[2];
        float spin;
		float movementAngle;

		bool hasTrail;
		bool hasGlow;
		int lifeGlow;
		int maskTrail;
		int maskGlow;

		int idxAttachedTo;
		void* ptrAttachedTo;//ONLY to be used for comparison purposes, an additional step to decrease index-replacement (attaching to new element filling released space)

		int idxDebugLine;





        virtual void die();//changes visibility to false (queue will dispose of it); virtual for obvious reasons.
        void UnloadImg();
        void LoadImg(Drawing_Info* di,string filename, int transcolor=0, int scaled=-1);
        void getCenterRel(float* x, float* y);//get the center relative to the image placement ( center of the sprite in the screen DstRect.left+center.x...)
        void setCenterRel(float x, float y);//move the placement according to a new center. (adjust screen coordinates by image center and set the Top-Left coord of sprite)
        void setCenterRel(SceneryObject* so, float d=0);//places the sprite a distance, d, from another sprite where the image rotation determines position on the d-radius circle.
        void Draw(Drawing_Info* di);//draw the Image_Info
        void DrawRect(Drawing_Info* di);//draw a debug rect (image destination rect)
        void DrawCirc(Drawing_Info* di);//draw a debug circle (initial image radius)
        void Turn(float radians=1);//set image rotation
        void TurnRel(float radians=1);//apply image rotation delta
        void CornerMoveTo(int x, int y);//set the sprite by a top-left position
        void CornerMoveTo(float x, float y);
        void Move(float dx, float dy);//apply deltas to the sprite by its top-left position
        void MoveAngle(float dn);//move the sprite deltaN units from its on-screen-center in the direction it is facing (image rotation angle)
        void VelocityStepRel(float fraction=1,bool randomize=false);//MoveRel using `velocity`. you can randomize this amount. (`fraction` argument is deprecated but re-implemented)
        void VelocityStepProject(float fraction, float& x, float& y);//project the next step coordinates.
		void Deccelerate(float n,float fraction=0);//lowers the velocity (multiplies first by fraction, then reduces velocities by 'n' units toward 0 [not past])
        float VelocAvg();
        void VelocStop();//zeros-out the velocity
        void AccelerateRel(float n,float rotation=666);//applies 'n' unit as a velocity delta in the 'rotation' angle. (flag 666 defaults rotation to the image rotation. read: facing direction)
        float getAngleTo(float x, float y);//find the angle to (x,y) from the sprite center (getCenterRel)
        float getAngleTo(SceneryObject* so);//find the angle to the center of a sprite from this sprite (above method)
        float distTo(float x, float y);//sprite-centered 2D Distance Equation
        float distTo(SceneryObject* so);
        float SweepAngle(float b);//find the angle-delta between the image rotation (facing angle) and `b`
        float SweepAngle(SceneryObject* so);//find the angle-delta between current rotation and the angle facing the sprite.
        bool isInWorld(bool forcecheck=false);//checks if any part of the sprite rect is within the game world rect (maximum world boundaries);  (was originally Screen, then Screen+500)
		bool isInScreen(Drawing_Info* di,bool forcecheck=false);//checks if the sprite is within the viewport (screen);


		void effectTrail(Image_Info* from,int lifespan=10);//EXPERIMENTAL: spawn a limited-life sprite with low alpha
		void effectGlow(Image_Info* from,int lifespan=10);//EXPERIMENTAL: spawn a limited-life sprite with low alpha

		void serialize();


        void virtual doStep(int index);
        void soinit();
        SceneryObject();
        SceneryObject(Drawing_Info* di,const char* filename, int transcolor=0);
        virtual ~SceneryObject();
};
