/**
@file Equipment.h
@brief Provides objects and storage information for virtual vessels.

...
@author crashenator (crashenator -at- gmail.com)
*/

#pragma once
#include "common.h"
#include <vector>

#define EQUIPMENT(type,sub) EQUIP_ ## type, type ## _ ## sub

enum EquipmentType{
	/// enumeration of types of equipment a vessel can carry
	/// while not all of these are explicitly "equipment" some are simply "cargo"; the term "equipment" is used because upgrades don't necessarily cost space.

	EQUIP_DEVICE_UNKNOWN=-2,
	EQUIP_UNKNOWN=-1,
	EQUIP_REACTOR,
	EQUIP_ENGINE,
	EQUIP_GENERATOR,//field generator, as well as repair facilities (implementation ease)
	EQUIP_WEAPON,
	EQUIP_STORAGE
};
enum EquipmentSubType/// enumeration defining more specifically from the equipment type
{
	SUBTYPE_UNKNOWN=-1,
	REACTOR_FUEL,
	REACTOR_FISSION,
	REACTOR_FUSION,
	REACTOR_ANTIMATTER,

	ENGINE_COMBUSTION,
	ENGINE_IMPULSE,
	ENGINE_WARP,
	ENGINE_HYPERSPACE,

	GENERATOR_REPAIR,//a generator, implementation-wise.
	GENERATOR_SHIELD,
	GENERATOR_FUEL,


	WEAPON_LASERTURRET,
	WEAPON_PLASMATURRET,
	WEAPON_PLASMACANNON,
	WEAPON_TRACTOR,
	WEAPON_MINELAYER,
	WEAPON_MISSLELAUNCHER,

	STORAGE_ENERGY,
	STORAGE_FUEL,
	STORAGE_PASSENGER,
	STORAGE_SHIELD,

	STORAGE_ENUM_MAX
};

const char* const  EquipmentNames[STORAGE_ENUM_MAX]={/// display names for equipment subtypes.
	"Fuel",
	"Fission",
	"Fusion",
	"Antimatter",

	"Combustion",
	"Impulse",
	"Warp",
	"Hyperspace",

	"Automatic Repair System",
	"Shield Generator",
	"Dust Scoop",

	"Laser Turret",
	"Plasma Turret",
	"Plasma Cannon",
	"Tractor Beam",
	"Mine Layer",
	"Missile Launcher",

	"Energy Cell",
	"Fuel Tank",
	"Living Quarters",
	"Shield Extender"
};

const char* const  EquipmentImages[STORAGE_ENUM_MAX]={/// sprite path strings for equipment subtypes.
	"./Img/equip/reactor_fuel.bmp",
	"./Img/equip/reactor_fission.bmp",
	"./Img/equip/reactor_fusion.bmp",
	"./Img/equip/none.bmp",

	"./Img/equip/none.bmp",
	"./Img/equip/none.bmp",
	"./Img/equip/none.bmp",
	"./Img/equip/none.bmp",

	"./Img/equip/none.bmp",
	"./Img/equip/generator_shield.bmp",
	"./Img/equip/none.bmp",

	"./Img/equip/none.bmp",
	"./Img/equip/none.bmp",
	"./Img/equip/none.bmp",
	"./Img/equip/none.bmp",
	"./Img/equip/none.bmp",
	"./Img/equip/none.bmp",

	"./Img/equip/storage_energy.bmp",
	"./Img/equip/none.bmp",
	"./Img/equip/none.bmp",
	"./Img/equip/storage_shield.bmp"
};




class Statistic
{
	/// class implementing logic and storage for an in-game statistic.
	/// statistics are named, follow min-max logic, and offer comparison and range-expansion functions.
public:
	string name;
	int minimum;
	int maximum;
	int current;
	inline bool valid() const;//is `current` in the range [minimum,maximum]
	inline bool full() const;
	inline bool empty() const;
	inline bool inRange(const int& i) const;//is an input valid in the range [minimum,maximum] ?

	float percent() const;
	void set(int n, int x);

	bool assign(const int& i);//Strict: assigns current value only if within bounds.
	bool increment(const int& i);//Non-strict: changes, but limits the change inside the boundaries eg: (1 decrement-by 2)=-1 will limit to 0
	bool decrement(const int& i);
	bool incrementStrict(const int& i);//Strict: only changes if the change is within min/max boundaries
	bool decrementStrict(const int& i);

	inline void expandBy(const int& change);//raise the maximum bound (for consistency with reduceBy)
	void reduceBy(const int& change);//lower the maximum bound AND sets the current value to within the bounds.
	void reduceTo(const int& newmax);//set the maximum bound (must be lower) and reset the current (same behavior as above)

