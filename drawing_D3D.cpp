#include "drawing_D3D.h"

Drawing_InfoX::~Drawing_InfoX(){}
Image_InfoX::~Image_InfoX(){}


void Image_InfoX::Unload(){if(tex!=NULL){ tex->Release();tex=NULL;}}
void Image_InfoX::Reload(Drawing_Info* dic)
{
	Drawing_InfoX* di=(Drawing_InfoX*) dic;
    if(D3DXCreateTextureFromFileEx(di->d3ddev,image,dxi.Width,dxi.Height,1,D3DPOOL_DEFAULT,D3DFMT_UNKNOWN,D3DPOOL_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,transcolor,&dxi,NULL,&tex)!=D3D_OK)
    {
        MessageBox(NULL,"Image Texture reload failed.",image,MB_OK);
    }
}

        
Vertex3TL CreateVertex3TL   (float X, float Y, float Z, float RHW, D3DCOLOR color, float U, float V)
{
	Vertex3TL v;
	v.fX = X;
	v.fY = Y;
	v.fZ = Z;
	v.fRHW = RHW;
	v.Color = color;
	v.fU = U;
	v.fV = V;
	return v;
}//CreateD3DTLVERTEX


Drawing_Info* Drawing_Startup(HWND hWnd, int width, int height, float scale,bool windowed)
{
    //if(!windowed) 
	ShowCursor(FALSE);
    Drawing_InfoX* di = new Drawing_InfoX();
    di->hWnd=hWnd;
    di->hdc=GetDC(hWnd);
    di->rect.left=0;
    di->rect.right=width;
    di->rect.top=0;
    di->rect.bottom=height;
    di->scale=scale;
    di->windowed=windowed;
    di->width=width;
    di->height=height;
	di->viewport.x=0;
	di->viewport.y=0;
	di->centering=true;
    
    
    di->d3d = Direct3DCreate9(D3D_SDK_VERSION);
	if(di->d3d==NULL){MessageBox(hWnd,"Error: Initialization of Direct3D Failed.","Error",MB_OK); return 0;}




	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed=windowed;
	d3dpp.SwapEffect=D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat=D3DFMT_X8R8G8B8;
	d3dpp.BackBufferCount = 1;
	d3dpp.BackBufferWidth=width;
	d3dpp.BackBufferHeight=height;
	d3dpp.hDeviceWindow=hWnd;
	//d3dpp.

	//create D3D device
	// adapter devicetype shownwindow behaviorflags(HW/SW) presentationParams DeviceInterface
	di->d3d->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,hWnd,D3DCREATE_SOFTWARE_VERTEXPROCESSING,&d3dpp,&(di->d3ddev));
	if(di->d3ddev==NULL){System_Alert("Error: Creation of Direct3D Device Failed.","Error"); return 0;}
	

	
	HRESULT result;
	//result=di->d3ddev->CreateOffscreenPlainSurface(width, height,D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &(di->backSurface), NULL);
    
    di->d3ddev->GetBackBuffer(0,0,D3DBACKBUFFER_TYPE_MONO,&(di->backBuffer));
    
	result=D3DXCreateSprite(di->d3ddev, &(di->spriteObj));
	if (result != D3D_OK) 
		System_Alert("CreateSprite Failed","Direct3D Error");
    
	result=D3DXCreateFont(di->d3ddev, 20, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &(di->fontObj) );
	if (result != D3D_OK) 
		System_Alert("CreateFont Failed","Direct3D Error");

	
    return di;
}
Drawing_Info* Drawing_Startup(Drawing_Info* dic)
{
    Drawing_Info* di=Drawing_Startup(dic->hWnd, dic->width,dic->height, dic->scale,dic->windowed);
	di->viewport.x=0;
	di->viewport.y=0;
    delete dic;
    return di;
}

