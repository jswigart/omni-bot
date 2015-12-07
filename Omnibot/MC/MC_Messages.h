////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
// Title: MC Message Structure Definitions
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __MC_MESSAGES_H__
#define __MC_MESSAGES_H__

#include "Base_Messages.h"
#include "MC_Config.h"

#pragma pack(push)
#pragma pack(4)

//////////////////////////////////////////////////////////////////////////

// struct: Event_PlayerSpree
struct Event_PlayerSpree
{
	static const int EventId = MC_EVENT_PLAYER_SPREE;

	GameEntity mPlayer;
	int		 mKills;
};

// struct: Event_PlayerSpreeEnd
struct Event_PlayerSpreeEnd
{
	static const int EventId = MC_EVENT_PLAYER_SPREE_END;

	GameEntity mPlayer;
	GameEntity mByWho;
	int		 mKills;
};

// struct: Event_SpreeWarStart
struct Event_SpreeWarStart
{
	static const int EventId = MC_EVENT_SPREEWAR_START;

	GameEntity mVictim;
};

struct Event_SpreeWarEnd
{
	static const int EventId = MC_EVENT_SPREEWAR_END;
};

struct Event_LevelUp
{
	static const int EventId = MC_EVENT_LEVEL_UP;
	int mLevel;
};

//////////////////////////////////////////////////////////////////////////

// struct: MC_PlayerStats
struct MC_PlayerStats
{
	enum QuantityType
	{
		Q_MINION,
		Q_LASERS,
		Q_CROWS,
		Q_MAGMINE,
		Q_TURRET,
		Q_MANHACK,

		Q_MAXTYPES = 10 // reserve some space for others
	};

	int			mExperienceTotal;
	int			mExperienceGame;
	int			mModulePoints;
	int			mWeaponPoints;
	int			mCredits;
	int			mLevel;		
	float		mAuxPower;
	float		mAuxPowerMax;
	float		mAuxRegenRate;
	short		mCount[ Q_MAXTYPES ];
	short		mCountMax[ Q_MAXTYPES ];

	MC_PlayerStats()
	{
		for ( int i = 0; i < Q_MAXTYPES; ++i )
			mCount[ i ] = mCountMax[ i ] = 0;
	}
};

// struct: MC_ModuleStats
struct MC_ModuleStats
{
	struct ModuleInfo
	{
		int		mLvl;
		int		mMaxLvl;
		int		mUpgradeCost;
		float	mAuxDrain;
		float	mCooldown;

		ModuleInfo()
			: mLvl( 0 )
			, mMaxLvl( 0 )
			, mUpgradeCost( 0 )
			, mAuxDrain( 0.f )
			, mCooldown( 0.f )
		{
		}
	};
	ModuleInfo mModule[ MC_MODULE_MAX ];
};

// struct: MC_UpgradeModule
struct MC_UpgradeModule
{
	int			mModuleId;
};

// struct: MC_CanPhysPickup
struct MC_CanPhysPickup
{
	GameEntity	mEntity;
	obBool		mCanPickUp;
};

// struct: MC_PhysGunInfo
struct MC_PhysGunInfo
{
	GameEntity	mHeldEntity;
	float		mLaunchSpeed;
};

struct MC_ChargerStatus
{
	float		mCurrentCharge;
	float		mMaxCharge;
};

#pragma pack(pop)

#endif
