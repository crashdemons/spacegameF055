#include "gamestrings.h"

#define SCPY(a,b,x) for(int i=0;i<x;i++)a[i]=b[i];
#define SCPY2(a,b,x,y) for(int i=0;i<x;i++)for(int j=0;j<y;j++)a[i][j]=b[i][j];


GameStrings::GameStrings()
{
	string a[FACTIONS]=
	{
		"",//none
		"Federation",
		"Rebel",
		"Ch'nik",//lol?
		"",//pirates won't call themselves pirates
		"Federation Trade",
		"Rebel Trade",
		"Trade",
		""//bounty hunters
	};
	SCPY(Factions_Self,a,FACTIONS);

	string b[FACTIONS]=
	{
		"Unaffiliated",
		"Federation",
		"Rebel",
		"Alien",
		"Pirate",
		"Federation Trade",
		"Rebel Trade",
		"Trade",
		"Bounty Hunter"
	};
	SCPY(Factions,b,FACTIONS);

	string c[3]=
	{
		"State your business.",
		"How can we be of assistance?",
		"Surrender and you will be spared."
	};
	SCPY(Generic_Hails,c,3);

	string d[FACTIONS][3]=
	{
		{Generic_Hails[0],Generic_Hails[1],Generic_Hails[2]},
		{Generic_Hails[0],Generic_Hails[1],"We order you to immediately surrender."},
		{Generic_Hails[0],Generic_Hails[1],"We order you to immediately surrender."},
		{"[No Response]","Mwba slth'ta ch'su.","S'ktu mral shu k'va!"},
		{Generic_Hails[0],"We're always ready for an easy target.","Prepare to be boarded!"},
		{Generic_Hails[0],Generic_Hails[1],"Stop, criminal!"},
		{Generic_Hails[0],Generic_Hails[1],"Stop, criminal!"},
		{Generic_Hails[0],Generic_Hails[1],Generic_Hails[2]},
		{Generic_Hails[0],"We're ready for a target.","Make peace with your personal deity!"}
	};
	SCPY2(Faction_Hails,d,FACTIONS,3);

	string e[6]=
	{
		"Shields at %d%% strength.",
		"Shields failing!",
		"Shields have failed!",
		"Hull integrity at %d%%.",
		"Hull breach imminent! %d%%",
		"Your ship has been obliterated!",
	};
	SCPY(Warnings,e,6);
	previousWarning=-1;
}

string GameStrings::getWarningString(ObjectStats& sh, ObjectStats& lsh)
{
	//if(sh==NULL || lsh==NULL) return "";
/*    float healthmax=sh->healthmax;
    float health=sh->health;
    float shields=0;if(health>healthmax){ shields=health-healthmax; health=healthmax; }
    float shieldmax=100;if(shields>shieldmax) shieldmax=shields;
	
    
    float lhealthmax=lsh->healthmax;
    float lhealth=lsh->health;
    float lshields=0;if(lhealth>lhealthmax){ lshields=lhealth-lhealthmax; lhealth=lhealthmax; }
    float lshieldmax=sh->shield_max;if(lshields>lshieldmax) lshieldmax=lshields;
    
    if(health<0) health=0;
    if(lhealth<0) lhealth=0;
	*/

	int lhealth=lsh.health;
	int health=sh.health;
    
	float lshield_percent=lsh.shields.percent()*100;
	float lhealth_percent=lsh.health.percent()*100;
    float shield_percent=sh.shields.percent()*100;
    float health_percent=sh.health.percent()*100;
    int iref=-1,ifmt=-1;
    float fval=shield_percent;
    
    if(fabs(shield_percent-50)<=10){ ifmt=0; iref=0;}
    if(fabs(shield_percent-25)<=10){ ifmt=0; iref=1;}
    if(lshield_percent>5 && shield_percent<=5) { ifmt=1; iref=2;}
    if(lshield_percent>0 && shield_percent==0) { ifmt=2; iref=3;}
    
    if(fabs(health_percent-50)<=10){ ifmt=3; iref=4;}
    if(fabs(health_percent-25)<=10){ ifmt=3; iref=5;}
    if(lhealth_percent>5 && health_percent<=5) { ifmt=4; iref=6;}
    if(lhealth>0          &&        health<=0) { ifmt=5; iref=7;}
    
    if(ifmt==-1) return "";
    if(iref==previousWarning) return "";
    previousWarning=iref;
    
    if(ifmt>=3) fval=health_percent;
    char* ret=new char[100];
	sprintf(ret,Warnings[ifmt].c_str(),int(fval));
    return ret;
}
