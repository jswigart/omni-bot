////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
// Title: RTCW Config
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __RTCW_CONFIG_H__
#define __RTCW_CONFIG_H__

#include "Omni-Bot_Events.h"

enum RTCW_Version
{
	RTCW_VERSION_0_7_beta = 1,
	RTCW_VERSION_0_7,
	RTCW_VERSION_0_71,
	RTCW_VERSION_0_8,
	RTCW_VERSION_0_83,
	RTCW_VERSION_LAST,
	RTCW_VERSION_LATEST = RTCW_VERSION_LAST - 1
};

enum RTCW_Weapon
{
	RTCW_WP_UNKNOWN = INVALID_WEAPON,
	RTCW_WP_NONE = INVALID_WEAPON,

	RTCW_WP_KNIFE,				// 1
	RTCW_WP_LUGER,				// 2
	RTCW_WP_MP40,				// 3
	RTCW_WP_MAUSER,				// 4
	RTCW_WP_GREN_AXIS,			// 5	WP_GRENADE_LAUNCHER
	RTCW_WP_PANZERFAUST,		// 6
	RTCW_WP_VENOM,				// 7
	RTCW_WP_FLAMETHROWER,		// 8
	RTCW_WP_COLT,				// 9	equivalent american weapon to german luger
	RTCW_WP_THOMPSON,			// 10	equivalent american weapon to german mp40
	RTCW_WP_GARAND,				// 11	equivalent american weapon to german mauser
	RTCW_WP_GREN_ALLIES,		// 12	WP_GRENADE_PINEAPPLE
	RTCW_WP_ROCKET_LAUNCHER,	// 13	equivalent american weapon to german panzerfaust
	RTCW_WP_SNIPERRIFLE,		// 14
	RTCW_WP_STEN,				// 15
	RTCW_WP_SYRINGE,			// 16	WP_MEDIC_SYRINGE
	RTCW_WP_AMMO_PACK,			// 17	WP_AMMO
	RTCW_WP_ARTY,				// 18
	RTCW_WP_DYNAMITE,			// 19
	RTCW_WP_SNIPER,				// 20
	RTCW_WP_MEDKIT,				// 21
	RTCW_WP_PLIERS,				// 22
	RTCW_WP_SMOKE_GRENADE,		// 23
	RTCW_WP_BINOCULARS,			// 24
	RTCW_WP_MOUNTABLE_MG42,		// 25
	RTCW_WP_FG42,				// 26
	RTCW_WP_FG42SCOPE,			// 27
	RTCW_WP_SHOTGUN,			// 28

	RTCW_WP_MAX
};

enum RTCW_PlayerClass
{
	RTCW_CLASS_UNKNOWN = 0,
	RTCW_CLASS_NULL = 0,
	RTCW_CLASS_SOLDIER,
	RTCW_CLASS_MEDIC,
	RTCW_CLASS_ENGINEER,
	RTCW_CLASS_LIEUTENANT,
	RTCW_CLASS_MAX,
	RTCW_CLASS_ANY = RTCW_CLASS_MAX,

	// Other values to identify the "class"
	RTCW_CLASSEX_MG42MOUNT,
	RTCW_CLASSEX_DYNAMITE,
	RTCW_CLASSEX_VEHICLE,
	RTCW_CLASSEX_BREAKABLE,
	RTCW_CLASSEX_INJUREDPLAYER,
	RTCW_CLASSEX_CORPSE,
	RTCW_CLASSEX_TREASURE,
	RTCW_CLASSEX_GRENADE,
	RTCW_CLASSEX_ROCKET,
	RTCW_CLASSEX_ARTY,
	RTCW_CLASSEX_AIRSTRIKE,
	RTCW_CLASSEX_FLAMECHUNK,
	RTCW_CLASSEX_BROKENCHAIR,
	RTCW_CLASSEX_MINE,

	RTCW_CLASSEX_WEAPON,
	RTCW_CLASSEX_WEAPON_LAST = RTCW_CLASSEX_WEAPON+RTCW_WP_MAX,

	RTCW_NUM_CLASSES
};

// typedef: RTCW_Team
//		The available teams for this gametype.
enum RTCW_Team
{
	RTCW_TEAM_NONE = OB_TEAM_NONE,
	RTCW_TEAM_AXIS,
	RTCW_TEAM_ALLIES,
	RTCW_TEAM_MAX
};

