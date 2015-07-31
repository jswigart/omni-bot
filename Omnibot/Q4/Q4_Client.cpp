////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "Q4_Client.h"
#include "Q4_Messages.h"

#include "IGame.h"
#include "IGameManager.h"

//////////////////////////////////////////////////////////////////////////

Q4_Client::Q4_Client()
{
	mStepHeight = 15.0f; // it's actually 16
}

Q4_Client::~Q4_Client()
{
}

NavFlags Q4_Client::GetTeamFlag( int _team ) const
{
	static const NavFlags defaultTeam = 0;
	switch ( _team )
	{
		case Q4_TEAM_MARINE:
			return NAVFLAGS_TEAM1_ONLY;
		case Q4_TEAM_STROGG:
			return NAVFLAGS_TEAM2_ONLY;
		default:
			return defaultTeam;
	}
}

void Q4_Client::GetNavFlags( NavFlags & includeFlags, NavFlags & excludeFlags )
{
	includeFlags = NAVFLAGS_WALK | NAVFLAGS_CROUCH;

	switch ( GetTeam() )
	{
		case Q4_TEAM_MARINE:
			excludeFlags = (NavFlags)( ~NAVFLAGS_TEAM1_ONLY & NAVFLAGS_ALLTEAMS );
			break;
		case Q4_TEAM_STROGG:
			excludeFlags = (NavFlags)( ~NAVFLAGS_TEAM2_ONLY & NAVFLAGS_ALLTEAMS );
			break;
	}
}

float Q4_Client::GetGameVar( GameVar _var ) const
{
	switch ( _var )
	{
		case JumpGapOffset:
			return 0.0f;
	}
	return 0.0f;
}

bool Q4_Client::DoesBotHaveFlag( MapGoalPtr _mapgoal )
{
	int iTeamFlags = 0;
	switch ( GetTeam() )
	{
		case Q4_TEAM_MARINE:
			iTeamFlags = Q4_PWR_STROGGFLAG;
			break;
		case Q4_TEAM_STROGG:
			iTeamFlags = Q4_PWR_MARINEFLAG;
			break;
	}

	if ( HasPowerup( iTeamFlags ) || HasPowerup( Q4_PWR_ONEFLAG ) || HasPowerup( Q4_PWR_DEADZONE ) )
		return true;
	return false;
}

void Q4_Client::SetupBehaviorTree()
{
	using namespace AiState;
	//FINDSTATEIF(HighLevel,GetStateRoot(),AppendState(new Deadzone))
}