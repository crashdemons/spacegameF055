#include "ObjectContainer.h"
#include "objects_All.h"//yeah I know about post-including, this looks bad.


bool ObjectFilter::getMatch(int type)
{
    if(a) return true;
    if(n) return false;
	if(!RANGE(type,0,OBJECT_TYPES)) return false;
    return omatch[type];
}
void ObjectFilter::setMatch(int type,bool match,bool all,bool none){
    a=all;n=none;
	if(RANGE(type,0,OBJECT_TYPES)) omatch[type]=match;
}
void ObjectFilter::setMatchOnly(int type,bool match)
{
    a=false;n=false;
    for(int i=0;i<OBJECT_TYPES;i++)
            if(i==type){omatch[i]=match;}
            else{omatch[i]=!match;}
}
ObjectFilter::ObjectFilter(){ for(int i=0;i<OBJECT_TYPES;i++) omatch[i]=true; }

//----------------------------------------------------------------------------------------

ObjectContainer::ObjectContainer()
{
	vector<int> grad;
	grad.push_back(0x7FFF0000);//red
	grad.push_back(0x7FFFFF00);//yellow
	grad.push_back(0x7F00FF00);//green
	grad.push_back(0x7F7F7FFF);//blue
	grad.push_back(0x7FFFFFFF);//white
	grShields.fill(grad);



	depthmin=255;
	depthmax=0;

	selectedObject=-1;
	count=0;
	memset(objects,0,sizeof(objects));
	memset(depth,0,sizeof(depth));
	/*
	CRITICAL: ERROR LOADING STRUCTS.
    loadFromFile("./Dat/Plasma.projectile",&Projectile_Plasma,sizeof(Projectile_Type));
    loadFromFile("./Dat/PlasmaBomb.projectile",&Projectile_PlasmaBomb,sizeof(Projectile_Type));
    loadFromFile("./Dat/Mine.projectile",&Projectile_Mine,sizeof(Projectile_Type));
    loadFromFile("./Dat/MiningLaser.projectile",&Projectile_MiningLaser,sizeof(Projectile_Type));
    loadFromFile("./Dat/Asteroid.projectile",&Projectile_Asteroid,sizeof(Projectile_Type));
    loadFromFile("./Dat/TractorBeam.projectile",&Projectile_TractorBeam,sizeof(Projectile_Type));
    if(Projectile_Asteroid.deathspawn!=NULL) Projectile_Asteroid.deathspawn=&Projectile_Asteroid;
    if(Projectile_PlasmaBomb.deathspawn!=NULL) Projectile_PlasmaBomb.deathspawn=&Projectile_Plasma;
	*/
	Projectile_Type Projectile_PlasmaC = {50,false,"./Img/plasma.bmp",0xFFFFFF,0,true,0.3f,1,5, false,true,true,true,true,false,true,-1,-1,0,0};
    Projectile_Type Projectile_PlasmaBombC = {50,false,"./Img/plasmab.bmp",0xFFFFFF,10*DEGRAD,true,0.5,1,5, false,true,true,true,true,true,true,-1,-1,&Projectile_Plasma,30,-50,-5};
   
	Projectile_Type Projectile_MissileC = {300,true,"./Img/missile.bmp",0xFFFFFF,0,true,0.5,1,50, false,true,true,true,true,true,true,-1,-1,0,0};
   
	
	Projectile_Type Projectile_MineC = {-1,false,"./Img/mine.bmp",0xFFFFFF,-0.005f,true,0.3f,1,80, false,true,false,true,true,false,true,-1,-1,0,0};
    Projectile_Type Projectile_MiningLaserC = {0,false,"./Img/mining_laser.bmp",0xFFFFFF,0,false,0.0f,-1,2,false,false,true,false,true,false,false,0x7F00FF00,-1,0,0};
    Projectile_Type Projectile_AsteroidC = {-1,false,"./Img/as1.bmp",0xFFFFFF,DEGRAD,true,0.5f,35,10,false,false,true,true,true,false,false,-1,-1,0,0,10,-3};
    Projectile_Type Projectile_ScrapsC   = {-1,false,"./Img/sc1.bmp",0xFFFFFF,DEGRAD,true,0.5f,2,10,false,false,true,true,true,false,true,-1,-1,0,0,10,-3};
    Projectile_Type Projectile_TractorBeamC = {0,false,"./Img/tractor_beam.bmp",0xFF000000,0,false,-0.1f,-1,0,false,false,true,false,false,false,false,-1,-1,0,0,0,0};       
    
	
	Projectile_Plasma=Projectile_PlasmaC;
    Projectile_PlasmaBomb=Projectile_PlasmaBombC;
    Projectile_Missile=Projectile_MissileC;
    Projectile_Mine=Projectile_MineC;
    Projectile_MiningLaser=Projectile_MiningLaserC;
    Projectile_Asteroid=Projectile_AsteroidC;
    Projectile_Scraps=Projectile_ScrapsC;
    Projectile_TractorBeam=Projectile_TractorBeamC;

}
ObjectContainer::~ObjectContainer()
{
    
}

