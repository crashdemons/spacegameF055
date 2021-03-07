#include "objects_Explosion.h"

Explosion::Explosion(){
	type=OT_EXPL;canmove=true; relative=true;
	hasTrail=true; hasGlow=false; 
	maskTrail=Drawing_GetColor(0x7F,random(0xBF,0xFF),random(0x3F,0x7F),0);
	//maskTrail=0x7FFF3F1F;
	maskGlow= 0x3FFF7F1F;
}
void Explosion::save(Image_Info* ix)
{
	//svRotation=ix->rotation;
    svRotation=frandom(-PI2,PI2);
	spin=0.0f;//frandom(0,PI);
	
	svDst=ix->DstRect;
	svCenter=ix->center;
    
    float w=svDst.right - svDst.left;
    float h=svDst.bottom - svDst.top;
    float dw=(w+29)/2 - w;
    float dh=(h+26)/2 - h;
    svDst.left-=dw/2;
    svDst.right+=dw/2;
    svDst.top-=dh/2;
    svDst.bottom+=dh/2;
    lifespan=16;
	restore();
}
void Explosion::restore()
{
    if(ii==NULL) return;
	//ii->DstRect=svDst;
	ii->mask=Drawing_GetColor(0x7F,0xFF,0xFF,0xFF);
	ii->CornerMoveTo(svDst.left,svDst.top);//ii->center=svCenter;
	ii->Rotate(svRotation);
	//ii->RecalculateCircle();
    //memcpy(&(ii->DstRect),&svDst   ,sizeof(fRECT));
    //memcpy(&(ii->center) ,&svCenter,sizeof(Vector2));
}
void Explosion::update()
{
	if(ii==NULL) return;
	svDst=ii->DstRect;
	//svCenter=ii->center;
//	svRotation=ii->rotation;
}
void Explosion::doStep(int index)
{
	void* oldii=ii;
    if(istep==0) LoadImg(container->di,"./Img/ex1.bmp", 0xFFFFFF);
    if(istep==4) LoadImg(container->di,"./Img/ex2.bmp", 0xFFFFFF);
    if(istep==8) LoadImg(container->di,"./Img/ex3.bmp", 0xFFFFFF);
	bool didReset=((istep%4)==0);
	if(didReset) restore();
	

    istep++;
    SceneryObject::doStep(index);
	update();//save movement updates to DestRect so we can carry-over from next LoadImg
}