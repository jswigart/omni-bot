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

#include "Omni-Bot_Color.h"
#include "Base_Messages.h"
#include "TF_Config.h"

#pragma pack(push)
#pragma pack(4)

//////////////////////////////////////////////////////////////////////////

// struct: TF_PlayerPipeCount
//	 mNumPipes - Current number of player pipes.
//	 mMaxPipes - Max player pipes.
struct TF_PlayerPipeCount
{
	int32_t	 mNumPipes;
	int32_t	 mMaxPipes;
};

// struct: TF_TeamPipeInfo
//	 mNumTeamPipes - Current number of team pipes.
//	 mNumTeamPipers - Current number of team pipers(demo-men).
//	 mMaxPipesPerPiper - Max pipes per piper
struct TF_TeamPipeInfo
{
	int32_t	 mNumTeamPipes;
	int32_t	 mNumTeamPipers;
	int32_t	 mMaxPipesPerPiper;
};

// struct: TF_DisguiseOptions
//	 mCheckTeam - team to check class disguises with.
//	 mTeam - true/false if each team is available to be disguised as.
//	 mDisguiseClass - true/false if each class is available to be disguised as.
struct TF_DisguiseOptions
{
	int		 mCheckTeam;
	obBool	 mTeam[TF_TEAM_MAX];
	obBool	 mClass[TF_CLASS_MAX];
};

// struct: TF_Disguise
//	 mDisguiseTeam - Team disguised as.
//	 mDisguiseClass - Class disguised as.
struct TF_Disguise
{
	int32_t	 mDisguiseTeam;
	int32_t	 mDisguiseClass;
};

// struct: TF_FeignDeath
//	 mSilentFeign - Silent feign or not.
struct TF_FeignDeath
{
	obBool	 mSilent;
};

// struct: TF_HudHint
//	 mTargetPlayer - Target player entity for the hint.
//	 mId - Id for the hint.
//	 mMessage[1024] - Hint message.
struct TF_HudHint
{
	GameEntity mTargetPlayer;
	int32_t	 mId;
	char	 mMessage[1024];
};

// struct: TF_HudMenu
//	 mTargetPlayer - Target player entity for the hint.
//	 mMenuType - The type of menu.
//	 mTitle[32] - Title of the menu.
//	 mCaption[32] - Caption of the menu.
//	 mMessage[512] - Message of the menu.
//	 mOption[10][64] - Array of options, max 10.
//	 mCommand[10][64] - Array of commands, max 10.
//	 mLevel - Menu level.
//	 mTimeOut - Duration of the menu.
//	 mColor - Text color.
struct TF_HudMenu
{
	enum GuiType
	{
		GuiAlert,
		GuiMenu,
		GuiTextBox,
	};
	GameEntity mTargetPlayer;
	GuiType	 mMenuType;
	char	 mTitle[32];
	char	 mCaption[32];
	char	 mMessage[512];
	char	 mOption[10][64];
	char	 mCommand[10][64];
	int		 mLevel;
	float	 mTimeOut;
	obColor	 mColor;
};

// struct: TF_HudText
//	 mTargetPlayer - Target player entity for the message.
//	 mMessage - Text to display.
struct TF_HudText
{
	enum MsgType
	{
		MsgConsole,
		MsgHudCenter,
	};
	GameEntity mTargetPlayer;
	MsgType	 mMessageType;
	char	 mMessage[512];
};

// struct: TF_LockPosition
//	 mTargetPlayer - Target player entity for the hint.
//	 mLock - Lock the player or not.
//	 mSucceeded - Status result.
struct TF_LockPosition
{
	GameEntity mTargetPlayer;
	obBool	 mLock;
	obBool	 mSucceeded;
};

//////////////////////////////////////////////////////////////////////////

struct Event_RadioTagUpdate_TF
{
	GameEntity mDetected;
};

struct Event_RadarUpdate_TF
{
	GameEntity mDetected;
};

struct Event_CantDisguiseTeamTF
{
	int		 mTeamId;
};

struct Event_CantDisguiseClass_TF
{
	int		 mClassId;
};

struct Event_Disguise_TF
{
	int		 mTeamId;
	int		 mClassId;
};

struct Event_SentryBuilding_TF
{
	GameEntity mSentry;
};