int ObjectContainer::selectFromCursor(void* pCursor){
	int mintype=OT_PHYS;
	SceneryObject* so=(SceneryObject*) pCursor;
	selectedObject=-1;

	for(int i=0;i<OBJECTS_MAX;i++) if(types[i]>=mintype){
		if(objects[i]!=NULL){
			SceneryObject* po=(SceneryObject*) objects[i];
			if(!po->isInScreen(di)) continue;


			fRECT fra=ShiftFRECT(so->ii->DstRect, -di->viewport.x, -di->viewport.y);
			RECT ra=Drawing_ApproxRECT(fra);
			RECT rb=Drawing_ApproxRECT(po->ii->DstRect);
			RECT rc;
			if(IntersectRect(&rc,&rb,&ra)==TRUE){ selectedObject=i; break; }
		}
	}
	return selectedObject;
}
string ObjectContainer::getSelectedInfo(){
	int i=selectedObject;
	if(selectedObject==-1 || objects[i]==NULL) return string("");
	string out=object_typenames[types[i]];
	if(types[i]>=OT_PHYS){
		PhysicalObject* po=(PhysicalObject*) objects[i];
		out+=" H:"+itos(po->stats.health.current)+"/"+itos(po->stats.health.maximum);
		out+=" S:"+itos(po->stats.shields.current)+"/"+itos(po->stats.shields.maximum);
	}else{

	}
	return out;
}


int ObjectContainer::findempty(int i)
{
	if(count>=512) return -1;
    if(objects[i]==NULL) return i;
    for(i=OBJECTS_LAST;i>=0;i--) if(objects[i]==NULL) return i;
    return -1;
}

inline void setContainerInfo(void* so, ObjectContainer* container, int i)
{
	((SceneryObject*)so)->container=container;
	((SceneryObject*)so)->object_index=i;
	if(container->types[i]>=OT_PHYS) ((PhysicalObject*) so)->ai->attach(container,i);
}
//inline
void clearContainerInfo(void* so)
{
	if(so==NULL) return;
	((SceneryObject*)so)->container=NULL;
	((SceneryObject*)so)->object_index=-1;
}


int ObjectContainer::add(void* so, unsigned char depthv, OBJECT_FLAG flagsv){
	//unsigned char depthv=OD_DEFAULT;
	//OBJECT_FLAG flagsv=OF_NONE;
    int i=findempty(next);
    if(i<0){
        release(so);//prevents items not queued from being leaked.
        next=-1;
        return -1;
    }else{
		count++;
        objects[i]=so;
        types[i]=((SceneryObject*) so)->type;
		setDepth(i,depthv);
		flags[i]=flagsv;
		setContainerInfo(so,this,i);
        ((SceneryObject*)so)->container=this;
		((SceneryObject*)so)->object_index=i;
		

        if(i==OBJECTS_MAX) next=-1;
        else next=i+1;
        return i;
    }
}
void ObjectContainer::setDepth(int i, unsigned char depthv){
	if(i!=-1){
		if(depthv<depthmin) depthmin=depthv;
		if(depthv>depthmax && depthv!=OD_NONE) depthmax=depthv;
		depth[i]=depthv;
	}
}
void ObjectContainer::setFlag(int i, OBJECT_FLAG of,bool enable){
	if(enable) flags[i]=(OBJECT_FLAG)(flags[i] |   of); // Flags OR of yields all the 1's present in both.
	else       flags[i]=(OBJECT_FLAG)(flags[i] & (~of));// creates a bitmask of all bits of except ones present in OF, then yields those of which are present in Flags
}
bool ObjectContainer::checkFlag(int i, OBJECT_FLAG of){
	if(flags[i]==OF_NONE) return of==OF_NONE;
	return (flags[i]&of)==of;//all of the flags specified are present.
}

