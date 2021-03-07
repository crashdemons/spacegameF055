#include "input_directinput.h"


char wrapper(const string& desc,HRESULT hr,bool show=true)
{
	if(hr!=DI_OK && show) System_Alert(desc+" Error ("+utos(hr)+")","Input Error");
	if(SUCCEEDED(hr)) return 1;
	return 0;
}
bool GetDevice(
			   const string& part, 
			   LPDIRECTINPUT8 in,
			   const GUID& guid, 
			   LPCDIDATAFORMAT fmt, 
			   LPDIRECTINPUTDEVICE8* pOut, 
			   HWND hWnd
			  )
{
	*pOut=NULL;
	char success=wrapper(part+" Creation", in->CreateDevice(guid, pOut, NULL) );
	if((*pOut)==NULL || success!=1) return false;

	success&=wrapper(part+" Format",  (*pOut)->SetDataFormat(fmt) );
	success&=wrapper(part+" Level",   (*pOut)->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE|DISCL_FOREGROUND) );
	success&=wrapper(part+" Acquire", (*pOut)->Acquire() ,false);
	if(success!=1) return false;

	
		
	DIDEVICEINSTANCE didi;
	memset(&didi,0,sizeof(DIDEVICEINSTANCE));
	didi.dwSize=sizeof(DIDEVICEINSTANCE);
	wrapper( "dev inst info", (*pOut)->GetDeviceInfo(&didi) );


	char buf[256];
	sprintf(buf,"Device:\n\tProduct: %s\n\tInstance: %s\n",didi.tszProductName,didi.tszInstanceName);
	OutputDebugString(buf);
	//MessageBox(didi.tszInstanceName,didi.tszProductName);
	return true;
}


BOOL CALLBACK DIEnumDevicesCallback(LPCDIDEVICEINSTANCE lpddi,LPVOID pvRef)
{
	
	//MessageBox(lpddi->tszInstanceName,lpddi->tszProductName);
	*((GUID*) pvRef) = lpddi->guidInstance;
	return DIENUM_STOP;
}

void* Input_Startup(HWND hWnd)
{
	Input_Info* ii=new Input_Info();
	ZeroMemory(ii,sizeof(Input_Info));
	ii->iCurrent=0;
	ii->iLast=1;
	ii->joyState[ii->iLast   ].rgdwPOV[0]=0xFFFF;//centered
	ii->joyState[ii->iCurrent].rgdwPOV[0]=0xFFFF;//centered

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
	


	wrapper("DirectInput Initialization",
		DirectInput8Create(
			GetModuleHandle(NULL),
			DIRECTINPUT_VERSION,
			IID_IDirectInput8,
			(void**)&(ii->pInput),
			NULL
		)
	);

	GetDevice("Mouse"   ,ii->pInput,GUID_SysMouse   ,&c_dfDIMouse2   ,&(ii->pMouse)   ,hWnd);
	GetDevice("Keyboard",ii->pInput,GUID_SysKeyboard,&c_dfDIKeyboard,&(ii->pKeyboard),hWnd);

	GUID guidGameCtrl;

	wrapper("Enum GameCtrl",
		ii->pInput->EnumDevices(
			DI8DEVCLASS_GAMECTRL,
			&DIEnumDevicesCallback,//LPDIENUMDEVICESCALLBACK lpCallback,
			&guidGameCtrl,//
			DIEDFL_ATTACHEDONLY//DIEDFL_FORCEFEEDBACK
		)
	);

	
	GetDevice("Game Controller",ii->pInput,guidGameCtrl,&c_dfDIJoystick2,&(ii->pGameCtrl),hWnd);





	return ii;
}

void ReleaseDevice(LPDIRECTINPUTDEVICE8 pDev)
{
	if(pDev!=NULL){
		pDev->Unacquire();
		pDev->Release();
	}
}

void Input_Shutdown(void* inputInfo)
{
	Input_Info* ii=(Input_Info*) inputInfo;//boy, that looks great. </sarcasm>
	ReleaseDevice(ii->pKeyboard);
	ReleaseDevice(ii->pMouse);
	ReleaseDevice(ii->pGameCtrl);
	ii->pInput->Release();
	//add release code.
}
inline bool Reacquire(LPDIRECTINPUTDEVICE8 dev)
{
	HRESULT hr=dev->Acquire();
	while(hr==DIERR_INPUTLOST) hr=dev->Acquire();
	return SUCCEEDED(hr);
}
inline bool Poll(LPDIRECTINPUTDEVICE8 dev, void* pBuf, unsigned int lBuf)
{
	if(dev==NULL) return false;
	HRESULT hr=dev->GetDeviceState(lBuf,(LPVOID) pBuf);
	if (FAILED(hr))
		if(Reacquire(dev)) return Poll(dev,pBuf,lBuf);
	return SUCCEEDED(hr);
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


	SwapTenses(ii);//previous: 0 current, 1 last.  we want the last check to be deprecated, so 1,0
	Poll( ii->pKeyboard, &(ii->keys[ii->iCurrent]       ), sizeof(ii->keys[ii->iCurrent]      ) );
	Poll( ii->pMouse   , &(ii->mouseState[ii->iCurrent] ), sizeof(ii->mouseState[ii->iCurrent]) );
	Poll( ii->pGameCtrl, &(ii->joyState[ii->iCurrent]   ), sizeof(ii->joyState[ii->iCurrent]  ) );

	ii->mousePos.x+=ii->mouseState[ii->iCurrent].lX;
	ii->mousePos.y+=ii->mouseState[ii->iCurrent].lY;



	switch(ii->scrollMode)
	{
	case SCROLL_RAW:
		ii->mousePos.z+=ii->mouseState[ii->iCurrent].lZ;
		break;
	case SCROLL_SINGLE:
		if(ii->mouseState[ii->iCurrent].lZ>0) ii->mousePos.z++;
		if(ii->mouseState[ii->iCurrent].lZ<0) ii->mousePos.z--;
		break;
	case SCROLL_MULTIPLE:
		ii->mousePos.z+= ii->mouseState[ii->iCurrent].lZ / 120;
		break;
	}
}
void Input_LimitMouseCoords(void* inputInfo, RECT rct)
{
	Input_Info* ii=(Input_Info*) inputInfo;//boy, that looks great. </sarcasm>
	if(ii->mousePos.x < rct.left  ) ii->mousePos.x=rct.left  ;
	if(ii->mousePos.y < rct.top   ) ii->mousePos.y=rct.top   ;
	if(ii->mousePos.x > rct.right ) ii->mousePos.x=rct.right ;
	if(ii->mousePos.y > rct.bottom) ii->mousePos.y=rct.bottom;
}


