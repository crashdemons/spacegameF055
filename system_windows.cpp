#include "system_windows.h"
#include <assert.h>

System_InfoEx* g_SI=NULL;

System_InfoEx* System_Windows_Startup(HINSTANCE hThisInstance,HINSTANCE hPrevInstance,LPSTR lpszArgument,int nCmdShow)
{   
	
	//http://msdn.microsoft.com/en-us/library/ms647232.aspx
	System_InfoEx* si=new System_InfoEx;
	si->hThisInstance=hThisInstance;
	si->hPrevInstance=hPrevInstance;
	si->lpszArgument=lpszArgument;
	si->nCmdShow=nCmdShow;
	si->argv = CommandLineToArgvA(lpszArgument, &(si->argc));

	strcpy_s(si->szClassName, "AbstractedSystemWindow");

	si->lastHwnd=NULL;
	si->wincl.hInstance = hThisInstance;
    si->wincl.lpszClassName = si->szClassName;
    si->wincl.lpfnWndProc = System_WinProc;      // This function is called by windows/DispatchMessage 
    si->wincl.style = CS_DBLCLKS;                 // Catch double-clicks 
    si->wincl.cbSize = sizeof (WNDCLASSEX);
    si->wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    si->wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    si->wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    si->wincl.lpszMenuName = NULL;                 // No menu 
    si->wincl.cbClsExtra = 0;                      // No extra bytes after the window class 
    si->wincl.cbWndExtra = 0;                      // structure or the window instance 
    si->wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    if (!RegisterClassEx ( &(si->wincl) ))
	{
		MessageBox(NULL,"Register Class failed.","System Error",MB_OK);
		delete si; return NULL;
	}

	g_SI=si;

	return si;
}


bool System_Alert(System_Info* si, System_Hwnd hwnd, const string& message,const string& title)
{
	MessageBox((HWND)hwnd,message.c_str(),title.c_str(),MB_OK);
	return true;
}
bool System_Alert(const string& message,const string& title)
{
	MessageBox(NULL,message.c_str(),title.c_str(),MB_OK);
	return true;
}
bool System_DebugOutput(const string& s)
{
	OutputDebugString(s.c_str());
	return true;
}



System_Hwnd System_GUICreate(System_Info* sic, const string& title, int clientWidth, int clientHeight, int left, int top)
{
	System_InfoEx* si=(System_InfoEx*) sic;
	if(left==-1) left=CW_USEDEFAULT;
	if(top==-1) top=CW_USEDEFAULT;
	RECT dim={0,0,clientWidth,clientHeight};
	AdjustWindowRectEx(&dim, WS_OVERLAPPEDWINDOW, FALSE, 0);
	si->lastHwnd = 
		CreateWindowEx (
			   0,                   // Extended possibilites for variation 
			   si->szClassName,         // Classname 
			   title.c_str(),       // Title Text 
			   WS_OVERLAPPEDWINDOW|WS_VISIBLE, // default window 
			   left,       // Windows decides the position 
			   top,       // where the window ends up on the screen 
			   dim.right,                 // The programs width 
			   dim.bottom,                 // and height in pixels 
			   HWND_DESKTOP,        // The window is a child-window to desktop 
			   NULL,                // No menu
			   si->hThisInstance,       // Program Instance handler 
			   NULL                 // No Window Creation data 
		);
	return si->lastHwnd;
}

bool System_GUIShow(System_Info* si, System_Hwnd hwnd, bool show)
{
	if(show) return ShowWindow((HWND)hwnd,SW_SHOW)==TRUE;
	return ShowWindow((HWND)hwnd,SW_HIDE)==TRUE;
}

int System_GUIPollLoop(System_Info* sic, System_Hwnd hwnd)
{
	System_InfoEx* si=(System_InfoEx*) sic;
	while (GetMessage (&(si->messages), NULL, 0, 0))
    {
        TranslateMessage(&(si->messages));
        DispatchMessage(&(si->messages));
    }
	int i=(int) (si->messages).wParam;
	if(i==0) return EXIT_SUCCESS;
	return EXIT_FAILURE;
}


