#include "AI.h"
#include "ObjectContainer.h"
#include "objects_Physical.h"
#include "objects_Projectile.h"
#include "objects_Ship.h"
/*
AICommand::AICommand(){
	type=AI_COMMAND_NONE;
	index=-1;
	f_value=0.0f;
}
*/
AICommand::AICommand(AICommandType t, int idx, float fval){
	type=t;
	index=idx;
	f_value=fval;
}
//--------------------------------------------------------------------------
void Scan::setParameters(void* cont, int index, Range range, ObjectFilter objfilter, ReputationFilter repfilter){
	self=index;
	container=cont;
	of=objfilter;
	rf=repfilter;
}
void Scan::update(){
	results.clear();
	if(container==NULL) return;
	ObjectContainer* cont=(ObjectContainer*) container;
	results=cont->getRelativeFilteredEx(self,&of,&rf,&rn,true,100);
}
int Scan::getClosestResult(){
	int closest=-1;
	float dist_min=9999.9999f;
	bool initial=true;
	for(vector<pair<int,float>>::iterator i=results.begin(); i!=results.end(); i++){
		if(initial || (i->second)<dist_min) { closest=i->first;  dist_min=i->second; }
		if(initial) initial=false;
	}
	return closest;
}
//--------------------------------------------------------------------------

bool followB(void* container, int object, int target,bool towards){
	if(object==-1 || target==-1) return false;
	ObjectContainer* cont=(ObjectContainer*) container;
	PhysicalObject* po=(PhysicalObject*)(cont->objects[object]);
	SceneryObject* so=(SceneryObject*)(cont->objects[target]);
	if(po==NULL || so==NULL) return false;
	SceneryObject* ship=so;//old code uses this.

	//--------------------
	bool turn=true;
	int accelerate=0;

	float s,a,a1=po->getAngleTo(ship);
	float dist2,dist=po->distTo(ship);
	
	ship->VelocityStepRel(20);//planning ahead
	po->VelocityStepRel(20);
	a=po->getAngleTo(ship);  if(!towards) a=-a;
	dist2=po->distTo(ship);
	po->VelocityStepRel(-20);
	ship->VelocityStepRel(-20);//end planning ahead

	bool goingaway=dist2>dist;
	float targetVeloc=dist2/20;
	bool ccw=po->TurnGetDirection(a,&s);
	s=fabs(s);

    float sweepMovement=realAngle(po->movementAngle)-realAngle(a);
            if(sweepMovement<0) sweepMovement=-sweepMovement;
            if(sweepMovement>PI) sweepMovement=PI2 - sweepMovement;

	float vm=po->VelocAvg();

	if(towards==dist<200){
		if(targetVeloc>vm || (s<( 5*DEGRAD) && sweepMovement>(90*DEGRAD)) ) accelerate=+1;//vm=missile velocity, s=sweep angle between projected coordinates and facing
		if(targetVeloc<vm || (s>(45*DEGRAD) && sweepMovement>(90*DEGRAD)) ) accelerate=-1;
		if(dist<100 && s<(5*DEGRAD) ) accelerate=+1;
	}else{
		if(s<(5*DEGRAD) ) accelerate=+1;
		else accelerate=-1;
	}

	if(towards==(sweepMovement>(45*DEGRAD) && vm>5.0)) accelerate=-1;

	if(ccw) a=realAngle(a);
	else a=realAngle(a);
        
	if(turn) po->TurnStepToward(a); 
	if(accelerate==+1 && vm<5.0) po->AccelerateRel((float) -(sweepMovement/3.14));
	if(accelerate==-1) po->Deccelerate(0.001f,0.95f);
	return (s<=(15*DEGRAD));
}

//---------------------------------------------------------------------------
AI::AI(){
	object_index=-1;
	type=AI_TYPE_NONE;
	objective=AI_SEQ_WAIT;
	objective_target=-1;
	sequence=AI_SEQ_WAIT;
	target=-1;
	seqstate=AI_SEQSTATE_UNINITIATED;
	seqchanged=true;
}
void AI::attach(void* pContainer, int self_index){
	container=pContainer;
	object_index=self_index;
	scan.container=container;
}
bool AI::sequenceChanged(){
	bool tmp=seqchanged;
	if(seqchanged) seqchanged=false;
	return tmp;
}
void AI::setSequence(AISequence seq){
	sequence=seq;
	seqstate=AI_SEQSTATE_UNINITIATED;
	seqchanged=true;
}
void AI::setSeqState(AISeqState state){
	seqstate=state;
}

