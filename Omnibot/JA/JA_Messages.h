////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
// Title: JA Message Structure Definitions
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __JA_MESSAGES_H__
#define __JA_MESSAGES_H__

#include "Base_Messages.h"
#include "JA_Config.h"

#pragma pack(push)
#pragma pack(4)

//////////////////////////////////////////////////////////////////////////

struct JA_HasFlag
{
	obBool	 mHasFlag;
};

struct JA_CanBeGrabbed
{
	GameEntity mEntity;
	obBool	 mCanBeGrabbed;
};

struct JA_TeamMines
{
	int		 mCurrent;
	int		 mMax;
};

struct JA_TeamDetpacks
{
	int		 mCurrent;
	int		 mMax;
};

struct JA_SelectWeapon
{
	JA_Weapon mSelection;
	obBool	 mGood;
};

/*struct JA_ReinforceTime
{
int		 mReinforceTime;
};*/

typedef struct
{
	int		 mForce;
	int		 mKnownBits;
	int		 mActiveBits;
	int		 mLevels[JA_MAX_FORCE_POWERS];
} JA_ForceData;

struct JA_Mindtricked
{
	GameEntity mEntity;
	obBool	 mIsMindtricked;
};

//////////////////////////////////////////////////////////////////////////
// Events

struct Event_SystemGametype
{
	int	 mGametype;
};

struct Event_ForceInflicted
{
	GameEntity mInflictor;
	int		 mType;
};

#pragma pack(pop)

#endif
