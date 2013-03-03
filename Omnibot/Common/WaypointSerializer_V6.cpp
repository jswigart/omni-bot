////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "WaypointSerializer_V6.h"
#include "Waypoint.h"
#include "FileSystem.h"

bool WaypointSerializer_V6::Load(File &_file, PathPlannerWaypoint::WaypointList &_wpl)
{
	// temporary structure for Reading in the connection indices since we
	// can't set up the correct pointers till after the waypoints have all
	// been Read in and created.
	
	typedef std::multimap<obuint32, WaypointConnection> WaypointConnections;
	WaypointConnections connections;

	Waypoint *pCurrentWp = NULL;

	// Save the next GUID
	CHECK_READ(_file.ReadInt32(Waypoint::m_NextUID));	

	obuint32 iSize = (obuint32)_wpl.size();
	for(obuint32 i = 0; i < iSize; ++i)
	{
		pCurrentWp = new Waypoint;

		CHECK_READ(_file.Read(&pCurrentWp->m_Position, sizeof(pCurrentWp->m_Position)));
		CHECK_READ(_file.ReadInt64(pCurrentWp->m_NavigationFlags));
		CHECK_READ(_file.ReadString(pCurrentWp->m_WaypointName));
		
		// Read the number of properties.
		obuint8 iNumProperties = 0;
		CHECK_READ(_file.ReadInt8(iNumProperties));
		// Read each property.
		for(obuint8 p = 0; p < iNumProperties; ++p)
		{
			char strname[512] = {0};
			char strvalue[512] = {0};

			// Read the string length, and string
			obuint8 iPropNameLength = 0;
			CHECK_READ(_file.ReadInt8(iPropNameLength));
			OBASSERT(iPropNameLength > 0, "Invalid Name Length");
			CHECK_READ(_file.Read(strname, sizeof(char) * iPropNameLength));
			
			obuint8 iPropValueLength = 0;
			CHECK_READ(_file.ReadInt8(iPropValueLength));
			OBASSERT(iPropValueLength > 0, "Invalid Property Number");
			CHECK_READ(_file.Read(strvalue, sizeof(char) * iPropValueLength));
			pCurrentWp->GetPropertyMap().AddProperty(strname, strvalue);
		}

		// Read the number of connections
		obuint8 iNumConnections = 0;
		CHECK_READ(_file.ReadInt8(iNumConnections));

		obuint32 index = 0, flags = 0;
		for(int w = 0; w < iNumConnections; ++w)
		{
			CHECK_READ(_file.ReadInt32(index));
			CHECK_READ(_file.ReadInt32(flags));
			flags = 0; // deprecated
			WaypointConnection conn = { index, flags };
			connections.insert(std::pair<obuint32, WaypointConnection>(i, conn));
		}

		CHECK_READ(_file.ReadFloat(pCurrentWp->m_Radius));
		CHECK_READ(_file.Read(&pCurrentWp->m_Facing, sizeof(pCurrentWp->m_Facing)));
		CHECK_READ(_file.ReadInt32(pCurrentWp->m_UID));

		//////////////////////////////////////////////////////////////////////////
		// Update the next guid
		if(Waypoint::m_NextUID <= pCurrentWp->m_UID)
			Waypoint::m_NextUID = pCurrentWp->m_UID+1;

		// Give it a new UID
		if(pCurrentWp->GetUID() == 0)
			pCurrentWp->AssignNewUID();
		//////////////////////////////////////////////////////////////////////////

		_wpl[i] = pCurrentWp;
	}

	// Connect the waypoints.
	for(unsigned int i = 0; i < _wpl.size(); ++i)
	{
		WaypointConnections::iterator it;
		for (it = connections.lower_bound(i); 
			it != connections.upper_bound(i); 
			++it) 
		{
			Waypoint::ConnectionInfo conn = { _wpl[it->second.m_Index], it->second.m_ConnectionFlags };
			if(it->second.m_Index < _wpl.size())
				_wpl[i]->m_Connections.push_back(conn);
			else 
				return false;
		}
	}
	return true;
}