void Drawing_Shutdown(Drawing_Info* dic,bool deletedi)
{
	Drawing_InfoX* di=(Drawing_InfoX*) dic;
	ShowCursor(TRUE);
	ReleaseDC(di->hWnd,di->hdc);
	di->fontObj->Release();
	di->spriteObj->Release();
	di->backBuffer->Release();
	//di->backSurface->Release();
	if(di->d3ddev!=NULL) di->d3ddev->Release();
	if(di->d3d!=NULL) di->d3d->Release();
	
    if(deletedi) delete di;
}
/*
Drawing_Info* Drawing_SwitchWindowed(Drawing_Info* dic)
{
	Drawing_InfoX* di=(Drawing_InfoX*) dic;
    //kept getting an undocumented error code from device.reset(), so lets do this the hard way
    Drawing_Info* dic=new Drawing_Info();
    memcpy(dic,di,sizeof(Drawing_Info));
    dic->windowed=!dic->windowed;
    
    
    Drawing_Shutdown(di);
    if(dic->windowed) ShowWindow(GetDesktopWindow(),SW_SHOW);
    if(dic->windowed) Sleep(250);
    di=Drawing_Startup(dic->hWnd, dic->width,dic->height, dic->scale,dic->windowed);
    
    return di;
}
*/

unsigned int Drawing_GetColor(int r, int g, int b){return D3DCOLOR_XRGB(r,g,b);}
unsigned int Drawing_GetColor(int a, int r, int g, int b){return D3DCOLOR_ARGB(a,r,g,b);}

void Drawing_GetColorComponents(unsigned int color, int& a, int& r, int& g, int& b)
{
	a=(color&0xFF000000)>>24;
	r=(color&0x00FF0000)>>16;
	g=(color&0x0000FF00)>>8;
	b=(color&0x000000FF)   ;
}

Image_Info* Drawing_LoadImage(Drawing_Info* dic, const char* filename, float rotation, int transcolor, bool scaled)
{
	Drawing_InfoX* di=(Drawing_InfoX*) dic;
    HRESULT result;
	Image_InfoX* ii=NULL;
//	try
//	{
		ii=new Image_InfoX();
//	}catch (bad_alloc& ba){
//		MessageBox("Zomg","Zomg");
//	}


    result = D3DXGetImageInfoFromFile(filename, &(ii->dxi));
    if (result != D3D_OK) 
		System_Alert(string("GetImageInfo Failed\n")+filename,"Direct3D Error");

    result = D3DXCreateTextureFromFileExA(di->d3ddev,filename,ii->dxi.Width,ii->dxi.Height,1,D3DPOOL_DEFAULT,D3DFMT_UNKNOWN,D3DPOOL_DEFAULT,D3DX_DEFAULT,D3DX_DEFAULT,transcolor,&(ii->dxi),NULL,&(ii->tex));
    if (result != D3D_OK) 
		System_Alert(string("CreateTextureFromFile Failed\n")+filename,"Direct3D Error");

    ii->transcolor=transcolor;
    ii->image=filename;
    ii->SrcRect.left=0;
    ii->SrcRect.top=0;
    ii->SrcRect.right=ii->dxi.Width;//possibly needs to be -1 since 0 to 0 is one pixel(one pixel on x=0)
    ii->SrcRect.bottom=ii->dxi.Height;

	float scale=1.0f;
	if(scaled) scale=di->scale;
    
    ii->DstRect.left=0;
    ii->DstRect.top=0;
    ii->DstRect.right=ii->dxi.Width*scale;//possibly needs to be -1 since 0 to 0 is one pixel(one pixel on x=0)
    ii->DstRect.bottom=ii->dxi.Height*scale;
    
    ii->rotation=rotation;
	

	ii->RecalculateCircle();
	/*
	ii->center.x=midpoint(ii->DstRect.left,ii->DstRect.right );//((ii->DstRect.right)-(ii->DstRect.left))/2;//maybe +1 (as above)
    ii->center.y=midpoint(ii->DstRect.top ,ii->DstRect.bottom);//((ii->DstRect.bottom)-(ii->DstRect.top))/2;
    ii->radius=(ii->center.x + ii->center.y)/2;
	*/


    ii->mask = D3DCOLOR_ARGB(255,255,255,255);
    
    

	ii->relativemult=1.0f;

    return ii;
}
void Drawing_UnloadImage(Image_Info* iic)
{
	Image_InfoX* ii=(Image_InfoX*) iic;
    ii->tex->Release();
    delete ii;
	ii=NULL;
}


