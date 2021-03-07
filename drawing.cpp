#include "drawing.h"

#define BOUND_DEBUG false

inline void fRECT::setRelative(fPOINT fp){
	fp.makeAbsolute();
	ptRelative=fp;
	isRelative=true;
}
inline void fRECT::makeRelative(fPOINT fp){
	fp.makeAbsolute();
	setRelative(fp);
	left-=fp.x;
	right-=fp.x;
	top-=fp.y;
	bottom-=fp.y;
}
inline void fRECT::makeAbsolute(){
	if(!isRelative) return;
	fPOINT ptAbsRel=ptRelative.getAbsolute();
	left+=ptAbsRel.x;
	right+=ptAbsRel.x;
	top+=ptAbsRel.y;
	bottom+=ptAbsRel.y;
	isRelative=false;
}
inline fRECT fRECT::getAbsolute() const{
	if(!isRelative) return *this;
	fPOINT ptAbsRel=ptRelative.getAbsolute();
	return fRECT(left+ptAbsRel.x,right+ptAbsRel.x,top+ptAbsRel.y,bottom+ptAbsRel.y);
}
inline fRECT fRECT::getRelative(fPOINT ptRel) const{
	ptRel.makeAbsolute();
	fRECT abs=getAbsolute();
	return fRECT(abs.left-ptRel.x,abs.right-ptRel.x,abs.top-ptRel.y,abs.bottom-ptRel.y,ptRel);
}
fRECT::fRECT(){
	isRelative=false;
	ptRelative=fPOINT(0,0);
}
fRECT::fRECT(float l, float r, float t, float b){
	left=l; right=r; top=t; bottom=b;
	isRelative=false;
	ptRelative=fPOINT(0,0);
}
fRECT::fRECT(float l, float r, float t, float b, fPOINT ptRel){
	left=l; right=r; top=t; bottom=b;
	setRelative(ptRel);
}
//---------------------
void fPOINT::debug(){
	if(isRelative){
		cout<<"("<<xrel<<","<<yrel<<" : "<<x<<","<<y<<")";
	}else{
		cout<<"("<<x<<","<<y<<")";
	}
}
inline float fPOINT::getPositionByIndex(int i){
	if(i==0) return x;
	return y;
}
inline void fPOINT::makeRelative(fPOINT fp){
	fp.makeAbsolute();
	xrel=fp.x;
	yrel=fp.y;
	x-=xrel;
	y-=yrel;
	isRelative=true;
}
inline void fPOINT::setRelative(fPOINT fp){
	fp.makeAbsolute();
	xrel=fp.x;
	yrel=fp.y;
	isRelative=true;
}
inline void fPOINT::makeAbsolute()
{
	if(!isRelative) return;
	x+=xrel;
	y+=yrel;
	isRelative=false;
}
inline fPOINT fPOINT::getAbsolute() const
{
	if(!isRelative) return *this;
	return fPOINT(x + xrel, y + yrel);
}
inline fPOINT fPOINT::getRelative(fPOINT ptRel) const
{
	ptRel.makeAbsolute();
	fPOINT abs=getAbsolute();
	return fPOINT(abs.x-ptRel.x,abs.y-ptRel.y,ptRel);
}
fPOINT::fPOINT(){
	x=0;y=0;
	xrel=0;yrel=0;
	isRelative=false;
}
fPOINT::fPOINT(float px, float py){
	x=px; y=py;
	xrel=0;yrel=0;
	isRelative=false;
}
fPOINT::fPOINT(float px, float py, fPOINT ptRel){
	x=px; y=py;
	setRelative(ptRel);
}
//-------------------------------------------------------
#define FAVG_INTS(x,y) (((float)x/(float)y)/2.0f)

#define WAVG_INTS(w,x,y) ( ((float)x)*(1.0-w) + ((float)y)*w ); //FAVG(x,y) would be approximately the same as WAVG(0.5,x,y)

int blend(int c1, int c2)
{
	int a1,r1,g1,b1;
	int a2,r2,g2,b2;
	Drawing_GetColorComponents(c1, a1,r1,g1,b1);
	Drawing_GetColorComponents(c2, a2,r2,g2,b2);
	a1=FAVG_INTS(a1,a2);
	r1=FAVG_INTS(r1,r2);
	g1=FAVG_INTS(g1,g2);
	b1=FAVG_INTS(b1,b2);
	return Drawing_GetColor(a1,r1,g1,b1);
}
int blend(float w1, int c1, int c2){
	int a1,r1,g1,b1;
	int a2,r2,g2,b2;
	Drawing_GetColorComponents(c1, a1,r1,g1,b1);
	Drawing_GetColorComponents(c2, a2,r2,g2,b2);
	a1=WAVG_INTS(w1,a1,a2);
	r1=WAVG_INTS(w1,r1,r2);
	g1=WAVG_INTS(w1,g1,g2);
	b1=WAVG_INTS(w1,b1,b2);
	return Drawing_GetColor(a1,r1,g1,b1);
}


