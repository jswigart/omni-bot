////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
// about: WOLF definitions
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __MC_EVENTS_H__
#define __MC_EVENTS_H__

#include "Omni-Bot_Types.h"
#include "Omni-Bot_Events.h"
#include "IEngineInterface.h"
#include "MC_Messages.h"

enum MC_Version
{
	MC_VERSION_CURRENT = 1,
	MC_VERSION_ALPHA1 = 5,
	MC_VERSION_207,
	MC_VERSION_LATEST,
};

// enumerations: TraceMasks_MC
enum TraceMasks_MC
{
	// combo masks can be defined separately
	MC_TR_MASK_PHYSGUN		= TR_MASK_SHOT|TR_MASK_GRATE, // physgun cant pull through grates
} ;

// enumerations: TF_ButtonFlags
enum MC_ButtonFlags
{
	MC_BOT_BUTTON_MODULE_FIRST = BOT_BUTTON_FIRSTUSER,

	MC_BOT_BUTTON_MODULE_ATTRITION = MC_BOT_BUTTON_MODULE_FIRST,
	MC_BOT_BUTTON_MODULE_CLOAK,
	MC_BOT_BUTTON_MODULE_DAMAGE_AMPLIFIER,
	MC_BOT_BUTTON_MODULE_ENERGY_BALL,
	MC_BOT_BUTTON_MODULE_FREEZE_GRENADE,
	MC_BOT_BUTTON_MODULE_HEALD,
	MC_BOT_BUTTON_MODULE_INCENDIARY_GRENADE,
	MC_BOT_BUTTON_MODULE_JETPACK,
	MC_BOT_BUTTON_MODULE_LONGJUMP,
	MC_BOT_BUTTON_MODULE_MIRV,
	MC_BOT_BUTTON_MODULE_POISON_SPIT,
	MC_BOT_BUTTON_MODULE_SPAWN_ANTLION,
	MC_BOT_BUTTON_MODULE_SPAWN_ANTLION_WORKER,
	MC_BOT_BUTTON_MODULE_SPAWN_CROW,
	MC_BOT_BUTTON_MODULE_SPAWN_FAST_HEADCRAB,
	MC_BOT_BUTTON_MODULE_SPAWN_FAST_ZOMBIE,
	MC_BOT_BUTTON_MODULE_SPAWN_LASER,
	MC_BOT_BUTTON_MODULE_SPAWN_MAGNETIC_MINE,
	MC_BOT_BUTTON_MODULE_SPAWN_MANHACK,
	MC_BOT_BUTTON_MODULE_SPAWN_TURRET,
	MC_BOT_BUTTON_MODULE_SPAWN_VORTIGAUNT,
	MC_BOT_BUTTON_MODULE_SPAWN_ZOMBIE,
	MC_BOT_BUTTON_MODULE_TELEPORT,
	MC_BOT_BUTTON_MODULE_WEAKEN,

	// must be last active module
	MC_BOT_BUTTON_MODULE_LAST,
};

// enumerations: MCentityCategory
enum MCentityCategory
{
	MC_ENT_CAT_PHYSPICKUP = ENT_CAT_MAX,
	MC_ENT_CAT_WALLUNIT,

	// THIS MUST BE LAST
	MC_ENT_CAT_MAX,
} ;

// typedef: MC_Weapon
//		The available weapons for this gametype
enum MC_Weapon
{
	MC_WP_NONE = INVALID_WEAPON,
	MC_WP_CROWBAR,
	MC_WP_GRAVGUN,
	MC_WP_STUNSTICK,
	MC_WP_PISTOL,
	MC_WP_REVOLVER,
	MC_WP_SMG,
	MC_WP_SMG_GRENADE,
	MC_WP_SHOTGUN,
	MC_WP_CROSSBOW,
	MC_WP_GRENADE,
	MC_WP_RPG,
	MC_WP_SLAM,
	MC_WP_AR2,
	MC_WP_COMBINEGUARD,
	MC_WP_FLAREGUN,
	MC_WP_ANNABELLE,
	MC_WP_BUGBAIT,
	MC_WP_SNIPER,
	MC_WP_MAX
};

// typedef: MC_PlayerClass
//		The available classes for this gametype
enum MC_PlayerClass
{
	MC_CLASS_NULL = ENT_CLASS_NONE,
	MC_CLASS_PLAYER,
	MC_CLASS_MAX,
	MC_CLASS_ANY = MC_CLASS_MAX,

