////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "HL2DM_Game.h"
#include "HL2DM_Client.h"
#include "System.h"
#include "ScriptManager.h"
#include "PathPlannerBase.h"
#include "BotSensoryMemory.h"
#include "FilterSensory.h"

HL2DM_Interface* gHL2DMFuncs = 0;

IGame *CreateGameInstance()
{
	return new HL2DM_Game;
}

HL2DM_Game::HL2DM_Game()
{
	mGameVars.mClientBase = -1;
	mGameVars.mGameVersion = HL2DM_VERSION_LATEST;
	mGameVars.mGameAbbrev = "hl2dm";
	mGameVars.mGameName = "Half-life 2 Deathmatch";
	mGameVars.mPlayerHeight = 72.f;
}

HL2DM_Game::~HL2DM_Game()
{
}

Client * HL2DM_Game::CreateGameClient()
{
	return new HL2DM_Client();
}

bool HL2DM_Game::Init( System & system )
{
	AiState::SensoryMemory::SetEntityTraceOffsetCallback( HL2DM_Game::HL2DM_GetEntityClassTraceOffset );
	AiState::SensoryMemory::SetEntityAimOffsetCallback( HL2DM_Game::HL2DM_GetEntityClassAimOffset );

	if ( !IGame::Init( system ) )
		return false;

	gHL2DMFuncs = dynamic_cast<HL2DM_Interface*>( gEngineFuncs );

	return true;
}

static const IntEnum HL2DM_TeamEnum [] =
{
	IntEnum( "SPECTATOR", OB_TEAM_SPECTATOR ),
	IntEnum( "TEAM1", HL2DM_TEAM_REBELS ),
	IntEnum( "TEAM2", HL2DM_TEAM_REBELS ),
};

void HL2DM_Game::GetTeamEnumeration( const IntEnum *&_ptr, int &num )
{
	num = sizeof( HL2DM_TeamEnum ) / sizeof( HL2DM_TeamEnum[ 0 ] );
	_ptr = HL2DM_TeamEnum;
}

static const IntEnum HL2DM_WeaponEnum [] =
{
	IntEnum( "CROWBAR", HL2DM_WP_CROWBAR ),
	IntEnum( "GRAVGUN", HL2DM_WP_GRAVGUN ),
	IntEnum( "STUNSTICK", HL2DM_WP_STUNSTICK ),
	IntEnum( "PISTOL", HL2DM_WP_PISTOL ),
	IntEnum( "REVOLVER", HL2DM_WP_REVOLVER ),
	IntEnum( "SMG", HL2DM_WP_SMG ),
	IntEnum( "SHOTGUN", HL2DM_WP_SHOTGUN ),
	IntEnum( "AR2", HL2DM_WP_AR2 ),
	IntEnum( "CROSSBOW", HL2DM_WP_CROSSBOW ),
	IntEnum( "GRENADE", HL2DM_WP_GRENADE ),
	IntEnum( "RPG", HL2DM_WP_RPG ),
	IntEnum( "SLAM", HL2DM_WP_SLAM ),
	IntEnum( "COMBINEGUARD", HL2DM_WP_COMBINEGUARD ),
	IntEnum( "FLAREGUN", HL2DM_WP_FLAREGUN ),
	IntEnum( "ANNABELLE", HL2DM_WP_ANNABELLE ),
	IntEnum( "COMBINEGUARD", HL2DM_WP_COMBINEGUARD ),
	IntEnum( "FLAREGUN", HL2DM_WP_FLAREGUN ),
	IntEnum( "ANNABELLE", HL2DM_WP_ANNABELLE ),
	IntEnum( "BUGBAIT", HL2DM_WP_BUGBAIT ),
};

void HL2DM_Game::GetWeaponEnumeration( const IntEnum *&_ptr, int &num )
{
	num = sizeof( HL2DM_WeaponEnum ) / sizeof( HL2DM_WeaponEnum[ 0 ] );
	_ptr = HL2DM_WeaponEnum;
}

