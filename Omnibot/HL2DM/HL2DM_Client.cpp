////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "HL2DM_Client.h"

#include "IGame.h"
#include "IGameManager.h"

HL2DM_Client::HL2DM_Client()
{
}

HL2DM_Client::~HL2DM_Client()
{
}

NavFlags HL2DM_Client::GetTeamFlag(int _team) const
{
	static const NavFlags defaultTeam = 0;
	switch(_team)
	{
	case HL2DM_TEAM_COMBINE:
		return NAVFLAGS_TEAM1_ONLY;
	case HL2DM_TEAM_REBELS:
		return NAVFLAGS_TEAM2_ONLY;
	default:
		return defaultTeam;
	}
}

void HL2DM_Client::GetNavFlags( NavFlags & includeFlags, NavFlags & excludeFlags )
{
	includeFlags = NAVFLAGS_WALK | NAVFLAGS_CROUCH;

	switch ( GetTeam() )
	{
		case HL2DM_TEAM_COMBINE:
			excludeFlags = (NavFlags)( ~NAVFLAGS_TEAM1_ONLY & NAVFLAGS_ALLTEAMS );
			break;
		case HL2DM_TEAM_REBELS:
			excludeFlags = (NavFlags)( ~NAVFLAGS_TEAM2_ONLY & NAVFLAGS_ALLTEAMS );
			break;
	}
}

float HL2DM_Client::GetGameVar(GameVar _var) const
{
	switch(_var)
	{
	case JumpGapOffset:
		return 0.0f;
	}
	return 0.0f;
}

bool HL2DM_Client::DoesBotHaveFlag(MapGoalPtr _mapgoal)
{
	return false;
}
