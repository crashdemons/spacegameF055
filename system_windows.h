#pragma once

#include "system.h"//ONLY included so we can use the class, the pragmas will stop recursive inclusion.
#include <Windows.h>

class System_InfoEx : public System_Info
{
public:
	HINSTANCE hThisInstance;
	HINSTANCE hPrevInstance;
	LPSTR lpszArgument;
	int nCmdShow;

	char szClassName[32];
    MSG messages;            // Here messages to the application are saved 
    WNDCLASSEX wincl;        // Data structure for the windowclass 
};

LRESULT CALLBACK System_WinProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LPSTR* CommandLineToArgvA(LPSTR lpCmdLine, INT *pNumArgs);
System_InfoEx* System_Windows_Startup(HINSTANCE hThisInstance,HINSTANCE hPrevInstance,LPSTR lpszArgument,int nCmdShow);

#ifndef SYSTEM_MAIN
#define SYSTEM_MAIN() int WINAPI WinMain (HINSTANCE hThisInstance,HINSTANCE hPrevInstance,LPSTR lpszArgument,int nCmdShow)
#endif
#ifndef SYSTEM_STARTUP
#define SYSTEM_STARTUP() System_Windows_Startup(hThisInstance,hPrevInstance,lpszArgument,nCmdShow);
#endif

	