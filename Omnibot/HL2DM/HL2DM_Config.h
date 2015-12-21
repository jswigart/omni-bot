////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
// about: HL2DM definitions
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __HL2DM_EVENTS_H__
#define __HL2DM_EVENTS_H__

#include "Omni-Bot_Types.h"
#include "Omni-Bot_Events.h"
#include "IEngineInterface.h"

enum HL2DM_Version
{
	HL2DM_VERSION_LATEST = 1
};

// enumerations: TraceMasks_MC
enum TraceMasks_HL2DM
{
	// combo masks can be defined separately
	HL2DM_TR_MASK_PHYSGUN = TR_MASK_SHOT | TR_MASK_GRATE, // physgun cant pull through grates
};

// typedef: HL2DM_Weapon
//		The available weapons for this gametype
enum HL2DM_Weapon
{
	HL2DM_WP_NONE = INVALID_WEAPON,
	HL2DM_WP_CROWBAR,
	HL2DM_WP_GRAVGUN,
	HL2DM_WP_STUNSTICK,
	HL2DM_WP_PISTOL,
	HL2DM_WP_REVOLVER,
	HL2DM_WP_SMG,
	HL2DM_WP_SHOTGUN,
	HL2DM_WP_CROSSBOW,
	HL2DM_WP_GRENADE,
	HL2DM_WP_RPG,
	HL2DM_WP_SLAM,
	HL2DM_WP_AR2,
	HL2DM_WP_COMBINEGUARD,
	HL2DM_WP_FLAREGUN,
	HL2DM_WP_ANNABELLE,
	HL2DM_WP_BUGBAIT,
	HL2DM_WP_SNIPER,
	HL2DM_WP_MAX
};

// typedef: HL2DM_PlayerClass_enum
//		The available classes for this gametype
enum HL2DM_PlayerClass
{
	HL2DM_CLASS_NULL = ENT_CLASS_NONE,
	HL2DM_CLASS_PLAYER,
	HL2DM_CLASS_MAX,
	HL2DM_CLASS_ANY = HL2DM_CLASS_MAX,

	HL2DM_CLASSEX_ZOMBIE,
	HL2DM_CLASSEX_ZOMBIE_FAST,
	HL2DM_CLASSEX_ANTLION,
	HL2DM_CLASSEX_ANTLION_WORKER,
	HL2DM_CLASSEX_ANTLION_GUARD,
	HL2DM_CLASSEX_HEADCRAB,
	HL2DM_CLASSEX_HEADCRAB_FAST,
	HL2DM_CLASSEX_VORTIGAUNT,
	HL2DM_CLASSEX_HUNTER,
	HL2DM_CLASSEX_MANHACK,
	HL2DM_CLASSEX_CROW,
	HL2DM_CLASSEX_ROLLERMINE,

	HL2DM_CLASSEX_HEALTH_WALLUNIT,
	HL2DM_CLASSEX_ENERGY_WALLUNIT,

	HL2DM_CLASSEX_TRIPMINE,
	HL2DM_CLASSEX_MAGMINE,
	HL2DM_CLASSEX_TURRET,
	HL2DM_CLASSEX_SATCHEL,

	HL2DM_NUM_CLASSES,
};

// typedef: HL2DM_Team
//		The available teams for this gametype
enum HL2DM_Team
{
	HL2DM_TEAM_NONE = OB_TEAM_NONE,
	HL2DM_TEAM_COMBINE = 0,
	HL2DM_TEAM_REBELS,
	HL2DM_TEAM_MAX
};

// enumerations: HL2DM_EntityCategory
enum HL2DM_EntityCategory
{
	HL2DM_ENT_CAT_PHYSPICKUP = ENT_CAT_MAX,
	HL2DM_ENT_CAT_WALLUNIT,

	// THIS MUST BE LAST
	HL2DM_ENT_CAT_MAX,
};

class HL2DM_Interface : public IEngineInterface
{
public:
	// HL2
	virtual bool GetPhysGunInfo( const GameEntity ent, GameEntity& heldEntity, float& launchSpeed ) = 0;
	virtual bool CanPhysPickup( const GameEntity ent, const GameEntity pickup ) = 0;
};

extern HL2DM_Interface* gHL2DMFuncs;

#endif
