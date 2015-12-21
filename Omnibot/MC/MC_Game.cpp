////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "MC_Game.h"
#include "MC_Client.h"

#include "System.h"
#include "PathPlannerBase.h"
#include "FilterSensory.h"
#include "BotSensoryMemory.h"
#include "ScriptManager.h"

ModularCombat_Interface* gModularCombatFuncs = 0;

IGame *CreateGameInstance()
{
	return new MC_Game;
}

MC_Game::MC_Game()
{
	mGameVars.mClientBase = -1;
	mGameVars.mGameVersion = MC_VERSION_LATEST;
	mGameVars.mGameAbbrev = "mc";
	mGameVars.mGameName = "Modular Combat";
	mGameVars.mPlayerHeight = 72.f;
	mGameVars.mRendersToGame = true;
}

MC_Game::~MC_Game()
{
}

Client * MC_Game::CreateGameClient()
{
	return new MC_Client();
}

bool MC_Game::Init( System & system )
{
	AiState::SensoryMemory::SetEntityTraceOffsetCallback( MC_Game::MC_GetEntityClassTraceOffset );
	AiState::SensoryMemory::SetEntityAimOffsetCallback( MC_Game::MC_GetEntityClassAimOffset );

	if ( !IGame::Init( system ) )
		return false;

	gModularCombatFuncs = dynamic_cast<ModularCombat_Interface*>( gEngineFuncs );

	return true;
}

static const IntEnum MC_TeamEnum [] =
{
	IntEnum( "SPECTATOR", OB_TEAM_SPECTATOR ),
	IntEnum( "COMBINE", MC_TEAM_COMBINE ),
	IntEnum( "SCIENCE", MC_TEAM_SCIENCE ),
	IntEnum( "REBELS", MC_TEAM_REBELS ),
};

void MC_Game::GetTeamEnumeration( const IntEnum *&_ptr, int &num )
{
	num = sizeof( MC_TeamEnum ) / sizeof( MC_TeamEnum[ 0 ] );
	_ptr = MC_TeamEnum;
}

static const IntEnum MC_WeaponEnum [] =
{
	IntEnum( "CROWBAR", MC_WP_CROWBAR ),
	IntEnum( "GRAVGUN", MC_WP_GRAVGUN ),
	IntEnum( "STUNSTICK", MC_WP_STUNSTICK ),
	IntEnum( "PISTOL", MC_WP_PISTOL ),
	IntEnum( "REVOLVER", MC_WP_REVOLVER ),
	IntEnum( "SMG", MC_WP_SMG ),
	IntEnum( "SHOTGUN", MC_WP_SHOTGUN ),
	IntEnum( "CROSSBOW", MC_WP_CROSSBOW ),
	IntEnum( "FRAG_GREN", MC_WP_GRENADE ),
	IntEnum( "RPG", MC_WP_RPG ),
	IntEnum( "SLAM", MC_WP_SLAM ),
	IntEnum( "AR2", MC_WP_AR2 ),
	IntEnum( "COMBINEGUARD", MC_WP_COMBINEGUARD ),
	IntEnum( "FLAREGUN", MC_WP_FLAREGUN ),
	IntEnum( "ANNABELLE", MC_WP_ANNABELLE ),
	IntEnum( "BUGBAIT", MC_WP_BUGBAIT ),
	IntEnum( "SNIPER", MC_WP_SNIPER ),
};

void MC_Game::GetWeaponEnumeration( const IntEnum *&_ptr, int &num )
{
	num = sizeof( MC_WeaponEnum ) / sizeof( MC_WeaponEnum[ 0 ] );
	_ptr = MC_WeaponEnum;
}

void MC_Game::InitScriptCategories( gmMachine *_machine, gmTableObject *_table )
{
	IGame::InitScriptCategories( _machine, _table );

	_table->Set( _machine, "PHYSPICKUP", gmVariable( MC_ENT_CAT_PHYSPICKUP ) );
	_table->Set( _machine, "WALLUNIT", gmVariable( MC_ENT_CAT_WALLUNIT ) );
}

