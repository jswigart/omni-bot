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
{
}

MC_Client::~MC_Client()
{
}

NavFlags MC_Client::GetTeamFlag( int _team ) const
{
	static const NavFlags defaultTeam = 0;
	switch ( _team )
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

bool MC_Client::CanUseModule( MC_Module mod ) const
{
	return mModuleStats.mModule[ mod ].mLvl > 0 && 
		mPlayerStats.mAuxPower > mModuleStats.mModule[ mod ].mAuxDrain;
}

void MC_Client::GetNavFlags( NavFlags & includeFlags, NavFlags & excludeFlags )
{
	includeFlags = NAVFLAGS_WALK | NAVFLAGS_CROUCH;

	switch ( GetTeam() )
	{
		case MC_TEAM_COMBINE:
			excludeFlags = (NavFlags)( ~NAVFLAGS_TEAM1_ONLY & NAVFLAGS_ALLTEAMS );
			break;
		case MC_TEAM_SCIENCE:
			excludeFlags = (NavFlags)( ~NAVFLAGS_TEAM2_ONLY & NAVFLAGS_ALLTEAMS );
			break;
		case MC_TEAM_REBELS:
			excludeFlags = (NavFlags)( ~NAVFLAGS_TEAM3_ONLY & NAVFLAGS_ALLTEAMS );
			break;
	}
	
	if ( CanUseModule( MC_MODULE_JETPACK ) )
		includeFlags = (NavFlags)( includeFlags | NAVFLAGS_JETPACK );
	else
		excludeFlags = (NavFlags)( excludeFlags | NAVFLAGS_JETPACK );

	if ( CanUseModule( MC_MODULE_TELEPORT ) )
		includeFlags = (NavFlags)( includeFlags | NAVFLAGS_TELEPORT );
	else
		excludeFlags = (NavFlags)( excludeFlags | NAVFLAGS_JETPACK );

	if ( CanUseModule( MC_MODULE_LONG_JUMP ) )
		includeFlags = (NavFlags)( includeFlags | NAVFLAGS_LONGJUMP );
	else
		excludeFlags = (NavFlags)( excludeFlags | NAVFLAGS_LONGJUMP );
}

float MC_Client::GetGameVar( GameVar _var ) const
{
	switch ( _var )
	{
		case JumpGapOffset:
			return 0.0f;
	}
	return 0.0f;
}

bool MC_Client::DoesBotHaveFlag( MapGoalPtr _mapgoal )
{
	return false;
}

void MC_Client::Update()
{
	InterfaceFuncs::GetPlayerStats( GetGameEntity(), mPlayerStats );
	InterfaceFuncs::GetModuleStats( GetGameEntity(), mModuleStats );

	Client::Update();
}

void MC_Client::ProcessEvent( const MessageHelper &_message, CallbackParameters &_cb )
{
	switch ( _message.GetMessageId() )
	{
		HANDLER( Event_PlayerSpree::EventId )
		{
			_cb.CallScript();
			const Event_PlayerSpree *m = _message.Get<Event_PlayerSpree>();
			_cb.AddEntity( "who", m->mPlayer );
			_cb.AddInt( "kills", m->mKills );
			break;
		}
		HANDLER( Event_PlayerSpreeEnd::EventId )
		{
			_cb.CallScript();
			const Event_PlayerSpreeEnd *m = _message.Get<Event_PlayerSpreeEnd>();
			_cb.AddEntity( "who", m->mPlayer );
			_cb.AddEntity( "bywho", m->mByWho );
			_cb.AddInt( "kills", m->mKills );
			break;
		}
		HANDLER( Event_SpreeWarStart::EventId )
		{
			_cb.CallScript();
			const Event_SpreeWarStart *m = _message.Get<Event_SpreeWarStart>();
			_cb.AddEntity( "victim", m->mVictim );
			break;
		}
		HANDLER( Event_SpreeWarEnd::EventId )
		{
			_cb.CallScript();
			/*const MC_SpreeWarEnd *m = _message.Get<MC_SpreeWarEnd>();
			_cb.AddEntity("victim", m->mVictim);*/
			break;
		}
		HANDLER( Event_LevelUp::EventId )
		{
			const Event_LevelUp *m = _message.Get<Event_LevelUp>();
			_cb.CallScript();
			_cb.AddInt( "level", m->mLevel );
			break;
		}
	}
	Client::ProcessEvent( _message, _cb );
}

void MC_Client::SendVoiceMacro( int _macroId )
{
}

const MC_PlayerStats &MC_Client::GetPlayerStats()
{
	return mPlayerStats;
}

const MC_ModuleStats &MC_Client::GetModuleStats()
{
	return mModuleStats;
}
