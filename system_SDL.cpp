#include "system_SDL.h"
#include <assert.h>
#include <iostream>

System_InfoEx* g_SI=NULL;

System_InfoEx* System_SDL_Startup(int argc, char *argv[])
{   
	SDL_Init( SDL_INIT_EVERYTHING );
	//SDL_Init(SDL_INIT_VIDEO);

	System_InfoEx* si=new System_InfoEx;
	si->argc=argc;
	si->argv=argv;
	si->lastHwnd=NULL;
	si->StateUpdate=NULL;
	si->inputHandler=NULL;
	g_SI=si;
	return si;
}


bool System_Alert(System_Info* si, System_Hwnd hwnd, const string& message,const string& title)
{
	return SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, title.c_str(), string(message+"      \n\n\n").c_str(), (SDL_Window *)hwnd)==0;
}
bool System_Alert(const string& message,const string& title)
{
	return SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, title.c_str(), string(message+"      \n\n\n").c_str(), NULL)==0;
}
bool System_DebugOutput(const string& s)
{
	std::cout<<s<<endl;
	//SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION,
    //               SDL_LOG_PRIORITY_DEBUG,
    //               "%s",
    //               s.c_str());
	return true;
}



System_Hwnd System_GUICreate(System_Info* sic, const string& title, int clientWidth, int clientHeight, int left, int top)
{

	
	System_InfoEx* si=(System_InfoEx*) sic;
		
	
	if(left==-1) left=SDL_WINDOWPOS_UNDEFINED;
	if(top==-1)  top=SDL_WINDOWPOS_UNDEFINED;

	
	RECT dim={0,0,clientWidth,clientHeight};


	SDL_Window* hWin= SDL_CreateWindow( 
		title.c_str(),                  //    window title
		left,           //    initial x position
		top,           //    initial y position
		clientWidth,                               //    width, in pixels
		clientHeight,                               //    height, in pixels
		SDL_WINDOW_OPENGL                  //    flags - see below
	);
	si->lastHwnd=hWin;


	SDL_SetWindowSize(hWin,
                       clientWidth,
                       clientHeight);//unsure if the CreateWindow dims are client or outer.

	return si->lastHwnd;
}

bool System_GUIShow(System_Info* si, System_Hwnd hwnd, bool show)
{
	if(show){ SDL_ShowWindow((SDL_Window*) hwnd); SDL_RaiseWindow((SDL_Window*) hwnd); }
	else SDL_HideWindow((SDL_Window*) hwnd);
	return true;
}

int System_GUIPollLoop(System_Info* sic, System_Hwnd hwnd)
{
	System_InfoEx* si=(System_InfoEx*) sic;

	int exit=0;
	
    SDL_Event* pEvent=new SDL_Event;
	while(1){
		if(SDL_PollEvent(pEvent)){
			exit=System_HandleEvent(si, *pEvent);
			if(exit!=SYSTEM_NONE) break;
		}else{
			SDL_Delay(1);
		}
		if(si->tickReady) {si->tickReady=false; si->NotifyState(SYSTEM_TIMER,hwnd); }
	}
	return exit;
}


bool System_GUIDestroy(System_Info* si, System_Hwnd hwnd)
{
	SDL_DestroyWindow((SDL_Window*)si->lastHwnd);
	return true;
}
bool System_GUIQuit(System_Info* si,System_Hwnd hwnd)
{
	si->NotifyState(SYSTEM_QUIT,hwnd);
	return true;
}
bool System_GUILockMouse(System_Info* si, System_Hwnd hwnd, bool lock)
{
	SDL_bool b=SDL_FALSE;
	if(lock) b=SDL_TRUE;
	SDL_SetWindowGrab((SDL_Window*) si->lastHwnd, b);
	if(SDL_SetRelativeMouseMode(b)==-1) System_Alert("Unsupported ....","");

	return true;
}
bool System_GUICursorShow(System_Info* si, System_Hwnd hwnd, bool show)
{
	if(show) SDL_ShowCursor(1);
	else SDL_ShowCursor(0);
	return true;
}

Uint32 System_SDL_TimerCallback(Uint32 interval, void *param)
{
	/*
	SDL_Event event;
    SDL_UserEvent userevent;



	userevent.type = SDL_USEREVENT;
    userevent.code = 0;
    userevent.data1 = (void*)SYSTEM_TIMER;
    userevent.data2 = (void*)SYSTEM_TIMER;

    event.type = SDL_USEREVENT;
    event.user = userevent;

    SDL_PushEvent(&event);
    return(interval);
	*/




	//(g_SI->NotifyState(SYSTEM_TIMER,g_SI->lastHwnd));
	g_SI->tickReady=true;
	return interval;
}


System_Handle System_TimerCreate(System_Info* si, System_Hwnd hwnd, unsigned int ms)
{
	return (void*) SDL_AddTimer(ms,System_SDL_TimerCallback, NULL);

	//return (System_Handle) SetTimer((HWND) hwnd,1,ms,NULL);
}

bool System_TimerDestroy(System_Info* si, System_Hwnd hwnd, System_Handle hTimer)
{
	return SDL_RemoveTimer((SDL_TimerID) hTimer)==SDL_TRUE;
	//return KillTimer((HWND) hwnd, (UINT_PTR) hTimer)==TRUE;
}


//-------------------------------------------------------------------------------------

int System_HandleEvent (System_InfoEx* si, SDL_Event e)
{
	//MessageBox(NULL,"WinProc","System",MB_OK);
	SYSTEMSTATE state=SYSTEM_NONE;
	if (e.type == SDL_WINDOWEVENT) {
		switch(e.window.event)
		{
		case SDL_WINDOWEVENT_FOCUS_GAINED:
		{
			state=SYSTEM_ACTIVE; break;
		}
		case SDL_WINDOWEVENT_FOCUS_LOST:
		{
			state=SYSTEM_INACTIVE; break;
		}
		case SDL_WINDOWEVENT_CLOSE:
		case SDL_QUIT:
			state=SYSTEM_CLOSE; break;
			//case TIMER state=timer

		}
	}
	//if(e.type==SDL_USEREVENT){
	//	if(e.user.data1==(void*)SYSTEM_TIMER) state=SYSTEM_TIMER;
	//}
	if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
		if(e.key.keysym.scancode==SDL_SCANCODE_ESCAPE){ state=SYSTEM_CLOSE; }
		if(si->inputHandler!=NULL) si->inputHandler(e);
	}
	if (e.type >= SDL_MOUSEBUTTONDOWN && e.type <= SDL_MOUSEMOTION) {
		if(si->inputHandler!=NULL) si->inputHandler(e);
	}
	return (g_SI->NotifyState(state,g_SI->lastHwnd));
}


