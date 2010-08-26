////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy: DrEvil $
// $LastChangedDate: 2006-11-26 17:46:01 -0800 (Sun, 26 Nov 2006) $
// $LastChangedRevision: 1340 $
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __WAYPOINTSERIALIZER_V3_H__
#define __WAYPOINTSERIALIZER_V3_H__

#include "WaypointSerializerImp.h"

// class: WaypointSerializer_V3
//		Implementation for waypoint file format version 3
//		File format specifications:
//		- Waypoint Header
//			+ has file version *(1 byte)*
//			+ has number of waypoints *(4 bytes)*
//			+ has map name *(32 bytes)*
//			+ has author comments *(256 bytes)*
//		- For _n_ waypoints
//			+ write the vector3 position out *(4 bytes x 3)* (v1)
//			+ write (__int64) the waypoint flags *(8 bytes)* (v1)
//			+ write (unsigned char) representing number of connections this wp has *(1 byte)* (v1)
//			- for _n_ connections
//				+ write (unsigned int) numeric index of waypoint for each connection *(4 bytes)* (v1)
//				+ write (unsigned int) connection flags for this connection *(4 bytes)* (v1)
//			+ write the radius of the waypoint *(4 bytes)* (v2)
//			+ write the waypoint orientation *(4 byte)* (v2)
class WaypointSerializer_V3 : public WaypointSerializerImp
{
public:

	WaypointSerializer_V3() {};
	virtual ~WaypointSerializer_V3() {};
protected:
	virtual bool Load(File &_file, PathPlannerWaypoint::WaypointList &_wpl);
	virtual bool Save(File &_file, PathPlannerWaypoint::WaypointList &_wpl);

};

#endif