void Gradient::fill(vector<int> primaries){
	//basic idea:  we have a 256-color table for the gradient, we will set our primary (chosen) colors spaced equally along the gradient, then blend in the empty gaps.

	memset(colors,0,sizeof(int)*256);
	float max=primaries.size()-1;//first color is always at the front, other colors are at index-defined positions
	float n=0;

	
	for(vector<int>::iterator it=primaries.begin(); it!=primaries.end(); it++){
		//set the primary colors along the gradient in locations based on their position in the input array.  (eg: for 3 colors, 0/2, 1/2, 2/2)
		float percent=n/max;
		int i=round(255*percent);
		colors[i]=*it;

		if(n!=max){//blend colors between this and the next color
			float percent2=(n+1)/max;
			int i2=round(255*percent2);
			int c2=*(it+1);

			float n3=0;
			float max3=(i2-i);
			for(int j=i+1; j<i2; j++){
				n3=j-i;
				colors[j]=blend(n3/max3,colors[i],c2);
			}
		}

		n++;
	}

	//blend colors in between primaries
	for(vector<int>::iterator it=primaries.begin(); it!=primaries.end(); it++){
		float percent=n/max;
		int idx=round(255*percent);
		colors[idx]=*it;
		n++;
	}




	
}

//---------------------------------------------------------------------------------------------------

#define dbgtm(x) if(t==x) return string(#x)

string dbgcollistype(COLLISTYPE t){
	dbgtm(COLLIS_NONE);
	dbgtm(COLLIS_COMPOSITES);
	dbgtm(COLLIS_COMPOSITES1);
	dbgtm(COLLIS_COMPOSITES2);
	dbgtm(COLLIS_COMPOSITESIMPLE);
	dbgtm(COLLIS_POINTS);
	dbgtm(COLLIS_POINTCIRCLE);
	dbgtm(COLLIS_POINTLINE);
	dbgtm(COLLIS_POINTPOLY);
	dbgtm(COLLIS_CIRCLES);
	dbgtm(COLLIS_CIRCLELINE);
	dbgtm(COLLIS_CIRCLEPOLY);
	dbgtm(COLLIS_LINES);
	dbgtm(COLLIS_LINEPOLY);
	dbgtm(COLLIS_POLYS);
	return string("COLLIS_UNKNOWN");
}
string dbgboundtype(BOUNDARYTYPE t){
	dbgtm(BOUNDARY_NONE);
	dbgtm(BOUNDARY_RADIUS);
	dbgtm(BOUNDARY_POLY);
	dbgtm(BOUNDARY_LINE);
	dbgtm(BOUNDARY_POINT);
	dbgtm(BOUNDARY_COMPOSITE_AND);
	dbgtm(BOUNDARY_COMPOSITE_OR);
	return string("BOUNDARY_UNKNOWN");
}



float midpoint(float a,float b){ return (a+b)/2; }
float midpoint(int a,int b){ return midpoint((float)a,(float)b); }
inline fPOINT midpoint(fPOINT a, fPOINT b){
	fPOINT p;
	a.makeAbsolute();
	b.makeAbsolute();
	p.x=midpoint(a.x,b.x);
	p.y=midpoint(a.y,b.y);
	return p;
}
inline fPOINT centroid(vector<fPOINT> p){
	fPOINT sum(0,0);
	for(vector<fPOINT>::iterator i=p.begin(); i!=p.end(); i++){
		i->makeAbsolute();
		sum.x+=i->x;
		sum.y+=i->y;

	}
	size_t n=p.size();
	sum.x/=n;
	sum.y/=n;
	return sum;
}
inline void rotate(fPOINT& point, const fPOINT centeri, const float radians){
	//bool relative=point.isRelative;
	//fPOINT oldRelative;
	//if(relative) oldRelative=*(point.ptRelative);

	point.makeAbsolute();
	fPOINT center=centeri.getAbsolute();
	//point.x=center.x+cos(radians)*(point.x-center.x)-sin(radians)*(point.y-center.y);
	//point.y=center.y+sin(radians)*(point.x-center.x)+cos(radians)*(point.y-center.y);
	
//return;
	fPOINT opoint=point;
	point.x=cos(radians)*(opoint.x-center.x)-sin(radians)*(opoint.y-center.y)+center.x;
    point.y=sin(radians)*(opoint.x-center.x)+cos(radians)*(opoint.y-center.y)+center.y;
	


	//if(relative) point=point.getRelative(oldRelative);
}

inline float dist(float x1, float y1, float x2, float y2)
{
	return sqrt(pow(x2-x1,2.0f)+pow(y2-y1,2.0f));
}
inline float dist(fPOINT a, fPOINT b){ 
	a.makeAbsolute();
	b.makeAbsolute();
	return dist(a.x,a.y,b.x,b.y);
}
inline float _fmin(float a, float b){// the standard min() macro will cause expression reevaluation, the idea here is to avoid that and just compare values inline
	return ((a<b)?a:b);
}