void AI::interruptSequence(bool hard){
	if(!hard && (sequence==AI_SEQ_ATTACK || sequence==AI_SEQ_RETREAT)) return;//do not interrupt these major responses.
	setSeqState(AI_SEQSTATE_INTERRUPTED);
}
void AI::resumeObjective(){
	sequence=objective;
	target=objective_target;
	seqstate=AI_SEQSTATE_UNINITIATED;
	seqchanged=true;
}

void AI::step(){
	switch(seqstate){
	case AI_SEQSTATE_UNINITIATED:
		seqInitiate();
		break;
	case AI_SEQSTATE_INITIATED:
		seqContinue();
		break;
	case AI_SEQSTATE_COMPLETED:
		seqTerminate();
		break;
	case AI_SEQSTATE_INTERRUPTED:
		seqRevise();
		break;
	}
}
void AI::seqInitiate(){
	seqstate=AI_SEQSTATE_INITIATED;
}
void AI::seqContinue(){
	//checkScan();
	return;//the basic AI only knows how to wait so we never allow this sequence to complete.
}
void AI::seqTerminate(){
	if(sequence==objective) objective=AI_SEQ_DEPART; //if our objective is complete, depart the system.
	resumeObjective();//if we completed our current sequence, but it was not our objective, return to our objective.  
}
void AI::seqRevise(){
	sequence=AI_SEQ_WAIT;//the basic AI only knows how to wait.
}
//------------------------------------------------------------------------
AI_Ship::AI_Ship(){
	type=AI_TYPE_SHIP;
}
void AI_Ship::seqRevise(){
	seqstate=AI_SEQSTATE_UNINITIATED;
	if(scan.results.empty()){ commands.push_back(AICommand(AI_COMMAND_SCAN)); resumeObjective(); }
	else{
		int enemy=-1;
		if(enemy>-1){//enemy in range found.
			commands.push_back(AICommand(AI_COMMAND_TARGET,enemy));
			if(carrier) commands.push_back(AICommand(AI_COMMAND_FIGHTERS,enemy)); // launch fighters
			if(armed) setSequence(AI_SEQ_ATTACK);
			else      setSequence(AI_SEQ_RETREAT);
		}else resumeObjective();// can't see an enemy, continue as I were.
	}
}
void AI_Ship::seqInitiate(){
	seqstate=AI_SEQSTATE_INITIATED;
}

void AI_Ship::seqContinue(){
	switch(sequence){
	case AI_SEQ_WAIT:
		seqstate=AI_SEQSTATE_COMPLETED;
		return;
	case AI_SEQ_LURK:
		scan.update();
		seqstate=AI_SEQSTATE_COMPLETED;
		if(target==-1) target=scan.getClosestResult();
		break;
	case AI_SEQ_ATTACK:
	case AI_SEQ_ARRIVE:
		followB(container,object_index,target,true);
		//fly to target and dock, then Complete.
		//if no target, fly to center then Complete.
		break;
	case AI_SEQ_RETREAT:
	case AI_SEQ_DEPART:
		followB(container,object_index,target,false);
		//fly from target (how far?), then Complete.
		//if no target, fly away from center then Complete.
		break;
	}
	//seqstate=AI_SEQSTATE_INITIATED;
}

//------------------------------------------------------------------------


AI_Proj::AI_Proj(){
	type=AI_TYPE_PROJECTILE;
}
void AI_Proj::seqRevise(){
	seqstate=AI_SEQSTATE_UNINITIATED;
	setSequence(AI_SEQ_ATTACK);
	if(scan.results.empty()){ commands.push_back(AICommand(AI_COMMAND_SCAN)); resumeObjective(); }
	else{
		int enemy=-1;
		if(enemy>-1){//enemy in range found.
			commands.push_back(AICommand(AI_COMMAND_TARGET,enemy));
			setSequence(AI_SEQ_ATTACK);
		}else resumeObjective();// can't see an enemy, continue as I were.
	}
}
void AI_Proj::seqInitiate(){
	seqstate=AI_SEQSTATE_INITIATED;
}

void AI_Proj::seqContinue(){
	switch(sequence){
	case AI_SEQ_WAIT:
		
		return;
	case AI_SEQ_LURK:
		scan.update();
		seqstate=AI_SEQSTATE_COMPLETED;
		if(target==-1) target=scan.getClosestResult();
		break;
	case AI_SEQ_ATTACK:
	case AI_SEQ_ARRIVE:
		followB(container,object_index,target,true);
		//fly to target and dock, then Complete.
		//if no target, fly to center then Complete.
		break;
	case AI_SEQ_RETREAT:
	case AI_SEQ_DEPART:
		followB(container,object_index,target,false);
		//fly from target (how far?), then Complete.
		//if no target, fly away from center then Complete.
		break;
	}
	//seqstate=AI_SEQSTATE_INITIATED;
}