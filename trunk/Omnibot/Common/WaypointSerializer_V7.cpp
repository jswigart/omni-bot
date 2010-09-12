////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "PrecompCommon.h"
#include "WaypointSerializer_V7.h"
#include "Waypoint.h"

// temp
#include "gmMachine.h"
#include "gmUtilityLib.h"

bool WaypointSerializer_V7::Load(File &_file, PathPlannerWaypoint::WaypointList &_wpl)
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

		// Read the position in.
		CHECK_READ(_file.Read(&pCurrentWp->m_Position, sizeof(pCurrentWp->m_Position)));
		CHECK_READ(_file.ReadInt64(pCurrentWp->m_NavigationFlags));
		CHECK_READ(_file.ReadString(pCurrentWp->m_WaypointName));

		// disable some deprecated stuff.
		const NavFlags F_NAV_BLOCKABLE	= (NavFlags(1)<<17); // deprecated
		const NavFlags F_NAV_MOVABLE	= (NavFlags(1)<<23); // deprecated
		pCurrentWp->m_NavigationFlags &= ~F_NAV_MOVABLE;
		pCurrentWp->m_NavigationFlags &= ~F_NAV_BLOCKABLE;
		
		obuint8 iNumProperties = 0;
		CHECK_READ(_file.ReadInt8(iNumProperties));
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

		//pCurrentWp->GetPropertyMap()->GetProperty("");

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
		{ obuint32 tmp; CHECK_READ(_file.ReadInt32(tmp)); }

		pCurrentWp->m_Facing.Normalize();

		//////////////////////////////////////////////////////////////////////////
		// Update the next guid
		if(Waypoint::m_NextUID <= pCurrentWp->m_UID)
			Waypoint::m_NextUID = pCurrentWp->m_UID+1;

		// Give it a new UID
		if(pCurrentWp->GetUID() == 0)
			pCurrentWp->AssignNewUID();
		//////////////////////////////////////////////////////////////////////////

		pCurrentWp->PostLoad();

		_wpl[i] = pCurrentWp;
	}

	// Connect the waypoints.
	for(obuint32 i = 0; i < _wpl.size(); ++i)
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

PathPlannerWaypoint *GetWpPlanner();
bool WaypointSerializer_V7::Save(File &_file, PathPlannerWaypoint::WaypointList &_wpl)
{
	// Save the next GUID
	CHECK_WRITE(_file.WriteInt32(Waypoint::m_NextUID));

	for(obuint32 i = 0; i < _wpl.size(); ++i)
	{
		Waypoint *pCurrentWp = _wpl[i];

		//////////////////////////////////////////////////////////////////////////
		// Write out the position.
		CHECK_WRITE(_file.Write(&pCurrentWp->m_Position, sizeof(pCurrentWp->m_Position)));
		CHECK_WRITE(_file.WriteInt64(pCurrentWp->m_NavigationFlags));
		CHECK_WRITE(_file.WriteString(pCurrentWp->m_WaypointName));
		
		// Write the number of properties.
		if(pCurrentWp->GetPropertyMap().GetProperties().size() > std::numeric_limits<obuint8>::max())
		{
			LOGERR("> 256 Properties on waypoint");
			return false;
		}
		CHECK_WRITE(_file.WriteInt8((obuint8)pCurrentWp->GetPropertyMap().GetProperties().size()));
		// Write each property.
		PropertyMap::ValueMap::const_iterator pIt = pCurrentWp->GetPropertyMap().GetProperties().begin();
		for( ; pIt != pCurrentWp->GetPropertyMap().GetProperties().end(); ++pIt)
		{
			const String &strName = pIt->first;
			const String &strData = pIt->second;
			
			// Write the string length, and string
			if(strName.length() > std::numeric_limits<obuint8>::max())
			{
				LOGERR("> 256 characters in property name");
				return false;
			}
			obuint8 iPropNameLength = (obuint8)strName.length();
			CHECK_WRITE(_file.WriteInt8(iPropNameLength));
			CHECK_WRITE(_file.Write(strName.c_str(), sizeof(obuint8) * iPropNameLength));
			
			// Write the data string length, and string
			if(strData.length() > std::numeric_limits<obuint8>::max())
			{
				LOGERR("> 256 characters in property name");
				return false;
			}
			obuint8 iPropValueLength = static_cast<obuint8>(strData.length());
			CHECK_WRITE(_file.WriteInt8(iPropValueLength));
			CHECK_WRITE(_file.Write(strData.c_str(), sizeof(obuint8) * iPropValueLength));
		}

		if(pCurrentWp->m_Connections.size() > std::numeric_limits<obuint8>::max())
			return false;

		// Write the number of connections.
		obuint8 iNumConnections = 0;

		// count the valid connections.
		Waypoint::ConnectionList::iterator it = pCurrentWp->m_Connections.begin();
		for(;it != pCurrentWp->m_Connections.end(); ++it)
		{
			if(it->m_ConnectionFlags & F_LNK_DONTSAVE)
				continue;
			++iNumConnections;
		}
		
		// Write out the number of connections(unsigned char 1 byte, shouldn't be more than 255 connections)
		CHECK_WRITE(_file.WriteInt8(iNumConnections));

		// Write the connections out as the numeric index into the vector
		it = pCurrentWp->m_Connections.begin();
		for(;it != pCurrentWp->m_Connections.end(); ++it)
		{
			bool bFound = false;

			if(it->m_ConnectionFlags & F_LNK_DONTSAVE)
				continue;

			// Look for this waypoint in the vector so we can get the index into the vector.
			for(obuint32 w = 0; w < _wpl.size(); ++w)
			{
				if(_wpl[w] == it->m_Connection)
				{
					bFound = true;
					// Write the index
					CHECK_WRITE(_file.WriteInt32(w));
					// Write the connection flags.
					CHECK_WRITE(_file.WriteInt32(it->m_ConnectionFlags));
					break;
				}
			}
			
			if(!bFound)
				return false;
		}

		CHECK_WRITE(_file.WriteFloat(pCurrentWp->m_Radius));
		CHECK_WRITE(_file.Write(&pCurrentWp->m_Facing, sizeof(pCurrentWp->m_Facing)));
		CHECK_WRITE(_file.WriteInt32(pCurrentWp->m_UID));
		CHECK_WRITE(_file.WriteInt32(0));
	}

	/*pM->ExecuteString("Navigation.TestFunc = function(p) { print(p); };\nNavigation.TestFunc2 = function(p) { print(p); };");
	gmUtility::DumpTable(pM,"testwp.gm","Navigation",gmUtility::DUMP_ALL);
	delete pM;*/

	return true;
}