inline float dist_AxisSegment_Point(fPOINT a1,fPOINT a2, fPOINT p, bool horz){///< determines the distance between a line segment and a point, when the segment is parallel to one of the 90-degree axes. - default is vertical line.
	int ix=0;
	int iy=1;
	if(horz){ ix=1; iy=0; }//same calculations, just with the axes swapped.
	float a1x=a1.getPositionByIndex(ix);
	float a1y=a1.getPositionByIndex(iy);
	float a2y=a2.getPositionByIndex(iy);
	float px=p.getPositionByIndex(ix);
	float py=p.getPositionByIndex(iy);

	//the line endpoints are define by their Y position along the vertical line - check if the center lies between the projected endpoints.
	if(RANGE2(py,a1y,a2y)){//the center IS within the endpoints.
		return abs(px-a1x);// use distance between the closest point (distance between vertical lines here)
	}else{
		//distance to nearest endpoint.
		return _fmin(dist(p, a1), dist(p, a2));
	}
}


inline bool Boundary::isCenterFixedType() const{
	return (type==BOUNDARY_RADIUS || type==BOUNDARY_POINT);
}
inline bool Boundary::isComposite() const{
	return (type==BOUNDARY_COMPOSITE_OR || type==BOUNDARY_COMPOSITE_AND);
}

inline void line_getparameters(fPOINT p1, fPOINT p2,   float& m, float& b, bool& vertical, bool& horizontal)
{
	vertical=false;
	horizontal=false;
	//line:  y=mx+b;   y1=mx1+b;  m=(y1-b)/x1=(y2-y1)/(x2-x1)   b=y1-mx1;
	p1.makeAbsolute();
	p2.makeAbsolute();
	//if(abs(p2.x-p1.x)<0.001) m = (p2.y-p1.y) / 0.0001;
	//else                     
	m = (p2.y-p1.y) / (p2.x-p1.x);
	b = (p1.y)-m*(p1.x);

	if(abs(p2.x-p1.x)<0.0001) vertical=true;
	if(abs(p2.y-p1.y)<0.0001) horizontal=true;

	if(BOUND_DEBUG) cout<<"LGP: "<<" m = ("<<p2.y<<"-"<<p1.y<<")/("<<p2.x<<"-"<<p1.x<<") = "<<m<<endl;
	if(BOUND_DEBUG) cout<<"LGP: "<<" b = "<<p1.y<<"-"<<m<<"*"<<p1.x<<" = "<<b<<endl;
}

inline float line_getx(float y, float m, float b){return (y-b)/m;}
inline float line_gety(float m, float x, float b){return m*x+b;}
inline bool pointinline(fPOINT p, float m, float b)
{
	p.makeAbsolute();
	float y_chk=line_gety(m, p.x, b);
	return abs(p.y - y_chk) < 0.001;//less than 1 thousandths difference.
}
inline bool lineinline(fPOINT p1a, fPOINT p2a, fPOINT p1b,fPOINT p2b,fPOINT& intersect)
{
	//y=ax+c  and y=bx+d http://en.wikipedia.org/wiki/Line-line_intersection#X_and_Y_values_of_intersection_on_a_linear_curve
			
	p1a.makeAbsolute();
	p2a.makeAbsolute();
	p1b.makeAbsolute();
	p2b.makeAbsolute();

	bool vert1,horz1,vert2,horz2;
	float m1,b1,m2,b2;
	line_getparameters(p1a,p2a, m1,b1, vert1,horz1);
	line_getparameters(p1b,p2b, m2,b2, vert2,horz2);
	if(BOUND_DEBUG) cout<<"y="<<m1<<"x+"<<b1<<endl;
	if(BOUND_DEBUG) cout<<"y="<<m2<<"x+"<<b2<<endl;

	if((vert1 && vert2) || (horz1 && horz2) || m1==m2 || abs(m1-m2)<0.001){//parallel lines
		if(b1==b2 || abs(b1-b2)<0.001) { if(BOUND_DEBUG) cout<<"LL: Line equivalence - intersection"<<endl; return true;}//same y-intercept, same line - collision is absolutely true.
		else                 { if(BOUND_DEBUG) cout<<"LL: Line parallel    - no intersection"<<endl; return false;}//different y-intercept, these parallel lines never intersect.
	}

	//vertical lines are defined by their X intercept
	if(vert1){//
		if(BOUND_DEBUG) cout<<"vert1"<<endl;
		float x=p1a.x;//line 1
		intersect=fPOINT( x, m2*x+b2);

	}else if(vert2){
		if(BOUND_DEBUG) cout<<"vert2"<<endl;
		float x=p1b.x;//line 2
		intersect=fPOINT( x, m1*x+b1);
	}else{
		if(BOUND_DEBUG) cout<<"normal"<<endl;
		intersect=fPOINT( (b2-b1)/(m1-m2), m1*(b2-b1)/(m1-m2)+b1);
	}


	
	

	bool inside_segments=( RANGE2(intersect.x,p1a.x,p2a.x) && RANGE2(intersect.y,p1a.y,p2a.y) &&
	         RANGE2(intersect.x,p1b.x,p2b.x) && RANGE2(intersect.y,p1b.y,p2b.y) );

	
	if(BOUND_DEBUG){ 
		intersect.debug();

		if(inside_segments) cout<<" does lie inside ";
		else cout<<" does NOT lie inside ";

		p1a.debug();
		p2a.debug();
		cout<<" ";
		p1b.debug();
		p2b.debug();
		cout<<endl;
	}

	

	return inside_segments;


}



