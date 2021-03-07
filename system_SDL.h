/**
@file system_SDL.h
@brief 

...
@author crashenator (crashenator -at- gmail.com)
*/

#pragma once

#include "system.h"//ONLY included so we can use the class, the pragmas will stop recursive inclusion.
#include <Windows.h>
#include "SDL.h"

class System_InfoEx : public System_Info
{
public:
	//...
	bool tickReady;
	int(*inputHandler)(SDL_Event e);
};
int System_HandleEvent  (System_InfoEx* si, SDL_Event e);
System_InfoEx* System_SDL_Startup(int argc, char *argv[]);

#ifndef SYSTEM_MAIN
#define SYSTEM_MAIN() int main (int argc, char *argv[])
#endif
#ifndef SYSTEM_STARTUP
#define SYSTEM_STARTUP() System_SDL_Startup(argc, argv);
#endif

/*
#ifndef RECT
typedef struct
{
    LONG    left;
    LONG    top;
    LONG    right;
    LONG    bottom;
} RECT;
#endif
*/

	