static const IntEnum gClassMapping [] =
{
	IntEnum( "PLAYER", MC_CLASS_PLAYER ),
	IntEnum( "ANYPLAYER", MC_CLASS_ANY ),

	/*IntEnum( "NPC_ZOMBIE", MC_CLASSEX_ZOMBIE ),
	IntEnum( "NPC_ZOMBIE_FAST", MC_CLASSEX_ZOMBIE_FAST ),
	IntEnum( "NPC_ANTLION", MC_CLASSEX_ANTLION ),
	IntEnum( "NPC_ANTLION_WORKER", MC_CLASSEX_ANTLION_WORKER ),
	IntEnum( "NPC_ANTLION_GUARD", MC_CLASSEX_ANTLION_GUARD ),
	IntEnum( "NPC_HEADCRAB", MC_CLASSEX_HEADCRAB ),
	IntEnum( "NPC_HEADCRAB_FAST", MC_CLASSEX_HEADCRAB_FAST ),
	IntEnum( "NPC_VORTIGAUNT", MC_CLASSEX_VORTIGAUNT ),
	IntEnum( "NPC_HUNTER", MC_CLASSEX_HUNTER ),
	IntEnum( "NPC_MANHACK", MC_CLASSEX_MANHACK ),
	IntEnum( "NPC_CROW", MC_CLASSEX_CROW ),
	IntEnum( "NPC_ROLLERMINE", MC_CLASSEX_ROLLERMINE ),

	IntEnum( "PROPBREAKABLE", MC_CLASSEX_PROPBREAKABLE ),
	IntEnum( "PROPEXPLOSIVE", MC_CLASSEX_PROPEXPLOSIVE ),

	IntEnum( "HEALTHKIT", MC_CLASSEX_HEALTHKIT ),
	IntEnum( "HEALTHVIAL", MC_CLASSEX_HEALTHVIAL ),
	IntEnum( "HEALTH_WALLUNIT", MC_CLASSEX_HEALTH_WALLUNIT ),
	IntEnum( "ENERGY_WALLUNIT", MC_CLASSEX_ENERGY_WALLUNIT ),

	IntEnum( "BATTERY", MC_CLASSEX_BATTERY ),
	IntEnum( "POWERCUBE", MC_CLASSEX_POWERCUBE ),
	IntEnum( "ITEMCRATE", MC_CLASSEX_ITEMCRATE ),

	IntEnum( "AMMO_PISTOL", MC_CLASSEX_PISTOL_AMMO ),
	IntEnum( "AMMO_PISTOL_L", MC_CLASSEX_LARGE_PISTOL_AMMO ),
	IntEnum( "AMMO_SMG", MC_CLASSEX_SMG_AMMO ),
	IntEnum( "AMMO_SMG_L", MC_CLASSEX_LARGE_SMG_AMMO ),
	IntEnum( "AMMO_AR2", MC_CLASSEX_AR2_AMMO ),
	IntEnum( "AMMO_AR2_L", MC_CLASSEX_LARGE_AR2_AMMO ),
	IntEnum( "AMMO_357", MC_CLASSEX_357_AMMO ),
	IntEnum( "AMMO_357_L", MC_CLASSEX_LARGE_357_AMMO ),
	IntEnum( "AMMO_CROSSBOW", MC_CLASSEX_CROSSBOW_AMMO ),
	IntEnum( "AMMO_FLARE", MC_CLASSEX_FLARE_AMMO ),
	IntEnum( "AMMO_FLARE_L", MC_CLASSEX_LARGE_FLARE_AMMO ),
	IntEnum( "AMMO_RPG", MC_CLASSEX_RPG_AMMO ),
	IntEnum( "AMMO_AR2GREN", MC_CLASSEX_AR2GREN_AMMO ),
	IntEnum( "AMMO_SNIPER", MC_CLASSEX_SNIPER_AMMO ),
	IntEnum( "AMMO_SHOTGUN", MC_CLASSEX_SHOTGUN_AMMO ),
	IntEnum( "AMMO_AR2_ALTFIRE", MC_CLASSEX_AR2_ALTFIRE_AMMO ),
	IntEnum( "AMMO_CRATE", MC_CLASSEX_AMMO_CRATE ),*/

	IntEnum( "TRIPMINE", MC_CLASSEX_TRIPMINE ),
	IntEnum( "MAGMINE", MC_CLASSEX_MAGMINE ),
	IntEnum( "TURRET", MC_CLASSEX_TURRET ),
};

