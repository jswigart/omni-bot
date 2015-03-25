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

#include "InterfaceFuncs.h"
#include "ETQW_Config.h"

#pragma pack(push)
#pragma pack(4)

//////////////////////////////////////////////////////////////////////////

struct ETQW_WeaponOverheated
{
	ETQW_Weapon mWeapon;
	obBool	 mIsOverheated;
};

struct ETQW_WeaponHeatLevel
{
	GameEntity mEntity;
	int		 mCurrent;
	int		 mMax;
};

struct ETQW_ExplosiveState
{
	GameEntity	 mExplosive;
	ExplosiveState mState;
};

struct ETQW_ConstructionState
{
	GameEntity		 mConstructable;
	ConstructableState mState;
};

struct ETQW_Destroyable
{
	GameEntity		 mEntity;
	ConstructableState mState;
};

struct ETQW_HasFlag
{
	obBool	 mHasFlag;
};

struct ETQW_CanBeGrabbed
{
	GameEntity mEntity;
	obBool	 mCanBeGrabbed;
};

struct ETQW_TeamMines
{
	int		 mCurrent;
	int		 mMax;
};

struct ETQW_WaitingForMedic
{
	obBool	 mWaitingForMedic;
};

struct ETQW_SelectWeapon
{
	ETQW_Weapon mSelection;
	obBool	 mGood;
};

struct ETQW_ReinforceTime
{
	int		 mReinforceTime;
};

struct ETQW_MedicNear
{
	obBool	 mMedicNear;
};

struct ETQW_GoLimbo
{
	obBool	 mGoLimbo;
};

struct ETQW_MG42MountedPlayer
{
	GameEntity mMG42Entity;
	GameEntity mMountedEntity;
};

struct ETQW_MG42MountedRepairable
{
	GameEntity mMG42Entity;
	obBool	 mRepairable;
};

struct ETQW_MG42Health
{
	GameEntity mMG42Entity;
	int		 mHealth;
};

struct ETQW_CursorHint
{
	int		 mType;
	int		 mValue;
};
struct ETQW_SpawnPoint
{
	int		 mSpawnPoint;
};

struct ETQW_MG42Info
{
	float	 mCenterFacing[ 3 ];
	float	 mMinHorizontalArc, mMaxHorizontalArc;
	float	 mMinVerticalArc, mMaxVerticalArc;
};

struct ETQW_CabinetData
{
	int		 mCurrentAmount;
	int		 mMaxAmount;
	int		 mRate;
};

struct ETQW_PlayerSkills
{
	int	 mSkill[ ETQW_SKILLS_NUM_SKILLS ];
};

//////////////////////////////////////////////////////////////////////////

struct Event_MortarImpact_ETQW
{
	float mPosition[ 3 ];
};

struct Event_TriggerMine_ETQW
{
	GameEntity mMineEntity;
};

#pragma pack(pop)

#endif