static const IntEnum gClassMapping[] =
{
	IntEnum( "PLAYER", HL2DM_CLASS_PLAYER ),
	IntEnum( "ANYPLAYER", HL2DM_CLASS_ANY ),

	/*IntEnum( "NPC_ZOMBIE", HL2DM_CLASSEX_ZOMBIE ),
	IntEnum( "NPC_ZOMBIE_FAST", HL2DM_CLASSEX_ZOMBIE_FAST ),
	IntEnum( "NPC_ANTLION", HL2DM_CLASSEX_ANTLION ),
	IntEnum( "NPC_ANTLION_WORKER", HL2DM_CLASSEX_ANTLION_WORKER ),
	IntEnum( "NPC_ANTLION_GUARD", HL2DM_CLASSEX_ANTLION_GUARD ),
	IntEnum( "NPC_HEADCRAB", HL2DM_CLASSEX_HEADCRAB ),
	IntEnum( "NPC_HEADCRAB_FAST", HL2DM_CLASSEX_HEADCRAB_FAST ),
	IntEnum( "NPC_VORTIGAUNT", HL2DM_CLASSEX_VORTIGAUNT ),
	IntEnum( "NPC_HUNTER", HL2DM_CLASSEX_HUNTER ),
	IntEnum( "NPC_MANHACK", HL2DM_CLASSEX_MANHACK ),
	IntEnum( "NPC_CROW", HL2DM_CLASSEX_CROW ),
	IntEnum( "NPC_ROLLERMINE", HL2DM_CLASSEX_ROLLERMINE ),

	IntEnum( "PROPBREAKABLE", HL2DM_CLASSEX_PROPBREAKABLE ),
	IntEnum( "PROPEXPLOSIVE", HL2DM_CLASSEX_PROPEXPLOSIVE ),

	IntEnum( "HEALTHKIT", HL2DM_CLASSEX_HEALTHKIT ),
	IntEnum( "HEALTHVIAL", HL2DM_CLASSEX_HEALTHVIAL ),
	IntEnum( "HEALTH_WALLUNIT", HL2DM_CLASSEX_HEALTH_WALLUNIT ),
	IntEnum( "ENERGY_WALLUNIT", HL2DM_CLASSEX_ENERGY_WALLUNIT ),

	IntEnum( "BATTERY", HL2DM_CLASSEX_BATTERY ),
	IntEnum( "POWERCUBE", HL2DM_CLASSEX_POWERCUBE ),
	IntEnum( "ITEMCRATE", HL2DM_CLASSEX_ITEMCRATE ),

	IntEnum( "AMMO_PISTOL", HL2DM_CLASSEX_PISTOL_AMMO ),
	IntEnum( "AMMO_PISTOL_L", HL2DM_CLASSEX_LARGE_PISTOL_AMMO ),
	IntEnum( "AMMO_SMG", HL2DM_CLASSEX_SMG_AMMO ),
	IntEnum( "AMMO_SMG_L", HL2DM_CLASSEX_LARGE_SMG_AMMO ),
	IntEnum( "AMMO_AR2", HL2DM_CLASSEX_AR2_AMMO ),
	IntEnum( "AMMO_AR2_L", HL2DM_CLASSEX_LARGE_AR2_AMMO ),
	IntEnum( "AMMO_357", HL2DM_CLASSEX_357_AMMO ),
	IntEnum( "AMMO_357_L", HL2DM_CLASSEX_LARGE_357_AMMO ),
	IntEnum( "AMMO_CROSSBOW", HL2DM_CLASSEX_CROSSBOW_AMMO ),
	IntEnum( "AMMO_FLARE", HL2DM_CLASSEX_FLARE_AMMO ),
	IntEnum( "AMMO_FLARE_L", HL2DM_CLASSEX_LARGE_FLARE_AMMO ),
	IntEnum( "AMMO_RPG", HL2DM_CLASSEX_RPG_AMMO ),
	IntEnum( "AMMO_AR2GREN", HL2DM_CLASSEX_AR2GREN_AMMO ),
	IntEnum( "AMMO_SNIPER", HL2DM_CLASSEX_SNIPER_AMMO ),
	IntEnum( "AMMO_SHOTGUN", HL2DM_CLASSEX_SHOTGUN_AMMO ),
	IntEnum( "AMMO_AR2_ALTFIRE", HL2DM_CLASSEX_AR2_ALTFIRE_AMMO ),

	IntEnum( "TRIPMINE", HL2DM_CLASSEX_TRIPMINE ),
	IntEnum( "MAGMINE", HL2DM_CLASSEX_MAGMINE ),
	IntEnum( "TURRET", HL2DM_CLASSEX_TURRET ),*/
};

void HL2DM_Game::InitScriptClasses( gmMachine *_machine, gmTableObject *_table )
{
	IGame::InitScriptClasses( _machine, _table );

	FilterSensory::ANYPLAYERCLASS = HL2DM_CLASS_ANY;

	const size_t iNumMappings = sizeof( gClassMapping ) / sizeof( gClassMapping[ 0 ] );
	for ( int i = 0; i < iNumMappings; ++i )
	{
		_table->Set( _machine, gClassMapping[ i ].mKey, gmVariable( gClassMapping[ i ].mValue ) );
	}
}

void HL2DM_Game::InitScriptEvents( gmMachine *_machine, gmTableObject *_table )
{
	IGame::InitScriptEvents( _machine, _table );
}

void HL2DM_Game::InitScriptTraceMasks( gmMachine *_machine, gmTableObject *_table )
{
	IGame::InitScriptTraceMasks( _machine, _table );
	_table->Set( _machine, "PHYSGUN", gmVariable( HL2DM_TR_MASK_PHYSGUN ) );
}

void HL2DM_Game::InitCommands()
{
	IGame::InitCommands();
}

const float HL2DM_Game::HL2DM_GetEntityClassTraceOffset( const TargetInfo &_target )
{
	if ( _target.mEntInfo.mClassId > HL2DM_CLASS_NULL && _target.mEntInfo.mClassId < HL2DM_CLASS_MAX )
	{
		if ( _target.mEntInfo.mFlags.CheckFlag( ENT_FLAG_CROUCHED ) )
			return 20.0f;
		return 48.0f;
	}
	return 0.0f;
}

const float HL2DM_Game::HL2DM_GetEntityClassAimOffset( const TargetInfo &_target )
{
	if ( _target.mEntInfo.mClassId > HL2DM_CLASS_NULL && _target.mEntInfo.mClassId < HL2DM_CLASS_MAX )
	{
		if ( _target.mEntInfo.mFlags.CheckFlag( ENT_FLAG_CROUCHED ) )
			return 20.0f;
		return 48.0f;
	}
	return 0.0f;
}
