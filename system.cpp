#include "system.h"

int System_Info::NotifyState(SYSTEMSTATE sta, System_Hwnd hwnd)
{
	if(StateUpdate!=NULL && sta!=SYSTEM_NONE) return StateUpdate(this,sta,hwnd);
	return SYSTEM_NONE;
}
System_Info::System_Info()
{
	argc=0;
	argv=NULL;
	StateUpdate=NULL;
}

System_Info* System_Std_Startup(int argc, char** argv)
{
	System_Info* si = new System_Info;
	si->argc=argc;
	si->argv=argv;
	return si;
}

/*
//dummy functions below
bool System_Alert(System_Info* si, System_Hwnd hwnd, const string& message, const string& title){ return false; }
bool System_Alert(const string& message,const string& title){ return false; }
bool System_DebugOutput(const string& s)
{
	printf("%s",s.c_str());
	return true;
}

System_Hwnd System_GUICreate(System_Info* si, const string& title, int clientWidth, int clientHeight, int left, int top){ return NULL; }
bool System_GUIShow(System_Info* si, System_Hwnd hwnd, bool show){ return false; }
int System_GUIPollLoop(System_Info* si, System_Hwnd hwnd){ return EXIT_FAILURE; }
bool System_GUIDestroy(System_Info* si, System_Hwnd hwnd){ return false; }
bool System_GUIQuit(System_Info* si,System_Hwnd hwnd){ return false; }
bool System_GUILockMouse(System_Info* si, System_Hwnd hwnd){ return false; }
bool System_GUICursorShow(System_Info* si, System_Hwnd hwnd, bool show){ return false; }


System_Handle System_TimerCreate(System_Info* si, System_Hwnd hwnd, unsigned int ms){ return NULL; }
bool System_TimerDestroy(System_Info* si, System_Hwnd hwnd, System_Handle hTimer){ return false; }


BOOL IntersectRect(LPRECT out, LPRECT a, LPRECT b)
{
	return TRUE;
}

*/