void Boundary::invalidate()
{
	width=0;
	length=0;
	radius=0;
	type=BOUNDARY_NONE;
}
inline fPOINT Boundary::recalculateCenter(int depth){
	if(isCenterFixedType()) { center.makeAbsolute(); return center; }
	if(isComposite()){
		vector<fPOINT> centers;
		for(vector<Boundary>::iterator i=composites.begin(); i!=composites.end(); i++){
			if(depth!=0) i->recalculateCenter(depth-1);
			centers.push_back(i->center);
		}
		center=centroid(centers);
	}else center=centroid(points);
	return center;
}
void Boundary::move(float dx, float dy){
	modifications++;
	shift.x+=dx;
	shift.y+=dx;
	if(isComposite()) for(vector<Boundary>::iterator i=composites.begin(); i!=composites.end(); i++) i->move(dx,dy);
	for(vector<fPOINT>::iterator i=points.begin(); i!=points.end(); i++){
		i->makeAbsolute();
		i->x+=dx;
		i->y+=dy;
	}
	center.x+=dx;
	center.y+=dy;
	__modcheck();
}
inline void Boundary::setCenter(float x, float y){
	fPOINT d(x-center.x,y-center.y);
	move(d.x,d.y);
}
void Boundary::addComposite(Boundary b)
{
	composites.push_back(b);
	recalculateCenter();
}

inline COLLISTYPE Boundary::getCollisType(const Boundary& b){
	if(type==BOUNDARY_NONE || b.type==BOUNDARY_NONE) return COLLIS_NONE;
	bool bcA=isComposite();
	bool bcB=b.isComposite();
	if(bcA && bcB) return COLLIS_COMPOSITES;
	if(bcA || bcB) return COLLIS_COMPOSITESIMPLE;

	return (COLLISTYPE) (type|b.type);
}

#define gca_A(ppa,ppb,b) *ppa=this;*ppb=  &b;
#define gca_B(ppa,ppb,b) *ppa=  &b;*ppb=this;
inline COLLISTYPE Boundary::getCollisArgs(Boundary& b,Boundary** ppA,Boundary** ppB){
	COLLISTYPE ct=getCollisType(b);
	bool cond=true;
	switch(ct){
		case COLLIS_COMPOSITESIMPLE:
			cond=isComposite(); break;
		case COLLIS_POINTCIRCLE:
		case COLLIS_POINTLINE:
		case COLLIS_POINTPOLY:
			cond=(type==BOUNDARY_POINT); break;
		case COLLIS_CIRCLELINE:
		case COLLIS_CIRCLEPOLY:
			cond=(type==BOUNDARY_CIRCLE); break;
		case COLLIS_LINEPOLY:
			cond=(type==BOUNDARY_LINE); break;
	}
	if(cond){ gca_A(ppA,ppB,b); }
	else{ gca_B(ppA,ppB,b); }
	return ct;
}