	MC_CLASSEX_ZOMBIE,
	MC_CLASSEX_ZOMBIE_FAST,
	MC_CLASSEX_ANTLION,
	MC_CLASSEX_ANTLION_WORKER,
	MC_CLASSEX_ANTLION_GUARD,
	MC_CLASSEX_HEADCRAB,
	MC_CLASSEX_HEADCRAB_FAST,
	MC_CLASSEX_VORTIGAUNT,
	MC_CLASSEX_HUNTER,
	MC_CLASSEX_MANHACK,
	MC_CLASSEX_CROW,
	MC_CLASSEX_ROLLERMINE,

	MC_CLASSEX_HEALTH_WALLUNIT,
	MC_CLASSEX_ENERGY_WALLUNIT,

	MC_CLASSEX_TRIPMINE,
	MC_CLASSEX_MAGMINE,
	MC_CLASSEX_TURRET,
	MC_CLASSEX_SATCHEL,

	MC_NUM_CLASSES,
};

// typedef: MC_Team
//		The available teams for this gametype
enum MC_Team
{
	MC_TEAM_NONE = OB_TEAM_NONE,
	MC_TEAM_COMBINE,
	MC_TEAM_SCIENCE,
	MC_TEAM_REBELS,
	MC_TEAM_MAX
};

// typedef: MC_Module
//		The available modules
enum MC_Module
{
	// core
	MC_MODULE_RECHARGE,
	MC_MODULE_VITALITY,	
	MC_MODULE_AUX_CAPACITY,
	MC_MODULE_ARMOR_CAPACITY,
	MC_MODULE_ARMOR_REGEN,

	// weapons
	MC_MODULE_IMPACT_STRENGTH,
	MC_MODULE_BRUTE_FORCE,
	MC_MODULE_CRITICAL_HIT,
	MC_MODULE_CLIP_SIZE,
	MC_MODULE_AMMO_REGEN,
		
	// mobility
	MC_MODULE_CLOAK,
	MC_MODULE_JETPACK,
	MC_MODULE_TELEPORT,
	MC_MODULE_LONG_JUMP,
	MC_MODULE_ADRENALINE,

	// projectiles
	MC_MODULE_ENERGY_BALL,
	MC_MODULE_FLECHETTE,
	MC_MODULE_POISON_GRENADE,
	MC_MODULE_FREEZE_GRENADE,
	MC_MODULE_INCEN_GRENADE,
	MC_MODULE_MIRV,

	// target effects
	MC_MODULE_HEALD,
	MC_MODULE_DAMAGE_AMP,	
	MC_MODULE_WEAKEN,
	MC_MODULE_PLAGUE,
	MC_MODULE_SHOCKWAVE,

	// deployables
	MC_MODULE_LASERS,
	MC_MODULE_TURRET,
	MC_MODULE_MAGMINE,
	MC_MODULE_CROW,

	// minions
	MC_MODULE_MINION_ZOMBIE,
	MC_MODULE_MINION_FASTZOMBIE,
	MC_MODULE_MINION_ANTLION,
	MC_MODULE_MINION_ANTLION_WORKER,
	MC_MODULE_MINION_VORTIGAUNT,
	MC_MODULE_MINION_MANHACK,

	MC_MODULE_MAX
};

class ModularCombat_Interface : public IEngineInterface
{
public:
	// HL2
	virtual bool GetPhysGunInfo( const GameEntity ent, GameEntity& heldEntity, float& launchSpeed ) = 0;
	virtual bool CanPhysPickup( const GameEntity ent, const GameEntity pickup ) = 0;

	// Modular Combat Specific
	virtual bool GetPlayerExperience( const GameEntity ent, int& experienceTotal, int& experianceGame ) = 0;
	virtual bool GetPlayerModulePoints( const GameEntity ent, int& modulePoints ) = 0;
	virtual bool GetPlayerLevel( const GameEntity ent, int& level ) = 0;
	virtual bool GetPlayerAux( const GameEntity ent, float& auxCurrent, float& auxMax, float& auxRegenRate ) = 0;
	virtual bool GetPlayerMinionCount( const GameEntity ent, int& current, int& max ) = 0;
	virtual bool GetPlayerLaserCount( const GameEntity ent, int& current, int& max ) = 0;
	virtual bool GetPlayerCrowCount( const GameEntity ent, int& current, int& max ) = 0;
	virtual bool GetPlayerMagmineCount( const GameEntity ent, int& current, int& max ) = 0;
	virtual bool GetPlayerTurretCount( const GameEntity ent, int& current, int& max ) = 0;
	virtual bool GetPlayerManhackCount( const GameEntity ent, int& current, int& max ) = 0;

	virtual bool ModuleStats( const GameEntity ent, MC_Module module, int& lvl, int& lvlMax, int& upgCost, float& auxDrain, float& cooldown ) = 0;
	virtual bool ModuleUpgrade( const GameEntity ent, MC_Module moduleId ) = 0;
};

extern ModularCombat_Interface* gModularCombatFuncs;

#endif
