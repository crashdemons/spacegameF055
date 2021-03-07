#include "UI.h"
#include <sstream>

#ifndef KEY_DOWN
	#define KEY_DOWN(z) (Input_GetKeyState(container.in,z)&INPUT_KEY_DOWN)
	#define KEY_UP(z) (Input_GetKeyState(container.in,z)&INPUT_KEY_UP)
	#define KEY_PRESSED(z) (Input_GetKeyState(container.in,z)&INPUT_KEY_PRESSED)
	#define KEY_RELEASED(z) (Input_GetKeyState(container.in,z)&INPUT_KEY_RELEASED)
#endif

void UI::Startup()
{

	soCursor.LoadImg(container.di,"./Img/UI/cursor.bmp",0xFFFFFFFF,200);
	soCursor.spin=0.1f;
	soCursor.container=&container;


	soEquipmentUI.LoadImg(container.di,"./Img/UI/equipment.bmp",   0,false);
	soEquipmentUI.CornerMoveTo(50,200);

	soEquipSelect.LoadImg(container.di,"./Img/UI/equip_select.bmp",0,false);
	soEquipSelect.required=true;
	soEquipSelect.CornerMoveTo(67,206);

	soObjectSelect.LoadImg(container.di,"./Img/UI/selection.bmp",0,100);
	soObjectSelect.relative=true;
	soObjectSelect.ii->viewrelative=true;
	//soEquipment.visible=false;
}
bool UI::toggle(UIType t)
{
	int s=(int) show;
	FLAG_TOGGLE(s,t);
	show=(UIType) s;

	if(FLAG_CHECK(s,UI_EQUIPMENT)){
		container.di->fade=0x3F;
		ageFade=101; 
		if(container.objects[player_idx]!=NULL) setShip( (PlayerShip*) container.objects[player_idx] );
	}
	else{ ageFade=0; }


	return FLAG_CHECK(show,t);
}
void UI::setShip(PlayerShip* sh)
{
	shDummy.deathspawn=NULL;
	shDummy.type=OT_NONE;
	shDummy.object_index=-1;
	shDummy.ptrAttachedTo=NULL;
	shDummy.idxAttachedTo=-1;
	shDummy.hasTrail=false;
	shDummy.container=&container;
	shDummy.equipment.list.clear();
	shDummy.LoadImg(container.di,sh->ii->image,sh->ii->transcolor,200);
	shDummy.required=true;
	shDummy.relative=false;
	shDummy.canmove=false;
	shDummy.ii->viewrelative=false;
	shDummy.spin=0.01f;
	shDummy.CornerMoveTo(400,50);
    shDummy.hasGlow=true;
	shDummy.age=0;
}
void UI::setCursorPos(const POINT3& pt)
{
	soCursor.setCenterRel((float) pt.x,(float) pt.y);
}
void UI::Draw()
{
	if(container.selectedObject!=-1){ 
		void* sel=container.objects[container.selectedObject];
		if(sel!=NULL){ soObjectSelect.Draw(container.di); }
	}
	if(FLAG_CHECK(show,UI_EQUIPMENT)) DrawEquipment();
	if(FLAG_CHECK(show,UI_CURSOR   )) soCursor.Draw(container.di);
}
ObjectStats getEquipmentDelta(EquipmentContainer& eq)//UNUSED
{
	ObjectStats dummy;
	dummy.cargo.set(0,INT_MAX);
	dummy.energy.set(0,INT_MAX);
	dummy.fuel.set(0,INT_MAX);
	dummy.health.set(0,INT_MAX);
	dummy.quarters.set(0,INT_MAX);
	dummy.shields.set(0,INT_MAX);
	ObjectStats dummy2=dummy;
	ObjectStats delta;

	return delta;
		
}
void UI::DrawEquipment()
{
    if(container.objects[player_idx]==NULL && (container.types[player_idx]!=OT_PSHP && container.types[player_idx]!=OT_NSHP)) return;
	if(shDummy.ii!=NULL) shDummy.Draw(container.di);

	Ship* sh=(Ship*)container.objects[player_idx];
	EquipmentContainer& equipment=sh->equipment;
	ObjectStats& stats=sh->stats;

	
	TextQuality texold=container.di->text_quality;
	container.di->text_quality=TEXT_BLENDED;

	//print out basic ship statistics text
	static const RECT rSt={80,40,500,140};
	Drawing_DrawTextMultiline(container.di,stats.toString().c_str(),rSt);

	//draw equipment UI background
	soEquipmentUI.Draw(container.di);

	//draw equipment images
	vector<Equipment*>::iterator i=equipment.list.begin();
	int j;
	for(j=0; (j<offsetEquipment && i!=equipment.list.end());  j++,i++);//set the iterator i to the beginning of the current equipment offset.
	for(j=0; (j<4               && i!=equipment.list.end());  j++,i++)//display equipment from offset to end (or 4 items max)
	{

		Equipment& equip=**i;
		soEquipment.LoadImg(container.di,EquipmentImages[equip.subType],0,0);
		soEquipment.CornerMoveTo(51+17+30 + 164*j,226);
		soEquipment.Draw(container.di);
	}

	if(iEquipSelect!=equipment.list.end()){//equipment selected/found - print out statistics about the equipment
		static const RECT rct={150,360,150+400,360+20};
		string sEquipStats="";
		if((*iEquipSelect)->capacity!=0) sEquipStats+="Capacity: "+itos((*iEquipSelect)->capacity)+"\n";
		sEquipStats+=(*iEquipSelect)->getProductionInfo(stats)+"\n";
		if((*iEquipSelect)->canEnable) sEquipStats+="Click to toggle on/off. ";
		sEquipStats+="Press 'E' to Eject\n";
		sEquipSelect+="\n"+sEquipStats;
		Drawing_DrawTextMultiline(container.di,sEquipSelect.c_str(),rct);
		soEquipSelect.Draw(container.di);//draw the equipment selector image on top of the equipment (transparent)
	}
	container.di->text_quality=texold;
}