bool System_GUIDestroy(System_Info* si, System_Hwnd hwnd)
{
	return DestroyWindow((HWND) hwnd)==TRUE;
}
bool System_GUIQuit(System_Info* si,System_Hwnd hwnd)
{
	si->NotifyState(SYSTEM_QUIT,hwnd);
	PostQuitMessage(0);
	return true;
}
bool System_GUILockMouse(System_Info* si, System_Hwnd hwnd)
{
	RECT rct={0,0,0,0};
	POINT p0={0,0};
	ClientToScreen((HWND)hwnd,&p0);
	GetClientRect((HWND)hwnd,&rct);
	rct.left+=p0.x;
	rct.right+=p0.x;
	rct.top+=p0.y;
	rct.bottom+=p0.y;
	return ClipCursor(&rct)==TRUE;
}
bool System_GUICursorShow(System_Info* si, System_Hwnd hwnd, bool show)
{
	if(show) ShowCursor(TRUE);
	else ShowCursor(FALSE);
	return true;
}

System_Handle System_TimerCreate(System_Info* si, System_Hwnd hwnd, unsigned int ms)
{
	return (System_Handle) SetTimer((HWND) hwnd,1,ms,NULL);
}

bool System_TimerDestroy(System_Info* si, System_Hwnd hwnd, System_Handle hTimer)
{
	return KillTimer((HWND) hwnd, (UINT_PTR) hTimer)==TRUE;
}


//-------------------------------------------------------------------------------------

LRESULT CALLBACK System_WinProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//MessageBox(NULL,"WinProc","System",MB_OK);
	if(g_SI==NULL) return DefWindowProc (hWnd, message, wParam, lParam);
	SYSTEMSTATE state=SYSTEM_NONE;
	switch (message)                  // handle the messages 
    {
		case WM_ACTIVATE:
			if(LOWORD(wParam) && HIWORD(wParam)==0) state=SYSTEM_ACTIVE;
			else state=SYSTEM_INACTIVE;
			break;
        case WM_DESTROY:
			state=SYSTEM_CLOSE;
            break;
        case WM_TIMER:
			state=SYSTEM_TIMER;
            break;
        default:                      // for messages that we don't deal with
            return DefWindowProc (hWnd, message, wParam, lParam);
    }
	g_SI->NotifyState(state,(System_Hwnd)hWnd);
    return 0;
}




//Sylvain Defresne ( http://stackoverflow.com/users/5353/sylvain-defresne )
//http://stackoverflow.com/questions/291424/canonical-way-to-parse-the-command-line-into-arguments-in-plain-c-windows-api
LPSTR* CommandLineToArgvA(LPSTR lpCmdLine, INT *pNumArgs)
{
    int retval;
    retval = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, lpCmdLine, -1, NULL, 0);
    if (!SUCCEEDED(retval))
        return NULL;

    LPWSTR lpWideCharStr = (LPWSTR)malloc(retval * sizeof(WCHAR));
    if (lpWideCharStr == NULL)
        return NULL;

    retval = MultiByteToWideChar(CP_ACP, MB_ERR_INVALID_CHARS, lpCmdLine, -1, lpWideCharStr, retval);
    if (!SUCCEEDED(retval))
    {
        free(lpWideCharStr);
        return NULL;
    }

    int numArgs;
    LPWSTR* args;
    args = CommandLineToArgvW(lpWideCharStr, &numArgs);
    free(lpWideCharStr);
    if (args == NULL)
        return NULL;

    int storage = numArgs * sizeof(LPSTR);
    for (int i = 0; i < numArgs; ++ i)
    {
        BOOL lpUsedDefaultChar = FALSE;
        retval = WideCharToMultiByte(CP_ACP, 0, args[i], -1, NULL, 0, NULL, &lpUsedDefaultChar);
        if (!SUCCEEDED(retval))
        {
            LocalFree(args);
            return NULL;
        }

        storage += retval;
    }

    LPSTR* result = (LPSTR*)LocalAlloc(LMEM_FIXED, storage);
    if (result == NULL)
    {
        LocalFree(args);
        return NULL;
    }

    int bufLen = storage - numArgs * sizeof(LPSTR);
    LPSTR buffer = ((LPSTR)result) + numArgs * sizeof(LPSTR);
    for (int i = 0; i < numArgs; ++ i)
    {
        assert(bufLen > 0);
        BOOL lpUsedDefaultChar = FALSE;
        retval = WideCharToMultiByte(CP_ACP, 0, args[i], -1, buffer, bufLen, NULL, &lpUsedDefaultChar);
        if (!SUCCEEDED(retval))
        {
            LocalFree(result);
            LocalFree(args);
            return NULL;
        }

        result[i] = buffer;
        buffer += retval;
        bufLen -= retval;
    }

    LocalFree(args);

    *pNumArgs = numArgs;
    return result;
}

