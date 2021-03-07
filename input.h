/**
@file input.h
@brief Abstracted device-input for control in the game.

...
@author crashenator (crashenator -at- gmail.com)
*/

#pragma once
#include "common.h"
#include <vector>

struct POINT3{ long int x,y,z; };
enum INPUTKEYSTATE
{
	INPUT_KEY_IND=0,
	INPUT_KEY_UP=1,
	INPUT_KEY_DOWN=2,
	INPUT_KEY_CHANGED=4,
	INPUT_KEY_PRESSED=8,
	INPUT_KEY_RELEASED=16,
	INPUT_KEY_REPEATED=32
};
enum INPUTKEYSOURCE
{
	INPUT_KEYS_UNKNOWN,//unspecified. for GetKeyState, it will autodetect between Mouse and Keyboard
	INPUT_KEYS_MOUSE,
	INPUT_KEYS_KEYBOARD,
	INPUT_KEYS_GAMECTRL,//actual buttons on the controller, not including POV/D-pad

	INPUT_KEYS_POV,//for an 4 or 8 button POV, interpreted as each button and also each angle.  (use INPUTKEYPOV values)//threshold between buttons, 45? 22?
	INPUT_KEYS_AXISBW//using On and Off thresholds, the axis can be treated like a button (use positive and negative versions of INPUTAXES values)
};
enum INPUTKEYPOV//D-pad/POV input "buttons"
{
	INPUT_POV_N=0,INPUT_POV_E, INPUT_POV_S, INPUT_POV_W,
	INPUT_POV_NE, INPUT_POV_SE,INPUT_POV_SW,INPUT_POV_NW
};
enum INPUTAXIS//axes to be used with sliding-scale calculations and with INPUT_KEYS_AXISBW (on/off detection)
{
	INPUT_AXIS_POV=0,//approximated axis
	INPUT_AXIS_U,INPUT_AXIS_V,//slider axes
	INPUT_AXIS_X,  INPUT_AXIS_Y, INPUT_AXIS_Z,
	INPUT_AXIS_RX, INPUT_AXIS_RY,INPUT_AXIS_RZ,//so-called rotation axes
	INPUT_AXES//invalid, just used for array declarations
};

//ON/OFF.... >deadzone  or on/off thresholds?


void* Input_Startup(System_Info* si);
void Input_Shutdown(void* inputInfo);
void Input_Poll(void* inputInfo);
void Input_Clear(void* inputInfo);
void Input_LimitMouseCoords(void* inputInfo, RECT rct);//bounds the recorded coordinates inside a given rectangle; does not effect actual mouse use.

INPUTKEYSTATE Input_GetExplicitKeyState(void* inputInfo, unsigned int keyExplicit, INPUTKEYSOURCE dev);//this function requires an explicit device.
INPUTKEYSTATE Input_GetKeyState(void* inputInfo, unsigned int key, INPUTKEYSOURCE dev=INPUT_KEYS_UNKNOWN);//this function can distinguish mouse keys from keyboard keys when dev=UNKNOWN (GameCtrl must be explicit)
const POINT3 Input_GetMousePos(void* inputInfo);//x,y,z; Z being a scroll wheel movement (one integer delta per click/tick of the wheel)
float Input_GetPovAngle(void* inputInfo, bool& isCentered);//get radian angle approximation based on D-pad/POV input
void Input_PovToMouseDelta(void* inputInfo,int multiplier);//shift saved mouse coordinates based on D-pad/POV input









