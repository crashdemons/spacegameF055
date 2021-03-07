/**
@file system.h
@brief 

...
@author crashenator (crashenator -at- gmail.com)
*/

#pragma once

#include <stdlib.h>
#include <string>
using namespace std;
// Dev note: standard supported: exit() abort() atexit(), main() EXIT_SUCCESS, EXIT_FAILURE

typedef void* System_Handle;
typedef System_Handle System_Hwnd;

#define SYSTEM_STD_MAIN() int main(int argc, char *argv[])

enum SYSTEMSTATE
{
	SYSTEM_NONE,//dummy message, these will not make it past NotifyState()
	SYSTEM_TIMER,//sent on Timer ticks
	SYSTEM_ACTIVE,//sent when a GUI becomes active (focused)
	SYSTEM_INACTIVE,//sent when a GUI becomes inactive (not focused)
	SYSTEM_CLOSE,//sent when the window is prompted to close but the program could possibly stay running.
	SYSTEM_QUIT,//sent when the program is prompted to exit
};

class System_Info
{
public:
	int argc;
	char** argv;
	System_Hwnd lastHwnd;


	int(*StateUpdate)(System_Info*,SYSTEMSTATE sta,System_Hwnd hwnd);
	int NotifyState(SYSTEMSTATE sta, System_Hwnd hwnd);
	System_Info();
};

System_Info* System_Std_Startup(int argc, char** argv);

//--------------------------------------------------------------------------------------
//****implementation defined functions with abstraction-defined input/output***
// int SYSTEM_MAIN(), if defined
// System_Info* SYSTEM_STARTUP() if defined
bool System_Alert(System_Info* si, System_Hwnd hwnd, const string& message,const string& title);//displays an alert/messagebox window with a message and an OK button (or likeness)
bool System_Alert(const string& message,const string& title="Program Alert Message");//same as above, but simpler.

bool System_DebugOutput(const string& s);//print debug output (console output, file log, none, whatever is applicable)


System_Hwnd System_GUICreate(System_Info* si, const string& title, int clientWidth, int clientHeight, int left=-1, int top=-1);//create a GUI (should set System_Info::lastHwnd)
bool System_GUIShow(System_Info* si, System_Hwnd hwnd, bool show=true);//show/hide a GUI
int System_GUIPollLoop(System_Info* si, System_Hwnd hwnd);//poll the GUI for messages and do Sound_Info::NotifyState as necessary.
//	For implementations with/without a message-loop, the following are required:
//		0. remove messages from the message queue (if applicable) while processing.
//		1. a non-busy wait (a loop that waits for SYSTEM_QUIT or an implementation-defined-quit-message but is not processor-intensive)
//		2. NotifyState(...) is called when appropriate in some way (perhaps not in this function),
//		3. return EXIT_SUCCESS or EXIT_FAILURE
bool System_GUIDestroy(System_Info* si, System_Hwnd hwnd);//close/hide and release the GUI
bool System_GUIQuit(System_Info* si,System_Hwnd hwnd);//prompts sending of SYSTEM_QUIT AND/OR an implementation-defined-quit-message such that: SYSTEM_QUIT is notified, and GUIPollLoop will return  *IN THAT ORDER*

bool System_GUILockMouse(System_Info* si, System_Hwnd hwnd, bool lock=true);//locks the mouse inside of the window
bool System_GUICursorShow(System_Info* si, System_Hwnd hwnd, bool show=true);

System_Handle System_TimerCreate(System_Info* si, System_Hwnd hwnd, unsigned int ms);//prompts the system implementation to begin notifying of SYSTEM_TIMER messages at intervals somehow
bool System_TimerDestroy(System_Info* si, System_Hwnd hwnd, System_Handle hTimer);//prompts the implementation to stop sending timer messages.

//bool System_GeometryRectRectCollision() ?

//NOTE: in addition to the above, the following types must be defined by implementation header:
// HWND, HDC, RECT, POINT ... VK_PAUSE,etc?   BOOL
// LP___
// BOOL IntersectRect(out, a,b),
/*
typedef System_Hwnd HWND;
typedef System_Handle HDC;
typedef bool BOOL;
#define TRUE true
#define FALSE false;
struct RECT{
	int left;
	int top;
	int right;
	int bottom;
};
struct POINT{
	int x; int y;
};
typedef RECT* LPRECT;
typedef POINT* LPPOINT;
BOOL IntersectRect(LPRECT out, LPRECT a, LPRECT b);
*/


//****implementation functions above***
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
//***place implementation header below***
// this is required because of main definition constants; there is no way to override this behavior for some platforms through main() or a C++
// (eg: Windows GUI programs require a WinMain)
// Note: if you require a platform-dependant Main, please define a SYSTEM_MAIN in your implementation header
// Note: if your main does not define 'argc' and 'argv', do so in SYSTEM_MAIN_HELPER

#include "system_SDL.h"

//***place implementation header above***
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
#ifndef SYSTEM_MAIN
#define SYSTEM_MAIN SYSTEM_STD_MAIN
#endif
#ifndef SYSTEM_STARTUP
#define SYSTEM_STARTUP() System_Std_Startup(argc,argv)
#endif
//--------------------------------------------------------------------------------------