////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: drevil $
// $LastChangedDate: 2009-02-11 09:14:36 -0800 (Wed, 11 Feb 2009) $
// $LastChangedRevision: 3934 $
//
////////////////////////////////////////////////////////////////////////////////

#include "PrecompJA.h"
#include "JA_GoalManager.h"
#include "JA_NavigationFlags.h"

JA_GoalManager::JA_GoalManager()
{
	m_Instance = this;
}

JA_GoalManager::~JA_GoalManager()
{
	Shutdown();
}

void JA_GoalManager::CheckWaypointForGoal(Waypoint *_wp, BitFlag64 _used)
{
	enum { MaxGoals=32 };

	MapGoalDef Definition[MaxGoals];
	int NumDefs = 0;

	//////////////////////////////////////////////////////////////////////////

	if(_wp->IsFlagOn(F_JA_NAV_CAPPOINT))
	{
		/*MapGoalPtr goal(new FlagCapGoal());
		newGoals.push_back(goal);*/

		Definition[NumDefs++].Props.SetString("Type","cappoint");
	}

	RegisterWaypointGoals(_wp,Definition,NumDefs);

	// Allow the base class to process it.
	GoalManager::CheckWaypointForGoal(_wp, _used);
}
