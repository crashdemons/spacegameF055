#include "factions.h"

bool ReputationFilter::getMatch(int type)
{
    if(a) return true;
    if(n) return false;
    return omatch[type];
}
void ReputationFilter::setMatch(int type,bool match,bool all,bool none){
    a=all;n=none;
    omatch[type]=match;
}
void ReputationFilter::setMatchOnly(int type,bool match)
{
    a=false;n=false;
    for(int i=0;i<REPUTATION_VALUES;i++)
            if(i==type){omatch[i]=match;}
            else{omatch[i]=!match;}
}
ReputationFilter::ReputationFilter(){ for(int i=0;i<REPUTATION_VALUES;i++) omatch[i]=true; }

Reputation::Reputation()
{ 
    for(int i=0;i<FACTIONS;i++) {r[i]=NEUTRAL;v[i]=NEUTRAL;}
}
void Reputation::setViewOn(int f,int view){v[f]=view;}
void Reputation::setReputationWith(int f,int view){r[f]=view;}
int Reputation::AsViewedBy_Indirect(int view, Reputation* subject)
{
    //A.IsViewedAs(ENEMY,Viewer)
    //A IsViewedAs __ by Enemies of Viewer
    
    int val[7]={ENEMY,CRIMINAL,NOT_FRIENDLY,NEUTRAL,FRIENDLY,EMPLOYEE,ALLY};
    int rep=0,arep=0;
    for(int i=0;i<FACTIONS;i++)
    {
        if(subject->v[i] == view)
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
int Reputation::AsViewedBy(Reputation* viewer)
{
    int iAlly=AsViewedBy_Indirect(ALLY, viewer);
    if(iAlly==-3) return ENEMY;//viewed as enemy
    int iEnemy=AsViewedBy_Indirect(ENEMY, viewer);
    if(iEnemy>=1) return ENEMY;//friendly/employed/in with enemy groups
    
    if(iAlly==-2) return CRIMINAL;//viewed as criminal
    int iCriminal=AsViewedBy_Indirect(CRIMINAL, viewer);
    if(iCriminal>=2) return CRIMINAL;//criminal or employed thereof.
    int iEmploy=AsViewedBy_Indirect(EMPLOYEE, viewer);
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

Faction::Faction()
{
    f=FNONE;
    v[FNONE]=ALLY;
    r[FNONE]=ALLY;
}
Faction::Faction(int facType)
{
    f=facType;
    v[facType]=ALLY;
    r[facType]=ALLY;
}
int Faction::getFaction()
{
    return f;
}
void Faction::Associate(Faction* fac, int view)
{
    fac->r[f]=view;
    fac->v[f]=view;
    r[fac->f]=view;
    v[fac->f]=view;
}
void Faction::Associate(Reputation* rep, int view)
{
    rep->r[f]=view;
    rep->v[f]=view;
}