void MC_Game::InitScriptClasses( gmMachine *_machine, gmTableObject *_table )
{
	IGame::InitScriptClasses( _machine, _table );

	FilterSensory::ANYPLAYERCLASS = MC_CLASS_ANY;

	const int32_t iNumMappings = sizeof( gClassMapping ) / sizeof( gClassMapping[ 0 ] );
	for ( int i = 0; i < iNumMappings; ++i )
	{
		_table->Set( _machine, gClassMapping[ i ].mKey, gmVariable( gClassMapping[ i ].mValue ) );
	}
}

void MC_Game::InitScriptEvents( gmMachine *_machine, gmTableObject *_table )
{
	IGame::InitScriptEvents( _machine, _table );

	_table->Set( _machine, "PLAYER_SPREE", gmVariable( MC_MSG_PLAYER_SPREE ) );
	_table->Set( _machine, "PLAYER_SPREE_END", gmVariable( MC_MSG_PLAYER_SPREE_END ) );
	_table->Set( _machine, "SPREEWAR_START", gmVariable( MC_MSG_SPREEWAR_START ) );
	_table->Set( _machine, "SPREEWAR_END", gmVariable( MC_MSG_SPREEWAR_END ) );
	_table->Set( _machine, "LEVEL_UP", gmVariable( MC_MSG_LEVEL_UP ) );
}

void MC_Game::InitScriptBotButtons( gmMachine *_machine, gmTableObject *_table )
{
	IGame::InitScriptBotButtons( _machine, _table );

	_table->Set( _machine, "ATTRITION", gmVariable( MC_BOT_BUTTON_MODULE_ATTRITION ) );
	_table->Set( _machine, "CLOAK", gmVariable( MC_BOT_BUTTON_MODULE_CLOAK ) );
	_table->Set( _machine, "DAMAGE_AMP", gmVariable( MC_BOT_BUTTON_MODULE_DAMAGE_AMPLIFIER ) );
	_table->Set( _machine, "ENERGY_BALL", gmVariable( MC_BOT_BUTTON_MODULE_ENERGY_BALL ) );
	_table->Set( _machine, "FREEZE_GRENADE", gmVariable( MC_BOT_BUTTON_MODULE_FREEZE_GRENADE ) );
	_table->Set( _machine, "HEALD", gmVariable( MC_BOT_BUTTON_MODULE_HEALD ) );
	_table->Set( _machine, "INCEN_GRENADE", gmVariable( MC_BOT_BUTTON_MODULE_INCENDIARY_GRENADE ) );
	_table->Set( _machine, "JETPACK", gmVariable( MC_BOT_BUTTON_MODULE_JETPACK ) );
	_table->Set( _machine, "LONG_JUMP", gmVariable( MC_BOT_BUTTON_MODULE_LONGJUMP ) );
	_table->Set( _machine, "MIRV", gmVariable( MC_BOT_BUTTON_MODULE_MIRV ) );
	_table->Set( _machine, "POISON_GRENADE", gmVariable( MC_BOT_BUTTON_MODULE_POISON_SPIT ) );

	_table->Set( _machine, "TELEPORT", gmVariable( MC_BOT_BUTTON_MODULE_TELEPORT ) );
	_table->Set( _machine, "WEAKEN", gmVariable( MC_BOT_BUTTON_MODULE_WEAKEN ) );
	_table->Set( _machine, "LASER", gmVariable( MC_BOT_BUTTON_MODULE_SPAWN_LASER ) );
	_table->Set( _machine, "MAGMINE", gmVariable( MC_BOT_BUTTON_MODULE_SPAWN_MAGNETIC_MINE ) );
	_table->Set( _machine, "TURRET", gmVariable( MC_BOT_BUTTON_MODULE_SPAWN_TURRET ) );
	_table->Set( _machine, "MANHACK", gmVariable( MC_BOT_BUTTON_MODULE_SPAWN_MANHACK ) );
	_table->Set( _machine, "CROW", gmVariable( MC_BOT_BUTTON_MODULE_SPAWN_CROW ) );

	_table->Set( _machine, "FASTHEADCRAB", gmVariable( MC_BOT_BUTTON_MODULE_SPAWN_FAST_HEADCRAB ) );
	_table->Set( _machine, "ZOMBIE", gmVariable( MC_BOT_BUTTON_MODULE_SPAWN_ZOMBIE ) );
	_table->Set( _machine, "FASTZOMBIE", gmVariable( MC_BOT_BUTTON_MODULE_SPAWN_FAST_ZOMBIE ) );
	_table->Set( _machine, "ANTLION", gmVariable( MC_BOT_BUTTON_MODULE_SPAWN_ANTLION ) );
	_table->Set( _machine, "ANTLION_WORKER", gmVariable( MC_BOT_BUTTON_MODULE_SPAWN_ANTLION_WORKER ) );
	_table->Set( _machine, "VORTIGAUNT", gmVariable( MC_BOT_BUTTON_MODULE_SPAWN_VORTIGAUNT ) );
}

