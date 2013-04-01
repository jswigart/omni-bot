////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy: jswigart $
// $LastChangedDate: 2010-08-28 00:12:05 -0500 (Sat, 28 Aug 2010) $
// $LastChangedRevision: 32 $
//
////////////////////////////////////////////////////////////////////////////////

#include "ETF_Game.h"
#include "TF_GoalManager.h"
#include "TF_NavigationFlags.h"
#include "TF_InterfaceFuncs.h"
#include "ETF_Client.h"

#include "System.h"
#include "RenderBuffer.h"

#include "Waypoint.h"
#include "PathPlannerBase.h"
#include "BotBaseStates.h"
#include "TF_BaseStates.h"
#include "NameManager.h"
#include "ScriptManager.h"
#include "gmTFBinds.h"

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
	return "etf\\";
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

const char *ETF_Game::IsDebugDrawSupported() const
{
	if(InterfaceFuncs::GetCvar("sv_running") != 1 && InterfaceFuncs::GetCvar("cl_running") != 1)
		return "Can't draw waypoints non-server client.";
	if(InterfaceFuncs::GetCvar("dedicated") != 0)
		return "Can't draw waypoints on dedicated server.";
	bool EnableIpc = false;
	Options::GetValue("Debug Render","EnableInterProcess",EnableIpc);
	if(!EnableIpc)
		return "Waypoints are not visible because option EnableInterProcess in file omni-bot.cfg is false.";
	if(InterfaceFuncs::GetCvar("cg_omnibotdrawing")==0)
		return "Waypoints are not visible because cg_omnibotdrawing is \"0\".";
	return NULL;
}

bool ETF_Game::Init( System & system )
{
	if ( !TF_Game::Init( system ) )
		return false;

	AiState::FollowPath::m_OldLadderStyle = false;

	// Set the sensory systems callback for getting aim offsets for entity types.
	AiState::SensoryMemory::SetEntityTraceOffsetCallback(ETF_Game::ETF_GetEntityClassTraceOffset);
	AiState::SensoryMemory::SetEntityAimOffsetCallback(ETF_Game::ETF_GetEntityClassAimOffset);
	AiState::SensoryMemory::SetEntityVisDistanceCallback(ETF_Game::ETF_GetEntityVisDistance);

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

	_table->Set(_machine, "DISGUISED",		gmVariable(ETF_ENT_FLAG_DISGUISED));
}

void ETF_Game::InitScriptPowerups(gmMachine *_machine, gmTableObject *_table)
{
	TF_Game::InitScriptPowerups(_machine, _table);

	_table->Set(_machine, "QUAD",		gmVariable(ETF_PWR_QUAD));
	_table->Set(_machine, "SUIT",		gmVariable(ETF_PWR_SUIT));
	_table->Set(_machine, "HASTE",		gmVariable(ETF_PWR_HASTE));
	_table->Set(_machine, "INVIS",		gmVariable(ETF_PWR_INVIS));
	_table->Set(_machine, "REGEN",		gmVariable(ETF_PWR_REGEN));
	_table->Set(_machine, "FLIGHT",		gmVariable(ETF_PWR_FLIGHT));
	_table->Set(_machine, "INVULN",		gmVariable(ETF_PWR_INVULN));
	_table->Set(_machine, "AQUALUNG",	gmVariable(ETF_PWR_AQUALUNG));
	//_table->Set(_machine, "CEASEFIRE",	gmVariable(ETF_PWR_CEASEFIRE));
}

const void ETF_Game::ETF_GetEntityVisDistance(float &_distance, const TargetInfo &_target, const Client *_client)
{
	switch(_target.m_EntityClass)
	{
	case TF_CLASSEX_RESUPPLY:
	case TF_CLASSEX_BACKPACK:
	case TF_CLASSEX_BACKPACK_AMMO:
	case TF_CLASSEX_BACKPACK_HEALTH:
	case TF_CLASSEX_BACKPACK_ARMOR:
	case TF_CLASSEX_BACKPACK_GRENADES:
		_distance = 2000.0f;
	default:
		//if(_target.m_EntityClass - ET_Game::CLASSEXoffset == ET_CLASSEX_BREAKABLE)
		//	_distance = static_cast<const ET_Client*>(_client)->GetBreakableTargetDist();
		//else if(_target.m_EntityCategory.CheckFlag(ENT_CAT_PICKUP_WEAPON))
		//	_distance = 1500.0f;
		//if(_target.m_EntityCategory.CheckFlag(TF_ENT_CAT_BUILDABLE))
		//	_distance = 1000.0f;
		/*else */if(_target.m_EntityCategory.CheckFlag(ENT_CAT_PROJECTILE))
			_distance = 500.0f;
	}
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

// PathPlannerWaypointInterface

NavFlags ETF_Game::WaypointBlockableFlags() const
{
	return F_TF_NAV_WALL | F_TF_NAV_DETPACK;
}

NavFlags ETF_Game::WaypointCallbackFlags() const
{
	return F_TF_NAV_ROCKETJUMP |
		F_TF_NAV_CONCJUMP;
}

PathPlannerWaypointInterface::BlockableStatus ETF_Game::WaypointPathCheck(const Waypoint * _wp1, const Waypoint * _wp2, bool _draw) const
{
	static bool bRender = false;
	PathPlannerWaypointInterface::BlockableStatus res = PathPlannerWaypointInterface::B_INVALID_FLAGS;

	Vector3f vStart, vEnd;

	if(/*_wp1->IsFlagOn(F_TF_NAV_WALL) &&*/ _wp2->IsFlagOn(F_TF_NAV_WALL))
	{
		static float fOffset = 0.0f;
		static Vector3f vMins(-5.f, -5.f, -5.f), vMaxs(5.f, 5.f, 5.f);
		AABB aabb(vMins, vMaxs);
		vStart = _wp1->GetPosition() + Vector3f(0, 0, fOffset);
		vEnd = _wp2->GetPosition() + Vector3f(0, 0, fOffset);

		obTraceResult tr;
		EngineFuncs::TraceLine(tr, vStart, vEnd, &aabb, (TR_MASK_SOLID | TR_MASK_PLAYERCLIP), -1, True);
		res = (tr.m_Fraction == 1.0f) ? PathPlannerWaypointInterface::B_PATH_OPEN : PathPlannerWaypointInterface::B_PATH_CLOSED;
	}
	else if(_wp1->IsFlagOn(F_TF_NAV_DETPACK) && _wp2->IsFlagOn(F_TF_NAV_DETPACK))
	{
		Vector3f vStart = _wp1->GetPosition() + Vector3f(0, 0, 40.0f);
		Vector3f vEnd = _wp2->GetPosition() + Vector3f(0, 0, 40.0f);

		obTraceResult tr;
		EngineFuncs::TraceLine(tr, vStart, vEnd, NULL, (TR_MASK_SOLID | TR_MASK_PLAYERCLIP), -1, True);
		res = (tr.m_Fraction == 1.0f) ? PathPlannerWaypointInterface::B_PATH_OPEN : PathPlannerWaypointInterface::B_PATH_CLOSED;
	}
	return res;
}