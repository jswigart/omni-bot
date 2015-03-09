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

IGame *CreateGameInstance()
{
	return new MC_Game;
}

int MC_Game::GetVersionNum() const
{
	return MC_VERSION_LATEST;
}

Client *MC_Game::CreateGameClient()
{
	return new MC_Client;
}

NavigatorID MC_Game::GetDefaultNavigator() const
{
	return NAVID_RECAST;
}

const char *MC_Game::GetDLLName() const
{
#ifdef WIN32
	return "omni-bot\\omnibot_mc.dll";
#else
	return "omni-bot/omnibot_mc.so";
#endif
}

const char *MC_Game::GetGameName() const
{
	return "Modular Combat";
}

const char *MC_Game::GetModSubFolder() const
{
#ifdef WIN32
	return "mc\\";
#else
	return "mc";
#endif
}

const char *MC_Game::GetNavSubfolder() const
{
	return "mc\\nav\\";
}

const char *MC_Game::GetScriptSubfolder() const
{
	return "mc\\scripts\\";
}

bool MC_Game::Init( System & system )
{
	// Set the sensory systems callback for getting aim offsets for entity types.
	AiState::SensoryMemory::SetEntityTraceOffsetCallback( MC_Game::MC_GetEntityClassTraceOffset );
	AiState::SensoryMemory::SetEntityAimOffsetCallback( MC_Game::MC_GetEntityClassAimOffset );

	if ( !IGame::Init( system ) )
		return false;

	// Run the games autoexec.
	int threadId;
	system.mScript->ExecuteFile( "scripts/mc_autoexec.gm", threadId );

	return true;
}

void MC_Game::GetGameVars( GameVars &_gamevars )
{
	_gamevars.mPlayerHeight = 72.f;
}

ClientPtr &MC_Game::GetClientFromCorrectedGameId( int _gameid )
{
	return m_ClientList[ _gameid - 1 ];
}

static IntEnum MC_TeamEnum [] =
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

