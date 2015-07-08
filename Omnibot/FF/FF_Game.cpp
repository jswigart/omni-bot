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
}

FF_Game::~FF_Game()
{
}

int FF_Game::GetVersionNum() const
{
	return FF_VERSION_LATEST;
}

Client *FF_Game::CreateGameClient()
{
	return new FF_Client;
}

const char *FF_Game::GetModSubFolder() const
{
#ifdef WIN32
	return "ff\\";
#else
	return "ff";
#endif
}

const char *FF_Game::GetDLLName() const
{
#ifdef WIN32
	return "omnibot_ff.dll";
#else
	return "omnibot_ff.so";
#endif
}

const char *FF_Game::GetGameName() const
{
	return "FF";
}

const char *FF_Game::GetNavSubfolder() const
{
#ifdef WIN32
	return "ff\\nav\\";
#else
	return "ff/nav";
#endif
}

const char *FF_Game::GetScriptSubfolder() const
{
#ifdef WIN32
	return "ff\\scripts\\";
#else
	return "ff/scripts";
#endif
}
const char *FF_Game::GetGameDatabaseAbbrev() const
{
	return "ff";
}
bool FF_Game::Init( System & system )
{
	if ( !TF_Game::Init( system ) )
		return false;

	// Set the sensory systems callback for getting aim offsets for entity types.
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

void FF_Game::GetGameVars( GameVars &_gamevars )
{
	_gamevars.mPlayerHeight = 72.f;
}

ClientPtr &FF_Game::GetClientFromCorrectedGameId( int _gameid )
{
	return mClientList[ _gameid - 1 ];
}

//void FF_Game::AddBot(const std::string &_name, int _team, int _class, const std::string _profile, bool _createnow)
//{
//	// Attempt to spawn a bot into the game.
//	if(_createnow)
//		.mBotJoining = true;
//	int iGameID = InterfaceFuncs::Addbot(_name, _team, _class);
//	if(_createnow)
//		.mBotJoining = false;
//	if(iGameID != -1 && _createnow)
//	{
//		if(!.mClientList[iGameID-1])
//		{
//			// Initialize the appropriate slot in the list.
//			.mClientList[iGameID-1].reset(CreateGameClient());
//			.mClientList[iGameID-1]->Init(iGameID);
//		}
//
//		.mClientList[iGameID-1]->mDesiredTeam = _team;
//		.mClientList[iGameID-1]->mDesiredClass = _class;
//
//		//////////////////////////////////////////////////////////////////////////
//		// Script callbacks
//		if(.mClientList[iGameID-1]->mDesiredTeam == -1)
//		{
//			gmVariable vteam = ScriptManager::GetInstance()->ExecBotCallback(
//				.mClientList[iGameID-1].get(),
//				"SelectTeam");
//			.mClientList[iGameID-1]->mDesiredTeam = vteam.IsInt() ? vteam.GetInt() : -1;
//		}
//		if(.mClientList[iGameID-1]->mDesiredClass == -1)
//		{
//			gmVariable vclass = ScriptManager::GetInstance()->ExecBotCallback(
//				.mClientList[iGameID-1].get(),
//				"SelectClass");
//			.mClientList[iGameID-1]->mDesiredClass = vclass.IsInt() ? vclass.GetInt() : -1;
//		}
//		//////////////////////////////////////////////////////////////////////////
//		gEngineFuncs->ChangeTeam(iGameID, .mClientList[iGameID-1]->mDesiredTeam, NULL);
//		gEngineFuncs->ChangeClass(iGameID, .mClientList[iGameID-1]->mDesiredClass, NULL);
//	}
//}

//////////////////////////////////////////////////////////////////////////

void FF_Game::GetNavParms( NavParms & navParms ) const
{
	navParms = NavParms();

	navParms.AgentHeightStand = 64.f;
	navParms.AgentHeightCrouch = 20.f;
	navParms.AgentRadius = 20.f;
}
