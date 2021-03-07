#include "Equipment.h"
#include <iostream>
#include <sstream>
using namespace std;

void Statistic::set(int n, int x)
{
	minimum=n;
	maximum=x;
	current=x;
}
Statistic::Statistic() : minimum(0), maximum(100), current(100)
{
}
Statistic::Statistic(int n, int x) : minimum(n), maximum(x), current(x)
{
}
Statistic::Statistic(int i, int n, int x) : minimum(n), maximum(x), current(n)
{ if(RANGE(i,minimum,maximum)) current=i; }


inline bool Statistic::valid() const{ return RANGE(current,minimum,maximum); }
inline bool Statistic::inRange(const int& i) const{ return RANGE(i,minimum,maximum); }
inline bool Statistic::full() const{ return current>=maximum; }
inline bool Statistic::empty() const{ return current<=minimum; }


float Statistic::percent() const
{ 
	if(empty()) return 0.0f;
	return ((float)(current-minimum))/(maximum-minimum); 
}


bool Statistic::incrementStrict(const int& i)
{
	int tmp=current+i;
	if( tmp > maximum ) return false;
	current=tmp;
	return true;
}
bool Statistic::decrementStrict(const int& i)
{
	int tmp=current-i;
	if( tmp < minimum ) return false;
	current=tmp;
	return true;
}
bool Statistic::increment(const int& i)
{
	current+=i;
	if(current>maximum){ current=maximum; return false; }
	return true;
}
bool Statistic::decrement(const int& i)
{
	current-=i;
	if(current<minimum){ current=minimum; return false; }
	return true;
}


inline void Statistic::expandBy(const int& change){maximum+=change;}
void Statistic::reduceBy(const int& change){maximum-=change; if(full()) current=maximum;}//note: full() returns current>=maximum; so this resets larger (read:invalid) current values too.
void Statistic::reduceTo(const int& newv){if(maximum>newv) maximum=newv; if(full()) current=maximum;}


string Statistic::toString()
{
	char buffer[500]={0};
	sprintf(buffer,"%10s: %6d / %-6d",name.c_str(),current,maximum);
	return string(buffer);
	//return name+": "+itos(current)+"/"+itos(maximum);
}

int& Statistic::operator=(const int& i)
{
	current=i;
	if(i>maximum) current=maximum;
	if(i<minimum) current=minimum;
	return current;
}
inline int& Statistic::operator+=(const int& i){increment(i); return current;}
inline int& Statistic::operator-=(const int& i){decrement(i); return current;}
Statistic::operator int() const{return current;}


void ObjectStats::setMax()
{
	health.set(0,INT_MAX);
	shields.set(0,INT_MAX);
	energy.set(0,INT_MAX);
	fuel.set(0,INT_MAX);
	cargo.set(0,INT_MAX);
	quarters.set(0,INT_MAX);
}
string ObjectStats::toString()
{
	string out=health.toString()+"\n";
	out+=shields.toString()+"\n";
	out+=energy.toString()+"\n";
	out+=fuel.toString()+"\n";
	out+=cargo.toString()+"\n";
	out+=quarters.toString()+"\n";
	return out;
}
ObjectStats::ObjectStats()
{
	health.name="Health";
	shields.name="Shields";
	energy.name="Energy";
	fuel.name="Fuel";
	cargo.name="Cargo";
	quarters.name="Quarters";
}


//---------------------------------------------------------------------------------


bool Equipment::checkRequirements(ObjectStats& stats)
{
	return
			( stats.cargo           >= (requirements.cargo   ) ) &&
			( stats.energy.maximum  >= (requirements.energy  ) ) &&
			( stats.fuel.maximum    >= (requirements.fuel    ) ) &&
			( stats.health.maximum  >= (requirements.health  ) ) &&
			( stats.shields.maximum >= (requirements.shields ) ) ;
}
bool Equipment::startup(ObjectStats& stats){
	if(present) return true;
	present = stats.cargo.decrement(requirements.cargo);
	return present;
}
bool Equipment::shutdown(ObjectStats& stats){
	if(!present) return true;
	present = !stats.cargo.increment(requirements.cargo.current);
	return !present;
}

string Equipment::getProductionInfo(ObjectStats& stats){
	ObjectStats dummy=stats;
	int tmptimer=timer;
	timer=0;
	for(int i=0;i<100;i++) step(dummy);
	timer=tmptimer;
#define sdelta(x) dummy.x.current-stats.x.current;

	string s="";
	int d=0;
	d=sdelta(cargo);
	if(d!=0) s+="[Cargo "+itoss(d)+"] ";
	d=sdelta(energy);
	if(d!=0) s+="[Energy "+itoss(d)+"] ";
	d=sdelta(fuel);
	if(d!=0) s+="[Fuel "+itoss(d)+"] ";
	d=sdelta(health);
	if(d!=0) s+="[Health "+itoss(d)+"] ";
	d=sdelta(shields);
	if(d!=0) s+="[Shields "+itoss(d)+"] ";
	d=sdelta(quarters);
	if(d!=0) s+="[Quarters "+itoss(d)+"]";

	if(s.length()>0) s="Production: "+s+" (per 100 ticks)";


	return s;
}

