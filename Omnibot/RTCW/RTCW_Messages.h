////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
// Title: RTCW Message Structure Definitions
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __RTCW_MESSAGES_H__
#define __RTCW_MESSAGES_H__

#include "Base_Messages.h"
#include "RTCW_Config.h"

#pragma pack(push)
#pragma pack(4)

//////////////////////////////////////////////////////////////////////////

struct RTCW_GetPlayerClass
{
	GameEntity mEntity;
	int		 mPlayerClass;
};

struct RTCW_WeaponOverheated
{
	RTCW_Weapon mWeapon;
	obBool	 mIsOverheated;
};

struct RTCW_WeaponHeatLevel
{
	GameEntity mEntity;
	int		 mCurrent;
	int		 mMax;
};

struct RTCW_ExplosiveState
{
	GameEntity	 mExplosive;
	ExplosiveState mState;
};

struct RTCW_Destroyable
{
	GameEntity		 mEntity;
	ConstructableState mState;
};

struct RTCW_HasFlag
{
	obBool	 mHasFlag;
};

struct RTCW_CanBeGrabbed
{
	GameEntity mEntity;
	obBool	 mCanBeGrabbed;
};

struct RTCW_WaitingForMedic
{
	obBool	 mWaitingForMedic;
};

struct RTCW_SelectWeapon
{
	RTCW_Weapon mSelection;
	obBool	 mGood;
};

struct RTCW_ReinforceTime
{
	int		 mReinforceTime;
};

struct RTCW_MedicNear
{
	obBool	 mMedicNear;
};

struct RTCW_GoLimbo
{
	obBool	 mGoLimbo;
};

struct RTCW_MG42MountedPlayer
{
	GameEntity mMG42Entity;
	GameEntity mMountedEntity;
};

struct RTCW_MG42MountedRepairable
{
	GameEntity mMG42Entity;
	obBool	 mRepairable;
};

struct RTCW_MG42Health
{
	GameEntity mMG42Entity;
	int		 mHealth;
};

struct RTCW_CursorHint
{
	int		 mType;
	int		 mValue;
};

struct RTCW_SpawnPoint
{
	int		 mSpawnPoint;
};

struct RTCW_GetSpawnPoint
{
	int		 mSpawnPoint;
};

struct RTCW_MG42Info
{
	float	 mCenterFacing[ 3 ];
	float	 mMinHorizontalArc, mMaxHorizontalArc;
	float	 mMinVerticalArc, mMaxVerticalArc;
};

struct RTCW_SendPM
{
	char *	 mTargetName;
	char *	 mMessage;
};

struct RTCW_CvarSet
{
	char *	 mCvar;
	char *	 mValue;
};

struct RTCW_CvarGet
{
	char *	 mCvar;
	int		 mValue;
};

struct RTCW_SetSuicide
{
	int		 mSuicide;
	int		 mPersist;
};

struct RTCW_DisableBotPush
{
	int		 mPush;
};

struct Event_Ammo
{
	GameEntity mWhoDoneIt;
};

#pragma pack(pop)

#endif