bool Boundary::collide(Boundary& b){
	if(type==BOUNDARY_NONE || b.type==BOUNDARY_NONE) return false;
	Boundary* pA=NULL;
	Boundary* pB=NULL;
	COLLISTYPE ct=getCollisArgs(b,&pA,&pB);
	if(BOUND_DEBUG)
		if(ct==COLLIS_CIRCLEPOLY || ct==COLLIS_CIRCLELINE) cout<<dbgboundtype(type)<<" v "<<dbgboundtype(b.type)<<" = "<<dbgcollistype(ct)<<endl;

	switch(ct)
	{
	case COLLIS_NONE:
		return false;

	case COLLIS_COMPOSITESIMPLE:
	case COLLIS_COMPOSITES:
		for(vector<Boundary>::iterator i=pA->composites.begin(); i!=pA->composites.end(); i++)
			if( i->collide(b) == (pA->type==BOUNDARY_COMPOSITE_OR)) return (pA->type==BOUNDARY_COMPOSITE_OR);//shorcircuit TRUEs on COMPOSITE_OR; shortcircuit FALSEs on COMPOSITE_AND
		return (pA->type==BOUNDARY_COMPOSITE_AND);// checked all entries, for OR: no shortcircuit TRUEs - return FALSE (all entries failed);   for AND: no shortcircuit FALSEs - return TRUE (all entries pass)

	case COLLIS_POINTS:
		return ((int)center.x)==((int)b.center.x) && ((int)center.y)==((int)b.center.y);
	case COLLIS_POINTCIRCLE:
		return (pow(pB->center.x-pA->center.x,2.0f) + pow(pB->center.y-pA->center.y,2.0f)) <= pow(pB->radius,2.0f);//using point=circle of radius 0, hopefully.
	case COLLIS_POINTLINE:
		{
			bool vert,horz;
			float m,b;
			line_getparameters(pB->points[0], pB->points[1],  m,b, vert,horz);
			if(vert){
				float x=pB->points[0].x;//line
				return abs(x - pA->center.x)<0.0001;
			}
			return pointinline(pA->center, m,b);
		}

	case COLLIS_POINTPOLY:
		{
			//do raytracing from our point to the center of the polygon and beyond -  and count border-crossings (between polygon vertex)
			//2 crossings = outside
			//1 crossing = inside
			// OR... can we just go from  center to center for simple polygons ? (trying this)
			//1 crossing  = outside
			//0 crossings = inside
			//System_Alert("polygon");

			fPOINT ray[2]={pA->center, pB->center};
			//float raym,rayb;
			//line_getparameters(ray[0], ray[1],  raym,rayb);


			//int crossings;
			fPOINT intersect;
			float m,b;
			if(pB->points.size()<2) {System_Alert("invalid polygon"); return false;}
			vector<fPOINT>::iterator B=pB->points.begin(); B++;
			for(vector<fPOINT>::iterator A=pB->points.begin(); A!=pB->points.end();A++,B++)
			{
				if(B==pB->points.end()) B=pB->points.begin();//last point to the first point.
				//line_getparameters(*A, *B,  m,b);
				//printf("(%.0f,%.0f)(%.0f,%.0f)  v  (%.0f,%.0f)(%.0f,%.0f)\n",ray[0].x,ray[0].y,ray[1].x,ray[1].y,  A->x,A->y,B->x,B->y);
				//System_Alert("dbg");
				if(BOUND_DEBUG) cout<<"------------Ray v Side"<<endl;
				bool intersected=lineinline(ray[0],ray[1],*A,*B,intersect);

				if(BOUND_DEBUG){
					cout<<"Ray v. Side || ";
					ray[0].debug();
					ray[1].debug();
					cout<<" vs ";
					A->debug();
					B->debug();
				}
				if(intersected){ if(BOUND_DEBUG) cout<<" --- crossed! not in polygon!"<<endl; return false;}
				if(BOUND_DEBUG) cout<<endl;
				if(BOUND_DEBUG) cout<<"------------"<<endl;
				

			}
			
			return true;
			
		}
	case COLLIS_CIRCLES:
		return (pow(b.center.x-center.x,2.0f) + pow(b.center.y-center.y,2.0f)) <= pow(b.radius+radius,2.0f);
		//http://stackoverflow.com/questions/1736734/circle-circle-collision
	case COLLIS_CIRCLELINE:
		{
			// Problem!  http://i.imgur.com/Snr9g11.png  ... and solution: http://i.imgur.com/CtVJ5DT.png
			return false;


			//http://en.wikipedia.org/wiki/Distance_from_a_point_to_a_line#Proof_2_.28geometric_proof.29
			//determine the parameters of the line, then draw a parallel line, and determine the distance between those

			//project out the line:
			bool vert,horz;
			float m,b1,b2;
			line_getparameters(pB->points[0],pB->points[1],m,b2,vert,horz);//get variables of the Line



			//project out the endpoints from the segment to the line.  Find a perpendicular line to the one being test, then find the intersection of the new line and the one projected to the circle center.
			// *** check LineinLine to be sure it supports vert+horz intersections.
			// *** second version of lineinline that allows m,b syntax.
			// use the projection of one single endpoint to determine the shift for the entire line segment - it's only a translation, so why not.
			//   find the perpendicular line:  slope=-1/m   except for initial verticals where slope=0,  initial horizontals   slope=INF (needs special handling
			fPOINT perpline[2]={pB->points[0],pB->points[1]};
			float perpslope=-1.0f/m;
			fPOINT paraline[2]={pB->points[0],pB->points[1]};

			float d;//distance between the center of the circle and the closest point on the line segment.
			bool useEndPoints=false;

			if(vert||horz) d=dist_AxisSegment_Point(pB->points[0],pB->points[1],pA->center,horz);
			else{
				//draw a parallel line  where the circle center x,y fits the linear equation:
				//y=mx+b,  and solve for b,   b=y-mx
				b1=pA->center.y-m*(pA->center.x);

				//project the centerpoint onto the line - intercepts of a perpendicular between line and the center-projected line
				fPOINT projcenter( (-b1*m) / (m*m+1),  b1/(m*m+1) );
				//fPOINT projcenter( (-b2*m) / (m*m+1),  b2/(m*m+1) );//hopefully this is the point on the second line, not our given one.

				if(fRange(projcenter.x,pB->points[0].x,pB->points[1].x) && fRange(projcenter.y,pB->points[0].y,pB->points[1].y) ){
					d=abs(b2-b1)/sqrt(m*m+1);//reduction shown for http://en.wikipedia.org/wiki/Distance_between_two_straight_lines
				}else{
					d = _fmin(dist(pA->center, pB->points[0]), dist(pA->center, pB->points[1]));

				}

			}
			return d < (pA->radius);//check if the distance between the line and circle-center lie inside the radius of the circle
		}
	case COLLIS_CIRCLEPOLY:
		{
			//http://www.gamedev.net/topic/546941-circle-to-polygon-collision-detection-solved/
			//POINTPOLY: circle.center vs. polygon   +
			//CIRCLELINE: circle vs. polygon edge

			vector<fPOINT> points;
			points.push_back(pA->center);
			Boundary A_CenterPoint(BOUNDARY_POINT,points);
			if(A_CenterPoint.collide(b)){  return true; }//polygon contains circle center.

			if(pB->points.size()<2) {System_Alert("invalid polygon"); return false;}
			vector<fPOINT>::iterator B=pB->points.begin(); B++;
			for(vector<fPOINT>::iterator A=pB->points.begin(); A!=pB->points.end();A++,B++){
				if(B==pB->points.end()) B=pB->points.begin();//last point to the first point.
				points.clear();
				points.push_back(*A);
				points.push_back(*B);
				Boundary B_PolyEdge(BOUNDARY_LINE,points);
				if(BOUND_DEBUG) pA->debug();
				if(BOUND_DEBUG) B_PolyEdge.debug();
				if(pA->collide(B_PolyEdge)){
					return true;
				}
			}
			return false;
		}
	case COLLIS_LINES:
		{
			fPOINT intersect;
			return lineinline(pA->points[0],pA->points[1],pB->points[0],pB->points[1],intersect);
		}
	case COLLIS_LINEPOLY:
		{

			fPOINT intersect;

			if(pB->points.size()<2) {System_Alert("invalid polygon"); return false;}
			vector<fPOINT>::iterator B=pB->points.begin(); B++;
			for(vector<fPOINT>::iterator A=pB->points.begin(); A!=pB->points.end();A++,B++){
				if(B==pB->points.end()) B=pB->points.begin();//last point to the first point.
				if(lineinline(pA->points[0],pA->points[1],*A,*B,intersect)) return true;//if our line intersects any polygon edges
			}

			vector<fPOINT> points;
			points.push_back(pA->points[0]);
			Boundary A_Point(BOUNDARY_POINT, points);
			if(A_Point.collide(b)) return true;//our line has a point on or inside of the polygon but does not intersect polygon edges - it must be entirely inside the polygon!



			return false;
		}
	case COLLIS_POLYS:
		{
			vector<fPOINT> points;
			if(pA->points.size()<2) {System_Alert("invalid polygon"); return false;}
			for(vector<fPOINT>::iterator A=pA->points.begin(); A!=pA->points.end();A++){
				points.clear();
				points.push_back(*A);
				Boundary A_Point(BOUNDARY_POINT,points);
				if(A_Point.collide(b)){
					if(BOUND_DEBUG){
						A_Point.debug();
						b.debug();
						cout<<endl;
					}
					return true;
				}
			}
			return false;
		}
	}
	return false;
}


