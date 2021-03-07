/**
@file factions.h
@brief Faction and Reputation interactions and relative-view logic.

...
@author crashenator (crashenator -at- gmail.com)
*/
#pragma once
#define FACTIONS 9/// maximum number of factions
#define REPUTATION_VALUES 7/// maximum number of reputation values
#include "common.h"
enum FACTION_TYPE/// Types of formal factions supported in-game
{
    FNONE,///< no assigned faction, generally neutral
    FFED,///< Federation - engages enemies & criminals
    FREBEL,///< Rebels - engages enemies & criminals
    FALIEN,///< Aliens - ???
    FPIRATE, ///< Pirates - ???
    FFEDTRADE,///< Federation-aligned Traders - engages enemies only (which have to be attackers)
    FREBTRADE,///< Rebel-aligned Traders - engages enemies only (which have to be attackers)
    FFREETRADE,///< Free trade - engages enemies only (which have to be attackers)
    FBHUNTER ///< Bounty-hunters
};
enum REPUTATION_VALUE/// Types of reputation/alignment toward a faction [or other object] a ship or player can have.
{
    ENEMY,
    CRIMINAL,
    NOT_FRIENDLY,
    NEUTRAL,
    FRIENDLY,
    EMPLOYEE,
    ALLY
};

class ReputationFilter///a structure for setting options related to matching-logic based on Reputation
{
    public:
    bool a,n;
    bool omatch[REPUTATION_VALUES];
    bool getMatch(int type);
    void setMatch(int type,bool match=true,bool all=false,bool none=false);
    void setMatchOnly(int type,bool match=true);///use to consider all reputation values non-matching except the given value match
    ReputationFilter();
};

class Reputation///
{
    public:
        int r[FACTIONS];//what [FACTION] considers you  eg: ENEMY to REBELS
        int v[FACTIONS];//what you think of [Faction]
        Reputation();
        void setViewOn(int f,int view);
        void setReputationWith(int f,int view);
        int AsViewedBy(Reputation* viewer);
        int AsViewedBy_Indirect(int view, Reputation* subject);/// retrieves your view from:  people who view 'subject' a given way.  (ENEMY,subject) == retrieves your view from Enemies of subject
};

class Faction : public Reputation
{
    public:
        int f;
		Faction();
        Faction(int facType);
		int getFaction();
        void Associate(Faction* fac, int view);
        void Associate(Reputation* rep, int view);
};





