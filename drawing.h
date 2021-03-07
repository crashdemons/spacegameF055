/**
@file drawing.h
@brief abstracted 2D drawing functions and structures

...
@author crashenator (crashenator -at- gmail.com)
*/


#pragma once

#include "common.h"

class fPOINT///floating-point 2D point.
{
public:
	float x,y;
	float xrel, yrel;
	bool isRelative;

	float getPositionByIndex(int i);
	void setRelative(fPOINT fp);
	void makeRelative(fPOINT fp);
	void makeAbsolute();
	fPOINT getAbsolute() const;
	fPOINT getRelative(fPOINT ptRel) const;

	void debug();

	fPOINT();
	fPOINT(float px, float py);
	fPOINT(float px, float py, fPOINT ptRel);

};
class fRECT{/// floating-point rectangle
public:
    float left;
    float right;
    float top;
    float bottom;
	bool isRelative;
	fPOINT ptRelative;

	void setRelative(fPOINT fp);
	void makeRelative(fPOINT fp);
	void makeAbsolute();
	fRECT getAbsolute() const;
	fRECT getRelative(fPOINT ptRel) const;

	fRECT();
	fRECT(float l, float r, float t, float b);
	fRECT(float l, float r, float t, float b,fPOINT ptRel);


	/*float width();
	float height();
	RECT approx();
	fRECT dim();*/
};

//----------------------------------------------------
enum TextQuality
{
	TEXT_NONE,
	TEXT_SOLID,
	TEXT_SHADED,
	TEXT_BLENDED
};


class Drawing_Info///base structure for drawing library information
{
public:
	char fade;
	System_Hwnd hWnd;
    HDC hdc;
    RECT rect;
    RECT world;
	fPOINT viewport;
    int width;
    int height;
    float scale;
    bool windowed;
	bool centering;
	TextQuality text_quality;
	virtual ~Drawing_Info()=0;
};
//----------------------------------------------------

enum BOUNDARYTYPE{/// enumeration of boundary types
	BOUNDARY_NONE=0,///< no boundary. checks for intersections and collisions fail.
	BOUNDARY_RADIUS=1,///< a circular boundary based on distance from a center point
	BOUNDARY_POLY=2,///< a polygon
	BOUNDARY_LINE=4,///< a 2D line
	BOUNDARY_POINT=8,///< a 2D point
	BOUNDARY_COMPOSITE_AND=16,///< a boundary that is defined of other boundary types, all of which must all be satisfied.
	BOUNDARY_COMPOSITE_OR=32///< a boundary that is defined of other boundary types, only one of which must be satisfied.
};
#define BOUNDARY_CIRCLE BOUNDARY_RADIUS
#define BOUNDARY_BOX BOUNDARY_POLY
#define BOUNDARY_QUAD BOUNDARY_POLY
#define COLLIS(x,y) BOUNDARY_##x|BOUNDARY_##y
enum COLLISTYPE{
	COLLIS_NONE,
	COLLIS_COMPOSITES=BOUNDARY_COMPOSITE_AND,
	COLLIS_COMPOSITES1=BOUNDARY_COMPOSITE_OR,
	COLLIS_COMPOSITES2=COLLIS(COMPOSITE_AND,COMPOSITE_OR),
	COLLIS_COMPOSITESIMPLE=COLLIS(COMPOSITE_AND,POINT),

	COLLIS_POINTS=BOUNDARY_POINT,
	COLLIS_POINTCIRCLE=COLLIS(POINT,CIRCLE),
	COLLIS_POINTLINE=COLLIS(POINT,LINE),
	COLLIS_POINTPOLY=COLLIS(POINT,POLY),

	COLLIS_CIRCLES=BOUNDARY_CIRCLE,
	COLLIS_CIRCLELINE=COLLIS(CIRCLE,LINE),
	COLLIS_CIRCLEPOLY=COLLIS(CIRCLE,POLY),
	COLLIS_LINES=BOUNDARY_LINE,
	COLLIS_LINEPOLY=COLLIS(LINE,POLY),
	COLLIS_POLYS=BOUNDARY_POLY
};

class Boundary{/// a boundary logic class implementing collision detection
private:
	bool __modchecklock;
	float rotation;
	fPOINT shift;
	unsigned int modifications;
	vector<fPOINT> unmodified_points;
	vector<Boundary> unmodified_composites;
	fPOINT unmodified_center;

	void __resync();
	void __modcheck();

	COLLISTYPE getCollisType(const Boundary& b);
	COLLISTYPE getCollisArgs(Boundary& b,Boundary** ppA,Boundary** ppB);

public:
	BOUNDARYTYPE type;
	fPOINT center;//center
	float radius,length, width;

	vector<fPOINT> points;
	vector<Boundary> composites;

	void invalidate();
	fPOINT recalculateCenter(int depth=0);
	void setComposites(vector<Boundary> bnds);
	void setPoints(vector<fPOINT> pts);
	void setCenter(float x, float y);///< sets a new center for the boundary, shifting all points.
	void move(float dx, float dy);///< shifts all points by a given amounts
	void rotate(float radians,fPOINT* pVertex=NULL);

	bool collide(Boundary& b);
	
	bool isCenterFixedType() const;///< is the boundary a circle or a point?  both of these types are defined by their centers and not on points.
	bool isComposite() const;
	void addComposite(Boundary b);


	void draw(Drawing_Info* di,bool drawcenter=true);
	void debug();
	
	
	
