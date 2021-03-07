#include <windows.h>
#define FACTIONS 9
enum FACTION_TYPE
{
    FACTION_NONE,
    FACTION_FED,//engages enemies & criminals
    FACTION_REBEL,//engages enemies & criminals
    FACTION_ALIEN,
    FACTION_PIRATE,
    FACTION_FEDTRADE,//engages enemies only (which have to be attackers)
    FACTION_REBTRADE,//engages enemies only (which have to be attackers)
    FACTION_FREETRADE,//engages enemies only (which have to be attackers)
    FACTION_BHUNTER,
};

enum FACTION_VALUES
{
    ENEMY,
    CRIMINAL,
    NOT_FRIENDLY,
    NEUTRAL,
    FRIENDLY,
    EMPLOYEE,
    ALLY,
    
    
};

class Reputation
{
    public:
        int r[FACTIONS];//what you are
        int v[FACTIONS];//what you think
        Reputation()
        { 
            for(int i=0;i<FACTIONS;i++) {r[i]=NEUTRAL;v[i]=NEUTRAL;}
        }
        int IsViewedAs(int view, Reputation* viewer)
        {
            int val[7]={ENEMY,CRIMINAL,NOT_FRIENDLY,NEUTRAL,FRIENDLY,EMPLOYEE,ALLY};
            int rep=0,arep=0;
            for(int i=0;i<FACTIONS;i++)
            {
                if(viewer->v[i] == view)
                {
                    for(int j=0;j<7;j++)
                    {
                        if(r[i]==val[j])
                        {
                            int tmp=j-3,atmp=abs(tmp);
                            if(arep<atmp){rep=tmp;arep=atmp;}
                            break;
                        }
                    }
                } 
            }
            return rep;
        }
        int AsViewedBy(Reputation* viewer)
        {
            int iAlly=IsViewedAs(ALLY, viewer);
            if(iAlly==-3) return ENEMY;//viewed as enemy
            int iEnemy=IsViewedAs(ENEMY, viewer);
            if(iEnemy>=1) return ENEMY;//friendly/employed/in with enemy groups
            
            if(iAlly==-2) return CRIMINAL;//viewed as criminal
            int iCriminal=IsViewedAs(CRIMINAL, viewer);
            if(iCriminal>=2) return CRIMINAL;//criminal or employed thereof.
            int iEmploy=IsViewedAs(EMPLOYEE, viewer);
            if(iEmploy<=-2) return CRIMINAL;//criminal/enemy of employees.
            
            if(iAlly==3) return ALLY;//viewed as ally
            
            if(iAlly==2) return EMPLOYEE;//viewed as employee
            if(iEmploy==3) return EMPLOYEE;//employed.
            
            if(iAlly==1) return FRIENDLY;
            if(iAlly==-1) return NOT_FRIENDLY;
            if(iCriminal==1) return NOT_FRIENDLY;//friendly with criminals
            if(iEmploy==2) return FRIENDLY;//employee of employed faction (indirect employee - like a trade faction)
            
            if(iEnemy<=-2) return FRIENDLY;//you support our war effort?
            if(iCriminal<=-2) return FRIENDLY;//you're a vigilante?
            
            return NEUTRAL;
            /*
            so, the idea here with Employee/Friendly/Neutral/NotFriendly is 
            that Employees get lowered prices and all trade options (of faction)
            Friendly get extra some trade options/lowered prices
            Neutral gets the regular stock.
            NotFriendly has higher prices/some item-trade restrictions.
            
            Further, if you're employed by the federation and attack a fed Trade faction
            you become an enemy of the Trade faction and a criminal of the federation.
            
            While you are being pursued by federation police, if you attack them, you become an enemy of the federation.
            */
        }
};

class Faction : public Reputation
{
    public:
        int f;
        Faction(int facType)
        {
            f=facType;
            v[facType]=ALLY;
            r[facType]=ALLY;
        }
        void Associate(Faction* fac, int view)
        {
            fac->r[f]=view;
            fac->v[f]=view;
            r[fac->f]=view;
            v[fac->f]=view;
        }
        void Associate(Reputation* rep, int view)
        {
            rep->r[f]=view;
            rep->v[f]=view;
        }
};


class SceneryObject//self-explanatory
{
    public:
        int draw_x,draw_y,draw_w,draw_h,  draw_a;
        int a;
        
        //img
        void LoadImage(){}
        void UnloadImage(){}
        void Draw(){}
        void Turn(int radians=1){draw_a+=radians;}
        void Move(int x, int y){draw_x+=x;draw_y+=y;}
        void Move(int n){/* Move 'n' Units based off of Angle 'a' from object center*/}
        SceneryObject()
        {
            draw_x=0;draw_y=0;draw_w=0;draw_h=0;draw_a=0;a=0;
        }
        ~SceneryObject()
        {
            UnloadImage();
        }
};
class PhysicalObject : public SceneryObject//asteroids, perhaps?
{
    public:
        bool immune;
        bool solid;
        int collisDmg;
        int health,healthmax;
        void die(){}
        void damage(int d){ health-=d; if(health<=0) die(); }
        
        
        PhysicalObject()
        {
            immune=false;
            solid=false;
            collisDmg=0;
            health=100;healthmax=100;
        }
        ~PhysicalObject()
        {
            
        }
};
class Ship : PhysicalObject
{
    int credits;
    int capacity;
    public:
        Ship()
        {
            credits=0;
            capacity=0;
            MessageBox(NULL,"Ship constructor","t",MB_OK);
        }
        ~Ship()
        {
        }
    
};
class PlayerShip : Ship
{
    public:
        Reputation* rep;
        PlayerShip(){rep=new Reputation();
        MessageBox(NULL,"PlayerShip constructor","t",MB_OK);
        }
        ~PlayerShip(){delete rep;}
    
};
class NPCShip : Ship
{
    public:
        Faction* rep;
        NPCShip(){rep=new Faction(FACTION_NONE);}
        NPCShip(Faction* fac){rep=fac;}
        ~NPCShip(){delete rep;}
};