void UI::Debug()
{
	soEquipmentUI.DrawRect(container.di);
	soEquipSelect.DrawRect(container.di);
	Drawing_DrawRect(container.di, Drawing_GetFRECT(rct), 0xFFFF0000,false,1.0f);
}

void UI::Step()
{
	if(ageFade<=50) ageFade++;
	if(ageFade==50) container.di->fade=0xFF;


	if(container.selectedObject!=-1){ 
		void* sel=container.objects[container.selectedObject];
		if(sel!=NULL){ soObjectSelect.setCenterRel((SceneryObject*)sel); }
	}
	
	if(FLAG_CHECK(show,UI_CURSOR   )) soCursor.doStep(-1);
	if(FLAG_CHECK(show,UI_EQUIPMENT)) StepEquipment();

	soEquipSelect.inscreen=true;
}
void UI::StepEquipment()
{
	if(shDummy.ii!=NULL) shDummy.doStep(-1);
	soEquipSelect.doStep(-1);
	if(container.objects[player_idx]==NULL && (container.types[player_idx]!=OT_PSHP && container.types[player_idx]!=OT_NSHP)){
		soEquipmentUI.visible=false;
		if(FLAG_CHECK(show,UI_EQUIPMENT)) toggle(UI_EQUIPMENT);//turn this off
		return;
	}
	soEquipmentUI.visible=true;
    Ship* sh=(Ship*)container.objects[player_idx];
	EquipmentContainer& equipment=sh->equipment;
	ObjectStats& stats=sh->stats;



	
	//INPUT
	POINT3 pt=Input_GetMousePos(container.in);
	int leftB=50;
	int left=50+18;
	int right=left+658;
	int rightB=right+12;
	
	if(pt.y>=200 && pt.y<=358)
	{
		rct.top=200; rct.bottom=358+2;
		rct.left=leftB; rct.right=rightB;
		if(pt.x>=50 && pt.x<=left)
		{
			rct.left=50; rct.right=left;
			if(KEY_PRESSED(SDL_BUTTON_LEFT)){
				offsetEquipment--;
				if(offsetEquipment<0) offsetEquipment=0;
			}
		}
		else if(pt.x>=right && pt.x<=rightB)
		{
			rct.left=right; rct.right=rightB;
			if(KEY_PRESSED(VK_LBUTTON)){
				offsetEquipment++;
			}
		}
		else if(pt.x>(left+5) && pt.x<(right-5))
		{
			rct.left=left; rct.right=right;
			pt.x-=left;
			offsetEquipSelect=(pt.x)/164;
			if(KEY_PRESSED(SDL_BUTTON_LEFT) || KEY_PRESSED(SDLK_RETURN)) toggleEquipSelect=true;
		}
	}
	if(KEY_PRESSED('e')  && soEquipmentUI.visible) notifyEquipRemove=true;// !!! NOTE: PRESSED/RELEASED tenses are saved until cleared, and can saved from before the UI opens
	




	//Equipment information
	int maxIdx=(int)equipment.list.size()-1;
	int maxOffset=0;
	if(maxIdx>=3) maxOffset=maxIdx-3;
	if(offsetEquipment>maxOffset) offsetEquipment=maxOffset;
	idxEquipSelect=offsetEquipment+offsetEquipSelect;

	soEquipSelectDestinationX=51+17 + 164*offsetEquipSelect;



	double angle;
	if(soEquipSelect.ii->DstRect.left > soEquipSelectDestinationX) angle=DEGRAD*90.0f;
	else angle=-DEGRAD*90.0f;
	if(abs(soEquipSelectDestinationX-soEquipSelect.ii->DstRect.left)<20) soEquipSelect.Deccelerate(2.0);
	if(soEquipSelect.ii->DstRect.left<=67){
		soEquipSelect.CornerMoveTo(67,206);
	}
	if(soEquipSelect.ii->DstRect.right>=726){
		soEquipSelect.CornerMoveTo(726-164,206);
	}
	soEquipSelect.AccelerateRel(0.5,angle);



	

	sEquipSelect=string(" ")+itos(idxEquipSelect);//space because of character cutoff issue (drawing rect is left-aligned, this shouldn't happen.)
	sEquipSelect+=": ";

	bool itemFound=false;
	iEquipSelect=equipment.list.end();
	vector<Equipment*>::iterator i=equipment.list.begin();
	int j;
	for(j=0; (j<offsetEquipment && i!=equipment.list.end());  j++,i++);//set the iterator i to the beginning of the current equipment offset.
	for(j=0; (j<4               && i!=equipment.list.end());  j++,i++)//get equipment from offset to end (or 4 items max)
	{

		Equipment& equip=**i;
		if((offsetEquipment+j)==idxEquipSelect){
			itemFound=true;
			iEquipSelect=i;
			sEquipSelect+=equip.name;
			sEquipSelect+=" v";
			sEquipSelect+=itos(equip.level+1);
			if(equip.canEnable)
			{
				if( equip.online ) sEquipSelect+=" [online]";
				else               sEquipSelect+=" [offline]";
			}
		}
	}
	if(!itemFound) sEquipSelect+="Empty Cargo";
	else{
		if(toggleEquipSelect && (*iEquipSelect)->canEnable){
			if( (*iEquipSelect)->online ) (*iEquipSelect)->disable(stats);
			else                          (*iEquipSelect)-> enable(stats);
		}
	}
	if(notifyEquipRemove) { notifyEquipRemove=false; equipment.remove(stats, iEquipSelect); }
	if(toggleEquipSelect) toggleEquipSelect=false;
}

