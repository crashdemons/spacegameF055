#include "drawing_SDL.h"

#define CACHE_MAXIMUM 512
static SDL_Texture* cache_textures[CACHE_MAXIMUM]={0};
static string       cache_filename[CACHE_MAXIMUM];
static int          cache_index=0;

//-----------------------------------------------------------------------------

inline bool Drawing_Cache_Add(SDL_Texture* pTex, string file){
	if(cache_index>=CACHE_MAXIMUM) return false;
	cache_textures[cache_index]=pTex;
	cache_filename[cache_index]=file;
	cache_index++;
	return true;
}
inline SDL_Texture* Drawing_Cache_Find(string file){
	for(int i=0;i<CACHE_MAXIMUM;i++)
		if(file==cache_filename[i])
			return cache_textures[i];

	return NULL;
}
SDL_Texture* Drawing_Cache_Load(Drawing_InfoX* di, string file, bool& isCached){
	isCached=true;
	SDL_Texture* pTex=Drawing_Cache_Find(file);
	if(pTex!=NULL) { return pTex;}//cache-find succeeded, leave isCached=true, and return the old texture pointer.
	//file="./Img/black.bmp";
	pTex=IMG_LoadTexture(di->renderer,file.c_str());
	if(Drawing_Cache_Add(pTex, file)){return pTex;}//cache-find faled, but load+cache succeeded, leave isCached=true, and return the new texture pointer.
	isCached=false;
	return pTex;//cached-find failed, cache new failed (cache was full), the Image_Info object or caller will be responsible for this new texture.
}

void Drawing_Cache_Clear(){
	for(int i=0;i<CACHE_MAXIMUM;i++)
		if(cache_textures[i]!=NULL){
			SDL_DestroyTexture(cache_textures[i]);//release all the cached textures we took responsibility for.
			cache_textures[i]=NULL;
			cache_filename[i]="";
		}
	cache_index=0;
}
//-----------------------------------------------------------------------------


Drawing_InfoX::Drawing_InfoX(){
	si=NULL;
	renderer=NULL;
	font=NULL;
}
Drawing_InfoX::~Drawing_InfoX(){}
Image_InfoX::~Image_InfoX(){}
Image_InfoX::Image_InfoX(){
	//circle.invalidate();
	//rectangle.invalidate();
	image="";
	img=NULL;
}

void Image_InfoX::Debug(){
	string cached="no";
	if(cachedTexture) cached="yes";
	cout<<"II="<<(void*)this<<" ";
	cout<<"FILE="<<image;
	cout<<" TEX="<<img<<" ";
	cout<<endl;
	for(int i=0;i<200;i++){
		cout<<"  CACHE "<<i<<" "<<cache_textures[i]<<" "<<cache_filename[i];
		cout<<endl;
	}
}
void Image_InfoX::Unload(){
	//circle.invalidate();
	//rectangle.invalidate();
	if(!cachedTexture) SDL_DestroyTexture(img);
	img=NULL;
}

void Image_InfoX::Reload(Drawing_Info* dic)
{
	Drawing_InfoX* di=(Drawing_InfoX*) dic;
	img=Drawing_Cache_Load(di,image,cachedTexture);//IMG_LoadTexture(di->renderer, image);
	

	


	SDL_SetTextureBlendMode(img,SDL_BLENDMODE_BLEND);
	if(img==NULL) System_Alert("Texture Load Failed","SDL");
	SDL_QueryTexture(img,&imgf, 
                     &imga,
                     &imgw, 
                     &imgh);
	//RecalculateRectangle();
	//RecalculateCircle();
}

        



