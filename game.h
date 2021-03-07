#pragma once
/**
@file game.h
@brief Game logic.

...
@author crashenator (crashenator -at- gmail.com)
*/

#define SOUND_NONE 1

#include "sound.h"
#include "input.h"
#include "drawing.h"
#include "objects.h"
#include "gamestrings.h"
#include "UI.h"
#include "Equipment.h"




class Game
{
	private:
		void setPlayerIdx(int i); 
    public:
		System_Info* si;


		GameStrings strings;
        Faction* fact[FACTIONS];
        ObjectContainer container;
		HUD hud;
		UI ui;

		void* inputInfo;
	


		System_Handle hTimer;
        int refreshRate;//ms
        bool debug;
        bool candraw;
		bool paused;
        int windowchangestate;
        int mode;
        int debug_mode;

		
		bool playersonly;
		int playertime;
        
        
        int asteroids[50];
        
        long int steps;
        
        void GenerateDats();
        void spawnstars();
        void asteroidspawner_step(int n=50);

		void state_toggle(bool& value,const string& name);
        void debug_set(bool value,bool toggle=false);
        void debug_settext(const string c);
        void debug_draw();
        void factions_associate();

		void pause(bool showui=false,bool pause=true);


        void startup();
        void begin(System_Info* sic, System_Hwnd hwnd, RECT& viewport, RECT& arena);
        void step();
        void end(System_Hwnd hwnd);
        void shutdown();
        Game();
};