static IntEnum MC_WeaponEnum [] =
{
	IntEnum( "GRAVGUN", MC_WP_GRAVGUN ),
	IntEnum( "PISTOL", MC_WP_PISTOL ),
	IntEnum( "CROWBAR", MC_WP_CROWBAR ),
	IntEnum( "STUNSTICK", MC_WP_STUNSTICK ),
	IntEnum( "SMG", MC_WP_SMG ),
	IntEnum( "SHOTGUN", MC_WP_SHOTGUN ),
	IntEnum( "SLAM", MC_WP_SLAM ),
	IntEnum( "RPG", MC_WP_RPG ),
	IntEnum( "FRAG_GREN", MC_WP_FRAG_GREN ),
	IntEnum( "MAGNUM", MC_WP_MAGNUM357 ),
	IntEnum( "CROSSBOW", MC_WP_CROSSBOW ),
	IntEnum( "AR2", MC_WP_AR2 ),
	IntEnum( "CAMERA", MC_WP_CAMERA ),
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

static IntEnum MC_ClassEnum [] =
{
	IntEnum( "DEFAULT", MC_CLASS_DEFAULT ),
	IntEnum( "ANYPLAYER", MC_CLASS_ANY ),

	IntEnum( "NPC_ZOMBIE", MC_CLASSEX_ZOMBIE ),
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
	IntEnum( "AMMO_CRATE", MC_CLASSEX_AMMO_CRATE ),

	IntEnum( "TRIPMINE", MC_CLASSEX_TRIPMINE ),
	IntEnum( "MAGMINE", MC_CLASSEX_MAGMINE ),
	IntEnum( "TURRET", MC_CLASSEX_TURRET ),
};

const char *MC_Game::FindClassName( obint32 _classId )
{
	obint32 iNumMappings = sizeof( MC_ClassEnum ) / sizeof( MC_ClassEnum[ 0 ] );
	for ( int i = 0; i < iNumMappings; ++i )
	{
		if ( MC_ClassEnum[ i ].m_Value == _classId )
			return MC_ClassEnum[ i ].m_Key;
	}
	return IGame::FindClassName( _classId );
}

void MC_Game::InitScriptClasses( gmMachine *_machine, gmTableObject *_table )
{
	IGame::InitScriptClasses( _machine, _table );

	FilterSensory::ANYPLAYERCLASS = MC_CLASS_ANY;

	obint32 iNumMappings = sizeof( MC_ClassEnum ) / sizeof( MC_ClassEnum[ 0 ] );
	for ( int i = 0; i < iNumMappings; ++i )
	{
		_table->Set( _machine, MC_ClassEnum[ i ].m_Key, gmVariable( MC_ClassEnum[ i ].m_Value ) );
	}

	InitScriptWeaponClasses( _machine, _table, MC_CLASSEX_WEAPON );
}

void MC_Game::InitScriptEvents( gmMachine *_machine, gmTableObject *_table )
{
	IGame::InitScriptEvents( _machine, _table );

	_table->Set( _machine, "PLAYER_SPREE", gmVariable( MC_EVENT_PLAYER_SPREE ) );
	_table->Set( _machine, "PLAYER_SPREE_END", gmVariable( MC_EVENT_PLAYER_SPREE_END ) );

	_table->Set( _machine, "SPREEWAR_START", gmVariable( MC_EVENT_SPREEWAR_START ) );
	_table->Set( _machine, "SPREEWAR_END", gmVariable( MC_EVENT_SPREEWAR_END ) );

	_table->Set( _machine, "LEVEL_UP", gmVariable( MC_EVENT_LEVEL_UP ) );
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

	_table->Set( _machine, "CROW", gmVariable( MC_BOT_BUTTON_MODULE_SPAWN_CROW ) );
	_table->Set( _machine, "MINION_FASTHEADCRAB", gmVariable( MC_BOT_BUTTON_MODULE_SPAWN_FAST_HEADCRAB ) );
	_table->Set( _machine, "MINION_ZOMBIE", gmVariable( MC_BOT_BUTTON_MODULE_SPAWN_ZOMBIE ) );
	_table->Set( _machine, "MINION_FASTZOMBIE", gmVariable( MC_BOT_BUTTON_MODULE_SPAWN_FAST_ZOMBIE ) );
	_table->Set( _machine, "MINION_ANTLION", gmVariable( MC_BOT_BUTTON_MODULE_SPAWN_ANTLION ) );
	_table->Set( _machine, "MINION_ANTLION_WORKER", gmVariable( MC_BOT_BUTTON_MODULE_SPAWN_ANTLION_WORKER ) );
	_table->Set( _machine, "MINION_VORTIGAUNT", gmVariable( MC_BOT_BUTTON_MODULE_SPAWN_VORTIGAUNT ) );
	_table->Set( _machine, "MINION_MANHACK", gmVariable( MC_BOT_BUTTON_MODULE_SPAWN_MANHACK ) );
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

const float MC_Game::MC_GetEntityClassTraceOffset( const int _class, const BitFlag64 &_entflags )
{
	switch ( _class )
	{
		case MC_CLASS_DEFAULT:
		{
			if ( _entflags.CheckFlag( ENT_FLAG_CROUCHED ) )
				return 20.0f;
			return 48.0f;
		}
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

	if ( _class >= MC_CLASSEX_WEAPON && _class < MC_CLASSEX_WEAPON_LAST )
		return 4.f;

	return 0.0f;
}

const float MC_Game::MC_GetEntityClassAimOffset( const int _class, const BitFlag64 &_entflags )
{
	switch ( _class )
	{
		case MC_CLASS_DEFAULT:
		{
			if ( _entflags.CheckFlag( ENT_FLAG_CROUCHED ) )
				return 20.0f;
			return 48.0f;
		}
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

	if ( _class >= MC_CLASSEX_WEAPON && _class < MC_CLASSEX_WEAPON_LAST )
		return 4.f;

	return 0.0f;
}
