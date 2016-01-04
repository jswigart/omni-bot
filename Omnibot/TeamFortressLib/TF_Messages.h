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

// enum:  TF_MessageId
enum TF_MessageId
{
	TF_MSG_BEGIN = MSG_BASE_LAST,

	// GENERAL EVENTS
	TF_MSG_CLASS_DISABLED, // TODO: IMPLEMENT THIS
	TF_MSG_CLASS_NOTAVAILABLE, // TODO: IMPLEMENT THIS
	TF_MSG_CLASS_CHANGELATER, // TODO: IMPLEMENT THIS
	TF_MSG_BUILD_MUSTBEONGROUND,
	TF_MSG_INFECTED,
	TF_MSG_CURED,
	TF_MSG_BURNLEVEL,

	TF_MSG_GOT_ENGY_ARMOR,
	TF_MSG_GAVE_ENGY_ARMOR,
	TF_MSG_GOT_MEDIC_HEALTH,
	TF_MSG_GAVE_MEDIC_HEALTH,

	TF_MSG_GOT_DISPENSER_AMMO,

	// SCOUT
	// GAME EVENTS
	TF_MSG_RADAR_DETECT_ENEMY,
	// INTERNAL EVENTS.

	// SNIPER
	// GAME EVENTS
	TF_MSG_RADIOTAG_UPDATE,
	// INTERNAL EVENTS

	// SOLDIER
	// GAME EVENTS
	// INTERNAL EVENTS

	// DEMO-MAN
	// GAME EVENTS	
	TF_MSG_PIPE_DETONATED,
	// INTERNAL EVENTS
	TF_MSG_PIPE_PROXIMITY,
	TF_MSG_DETPIPES,		// THE BOT HAS DETECTED THE DESIRE TO DET PIPES.
	TF_MSG_DETPIPESNOW,		// CONFIGURABLE DELAYED MESSAGE FOR THE ACTUAL DETTING.

	// MEDIC
	// GAME EVENTS
	TF_MSG_CALLEDFORMEDIC,
	// INTERNAL EVENTS

	// HW-GUY
	// GAME EVENTS
	// INTERNAL EVENTS

	// PYRO
	// GAME EVENTS
	// INTERNAL EVENTS

	// SPY
	// GAME EVENTS
	TF_MSG_DISGUISING,
	TF_MSG_DISGUISED,
	TF_MSG_DISGUISE_LOST,
	TF_MSG_CANT_CLOAK,
	TF_MSG_CLOAKED,
	TF_MSG_UNCLOAKED,
	TF_MSG_SABOTAGED_SENTRY,
	TF_MSG_SABOTAGED_DISPENSER,
	TF_MSG_CANTDISGUISE_AS_TEAM,
	TF_MSG_CANTDISGUISE_AS_CLASS,
	// INTERNAL EVENTS

	// ENGINEER
	// GAME EVENTS
	TF_MSG_CALLEDFORENGINEER,

	TF_MSG_BUILD_NOTENOUGHAMMO,
	TF_MSG_BUILD_ALREADYBUILT,
	TF_MSG_BUILD_CANTBUILDHERE,
	TF_MSG_BUILD_BUILDING,
	TF_MSG_BUILD_BUILT,
	TF_MSG_BUILD_BUILDCANCEL,
	TF_MSG_BUILD_DESTROYED,
	TF_MSG_BUILD_SPOTENEMY,
	TF_MSG_BUILD_AIMED,
	TF_MSG_BUILD_DAMAGED,
	TF_MSG_BUILD_UPGRADED,
	TF_MSG_BUILD_DETONATED,
	TF_MSG_BUILD_DISMANTLED,
	TF_MSG_BUILD_SABOTAGED,
	TF_MSG_BUILD_ENEMYUSED,
	TF_MSG_BUILD_DETONATE,
		
	// THIS MUST STAY LAST
	TF_MSG_END_EVENTS
};

#pragma pack(push)
#pragma pack(4)

struct EvEntityClassDisabled
{
	typedef MessageT<EvEntityClassDisabled, TF_MSG_CLASS_DISABLED> Msg;

	int			mClassId;
};

struct EvEntityClassNotAvailable
{
	typedef MessageT<EvEntityClassNotAvailable, TF_MSG_CLASS_NOTAVAILABLE> Msg;

	int			mClassId;
};

struct EvEntityClassChangeLater
{
	typedef MessageT<EvEntityClassChangeLater, TF_MSG_CLASS_CHANGELATER> Msg;

	int			mClassId;
};

struct EvEntityMustBeOnGround
{
	typedef MessageT<EvEntityClassChangeLater, TF_MSG_BUILD_MUSTBEONGROUND> Msg;
};