UI::UI(ObjectContainer& cont) : container(cont), offsetEquipment(0), offsetEquipSelect(0), toggleEquipSelect(false), notifyEquipRemove(false)
{
	ageFade=0;
	show=UI_CURSOR;
}

UI::~UI(void)
{

	OutputDebugString("~UI\n");
	shDummy.UnloadImg();
	OutputDebugString("~UI\n");


}

void Calculate()
{
}


void HUD::Startup()
{
	barX.LoadImg(container.di,"./Img/HUD/bar.bmp"     ,0x00000000,false);
	barX.container=&container;
	barX.canmove=false;
	barX.required=true;
    barC.LoadImg(container.di,"./Img/HUD/bar_cont.bmp",0xFF000000,false);
    barE.LoadImg(container.di,"./Img/HUD/bar_end.bmp" ,0xFF000000,false);
    barB.LoadImg(container.di,"./Img/HUD/bar_sep.bmp" ,0xFF000000,false);
    bar2.LoadImg(container.di,"./Img/HUD/bar_end2.bmp",0xFF000000,false);

}
void HUD::Step(){ 
	barX.doStep(-1);
}
void HUD::drawStats()
{
    if(container.objects[player_idx]!=NULL)
    {
        if(container.types[player_idx]==OT_PSHP || container.types[player_idx]==OT_NSHP){
            Ship* sh=(Ship*)container.objects[player_idx];
			//if(player_last==NULL) player_last=sh;
            
            float xd=10,yd=20,w=10;
			/*
            float energy=sh->energy;
            float energymax=sh->energymax;if(energy>energymax) energymax=energy;
            float health=sh->health;
            float healthmax=sh->healthmax;
            float shields=0;if(health>healthmax){ shields=health-healthmax; health=healthmax; }
            float shieldmax=sh->shield_max;if(shields>shieldmax) shieldmax=shields;

            if(health<0) health=0;
            if(energy<0) energy=0;
			*/
			ObjectStats& stats=sh->stats;
            
            
            float ht_health,ht_shield,ht_energy;
			ht_health=limit((float)stats.health.maximum,10,200);
			ht_shield=limit((float)stats.shields.maximum,10,200);
			ht_energy=limit((float)stats.energy.maximum,10,200);
            if(stats.shields.empty()) ht_shield=0.0f;
			if(stats.energy.empty()) ht_energy=0.0f;


            

			if(stats.health.current==9999)
			{
				float xyz=stats.health.percent();
				xyz=stats.health.percent();

			}
			fRECT rHeal(xd,xd+w,yd,yd+stats.health.percent()*ht_health);
            fRECT rHEnd2(xd,xd+w,rHeal.bottom,rHeal.bottom+4);
            
            fRECT rCont(xd-3,xd+w+3,yd,yd+ht_health+ht_shield);
            if(!stats.shields.empty()) rCont.bottom+=3;
            yd+=ht_health;
            fRECT lCont(xd-3,xd+w+3,yd,yd+3);
			fRECT rShld(xd,xd+w,yd+3,yd+stats.shields.percent()*ht_shield+3);
            fRECT rSEnd2(xd,xd+w,rShld.bottom,rShld.bottom+4);
            
            yd+=ht_shield+20;
			fRECT rEner(xd,xd+w,yd,yd+stats.energy.percent()*ht_energy);
			fRECT rECnt(xd-3,xd+w+3,yd,yd+ht_energy);
            fRECT rEEnd2(xd,xd+w,rEner.bottom,rEner.bottom+4);
            
            
            //bars
			int flash_sequence=40;
			int flash_change=20;
			float flash_statmin=0.40f;


            barX.ii->DstRect=rHeal;
            barX.ii->mask=0xFFFF0000;
			if(stats.health.percent()<flash_statmin){ if(barX.age%flash_sequence>flash_change) barX.ii->mask|=0xFF7F7F7F; }
			barX.Draw(container.di); 
            if(!stats.health.full() && !stats.health.empty()){
                memcpy(&(bar2.ii->DstRect),&rHEnd2,sizeof(fRECT));//bar end
                bar2.ii->mask=barX.ii->mask;bar2.Draw(container.di);
            }

            barX.ii->DstRect=rShld;
            barX.ii->mask=0xFF7F7FFF;
			if(stats.shields.percent()<flash_statmin){ if(barX.age%flash_sequence>flash_change) barX.ii->mask^=0x000000FF; }
			barX.Draw(container.di);
            if(!stats.shields.full() && !stats.shields.empty()){
                bar2.ii->DstRect=rSEnd2;//bar end
                bar2.ii->mask=barX.ii->mask;bar2.Draw(container.di);
            }

			
			barX.ii->DstRect=rEner;

			barX.ii->mask=0xFFFF7F00;
			if(stats.energy.percent()<flash_statmin){ if(barX.age%flash_sequence>flash_change) barX.ii->mask|=0xFF7F7FFF; }
			barX.Draw(container.di);
			if(!stats.energy.full() && !stats.energy.empty()){
				bar2.ii->DstRect=rEEnd2;//bar end
				bar2.ii->mask=barX.ii->mask;bar2.Draw(container.di);
			}
			
			
            //end of bars
            
            
            barC.ii->DstRect=rCont;
            barC.Draw(container.di);

			barC.ii->DstRect=rECnt;
            barC.Draw(container.di);
            
			barE.ii->DstRect=rCont;
            barE.ii->DstRect.top=barE.ii->DstRect.bottom;
            barE.ii->DstRect.bottom+=3;
            barE.Draw(container.di);

			barE.ii->DstRect=rCont;
            barE.ii->DstRect.bottom=barE.ii->DstRect.top -3;
            barE.ii->DstRect.top+=3 -3;
            barE.Draw(container.di);
            

			if(stats.energy.maximum>0){
				barE.ii->DstRect=rECnt;
				barE.ii->DstRect.top=barE.ii->DstRect.bottom;
				barE.ii->DstRect.bottom+=3;
				barE.Draw(container.di);

				barE.ii->DstRect=rECnt;
				barE.ii->DstRect.bottom=barE.ii->DstRect.top -3;
				barE.ii->DstRect.top+=3 -3;
				barE.Draw(container.di);
			}

            
			barB.ii->DstRect=lCont;
            if(!stats.shields.empty()) barB.Draw(container.di);
            
            
            
			lastPlayerStats=stats;
            setMessage(strings.getWarningString(stats,lastPlayerStats));
        }
    }
}


