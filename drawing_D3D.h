// *** for Dev-CPP (Home); these files are included in the default directory
//note: these are MingW conversions, the original SDK source files were made to work with Visual C++ alone.
#pragma once
#include "common.h"
#include "drawing.h"
#include <d3d9.h>
#include <d3dx9.h>

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")




/*


// *** for Visual Studio; these files are included in an external directory, using direct paths saves the trouble of adding the directories
#include "C:\Program Files\Microsoft DirectX SDK (March 2009)\Include\d3d9.h"
#include "C:\Program Files\Microsoft DirectX SDK (March 2009)\Include\d3dx9.h"
#pragma comment (lib, "C:\\Program Files\\Microsoft DirectX SDK (March 2009)\\Lib\\x86\\d3d9.lib")
#pragma comment (lib, "C:\\Program Files\\Microsoft DirectX SDK (March 2009)\\Lib\\x86\\d3dx9.lib")

*/


#define D3DFVF_TL (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)
struct Vertex3TL
{
	float fX;
	float fY;
	float fZ;
	float fRHW;
	int Color;
	float fU;
	float fV;
};





typedef D3DXVECTOR3 Vector3;
typedef D3DXVECTOR2 Vector2;
//typedef D3DXVECTOR2 fPOINT;


struct Vertex3C{
    float x,y,z;//rhw;
	D3DCOLOR color;
};




class Drawing_InfoX : public Drawing_Info{
public:

    HDC hdc;
    LPDIRECT3D9 d3d;
    LPDIRECT3DDEVICE9 d3ddev;
    //LPDIRECT3DSURFACE9 backSurface;
    //LPDIRECT3DSURFACE9 tileSurface;
    LPDIRECT3DSURFACE9 backBuffer;
    LPD3DXSPRITE spriteObj;
	LPD3DXFONT fontObj;
	~Drawing_InfoX();
};
class Image_InfoX : public Image_Info{
    public:
       /* const char* image;
        int transcolor;
        RECT SrcRect;
        fRECT DstRect;
        float rotation;
        fPOINT center;
        float radius;
        int mask;

		
		bool viewcentered;
		bool viewrelative;
		float relativemult;*/
        
        D3DXIMAGE_INFO dxi;
        LPDIRECT3DTEXTURE9 tex;

        void Unload();
        void Reload(Drawing_Info* di);
		~Image_InfoX();
};





Vertex3TL CreateVertex3TL   (float X, float Y, float Z, float RHW, D3DCOLOR color, float U, float V);