void Boundary::debug(){
	cout<<dbgboundtype(type)<<" radius="<<radius<<" center=";
	center.debug();
	cout<<" "<<points.size()<<" points:";
	for(vector<fPOINT>::iterator i=points.begin(); i!=points.end(); i++){
		i->debug();
	}
	cout<<endl;
}


void Boundary::draw(Drawing_Info* di, bool drawcenter)
{
	if(isComposite()) for(vector<Boundary>::iterator i=composites.begin(); i!=composites.end(); i++) i->draw(di,false);
	if(type==BOUNDARY_RADIUS) Drawing_DrawCircle(di,center,radius,0xFF00FF00,true);
	else Drawing_DrawPoly(di,points,0xFF00FF00,true);

	
	if(drawcenter || type==BOUNDARY_POINT){
		int color=0xFF00FF00;
		if(type==BOUNDARY_POINT) color=0xFFFF0000;
		Drawing_DrawLine(di,center.x-5,center.y-5,center.x+5,center.y+5,color,true);
		Drawing_DrawLine(di,center.x+5,center.y-5,center.x-5,center.y+5,color,true);
	}
}
void Boundary::rotate(float radians,fPOINT* pVertex)
{
	for(;radians>=(2*PI);) radians-=(2*PI);
	for(;radians<=(-2*PI);) radians+=(2*PI);
	rotation+=radians;
	//pVertex->makeAbsolute();

	bool selfcentered=(pVertex==NULL);
	if(selfcentered) pVertex=&center;

	if(isComposite()){
		for(vector<Boundary>::iterator i=composites.begin();i!=composites.end();i++)
			i->rotate(radians,pVertex);
	}else
		for(vector<fPOINT>::iterator i=points.begin();i!=points.end();i++)
			::rotate(*i,*pVertex,radians);//global rotate()

	if(!selfcentered) ::rotate(center,*pVertex,radians);
	__modcheck();
}
Boundary::Boundary(){
	__modchecklock=false;
	rotation=0.0f;
	modifications=0;
	invalidate();
}
Boundary::Boundary(BOUNDARYTYPE t, vector<fPOINT> p)
{
	__modchecklock=false;
	rotation=0.0f;
	modifications=0;
	type=t;
	setPoints(p);
	for(vector<fPOINT>::iterator i=points.begin();i!=points.end();i++) i->makeAbsolute();
	recalculateCenter();
	switch(t){
		case BOUNDARY_NONE:
			invalidate();
			break;
		case BOUNDARY_LINE:
			if(p.size()>=2){
				center=midpoint(p[0],p[1]);
				length=dist(p[0],p[1]);
				width=1;
				radius=0;
			}else invalidate();
		case BOUNDARY_RADIUS:
			if(p.size()>=2){
				center=p[0];
				radius=dist(p[0],p[1]);
				length=radius*2.0f;
				width=length;
			}else invalidate();
			break;
		case BOUNDARY_BOX:
			//length,width not supported at this time for 4-point quads. center already calculated.
			break;
		case BOUNDARY_POINT:
			center=p[0];
			length=1;
			width=1;
			radius=1;
			break;
		case BOUNDARY_COMPOSITE_OR:
		case BOUNDARY_COMPOSITE_AND:
			invalidate();//composites are defined by boundaries, not points.
			return;
	}
}
void Boundary::setComposites(vector<Boundary> bnds){
	composites=bnds;
	unmodified_composites=bnds;
	modifications=0;
	unmodified_center=center;
	rotation=0.0f;
	shift=fPOINT(0,0);
}
void Boundary::setPoints(vector<fPOINT> pts){
	points=pts;
	unmodified_points=pts;
	unmodified_center=center;
	modifications=0;
	rotation=0.0f;
	shift=fPOINT(0,0);
}
void Boundary::__resync(){
	return;
	fPOINT newcenter=center;
	points=unmodified_points;
	composites=unmodified_composites;
	//center=unmodified_center;
	recalculateCenter();
	setCenter(newcenter.x,newcenter.y);
	//move(shift.x,shift.y);
	rotate(rotation);
	modifications=1;
}
void Boundary::__modcheck(){
	if(__modchecklock) return;//already in a modcheck and resync. do not recurse.
	__modchecklock=true;
	modifications++;
	if(modifications>100) __resync();
	__modchecklock=false;
}
Boundary::Boundary(BOUNDARYTYPE t, fPOINT p, float fRadius)
{
	__modchecklock=false;
	rotation=0.0f;
	modifications=0;
	if(t!=BOUNDARY_RADIUS) { invalidate(); return; }
	type=t;
	p.makeAbsolute();
	points.push_back(p);
	center=p;
	setPoints(points);
	radius=fRadius;
	width=fRadius*2.0f;
	length=width;
}
Boundary::Boundary(BOUNDARYTYPE t, vector<Boundary> b)
{
	__modchecklock=false;
	rotation=0.0f;
	modifications=0;
	type=t;
	setComposites(b);
	//composites=b;
	recalculateCenter();
}

