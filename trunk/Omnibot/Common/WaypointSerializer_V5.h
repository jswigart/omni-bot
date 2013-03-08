////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __WAYPOINTSERIALIZER_V5_H__
#define __WAYPOINTSERIALIZER_V5_H__

#include "WaypointSerializerImp.h"

// class: WaypointSerializer_V5
//		Implementation for waypoint file format version 5
//		File format specifications:
//		- Waypoint Header
//			+ has file version *(1 byte)*
//			+ has number of waypoints *(4 bytes)*
//			+ has author comments *(288 bytes)*

//			+ next uid *(4 bytes)*

//		- For _n_ waypoints
//			+ write the vector3 position out *(4 bytes x 3)* (v1)
//			+ write (__int64) the waypoint flags *(8 bytes)* (v1)
//			+ write the waypoint name length *(4 bytes)* (v4)
//			+ write the waypoint name *(1 byte * length)* (v4)

//			+ write the number of custom properties *(1 byte)* (v4)
//			- for _n_ custom properties
//				+ write the property name length *(1 byte)* (v4)
//				+ write the property name *(1 byte * length)* (v4)
//				+ write a <BotUserData> struct *()*(v4)

//			+ write (unsigned char) representing number of connections this wp has *(1 byte)* (v1)
//			- for _n_ connections
//				+ write (unsigned int) numeric index of waypoint for each connection *(4 bytes)* (v1)
//				+ write (unsigned int) connection flags for this connection *(4 bytes)* (v1)
//			+ write the radius of the waypoint *(4 bytes)* (v2)
//			+ write the waypoint orientation *(4 byte)* (v2)
//			+ write the waypoint guid *(4 byte)* (v5)
class WaypointSerializer_V5 : public WaypointSerializerImp
{
public:

	WaypointSerializer_V5() {};
	virtual ~WaypointSerializer_V5() {};
protected:
	virtual bool Load(File &_file, PathPlannerWaypoint::WaypointList &_wpl);
	virtual bool Save(File &_file, PathPlannerWaypoint::WaypointList &_wpl);
};

#endif