void MC_Game::InitScriptTraceMasks( gmMachine *_machine, gmTableObject *_table )
{
	IGame::InitScriptTraceMasks( _machine, _table );
	_table->Set( _machine, "PHYSGUN", gmVariable( MC_TR_MASK_PHYSGUN ) );
}

void MC_Game::InitScriptModules( gmMachine *_machine, gmTableObject *_table )
{
	// core
	_table->Set( _machine, "RECHARGE", gmVariable( MC_MODULE_RECHARGE ) );
	_table->Set( _machine, "VITALITY", gmVariable( MC_MODULE_VITALITY ) );
	_table->Set( _machine, "AUX_CAPACITY", gmVariable( MC_MODULE_AUX_CAPACITY ) );
	_table->Set( _machine, "ARMOR_CAPACITY", gmVariable( MC_MODULE_ARMOR_CAPACITY ) );
	_table->Set( _machine, "ARMOR_REGEN", gmVariable( MC_MODULE_ARMOR_REGEN ) );

	// weapons
	_table->Set( _machine, "IMPACT_STRENGTH", gmVariable( MC_MODULE_IMPACT_STRENGTH ) );
	_table->Set( _machine, "BRUTE_FORCE", gmVariable( MC_MODULE_BRUTE_FORCE ) );
	_table->Set( _machine, "CRITICAL_HIT", gmVariable( MC_MODULE_CRITICAL_HIT ) );
	_table->Set( _machine, "CLIP_SIZE", gmVariable( MC_MODULE_CLIP_SIZE ) );
	_table->Set( _machine, "AMMO_REGEN", gmVariable( MC_MODULE_AMMO_REGEN ) );

	// mobility
	_table->Set( _machine, "MODULE_CLOAK", gmVariable( MC_MODULE_CLOAK ) );
	_table->Set( _machine, "JETPACK", gmVariable( MC_MODULE_JETPACK ) );
	_table->Set( _machine, "TELEPORT", gmVariable( MC_MODULE_TELEPORT ) );
	_table->Set( _machine, "LONG_JUMP", gmVariable( MC_MODULE_LONG_JUMP ) );
	_table->Set( _machine, "ADRENALINE", gmVariable( MC_MODULE_ADRENALINE ) );

	// projectiles
	_table->Set( _machine, "ENERGY_BALL", gmVariable( MC_MODULE_ENERGY_BALL ) );
	_table->Set( _machine, "FLECHETTE", gmVariable( MC_MODULE_FLECHETTE ) );
	_table->Set( _machine, "POISON_GRENADE", gmVariable( MC_MODULE_POISON_GRENADE ) );
	_table->Set( _machine, "FREEZE_GRENADE", gmVariable( MC_MODULE_FREEZE_GRENADE ) );
	_table->Set( _machine, "INCEN_GRENADE", gmVariable( MC_MODULE_INCEN_GRENADE ) );
	_table->Set( _machine, "MIRV", gmVariable( MC_MODULE_MIRV ) );

	// target effects
	_table->Set( _machine, "HEALD", gmVariable( MC_MODULE_HEALD ) );
	_table->Set( _machine, "DAMAGE_AMP", gmVariable( MC_MODULE_DAMAGE_AMP ) );
	_table->Set( _machine, "WEAKEN", gmVariable( MC_MODULE_WEAKEN ) );
	_table->Set( _machine, "PLAGUE", gmVariable( MC_MODULE_PLAGUE ) );
	_table->Set( _machine, "SHOCKWAVE", gmVariable( MC_MODULE_SHOCKWAVE ) );

	// deployables
	_table->Set( _machine, "DEPLOY_LASER", gmVariable( MC_MODULE_LASERS ) );
	_table->Set( _machine, "DEPLOY_TURRET", gmVariable( MC_MODULE_TURRET ) );
	_table->Set( _machine, "DEPLOY_MAGMINE", gmVariable( MC_MODULE_MAGMINE ) );
	_table->Set( _machine, "DEPLOY_CROW", gmVariable( MC_MODULE_CROW ) );

	// minions
	_table->Set( _machine, "MINION_ZOMBIE", gmVariable( MC_MODULE_MINION_ZOMBIE ) );
	_table->Set( _machine, "MINION_FASTZOMBIE", gmVariable( MC_MODULE_MINION_FASTZOMBIE ) );
	_table->Set( _machine, "MINION_ANTLION", gmVariable( MC_MODULE_MINION_ANTLION ) );
	_table->Set( _machine, "MINION_ANTLION_WORKER", gmVariable( MC_MODULE_MINION_ANTLION_WORKER ) );
	_table->Set( _machine, "MINION_VORTIGAUNT", gmVariable( MC_MODULE_MINION_VORTIGAUNT ) );
	_table->Set( _machine, "MINION_MANHACK", gmVariable( MC_MODULE_MINION_MANHACK ) );
}

