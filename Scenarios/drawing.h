#include <windows.h>
#include <wingdi.h>

typedef struct{
} Drawing_Info;

Drawing_Info* Drawing_Startup(HWND hwnd)
{
    Drawing_Info* di = new Drawing_Info();
}

void Drawing_Shutdown(Drawing_Info* di)
{
}
