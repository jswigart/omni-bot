////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
// Title: ET Config
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __ET_CONFIG_H__
#define __ET_CONFIG_H__

#include "Omni-Bot_Events.h"
#include "Base_Messages.h"

enum ET_Version
{
	ET_VERSION_0_52_beta1 = 1,
	ET_VERSION_0_52_beta2,
	ET_VERSION_0_52_beta3,
	ET_VERSION_0_52_beta4,
	ET_VERSION_0_52_beta5,
	ET_VERSION_0_52_beta6,
	ET_VERSION_0_52_FINAL = ET_VERSION_0_52_beta6,
	ET_VERSION_0_53_beta1,
	ET_VERSION_0_53_beta2,
	ET_VERSION_0_6,
	ET_VERSION_0_65,
	ET_VERSION_0_7_alpha1,
	ET_VERSION_0_7_alpha2,
	ET_VERSION_0_7,
	ET_VERSION_0_71,
	ET_VERSION_0_8_alpha1,
	ET_VERSION_0_8_alpha2,
	ET_VERSION_0_8,
	ET_VERSION_LAST,
	ET_VERSION_LATEST = ET_VERSION_LAST - 1
} ;

enum ET_Weapon
{
	ET_WP_UNKNOWN = INVALID_WEAPON,
	ET_WP_NONE = INVALID_WEAPON,

	// general
	ET_WP_KNIFE,
	ET_WP_BINOCULARS,

	// axis
	ET_WP_GREN_AXIS,	// grenades used by axis
	ET_WP_LUGER,		// axis standard secondary weapon
	ET_WP_AKIMBO_LUGER, // double luger
	ET_WP_MP40,			// axis standard weapon

	// allies
	ET_WP_GREN_ALLIES,	// grenades used by allies
	ET_WP_COLT,			// allies standard secondary weapon
	ET_WP_AKIMBO_COLT,	// double colt
	ET_WP_THOMPSON,		// allies standard weapon

	// soldier
	ET_WP_PANZERFAUST,
	ET_WP_FLAMETHROWER,
	ET_WP_MORTAR,
	ET_WP_MORTAR_SET,
	ET_WP_MOBILE_MG42,
	ET_WP_MOBILE_MG42_SET,

	// medic
	ET_WP_SYRINGE,
	ET_WP_MEDKIT,
	ET_WP_ADRENALINE,

	// covertops
	ET_WP_STEN,
	ET_WP_FG42,
	ET_WP_FG42_SCOPE,
	ET_WP_SATCHEL,
	ET_WP_SATCHEL_DET,	// remote control for satchel
	ET_WP_SMOKE_GRENADE,
	// covertops (axis)
	ET_WP_SILENCED_LUGER,
	ET_WP_AKIMBO_SILENCED_LUGER,
	ET_WP_K43,
	ET_WP_K43_SCOPE,
	// covertops (allies)
	ET_WP_SILENCED_COLT,
	ET_WP_AKIMBO_SILENCED_COLT,
	ET_WP_GARAND,
	ET_WP_GARAND_SCOPE,

	// fieldops
	ET_WP_AMMO_PACK,
	ET_WP_SMOKE_MARKER,

	// engineer
	ET_WP_DYNAMITE,
	ET_WP_PLIERS,
	ET_WP_LANDMINE,
	// engineer (axis)
	ET_WP_KAR98,
	ET_WP_GPG40,		// grenade launcher
	// engineer (allies)
	ET_WP_CARBINE,
	ET_WP_M7,			// grenade launcher

	ET_WP_MOUNTABLE_MG42,

	ET_WP_MAX = ET_WP_NONE+128
};

enum ET_PlayerClass
{
	ET_CLASS_UNKNOWN = 0,
	ET_CLASS_NULL = 0,
	ET_CLASS_SOLDIER,
	ET_CLASS_MEDIC,
	ET_CLASS_ENGINEER,
	ET_CLASS_FIELDOPS,
	ET_CLASS_COVERTOPS,
	ET_CLASS_MAX,
	ET_CLASS_ANY = ET_CLASS_MAX,

