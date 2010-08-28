////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "PrecompCommon.h"
#include "Waypoint.h"
#include "NavigationFlags.h"

extern float g_fTopWaypointOffset;
extern float g_fBottomWaypointOffset;
extern float g_fTopPathOffset;
extern float g_fBottomPathOffset;
extern float g_fBlockablePathOffset;
extern float g_fFacingOffset;
extern float g_fWaypointTextOffset;
extern float g_fWaypointTextDuration;

obuint32 Waypoint::m_NextUID = 1;
bool g_DrawLines = false;

Waypoint::Waypoint()
{
	Reset();
}

Waypoint::Waypoint(const Vector3f &_pos, float _radius, const Vector3f &_face)
{
	Reset();
	m_Position = _pos;
	m_Radius = _radius;
	m_Facing = _face;
}

Waypoint::~Waypoint()
{	
}

void Waypoint::Reset()
{
	m_Radius = 100.0f;
	m_NavigationFlags = 0;
	m_ConnectionFlags = 0;
	m_Position = Vector3f::ZERO;
	m_Facing = Vector3f::ZERO;
	m_UID = 0;
	m_HeuristicCost = 0.0f;
	m_GivenCost = 0.0f;
	m_FinalCost = 0.0f;
	m_Parent = 0;
	m_Mark = 0;
	m_GoalIndex = 0;
	m_Locked = false;

	m_OnPathThrough = 0;
	m_OnPathThroughParam = 0;
}

Segment3f Waypoint::GetSegment()
{
	Vector3f vWpBottom = GetPosition() + Vector3f(0,0,g_fBottomWaypointOffset);
	return Segment3f(vWpBottom, Vector3f::UNIT_Z, g_fTopWaypointOffset-g_fBottomWaypointOffset);
}

void Waypoint::AssignNewUID()
{
	m_UID = m_NextUID++;
}

const Vector3f &Waypoint::GetPosition() const 
{ 
	return m_Position; 
}

const Vector3f &Waypoint::GetFacing() const 
{ 
	return m_Facing; 
}

bool Waypoint::IsConnectedTo(const Waypoint *_wp) const
{
	ConnectionList::const_iterator cIt = m_Connections.begin(), cItEnd = m_Connections.end();
	for(; cIt != cItEnd; ++cIt)
	{
		if((*cIt).m_Connection == _wp)
			return true;
	}
	return false;
}

bool Waypoint::ConnectTo(Waypoint *_wp, obuint32 _flags)
{
	if(_wp)
	{
		Waypoint::ConnectionList::iterator it = m_Connections.begin();
		for( ; it != m_Connections.end(); ++it)
		{
			if(it->m_Connection == _wp)
				return false;
		}

		Waypoint::ConnectionInfo info;
		info.m_Connection = _wp;
		info.m_ConnectionFlags = _flags;
		m_Connections.push_back(info);
		return true;
	}
	return false;
}

void Waypoint::DisconnectFrom(const Waypoint *_wp)
{
	ConnectionList::iterator it = m_Connections.begin(), itEnd = m_Connections.end();
	for(; it != itEnd; )
	{
		if((*it).m_Connection == _wp)
			m_Connections.erase(it++);
		else
			++it;
	}
}

void Waypoint::PostLoad()
{
	m_OnPathThrough = 0;
	m_OnPathThroughParam = 0;

	String s = GetPropertyMap().GetProperty("paththrough");
	if(s.size()>1)
	{
		StringVector sv;
		Utils::Tokenize(s," :",sv);
		if(sv.size()>1)
			m_OnPathThroughParam = Utils::MakeHash32(sv[1]); 
		if(!sv.empty())
			m_OnPathThrough = Utils::MakeHash32(sv[0]);
	}
}
