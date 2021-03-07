
int threads=0;
bool lockgame=false;
#include "game.h"

Game* game=new Game();
int StateUpdate(System_Info*,SYSTEMSTATE, System_Hwnd);

SYSTEM_MAIN()
{
	
	System_Info* si=SYSTEM_STARTUP();
	si->StateUpdate=StateUpdate;

	
	System_DebugOutput("test");



	int viewport_w=1024;
	int viewport_h=768;

	int arena_wh=4096;

    game->startup();

	System_Hwnd hwnd=System_GUICreate(si,"GG2",viewport_w,viewport_h);

	System_GUIShow(si,hwnd);

	//while(GetForegroundWindow()!=(si->hwnd)) Sleep(300); //Not system.h implemented yet.

	RECT viewport={0,0,viewport_w,viewport_h};
	RECT arena={0,0,arena_wh,arena_wh};
    game->begin(si, (HWND)hwnd, viewport, arena);

	int exitCode=System_GUIPollLoop(si, hwnd);
    game->shutdown();
    delete game;
    return exitCode;
	return 0;
}


int StateUpdate(System_Info* si,SYSTEMSTATE state, System_Hwnd hwnd)
{
	switch(state)
	{
	case SYSTEM_ACTIVE:
		System_GUILockMouse(si, hwnd,!game->paused);
		lockgame=false;
		break;
	case SYSTEM_INACTIVE:
		System_GUILockMouse(si, hwnd,false);
		lockgame=true;
		break;
	case SYSTEM_TIMER:
		if(!lockgame){
            lockgame=true;
                threads++;
                if(threads>1) System_Alert("Thread race-condition imminent.","Warning");
                game->step();
				//stress test//for(int i=0;i<INT_MAX;i++) {SDL_Delay(random(0,100)); Sound_ClipPlay(game->si,"./Snd/shot.wav",random(0,1000),1000);}
                threads--;
            lockgame=false;
        }
		break;
	case SYSTEM_CLOSE://window prompted to close.  fine by me, but I want to quit the game too; sending Quit message (will also force a return from the Polling loop)
		return si->NotifyState(SYSTEM_QUIT,hwnd);
	case SYSTEM_QUIT://once this finishes, System_GUIPollLoop() will return in main, then the program will release game and exit.
		lockgame=true;
        game->end(NULL);
		return SYSTEM_QUIT;//break;
	}
	return SYSTEM_NONE;
}
