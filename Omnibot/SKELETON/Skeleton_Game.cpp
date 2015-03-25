////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "Skeleton_Game.h"
#include "Skeleton_Client.h"

#include "System.h"
#include "PathPlannerBase.h"
#include "ScriptManager.h"
#include "FilterSensory.h"
#include "BotSensoryMemory.h"

IGame *CreateGameInstance()
{
	return new Skeleton_Game;
}

Skeleton_Game::Skeleton_Game()
{
}

Skeleton_Game::~Skeleton_Game()
{
}

int Skeleton_Game::GetVersionNum() const
{
	return SKELETON_VERSION_LATEST;
}

Client *Skeleton_Game::CreateGameClient()
{
	return new Skeleton_Client;
}

const char *Skeleton_Game::GetDLLName() const
{
	return "omni-bot\\omnibot_skeleton.dll";
}

const char *Skeleton_Game::GetGameName() const
{
	return "Skeleton";
}

const char *Skeleton_Game::GetModSubFolder() const
{
#ifdef WIN32
	return "skeleton\\";
#else
	return "skeleton";
#endif
}

const char *Skeleton_Game::GetNavSubfolder() const
{
	return "skeleton\\nav\\";
}

const char *Skeleton_Game::GetScriptSubfolder() const
{
	return "skeleton\\scripts\\";
}
const char *Skeleton_Game::GetGameDatabaseAbbrev() const
{
	return "skele";
}
bool Skeleton_Game::Init( System & system )
{
	// Set the sensory systems callback for getting aim offsets for entity types.
	AiState::SensoryMemory::SetEntityTraceOffsetCallback( Skeleton_Game::Skeleton_GetEntityClassTraceOffset );
	AiState::SensoryMemory::SetEntityAimOffsetCallback( Skeleton_Game::Skeleton_GetEntityClassAimOffset );

	if ( !IGame::Init( system ) )
		return false;

	// Run the games autoexec.
	int threadId;
	system.mScript->ExecuteFile( "scripts/skeleton_autoexec.gm", threadId );

	return true;
}

void Skeleton_Game::GetGameVars( GameVars &_gamevars )
{
	_gamevars.mPlayerHeight = 72.f;
}

static IntEnum Skel_TeamEnum [] =
{
	IntEnum( "SPECTATOR", OB_TEAM_SPECTATOR ),
	IntEnum( "TEAM1", SKELETON_TEAM_1 ),
	IntEnum( "TEAM2", SKELETON_TEAM_2 ),
};

void Skeleton_Game::GetTeamEnumeration( const IntEnum *&_ptr, int &num )
{
	num = sizeof( Skel_TeamEnum ) / sizeof( Skel_TeamEnum[ 0 ] );
	_ptr = Skel_TeamEnum;
}

void Skeleton_Game::GetWeaponEnumeration( const IntEnum *&_ptr, int &num )
{
}

static const IntEnum gClassMapping [] =
{
	IntEnum( "PLAYER", SKELETON_CLASS_PLAYER ),
	IntEnum( "ANYPLAYER", SKELETON_CLASS_ANY ),
	IntEnum( "GRENADE", SKELETON_CLASSEX_GRENADE ),
};

void Skeleton_Game::InitScriptClasses( gmMachine *_machine, gmTableObject *_table )
{
	IGame::InitScriptClasses( _machine, _table );

	FilterSensory::ANYPLAYERCLASS = SKELETON_CLASS_ANY;

	int32_t iNumMappings = sizeof( gClassMapping ) / sizeof( gClassMapping[ 0 ] );
	for ( int i = 0; i < iNumMappings; ++i )
	{
		_table->Set( _machine, gClassMapping[ i ].mKey, gmVariable( gClassMapping[ i ].mValue ) );
	}
}

void Skeleton_Game::InitScriptEvents( gmMachine *_machine, gmTableObject *_table )
{
	IGame::InitScriptEvents( _machine, _table );
}

void Skeleton_Game::InitCommands()
{
	IGame::InitCommands();
}

const float Skeleton_Game::Skeleton_GetEntityClassTraceOffset( const TargetInfo &_target )
{
	if ( _target.mEntInfo.mGroup == ENT_GRP_PLAYER )
	{
		if ( _target.mEntInfo.mClassId > SKELETON_CLASS_NULL && _target.mEntInfo.mClassId < FilterSensory::ANYPLAYERCLASS )
		{
			if ( _target.mEntInfo.mFlags.CheckFlag( ENT_FLAG_PRONED ) )
				return 16.0f;
			if ( _target.mEntInfo.mFlags.CheckFlag( ENT_FLAG_CROUCHED ) )
				return 24.0f;
			return 48.0f;
		}
	}
	return 0.0f;
}

const float Skeleton_Game::Skeleton_GetEntityClassAimOffset( const TargetInfo &_target )
{
	if ( _target.mEntInfo.mGroup == ENT_GRP_PLAYER )
	{
		if ( _target.mEntInfo.mClassId > SKELETON_CLASS_NULL && _target.mEntInfo.mClassId < FilterSensory::ANYPLAYERCLASS )
		{
			if ( _target.mEntInfo.mFlags.CheckFlag( ENT_FLAG_PRONED ) )
				return 16.0f;
			if ( _target.mEntInfo.mFlags.CheckFlag( ENT_FLAG_CROUCHED ) )
				return 24.0f;
			return 48.0f;
		}
	}
	return 0.0f;
}
