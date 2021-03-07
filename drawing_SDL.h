/**
@file drawing_SDL.h
@brief SDL implementation of abstracted drawing functions.

...
@author crashenator (crashenator -at- gmail.com)
*/

#pragma once
#include "Common.h"
#include "drawing.h"
#include "system_SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"




class Drawing_InfoX : public Drawing_Info{
private:
	//imagecache
public:
	System_InfoEx* si;//redundant, but keeps us from losing it.
	//SDL_Surface* surface;
	SDL_Renderer* renderer;
	TTF_Font* font;



	/*
    HDC hdc;
    LPDIRECT3D9 d3d;
    LPDIRECT3DDEVICE9 d3ddev;
    LPDIRECT3DSURFACE9 backBuffer;
    LPD3DXSPRITE spriteObj;
	LPD3DXFONT fontObj;
	*/
	Drawing_InfoX();
	~Drawing_InfoX();
};
class Image_InfoX : public Image_Info{
    public:
		bool cachedTexture;
		SDL_Texture* img;
		int imgw, imgh;
		Uint32 imgf;
		int imga;
        
		void Debug();
        void Unload();
        void Reload(Drawing_Info* di);
		Image_InfoX();
		~Image_InfoX();
};





