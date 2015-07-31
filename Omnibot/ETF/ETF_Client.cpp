////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "ETF_Client.h"
#include "ETF_Messages.h"
#include "TF_BaseStates.h"
#include "FilterClosestETF.h"
#include "BotWeaponSystem.h"
#include "BotTargetingSystem.h"

#include "IGame.h"
#include "IGameManager.h"

//////////////////////////////////////////////////////////////////////////

ETF_Client::ETF_Client()
{
	mStepHeight = 8.0f; // subtract a small value as a buffer to jump
}

ETF_Client::~ETF_Client()
{
}

void ETF_Client::Init( int _gameid )
{
	//NOTENOTE: PURPOSELY BYPASSING TF_Client::Init
	Client::Init( _gameid );

	// We want to use a custom targeting filter.
	FilterPtr filter( new FilterClosestETF( this, AiState::SensoryMemory::EntEnemy ) );
	filter->AddCategory( ENT_CAT_SHOOTABLE );
	GetTargetingSystem()->SetDefaultTargetingFilter( filter );

	/*using namespace AiState;
	FINDSTATE(ws,WeaponSystem,GetStateRoot());
	if(ws)
	{
	ws->AddWeaponToInventory(TF_WP_GRENADE1);
	ws->AddWeaponToInventory(TF_WP_GRENADE2);
	}*/
}

NavFlags ETF_Client::GetTeamFlag( int _team ) const
{
	static const NavFlags defaultTeam = 0;
	switch ( _team )
	{
		case ETF_TEAM_RED:
			return NAVFLAGS_TEAM1_ONLY;
		case ETF_TEAM_BLUE:
			return NAVFLAGS_TEAM2_ONLY;
		case ETF_TEAM_YELLOW:
			return NAVFLAGS_TEAM3_ONLY;
		case ETF_TEAM_GREEN:
			return NAVFLAGS_TEAM4_ONLY;
		default:
			return defaultTeam;
	}
}

void ETF_Client::GetNavFlags( NavFlags & includeFlags, NavFlags & excludeFlags )
{
	includeFlags = NAVFLAGS_WALK | NAVFLAGS_CROUCH;

	switch ( GetTeam() )
	{
		case ETF_TEAM_RED:
			excludeFlags = (NavFlags)( ~NAVFLAGS_TEAM1_ONLY & NAVFLAGS_ALLTEAMS );
			break;
		case ETF_TEAM_BLUE:
			excludeFlags = (NavFlags)( ~NAVFLAGS_TEAM2_ONLY & NAVFLAGS_ALLTEAMS );
			break;
		case ETF_TEAM_YELLOW:
			excludeFlags = (NavFlags)( ~NAVFLAGS_TEAM3_ONLY & NAVFLAGS_ALLTEAMS );
			break;
		case ETF_TEAM_GREEN:
			excludeFlags = (NavFlags)( ~NAVFLAGS_TEAM4_ONLY & NAVFLAGS_ALLTEAMS );
			break;
	}
}

//float ETF_Client::NavCallback(const NavFlags &_flag, Waypoint *from, Waypoint *to)
//{
//	using namespace AiState;
//
//	WeaponSystem *wsys = GetWeaponSystem();
//	if(_flag & F_TF_NAV_ROCKETJUMP)
//	{
//		if(wsys->HasWeapon(TF_Options::ROCKETJUMP_WPN) && wsys->HasAmmo(TF_Options::ROCKETJUMP_WPN))
//			return 1.f;
//	}
//
//	if(_flag & F_TF_NAV_CONCJUMP)
//	{
//		int iCls = GetClass(), iGren2s = 0, iGren2sMax = 0;
//		gEngineFuncs->GetCurrentAmmo(GetGameEntity(), TF_WP_GRENADE2, Primary, iGren2s, iGren2sMax);
//		if((iCls == TF_CLASS_SCOUT || iCls == TF_CLASS_MEDIC) && iGren2s)
//			return 1.f;
//	}
//
//	return 0.f;
//}
