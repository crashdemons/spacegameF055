/* TODO (#1#): add game sounds/audio */
/* TODO (#1#): add system changing/reloading by going off-screen or 
               warping. */
/* TODO (#1#): add galaxy map interfaces. */
#include "game.h"

#ifndef INPUT_MACROS
	#define INPUT_MACROS
	#define KEYSTATE(y,z) (Input_GetKeyState(container.in,z)&y)==y
	#define KEYSTATEEX(x,y,z) (Input_GetExplicitKeyState(container.in,z,x)&y)==y
	#define MOUSE_DOWN(y) KEYSTATEEX(INPUT_KEYS_MOUSE,INPUT_KEY_DOWN,y)
	#define MOUSE_UP(y) KEYSTATEEX(INPUT_KEYS_MOUSE,INPUT_KEY_UP,y)
	#define MOUSE_PRESSED(y) KEYSTATEEX(INPUT_KEYS_MOUSE,INPUT_KEY_PRESSED,y)
	#define MOUSE_RELEASED(y) KEYSTATEEX(INPUT_KEYS_MOUSE,INPUT_KEY_RELEASED,y)

	#define KEY_DOWN(y) KEYSTATE(INPUT_KEY_DOWN,y)
	#define KEY_UP(y) KEYSTATE(INPUT_KEY_UP,y)
	#define KEY_PRESSED(y) KEYSTATE(INPUT_KEY_PRESSED,y)
	#define KEY_RELEASED(y) KEYSTATE(INPUT_KEY_RELEASED,y)
#endif

Boundary point1;
Boundary poly2;