void ObjectContainer::remove(int i,bool free)
{
	if(i==-1) return;
	if(i==selectedObject) selectedObject=-1;
	
	//cout<<"REMV "<<objects[i]<<" "<<i<<endl;
	if(checkFlag(i,OF_NODELETE)) free=false;

	if(objects[i]!=NULL) count--;
	//if(objects[i]!=NULL) clearContainerInfo(objects[i]);//prevents referencing itself in the container if it was removed.
    if(free) release(i);
    objects[i]=NULL;//we need to free this
    types[i]=OT_NONE;
    next=i;
}
void ObjectContainer::clear(bool free)
{
    for(int i=0;i<OBJECTS_MAX;i++) remove(i,free);
    next=0;
}
/*int ObjectContainer::count()//deprecated - an active counter variable is more efficient.
{
    int c=0;
    for(int i=0;i<OBJECTS_MAX;i++) if(objects[i]!=NULL) c++;
    return c;
}*/
int ObjectContainer::countScreen()
{
    int c=0;
    for(int i=0;i<OBJECTS_MAX;i++)
		if(objects[i]!=NULL)
			if( ((SceneryObject*)objects[i])->isInScreen(di) ) c++;
    return c;
}
void ObjectContainer::step()
{
	int idxCenter=-1;
	SceneryObject* soCenter=NULL;
	for(int i=0;i<OBJECTS_MAX;i++){
		step(i);

		if(idxCenter==-1 && objects[i]!=NULL)//hunt for the first view-centering object.
		{
			soCenter=(SceneryObject*) objects[i];
			if(soCenter->centered) idxCenter=i;
		}
	}

	if(idxCenter!=-1) Drawing_CenterViewOn(di,soCenter->ii);//apply the view-center
}
void ObjectContainer::stepType(int type)
{
	int idxCenter=-1;
	SceneryObject* soCenter=NULL;
	for(int i=0;i<OBJECTS_MAX;i++){
		if(types[i]==type) step(i);

		if(idxCenter==-1 && objects[i]!=NULL)//hunt for the first view-centering object.
		{
			soCenter=(SceneryObject*) objects[i];
			if(soCenter->centered) idxCenter=i;
		}
	}

	if(idxCenter!=-1) Drawing_CenterViewOn(di,soCenter->ii);//apply the view-center



	//for(int i=0;i<OBJECTS_MAX;i++) if(types[i]==type) step(i);
}
void ObjectContainer::release(int i)
{
	if(i==-1) return;
    if(objects[i]==NULL) return;
	//OutputDebugString( (string("OC_Freeing: ")+ ptos((void*)objects[i])+"\n").c_str());
    release(objects[i],types[i]);
}


void ObjectContainer::unloadimages(Drawing_Info* di)
{
    for(int i=0;i<OBJECTS_MAX;i++)
        if(objects[i]!=NULL){
            SceneryObject* so=(SceneryObject*) objects[i];
			so->UnloadImg();
            //if(so->ii!=NULL) so->ii->Unload();
        }
}
void ObjectContainer::reloadimages(Drawing_Info* di)
{
    for(int i=0;i<OBJECTS_MAX;i++)
        if(objects[i]!=NULL){
            SceneryObject* so=(SceneryObject*) objects[i];
            if(so->ii!=NULL) so->ii->Reload(di);
        }
}