enum RTCW_EntityFlags
{
	// bit: RTCW_ENT_FLAG_MNT_MG42
	//		Currently mounted on an MG42
	RTCW_ENT_FLAG_MNT_MG42 = ENT_FLAG_FIRST_USER,
	// bit: RTCW_ENT_FLAG_ISMOUNTABLE
	//		This entity is mountable.
	RTCW_ENT_FLAG_ISMOUNTABLE,
	// bit: RTCW_ENT_FLAG_CARRYINGGOAL
	//		Currently carrying a goal
	RTCW_ENT_FLAG_CARRYINGGOAL,
	// bit: RTCW_ENT_FLAG_INLIMBO
	//		This entity is currently in limbo mode. Players only.
	RTCW_ENT_FLAG_INLIMBO,
	// bit: RTCW_ENT_FLAG_DISGUISED
	//		This entity is disguised
	RTCW_ENT_FLAG_DISGUISED,
	// bit: RTCW_ENT_FLAG_POISONED
	//		This entity is poisoned.
	RTCW_ENT_FLAG_POISONED,
	// bit: RTCW_ENT_FLAG_MOUNTED
	//		This entity is mounted
	RTCW_ENT_FLAG_MOUNTED,
	// bit: RTCW_ENT_FLAG_INJURED
	//		This entity is injured
	RTCW_ENT_FLAG_INJURED,
};

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

enum RTCW_Powerups
{
	RTCW_PWR_INVULNERABLE = PWR_FIRST_USER,
	RTCW_PWR_FIRE,
	RTCW_PWR_ELECTRIC,
	RTCW_PWR_BREATHER,
	RTCW_PWR_NOFATIGUE,
	RTCW_PWR_REDFLAG,
	RTCW_PWR_BLUEFLAG,
	RTCW_PWR_BALL,
};

struct ParamsMG42Info
{
	float	 mCenterFacing[ 3 ];
	float	 mMinHorizontalArc, mMaxHorizontalArc;
	float	 mMinVerticalArc, mMaxVerticalArc;
};

class RTCW_Interface : public IEngineInterface
{
public:
	virtual bool SendPrivateMessage( GameEntity ent, const char* toName, const char * message ) = 0;
	virtual bool IsWeaponOverheated( GameEntity ent, RTCW_Weapon weapon ) = 0;
	virtual void GetMountedGunHeat( GameEntity ent, const GameEntity gun, int &cur, int &max ) = 0;
	virtual ExplosiveState GetExplosiveState( GameEntity ent, const GameEntity dynamite ) = 0;
	virtual ConstructableState IsDestroyable( GameEntity ent, const GameEntity other ) = 0;
	virtual bool HasFlag( GameEntity ent ) = 0;
	virtual bool ItemCanBeGrabbed( GameEntity ent, const GameEntity item ) = 0;
	virtual void NumTeamMines( GameEntity ent, int &current, int &max ) = 0;
	virtual bool SelectPrimaryWeapon( GameEntity ent, RTCW_Weapon weapon ) = 0;
	virtual bool SelectSecondaryWeapon( GameEntity ent, RTCW_Weapon weapon ) = 0;
	virtual float GetReinforceTime( GameEntity ent ) = 0;
	virtual bool IsMedicNear( GameEntity ent ) = 0;
	virtual bool GoToLimbo( GameEntity ent ) = 0;
	virtual GameEntity GetMountedPlayerOnMG42( GameEntity ent, const GameEntity gun ) = 0;
	virtual bool IsMountableGunRepairable( GameEntity ent, GameEntity gun ) = 0;
	virtual int GetGunHealth( GameEntity ent, const GameEntity gun ) = 0;
	virtual void GetCurrentCursorHint( GameEntity ent, int &type, int &val ) = 0;
	virtual bool CanSnipe( GameEntity ent ) = 0;
	virtual void ChangeSpawnPoint( GameEntity ent, int _spawnpoint ) = 0;
	virtual bool GetMg42Properties( GameEntity ent, ParamsMG42Info &data ) = 0;
	virtual int GetSpawnPoint( GameEntity ent ) = 0;
	virtual void SetSuicide( GameEntity ent, int sui, int pers ) = 0;
	virtual void DisableBotPush( GameEntity ent, int push ) = 0;
	virtual int GetPlayerClass( GameEntity ent ) = 0;
};

extern RTCW_Interface* gRTCWFuncs;

#endif