Game::Game() : ui(container), hud(container,strings,steps)
{
    
    refreshRate=22;//ms
    debug=false;
    candraw=false;
	paused=false;
	playersonly=false;
    windowchangestate=0;
    mode=0;
    debug_mode=2;
	playertime=1;
    
    steps=0;
	hTimer=NULL;
	si=NULL;


	vector<fPOINT> points;
	points.push_back(fPOINT(10,15));
	point1=Boundary(BOUNDARY_CIRCLE,fPOINT(10,15),20);

	vector<fPOINT> points2;
	points2.push_back(fPOINT(10,10));
	points2.push_back(fPOINT(20,10));
	points2.push_back(fPOINT(20,20));
	points2.push_back(fPOINT(10,20));
	poly2=Boundary(BOUNDARY_POLY,points2);
	poly2.move(100,0);

}
void Game::setPlayerIdx(int i)
{
	hud.player_idx=i;
	ui.player_idx=i;
}
void Game::GenerateDats()
{
    /*
    Projectile_Type Projectile_Plasma = {50,"./Img/plasma.bmp",0xFFFFFF,0,true,0.3,1,5, false,true,true,true,true,0,0};
    Projectile_Type Projectile_PlasmaBomb = {50,"./Img/plasmab.bmp",0xFFFFFF,10*DEGRAD,true,0.5,1,5, false,true,true,true,true,&container.Projectile_Plasma,10,-50,-5};
    Projectile_Type Projectile_Mine = {-1,"./Img/mine.bmp",0xFFFFFF,0,true,0.3,1,80, false,true,false,true,true,0,0};
    Projectile_Type Projectile_MiningLaser = {0,"./Img/mining_laser.bmp",0xFFFFFF,0,false,0.1,-1,2,false,false,true,false,true,0,0};
    Projectile_Type Projectile_Asteroid = {-1,"./Img/as1.bmp",0xFFFFFF,DEGRAD,true,0.5,35,10,false,false,true,true,true,0,0,10,-3};
    Projectile_Type Projectile_TractorBeam = {0,"./Img/tractor_beam.bmp",0xFF000000,0,false,-0.1,-1,0,false,false,true,false,false,0,0,0,0};       

    saveToFile("./Dat/Plasma.projectile",&Projectile_Plasma,sizeof(Projectile_Type));
    saveToFile("./Dat/PlasmaBomb.projectile",&Projectile_PlasmaBomb,sizeof(Projectile_Type));
    saveToFile("./Dat/Mine.projectile",&Projectile_Mine,sizeof(Projectile_Type));
    saveToFile("./Dat/MiningLaser.projectile",&Projectile_MiningLaser,sizeof(Projectile_Type));
    saveToFile("./Dat/Asteroid.projectile",&Projectile_Asteroid,sizeof(Projectile_Type));
    saveToFile("./Dat/TractorBeam.projectile",&Projectile_TractorBeam,sizeof(Projectile_Type));
    */

}
void Game::startup()
{
    for(int i=0;i<50;i++) asteroids[i]=-1;
    factions_associate();
    container.clear(false);
}
void Game::begin(System_Info* sic, System_Hwnd hwnd, RECT& viewport, RECT& arena1)
{
	si=sic;

	container.in=Input_Startup(sic);
	container.si=Sound_Startup();

	//Sound_SetMusic(container.si,"./Snd/test.wav",3000);
	//SDL_Delay(5000);




	POINT vpdim=Dim(viewport);
	fPOINT vpcenter=Drawing_GetCenter(viewport);

	fRECT arena=Drawing_GetFRECT(arena1);
	Drawing_CenterOn(arena, vpcenter);//byref modification

	container.di = Drawing_Startup(sic,vpdim.x,vpdim.y,1.0 ,true);
	container.di->world=Drawing_ApproxRECT(arena);

	ui.Startup();
	hud.Startup();


	
	


    candraw=true;


    
    spawnstars();


	Projectile* planet=new Projectile();//new SceneryObject(container.di,"./Img/circle.bmp",0);
	planet->LoadImg(container.di,"./Img/circle.bmp",0xFFFFFFFF);
	planet->immune=true;
	planet->canmove=false;
	planet->hasgravity=true;
	planet->gravity=0.08f;
	planet->killsProjectiles=false;
	planet->spin=0.00125f;
	planet->bumpforce=0.0f;
	planet->physical=true;
	planet->solid=false;
	planet->collisDmg=0;
	planet->ii->radius*=2.0f;
	////planet->ii->mask=0xFFFF0000;
	container.setDepth(container.add(planet),2);

	


    
    PlayerShip* Ship1=new PlayerShip();//this is fine, ObjectContainer deletes objects when cleaning up.
    
	Ship1->centered=true;
    Ship1->LoadImg(container.di,"./Img/miner.bmp",0xFFFFFF);
    Ship1->setCenterRel(400,300);

	
	OutputDebugString(Q(EQUIPMENT(REACTOR,FUSION)));
	Ship1->equipment.add(EQUIPMENT(REACTOR,FUSION)).levelUp(Ship1->stats,2);
	Ship1->equipment.add(EQUIPMENT(GENERATOR,SHIELD));
	Ship1->equipment.add(EQUIPMENT(STORAGE,SHIELD));
	Ship1->equipment.add(EQUIPMENT(STORAGE,SHIELD));
	Ship1->equipment.startup(Ship1->stats);

	
	


	

    
    NPCShip* Ship2=new NPCShip(fact[FFED]);
	
    Ship2->LoadImg(container.di,"./Img/fighter.bmp",0xFFFFFF);
    Ship2->Move(200,200);
    Ship2->object_target=0;
    Ship2->rof*=3;


	container.add(Ship2);
    


    Ship2=new NPCShip(fact[FFED]);
    Ship2->LoadImg(container.di,"./Img/fighter.bmp",0xFFFFFF);
    Ship2->Move(500,200);
    Ship2->object_target=0;
    Ship2->rof*=3;
    container.add(Ship2);
	Ship2->equipment.add(EQUIPMENT(REACTOR,ANTIMATTER));
	Ship2->equipment.add(EQUIPMENT(GENERATOR,SHIELD));
	Ship2->equipment.startup(Ship2->stats);

	

	int player_idx=container.add(Ship1);
    setPlayerIdx(player_idx);
	container.setDepth(player_idx,OD_TOP);
    
    
	

    /*
    SceneryObject* ms= new SceneryObject(container.di,"./Img/map_system.bmp",0xFFFFFF);
    ms->setCenterRel(20,20);
    object_queue_add(ms);
    SceneryObject* ml= new SceneryObject(container.di,"./Img/map_link.bmp",0xFFFFFF);
    ml->setCenterRel(53,20);
    object_queue_add(ml);
    */

   hTimer=System_TimerCreate(si,hwnd,refreshRate);
   //Timer=SetTimer(hwnd,1,refreshRate,NULL);

}

