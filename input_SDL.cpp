#include "input_SDL.h"

Input_Info* g_II=NULL;

#define CLEARTENSES(x) x=x&(INPUT_KEY_DOWN|INPUT_KEY_UP);


int Input_Handle_KeyState(int oldflags, Uint32 type, Uint8 state, Uint8 repeat){
	int keep=(oldflags&(INPUT_KEY_PRESSED|INPUT_KEY_RELEASED|INPUT_KEY_REPEATED|INPUT_KEY_CHANGED));//we keep these flags because they must queue until checked.
	int flags=INPUT_KEY_DOWN;
	if(type==SDL_KEYUP || type==SDL_MOUSEBUTTONUP) flags=INPUT_KEY_UP;
	//if(type==INPUT_KEY_UP) System_Alert("bob","x");
	if((oldflags&(INPUT_KEY_DOWN|INPUT_KEY_UP))!=flags){
		flags|=INPUT_KEY_CHANGED;
		if(state==SDL_PRESSED) flags|=INPUT_KEY_PRESSED;
		else flags|=INPUT_KEY_RELEASED;
	}
	if(repeat!=0) flags|=INPUT_KEY_REPEATED;
	return flags|keep;
}
int Input_Handle_KeyState2(int oldflags, Uint32 state,Uint32 button){
	int keep=(oldflags&(INPUT_KEY_PRESSED|INPUT_KEY_RELEASED|INPUT_KEY_REPEATED|INPUT_KEY_CHANGED));//we keep these flags because they must queue until checked.
	int flags=INPUT_KEY_UP;
	if(state&button) flags=INPUT_KEY_DOWN;
	if((oldflags&(INPUT_KEY_DOWN|INPUT_KEY_UP))!=flags){
		flags|=INPUT_KEY_CHANGED;
		if(flags&INPUT_KEY_DOWN) flags|=INPUT_KEY_PRESSED;
		else flags|=INPUT_KEY_RELEASED;
	}
	return flags|keep;
}



int Input_Handle(SDL_Event e)
{
	Input_Info* ii=g_II;

	switch(e.type){
	case SDL_KEYDOWN:
	case SDL_KEYUP:
		ii->keys[e.key.keysym.sym]=Input_Handle_KeyState(ii->keys[e.key.keysym.sym], e.type, e.key.state, e.key.repeat);
		break;
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
		ii->mousekeys[e.button.which]=Input_Handle_KeyState(ii->mousekeys[e.button.which], e.type, e.button.state, 0);
		break;

	case SDL_MOUSEMOTION:
		ii->mousePos.x+=e.motion.xrel;
		ii->mousePos.y+=e.motion.yrel;
		break;
	case SDL_MOUSEWHEEL:

		switch(ii->scrollMode)
		{
		case SCROLL_RAW:
			ii->mousePos.z+=e.wheel.y;
			//ii->mousePos.z+=ii->mouseState[ii->iCurrent].lZ;
			break;
		case SCROLL_SINGLE:
			if(e.wheel.y>0) ii->mousePos.z++;
			if(e.wheel.y<0) ii->mousePos.z--;
			break;
		case SCROLL_MULTIPLE:
			ii->mousePos.z+= e.wheel.y / 120;
			break;
		}
	}
	return 0;
}


void* Input_Startup(System_Info* si)
{
	((System_InfoEx*)si)->inputHandler=Input_Handle;
	HWND hWnd=NULL;

	Input_Info* ii=new Input_Info();
	ZeroMemory(ii,sizeof(Input_Info));
	g_II=ii;




	ii->iCurrent=0;
	ii->iLast=1;
//	ii->joyState[ii->iLast   ].rgdwPOV[0]=0xFFFF;//centered
//	ii->joyState[ii->iCurrent].rgdwPOV[0]=0xFFFF;//centered

	ii->scrollMode=SCROLL_MULTIPLE;
	ZeroMemory(ii->keys,sizeof(ii->keys));

	RECT rct={0,0,0,0};
	POINT p0={0,0};
	ClientToScreen(hWnd,&p0);
	GetClientRect(hWnd,&rct);
	rct.left+=p0.x;
	rct.right+=p0.x;
	rct.top+=p0.y;
	rct.bottom+=p0.y;
	/*
	if(ClipCursor(&rct)==FALSE) System_Alert("ClipCursor failure","");*/

	POINT3 tmp={(rct.right-rct.left)/2,  (rct.bottom-rct.top)/2,0};
	ii->mousePos=tmp;
	


	return ii;
}



void Input_Shutdown(void* inputInfo)
{
	Input_Info* ii=(Input_Info*) inputInfo;//boy, that looks great. </sarcasm>
	//add release code.
}





inline void SwapTenses(Input_Info* ii)
{
	char tmp=ii->iCurrent;
	ii->iCurrent=ii->iLast;
	ii->iLast=tmp;
}