void ObjectContainer::draw(Drawing_Info* di)
{	/*
	for(int i=0;i<OBJECTS_MAX;i++)
		if( objects[i]!=NULL) {
			cout<<(unsigned int)depth[i]<<endl;
			((SceneryObject*) objects[i])->Draw(di);
		}
		*/
	
	
	for(unsigned char d=OD_BOTTOM;d>=OD_TOP && d!=OD_NONE;d--){//draw all objects for a particular depth
		//cout<<(unsigned int)d<<endl;
		for(int i=0;i<OBJECTS_MAX;i++){
			if(objects[i]!=NULL && depth[i]==d){
				((SceneryObject*) objects[i])->Draw(di);
			} 
		}
	}
 
}
void ObjectContainer::draw_debug(Drawing_Info* di,int debug_mode)
{
	for(int i=0;i<OBJECTS_MAX;i++)
    {
        if(objects[i]==NULL) continue;
		if(((SceneryObject*) objects[i])->isInScreen(di)){
			if(debug_mode>2) debug_mode%=3;
			if(debug_mode==0 || debug_mode==2) ((SceneryObject*) objects[i])->DrawRect(di);
			if(debug_mode==1 || debug_mode==2) ((SceneryObject*) objects[i])->DrawCirc(di);
		}
    }
	//for(int i=0;i<256;i++) Drawing_DrawLine(di,100+i,100,100+i,200,grShields.colors[i]);
}

void ObjectContainer::step(int i)
{
	if(i==-1) return;
    if(objects[i]==NULL) return;
    SceneryObject* so=(SceneryObject*) objects[i];

	 if(so->visible)
	 {
		so->doStep(i);
	 }else{
		remove(i,true);//WHOA WHOA WHOA! when did I decide that we can't queue invisible objects!? NOTE: world-exiting items are "killed" this way.
	 }
}
void ObjectContainer::release(void* p,int type)
{
	if(p==NULL) return; 
    SceneryObject* so=(SceneryObject*) p;
    delete so;//all overriden virtual destructors will get called as a fall-through.(BC++ GP p306)
    
}
int ObjectContainer::getFactionView(int i, FACTION_TYPE fact)
{
	if(i==-1) return NEUTRAL;
    if(objects[i]==NULL) return NEUTRAL;
    if(types[i]==OT_PROJ){
        Projectile* pj=(Projectile*) objects[i];
        if((pj->object_owner)>=-1) return NEUTRAL;//this function does NOT calculate owner views
        else if((pj->object_owner)<-1) return pj->getUnownedView();
    }
	if(types[i]==OT_NSHP || types[i]==OT_NSHP || types[i]==OT_SHIP){
		int& faction=((Ship*) objects[i])->reputation.f;
		if(fact==faction && faction!=FNONE) return ALLY;
		return ((Ship*) objects[i])->reputation.r[fact];
	}
	return NEUTRAL;
}
int ObjectContainer::getOwner(int i)
{
	if(i==-1) return -1;
    int oowner=i;
    if(types[i]==OT_PROJ){
        if(objects[i]!=NULL){
            int tmpowner=((Projectile*) objects[i])->object_owner;
            if(tmpowner>=0) oowner=getOwner(tmpowner);
        }
    }
    return oowner;
}
int ObjectContainer::getView(int i, int j)
{
	if(i==-1 || j==-1) return NEUTRAL;
    if(objects[i]==NULL || objects[j]==NULL) return NEUTRAL;
    if(i==j) return ALLY;
    if(types[i]==OT_PROJ){
        Projectile* pj=(Projectile*) objects[i];
        if((pj->object_owner)>=-1) return NEUTRAL;//this function does NOT calculate owner views
        else if((pj->object_owner)<-1) return pj->getUnownedView();
    }
    Reputation* rp1=NULL;
    Reputation* rp2=NULL;
    if(types[i]==OT_NSHP && types[j]==OT_NSHP){
        NPCShip* ns1=(NPCShip*)objects[i];
        NPCShip* ns2=(NPCShip*)objects[j];
        rp1=&ns1->reputation;
        rp2=&ns2->reputation;

        /*
        int viewtest=rp1->r[FFED];
        char buffer[20];
        itoa(viewtest,buffer,10);
        MessageBox(NULL,buffer,"npc rep1",MB_OK);
        
        viewtest=rp2->r[FFED];
        itoa(viewtest,buffer,10);
        MessageBox(NULL,buffer,"npc rep2",MB_OK);
        */
        
        
    }else if(types[i]==OT_PSHP && types[j]==OT_NSHP){
        PlayerShip* ps=(PlayerShip*)objects[i];
        NPCShip* ns=(NPCShip*)objects[j];
        rp1=&ps->reputation;
        rp2=&ns->reputation;
    }else if(types[i]==OT_NSHP && types[j]==OT_PSHP){
        PlayerShip* ps=(PlayerShip*)objects[j];
        NPCShip* ns=(NPCShip*)objects[i];
        rp1=&ps->reputation;
        rp2=&ns->reputation;
    }
    if(rp1!=NULL && rp2!=NULL) return rp1->AsViewedBy(rp2);
    return NEUTRAL;
}
int ObjectContainer::getOwnerView(int i, int j)
{
	if(i==-1 || j==-1) return NEUTRAL;
    if(objects[i]==NULL || objects[j]==NULL) return NEUTRAL;
    return getView(getOwner(i),getOwner(j));
}