Drawing_Info* Drawing_Startup(System_Info* si, int width, int height, float scale,bool windowed)
{
    Drawing_InfoX* di = new Drawing_InfoX();
    //if(!windowed)
	di->fade=0x7F;
	di->text_quality=TEXT_SHADED;
	TTF_Init();
	IMG_Init(IMG_INIT_PNG);
	System_GUICursorShow(si,si->lastHwnd,false);

	di->si=(System_InfoEx*) si;
    di->hWnd=si->lastHwnd;
	di->renderer=SDL_CreateRenderer((SDL_Window*) si->lastHwnd,
                                 -1,
                                 SDL_RENDERER_ACCELERATED);


	di->font=TTF_OpenFont("FreeMono.ttf",17);
	
	SDL_SetRenderDrawBlendMode(di->renderer, SDL_BLENDMODE_BLEND);
	if(!windowed) SDL_SetWindowFullscreen((SDL_Window*) si->lastHwnd, SDL_WINDOW_FULLSCREEN);




    di->rect.left=0;
    di->rect.right=width-1;
    di->rect.top=0;
    di->rect.bottom=height-1;
    di->scale=scale;
    di->windowed=windowed;
    di->width=width;
    di->height=height;
	di->viewport.x=0;
	di->viewport.y=0;
	di->centering=true;

	
    return di;
}
Drawing_Info* Drawing_Startup(System_Info* si, Drawing_Info* dic)
{
    Drawing_Info* di=Drawing_Startup(si, dic->width,dic->height, dic->scale,dic->windowed);
	di->viewport.x=0;
	di->viewport.y=0;
    delete dic;
    return di;
}

void Drawing_Shutdown(Drawing_Info* dic,bool deletedi)
{
	Drawing_InfoX* di=(Drawing_InfoX*) dic;
	System_GUICursorShow(di->si,di->hWnd,true);

	Drawing_Cache_Clear();

	TTF_CloseFont(di->font);
	
	SDL_DestroyRenderer(di->renderer);
//	SDL_FreeSurface(di->surface);


    if(deletedi) delete di;
	IMG_Quit();
	TTF_Quit();
}

#define COLOR_ARGB(a,r,g,b) (((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define COLOR_XRGB(  r,g,b) COLOR_ARGB(0xff,r,g,b)

unsigned int Drawing_GetColor(int r, int g, int b){return COLOR_XRGB(r,g,b);}
unsigned int Drawing_GetColor(int a, int r, int g, int b){return COLOR_ARGB(a,r,g,b);}

void Drawing_GetColorComponents(unsigned int color, int& a, int& r, int& g, int& b)
{
	a=(color&0xFF000000)>>24;
	r=(color&0x00FF0000)>>16;
	g=(color&0x0000FF00)>>8;
	b=(color&0x000000FF)   ;
}







Image_Info* Drawing_LoadImage(Drawing_Info* dic, string filename, float rotation, int transcolor, bool scaled,int scaleoverride,bool requires_boundaries)
{
	Drawing_InfoX* di=(Drawing_InfoX*) dic;
//    HRESULT result;
	Image_InfoX* ii=ii=new Image_InfoX();

	/*
	SDL_Surface* surf=SDL_LoadBMP(filename);
	
	if(surf==NULL) System_Alert("LoadBMP Failed........","SDL");



	ii->img=SDL_CreateTextureFromSurface(di->renderer,
                                          surf);

*/

	ii->img=Drawing_Cache_Load(di,filename,ii->cachedTexture);//IMG_LoadTexture(di->renderer, filename);
	if(ii->img==NULL) System_Alert("Texture Load Failed","SDL");
	SDL_QueryTexture(ii->img,&ii->imgf, 
                     &ii->imga,
                     &ii->imgw, 
                     &ii->imgh);



	ii->requires_boundaries=requires_boundaries;
    ii->transcolor=transcolor;
    ii->image=filename;
    ii->SrcRect.left=0;
    ii->SrcRect.top=0;
    ii->SrcRect.right=ii->imgw-1;//possibly needs to be -1 since 0 to 0 is one pixel(one pixel on x=0)
    ii->SrcRect.bottom=ii->imgh-1;

	float scale=1.0f;
	if(scaled) scale=di->scale;
	if(scaled && scaleoverride>1) scale=((float)scaleoverride)/100.0f;
    
    ii->DstRect.left=0;
    ii->DstRect.top=0;
	if(scale>0.995 && scale<1.001){
		ii->DstRect.right=ii->imgw-1;//possibly needs to be -1 since 0 to 0 is one pixel(one pixel on x=0)
		ii->DstRect.bottom=ii->imgh-1;
	}else{
		ii->DstRect.right=(ii->imgw-1)*scale;//possibly needs to be -1 since 0 to 0 is one pixel(one pixel on x=0)
		ii->DstRect.bottom=(ii->imgh-1)*scale;
	}
	ii->Rotate(rotation);
    //ii->rotation=rotation;
	

	ii->RecalculateCircle();
	ii->RecalculateRectangle();
	/*
	ii->center.x=midpoint(ii->DstRect.left,ii->DstRect.right );//((ii->DstRect.right)-(ii->DstRect.left))/2;//maybe +1 (as above)
    ii->center.y=midpoint(ii->DstRect.top ,ii->DstRect.bottom);//((ii->DstRect.bottom)-(ii->DstRect.top))/2;
    ii->radius=(ii->center.x + ii->center.y)/2;
	*/


    ii->mask = COLOR_ARGB(255,255,255,255);
    
    

	ii->relativemult=1.0f;

    return ii;
}
void Drawing_UnloadImage(Image_Info* iic)
{
	if(iic==NULL) return;
	//OutputDebugString( (string("Freeing: ")+ ptos((void*)iic)+" "+iic->image+"\n").c_str());
	Image_InfoX* ii=(Image_InfoX*) iic;
	if(!(ii->cachedTexture)) SDL_DestroyTexture(ii->img);
	ii->img=NULL;
    delete ii;
}


