/**
@file UI.h
@brief provides UI sprites and HUD control

...
@author crashenator (crashenator -at- gmail.com)
*/

#pragma once
#include "drawing.h"
#include "gamestrings.h"
#include "objects.h"
#include "ObjectContainer.h"
#include "Equipment.h"

enum UIType
{
	UI_NONE=0,
	UI_CURSOR   =1,
	UI_EQUIPMENT=2,
	UI_DOCK     =4,
	UI_DIALOG   =8
};

class UI
{
private:
	unsigned int ageFade;
	int soEquipSelectDestinationX;
public:
	ObjectContainer& container;
    UIType show;


	Ship shDummy;
	SceneryObject soCursor;
	SceneryObject soObjectSelect;

	SceneryObject soEquipmentUI;
	SceneryObject soEquipSelect;
	SceneryObject soEquipment;
	int player_idx;

	int offsetEquipment;
	int offsetEquipSelect;
	bool toggleEquipSelect;
	bool notifyEquipRemove;
	vector<Equipment*>::iterator iEquipSelect;
	int idxEquipSelect;
	string sEquipSelect;



	RECT rct;
	void Debug();


	bool toggle(UIType t);
	void setShip(PlayerShip* sh);
	void setCursorPos(const POINT3& pt);
	void DrawEquipment();
	void StepEquipment();



	void Startup();

	void Draw();
	void Step();

	UI(ObjectContainer& cont);
	~UI(void);
};


class HUD
{
	private:
		void drawMinimap();
		void drawStats();
		void drawMessage();
	public:
		ObjectContainer& container;
		int player_idx;
        ObjectStats lastPlayerStats;

		GameStrings& strings;
		string message;
        long int text_time;
		long int& steps;

		SceneryObject barX;//filled-in
		SceneryObject barC;//container
		SceneryObject barE;//end of container
		SceneryObject barB;//separator
		SceneryObject bar2;//end of filled-in

		
		void setMessage(const string& s);

		void Startup();
		void Step();
		void Draw();


		HUD(ObjectContainer& cont, GameStrings& gstrings, long int& isteps);
		~HUD(void);
};