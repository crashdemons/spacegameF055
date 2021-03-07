#include "objects_Scenery.h"


void SceneryObject::serialize()
{
	//type
	//istep,jstep, fstep
	//age, lifespan
	//velocity, spin
	//visible,randmove,physical,canmove;
	//ii->viewcentered
	//ii->viewrelative
	//ii->rotation
	//ii->image
	//ii->DstRect
}

void SceneryObject::die(){ if(!required) visible=false;}
        void SceneryObject::UnloadImg(){
			if(ii!=NULL){
				Drawing_UnloadImage(ii);
				ii=NULL;
			}
		}
        void SceneryObject::LoadImg(Drawing_Info* di,string filename, int transcolor, int scaleoverride)
        {
			if(scaleoverride==0) scaled=false;
			else if(scaleoverride>0) scaled=true;
            UnloadImg(); 
            ii=Drawing_LoadImage(di, filename, 0, transcolor, scaled,scaleoverride,requires_boundaries);
			ii->viewrelative=relative;
			ii->viewcentered=centered;
			//ii->RecalculateCircle();
			//ii->RecalculateRectangle();
        }
        void SceneryObject::getCenterRel(float* x, float* y)//get the center relative to the image placement
        {
			if(true){
				//cout<<"P: ";
				//ii->center.debug();
				fPOINT cabs=ii->center.getAbsolute();
				*x=cabs.x;
				*y=cabs.y;
				//cabs.debug();
				//cout<<endl;
				//System_Alert("x");
			}else{
			///*
            *x = ii->center.x + ii->DstRect.left;
            *y = ii->center.y + ii->DstRect.top ;

		//	fPOINT cabs(*x,*y);
				//cout<<"P: ";
				//ii->center.debug();
				//cabs.debug();
				//cout<<endl;
				//System_Alert("x");
			//*/
			}
        }
        void SceneryObject::setCenterRel(float x, float y)//move the placement according to a new center.
        {
			ii->MoveTo(x,y);
            //x-=ii->center.x;
            //y-=ii->center.y;
            //Move(x, y);
        }
        void SceneryObject::setCenterRel(SceneryObject* so, float d)
        {
			//move placement according to a new center that lies on a radius (d; distance) from the center of another object.
			//the position along the radius-circle will be determined by the image rotation. (shortcut)
    		setCenterRel(so->ii->center.x + so->ii->DstRect.left, so->ii->center.y + so->ii->DstRect.top);
            if(d!=0)
            {
                setCenterRel(
                    so->ii->DstRect.left+so->ii->center.x +sin(ii->rotation)*d, 
                    so->ii->DstRect.top +so->ii->center.y -cos(ii->rotation)*d
                );
            }
        }
		void SceneryObject::Draw(Drawing_Info* di){
			if(visible && isInScreen(di)){
				if(ii) { ii->SyncBoundaries(); }
				reattach();
				Drawing_DrawImage(di,ii);
			} 
		}
        void SceneryObject::DrawRect(Drawing_Info* di){
			if(type>OT_SCEN && visible){
				ii->rectangle.draw(di,false);
				//if(ii->DstRect.left<10 && ii->DstRect.left>-10) if(string(ii->image)!="./Img/circle.bmp"){
					
					//ii->Debug();
					//cout<<ii->image<<endl;
				//}
			}
            //Drawing_DrawRectFilled(di, ii->DstRect, 0x3F00FF00,ii->viewrelative,ii->relativemult);
        }
        void SceneryObject::DrawCirc(Drawing_Info* di)
        {
			if(type>OT_SCEN && visible) 
				ii->circle.draw(di,false);
			//fPOINT Absolute(ii->center.x + ii->DstRect.left, ii->center.y + ii->DstRect.top);
           // Drawing_DrawCircle(di,Absolute, ii->radius, 0xFFFF0000,ii->viewrelative,ii->relativemult);


			if(idxDebugLine>-1){
				SceneryObject* so=(SceneryObject*) container->objects[idxDebugLine];
				if(so!=NULL){
					float x1,y1;
					getCenterRel(&x1,&y1);
					float x2,y2;
					so->getCenterRel(&x2,&y2);
					Drawing_DrawLine(container->di,x1,y1,x2,y2,0xFFFF0000);
				}
			}

        }
		void SceneryObject::Turn(float radians){ii->RotateTo(realAngle(radians));}//ii->rotation=realAngle(radians);}
		void SceneryObject::TurnRel(float radians){ii->Rotate(realAngle(radians));}//ii->rotation=realAngle(ii->rotation+radians);}
        void SceneryObject::CornerMoveTo(int x, int y){
            return CornerMoveTo((float) x, (float) y);
        }
		void SceneryObject::CornerMoveTo(float x, float y){
			float x0,y0;
            getCenterRel(&x0,&y0);
			movementAngle=getAngleTo(x, y);

			ii->CornerMoveTo(x,y);

			/*
            float dx=x-ii->DstRect.left;
            float dy=y-ii->DstRect.top;
            MoveRel(dx, dy);
			*/
        }
        void SceneryObject::Move(float dx, float dy){
			float x0,y0;
            getCenterRel(&x0,&y0);
			movementAngle=getAngleTo(x0+dx, y0+dy);

			ii->Move(dx,dy);

            //ii->DstRect.left+=dx; ii->DstRect.right+=dx;
            //ii->DstRect.top+=dy; ii->DstRect.bottom+=dy;
        }
        void SceneryObject::MoveAngle(float n)
        {//will be parallel with center ray
            float dx,dy;
            dx=-sin(ii->rotation)*n;
            dy=cos(ii->rotation)*n;
            Move(dx, dy);
            /* Move 'n' Units based off of Angle 'a' from object center*/
        }
        void SceneryObject::VelocityStepRel(float fraction,bool randomize)
        {
			float vel[2]={velocity[0]*fraction,velocity[1]*fraction};//RE-implemented, I hope this doesn't break anything
            if(randomize)
            {
				//I may be wrong, looking back but based on `rand()%((max-min)+1) + min` (random() : common.h)
				// this appears to apply a random movement between -1 and +1
                if(vel[0]!=0) Move(0, (float)(rand()%3 - 1)/(float)2);
                if(vel[1]!=0) Move((float)(rand()%3 - 1)/(float)2 ,0);
                
            }
            Move(vel[0],vel[1]);
        }
		void SceneryObject::VelocityStepProject(float fraction, float& x, float& y)
        {
			float vel[2]={velocity[0]*fraction,velocity[1]*fraction};

            getCenterRel(&x,&y);
			x+=vel[0];
			y+=vel[1];
        }
        void SceneryObject::Deccelerate(float n,float fraction)
        {
            if(fraction!=0)
            {
                velocity[0]*=fraction;
                velocity[1]*=fraction;
            }
            float signs[2]={sign(velocity[0]),sign(velocity[1])};
            velocity[0]-=signs[0]*n;
            velocity[1]-=signs[1]*n;
            if(signs[0]!=sign(velocity[0])) velocity[0]=0;
            if(signs[1]!=sign(velocity[1])) velocity[1]=0;
        }
        float SceneryObject::VelocAvg(){
            return sqrt(velocity[0]*velocity[0] + velocity[1]*velocity[1]);
        }
        void SceneryObject::VelocStop(){velocity[0]=0;velocity[1]=0;}
        void SceneryObject::AccelerateRel(float n,float rotation)
        {
            if(rotation==666) rotation=ii->rotation;
            float dvx=-sin(rotation)*n;
            float dvy=cos(rotation)*n;
            velocity[0]+=dvx;
            velocity[1]+=dvy;
        }
        float SceneryObject::getAngleTo(float x, float y)
        {
            float x0,y0;
            getCenterRel(&x0,&y0);
            float dx=x-x0;
            float dy=y-y0;
            float a=atan(-dx/dy);
            if(dy>=0) a+=PI;
            return a;
        }
        float SceneryObject::getAngleTo(SceneryObject* so)
        {
            float x,y;
            so->getCenterRel(&x,&y);
            return getAngleTo(x, y);
        }
        float SceneryObject::distTo(float x, float y)
        {
            float x0,y0;
            getCenterRel(&x0,&y0);
            return sqrt((x-x0)*(x-x0) + (y-y0)*(y-y0));
        }
        float SceneryObject::distTo(SceneryObject* so)
        {
            float x,y;
            so->getCenterRel(&x,&y);
            return distTo(x,y);
        }
        float SceneryObject::SweepAngle(float b)
        {
            float a=ii->rotation;
            float sweep=realAngle(b)-realAngle(a);
            if(sweep<0) sweep=-sweep;
            if(sweep>PI) sweep=PI2 - sweep;
            return sweep;
        }
        float SceneryObject::SweepAngle(SceneryObject* so){return SweepAngle(getAngleTo(so));}
        bool SceneryObject::isInWorld(bool forcecheck)
        {
			if(forcecheck){
				inworld=true;
				if(container!=NULL){
					RECT out;
					RECT chk=Drawing_ApproxRECT(ii->DstRect);
					inworld=(IntersectRect(&out,&(container->di->world),&chk)==TRUE);
				}
			}
            return inworld;
        }
		bool SceneryObject::isInScreen(Drawing_Info* di,bool forcecheck)
		{
			if(!forcecheck) return inscreen;
			if(di==NULL) return false;
			RECT out;
			RECT chk=Drawing_ApproxRECT(ii->DstRect);
			RECT vwp=di->rect;

			if(ii==NULL) return false;

			if(!ii->viewrelative) return true;//this check doesn't work for absolutely-positioned items consistently (without below rect adjustment)... I have no idea why.

			chk.left-=ii->relativemult * di->viewport.x;
			chk.top-=ii->relativemult * di->viewport.y;
			chk.right-=ii->relativemult * di->viewport.x;
			chk.bottom-=ii->relativemult * di->viewport.y;

			return IntersectRect(&out,&vwp,&chk)==TRUE;
		}
		void SceneryObject::reattach()
		{
			if(!visible) return;
			if(idxAttachedTo>-1 && ptrAttachedTo!=NULL){
				if(container!=NULL){
					void* p=container->objects[idxAttachedTo];
					if(p==NULL || p!=ptrAttachedTo) return die();
					SceneryObject* so =(SceneryObject*) p;
				    setCenterRel(so);
				}
			}
		}
        void SceneryObject::doStep(int index)
        {
			inscreen=false;

            if(lifespan!=-1 && age>lifespan) die();
            else{
                if(canmove) VelocityStepRel(1,randmove);
                if(spin!=0 && inscreen) TurnRel(spin);
				if(!required && !isInWorld(true)){
					if(type>=OT_PHYS && visible){
						idxAttachedTo=-1;
						ptrAttachedTo=NULL;

						float r=getAngleTo(0,0);

						AccelerateRel(1,r+PI);
					}else die();
				}

				
				if(container!=NULL) inscreen=isInScreen(container->di,true);

				if(hasTrail && inscreen) effectTrail(ii,10);
				if(hasGlow && inscreen && age<5) effectGlow(ii,lifeGlow);

				//reattach();

                age++;
				if(age>65535) age=0;
            }
        }

		void SceneryObject::effectTrail(Image_Info* from,int lifespan)
		{
			//return;
			if(from==NULL || container==NULL) return;
			if(age%2) return;
			SceneryObject* so=new SceneryObject();
			so->relative=from->viewrelative;
			so->scaled=false;
			so->LoadImg(container->di,"./Img/particle.bmp",0,0);
			so->ii->viewrelative=from->viewrelative;


			if(maskTrail==-1)
				so->ii->mask=Drawing_GetColor(0x7F,random(0,0x3F),random(0x3F,0xFF),random(0x7F,0xFF));
			else
				so->ii->mask=maskTrail | Drawing_GetColor(0,random(0,0x1F),random(0,0x1F),random(0,0x1F));


			//so->ii->DstRect=from->DstRect;
			//so->ii->RecalculateCircle();
			
			so->ii->radius=ii->radius;
			so->lifespan=lifespan;
			so->canmove=true;
			so->container=container;
			int idx=container->add(so);//add() automatically cleans up SceneryObjects when no index is available.
			if(idx==-1) return;
			container->setDepth(idx,2);


			float ferror=frandom(-0.5f,+0.5f);//((float)random(-1,1))*fe;
			so->ii->RotateTo( realAngle( ii->rotation + ferror ));
			so->setCenterRel(this, -20);
			//so->setCenterRel(this);
			so->velocity[0]=- (+sin(so->ii->rotation)*1);//vx+2
			so->velocity[1]=- (-cos(so->ii->rotation)*1);//vy+2
		}
		void SceneryObject::effectGlow(Image_Info* from,int lifespan)
		{
			//return;
			if(from==NULL || container==NULL) return;
			SceneryObject* so=new SceneryObject();
			so->relative=from->viewrelative;//false
			so->scaled=false;//true;//viewrel
			so->LoadImg(container->di,"./Img/particle.bmp",0,0);
			so->ii->viewrelative=from->viewrelative;

			//fRECT fr=from->DstRect;
			//fr.left-=10;
			//fr.right+=10;
			//fr.top-=10;
			//fr.bottom+=10;
			//so->ii->DstRect=fr;
			//so->ii->RecalculateCircle();

			if(maskGlow==-1)
				so->ii->mask=Drawing_GetColor(0x7F,0xFF,random(0x3F,0xFF),0x1F);//0x7FFF007F+
			else
				so->ii->mask=maskGlow | Drawing_GetColor(0,random(0,0x1F),random(0,0x1F),random(0,0x1F));

			//so->ii->RotateTo(ii->rotation);
			//so->setCenterRel
			so->setCenterRel(this);
			so->lifespan=lifespan;
			so->canmove=false;//false;
			so->idxAttachedTo=object_index;
			so->ptrAttachedTo=this;
			container->setDepth(container->add(so),0);
		}


        void SceneryObject::soinit()
        {
			requires_boundaries=false;
			inworld=true;
			inscreen=true;

			idxDebugLine=-1;
			movementAngle=0.0f;

			relative=false;
			centered=false;
			scaled=true;
			//slack=500;

            container=NULL;
            type=OT_SCEN;
            VelocStop();
            visible=true;
            randmove=false;
            physical=false;
            canmove=true;
			required=false;
            spin=0;
			hasTrail=false;
			hasGlow=false;
			lifeGlow=10;
			maskTrail=-1;
			maskGlow=-1;

			idxAttachedTo=-1;
			ptrAttachedTo=NULL;

			object_index=-1;

            istep=0;jstep=0;fstep=0;
            ii=NULL;
            lifespan=-1;
            age=0;
        }
        SceneryObject::SceneryObject(){soinit();}
        SceneryObject::SceneryObject(Drawing_Info* di,const char* filename, int transcolor)
        {
            soinit();
            LoadImg(di,filename,transcolor);
			if(ii){
				boundary=Boundary(BOUNDARY_CIRCLE,ii->center,ii->radius);
				//boundary.setCenter(ii->center.x,ii->center.y);
			}
        }
        SceneryObject::~SceneryObject()
        {
			//cout<<"DTOR p="<<this<<" idx="<<object_index;
			//cout<<" ii="<<ii;
			//if(ii) cout<<" img="<<(void*)(ii->image);
			//if(ii && ii->image) cout<<" f="<<ii->image<<endl;

			clearContainerInfo(this);
            UnloadImg();
			if(container!=NULL && object_index!=-1) container->remove(object_index,false);

			//this code ensures that objects being released already cannot be released a second time from ObjectContainer
        }