unsigned char Input_GetExplicitKey(unsigned char c)
{
	switch(c)//why? otherwise they will map to Numpad keys.
	{
	case VK_UP:     return DIK_UP;
	case VK_DOWN:   return DIK_DOWN;
	case VK_LEFT:   return DIK_LEFT;
	case VK_RIGHT:  return DIK_RIGHT;
	}
	return (unsigned char) MapVirtualKey(c,MAPVK_VK_TO_VSC);
}

INPUTKEYSTATE Input_GetExplicitKeyState(void* inputInfo, unsigned char key, INPUTKEYSOURCE dev)
{

	Input_Info* ii=(Input_Info*) inputInfo;//boy, that looks great. </sarcasm>
	int flags=INPUT_KEY_IND;
	bool stateA,stateB;

	switch(dev)
	{
	case INPUT_KEYS_MOUSE:
		if(!RANGE(key,0,7)) return INPUT_KEY_IND;//0-3 supported implicitly by GetKeyState
		stateA=DI_KEYDOWN(ii->mouseState[ii->iCurrent].rgbButtons[key]);
		stateB=DI_KEYDOWN(ii->mouseState[ii->iLast].rgbButtons[key]);
		break;
	case INPUT_KEYS_KEYBOARD:
		if(!RANGE(key,0,255)) return INPUT_KEY_IND;
		stateA=DI_KEYDOWN(ii->keys[ii->iCurrent][key]);//isDown
		stateB=DI_KEYDOWN(ii->keys[ii->iLast   ][key]);
		break;
	case INPUT_KEYS_GAMECTRL:
		if(!RANGE(key,0,7)) return INPUT_KEY_IND;//0-3 supported implicitly by GetKeyState
		stateA=DI_KEYDOWN(ii->joyState[ii->iCurrent].rgbButtons[key]);
		stateB=DI_KEYDOWN(ii->joyState[ii->iLast].rgbButtons[key]);
		break;
	default:
		return INPUT_KEY_IND;//anything else is not implemented (including INPUT_DEV_UNKNOWN)
		break;
	}



	if( stateA ) FLAG_SET(flags,INPUT_KEY_DOWN);
	else         FLAG_SET(flags,INPUT_KEY_UP  );

	if(stateA!=stateB){
		FLAG_SET(flags,INPUT_KEY_CHANGED);
		if( stateA ) FLAG_SET(flags,INPUT_KEY_PRESSED );//was changed to Up to Down
		else         FLAG_SET(flags,INPUT_KEY_RELEASED);//Down to Up
	}
	return (INPUTKEYSTATE)flags;
}

INPUTKEYSTATE Input_GetKeyState(void* inputInfo, unsigned char k, INPUTKEYSOURCE dev)
{

	unsigned char k2=Input_GetExplicitKey(k);

	if(dev==INPUT_KEYS_UNKNOWN)
	{
		if(k2==0){//keycode translation chokes when given mouse codes
			if(RANGE(k,0x01,0x02)){ k2=k-1; dev=INPUT_KEYS_MOUSE; }
			if(RANGE(k,0x04,0x05)){ k2=k-2; dev=INPUT_KEYS_MOUSE; }
		}else dev=INPUT_KEYS_KEYBOARD;
	}
	return Input_GetExplicitKeyState(inputInfo, k2, dev);
}


const POINT3& Input_GetMousePos(void* inputInfo)
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
	Input_Info* ii=(Input_Info*) inputInfo;//boy, that looks great. </sarcasm>
	DWORD pov=ii->joyState[ii->iCurrent].rgdwPOV[0];
	isCentered=(LOWORD(pov) == 0xFFFF);//via MSDN (Remarks) http://msdn.microsoft.com/en-us/library/windows/desktop/microsoft.directx_sdk.reference.dijoystate2%28v=vs.85%29.aspx
	if(isCentered) return 0.0f;
	return (((float)pov)/100)*DEGRAD;

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
	return 0;
}
unsigned int GetAxisValueEx(void* inputInfo, INPUTAXIS ax, int sign)
{
	return 0;
}