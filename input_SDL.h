/**
@file input_SDL.h
@brief SDL implementation for input-device functions

...
@author crashenator (crashenator -at- gmail.com)
*/

#pragma once
#include "common.h"
#include "input.h"
#include "system_SDL.h"




enum ScrollMode
{
	SCROLL_RAW,
	SCROLL_SINGLE,
	SCROLL_MULTIPLE
};


class Input_Info
{
public:

    
	char iLast;
	char iCurrent;
	int keys[1024];
	int mousekeys[10];
	POINT3 mousePos;

	bool hasController;
	ScrollMode scrollMode;

	char iSelectedDPAD;//0-3, one of four d-pads (wow, seriously? four d-pads on a single controller? Microsoft is forward-thinking.)
						//we'll limit (ha!) or rather extend this amount to other implementations, they can choose to ignore it if necessary.
						// for our implementations, we only want to work with one d-pad at once
//----------------------------------


	bool axisBW[2][2][INPUT_AXES];//0 to 65535, for each axis there is a -/+ axis.


};


int Input_Handle(SDL_Event e);


//HRESULT wrapper(const string& desc,HRESULT hr);