void Drawing_RotateImage(Drawing_Info* dic, Image_Info* ii)//unused in SDL port
{
	return;
}

void Drawing_SetTextureMask(SDL_Texture* texture, unsigned int color)
{
	if(texture==NULL) return;
	int a,r,g,b;
	Drawing_GetColorComponents(color,a,r,g,b);
	SDL_SetTextureAlphaMod(texture,a);//why are these two separate functions?
	SDL_SetTextureColorMod(texture,r,g,b);
}

void Drawing_DrawImage(Drawing_Info* dic,Image_Info* iic)
{
	if(dic==NULL) return;
	if(iic==NULL) return;
	Image_InfoX* ii=(Image_InfoX*) iic;
	Drawing_InfoX* di=(Drawing_InfoX*) dic;
	
	SDL_Rect srcrect={ii->SrcRect.left, ii->SrcRect.top, (ii->SrcRect.right - ii->SrcRect.left)+1, (ii->SrcRect.bottom - ii->SrcRect.top)+1 };
	SDL_Rect dstrect={ii->DstRect.left, ii->DstRect.top, (ii->DstRect.right - ii->DstRect.left)+1, (ii->DstRect.bottom - ii->DstRect.top)+1 };
	
	SDL_Point ctr={round(ii->center.x), round(ii->center.y) };


	SDL_Point scale={(ii->DstRect.right-ii->DstRect.left)/(ii->SrcRect.right-ii->SrcRect.left),(ii->DstRect.bottom-ii->DstRect.top)/(ii->SrcRect.bottom-ii->SrcRect.top)};
	//Vector2 scale(1,1);
	float topy=ii->DstRect.top;
	float leftx=ii->DstRect.left;

	if(ii->viewrelative){
		leftx-=ii->relativemult * di->viewport.x;
		topy-=ii->relativemult * di->viewport.y;
	}

	
	dstrect.x=leftx;
	dstrect.y=topy;
	//ctr.x+=leftx;
	//ctr.y+=topy;

	//ii->a

	Drawing_SetTextureMask(ii->img, ii->mask);
	SDL_SetTextureBlendMode(ii->img, SDL_BLENDMODE_BLEND);

	
	if( (dstrect.x > di->width) || (dstrect.y > di->height) || ((dstrect.x+dstrect.w)<0) || ((dstrect.y+dstrect.h)<0)  ) return;//do not draw out-of-bounds images


	if(SDL_RenderCopyEx(di->renderer,
                   ii->img,
                   &srcrect,
                   &dstrect,
				   ii->rotation*(180/3.14159),
				   &ctr,
                   SDL_FLIP_NONE)!=0);//System_Alert("RenderCopyEx failed!","SDL");


}

