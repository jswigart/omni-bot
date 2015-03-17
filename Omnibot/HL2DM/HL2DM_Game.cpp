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

IGame *CreateGameInstance()
{
	return new HL2DM_Game;
}

int HL2DM_Game::GetVersionNum() const
{
	return HL2DM_VERSION_LATEST;
}

Client *HL2DM_Game::CreateGameClient()
{
	return new HL2DM_Client;
}

const char *HL2DM_Game::GetDLLName() const
{
#ifdef WIN32
	return "omni-bot\\omnibot_hl2dm.dll";
#else
	return "omni-bot/omnibot_hl2dm.so";
#endif
}

const char *HL2DM_Game::GetGameName() const
{
	return "HL2DM";
}

const char *HL2DM_Game::GetModSubFolder() const
{
#ifdef WIN32
	return "hl2dm\\";
#else
	return "hl2dm";
#endif
}

const char *HL2DM_Game::GetNavSubfolder() const
{
	return "hl2dm\\nav\\";
}

const char *HL2DM_Game::GetScriptSubfolder() const
{
	return "hl2dm\\scripts\\";
}

bool HL2DM_Game::Init( System & system )
{
	// Set the sensory systems callback for getting aim offsets for entity types.
	AiState::SensoryMemory::SetEntityTraceOffsetCallback( HL2DM_Game::HL2DM_GetEntityClassTraceOffset );
	AiState::SensoryMemory::SetEntityAimOffsetCallback( HL2DM_Game::HL2DM_GetEntityClassAimOffset );

	if ( !IGame::Init( system ) )
		return false;

	// Run the games autoexec.
	int threadId;
	system.mScript->ExecuteFile( "scripts/hl2dm_autoexec.gm", threadId );

	return true;
}

void HL2DM_Game::GetGameVars( GameVars &_gamevars )
{
	_gamevars.mPlayerHeight = 72.f;
}

static IntEnum HL2DM_TeamEnum [] =
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

static IntEnum HL2DM_WeaponEnum [] =
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

IntEnum g_HL2DMClassMappings [] =
{
	IntEnum( "PLAYER", HL2DM_CLASS_PLAYER ),
	IntEnum( "ANYPLAYER", HL2DM_CLASS_ANY ),
};

const char *HL2DM_Game::FindClassName( obint32 _classId )
{
	obint32 iNumMappings = sizeof( g_HL2DMClassMappings ) / sizeof( g_HL2DMClassMappings[ 0 ] );
	for ( int i = 0; i < iNumMappings; ++i )
	{
		if ( g_HL2DMClassMappings[ i ].m_Value == _classId )
			return g_HL2DMClassMappings[ i ].m_Key;
	}
	return IGame::FindClassName( _classId );
}

void HL2DM_Game::InitScriptClasses( gmMachine *_machine, gmTableObject *_table )
{
	IGame::InitScriptClasses( _machine, _table );

	FilterSensory::ANYPLAYERCLASS = HL2DM_CLASS_ANY;

	obint32 iNumMappings = sizeof( g_HL2DMClassMappings ) / sizeof( g_HL2DMClassMappings[ 0 ] );
	for ( int i = 0; i < iNumMappings; ++i )
	{
		_table->Set( _machine, g_HL2DMClassMappings[ i ].m_Key, gmVariable( g_HL2DMClassMappings[ i ].m_Value ) );
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

const float HL2DM_Game::HL2DM_GetEntityClassTraceOffset( const int _class, const BitFlag64 &_entflags )
{
	if ( _class > HL2DM_CLASS_NULL && _class < HL2DM_CLASS_MAX )
	{
		if ( _entflags.CheckFlag( ENT_FLAG_CROUCHED ) )
			return 20.0f;
		return 48.0f;
	}
	return 0.0f;
}

const float HL2DM_Game::HL2DM_GetEntityClassAimOffset( const int _class, const BitFlag64 &_entflags )
{
	if ( _class > HL2DM_CLASS_NULL && _class < HL2DM_CLASS_MAX )
	{
		if ( _entflags.CheckFlag( ENT_FLAG_CROUCHED ) )
			return 20.0f;
		return 48.0f;
	}
	return 0.0f;
}