void HUD::drawMessage()
{
	if(message.length())
    {
        //DrawText(container.di->hdc,sdebug.c_str(),-1,&(container.di->rect),DT_TOP);
		//Drawing_DrawRectFilled(container.di, Drawing_GetFRECT(container.di->rect), 0xFFFFFFFF);
		Drawing_DrawText(container.di,message.c_str(),container.di->rect);
        if((steps-text_time)>60){message="";}
    }
}
void HUD::drawMinimap()///Draw minimap, INEFFICIENT!
{
	float iSize=150;
	float fSize=iSize;
	float world_w=(container.di->world.right-container.di->world.left)+1;
	float world_h=(container.di->world.bottom-container.di->world.top)+1;
	float fw=fSize / world_w;
	float fh=fSize / world_h;
	fRECT frctMap(container.di->width-(iSize+1),container.di->width-1,1,iSize+1);
	Drawing_DrawRectFilled(container.di,frctMap,0xAF000000,false);
	Drawing_DrawRect(container.di,frctMap,0xAF00FF00,false);


	ObjectFilter of;
	ReputationFilter rf;

	of.setMatchOnly(OT_SHIP);
	of.setMatch(OT_NSHP);
	of.setMatch(OT_PSHP);
	//of.setMatch(OT_PROJ);
	rf.setMatch(NEUTRAL,true,true);//third parameter = match ALL.

	fPOINT pt;
	vector<int> objects=container.getFiltered(&of,&rf);
	for(vector<int>::iterator i=objects.begin(); i!=objects.end(); i++){
		SceneryObject* so=(SceneryObject*) container.objects[*i];
		pt.x=so->ii->DstRect.left-container.di->world.left;
		pt.y=so->ii->DstRect.top-container.di->world.top;
		pt.x*=fw;
		pt.x+=frctMap.left;
		pt.y*=fh;


		//Drawing_DrawLine(container.di,0,0,pt.x,pt.y,0xFFFF0000,false);
		//Drawing_DrawPoint(container.di, pt,0xFFFF0000);
		fRECT frct(pt.x-1,pt.x+1.0f,pt.y-1,pt.y+1);

		int color=0x0000FF;
		//if(container.types[*i]==OT_PROJ) color=0x1F0000;
		if(container.types[*i]==OT_NSHP) color=0xFF0000;
		if(container.types[*i]==OT_PSHP) color=0x00FF00;

		Drawing_DrawRect(container.di,frct,color|0xFF000000,false);
	}

}

void HUD::setMessage(const string& s)
{
	if(s.length()==0) return;
    text_time=steps;
    message=s;
}

void HUD::Draw()
{
	drawStats();
	drawMessage();
	drawMinimap();
}

HUD::HUD(ObjectContainer& cont,GameStrings& gstrings, long int& isteps) : container(cont), strings(gstrings), steps(isteps)
{
	//player_last=new PlayerShip();
   // player_last->ii=NULL;//don't release that pointer.
}
HUD::~HUD(void){
}