void Game::spawnstars()
{
    SceneryObject* star;
	float rot=frandom(0,PI2);//-PI2 to PI2?
    for(int i=0;i<100;i++)
    {
        char name[50]="./Img/stars/st1.bmp";
		int num=1;//random(1,3);

		float maxmult=0.25f;
		float dist=frandom(0.0f,0.333333f*maxmult);
		if( random(1,100)>75 ){ num=4; dist=frandom(0.25f*maxmult,0.666666f*maxmult); }
		if( random(1,100)>95 ){ num=7; dist=frandom(0.5f*maxmult,        maxmult); }//random(7,9)

        
        name[14]=num+0x30;
        star=new SceneryObject();
		star->scaled=true;
        star->LoadImg(container.di,name,0xFF000000);

		if(random(0,100)<50){// rpughly half of all stars are white, the other half undergo random selection.  this creates many more white stars.
			star->ii->mask=0xFFFFFFFF;
		}else{
			int r=random(0x3F,0xFF);
			int b=random(0x3F,0xFF);
			int g=random(min(r,b),max(r,b));
			star->ii->mask=Drawing_GetColor(0xFF, r, g, b);
		}
        //star->Move( random(-32,container.di->rect.right), random(-32,container.di->rect.bottom));
        star->Move( random(-32,container.di->rect.right), random(-32,container.di->rect.bottom));
        star->ii->rotation=rot;// glare lines on stars are parallel
		star->ii->viewrelative=true;
		star->ii->relativemult=dist;
		container.add(star,2);
        //object_queue_add(star);
    }
}
void Game::asteroidspawner_step(int n)
{
    Debris* asteroid;
    if(n>50) n=50;
    for(int i=0;i<n;i++)
    {
        bool spawn=(asteroids[i]<0);
        if(!spawn) spawn=(container.objects[asteroids[i]]==NULL);
        if(spawn)
        {
            int num=rand()%3+1;
            asteroid=new Debris();
            Projectile_Type pt=container.Projectile_Asteroid;
			pt.image[8]=(char) (num+0x30);
			asteroid->set(&container,pt);
			asteroid->ii->image=string(pt.image).c_str();
            
            switch(random(1,4))
            {
                case 1: //left
                    asteroid->Move(random(-500,-50),random(0,container.di->rect.bottom));
                    asteroid->velocity[0]=1;
                    asteroid->velocity[1]=frandom(-1,1);
                    break;
                case 2: //right
                    asteroid->Move(random(container.di->rect.right+50,container.di->rect.right+500),random(0,container.di->rect.bottom));
                    asteroid->velocity[0]=-1;
                    asteroid->velocity[1]=frandom(-1,1);
                    break;
                case 3: //top 
                    asteroid->Move(random(0,container.di->rect.right),random(-500,-50));
                    asteroid->velocity[0]=frandom(-1,1);
                    asteroid->velocity[1]=1;
                    break;
                case 4: // bottom
                    asteroid->Move(random(0,container.di->rect.right),random(container.di->rect.bottom+50,container.di->rect.bottom+500));
                    asteroid->velocity[0]=frandom(-1,1);
                    asteroid->velocity[1]=-1;
                    break;
            }
            asteroid->deathspawn_n=num-1;
            asteroid->deathspawn=&container.Projectile_Asteroid;
			asteroid->stats.health.current*=num;
            asteroid->collisDmg*=num;
            asteroid->bumpforce*=num;
            asteroids[i]=container.add(asteroid);//object_queue_add(asteroid);
        }
    }
}
void Game::pause(bool showui,bool pause)
{
	System_GUILockMouse(si, container.di->hWnd,!pause);
	if(showui) paused=ui.toggle(UI_EQUIPMENT);
	else state_toggle(paused,"Game Pause");
}
void Game::step()
{
    steps++;
	Input_Poll(container.in);
	Input_PovToMouseDelta(container.in,10);
	Input_LimitMouseCoords(container.in,container.di->rect);
	POINT3 pt3=Input_GetMousePos(container.in);
	
	ui.setCursorPos(pt3);
	
    
	if(KEY_RELEASED(VK_ESCAPE)) System_GUIQuit(si,si->lastHwnd);
	if(MOUSE_PRESSED(SDL_BUTTON_RIGHT)) container.selectFromCursor(&ui.soCursor);
    if(KEY_PRESSED('p') || KEY_PRESSED(VK_PAUSE)) pause(false,!paused);
    if(KEY_PRESSED('1')){debug_set(false,true);}
    if(KEY_PRESSED('2')){debug=true;debug_mode++;}
    if(KEY_PRESSED('d')) GenerateDats();
	if(KEY_PRESSED('c') || KEY_PRESSED('i')){ pause(true,!paused); }
	if(KEY_PRESSED('0')){
		container.di->centering=!(container.di->centering);
		if(!(container.di->centering)){
			debug_settext("View Center-locked");
			container.di->viewport.x=0;
			container.di->viewport.y=0;



		}else debug_settext("View Sprite-centered");
	}
	if(KEY_PRESSED('l')) state_toggle(playersonly,"PlayersOnly");

	//Input_Clear(container.in);
    
    if(mode==0 && candraw){
		if(paused){
				//FIXME: cursor centered-mode - no support for view-scrolling
				//FIXME: permanently breaks the cursor when returning to sprite-centered, set back when disabled
			/*
				ui.soCursor.relative=true;//move to UI func?
				ui.soCursor.ii->viewcentered=true;
				ui.soCursor.ii->viewrelative=true;
				ui.soCursor.visible=false;
				Drawing_CenterViewOn(container.di,ui.soCursor.ii);*/
				//Drawing_CenterViewAdjust(Drawing_Info* di, float dx,float dy) - use for screen-edge scrolling.
		}else{
			/*
			ui.soCursor.relative=false;
			ui.soCursor.ii->viewcentered=false;
			ui.soCursor.ii->viewrelative=false;
			ui.soCursor.visible=true;
			*/



			asteroidspawner_step(25);

			if(playersonly){//pause time except for the player
				container.stepType(OT_PSHP);
				//container.stepType(OT_SCEN);// would allow cleaning up visual effects objects
				//container.stepType(OT_EXPL);
				

			}
			else if(playertime>1)//slow time except for player - not implemented correctly.
			{
				container.step(ui.player_idx);
				if(steps%playertime==0) container.stepType(OT_PSHP);//ui.player_idx);
			}else container.step();//normal step for everything.
		}
		hud.Step();
		ui.Step();


		
         Drawing_Begin(container.di);
         container.draw(container.di);
		 hud.Draw();
		 ui.Draw();

		 RECT rctDisc={300,20,900,40};
		 Drawing_DrawText(container.di, "Alpha version - Do Not Reproduce or Distribute.", rctDisc,0,0xFFFF0000);

		 
		 RECT rctSel={300,20+50,900,40+50};
		 Drawing_DrawText(container.di, container.getSelectedInfo().c_str(), rctSel,0,0xFFFFFFFF);




		if(debug){

			debug_draw();

			char buf[256]={0};
			//ui.soCursor.ii->DstRect
//			fRECT frt=ui.soCursor.ii->DstRect;
			//ui.soCursor.ii->DstRect.left
			sprintf(buf,"vp(%.0f,%.0f), ms(%d,%d,%d) mw(%.0f,%.0f) obj(%d:S/%d:W/%d:X)",
				container.di->viewport.x,container.di->viewport.y,
				pt3.x,pt3.y,pt3.z,
				pt3.x+container.di->viewport.x,pt3.y+container.di->viewport.y,
				container.countScreen(),container.count,OBJECTS_MAX
			);
			debug_settext(string(buf));






		}
        Drawing_End(container.di);
        Drawing_Present(container.di);
        
    }
    
}
void Game::state_toggle(bool& value,const string& name)
{
	value=!value;
	string nf="OFF"; if(value) nf="ON";
	debug_settext(name+": "+nf);
}
void Game::debug_set(bool value,bool toggle)
{
    if(toggle) debug=!debug;
    else debug=value;
}
void Game::debug_draw()
{
	container.draw_debug(container.di,debug_mode);

	int x1=container.di->viewport.x;
	int y1=container.di->viewport.y;
	int x2=x1+container.di->width/2;
	int y2=y1+container.di->height/2;
	//Sound_ClipPlay(container->si,"./Snd/boom.wav",fabs(distTo(x2,y2)),1000);
	
	Drawing_DrawLine(container.di,x1,y1,x2,y2,0xFFFF0000);

	POINT3 pt3=Input_GetMousePos(container.in);
	fPOINT fpt;
	fpt.x=(float)pt3.x;
	fpt.y=(float)pt3.y;
	fpt.x+=container.di->viewport.x;
	fpt.y+=container.di->viewport.y;
	fRECT fr;
	fr.left=0; fr.right=fpt.x;
	fr.top=0; fr.bottom=fpt.y;
	Drawing_DrawLine(container.di, fr, 0xFF00FF00);


	Drawing_DrawRect(container.di, Drawing_GetFRECT(container.di->world), 0xFFFF0000);
	Drawing_DrawRect(container.di, Drawing_GetFRECT(container.di->rect), 0xFFFFFFFF,false);
			
	ui.Debug();

	point1.move(1,0);
	point1.draw(container.di);
	poly2.move(1,0);
	poly2.rotate(0.1);
	poly2.draw(container.di,false);
	//if(point1.collide(poly2)) System_Alert("bob");

}
void Game::debug_settext(const string s)
{
	hud.setMessage(s);
}
void Game::end(System_Hwnd hwnd)
{
	if(hwnd==NULL) hwnd=si->lastHwnd;
	System_TimerDestroy(si,hwnd,hTimer);
    //KillTimer(hwnd,Timer);
    candraw=false;
}
void Game::shutdown()
{
    container.clear(true);
    Drawing_Shutdown(container.di);
	Input_Shutdown(container.in);
	Sound_Shutdown(container.si);
}

