////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: drevil $
// $LastChangedDate: 2008-02-11 20:03:50 -0800 (Mon, 11 Feb 2008) $
// $LastChangedRevision: 2438 $
//
////////////////////////////////////////////////////////////////////////////////

#include "PrecompSkeleton.h"
#include "Skeleton_Client.h"
#include "Skeleton_NavigationFlags.h"

#include "IGame.h"
#include "IGameManager.h"

Skeleton_Client::Skeleton_Client()
{
}

Skeleton_Client::~Skeleton_Client()
{
}

NavFlags Skeleton_Client::GetTeamFlag()
{
	return GetTeamFlag(GetTeam());
}

NavFlags Skeleton_Client::GetTeamFlag(int _team)
{
	static const NavFlags defaultTeam = 0;
	switch(_team)
	{
	case SKELETON_TEAM_1:
		return F_NAV_TEAM1;
	case SKELETON_TEAM_2:
		return F_NAV_TEAM2;
	default:
		return defaultTeam;
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

float Skeleton_Client::GetAvoidRadius(int _class) const
{
	return 0.0f;
}

bool Skeleton_Client::DoesBotHaveFlag(MapGoalPtr _mapgoal)
{
	return false;
}