bool WaypointSerializer_V6::Save(File &_file, PathPlannerWaypoint::WaypointList &_wpl)
{
	OBASSERT(0, "This Shouldn't get called!");
	return false;

	// Save the next GUID
	//CHECK_WRITE(_file.Write((char *)&Waypoint::m_NextUID, sizeof(Waypoint::m_NextUID)));

	//for(unsigned int i = 0; i < _wpl.size(); ++i)
	//{
	//	Waypoint *pCurrentWp = _wpl[i];

	//	// Write out the position.
	//	CHECK_WRITE(_file.Write((char *)&pCurrentWp->m_Position, sizeof(pCurrentWp->m_Position)));

	//	// Write the waypoint flags
	//	CHECK_WRITE(_file.Write((char *)&pCurrentWp->m_NavigationFlags, sizeof(pCurrentWp->m_NavigationFlags)));

	//	// Write the waypoint name
	//	unsigned int iNameLength = pCurrentWp->m_WaypointName.length();
	//	CHECK_WRITE(_file.Write((char *)&iNameLength, sizeof(iNameLength)));
	//	if(iNameLength > 0 && iNameLength < std::numeric_limits<unsigned int>::max())
	//		CHECK_WRITE(_file.Write((char *)pCurrentWp->m_WaypointName.c_str(), sizeof(char) * iNameLength));

	//	// Write the number of properties.
	//	if(pCurrentWp->m_PropertyList.size() > std::numeric_limits<unsigned char>::max())
	//	{
	//		LOGERR("> 256 Properties on waypoint");
	//		return false;
	//	}
	//	unsigned char iNumProperties = (unsigned char)pCurrentWp->m_PropertyList.size();
	//	CHECK_WRITE(_file.Write((char *)&iNumProperties, sizeof(iNumProperties)));
	//	// Write each property.
	//	PropertyMap::const_iterator pIt = pCurrentWp->m_PropertyList.begin();
	//	for( ; pIt != pCurrentWp->m_PropertyList.end(); ++pIt)
	//	{
	//		const String &strName = pIt->first;
	//		const String &strData = pIt->second;
	//		
	//		// Write the string length, and string
	//		if(strName.length() > std::numeric_limits<unsigned char>::max())
	//		{
	//			LOGERR("> 256 characters in property name");
	//			return false;
	//		}
	//		unsigned char iPropNameLength = (unsigned char)strName.length();
	//		CHECK_WRITE(_file.Write((char *)&iPropNameLength, sizeof(iPropNameLength)));
	//		CHECK_WRITE(_file.Write((char *)strName.c_str(), sizeof(char) * iPropNameLength));
	//		
	//		// Write the data string length, and string
	//		if(strData.length() > std::numeric_limits<unsigned char>::max())
	//		{
	//			LOGERR("> 256 characters in property name");
	//			return false;
	//		}
	//		unsigned char iPropValueLength = static_cast<unsigned char>(strData.length());
	//		CHECK_WRITE(_file.Write((char *)&iPropValueLength, sizeof(iPropValueLength)));
	//		CHECK_WRITE(_file.Write((char *)strData.c_str(), sizeof(char) * iPropValueLength));
	//	}

	//	// Write the number of connections.
	//	unsigned int iNumConnections = (unsigned int)pCurrentWp->m_Connections.size();
	//	if(iNumConnections > std::numeric_limits<unsigned char>::max())
	//		return false;

	//	unsigned char iNumOutput = (unsigned char)iNumConnections;
	//	
	//	// Write out the number of connections(unsigned char 1 byte, shouldn't be more than 255 connections)
	//	CHECK_WRITE(_file.Write((char *)&iNumOutput, sizeof(iNumOutput)));

	//	// Write the connections out as the numeric index into the vector
	//	Waypoint::ConnectionList::iterator it = pCurrentWp->m_Connections.begin();
	//	while(it != pCurrentWp->m_Connections.end())
	//	{
	//		bool bFound = false;

	//		// Look for this waypoint in the vector so we can get the index into the vector.
	//		for(unsigned int w = 0; w < _wpl.size(); ++w)
	//		{
	//			if(_wpl[w] == it->m_Connection)
	//			{
	//				bFound = true;
	//				// Write the index
	//				CHECK_WRITE(_file.Write((char *)&w, sizeof(w)));
	//				// Write the connection flags.
	//				CHECK_WRITE(_file.Write((char *)&it->m_ConnectionFlags, sizeof(it->m_ConnectionFlags)));
	//				break;
	//			}
	//		}
	//		
	//		if(!bFound)
	//			return false;

	//		++it;
	//	}

	//	// Radius
	//	CHECK_WRITE(_file.Write((char *)&pCurrentWp->m_Radius, sizeof(pCurrentWp->m_Radius)));

	//	// Orientation
	//	CHECK_WRITE(_file.Write((char *)&pCurrentWp->m_Facing, sizeof(pCurrentWp->m_Facing)));

	//	// Write out the waypoint GUID
	//	CHECK_WRITE(_file.Write((char *)&pCurrentWp->m_UID, sizeof(pCurrentWp->m_UID)));
	//}
	//return true;
}
