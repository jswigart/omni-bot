////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "MC_Client.h"
#include "MC_Messages.h"

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
	float auxCur, auxMax, auxRegen;
	if ( !gModularCombatFuncs->GetPlayerAux( GetGameEntity(), auxCur, auxMax, auxRegen ) )
		return false;

	int lvl, lvlMax, upgCost;
	float auxDrain, cooldown;
	if ( !gModularCombatFuncs->ModuleStats( GetGameEntity(), mod, lvl, lvlMax, upgCost, auxDrain, cooldown ) )
		return false;
	return lvl > 0 && auxCur > auxDrain;
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
	Client::Update();
}

void MC_Client::ProcessEvent( const Message & message, CallbackParameters & cb )
{
	switch ( message.Id() )
	{
		CASE_MSG( EvPlayerSpree )
		{
			cb.CallScript();
			cb.AddEntity( "who", msg->mPlayer );
			cb.AddInt( "kills", msg->mKills );
			break;
		}
		CASE_MSG( EvPlayerSpreeEnd )
		{
			cb.CallScript();
			cb.AddEntity( "who", msg->mPlayer );
			cb.AddEntity( "bywho", msg->mByWho );
			cb.AddInt( "kills", msg->mKills );
			break;
		}
		CASE_MSG( EvSpreeWarStart )
		{
			cb.CallScript();
			cb.AddEntity( "victim", msg->mVictim );
			break;
		}
		CASE_MSG( EvSpreeWarEnd )
		{
			cb.CallScript();
			/*const MC_SpreeWarEnd *m = _message.Get<MC_SpreeWarEnd>();
			_cb.AddEntity("victim", m->mVictim);*/
			break;
		}
		CASE_MSG( EvLevelUp )
		{
			cb.CallScript();
			cb.AddInt( "level", msg->mLevel );
			break;
		}
	}
	Client::ProcessEvent( message, cb );
}

void MC_Client::SendVoiceMacro( int _macroId )
{
}