struct EvEntityInfected
{
	typedef MessageT<EvEntityInfected, TF_MSG_INFECTED> Msg;

	GameEntity		mInfectedBy;
};

struct EvEntityCured
{
	typedef MessageT<EvEntityCured, TF_MSG_CURED> Msg;

	GameEntity		mCuredBy;
};

struct EvEntityBurnLevel
{
	typedef MessageT<EvEntityBurnLevel, TF_MSG_BURNLEVEL> Msg;

	GameEntity		mBurnedBy;
	int				mBurnLevel;
};

struct EvEntityGotArmorFromEngineer
{
	typedef MessageT<EvEntityGotArmorFromEngineer, TF_MSG_GOT_ENGY_ARMOR> Msg;

	GameEntity		mEngineer;
	float			mValueBefore;
	float			mValueAfter;
};

struct EvEntityGaveArmorAsEngineer
{
	typedef MessageT<EvEntityGaveArmorAsEngineer, TF_MSG_GAVE_ENGY_ARMOR> Msg;

	GameEntity		mRecipient;
	float			mValueBefore;
	float			mValueAfter;
};

struct EvEntityGotHealthFromMedic
{
	typedef MessageT<EvEntityGotHealthFromMedic, TF_MSG_GOT_MEDIC_HEALTH> Msg;

	GameEntity		mMedic;
	float			mValueBefore;
	float			mValueAfter;
};

struct EvEntityGaveHealthAsMedic
{
	typedef MessageT<EvEntityGaveHealthAsMedic, TF_MSG_GAVE_MEDIC_HEALTH> Msg;

	GameEntity		mRecipient;
	float			mValueBefore;
	float			mValueAfter;
};

struct EvEntityGotAmmoFromDispenser
{
	typedef MessageT<EvEntityGotAmmoFromDispenser, TF_MSG_GOT_DISPENSER_AMMO> Msg;

	GameEntity		mDispenser;
	GameEntity		mDispenserOwner;
};

struct EvEntityRadarDetectEntity
{
	typedef MessageT<EvEntityRadarDetectEntity, TF_MSG_RADAR_DETECT_ENEMY> Msg;

	GameEntity		mDetected;
};

struct EvEntityRadioTagUpdate
{
	typedef MessageT<EvEntityRadioTagUpdate, TF_MSG_RADIOTAG_UPDATE> Msg;

	GameEntity		mDetected;
};

struct EvEntityPipeDetonated
{
	typedef MessageT<EvEntityPipeDetonated, TF_MSG_PIPE_DETONATED> Msg;

	GameEntity		mPipe;
};

struct EvEntityPipeProximity
{
	typedef MessageT<EvEntityPipeProximity, TF_MSG_PIPE_PROXIMITY> Msg;

	GameEntity		mPipe;
	GameEntity		mEntity;
};

struct EvEntityPipeDetonate
{
	typedef MessageT<EvEntityPipeDetonate, TF_MSG_DETPIPES> Msg;
};

struct EvEntityPipeDetonateNow
{
	typedef MessageT<EvEntityPipeDetonateNow, TF_MSG_DETPIPESNOW> Msg;
};

struct EvEntityCalledForMedic
{
	typedef MessageT<EvEntityCalledForMedic, TF_MSG_CALLEDFORMEDIC> Msg;

	GameEntity		mEntity;
};

struct EvEntityDisguising
{
	typedef MessageT<EvEntityDisguising, TF_MSG_DISGUISING> Msg;

	int		 mTeamId;
	int		 mClassId;
};

struct EvEntityDisguised
{
	typedef MessageT<EvEntityDisguised, TF_MSG_DISGUISED> Msg;

	int		 mTeamId;
	int		 mClassId;
};

struct EvEntityDisguiseLost
{
	typedef MessageT<EvEntityDisguiseLost, TF_MSG_DISGUISE_LOST> Msg;

	int		 mTeamId;
	int		 mClassId;
};

struct EvEntityCantCloak
{
	typedef MessageT<EvEntityCantCloak, TF_MSG_CANT_CLOAK> Msg;
};

struct EvEntityCloaked
{
	typedef MessageT<EvEntityCloaked, TF_MSG_CLOAKED> Msg;
};

struct EvEntityUnCloaked
{
	typedef MessageT<EvEntityUnCloaked, TF_MSG_UNCLOAKED> Msg;
};

struct EvEntitySabotagedSentry
{
	typedef MessageT<EvEntitySabotagedSentry, TF_MSG_SABOTAGED_SENTRY> Msg;

	GameEntity		mSentry;
};