void Drawing_RotateImage(Drawing_Info* dic, Image_Info* ii)
{
	Drawing_InfoX* di=(Drawing_InfoX*) dic;
    D3DXMATRIX mat;

	Vector2 scale((ii->DstRect.right-ii->DstRect.left)/(ii->SrcRect.right-ii->SrcRect.left),(ii->DstRect.bottom-ii->DstRect.top)/(ii->SrcRect.bottom-ii->SrcRect.top));
	//Vector2 scale(1,1);

	float topy=ii->DstRect.top;
	float leftx=ii->DstRect.left;

	if(ii->viewrelative){
		leftx-=ii->relativemult * di->viewport.x;
		topy-=ii->relativemult * di->viewport.y;
	}


	Vector2 trans(leftx,topy);
	Vector2 center(ii->center.x,ii->center.y);
	D3DXMatrixTransformation2D(&mat,NULL,0,&scale,&center,ii->rotation,&trans);
    di->spriteObj->SetTransform(&mat);
}
void Drawing_DrawImage(Drawing_Info* dic,Image_Info* iic)
{
	Image_InfoX* ii=(Image_InfoX*) iic;
	Drawing_InfoX* di=(Drawing_InfoX*) dic;
    if(ii==NULL) return;
    //if(ii->rotation!=0 || di->scale!=1) 
	Drawing_RotateImage(di, ii);
	D3DXVECTOR3 pos(0.0f,0.0f,1.0f);
	di->spriteObj->Draw(ii->tex, &(ii->SrcRect), NULL, &pos, ii->mask);
}

void Drawing_DrawLine(Drawing_Info* dic, fRECT draw1, int color)
{
	Drawing_InfoX* di=(Drawing_InfoX*) dic;
    /* TODO (#1#): test Drawing_DrawLine */
	
	fRECT draw=ShiftFRECT(draw1,di->viewport.x,di->viewport.y);

    Vertex3C vertices[2];
	vertices[0].x = draw.left; // top left
	vertices[0].y = draw.top;
	vertices[0].z = 1.0f;
	vertices[0].color = color;
	vertices[1].x = draw.right; // top left
	vertices[1].y = draw.bottom;
	vertices[1].z = 1.0f;
	vertices[1].color = color;
    di->d3ddev->DrawPrimitiveUP( D3DPT_LINESTRIP, 1, (void*) vertices, sizeof( Vertex3C ) );
}
void Drawing_DrawRect(Drawing_Info* dic, fRECT draw1, int color, bool relative, float relmult)
{
	Drawing_InfoX* di=(Drawing_InfoX*) dic;
	fRECT draw;
	if(relative) draw=ShiftFRECT(draw1,relmult * (di->viewport.x), relmult * (di->viewport.y));
	else         draw=draw1;
	Vertex3C vertices[5];
	vertices[0].x = draw.left; // top left
	vertices[0].y = draw.top;
	vertices[0].z = 0.0f;
	vertices[0].color = color;
	vertices[1].x = draw.right; // top right
	vertices[1].y = draw.top;
	vertices[1].z = 0.0f;
	vertices[1].color = color;
	vertices[2].x = draw.right; // right bottom
	vertices[2].y = draw.bottom;
	vertices[2].z = 0.0f;
	vertices[2].color = color;
	vertices[3].x = draw.left; // left bottom
	vertices[3].y = draw.bottom;
	vertices[3].z = 0.0f;
	vertices[3].color = color;
	vertices[4].x = draw.left; // top left again
	vertices[4].y = draw.top;
	vertices[4].z = 0.0f;
	vertices[4].color = color;
	


	//DWORD prevFVF;
    //HRESULT hResult = di->d3ddev->GetFVF(&prevFVF);
   // di->d3ddev->SetFVF(D3DFVF_TL);
	//di->d3ddev->SetTexture(0, NULL);
	di->d3ddev->DrawPrimitiveUP( D3DPT_LINESTRIP, 4, (void*) vertices, sizeof( Vertex3C ) );
	//if(hResult==D3D_OK) di->d3ddev->SetFVF(prevFVF);
}
void Drawing_DrawRectFilled(Drawing_Info* di, fRECT draw, int color, bool relative, float relmult)
{
	//fRECT draw;
	//if(relative) draw=ShiftFRECT(draw1,relmult * (di->viewport.x), relmult * (di->viewport.y));
	//else         draw=draw1;

	/*
    int h=(int)(draw.bottom-draw.top) + 1;
	Vertex3C* vertices=new Vertex3C[h*2];//no dynamic arrays in C++ unless you want to use vectors.
	int i=0;


	for(float y=draw.top;y<=draw.bottom;y++)
	{
       if(i>(h*2)) break;
       vertices[i].x = draw.left;
	   vertices[i].y = y;
	   vertices[i].z = 0.0f;
	   vertices[i].color = color;
	   i++;
	   if(i>(h*2)) break;
	   vertices[i].x = draw.right;
	   vertices[i].y = y;
	   vertices[i].z = 0.0f;
	   vertices[i].color = color;
	   i++;
    }
	*/

	bool width=true;
	bool height=true;
	do
	{
		Drawing_DrawRect(di, draw, color, relative, relmult);
		width =draw.top <draw.bottom;
		height=draw.left<draw.right;
		if(width ) { draw.top++;  draw.bottom--; }
		if(height) { draw.left++; draw.right--;  }
	}while( width && height );

	


	//di->d3ddev->DrawPrimitiveUP( D3DPT_LINESTRIP, h*2, (void*) vertices, sizeof( Vertex3C ) );
	
	//delete[] vertices;
}



