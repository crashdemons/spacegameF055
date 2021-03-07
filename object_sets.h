/**
@file object_sets.h
@brief 

...
@author crashenator (crashenator -at- gmail.com)
*/

#pragma once
#pragma pack(1)




struct Projectile_Type{
    long int lifespan;
	bool targeted;
    char image[255];
    int transcolor;
    float spin;
    bool randmove;
    
    float bumpforce;
    int health;
    int collisDmg;
    bool killsProjectiles;
    bool collisStops;
    bool canmove;
    bool explode;
    bool hitnotify;
    bool hasTrail;
    bool hasGlow;
	int maskTrail;
	int maskGlow;
    
    void* deathspawn;
    int deathspawn_n; 
    float deathspawndist;
    float deathspawnveloc;
	
};


typedef struct{
} Ship_Type;


//etc.



//const char* Image_Asteroid="./Img/as1.bmp";
