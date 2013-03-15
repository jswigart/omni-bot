////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy: jswigart $
// $LastChangedDate: 2010-08-28 00:12:05 -0500 (Sat, 28 Aug 2010) $
// $LastChangedRevision: 32 $
//
////////////////////////////////////////////////////////////////////////////////

#include "ETF_Game.h"
#include "ETF_Client.h"
#include "TF_BaseStates.h"
#include "ETF_Config.h"

#include "gmTFBinds.h"

#include "System.h"
#include "NameManager.h"
#include "ScriptManager.h"

BOOST_STATIC_ASSERT( ETF_TEAM_MAX == TF_TEAM_MAX );

IGame *CreateGameInstance()
{
	return new ETF_Game;
}

int ETF_Game::GetVersionNum() const
{
	return ETF_VERSION_LATEST;
}

Client *ETF_Game::CreateGameClient()
{
	return new ETF_Client;
}

const char *ETF_Game::GetModSubFolder() const
{
#ifdef WIN32
	return "ETF\\";
#else
	return "etf";
#endif
}

const char *ETF_Game::GetDLLName() const
{
#ifdef WIN32
	return "omnibot_etf.dll";
#else
	return "omnibot_etf.so";
#endif
}

const char *ETF_Game::GetGameName() const
{
	return "ETF";
}

const char *ETF_Game::GetNavSubfolder() const
{
#ifdef WIN32
	return "etf\\nav\\";
#else
	return "etf/nav";
#endif
}

const char *ETF_Game::GetScriptSubfolder() const
{
#ifdef WIN32
	return "etf\\scripts\\";
#else
	return "etf/scripts";
#endif
}

bool ETF_Game::Init( System & system )
{
	if ( !TF_Game::Init( system ) )
		return false;

	// Set the sensory systems callback for getting aim offsets for entity types.
	AiState::SensoryMemory::SetEntityTraceOffsetCallback(ETF_Game::ETF_GetEntityClassTraceOffset);
	AiState::SensoryMemory::SetEntityAimOffsetCallback(ETF_Game::ETF_GetEntityClassAimOffset);

	// Run the games autoexec.
	int threadId;

	system.mScript->ExecuteFile("scripts/etf_autoexec.gm", threadId);

	// Set up ETF specific data.
	using namespace AiState;
	TF_Options::GRENADE_VELOCITY = 650.f;
	TF_Options::PIPE_MAX_DEPLOYED = 6;

	TF_Options::DisguiseTeamFlags[ ETF_TEAM_BLUE ] = TF_PWR_DISGUISE_BLUE;
	TF_Options::DisguiseTeamFlags[ ETF_TEAM_RED ] = TF_PWR_DISGUISE_RED;
	TF_Options::DisguiseTeamFlags[ ETF_TEAM_GREEN ] = TF_PWR_DISGUISE_GREEN;
	TF_Options::DisguiseTeamFlags[ ETF_TEAM_YELLOW ] = TF_PWR_DISGUISE_YELLOW;

	return true;
}

void ETF_Game::GetGameVars(GameVars &_gamevars)
{
	_gamevars.mPlayerHeight = 64.f;
}

void ETF_Game::InitScriptBinds(gmMachine *_machine)
{
	LOG("Binding ETF Library...");
	gmBindTFLibrary(_machine);
}

static IntEnum ETF_TeamEnum[] =
{
	IntEnum("SPECTATOR",OB_TEAM_SPECTATOR),
	IntEnum("NONE",ETF_TEAM_NONE),
	IntEnum("RED",ETF_TEAM_RED),
	IntEnum("BLUE",ETF_TEAM_BLUE),
	IntEnum("YELLOW",ETF_TEAM_YELLOW),
	IntEnum("GREEN",ETF_TEAM_GREEN),
};

void ETF_Game::GetTeamEnumeration(const IntEnum *&_ptr, int &num)
{
	num = sizeof(ETF_TeamEnum) / sizeof(ETF_TeamEnum[0]);
	_ptr = ETF_TeamEnum;
}

void ETF_Game::InitScriptEntityFlags(gmMachine *_machine, gmTableObject *_table)
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
	_table->Set(_machine, "BLIND",			gmVariable(ETF_ENT_FLAG_BLIND));
	_table->Set(_machine, "BUILDING_SG",	gmVariable(TF_ENT_FLAG_BUILDING_SG));
	_table->Set(_machine, "BUILDING_DISP",	gmVariable(TF_ENT_FLAG_BUILDING_DISP));
	_table->Set(_machine, "BUILDING_DETP",	gmVariable(TF_ENT_FLAG_BUILDING_DETP));
	_table->Set(_machine, "BUILDINPROGRESS",gmVariable(TF_ENT_FLAG_BUILDINPROGRESS));
	_table->Set(_machine, "LEVEL2",			gmVariable(TF_ENT_FLAG_LEVEL2));
	_table->Set(_machine, "LEVEL3",			gmVariable(TF_ENT_FLAG_LEVEL3));
}

const float ETF_Game::ETF_GetEntityClassTraceOffset(const int _class, const BitFlag64 &_entflags)
{
	if(InRangeT<int>(_class, TF_CLASS_NONE, TF_CLASS_MAX))
	{
		if (_entflags.CheckFlag(ENT_FLAG_CROUCHED))
		{
			switch(_class)
			{
			case TF_CLASS_HWGUY:
				return 34.0f;
			case TF_CLASS_SCOUT:
			case TF_CLASS_SNIPER:
				return 22.4f;
			default:
				return 24.0f;
			}
		}
		if(_class == TF_CLASS_HWGUY)
			return 40.0f;
		return 32.0f;
	}
	return TF_Game::TF_GetEntityClassTraceOffset(_class,_entflags);
}

const float ETF_Game::ETF_GetEntityClassAimOffset(const int _class, const BitFlag64 &_entflags)
{
	if(InRangeT<int>(_class, TF_CLASS_NONE, TF_CLASS_MAX))
	{
		if (_entflags.CheckFlag(ENT_FLAG_CROUCHED))
		{
			switch(_class)
			{
			case TF_CLASS_HWGUY:
				return 26.0f;
			case TF_CLASS_SCOUT:
			case TF_CLASS_SNIPER:
				return 14.4f;
			default:
				return 16.0f;
			}
		}
		if(_class == TF_CLASS_HWGUY)
			return 32.0f;
		return 24.0f;
	}
	return TF_Game::TF_GetEntityClassAimOffset(_class,_entflags);
}