void Input_Poll(void* inputInfo)
{
	Input_Info* ii=(Input_Info*) inputInfo;//boy, that looks great. </sarcasm>

	int x,y;
	Uint32 state=SDL_GetMouseState(&x,&y);
	ii->mousePos.x=x;
	ii->mousePos.y=y;

	
	ii->mousekeys[SDL_BUTTON_LEFT]=Input_Handle_KeyState2(ii->mousekeys[SDL_BUTTON_LEFT], state,SDL_BUTTON(SDL_BUTTON_LEFT));
	ii->mousekeys[SDL_BUTTON_MIDDLE]=Input_Handle_KeyState2(ii->mousekeys[SDL_BUTTON_MIDDLE], state,SDL_BUTTON(SDL_BUTTON_MIDDLE));
	ii->mousekeys[SDL_BUTTON_RIGHT]=Input_Handle_KeyState2(ii->mousekeys[SDL_BUTTON_RIGHT], state,SDL_BUTTON(SDL_BUTTON_RIGHT));
	

	return;//SDL implementation is message-driven (mouse support not working in Events yet though...)
}
void Input_Clear(void* inputInfo)
{
	Input_Info* ii=(Input_Info*) inputInfo;//boy, that looks great. </sarcasm>
	for(int i=0;i<1024;i++){CLEARTENSES(ii->keys[i]);}
	for(int i=0;i<10;i++){CLEARTENSES(ii->mousekeys[i]);}
}



void Input_LimitMouseCoords(void* inputInfo, RECT rct)
{
	Input_Info* ii=(Input_Info*) inputInfo;//boy, that looks great. </sarcasm>
	if(ii->mousePos.x < rct.left  ) ii->mousePos.x=rct.left  ;
	if(ii->mousePos.y < rct.top   ) ii->mousePos.y=rct.top   ;
	if(ii->mousePos.x > rct.right ) ii->mousePos.x=rct.right ;
	if(ii->mousePos.y > rct.bottom) ii->mousePos.y=rct.bottom;
}


INPUTKEYSTATE Input_GetExplicitKeyState(void* inputInfo, unsigned int key, INPUTKEYSOURCE dev)
{

	Input_Info* ii=(Input_Info*) inputInfo;//boy, that looks great. </sarcasm>
	int flags=INPUT_KEY_IND;
	bool stateA,stateB;

	switch(dev)
	{
	case INPUT_KEYS_MOUSE:
		flags=ii->mousekeys[key];
		CLEARTENSES(ii->mousekeys[key]);
		break;
	case INPUT_KEYS_KEYBOARD:
		flags=ii->keys[key];
		CLEARTENSES(ii->keys[key]);
		break;
	case INPUT_KEYS_GAMECTRL:
		//not supported
		break;
	default:
		return INPUT_KEY_IND;//anything else is not implemented (including INPUT_DEV_UNKNOWN)
		break;
	}

	return (INPUTKEYSTATE)flags;
}

INPUTKEYSTATE Input_GetKeyState(void* inputInfo, unsigned int k, INPUTKEYSOURCE dev)
{
	if(dev==INPUT_KEYS_UNKNOWN)
	{
		dev=INPUT_KEYS_KEYBOARD;
		if(RANGE(k,SDL_BUTTON_LEFT,SDL_BUTTON_X2)) dev=INPUT_KEYS_MOUSE;
	}
	return Input_GetExplicitKeyState(inputInfo, k, dev);
}


const POINT3 Input_GetMousePos(void* inputInfo)
{
	if(inputInfo==NULL)
	{
		POINT3 pt={0,0,0};
		return pt;
	}
	return ((Input_Info*) inputInfo)->mousePos;
}
float Input_GetPovAngle(void* inputInfo, bool& isCentered)
{
	/*
	Input_Info* ii=(Input_Info*) inputInfo;//boy, that looks great. </sarcasm>
	DWORD pov=ii->joyState[ii->iCurrent].rgdwPOV[0];
	isCentered=(LOWORD(pov) == 0xFFFF);//via MSDN (Remarks) http://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.reference.dijoystate2%28v=vs.85%29.aspx
	if(isCentered) return 0.0f;
	return (((float)pov)/100)*DEGRAD;
	*/
	return 0.0f;

}
void Input_PovToMouseDelta(void* inputInfo,int multiplier)
{
	Input_Info* ii=(Input_Info*) inputInfo;//boy, that looks great. </sarcasm>
	bool isCentered=true;
	float radPov=Input_GetPovAngle(inputInfo,isCentered);//isCentered passed ByRef as an output value.
	if(!isCentered){
		ii->mousePos.x+=(int) (sin(radPov)*multiplier);//works GREAT with like, mult=10
		ii->mousePos.y-=(int) (cos(radPov)*multiplier);
	}
}


int GetAxisValue(Input_Info* ii, INPUTAXIS ax, bool bCentered_out)
{
	/*
	DIJOYSTATE2& joy=ii->joyState[ii->iCurrent];

	switch(ax)
	{
	case INPUT_AXIS_POV:
		bCentered_out=(LOWORD(joy.rgdwPOV[0]) == 0xFFFF);
		return joy.rgdwPOV[0];
	case INPUT_AXIS_U:
		return joy.rglSlider[0];
	case INPUT_AXIS_V:
		return joy.rglSlider[1];
	case INPUT_AXIS_X:
	case INPUT_AXIS_Y:
	case INPUT_AXIS_Z:
	case INPUT_AXIS_RX:
	case INPUT_AXIS_RY:
	case INPUT_AXIS_RZ:
	default:
	;
	}
	*/
	return 0;
}
unsigned int GetAxisValueEx(void* inputInfo, INPUTAXIS ax, int sign)
{
	return 0;
}