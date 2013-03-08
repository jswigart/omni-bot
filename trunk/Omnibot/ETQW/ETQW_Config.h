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

#ifndef __ETQW_CONFIG_H__
#define __ETQW_CONFIG_H__

#include "Omni-Bot_Events.h"

typedef enum eETQW_Version
{
	ETQW_VERSION_0_52_beta1 = 1,
	ETQW_VERSION_0_52_beta2,
	ETQW_VERSION_0_52_beta3,
	ETQW_VERSION_0_52_beta4,
	ETQW_VERSION_0_52_beta5,
	ETQW_VERSION_0_52_beta6,
	ETQW_VERSION_0_52_FINAL = ETQW_VERSION_0_52_beta6,
	ETQW_VERSION_0_53_beta1,
	ETQW_VERSION_0_53_beta2,
	ETQW_VERSION_0_6,
	ETQW_VERSION_0_65,
	ETQW_VERSION_0_7_alpha1,
	ETQW_VERSION_0_7_alpha2,
	ETQW_VERSION_0_7,
	ETQW_VERSION_0_8,
	ETQW_VERSION_LAST,
	ETQW_VERSION_LATEST = ETQW_VERSION_LAST - 1
} ETQW_Version;

typedef enum eETQW_Events
{
	ETQW_EVENT_BEGIN = EVENT_NUM_EVENTS,
	ETQW_EVENT_PRETRIGGER_MINE,
	ETQW_EVENT_POSTTRIGGER_MINE,
	ETQW_EVENT_MORTAR_IMPACT,
	ETQW_EVENT_END
} ETQW_Event;

typedef enum eETQW_Msgs
{
	ETQW_MSG_BEGIN = GEN_MSG_END,

	// actions
	//ETQW_MSG_PLANTDYNAMITE,
	//ETQW_MSG_PLANTMINE,
	ETQW_MSG_GOTOLIMBO,

	// misc query sutff
	ETQW_MSG_ISMEDICNEAR,
	ETQW_MSG_ISWAITINGFORMEDIC,
	ETQW_MSG_REINFORCETIME,

	ETQW_MSG_GETGUNHEALTH,
	ETQW_MSG_GETGUNHEAT,
	ETQW_MSG_ISGUNMOUNTED,
	ETQW_MSG_ISGUNREPAIRABLE,
	ETQW_MSG_MOUNTEDMG42INFO,

	// weapon query stuff
	ETQW_MSG_WPOVERHEATED,
	//ETQW_MSG_WPGRENSPEED,
	ETQW_MSG_PICKWEAPON,
	ETQW_MSG_PICKWEAPON2,
	ETQW_MSG_GETHINT,
	ETQW_MSG_CHANGESPAWNPOINT,

	// goal query stuff
	ETQW_MSG_GHASFLAG,
	ETQW_MSG_GCONSTRUCTABLE,		// check if goal is constructible at the moment
	ETQW_MSG_GDYNDESTROYABLE,		// check if goal can be destroyed by dynamite
	ETQW_MSG_GSATDESTROYABLE,		// check if goal can be destroyed by satchel charge
	ETQW_MSG_GEXPLOSIVESTATE,		// check if state of an explosive
	ETQW_MSG_GCANBEGRABBED,		// check if an entity can be grabbed
	ETQW_MSG_GNUMTEAMMINES,		// check the number of team mines
	ETQW_MSG_CABINETDATA,			// stats from a health/ammo cabinet
	ETQW_MSG_SKILLLEVEL,			// skill level for all the skills of the bot.

	ETQW_MSG_END
} ETQW_Msg;