//-----------------------------------------------------------------------------------
//Draw a simple circle centered around the given point, using the given
//radius and color.  This is in Screen Space (2D).
//-----------------------------------------------------------------------------------
void Drawing_DrawCircle(Drawing_Info* dic, fPOINT pt, float radius, int color, bool relative, float relmult)
{
	Drawing_InfoX* di=(Drawing_InfoX*) dic;
	if(relative){
		pt.x-=relmult * (di->viewport.x);
		pt.y-=relmult * (di->viewport.y);
	}
    //credits Terin - http://www.gamedev.net/community/forums/topic.asp?topic_id=259860
    
	const int NUMPOINTS = 24;
	Vertex3TL Circle[NUMPOINTS + 1];
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

		Circle[i] = CreateVertex3TL(X, Y, 0.0f, 1.0f, color, 0.0f, 0.0f);
	}

	//Now draw the circle
	DWORD prevFVF;
    HRESULT hResult = di->d3ddev->GetFVF(&prevFVF);

    di->d3ddev->SetFVF(D3DFVF_TL);
	di->d3ddev->SetTexture(0, NULL);
	di->d3ddev->DrawPrimitiveUP(D3DPT_LINESTRIP, NUMPOINTS, &Circle[0], sizeof(Circle[0]));
    
    if(hResult==D3D_OK) di->d3ddev->SetFVF(prevFVF);
    
    /* DONE (#1#): Make DrawCircle's FVF mode stop interfering with 
                   other drawings */
    
}//Draw2DCircle



void Drawing_DrawText(Drawing_Info* dic, const char* str, RECT draw,unsigned int format,int color,bool lastsprite)
{
	
	Drawing_InfoX* di=(Drawing_InfoX*) dic;
	LPD3DXSPRITE spr=NULL;
	//if(lastsprite || true) 
	spr=di->spriteObj;
	D3DXMATRIX mat;
	D3DXMatrixIdentity(&mat);
	spr->SetTransform(&mat);
	di->fontObj->DrawTextA(spr, str, -1, &draw, format, color );
}


void Drawing_Begin(Drawing_Info* dic)
{
	Drawing_InfoX* di=(Drawing_InfoX*) dic;
	di->d3ddev->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE,TRUE);//works, looks good, so why not?

	//D3DVIEWPORT9 vp={20,20,200,200,0.0f,1.0f};//includes a rectangle for drawing, could be useful for future  container-HUD dev, possibly unused though.
	//di->d3ddev->SetViewport(&vp);


	//di->d3ddev->SetRenderState(D3DRS_ZENABLE,D3DZB_FALSE);
    di->d3ddev->Clear(0,NULL,D3DCLEAR_TARGET,D3DCOLOR_XRGB(0,0,0), 1.0f, 0);
	di->d3ddev->BeginScene();
    di->spriteObj->Begin(D3DXSPRITE_ALPHABLEND);
}
void Drawing_End(Drawing_Info* dic)
{
	Drawing_InfoX* di=(Drawing_InfoX*) dic;
   di->spriteObj->End();
   di->d3ddev->EndScene();
}
void Drawing_Present(Drawing_Info* dic)
{
	Drawing_InfoX* di=(Drawing_InfoX*) dic;
   di->spriteObj->End();
   di->d3ddev->Present(NULL, NULL, NULL, NULL);
}