	Boundary(BOUNDARYTYPE t, vector<Boundary> b);//composites
	Boundary(BOUNDARYTYPE t, vector<fPOINT> p);
	Boundary(BOUNDARYTYPE t, fPOINT p, float fRadius);
	Boundary();

};


//----------------------------------------------------





class Image_Info///class defining a single drawn image and placement behavior.
{
public:
	    string image;
        int transcolor;
        RECT SrcRect;
        fRECT DstRect;
		Boundary rectangle;
		Boundary circle;
        float rotation;
        fPOINT center;
        float radius;
        int mask;

		bool requires_boundaries;

		
		bool viewcentered;
		bool viewrelative;
		float relativemult;

		void RecalculateCircle();
		void RecalculateRectangle();
		void SyncBoundaries();

		virtual void Debug();
		virtual void Unload();
		virtual void Reload(Drawing_Info* di);
		void RotateTo(float r);
		void Rotate(float r);
		
		void CornerMove(float dx, float dy);
		void CornerMoveTo(float x, float y);
		void Move(float dx, float dy);
		void MoveTo(float x, float y);
        /*

        void Unload();
        void Reload(Drawing_Info* di);
		//for radius and center calculation
		*/
		Image_Info();
		virtual ~Image_Info();
};

class Gradient{///class used for generating a smooth range of ARGB colors from specific given colors
public:
	int colors[256];
	void fill(vector<int> primaries);	///< generates the color range, taking a vector of ARGB colors.
										///< the given colors (primaries) twill be evenly distributed through the range then intermediate colors are then blended between the primaries (average weighted by distance between primaries)
};


float dist(float x1, float y1, float x2, float y2);
inline float dist(fPOINT a, fPOINT b);
void rotate(fPOINT& point, const fPOINT center, const float radians);

#define round(d) floor(d+0.5f)
fPOINT midpoint(fPOINT a, fPOINT b);
fPOINT centroid(vector<fPOINT> p);
float midpoint(float a,float b);
float midpoint(int a,int b);



POINT Dim(const RECT& r);
fPOINT Dim(const fRECT& fr);
fRECT ShiftFRECT(fRECT fr, float dx, float dy);
fPOINT Drawing_GetCenter(const RECT& r);
fPOINT Drawing_GetCenter(const fRECT& r);
void Drawing_CenterOn(fRECT& fr, const fPOINT& pt);



fRECT ShiftFRECT(fRECT fr, float dx, float dy);
fRECT Drawing_GetFRECT(RECT r);
RECT Drawing_ApproxRECT(fRECT fr);



unsigned int Drawing_GetColor(int r, int g, int b);
unsigned int Drawing_GetColor(int a, int r, int g, int b);
void Drawing_GetColorComponents(unsigned int color, int& a, int& r, int& g, int& b);

//--------------------------------

Drawing_Info* Drawing_Startup(System_Info* si, int width, int height, float scale,bool windowed);
Drawing_Info* Drawing_Startup(Drawing_Info* dic);
void Drawing_Shutdown(Drawing_Info* di,bool deletedi=true);
Image_Info* Drawing_LoadImage(Drawing_Info* di, string filename, float rotation, int transcolor, bool scaled=true,int scaleoverride=0,bool requires_boundaries=false);
void Drawing_UnloadImage(Image_Info* ii);


void Drawing_CenterViewOn(Drawing_Info* di, const fPOINT& pt);
void Drawing_CenterViewOn(Drawing_Info* di, Image_Info* ii);
void Drawing_CenterViewAdjust(Drawing_Info* di, float dx,float dy);

void Drawing_RotateImage(Drawing_Info* di, Image_Info* ii);
void Drawing_DrawImage(Drawing_Info* di,Image_Info* ii);
void Drawing_DrawPoint(Drawing_Info* dic, fPOINT draw1, int color);
void Drawing_DrawLine(Drawing_Info* dic, int x1,int y1, int x2, int y2, int color,bool relative=true);
void Drawing_DrawLine(Drawing_Info* di, fRECT draw, int color);
void Drawing_DrawLines(Drawing_Info* dic, vector<fPOINT> points,int color,bool relative=true);///< draw a series of line segments.
void Drawing_DrawPoly(Drawing_Info* dic, vector<fPOINT> points,int color,bool relative=true);///< draw a polygon - this is the same as DrawLines except the last point is connect to the first.
void Drawing_DrawRect(Drawing_Info* di, fRECT draw, int color, bool relative=true, float relmult=1.0f);
void Drawing_DrawRectFilled(Drawing_Info* di, fRECT draw, int color, bool relative=true, float relmult=1.0f);
void Drawing_DrawCircle(Drawing_Info* di, fPOINT pt, float radius, int color, bool relative=true, float relmult=1.0f);
void Drawing_DrawText(Drawing_Info* di, const char* str, RECT draw,unsigned int format=0,int color=0xFFFFFFFF,bool lastsprite=false);
void Drawing_DrawTextMultiline(Drawing_Info* dic, const char* str, RECT draw,unsigned int format=0,int color=0xFFFFFFFF,bool lastsprite=false);
void Drawing_Begin(Drawing_Info* di);
void Drawing_End(Drawing_Info* di);
void Drawing_Present(Drawing_Info* di);

/*
we could have a drawing class.... but how would subclasses elsewhere access it from Game?
class Drawing
{
public:
	int getColor();
	bool Startup();
	bool Begin();
	bool Present();
	bool End();
	bool Shutdown();
}
*/