void Drawing_DrawLine(Drawing_Info* dic, int x1,int y1, int x2, int y2, int color,bool relative)
{
	if(dic==NULL) return;
	Drawing_InfoX* di=(Drawing_InfoX*) dic;
	int a,r,g,b;
	Drawing_GetColorComponents(color,a,r,g,b);
	SDL_SetRenderDrawColor(di->renderer,r,g,b,a);
	if(relative) SDL_RenderDrawLine(di->renderer,x1-di->viewport.x,y1-di->viewport.y,x2-di->viewport.x,y2-di->viewport.y);
	else         SDL_RenderDrawLine(di->renderer,x1,y1,x2,y2);
}


void Drawing_DrawPoint(Drawing_Info* dic, fPOINT draw1, int color)
{
	if(dic==NULL) return;
	Drawing_InfoX* di=(Drawing_InfoX*) dic;
	int a,r,g,b;
	Drawing_GetColorComponents(color,a,r,g,b);
	SDL_SetRenderDrawColor(di->renderer,r,g,b,a);
	SDL_RenderDrawPoint(di->renderer,draw1.x,draw1.y);

}
void Drawing_DrawLine(Drawing_Info* dic, fRECT draw1, int color)
{
	if(dic==NULL) return;
	Drawing_InfoX* di=(Drawing_InfoX*) dic;
	int a,r,g,b;
	Drawing_GetColorComponents(color,a,r,g,b);
	SDL_SetRenderDrawColor(di->renderer,r,g,b,a);
	SDL_RenderDrawLine(di->renderer,draw1.left-di->viewport.x,draw1.top-di->viewport.y,draw1.right-di->viewport.x,draw1.bottom-di->viewport.y);

}
void Drawing_DrawRect(Drawing_Info* dic, fRECT draw1, int color, bool relative, float relmult)
{
	if(dic==NULL) return;
	Drawing_InfoX* di=(Drawing_InfoX*) dic;
	if(!relative) relmult=0;
	SDL_Rect rt={draw1.left - relmult * (di->viewport.x),draw1.top - relmult * (di->viewport.y), (draw1.right-draw1.left)+1, (draw1.bottom-draw1.top)+1 };
	int a,r,g,b;
	Drawing_GetColorComponents(color,a,r,g,b);
	SDL_SetRenderDrawColor(di->renderer,r,g,b,a);
	SDL_RenderDrawRect(di->renderer,
                       &rt);
}
void Drawing_DrawRectFilled(Drawing_Info* dic, fRECT draw, int color, bool relative, float relmult)
{
	if(dic==NULL) return;
	Drawing_InfoX* di=(Drawing_InfoX*) dic;
	if(!relative) relmult=0;
	SDL_Rect rt={draw.left - relmult * (di->viewport.x),draw.top - relmult * (di->viewport.y), (draw.right-draw.left)+1, (draw.bottom-draw.top)+1 };
	int a,r,g,b;
	Drawing_GetColorComponents(color,a,r,g,b);
	SDL_SetRenderDrawColor(di->renderer,r,g,b,a);
	SDL_RenderFillRect(di->renderer,
                       &rt);

}



//-----------------------------------------------------------------------------------
//Draw a simple circle centered around the given point, using the given
//radius and color.  This is in Screen Space (2D).
//-----------------------------------------------------------------------------------
void Drawing_DrawCircle(Drawing_Info* dic, fPOINT pt, float radius, int color, bool relative, float relmult)
{
	if(dic==NULL) return;
	Drawing_InfoX* di=(Drawing_InfoX*) dic;
	if(relative){
		pt.x-=relmult * (di->viewport.x);
		pt.y-=relmult * (di->viewport.y);
	}
    //credits Terin - http://www.gamedev.net/community/forums/topic.asp?topic_id=259860
    
	const int NUMPOINTS = 10;
	SDL_Point Circle[NUMPOINTS + 1 +1];
	int i;
	float X;
	float Y;
	float Theta;
	float WedgeAngle;	//Size of angle between two points on the circle (single wedge)

	//Precompute WedgeAngle
	WedgeAngle = (float)((PI2) / NUMPOINTS);

	//Set up vertices for a circle
	//Used <= in the for statement to ensure last point meets first point (closed circle)
	for(i=0; i<=NUMPOINTS; i++)
	{
		//Calculate theta for this vertex
		Theta = i * WedgeAngle;
		
		//Compute X and Y locations
		X = (float)(pt.x + radius * cos(Theta));
		Y = (float)(pt.y - radius * sin(Theta));

		Circle[i].x=X;
		Circle[i].y=Y;
	}
	Circle[NUMPOINTS + 1].x=Circle[0].x;
	Circle[NUMPOINTS + 1].y=Circle[0].y;

	int a,r,g,b;
	Drawing_GetColorComponents(color,a,r,g,b);
	SDL_SetRenderDrawColor(di->renderer,r,g,b,a);
	SDL_RenderDrawLines(di->renderer,
                        Circle,
                        NUMPOINTS+1);
    
    
}//Draw2DCircle