	// Other values to identify the "class"
	ET_CLASSEX_MG42BASE,
	ET_CLASSEX_MG42MOUNT,
	ET_CLASSEX_DYNAMITE,
	ET_CLASSEX_MINE,
	ET_CLASSEX_SATCHEL,
	ET_CLASSEX_SMOKEBOMB,
	ET_CLASSEX_SMOKEMARKER,
	ET_CLASSEX_VEHICLE,
	ET_CLASSEX_VEHICLE_HVY,
	ET_CLASSEX_VEHICLE_NODAMAGE,
	ET_CLASSEX_BREAKABLE,
	//ET_CLASSEX_INJUREDPLAYER,
	ET_CLASSEX_CORPSE,
	ET_CLASSEX_TREASURE,
	ET_CLASSEX_GRENADE,
	ET_CLASSEX_ROCKET,
	ET_CLASSEX_MORTAR,
	ET_CLASSEX_ARTY,
	ET_CLASSEX_AIRSTRIKE,
	ET_CLASSEX_FLAMECHUNK,
	ET_CLASSEX_M7_GRENADE,
	ET_CLASSEX_GPG40_GRENADE,
	ET_CLASSEX_HEALTHCABINET,
	ET_CLASSEX_AMMOCABINET,
	ET_CLASSEX_BROKENCHAIR,
	ET_CLASSEX_CONSTRUCTIBLE,

	ET_NUM_CLASSES
};

// typedef: ET_Team
//		The available teams for this gametype.
enum ET_Team
{
	ET_TEAM_NONE = OB_TEAM_NONE,
	ET_TEAM_AXIS,
	ET_TEAM_ALLIES,
	ET_TEAM_MAX
} ;

enum ET_Skills
{
	ET_SKILL_BATTLE_SENSE,
	ET_SKILL_ENGINEERING,
	ET_SKILL_FIRST_AID,
	ET_SKILL_SIGNALS,
	ET_SKILL_LIGHT_WEAPONS,
	ET_SKILL_HEAVY_WEAPONS,
	ET_SKILL_COVERTOPS,

	// THIS MUST STAY LAST
	ET_SKILLS_NUM_SKILLS,
} ;

enum ET_EntityFlags
{
	// bit: ET_ENT_FLAG_DISGUISED
	//		This entity is disguised
	ET_ENT_FLAG_DISGUISED = ENT_FLAG_FIRST_USER,
	// bit: ET_ENT_MOUNTED
	//		This entity is mounted on an mg42, tank, or aagun
	ET_ENT_FLAG_MOUNTED,
	// bit: ET_ENT_FLAG_MNT_MG42
	//		Currently mounted on an mg42
	ET_ENT_FLAG_MNT_MG42,
	// bit: ET_ENT_FLAG_MNT_TANK
	//		Currently mounted on a tank
	ET_ENT_FLAG_MNT_TANK,
	// bit: ET_ENT_FLAG_MNT_AAGUN
	//		Currently mounted on an aa gun
	ET_ENT_FLAG_MNT_AAGUN,
	// bit: ET_ENT_FLAG_CARRYINGGOAL
	//		This entity is carrying a goal of some sort
	ET_ENT_FLAG_CARRYINGGOAL,
	// bit: ET_ENT_FLAG_INLIMBO
	//		This entity is currently in limbo mode. Players only.
	ET_ENT_FLAG_INLIMBO,
	// bit: ET_ENT_FLAG_ISMOUNTABLE
	//		This entity is mountable.
	ET_ENT_FLAG_ISMOUNTABLE,
	// bit: ET_ENT_FLAG_POISONED
	//		This entity is poisoned.
	ET_ENT_FLAG_POISONED,
} ;

//////////////////////////////////////////////////////////////////////////

enum CursorHintType
{
	CURSOR_HINT_NONE,
	CURSOR_HINT_PLAYER,
	CURSOR_HINT_ACTIVATE,
	CURSOR_HINT_DOOR,
	CURSOR_HINT_DOOR_ROTATING,
	CURSOR_HINT_DOOR_LOCKED,
	CURSOR_HINT_DOOR_ROTATING_LOCKED,
	CURSOR_HINT_MG42,
	CURSOR_HINT_BREAKABLE,
	CURSOR_HINT_BREAKABLE_DYNAMITE,
	CURSOR_HINT_CHAIR,
	CURSOR_HINT_ALARM,
	CURSOR_HINT_HEALTH,
	CURSOR_HINT_TREASURE,
	CURSOR_HINT_KNIFE,
	CURSOR_HINT_LADDER,
	CURSOR_HINT_BUTTON,
	CURSOR_HINT_WATER,
	CURSOR_HINT_CAUTION,
	CURSOR_HINT_DANGER,
	CURSOR_HINT_SECRET,
	CURSOR_HINT_QUESTION,
	CURSOR_HINT_EXCLAMATION,
	CURSOR_HINT_CLIPBOARD,
	CURSOR_HINT_WEAPON,
	CURSOR_HINT_AMMO,
	CURSOR_HINT_ARMOR,
	CURSOR_HINT_POWERUP,
	CURSOR_HINT_HOLDABLE,
	CURSOR_HINT_INVENTORY,
	CURSOR_HINT_SCENARIC,
	CURSOR_HINT_EXIT,
	CURSOR_HINT_NOEXIT,
	CURSOR_HINT_PLYR_FRIEND,
	CURSOR_HINT_PLYR_NEUTRAL,
	CURSOR_HINT_PLYR_ENEMY,
	CURSOR_HINT_PLYR_UNKNOWN,
	CURSOR_HINT_BUILD,
	CURSOR_HINT_DISARM,
	CURSOR_HINT_REVIVE,
	CURSOR_HINT_DYNAMITE,
	CURSOR_HINT_CONSTRUCTIBLE,
	CURSOR_HINT_UNIFORM,
	CURSOR_HINT_LANDMINE,
	CURSOR_HINT_TANK,
	CURSOR_HINT_SATCHELCHARGE,
	CURSOR_HINT_LOCKPICK
};

