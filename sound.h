/**
@file sound.h
@brief 

...
@author crashenator (crashenator -at- gmail.com)
*/
#pragma once
#include "common.h"




void* Sound_Startup();
void Sound_Shutdown(void* pS);

bool Sound_SetMusic(void* pS, const string& filename,int fadems=0);
//void* Sound_ClipLoad(void* pS, const string& filename);
//bool Sound_ClipPlay(void* pS, void* pClip, POINT* direction);
//bool Sound_ClipUnload(void* pS, void* pClip);
bool Sound_ClipPlay(void* pS, const string& filename, int distance=0, int distmax=100);


//#include "sound_winapi.h"

//#include "sound_directsound.h"//Unfinished: couldn't find a good tutorial that was NOT managed and DIDNT use unlicensed DX Sample code.