typedef enum eETQW_PlayerClass
{
	ETQW_CLASS_UNKNOWN = 0,
	ETQW_CLASS_NULL = 0,
	ETQW_CLASS_SOLDIER,
	ETQW_CLASS_MEDIC,
	ETQW_CLASS_ENGINEER,
	ETQW_CLASS_FIELDOPS,
	ETQW_CLASS_COVERTOPS,
	ETQW_CLASS_MAX,
	ETQW_CLASS_ANY = ETQW_CLASS_MAX,

	// Other values to identify the "class"
	ETQW_CLASSEX_MG42MOUNT,
	ETQW_CLASSEX_DYNAMITE,
	ETQW_CLASSEX_MINE,
	ETQW_CLASSEX_SATCHEL,
	ETQW_CLASSEX_SMOKEBOMB,
	ETQW_CLASSEX_SMOKEMARKER,
	ETQW_CLASSEX_VEHICLE,
	ETQW_CLASSEX_VEHICLE_HVY,
	ETQW_CLASSEX_VEHICLE_NODAMAGE,
	ETQW_CLASSEX_BREAKABLE,
	ETQW_CLASSEX_INJUREDPLAYER,
	ETQW_CLASSEX_CORPSE,
	ETQW_CLASSEX_TREASURE,
	ETQW_CLASSEX_GRENADE,
	ETQW_CLASSEX_ROCKET,
	ETQW_CLASSEX_MORTAR,
	ETQW_CLASSEX_ARTY,
	ETQW_CLASSEX_AIRSTRIKE,
	ETQW_CLASSEX_FLAMECHUNK,
	ETQW_CLASSEX_M7_GRENADE,
	ETQW_CLASSEX_GPG40_GRENADE,
	ETQW_CLASSEX_HEALTHCABINET,
	ETQW_CLASSEX_AMMOCABINET,

	ETQW_NUM_CLASSES
} ETQW_PlayerClass;

typedef enum eETQW_Weapons
{
	ETQW_WP_UNKNOWN = INVALID_WEAPON,
	ETQW_WP_NONE = INVALID_WEAPON,

	ETQW_WP_KNIFE,
	ETQW_WP_PISTOL,			// blaster/pistol
	ETQW_WP_SMG,			// assault rifle and lacerator
	ETQW_WP_GRENADE,		// shrap grenade of either team
	ETQW_WP_EMP,			// emp grenade of either team
	ETQW_WP_ROCKET,			// rocket launcher
	ETQW_WP_HEAVY_MG,		// heavy mg/ hyper blaster
	ETQW_WP_HEALTH,			// health packs
	ETQW_WP_NEEDLE,			// defib/stroy tool
	ETQW_WP_BINOCS,			// normal binoculars - NOT deploy tools!
	ETQW_WP_AIRCAN,			// violator/airstrike marker
	ETQW_WP_SHOTGUN,		// shotgun, nailgun
	ETQW_WP_PLIERS,			// tool for fixing stuff
	ETQW_WP_SCOPED_SMG,		// covert's SMG
	ETQW_WP_NADE_SMG,		// eng's SMG
	ETQW_WP_SNIPERRIFLE,	// sniper rifle/railgun
	ETQW_WP_HE_CHARGE,		// he and plasma charge
	ETQW_WP_LANDMINE,		// landmine
	ETQW_WP_HACK_TOOL,		// hack tool for covertops
	ETQW_WP_AMMO_PACK,		// just what it says.
	ETQW_WP_SMOKE_NADE,		// GDF smoke grenades
	ETQW_WP_DEPLOY_TOOL,	// Eng/FOps/Covert deploy tool
	ETQW_WP_SHIELD_GUN,		// Strogg Force Shield weapon
	ETQW_WP_TELEPORTER,		// Strogg Covert teleport weapon
	ETQW_WP_FLYER_HIVE,		// Strogg Covert Flyer Hive
	ETQW_WP_SUPPLY_MARKER,	// GDF Supply Marker
	ETQW_WP_THIRD_EYE,		// GDF Covert 3rd Eye camera.
	ETQW_WP_PARACHUTE,		// GDF and STROGG have this when spawning in.

	ETQW_WP_MAX
} ETQW_Weapon;

