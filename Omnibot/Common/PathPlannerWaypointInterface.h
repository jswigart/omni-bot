////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy: plastovicka $
// $LastChangedDate: 2012-07-14 19:38:48 -0500 (Sat, 14 Jul 2012) $
// $LastChangedRevision: 684 $
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __PATHPLANNERWAYPOINTINTERFACE_H__
#define __PATHPLANNERWAYPOINTINTERFACE_H__

class Waypoint;

class PathPlannerWaypointInterface
{
public:
	typedef enum
	{
		B_PATH_OPEN,
		B_PATH_CLOSED,
		B_INVALID_FLAGS
	} BlockableStatus;

	virtual BlockableStatus WaypointPathCheck(const Waypoint*, const Waypoint*, bool _draw) = 0;
};

#endif