enum ExplosiveTargetType
{
	XPLO_TYPE_DYNAMITE	= 1<<0,
	XPLO_TYPE_SATCHEL	= 1<<1
};

enum ExplosiveState
{
	XPLO_INVALID			= -1,
	XPLO_ARMED				= 0,
	XPLO_UNARMED			= 1,
};

enum ConstructableState
{
	CONST_INVALID			= -1,

	CONST_BUILT				= 0,
	CONST_UNBUILT			= 1,

	CONST_NOTDESTROYABLE	= 0,
	CONST_DESTROYABLE		= 1,

	CONST_BROKEN			= 2,
};

enum LoadOut
{
	LoadOutPrimary,
	LoadOutSecondary,
};

struct ParamsMG42Info
{
	float	 mCenterFacing[ 3 ];
	float	 mMinHorizontalArc, mMaxHorizontalArc;
	float	 mMinVerticalArc, mMaxVerticalArc;
};

struct ParamsCabinetData
{
	int		 mCurrentAmount;
	int		 mMaxAmount;
	int		 mRate;
};

struct ParamsSkills
{
	int		mSkill[ ET_SKILLS_NUM_SKILLS ];
};

class EnemyTerritory_Interface : public IEngineInterface
{
public:
	virtual ExplosiveState GetExplosiveState( const GameEntity ent ) = 0;
	virtual ConstructableState GetConstructableState( const GameEntity ent, const GameEntity constructable ) = 0;
	virtual ConstructableState IsDestroyable( const GameEntity ent, const GameEntity destroyable ) = 0;
	virtual bool IsCarryingFlag( const GameEntity ent ) = 0;
	virtual bool ItemCanBeGrabbed( const GameEntity ent, const GameEntity item ) = 0;
	virtual bool TeamMineCount( const GameEntity ent, int & current, int & max ) = 0;
	virtual bool IsWaitingForMedic( const GameEntity ent ) = 0;
	virtual bool SelectWeapon( LoadOut load, const GameEntity ent, ET_Weapon weapon ) = 0;
	virtual int GetReinforceTime( const GameEntity ent ) = 0;
	virtual bool IsMedicNear( const GameEntity ent ) = 0;
	virtual bool GoToLimbo( const GameEntity ent ) = 0;
	virtual GameEntity GetMountedPlayerOnMG42( const GameEntity gun ) = 0;
	virtual bool IsMountableGunRepairable( const GameEntity ent, const GameEntity gun ) = 0;
	virtual int GetGunHealth( const GameEntity gun ) = 0;
	virtual bool GetCurrentCursorHint( const GameEntity ent, int & type, int & val ) = 0;
	virtual bool ChangeSpawnPoint( const GameEntity ent, int spawnpoint ) = 0;
	virtual bool GetMg42Properties( const GameEntity ent, ParamsMG42Info &data ) = 0;
	virtual bool GetCabinetData( GameEntity ent, ParamsCabinetData& data ) = 0;
	virtual void DisableBotPush( const GameEntity ent, bool push ) = 0;
	virtual bool GetSkills( const GameEntity ent, ParamsSkills& skills ) = 0;
	
	// Enemy Territory Specifics
	virtual bool FireTeamCreate( const GameEntity ent ) = 0;
	virtual bool FireTeamDisband( const GameEntity ent ) = 0;
	virtual bool FireTeamLeave( const GameEntity ent ) = 0;
	virtual bool FireTeamApply( const GameEntity ent, int fireteamnum ) = 0;
	virtual bool FireTeamInvite( const GameEntity ent, GameEntity target ) = 0;
	virtual bool FireTeamWarn( const GameEntity ent, GameEntity target ) = 0;
	virtual bool FireTeamKick( const GameEntity ent, GameEntity target ) = 0;
	virtual bool FireTeamPropose( const GameEntity ent, GameEntity target ) = 0;
	virtual int FireTeamGetInfo( const GameEntity ent, int& fireTeamNum, GameEntity& leader, GameEntity members[ 64 ] ) = 0;
};

extern EnemyTerritory_Interface* gEnemyTerritoryFuncs;

#endif
