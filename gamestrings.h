/**
@file gamestrings.h
@brief provides and constructs text seen in-game.

...
@author crashenator (crashenator -at- gmail.com)
*/

#pragma once
#include "common.h"
#include "factions.h"
#include "objects_PlayerShip.h"


class GameStrings
{
public:
	string Factions_Self[FACTIONS];
	string Factions[FACTIONS];
	string Generic_Hails[3];
	string Faction_Hails[FACTIONS][3];
	string Warnings[6];
	int previousWarning;
	GameStrings();
	string getWarningString(ObjectStats& sh, ObjectStats& lsh);
};