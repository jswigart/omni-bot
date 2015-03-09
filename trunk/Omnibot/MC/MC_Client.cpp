////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "MC_Client.h"
#include "MC_Messages.h"
#include "MC_InterfaceFuncs.h"

#include "IGame.h"
#include "IGameManager.h"

MC_Client::MC_Client()
	: m_ModuleTimeStamp(0)
	, m_PlayerTimeStamp(0)
{
}

MC_Client::~MC_Client()
{
}

NavFlags MC_Client::GetTeamFlag(int _team) const
{
	static const NavFlags defaultTeam = 0;
	switch(_team)
	{
	case MC_TEAM_COMBINE:
		return NAVFLAGS_TEAM1_ONLY;
	case MC_TEAM_SCIENCE:
		return NAVFLAGS_TEAM2_ONLY;
	case MC_TEAM_REBELS:
		return NAVFLAGS_TEAM3_ONLY;
	default:
		return defaultTeam;
	}
}

void MC_Client::GetNavFlags( NavFlags & includeFlags, NavFlags & excludeFlags )
{
	includeFlags = NAVFLAGS_WALK;

	switch ( GetTeam() )
	{
		case MC_TEAM_COMBINE:
			excludeFlags = (NavFlags)( ~NAVFLAGS_TEAM1_ONLY & sTeamMask );
			break;
		case MC_TEAM_SCIENCE:
			excludeFlags = (NavFlags)( ~NAVFLAGS_TEAM2_ONLY & sTeamMask );
			break;
		case MC_TEAM_REBELS:
			excludeFlags = (NavFlags)( ~NAVFLAGS_TEAM3_ONLY & sTeamMask );
			break;
	}
}

float MC_Client::GetGameVar(GameVar _var) const
{
	switch(_var)
	{
	case JumpGapOffset:
		return 0.0f;
	}
	return 0.0f;
}

float MC_Client::GetAvoidRadius(int _class) const
{
	return 0.0f;
}

bool MC_Client::DoesBotHaveFlag(MapGoalPtr _mapgoal)
{
	return false;
}

void MC_Client::ProcessEvent(const MessageHelper &_message, CallbackParameters &_cb)
{
	switch(_message.GetMessageId())
	{
		HANDLER(Event_PlayerSpree::EventId)
		{
			_cb.CallScript();
			const Event_PlayerSpree *m = _message.Get<Event_PlayerSpree>();
			_cb.AddEntity("who", m->m_Player);
			_cb.AddInt("kills",m->m_Kills);
			break;
		}
		HANDLER(Event_PlayerSpreeEnd::EventId)
		{
			_cb.CallScript();
			const Event_PlayerSpreeEnd *m = _message.Get<Event_PlayerSpreeEnd>();
			_cb.AddEntity("who", m->m_Player);
			_cb.AddEntity("bywho", m->m_ByWho);
			_cb.AddInt("kills",m->m_Kills);
			break;
		}
		HANDLER(Event_SpreeWarStart::EventId)
		{
			_cb.CallScript();
			const Event_SpreeWarStart *m = _message.Get<Event_SpreeWarStart>();
			_cb.AddEntity("victim", m->m_Victim);
			break;
		}
		HANDLER(Event_SpreeWarEnd::EventId)
		{
			_cb.CallScript();
			/*const MC_SpreeWarEnd *m = _message.Get<MC_SpreeWarEnd>();
			_cb.AddEntity("victim", m->m_Victim);*/
			break;
		}
		HANDLER(Event_LevelUp::EventId)
		{
			const Event_LevelUp *m = _message.Get<Event_LevelUp>();
			_cb.CallScript();
			_cb.AddInt("level",m->m_Level);
			break;
		}
	}
	Client::ProcessEvent(_message, _cb);
}

const MC_PlayerStats &MC_Client::GetPlayerStats()
{
	if(m_PlayerTimeStamp != IGame::GetTime())
	{
		m_PlayerTimeStamp = IGame::GetTime();
		InterfaceFuncs::GetPlayerStats(GetGameEntity(),m_PlayerStats);
	}
	return m_PlayerStats;
}

const MC_ModuleStats &MC_Client::GetModuleStats()
{
	if(m_ModuleTimeStamp != IGame::GetTime())
	{
		m_ModuleTimeStamp = IGame::GetTime();
		InterfaceFuncs::GetModuleStats(GetGameEntity(),m_ModuleStats);
	}
	return m_ModuleStats;
}