struct EvEntitySabotagedDispenser
{
	typedef MessageT<EvEntitySabotagedDispenser, TF_MSG_SABOTAGED_DISPENSER> Msg;

	GameEntity		mDispenser;
};

struct EvEntityCantDisguiseAsTeam
{
	typedef MessageT<EvEntityCantDisguiseAsTeam, TF_MSG_CANTDISGUISE_AS_TEAM> Msg;

	int				mTeamId;
};

struct EvEntityCantDisguiseAsClass
{
	typedef MessageT<EvEntityCantDisguiseAsClass, TF_MSG_CANTDISGUISE_AS_CLASS> Msg;

	int				mClassId;
};

struct EvEntityCalledForEngineer
{
	typedef MessageT<EvEntityCalledForEngineer, TF_MSG_CALLEDFORENGINEER> Msg;

	GameEntity		mEntity;
};

struct EvEntityBuildNotEnoughAmmo
{
	typedef MessageT<EvEntityBuildNotEnoughAmmo, TF_MSG_BUILD_NOTENOUGHAMMO> Msg;

	int		mClassId;
};

struct EvEntityAlreadyBuilt
{
	typedef MessageT<EvEntityAlreadyBuilt, TF_MSG_BUILD_ALREADYBUILT> Msg;

	int				mClassId;
	GameEntity		mExisting;
};

struct EvEntityCantBuild
{
	typedef MessageT<EvEntityCantBuild, TF_MSG_BUILD_CANTBUILDHERE> Msg;

	int				mClassId;
};

struct EvEntityBuilding
{
	typedef MessageT<EvEntityBuilding, TF_MSG_BUILD_BUILDING> Msg;

	int				mClassId;
	GameEntity		mEntity;
};

struct EvEntityBuilt
{
	typedef MessageT<EvEntityBuilt, TF_MSG_BUILD_BUILT> Msg;

	int				mClassId;
	GameEntity		mEntity;
};

struct EvEntityBuildCancel
{
	typedef MessageT<EvEntityBuildCancel, TF_MSG_BUILD_BUILDCANCEL> Msg;

	int				mClassId;
	GameEntity		mEntity;
};

struct EvEntityBuildDestroyed
{
	typedef MessageT<EvEntityBuildDestroyed, TF_MSG_BUILD_DESTROYED> Msg;

	int				mClassId;
	GameEntity		mEntity;
};

struct EvEntityBuildSpotEnemy
{
	typedef MessageT<EvEntityBuildSpotEnemy, TF_MSG_BUILD_SPOTENEMY> Msg;

	int				mClassId;
	GameEntity		mEntity;
	GameEntity		mEnemy;
};

struct EvEntityBuildAimed
{
	typedef MessageT<EvEntityBuildAimed, TF_MSG_BUILD_AIMED> Msg;

	int				mClassId;
	GameEntity		mEntity;
	float			mDirection[ 3 ];
};

struct EvEntityBuildDamaged
{
	typedef MessageT<EvEntityBuildDamaged, TF_MSG_BUILD_DAMAGED> Msg;

	int				mClassId;
	GameEntity		mEntity;
	GameEntity		mInflictor;
};

struct EvEntityBuildUpgraded
{
	typedef MessageT<EvEntityBuildUpgraded, TF_MSG_BUILD_UPGRADED> Msg;

	int				mClassId;
	GameEntity		mEntity;
	int				mLevel;
};

struct EvEntityBuildDetonated
{
	typedef MessageT<EvEntityBuildDetonated, TF_MSG_BUILD_DETONATED> Msg;

	int				mClassId;
	GameEntity		mEntity;
};

struct EvEntityBuildDismantled
{
	typedef MessageT<EvEntityBuildDismantled, TF_MSG_BUILD_DISMANTLED> Msg;

	int				mClassId;
	GameEntity		mEntity;
};

struct EvEntityBuildSabotaged
{
	typedef MessageT<EvEntityBuildSabotaged, TF_MSG_BUILD_SABOTAGED> Msg;

	int				mClassId;
	GameEntity		mEntity;
	GameEntity		mByWhom;
};

struct EvEntityBuildEnemyUsed
{
	typedef MessageT<EvEntityBuildEnemyUsed, TF_MSG_BUILD_ENEMYUSED> Msg;

	int				mClassId;
	GameEntity		mEntity;
	GameEntity		mByWhom;
};

struct EvEntityBuildDetonateIt
{
	typedef MessageT<EvEntityBuildDetonateIt, TF_MSG_BUILD_DETONATE> Msg;

	int				mClassId;
	GameEntity		mEntity;
	GameEntity		mByWhom;
};

//////////////////////////////////////////////////////////////////////////

#pragma pack(pop)

#endif