vector<int> ObjectContainer::getFiltered(ObjectFilter* of,ReputationFilter* rf,int max_results)
{
	vector<int> entries;
	int num=0;
    for(int i=0;i<OBJECTS_MAX;i++)
    {
        if(objects[i]==NULL) continue;
        int type=types[i];
        int view=getFactionView(i,FNONE);
        
        bool bof,brf;
        bof=of->getMatch(type);
        brf=rf->getMatch(view);
        if(bof && brf){
			entries.push_back(i);
			num++;
			if(max_results>=0 && num>=max_results) break;
        }
    }
    return entries;
}
vector<int> ObjectContainer::getRelativeFiltered(int j,ObjectFilter* of,ReputationFilter* rf,Range* rn,bool excludej,int max_results)
{
	vector<int> entries;
	int num=0;
	if(j==-1) return entries;
    SceneryObject* jso=(SceneryObject*) objects[j];
    for(int i=0;i<OBJECTS_MAX;i++)
    {
        if(objects[i]==NULL) continue;
        if(excludej && i==j) continue;
        int type=types[i];
        int view=getOwnerView(i,j);
        float dist=jso->distTo((SceneryObject*)objects[i]);
        
        bool bof,brf,brn;
        bof=of->getMatch(type);
        brf=rf->getMatch(view);
        brn=rn->isInside(dist);
        if(bof && brf && brn){
			entries.push_back(i);
			num++;
			if(max_results>=0 && num>=max_results) break;
        }
    }
    return entries;
}

vector<pair<int,float>> ObjectContainer::getRelativeFilteredEx(int j,ObjectFilter* of,ReputationFilter* rf,Range* rn,bool excludej,int max_results)
{
	vector<pair<int,float>> entries;
	int num=0;
	if(j==-1) return entries;
    SceneryObject* jso=(SceneryObject*) objects[j];
    for(int i=0;i<OBJECTS_MAX;i++)
    {
        if(objects[i]==NULL) continue;
        if(excludej && i==j) continue;
        int type=types[i];
        int view=getOwnerView(i,j);
        float dist=jso->distTo((SceneryObject*)objects[i]);
        
        bool bof,brf,brn;
        bof=of->getMatch(type);
        brf=rf->getMatch(view);
        brn=rn->isInside(dist);
        if(bof && brf && brn){
			entries.push_back(make_pair(i,dist));
			num++;
			if(max_results>=0 && num>=max_results) break;
        }
    }
    return entries;
}
int ObjectContainer::getClosestFiltered(int j,ObjectFilter* of,ReputationFilter* rf,Range* rn,bool excludej)
{
	if(j==-1) return -1;
    int iClosest=-1;
    float lClosest=-1;
    SceneryObject* jso=(SceneryObject*) objects[j];
    for(int i=0;i<OBJECTS_MAX;i++)
    {
        if(objects[i]==NULL) continue;
        if(excludej && i==j) continue;
        int type=types[i];
		if(type>=OBJECT_TYPES) System_Alert(itos(depth[i]),"TYPE ERROR");
        int view=getOwnerView(i,j);
        float dist=jso->distTo((SceneryObject*)objects[i]);
        
        bool bof,brf,brn;
        bof=of->getMatch(type);
        brf=rf->getMatch(view);
        brn=rn->isInside(dist);
        if(bof && brf && brn){
         if(dist<lClosest || lClosest==-1){iClosest=i; lClosest=dist;}
        }
    }
    return iClosest;
}


