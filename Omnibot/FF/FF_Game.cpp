////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "FF_Game.h"
#include "FF_Config.h"
#include "TF_BaseStates.h"
#include "FF_Client.h"

#include "System.h"
#include "ScriptManager.h"

IGame *CreateGameInstance()
{
	return new FF_Game;
}

FF_Game::FF_Game()
{
	mGameVars.mClientBase = -1;
	mGameVars.mGameVersion = FF_VERSION_LATEST;
	mGameVars.mGameAbbrev = "ff";
	mGameVars.mGameName = "Fortress Forever";
	mGameVars.mPlayerHeight = 72.f;
	mGameVars.mRendersToGame = true;
}

FF_Game::~FF_Game()
{
}

Client *FF_Game::CreateGameClient()
{
	return new FF_Client;
}

bool FF_Game::Init( System & system )
{
	if ( !TF_Game::Init( system ) )
		return false;

	//AiState::SensoryMemory::SetEntityTraceOffsetCallback(FF_Game::FF_GetEntityClassTraceOffset);
	//AiState::SensoryMemory::SetEntityAimOffsetCallback(FF_Game::FF_GetEntityClassAimOffset);

	TF_Options::DisguiseTeamFlags[ TF_TEAM_BLUE ] = TF_PWR_DISGUISE_BLUE;
	TF_Options::DisguiseTeamFlags[ TF_TEAM_RED ] = TF_PWR_DISGUISE_RED;
	TF_Options::DisguiseTeamFlags[ TF_TEAM_GREEN ] = TF_PWR_DISGUISE_GREEN;
	TF_Options::DisguiseTeamFlags[ TF_TEAM_YELLOW ] = TF_PWR_DISGUISE_YELLOW;

	return true;
}

static const IntEnum FF_TeamEnum [] =
{
	IntEnum( "SPECTATOR", OB_TEAM_SPECTATOR ),
	IntEnum( "NONE", TF_TEAM_NONE ),
	IntEnum( "RED", TF_TEAM_RED ),
	IntEnum( "BLUE", TF_TEAM_BLUE ),
	IntEnum( "YELLOW", TF_TEAM_YELLOW ),
	IntEnum( "GREEN", TF_TEAM_GREEN ),
};

void FF_Game::GetTeamEnumeration( const IntEnum *&_ptr, int &num )
{
	num = sizeof( FF_TeamEnum ) / sizeof( FF_TeamEnum[ 0 ] );
	_ptr = FF_TeamEnum;
}

void FF_Game::GetNavParms( NavParms & navParms ) const
{
	navParms = NavParms();

	navParms.AgentHeightStand = 64.f;
	navParms.AgentHeightCrouch = 24.f;
	navParms.AgentRadius = 20.f;
}