//---------------------------------------------------------------------------------------------------





POINT Dim(const RECT& r)
{
	POINT dim;
	dim.x=(r.right -r.left);//+1;
	dim.y=(r.bottom-r.top );//+1;
	return dim;
}
fPOINT Dim(const fRECT& fr)
{
	fPOINT dim;
	dim.x=(fr.right -fr.left);//+1.0f;
	dim.y=(fr.bottom-fr.top );//+1.0f;
	return dim;
}
fPOINT Drawing_GetCenter(const RECT& r)
{
	//POINT dim=Dim(r);
	fPOINT pt;
	pt.x=midpoint(r.left,r.right);
	pt.y=midpoint(r.top,r.bottom);
	return pt;
}
fPOINT Drawing_GetCenter(const fRECT& r)
{
	//fPOINT dim=Dim(r);
	if(r.isRelative){
		return fPOINT(midpoint(r.left,r.right), midpoint(r.top,r.bottom),r.ptRelative);
	}else{
		return fPOINT(midpoint(r.left,r.right), midpoint(r.top,r.bottom));
	}
}

void Drawing_CenterOn(fRECT& fr, const fPOINT& pt)
{
	//fPOINT pt=Drawing_GetCenter(fr);
	fr=fr.getAbsolute();
	fPOINT pt2=pt.getAbsolute();
	fPOINT dim=Dim(fr);
	fr.left=  (pt2.x - (dim.x/2)) ;//+ 0.5f;
	fr.right= (pt2.x + (dim.x/2)) ;//- 0.5f;
	fr.top=   (pt2.y - (dim.y/2)) ;//+ 0.5f;
	fr.bottom=(pt2.y + (dim.y/2)) ;//- 0.5f;
}


fRECT ShiftFRECT(fRECT fr, float dx, float dy)
{
	fr.left-=dx;
	fr.right-=dx;
	fr.top-=dy;
	fr.bottom-=dy;
	return fr;
}

fRECT Drawing_GetFRECT(RECT r)
{
	fRECT fr;
	fr.left=(float) r.left;
	fr.top=(float) r.top;
	fr.bottom=(float) r.bottom;
	fr.right=(float) r.right;
	return fr;
}
RECT Drawing_ApproxRECT(fRECT fri)
{
	fRECT fr=fri.getAbsolute();
	RECT r;
	r.left=(int) round(fr.left);
	r.top=(int) round(fr.top);
	r.bottom=(int) round(fr.bottom);
	r.right=(int) round(fr.right);
	return r;
}

