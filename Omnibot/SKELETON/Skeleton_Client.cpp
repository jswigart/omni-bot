////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "Skeleton_Client.h"

#include "IGame.h"
#include "IGameManager.h"

Skeleton_Client::Skeleton_Client()
{
}

Skeleton_Client::~Skeleton_Client()
{
}

NavFlags Skeleton_Client::GetTeamFlag(int _team) const
{
	static const NavFlags defaultTeam = 0;
	switch(_team)
	{
	case SKELETON_TEAM_1:
		return NAVFLAGS_TEAM1_ONLY;
	case SKELETON_TEAM_2:
		return NAVFLAGS_TEAM2_ONLY;
	default:
		return defaultTeam;
	}
}

void Skeleton_Client::GetNavFlags( NavFlags & includeFlags, NavFlags & excludeFlags )
{
	includeFlags = NAVFLAGS_WALK;

	switch ( GetTeam() )
	{
		case SKELETON_TEAM_1:
			excludeFlags = (NavFlags)( ~NAVFLAGS_TEAM1_ONLY & sTeamMask );
			break;
		case SKELETON_TEAM_2:
			excludeFlags = (NavFlags)( ~NAVFLAGS_TEAM2_ONLY & sTeamMask );
			break;
	}
}

float Skeleton_Client::GetGameVar(GameVar _var) const
{
	switch(_var)
	{
	case JumpGapOffset:
		return 0.0f;
	}
	return 0.0f;
}

bool Skeleton_Client::DoesBotHaveFlag(MapGoalPtr _mapgoal)
{
	return false;
}