void MC_Game::InitCommands()
{
	IGame::InitCommands();
}

const float MC_Game::MC_GetEntityClassTraceOffset( const TargetInfo &_target )
{
	if ( _target.mEntInfo.mGroup == ENT_GRP_PLAYER )
	{
		if ( _target.mEntInfo.mClassId > MC_CLASS_NULL && _target.mEntInfo.mClassId < FilterSensory::ANYPLAYERCLASS )
		{
			if ( _target.mEntInfo.mFlags.CheckFlag( ENT_FLAG_PRONED ) )
				return 16.0f;
			if ( _target.mEntInfo.mFlags.CheckFlag( ENT_FLAG_CROUCHED ) )
				return 24.0f;
			return 48.0f;
		}
	}
	else if ( _target.mEntInfo.mGroup == ENT_GRP_MONSTER )
	{
		switch ( _target.mEntInfo.mClassId )
		{
			case MC_CLASSEX_ZOMBIE:
			case MC_CLASSEX_ZOMBIE_FAST:
				return 48.f;
			case MC_CLASSEX_ANTLION_WORKER:
				return 20.f;
			case MC_CLASSEX_ANTLION:
			case MC_CLASSEX_ANTLION_GUARD:
				return 32.f;
			case MC_CLASSEX_HEADCRAB:
			case MC_CLASSEX_HEADCRAB_FAST:
				return 2.f;
			case MC_CLASSEX_VORTIGAUNT:
				return 48.f;
			case MC_CLASSEX_MANHACK:
				return 0.f;
			case MC_CLASSEX_TURRET:
				return 40.f;
			case MC_CLASSEX_ROLLERMINE:
				return 4.f;
			case MC_CLASSEX_HUNTER:
				return 45.f;
		}
	}
	return 0.0f;
}

const float MC_Game::MC_GetEntityClassAimOffset( const TargetInfo &_target )
{
	if ( _target.mEntInfo.mGroup == ENT_GRP_PLAYER )
	{
		if ( _target.mEntInfo.mClassId > MC_CLASS_NULL && _target.mEntInfo.mClassId < FilterSensory::ANYPLAYERCLASS )
		{
			if ( _target.mEntInfo.mFlags.CheckFlag( ENT_FLAG_PRONED ) )
				return 16.0f;
			if ( _target.mEntInfo.mFlags.CheckFlag( ENT_FLAG_CROUCHED ) )
				return 24.0f;
			return 48.0f;
		}
	}
	else if ( _target.mEntInfo.mGroup == ENT_GRP_MONSTER )
	{
		switch ( _target.mEntInfo.mClassId )
		{
			case MC_CLASSEX_ZOMBIE:
			case MC_CLASSEX_ZOMBIE_FAST:
				return 48.f;
			case MC_CLASSEX_ANTLION_WORKER:
				return 20.f;
			case MC_CLASSEX_ANTLION:
			case MC_CLASSEX_ANTLION_GUARD:
				return 32.f;
			case MC_CLASSEX_HEADCRAB:
			case MC_CLASSEX_HEADCRAB_FAST:
				return 2.f;
			case MC_CLASSEX_VORTIGAUNT:
				return 48.f;
			case MC_CLASSEX_MANHACK:
				return 0.f;
			case MC_CLASSEX_TURRET:
				return 40.f;
			case MC_CLASSEX_ROLLERMINE:
				return 4.f;
			case MC_CLASSEX_HUNTER:
				return 45.f;
		}
	}
	return 0.0f;
}