struct Event_SentryBuilt_TF
{
	GameEntity mSentry;
};

struct Event_SentryAimed_TF
{
	GameEntity mSentry;
	float	 mDirection[3];
};

struct Event_SentrySpotEnemy_TF
{
	GameEntity mSpottedEnemy;
};

struct Event_SentryTakeDamage_TF
{
	GameEntity mInflictor;
};

struct Event_SentryUpgraded_TF
{
	int			 mLevel;
};

struct Event_SentryStatus_TF
{
	GameEntity	 mEntity;
	float		 mPosition[3];
	float		 mFacing[3];
	int			 mLevel;
	int			 mHealth;
	int			 mMaxHealth;
	int			 mShells[2];
	int			 mRockets[2];
	obBool		 mSabotaged;
};

struct Event_DispenserStatus_TF
{
	GameEntity	 mEntity;
	float		 mPosition[3];
	float		 mFacing[3];
	int			 mHealth;
	int			 mMaxHealth;
	int			 mCells;
	int			 mNails;
	int			 mRockets;
	int			 mShells;
	int			 mArmor;
	obBool		 mSabotaged;
};

struct Event_TeleporterStatus_TF
{
	GameEntity	 mEntityEntrance;
	GameEntity	 mEntityExit;
	int			 mNumTeleports;
	int			 mTimeToActivation;
	obBool		 mSabotagedEntry;
	obBool		 mSabotagedExit;
};

struct Event_DetpackStatus_TF
{
	GameEntity	 mEntity;
	float		 mTimeLeft;
};

struct Event_TeleporterBuilding_TF
{
	GameEntity mTeleporter;
};

struct Event_TeleporterBuilt_TF
{
	GameEntity mTeleporter;
};

struct Event_DispenserBuilding_TF
{
	GameEntity mDispenser;
};

struct Event_DispenserBuilt_TF
{
	GameEntity mDispenser;
};

struct Event_DispenserEnemyUsed_TF
{
	GameEntity mEnemy;
};

struct Event_DispenserTakeDamage_TF
{
	GameEntity mInflictor;
};

struct Event_DetpackBuilding_TF
{
	GameEntity mDetpack;
};

struct Event_DetpackBuilt_TF
{
	GameEntity mDetpack;
};

struct Event_BuildableDamaged_TF
{
	GameEntity mBuildable;
};

struct Event_BuildableDestroyed_TF
{
	GameEntity mWhoDoneIt;
	GameEntity mWhoAssistedIt;
};

struct Event_BuildableSabotaged_TF
{
	GameEntity mWhoDoneIt;
};

struct TF_BuildInfo
{
	Event_SentryStatus_TF		mSentryStats;
	Event_DispenserStatus_TF	mDispenserStats;
	Event_TeleporterStatus_TF	mTeleporterStats;
	Event_DetpackStatus_TF		mDetpackStats;
};

struct TF_HealTarget
{
	GameEntity				 mTarget;
	float					 mUberLevel;
	bool					 mHealing;
};

struct Event_GotMedicHealth
{
	GameEntity mFromWho;
	int		 mBeforeValue;
	int		 mAfterValue;
};

struct Event_GaveMedicHealth
{
	GameEntity mToWho;
	int		 mBeforeValue;
	int		 mAfterValue;
};

struct Event_GotEngyArmor
{
	GameEntity mFromWho;
	int		 mBeforeValue;
	int		 mAfterValue;
};

struct Event_GaveEngyArmor
{
	GameEntity mToWho;
	int		 mBeforeValue;
	int		 mAfterValue;
};

struct Event_Infected
{
	GameEntity mFromWho;
};

struct Event_Cured
{
	GameEntity mByWho;
};

struct Event_Burn
{
	GameEntity mByWho;
	int		 mBurnLevel;
};

struct Event_MedicCall
{
	GameEntity mByWho;
};

struct Event_EngineerCall
{
	GameEntity mByWho;
};

struct Event_MedicUberCharged
{
	GameEntity mByWho;
};

struct TF_GameMode
{
	int		 mGameMode;
};

struct TF_CvarSet
{
	char *	 mCvar;
	char *	 mValue;
};

struct TF_CvarGet
{
	char *	 mCvar;
	int		 mValue;
};

#pragma pack(pop)

#endif
