////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
// Title: TF Message Structure Definitions
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __TF_MESSAGES_H__
#define __TF_MESSAGES_H__

#include "Base_Messages.h"
#include "ET_Config.h"

#pragma pack(push)
#pragma pack(4)

//////////////////////////////////////////////////////////////////////////

struct ET_WeaponOverheated
{
	ET_Weapon mWeapon;
	obBool	 mIsOverheated;
};

struct ET_WeaponHeatLevel
{
	GameEntity mEntity;
	int		 mCurrent;
	int		 mMax;
};

struct ET_ExplosiveState
{
	GameEntity	 mExplosive;
	ExplosiveState mState;
};

struct ET_ConstructionState
{
	GameEntity		 mConstructable;
	ConstructableState mState;
};

struct ET_Destroyable
{
	GameEntity		 mEntity;
	ConstructableState mState;
};

struct ET_HasFlag
{
	obBool	 mHasFlag;
};

struct ET_CanBeGrabbed
{
	GameEntity mEntity;
	obBool	 mCanBeGrabbed;
};

struct ET_TeamMines
{
	int		 mCurrent;
	int		 mMax;
};

struct ET_WaitingForMedic
{
	obBool	 mWaitingForMedic;
};

struct ET_SelectWeapon
{
	ET_Weapon mSelection;
	obBool	 mGood;
};

struct ET_ReinforceTime
{
	int		 mReinforceTime;
};

struct ET_MedicNear
{
	obBool	 mMedicNear;
};

struct ET_GoLimbo
{
	obBool	 mGoLimbo;
};

struct ET_MG42MountedPlayer
{
	GameEntity mMG42Entity;
	GameEntity mMountedEntity;
};

struct ET_MG42MountedRepairable
{
	GameEntity mMG42Entity;
	obBool	 mRepairable;
};

struct ET_MG42Health
{
	GameEntity mMG42Entity;
	int		 mHealth;
};

struct ET_CursorHint
{
	int		 mType;
	int		 mValue;
};

struct ET_SpawnPoint
{
	int		 mSpawnPoint;
};

struct ET_MG42Info
{
	float	 mCenterFacing[ 3 ];
	float	 mMinHorizontalArc, mMaxHorizontalArc;
	float	 mMinVerticalArc, mMaxVerticalArc;
};

struct ET_CabinetData
{
	int		 mCurrentAmount;
	int		 mMaxAmount;
	int		 mRate;
};

struct ET_PlayerSkills
{
	int	 mSkill[ ET_SKILLS_NUM_SKILLS ];
};

struct ET_FireTeamApply
{
	int	 mFireTeamNum;
};

//struct ET_FireTeamJoin
//{
//	int	 mFireTeamNum;
//};

struct ET_FireTeam
{
	GameEntity mTarget;
};

struct ET_FireTeamInfo
{
	enum
	{
		MaxMembers = 64
	};
	obBool	 mInFireTeam;
	GameEntity mLeader;
	GameEntity mMembers[ MaxMembers ];
	int		 mFireTeamNum;

	ET_FireTeamInfo()
		: mInFireTeam( False )
		, mFireTeamNum( 0 )
	{
	}
};

//////////////////////////////////////////////////////////////////////////

struct Event_MortarImpact_ET
{
	float mPosition[ 3 ];
};

struct Event_TriggerMine_ET
{
	GameEntity mMineEntity;
};

struct Event_FireTeamCreated
{
	int		 mFireTeamNum;
};

struct Event_FireTeamDisbanded
{
	int		 mFireTeamNum;
};

struct Event_FireTeamJoined
{
	GameEntity mTeamLeader;
	int		 mFireTeamNum;
};

struct Event_FireTeamLeft
{
	int		 mFireTeamNum;
};

struct Event_FireTeamInvited
{
	GameEntity mTeamLeader;
	int		 mFireTeamNum;
};

struct Event_FireTeamProposal
{
	GameEntity mInvitee;
	int		 mFireTeamNum;
};

struct Event_FireTeamWarning
{
	GameEntity mWarnedBy;
	int		 mFireTeamNum;
};

struct Event_Ammo
{
	GameEntity mWhoDoneIt;
};

struct ET_DisableBotPush
{
	int		 mPush;
};

#pragma pack(pop)

#endif
