#pragma once
#include "common.h"
#include "input.h"

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")


#define DI_FLAG_KEYDOWN 0x80
#define DI_KEYDOWN(x) ((x&DI_FLAG_KEYDOWN)==DI_FLAG_KEYDOWN)

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
	char keys[2][256];
	POINT3 mousePos;

	bool hasController;
	ScrollMode scrollMode;

	char iSelectedDPAD;//0-3, one of four d-pads (wow, seriously? four d-pads on a single controller? Microsoft is forward-thinking.)
						//we'll limit (ha!) or rather extend this amount to other implementations, they can choose to ignore it if necessary.
						// for our implementations, we only want to work with one d-pad at once
//----------------------------------


	bool axisBW[2][2][INPUT_AXES];//0 to 65535, for each axis there is a -/+ axis.


	DIMOUSESTATE2 mouseState[2];
	DIJOYSTATE2   joyState[2];

	LPDIRECTINPUT8  pInput;
	LPDIRECTINPUTDEVICE8 pMouse;
	LPDIRECTINPUTDEVICE8 pKeyboard;
	LPDIRECTINPUTDEVICE8 pGameCtrl;
};



//HRESULT wrapper(const string& desc,HRESULT hr);