void Equipment::step(ObjectStats& stats){
	timer++;
}
void Equipment::levelUp(ObjectStats& stats, int multiple){ level.increment(multiple); }


void Equipment::setSubTypeName(int sub, const string& sTypeName)
{
	if(sub==-1) sub=subType;
	name="Invalid";
	if(sub>0) name=string(EquipmentNames[sub]);
	name+=" "; name+=sTypeName;
}
/*
inline bool sable(ObjectStats& stats){ return false; }
inline bool Equipment::disable(ObjectStats& stats){ return false; }
*/
void Equipment::init(EquipmentType type,EquipmentSubType sub)
{
	equipmentType=type;
	subType=sub;

	setSubTypeName(-1,"Equipment");
	baseValue=0;
	canBeSold=true;
	canBeRemoved=true;
	canEnable=false;
	present=false;
	online=false;
	timer=0;
	capacity=0;
	level.current=0;
	requirements.cargo=0;
	requirements.energy=0;
	requirements.fuel=0;
	requirements.health=0;
	requirements.shields=0;
}

Equipment::Equipment()
{
	init();
}


Equipment::~Equipment()
{
	stringstream ss;
	ss<<"Equipment: "<<this<<" - Destructor Called"<<endl;
	OutputDebugString(ss.str().c_str());
	//MessageBox("DCTOR","");
}

//---------------------------------------------------------------------------------
inline bool Storage::enable(ObjectStats& stats){ return false; }//dummy functions 
inline bool Storage::disable(ObjectStats& stats){ return false; }

Statistic& Storage::getConnectedStat(ObjectStats& stats)
{
	switch(subType)
	{
	case STORAGE_ENERGY:
		return stats.energy;
	case STORAGE_PASSENGER:
		return stats.quarters;
	case STORAGE_SHIELD:
		return stats.shields;
	case STORAGE_FUEL:
	default:
		return stats.fuel;
	}
}
void Storage::levelUp(ObjectStats& stats, int multiple)
{
	Equipment::levelUp(stats,multiple);
	int change=20*multiple;
	capacity+=change;
	if(present) getConnectedStat(stats).expandBy(change);

}
bool Storage::startup(ObjectStats& stats){
	if(!present){ getConnectedStat(stats).expandBy(capacity); Equipment::startup(stats); }
	return present;
}
bool Storage::shutdown(ObjectStats& stats){
	if(present){ getConnectedStat(stats).reduceBy(capacity); Equipment::shutdown(stats); }
	return !present;
}
Storage::Storage(EquipmentSubType sub){
	init(EQUIP_STORAGE,sub);
	capacity=50;
	subType=sub;
	setSubTypeName(sub,"");
}
//-------------------------------------------------

Device::Device()
{
	init(EQUIP_DEVICE_UNKNOWN,SUBTYPE_UNKNOWN);
	setSubTypeName(-1,"Device");
	canEnable=true;
}
inline bool Device::enable(ObjectStats& stats){return false; }
inline bool Device::disable(ObjectStats& stats){return false; }
bool Device::startup(ObjectStats& stats){ 
	return Equipment::startup(stats) && enable(stats); 
}
bool Device::shutdown(ObjectStats& stats){ 
	return disable(stats) && Equipment::shutdown(stats); 
}

//---------------------------------------------------------------------------------

Reactor::Reactor(EquipmentSubType sub)
{
	init(EQUIP_REACTOR,sub);
	canEnable=true;
	capacity=20;
	setSubTypeName(sub,"Reactor");
}
bool Reactor::enable(ObjectStats& stats)
{
	if(!present) return false;
	if(online) return true;
	stats.energy.expandBy(capacity);
	online=true;
	return true;

}
bool Reactor::disable(ObjectStats& stats)
{
	if(!present) return false;
	if(!online) return true;
	stats.energy.reduceBy(capacity);
	online=false;
	return true;
}
void Reactor::step(ObjectStats& stats)
{	
	int recharge;
	int mod=15;
	if(stats.energy.empty()) mod*=2;
	timer%=mod;
	if(online && present && timer==0)
	{
		//the timer is autoincrementing, we need to reduce the rate of steps

		switch(subType)
		{
		case REACTOR_FUEL:
			recharge=1+1*level;
			if(stats.fuel.decrementStrict(requirements.fuel.current))//FUTURE NOTE: don't decrement if energy is full?
				stats.energy+=recharge;
			break;
		case REACTOR_FISSION:
			recharge=1+2*level;
			stats.energy+=recharge;
			break;
		case REACTOR_FUSION:
			recharge=1+2*level;
			stats.energy+=recharge;
			break;
		case REACTOR_ANTIMATTER:
			recharge=1+3*level;
			stats.energy+=recharge;
			break;
		}
	}
	Device::step(stats);
}