//---------------------------------------------------------------------------------------------------
Drawing_Info::~Drawing_Info(){;}
void Image_Info::RotateTo(float r){
	float dr=r-rotation;
	Rotate(dr);
}
void Image_Info::Debug(){
}
void Image_Info::Rotate(float r){
	rotation+=r;
	if(requires_boundaries) rectangle.rotate(r);
}
void Image_Info::CornerMove(float dx, float dy){
	DstRect.left+=dx;
	DstRect.right+=dx;
	DstRect.top+=dy;
	DstRect.bottom+=dy;
	center.isRelative=true;
	center.xrel=DstRect.left;
	center.yrel=DstRect.top;
	if(requires_boundaries) SyncBoundaries();
}
void Image_Info::CornerMoveTo(float x, float y){
	float dx=x-DstRect.left;
	float dy=y-DstRect.top;
	CornerMove(dx,dy);
}
void Image_Info::Move(float dx, float dy){
	CornerMove(dx,dy);
}
void Image_Info::MoveTo(float x, float y){
	fPOINT cabs=center.getAbsolute();
	float dx=x-cabs.x;
	float dy=y-cabs.y;
	CornerMove(dx,dy);
}
void Image_Info::RecalculateRectangle(){
	if(!requires_boundaries) return;

	fRECT fr=DstRect;

	if(!center.isRelative){
	fPOINT rel=fPOINT(fr.left,fr.top);
	center=fPOINT(center.x,center.y,rel);
	}


	rectangle.points.clear();
	vector<fPOINT> points;
	rectangle.type=BOUNDARY_POLY;
	points.push_back(fPOINT(fr.left,fr.top));
	points.push_back(fPOINT(fr.right,fr.top));
	points.push_back(fPOINT(fr.right,fr.bottom));
	points.push_back(fPOINT(fr.left,fr.bottom));
	rectangle.points=points;
	rectangle.center=center.getAbsolute();
	rectangle.radius=radius;
	//cout<<fr.left<<" "<<fr.top<<endl;
}
void Image_Info::RecalculateCircle()
{
	fRECT fr=DstRect;

	center.isRelative=true;
	center.xrel=fr.left;
	center.yrel=fr.top;
	center.x=(fr.right-fr.left)/2.0f;
	center.y=(fr.bottom-fr.top)/2.0f;
	

	//center.x=midpoint(fr.left,fr.right );//((ii->DstRect.right)-(ii->DstRect.left))/2;//maybe +1 (as above)
    //center.y=midpoint(fr.top ,fr.bottom);//((ii->DstRect.bottom)-(ii->DstRect.top))/2
    radius=(center.x + center.y)/2.0f;

	if(!requires_boundaries) return;
	
	fPOINT cabs=center.getAbsolute();

	circle=Boundary(BOUNDARY_CIRCLE,cabs,radius*2.0f);
	//rectangle.setCenter(cabs.x,cabs.y);
	//rectangle.radius=radius;
}
void Image_Info::Unload(){
	rectangle.invalidate();
	circle.invalidate();
}
void Image_Info::Reload(Drawing_Info* di){;}
Image_Info::~Image_Info(){;}
Image_Info::Image_Info(){
	requires_boundaries=false;
	circle.invalidate();
	rectangle.invalidate();
	rotation=0.0f;
	image="";
}

void Image_Info::SyncBoundaries(){
	if(!requires_boundaries) return;
	rectangle.setCenter(DstRect.left+center.x,DstRect.top+center.y);
	circle.setCenter(DstRect.left+center.x,DstRect.top+center.y);
}

void Drawing_CenterViewOn(Drawing_Info* di, const fPOINT& pt)
{
	if(di==NULL || !(di->centering) ) return;
	di->viewport=pt;
	di->viewport.x-=(float)di->width/2.0f;
	di->viewport.y-=(float)di->height/2.0f;
}
void Drawing_CenterViewOn(Drawing_Info* di, Image_Info* ii)
{
	if(di==NULL || ii==NULL || !(di->centering) || !(ii->viewcentered)) return;
	fPOINT pt(ii->DstRect.left + ii->center.x , ii->DstRect.top + ii->center.y);
	Drawing_CenterViewOn(di, pt);
}
void Drawing_CenterViewAdjust(Drawing_Info* di, float dx,float dy)
{
	if(di==NULL || !(di->centering) ) return;
	di->viewport.x+=dx;
	di->viewport.y+=dy;
}

void Drawing_DrawLines(Drawing_Info* di, vector<fPOINT> points,int color,bool relative)
{
	if(points.size()<2) return;
	vector<fPOINT>::iterator B=points.begin(); B++;
	for(vector<fPOINT>::iterator A=points.begin(); (A!=points.end() && B!=points.end());A++,B++)
		Drawing_DrawLine(di,A->x,A->y,B->x,B->y,color,relative);
}

void Drawing_DrawPoly(Drawing_Info* di, vector<fPOINT> points,int color,bool relative)
{
	if(points.size()<2) return;
	vector<fPOINT>::iterator B=points.begin(); B++;
	for(vector<fPOINT>::iterator A=points.begin(); A!=points.end();A++,B++){
		if(B==points.end()) B=points.begin();//draw a line from the last point to the first
		Drawing_DrawLine(di,A->x,A->y,B->x,B->y,color,relative);
	}
}