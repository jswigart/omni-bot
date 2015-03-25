////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "OF_Game.h"
#include "OF_Client.h"
#include "TF_BaseStates.h"

#include "System.h"

#include "ScriptManager.h"

IGame *CreateGameInstance()
{
	return new OF_Game;
}

OF_Game::OF_Game()
{
}

OF_Game::~OF_Game()
{
}

int OF_Game::GetVersionNum() const
{
	return OF_VERSION_LATEST;
}

Client *OF_Game::CreateGameClient()
{
	return new OF_Client;
}

const char *OF_Game::GetModSubFolder() const
{
#ifdef WIN32
	return "of\\";
#else
	return "of";
#endif
}

const char *OF_Game::GetDLLName() const
{
#ifdef WIN32
	return "omnibot_of.dll";
#else
	return "omnibot_of.so";
#endif
}

const char *OF_Game::GetGameName() const
{
	return "OF";
}

const char *OF_Game::GetNavSubfolder() const
{
#ifdef WIN32
	return "of\\nav\\";
#else
	return "of/nav";
#endif
}

const char *OF_Game::GetScriptSubfolder() const
{
#ifdef WIN32
	return "of\\scripts\\";
#else
	return "of/scripts";
#endif
}

const char *OF_Game::GetGameDatabaseAbbrev() const
{
	return "of";
}

bool OF_Game::Init( System & system )
{
	if ( !TF_Game::Init( system ) )
		return false;

	// Set the sensory systems callback for getting aim offsets for entity types.
	AiState::SensoryMemory::SetEntityTraceOffsetCallback(OF_Game::OF_GetEntityClassTraceOffset);
	AiState::SensoryMemory::SetEntityAimOffsetCallback(OF_Game::OF_GetEntityClassAimOffset);

	// Run the games autoexec.
	int threadId;
	system.mScript->ExecuteFile("scripts/of_autoexec.gm", threadId);

	// Set up OF specific data.
	using namespace AiState;
	TF_Options::GRENADE_VELOCITY = 650.f;
	TF_Options::PIPE_MAX_DEPLOYED = 6;

	TF_Options::DisguiseTeamFlags[ OF_TEAM_BLUE ] = TF_PWR_DISGUISE_BLUE;
	TF_Options::DisguiseTeamFlags[ OF_TEAM_RED ] = TF_PWR_DISGUISE_RED;
	TF_Options::DisguiseTeamFlags[ OF_TEAM_GREEN ] = TF_PWR_DISGUISE_GREEN;
	TF_Options::DisguiseTeamFlags[ OF_TEAM_YELLOW ] = TF_PWR_DISGUISE_YELLOW;

	return true;
}

void OF_Game::GetGameVars(GameVars &_gamevars)
{
	_gamevars.mPlayerHeight = 64.f;
}

static IntEnum OF_TeamEnum[] =
{
	IntEnum("SPECTATOR",OB_TEAM_SPECTATOR),
	IntEnum("NONE",OF_TEAM_NONE),
	IntEnum("RED",OF_TEAM_RED),
	IntEnum("BLUE",OF_TEAM_BLUE),
	IntEnum("YELLOW",OF_TEAM_YELLOW),
	IntEnum("GREEN",OF_TEAM_GREEN),
};

void OF_Game::GetTeamEnumeration(const IntEnum *&_ptr, int &num)
{
	num = sizeof(OF_TeamEnum) / sizeof(OF_TeamEnum[0]);
	_ptr = OF_TeamEnum;
}

void OF_Game::InitScriptEntityFlags(gmMachine *_machine, gmTableObject *_table)
{
	//Override TF_Game, because we don't want caltrops, sabotage or radiotagged but we do want blind flag
	IGame::InitScriptEntityFlags(_machine, _table);

	_table->Set(_machine, "NEED_HEALTH",	gmVariable(TF_ENT_FLAG_SAVEME));
	_table->Set(_machine, "NEED_ARMOR",		gmVariable(TF_ENT_FLAG_ARMORME));
	_table->Set(_machine, "BURNING",		gmVariable(TF_ENT_FLAG_BURNING));
	_table->Set(_machine, "TRANQUED",		gmVariable(TF_ENT_FLAG_TRANQED));
	_table->Set(_machine, "INFECTED",		gmVariable(TF_ENT_FLAG_INFECTED));
	_table->Set(_machine, "GASSED",			gmVariable(TF_ENT_FLAG_GASSED));
	_table->Set(_machine, "SNIPE_AIMING",	gmVariable(ENT_FLAG_IRONSIGHT));
	_table->Set(_machine, "AC_FIRING",		gmVariable(TF_ENT_FLAG_ASSAULTFIRING));
	_table->Set(_machine, "LEGSHOT",		gmVariable(TF_ENT_FLAG_LEGSHOT));
	_table->Set(_machine, "BLIND",			gmVariable(OF_ENT_FLAG_BLIND));
	_table->Set(_machine, "BUILDING_SG",	gmVariable(TF_ENT_FLAG_BUILDING_SG));
	_table->Set(_machine, "BUILDING_DISP",	gmVariable(TF_ENT_FLAG_BUILDING_DISP));
	_table->Set(_machine, "BUILDING_DETP",	gmVariable(TF_ENT_FLAG_BUILDING_DETP));
	_table->Set(_machine, "BUILDINPROGRESS",gmVariable(TF_ENT_FLAG_BUILDINPROGRESS));
}

const float OF_Game::OF_GetEntityClassTraceOffset( const TargetInfo &_target )
{
	if ( _target.mEntInfo.mGroup == ENT_GRP_PLAYER )
	{
		if ( _target.mEntInfo.mClassId > TF_CLASS_NONE && _target.mEntInfo.mClassId < FilterSensory::ANYPLAYERCLASS )
		{
			if ( _target.mEntInfo.mFlags.CheckFlag( ENT_FLAG_PRONED ) )
				return 16.0f;
			if ( _target.mEntInfo.mFlags.CheckFlag( ENT_FLAG_CROUCHED ) )
				return 24.0f;
			return 48.0f;
		}
	}
	return TF_Game::TF_GetEntityClassTraceOffset( _target );
}

const float OF_Game::OF_GetEntityClassAimOffset( const TargetInfo &_target )
{
	if ( _target.mEntInfo.mGroup == ENT_GRP_PLAYER )
	{
		if ( _target.mEntInfo.mClassId > TF_CLASS_NONE && _target.mEntInfo.mClassId < FilterSensory::ANYPLAYERCLASS )
		{
			if ( _target.mEntInfo.mFlags.CheckFlag( ENT_FLAG_PRONED ) )
				return 16.0f;
			if ( _target.mEntInfo.mFlags.CheckFlag( ENT_FLAG_CROUCHED ) )
				return 24.0f;
			return 48.0f;
		}
	}
	return TF_Game::TF_GetEntityClassAimOffset( _target );
}