#pragma once
#include "common.h"
#include "factions.h"// we need this to set filters for Criminal, Enemy etc.
#include "ObjectContainer.h"// we need this for scanning for objects.

enum AIType{ ///< Types of AI
	AI_TYPE_NONE,
	AI_TYPE_SHIP,
	AI_TYPE_PROJECTILE
};

enum AISequence{
	AI_SEQ_WAIT,
	AI_SEQ_LURK,
	AI_SEQ_ARRIVE,
	AI_SEQ_DEPART,
	AI_SEQ_SURVEY,
	AI_SEQ_FOLLOW,
	AI_SEQ_RETREAT,
	AI_SEQ_ATTACK,
	AI_SEQ_BOARD
};
enum AISeqState{
	AI_SEQSTATE_UNINITIATED,
	AI_SEQSTATE_INITIATED,
	AI_SEQSTATE_INTERRUPTED,
	AI_SEQSTATE_COMPLETED
};

enum AICommandType{ ///< Commands the AI performs under certain conditions
	AI_COMMAND_NONE,
	AI_COMMAND_SCAN,
	AI_COMMAND_TURN,
	AI_COMMAND_ACCELERATE,
	AI_COMMAND_DECELERATE,
	AI_COMMAND_DOCK,
	AI_COMMAND_WARP,
	AI_COMMAND_TARGET,
	AI_COMMAND_FIGHTERS,
	AI_COMMAND_FIRE,
	AI_COMMAND_DIE,
};

class AICommand{
public:
	AICommandType type;
	int index;
	float f_value;
	AICommand(AICommandType t=AI_COMMAND_NONE, int idx=-1, float fval=0.0f);
	//AICommand();
};

class Scan{
public:
	void* container;
	int self;
	Range rn;
	ObjectFilter of;
	ReputationFilter rf;
	vector<pair<int,float>> results;
	void setParameters(void* cont, int index, Range range, ObjectFilter objfilter, ReputationFilter repfilter);
	void update();
	int getClosestResult();
};




class AI{
private:
	bool seqchanged;///< internal variable; Has the sequence changed since the last request
	virtual void seqRevise();
	virtual void seqInitiate();
	virtual void seqContinue();
	virtual void seqTerminate();
public:
	void* container;
	int object_index;
	AIType type;
	Scan scan;

	
	AISequence objective;
	int objective_target;



	AISequence sequence;
	int target;
	AISeqState seqstate;
	vector<AICommand> commands;


	bool armed;
	bool carrier;

	bool sequenceChanged();
	void setSequence(AISequence seq);
	void setSeqState(AISeqState state);
	void resumeObjective();

	void interruptSequence(bool hard=false);
	void step();
	void attach(void* pContainer, int self_index);
	AI();
};


class AI_Ship:AI{
private:
	virtual void seqRevise();
	virtual void seqInitiate();
	virtual void seqContinue();
	//virtual void seqTerminate();
public:
	AI_Ship();
};

class AI_Proj:AI{
private:
	virtual void seqRevise();
	virtual void seqInitiate();
	virtual void seqContinue();
	//virtual void seqTerminate();
public:
	AI_Proj();
};

class Captain{
public:
	AI ai;
	AICommand getCommand();

};

