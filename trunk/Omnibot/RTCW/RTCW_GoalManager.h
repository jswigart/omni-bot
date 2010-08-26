////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: drevil $
// $LastChangedDate: 2009-02-08 23:38:11 -0800 (Sun, 08 Feb 2009) $
// $LastChangedRevision: 3917 $
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __RTCW_GOALMANAGER_H__
#define __RTCW_GOALMANAGER_H__

#include "GoalManager.h"

// class: RTCW_GoalManager
//		The goal manager is responsible for keeping track of various goals,
//		from flags to capture points. Bots can request goals from the goal 
//		manager and the goal manager can assign goals to the bot based on
//		the needs of the game, and optionally the bot properties
class RTCW_GoalManager : public GoalManager
{
public:

	friend class RTCW_Game;

	void CheckWaypointForGoal(Waypoint *_wp, BitFlag64 _used = BitFlag64());

protected:

	RTCW_GoalManager();
	virtual ~RTCW_GoalManager();
};

#endif
