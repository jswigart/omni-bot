////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __TACTICALMANAGER_H__
#define __TACTICALMANAGER_H__

#include "CommandReciever.h"
#include "PathQuery.h"

class System;
class Client;

struct EntityInstance;

// Title: TacticalManager

enum BehaviorAction
{
	BEHAVIOR_NONE,
	BEHAVIOR_KILL,
	BEHAVIOR_PRESSBUTTON,
	BEHAVIOR_RESUPPLY,
	BEHAVIOR_GET_WEAPON,
	BEHAVIOR_GET_POWERUP,
	BEHAVIOR_GET_FLAG,
	BEHAVIOR_CAP_FLAG,
	BEHAVIOR_CAP_CONTROL_POINT,
	BEHAVIOR_DEFEND,
};

typedef EnumerationValues<BehaviorAction> BehaviorActionEnum;

struct Stimulus
{
	GameEntity				mEntity;
	EntityInfo				mEntInfo;

	Vector3f				mPosition;
	uint64_t				mNavArea;

	uint32_t				mMaxUsers;
	float					mDesirability;

	Stimulus();
	~Stimulus();

	void Clear();
};

typedef std::shared_ptr<Stimulus> StimulusPtr;

struct StimulusBehavior
{
	GameEntity				mUser;
	StimulusPtr				mStimulus;
	BehaviorAction			mAction;

	float					mDesirability;
	float					mNavigationCost;
	float					mNavigationThreat;
	float					mFinalCost;

	StimulusBehavior();
};

typedef std::vector<StimulusBehavior> Behaviors;

struct StimulusUser
{
	GameEntity				mEntity;
	GameEntity				mReservedStimulus;

	Vector3f				mUserPos;

	QueryRef				mQuery;
	Behaviors				mBehaviorsAnalyze;
	Behaviors				mBehaviorActive;

	StimulusUser();
	~StimulusUser();

	void Clear();
};

// class: TacticalManager
class TacticalManager : public CommandReciever
{
public:
	void Init( System & system );
	void Update( System & system );
	void Shutdown();
	void Reset();

	bool Load( const std::string &_map, ErrorObj &_err );
	bool Save( const std::string &_map, ErrorObj &_err );

	void EntityCreated( const EntityInstance &ei );
	void EntityDeleted( const EntityInstance &ei );
	
	TacticalManager();
	virtual ~TacticalManager();
protected:
	void InitCommands();
	void cmdLoad( const StringVector & args );
	void cmdSave( const StringVector & args );
	void cmdShowTokens( const StringVector & args );
private:
	StimulusUser				mUsers[ Constants::MAX_PLAYERS ];
	StimulusPtr					mStimulus[ Constants::MAX_ENTITIES ];

	int							mShowTokenClient;
	
	void UpdateBehaviorTokens( System & system );
};

#endif