	string toString();

	operator int() const;//implicit and explicit cast operator, returns this->current
	int& operator=(const int& i);
	inline int& operator+=(const int& i);
	inline int& operator-=(const int& i);
	Statistic();
	Statistic(int n, int x);//sets min/max and defaults current to maximum
	Statistic(int i,int n, int x);//sets min/max and assigns current if i is in the range [n,x], else it defaults to minimum
};

class ObjectStats/// object containing all of the statistics required for an in-game object.
{
public:
	Statistic health;
	Statistic cargo;
	Statistic energy;
	Statistic fuel;
	Statistic shields;
	Statistic quarters;
	void setMax();
	string toString();
	ObjectStats();
};


class Equipment/// class providing control and logic about a piece of in-game equipment, statistic and resource manipulation.
{
public:
	EquipmentType equipmentType;
	EquipmentSubType subType;

	string name;
	Statistic level;//for upgrades
	int baseValue;//for price and selling 
	int capacity;
	int timer;

	bool present;
	bool online;

	
	bool canBeSold;
	bool canBeRemoved;
	bool canEnable;

	ObjectStats requirements;


	virtual string getProductionInfo(ObjectStats& stats);



	void setSubTypeName(int sub, const string& sTypeName);


	virtual bool checkRequirements(ObjectStats& stats);

	virtual void levelUp(ObjectStats& stats, int multiple=1);



	virtual bool startup(ObjectStats& stats);//adds the equipment and enables it (return: isStartedUp)
	virtual bool shutdown(ObjectStats& stats);//disables and removes the equipment (return: isShutDown)

	virtual bool enable(ObjectStats& stats)=0;//for `Devices` that can be activated (turned off/on without being uninstalled)
	virtual bool disable(ObjectStats& stats)=0;

	virtual void step(ObjectStats& stats);

	void init(EquipmentType type=EQUIP_UNKNOWN,EquipmentSubType sub=SUBTYPE_UNKNOWN);
	Equipment();
	~Equipment();
};

class Storage : public Equipment/// a piece of equipment which provides additional storage for a resource/statistic
{
private:
	Statistic& getConnectedStat(ObjectStats& stats);
public:
	inline bool enable(ObjectStats& stats);//dummy functions 
	inline bool disable(ObjectStats& stats);

	virtual void levelUp(ObjectStats& stats, int multiple=1);
	virtual bool startup(ObjectStats& stats);
	virtual bool shutdown(ObjectStats& stats);
	Storage(EquipmentSubType sub);
};

class Device : public Equipment/// a piece of Equipment which can be turned on or off.
{
public:
	virtual bool enable(ObjectStats& stats);//for devices that can be activated (turned off/on without being uninstalled)
	virtual bool disable(ObjectStats& stats);
	virtual bool startup(ObjectStats& stats);//by defaults these call the overriden Enable/Disable and the base Startup/Shutdown (read: turned off/on and installed/removed in appropriate order);
	virtual bool shutdown(ObjectStats& stats);
	Device();

};







class Reactor : public Device/// a device which generates Energy statistic, generally free of charge (except for Fuel Reactors)
{
public:
	//virtual void levelUp(ObjectStats& stats);
	//virtual bool startup(ObjectStats& stats);
	//virtual bool shutdown(ObjectStats& stats);

	void step(ObjectStats& stats);
	bool enable(ObjectStats& stats);
	bool disable(ObjectStats& stats);
	Reactor(EquipmentSubType sub);

};
class Engine : public Device/// NOT IMPLEMENTED: a device which provides movement capability - possibly at the cost of either energy or fuel(?)
{
public:
	Engine(EquipmentSubType sub);
};

class Generator : public Device/// a device which generates a set Statistic (Health,Shields,Fuel) at the cost of Energy
{
private:
	Statistic& getConnectedStat(ObjectStats& stats);
public:
	bool enable(ObjectStats& stats);
	bool disable(ObjectStats& stats);
	void step(ObjectStats& stats);
	Generator(EquipmentSubType sub);
};
class Weapon : public Device/// NOT IMPLEMENTED: a device that provides offensive/attack capabilities at the cost of energy.
{
public:
	Weapon(EquipmentSubType sub);
};




class EquipmentContainer
{
public:
	vector<Equipment*> list;
	void startup(ObjectStats& stats);
	void shutdown(ObjectStats& stats);
	void step(ObjectStats& stats);
	void clear(ObjectStats& stats);
	//void add(Equipment* equip);
	Equipment& add(EquipmentType type, EquipmentSubType=SUBTYPE_UNKNOWN);
	void remove(ObjectStats& stats,vector<Equipment*>::iterator& item);
	void Release();
	~EquipmentContainer();
};