void Game::factions_associate()
{
    for(int i=0;i<FACTIONS;i++){fact[i]=new Faction(i);}
    
    fact[FFED]->Associate(fact[FREBEL],ENEMY);
    //fFed.Associate(&fAlien,NOT_FRIENDLY);// ???
    fact[FFED]->Associate(fact[FPIRATE],CRIMINAL);
    fact[FFED]->Associate(fact[FFEDTRADE],EMPLOYEE);
    fact[FFED]->Associate(fact[FREBTRADE],ENEMY);
    
    //fRebel.Associate(&fAlien,FRIENDLY);// ??? who knows!?
    //fRebel.Associate(&fPirate,CRIMINAL);// ??? are pirates criminals to the rebels or are they friendly!?
    fact[FREBEL]->Associate(fact[FREBTRADE],EMPLOYEE);
    fact[FREBEL]->Associate(fact[FFEDTRADE],ENEMY);
    fact[FREBEL]->Associate(fact[FFREETRADE],FRIENDLY);
    //fRebel.Associate(&fBHunter,FRIENDLY); ???
    
    fact[FPIRATE]->Associate(fact[FBHUNTER],EMPLOYEE);// ??? maybe this should just be friendly
    fact[FPIRATE]->Associate(fact[FFREETRADE],FRIENDLY);
    
    fact[FFEDTRADE]->Associate(fact[FREBTRADE],CRIMINAL);
    fact[FFREETRADE]->Associate(fact[FFEDTRADE],FRIENDLY);
    fact[FFREETRADE]->Associate(fact[FREBTRADE],FRIENDLY);
    
    //overpowering Federation, medium Rebel resistance, both with trade factions.
    //Pirates raid trade factions and sell items on the black market - sometimes hiring bounty hunters.
    //so-called "Free Trade" factions trade both with Federation and Rebels, and have some connections to the black market.

    //other ideas: unique ships, hail text, commander names for unique ships, tracking/placement for unique ships instead of random.
    // 

}