// typedef: ETQW_Team
//		The available teams for this gametype.
typedef enum eETQW_Team
{
	ETQW_TEAM_NONE = OB_TEAM_NONE,
	ETQW_TEAM_STROGG,
	ETQW_TEAM_GDF,
	ETQW_TEAM_MAX
} ETQW_Team;

typedef enum eETQW_Skills
{
	ETQW_SKILL_BATTLE_SENSE,
	ETQW_SKILL_ENGINEERING,
	ETQW_SKILL_FIRST_AID,
	ETQW_SKILL_SIGNALS,
	ETQW_SKILL_LIGHT_WEAPONS,
	ETQW_SKILL_HEAVY_WEAPONS,
	ETQW_SKILL_COVERTOPS,

	// THIS MUST STAY LAST
	ETQW_SKILLS_NUM_SKILLS,
} ETQW_Skills;

typedef enum eETQW_EntityFlags
{
	// bit: ETQW_ENT_FLAG_DISGUISED
	//		This entity is disguised
	ETQW_ENT_FLAG_DISGUISED = ENT_FLAG_FIRST_USER,
	// bit: ETQW_ENT_MOUNTED
	//		This entity is mounted on an mg42, tank, or aagun
	ETQW_ENT_FLAG_MOUNTED,
	// bit: ETQW_ENT_FLAG_MNT_MG42
	//		Currently mounted on an mg42
	ETQW_ENT_FLAG_MNT_MG42,
	// bit: ETQW_ENT_FLAG_MNT_TANK
	//		Currently mounted on a tank
	ETQW_ENT_FLAG_MNT_TANK,
	// bit: ETQW_ENT_FLAG_MNT_AAGUN
	//		Currently mounted on an aa gun
	ETQW_ENT_FLAG_MNT_AAGUN,
	// bit: ETQW_ENT_FLAG_CARRYINGGOAL
	//		This entity is carrying a goal of some sort
	ETQW_ENT_FLAG_CARRYINGGOAL,
	// bit: ETQW_ENT_FLAG_INLIMBO
	//		This entity is currently in limbo mode. Players only.
	ETQW_ENT_FLAG_INLIMBO,
	// bit: ETQW_ENT_FLAG_ISMOUNTABLE
	//		This entity is mountable.
	ETQW_ENT_FLAG_ISMOUNTABLE,
	// bit: ETQW_ENT_FLAG_POISONED
	//		This entity is poisoned.
	ETQW_ENT_FLAG_POISONED,
} ETQW_EntityFlags;

//////////////////////////////////////////////////////////////////////////

typedef enum eCursorHintType
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
} CursorHintType;

typedef enum eExplosiveTargetType
{
	XPLO_TYPE_DYNAMITE	= 1<<0,
	XPLO_TYPE_SATCHEL	= 1<<1
} ExplosiveTargetType;

typedef enum eExplosiveState
{
	XPLO_INVALID			= -1,
	XPLO_ARMED				= 0,
	XPLO_UNARMED			= 1,
} ExplosiveState;

typedef enum eConstructableState
{
	CONST_INVALID			= -1,

	CONST_BUILT				= 0,
	CONST_UNBUILT			= 1,

	CONST_NOTDESTROYABLE	= 0,
	CONST_DESTROYABLE		= 1,

	CONST_BROKEN			= 2,
} ConstructableState;

typedef enum eVehicleType
{
	VEH_NONE,

	// strogg
	VEH_ICARUS,
	VEH_HOG,
	VEH_DESECRATOR,
	VEH_GOLIATH_WALKER,
	VEH_HORNET,

	// gdf
	VEH_HUSKY,
	VEH_BADGER,
	VEH_TROJAN,
	VEH_TITAN,
	VEH_ANANSI,
	VEH_BUFFALO,
	VEH_MCP,
	VEH_PLATYPUS,
} VehicleType;

#endif