//-------------------------------------------------

Statistic& Generator::getConnectedStat(ObjectStats& stats)
{
	switch(subType)
	{
	case GENERATOR_REPAIR:
		return stats.health;
	case GENERATOR_SHIELD:
		return stats.shields;
	case GENERATOR_FUEL:
	default:
		return stats.fuel;
	}
}

bool Generator::enable(ObjectStats& stats)
{
	if(!present) return false;
	if(online) return true;
	if(capacity) getConnectedStat(stats).expandBy(capacity);
	online=true;
	return true;
}
bool Generator::disable(ObjectStats& stats)
{
	if(!present) return false;
	if(!online) return true;
	if(capacity) getConnectedStat(stats).reduceBy(capacity);
	online=false;
	return true;
}
void Generator::step(ObjectStats& stats)
{
	int recharge;
	if(online && present)
	{
		Statistic& stat=getConnectedStat(stats);
		int mod=15;
		if(stat.empty()) mod*=2;
		timer%=mod;
		if(timer==0){
			recharge=1+2*level;
			if(!stat.full()) if(stats.energy.decrementStrict(requirements.energy)) stat.increment(recharge);
		}

	}
	Device::step(stats);
}
Generator::Generator(EquipmentSubType sub)
{
	init(EQUIP_GENERATOR,sub);
	canEnable=true;
	switch(subType)
	{
	case GENERATOR_SHIELD:
	case GENERATOR_FUEL:
		capacity=50;
		break;
	}
	requirements.energy=1;
	setSubTypeName(sub,"");
}

//-------------------------------------------------

Engine::Engine(EquipmentSubType sub){ init(EQUIP_ENGINE,sub); canEnable=true; }
Weapon::Weapon(EquipmentSubType sub){ init(EQUIP_ENGINE,sub); canEnable=true; }
//-------------------------------------------------

//-------------------------------------------------




void EquipmentContainer::startup(ObjectStats& stats)
{
	for(vector<Equipment*>::iterator i=list.begin();i!=list.end();i++)
		if( (*i)->checkRequirements(stats) )  (*i)->startup(stats);
}
void EquipmentContainer::shutdown(ObjectStats& stats)
{
	for(vector<Equipment*>::iterator i=list.begin();i!=list.end();i++)
		(*i)->shutdown(stats);
}
void EquipmentContainer::step(ObjectStats& stats)
{
	for(vector<Equipment*>::iterator i=list.begin();i!=list.end();i++)
		(*i)->step(stats);
}

void EquipmentContainer::Release()
{
	Equipment* p=NULL;
	while(!list.empty()){
		p=list.back();
		//stringstream ss;
		//ss<<"Equipment: "<<p<<" - Releasing"<<endl;
		//OutputDebugString(ss.str().c_str());
		if(p!=NULL) {delete p; p=NULL;}

		list.pop_back(); 
	}
}

//inline 
//void EquipmentContainer::add(Equipment* equip){ if(equip!=NULL) list.push_back(equip); }
Equipment& EquipmentContainer::add(EquipmentType type, EquipmentSubType subtype){
	Equipment* pE=NULL;
	switch(type){
	case EQUIP_REACTOR:
		pE=new Reactor(subtype); break;
	case EQUIP_ENGINE:
		pE=new Engine(subtype); break;
	case EQUIP_GENERATOR:
		pE=new Generator(subtype); break;
	case EQUIP_WEAPON:
		pE=new Weapon(subtype); break;
	case EQUIP_STORAGE:
		pE=new Storage(subtype); break;
	default:
		pE=new Device();
	}
	list.push_back(pE);
	return *pE;
}
void EquipmentContainer::remove(ObjectStats& stats,vector<Equipment*>::iterator& item)
{
	if(item==list.end()) return;
	if( (*item)->shutdown(stats) ){
		delete *item;
		list.erase(item);
		item=list.end();
	}
}

void EquipmentContainer::clear(ObjectStats& stats)
{
	shutdown(stats);
	Release();
}
EquipmentContainer::~EquipmentContainer()
{
	ObjectStats dummy;
	shutdown(dummy);//shutdown routines done (stats changes not done)
	Release();//release all Equipment objects
}