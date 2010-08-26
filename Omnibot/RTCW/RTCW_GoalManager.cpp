////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: drevil $
// $LastChangedDate: 2010-01-23 03:46:48 -0600 (Sat, 23 Jan 2010) $
// $LastChangedRevision: 4614 $
//
////////////////////////////////////////////////////////////////////////////////

#include "PrecompRTCW.h"
#include "RTCW_GoalManager.h"
#include "RTCW_NavigationFlags.h"

RTCW_GoalManager::RTCW_GoalManager()
{
	m_Instance = this;
}

RTCW_GoalManager::~RTCW_GoalManager()
{
	Shutdown();
}

void RTCW_GoalManager::CheckWaypointForGoal(Waypoint *_wp, BitFlag64 _used)
{
	enum { MaxGoals=32 };

	MapGoalDef Definition[MaxGoals];
	int NumDefs = 0;

	//////////////////////////////////////////////////////////////////////////

	if(_wp->IsFlagOn(F_RTCW_NAV_ARTSPOT))
	{
		/*MapGoalPtr goal(new RTCW_CallArtyGoal());
		newGoals.push_back(goal);*/

		Definition[NumDefs++].Props.SetString("Type","CALLARTILLERY");
	}
	if(_wp->IsFlagOn(F_RTCW_NAV_ARTYTARGET_S))
	{
		/*MapGoalPtr goal(new RTCW_CallArtyTargetGoal_Static());
		newGoals.push_back(goal);*/

		Definition[NumDefs++].Props.SetString("Type","ARTILLERY_S");
	}
	if(_wp->IsFlagOn(F_RTCW_NAV_ARTYTARGET_D))
	{
		/*MapGoalPtr goal(new RTCW_CallArtyTargetGoal_Dynamic());
		newGoals.push_back(goal);*/

		Definition[NumDefs++].Props.SetString("Type","ARTILLERY_D");
	}
	if(_wp->IsFlagOn(F_RTCW_NAV_CAPPOINT))
	{
		/*MapGoalPtr goal(new FlagCapGoal());
		newGoals.push_back(goal);*/

		Definition[NumDefs++].Props.SetString("Type","cappoint");
	}
	if(_wp->IsFlagOn(F_RTCW_NAV_PANZER))
	{
		/*MapGoalPtr goal(new RTCW_PanzerGoal());
		newGoals.push_back(goal);*/

		Definition[NumDefs++].Props.SetString("Type","PANZER");
	}
	if(_wp->IsFlagOn(F_RTCW_NAV_VENOM))
	{
		/*MapGoalPtr goal(new RTCW_VenomGoal());
		newGoals.push_back(goal);*/

		Definition[NumDefs++].Props.SetString("Type","VENOM");
	}
	if(_wp->IsFlagOn(F_RTCW_NAV_FLAMETHROWER))
	{
		/*MapGoalPtr goal(new RTCW_FlamethrowerGoal());
		newGoals.push_back(goal);*/

		Definition[NumDefs++].Props.SetString("Type","FLAME");
	}

//  version 0.7x feature, per gfs possibly auto create if wp has a name
	/*if(_wp->IsFlagOn(F_RTCW_NAV_USERGOAL))
	{
		MapGoalPtr goal(new RTCW_UserGoal());
		newGoals.push_back(goal);
	}*/

	RegisterWaypointGoals(_wp,Definition,NumDefs);
	
	// Allow the base class to process it.
	GoalManager::CheckWaypointForGoal(_wp, _used);
}