void Drawing_DrawText(Drawing_Info* dic, const char* str, RECT draw,unsigned int format,int color,bool lastsprite)
{
	if(dic==NULL) return;
	if(str==NULL || strlen(str)==0) return;
	Drawing_InfoX* di=(Drawing_InfoX*) dic;
	int a,r,g,b;
	Drawing_GetColorComponents(color,a,r,g,b);

	int w=(draw.right-draw.left)+1;
	int h=(draw.bottom-draw.top)+1;
	TTF_SizeText(di->font,str,&w,&h);

	
	SDL_Color bcolor={0,0,0};


	SDL_Color scolor={r,g,b};
	SDL_Surface *text_surface=NULL;

	switch(di->text_quality){
	case TEXT_SOLID  : { text_surface=TTF_RenderText_Solid  (di->font,str,scolor       ); break; }
	case TEXT_SHADED : { text_surface=TTF_RenderText_Shaded (di->font,str,scolor,bcolor); break; }
	case TEXT_BLENDED: { text_surface=TTF_RenderText_Blended(di->font,str,scolor       ); break; }
	}


	if(!text_surface) {
		System_Alert("Drawing_DrawText: SDL RenderText error.");
		//handle error here, perhaps print TTF_GetError at least
	} else {
		SDL_Texture* tex= SDL_CreateTextureFromSurface(di->renderer, text_surface);
		if(text_surface!=NULL) SDL_FreeSurface(text_surface);
		SDL_Rect srt={0        ,       0,w,h};
		SDL_Rect drt={draw.left,draw.top,w,h};
		SDL_Point ctr={0,0};
		if(tex!=NULL) {
			SDL_RenderCopyEx(di->renderer,tex,NULL,&drt,0.0,&ctr,SDL_FLIP_NONE);
			SDL_DestroyTexture(tex);
		}
	}

	/*
	LPD3DXSPRITE spr=NULL;
	//if(lastsprite || true) 
	spr=di->spriteObj;
	D3DXMATRIX mat;
	D3DXMatrixIdentity(&mat);
	spr->SetTransform(&mat);
	di->fontObj->DrawTextA(spr, str, -1, &draw, format, color );
	*/
}

void Drawing_DrawTextMultiline(Drawing_Info* dic, const char* str, RECT draw,unsigned int format,int color,bool lastsprite)
{
	if(dic==NULL) return;
	string s="";
	int l=strlen(str);
	for(int i=0;i<l;i++){
		if(str[i]==0x0A || str[i]==0x00){
			Drawing_DrawText(dic, s.c_str(), draw,format,color,lastsprite);
			draw.top   +=25;
			draw.bottom+=25;
			s="";
			if(str[i]==0x00) break;
			continue;
		}
		s+=str[i];
	}
	if(s.length()>0) Drawing_DrawText(dic, s.c_str(), draw,format,color,lastsprite);
}


void Drawing_Begin(Drawing_Info* dic)
{
	if(dic==NULL) return;
	Drawing_InfoX* di=(Drawing_InfoX*) dic;
	if(di->fade==0xFF){ SDL_RenderClear(di->renderer); return; }
	SDL_SetRenderDrawColor(di->renderer,0,0,0,di->fade);
	SDL_RenderFillRect(di->renderer,NULL);
}
void Drawing_End(Drawing_Info* dic)
{
	/*
	Drawing_InfoX* di=(Drawing_InfoX*) dic;
   di->spriteObj->End();
   di->d3ddev->EndScene();
   */
}
void Drawing_Present(Drawing_Info* dic)
{
	if(dic==NULL) return;
	Drawing_InfoX* di=(Drawing_InfoX*) dic;
	SDL_RenderPresent(di->renderer);
}
