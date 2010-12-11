////////////////////////////////////////////////////////////////////////////////
// 
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __WAYPOINT_H__
#define __WAYPOINT_H__

#include "NavigationFlags.h"

// class: Waypoint
//		3d waypoint within the game for the waypoint path planner.
//		Represents a point in the game where
//		the bot can travel. Also holds connections to other waypoints to ultimately
//		create a large graph of waypoints that can be used in A* to find specific goals.
//		Can also hold flags or designators to give the waypoint additional properties 
//		that the A* search should take into account.
class Waypoint
{
public:
	friend class PathPlannerWaypoint;
	friend class WaypointSerializer_V1;
	friend class WaypointSerializer_V2;
	friend class WaypointSerializer_V3;
	friend class WaypointSerializer_V4;
	friend class WaypointSerializer_V5;
	friend class WaypointSerializer_V6;
	friend class WaypointSerializer_V7;

	// typedef: ConnectionInfo
	//	Represents a connection to another waypoint.
	//	Contains a pointer to the other waypoint, as well as a
	//	variable to hold flags for the connection.
	typedef struct {
		Waypoint		*m_Connection;
		obuint32		m_ConnectionFlags;
		bool CheckFlag(unsigned int _flag) const { return CheckBitT<unsigned int>(m_ConnectionFlags, _flag); }
		void SetFlag(unsigned int _flag) { SetBitT<unsigned int>(m_ConnectionFlags, _flag); }
		void ClearFlag(unsigned int _flag) { ClearBitT<unsigned int>(m_ConnectionFlags, _flag); }
	} ConnectionInfo;
	typedef std::list<ConnectionInfo> ConnectionList;
	
	PropertyMap &GetPropertyMap() { return m_PropertyList; }
	const PropertyMap &GetPropertyMap() const { return m_PropertyList; }
	
	// Accessors and modifiers
	const Vector3f &GetPosition() const;
	const Vector3f &GetFacing() const;

	inline const NavFlags &GetNavigationFlags() const { return m_NavigationFlags; }

	inline void AddFlag(const NavFlags _flag) { m_NavigationFlags |= _flag; m_NeedsSynced = true; }
	inline void RemoveFlag(const NavFlags _flag) { m_NavigationFlags &= ~_flag; m_NeedsSynced = true; }
	inline void ClearFlags() { m_NavigationFlags = 0; m_NeedsSynced = true; }
	inline bool IsFlagOn(const NavFlags _flag) const { return (m_NavigationFlags & _flag) ? true : false; }
	inline bool IsAnyFlagOn(const NavFlags _flag) const { return (m_NavigationFlags & _flag) ? true : false; }
	inline void SetPosition( const Vector3f & v ) { m_Position = v; m_NeedsSynced = true; }
	inline float GetRadius() const { return m_Radius; }
	inline void SetRadius(float _rad) { m_Radius = _rad; m_NeedsSynced = true; }

	inline void SetName( const String & s ) { m_WaypointName = s; m_NeedsSynced = true; }
	inline const String &GetName() const { return m_WaypointName; }
	inline int GetUID() const { return m_UID; }
	inline int GetHash() const { return (unsigned int)this; }
	inline void SetEntity(GameEntity _ent) { m_Entity = _ent; }

	bool IsConnectedTo(const Waypoint *_wp) const;
	bool ConnectTo(Waypoint *_wp, obuint32 _flags = 0);
	void DisconnectFrom(const Waypoint *_wp);

	void AssignNewUID();

	friend bool waypoint_comp(const Waypoint* _wp1, const Waypoint* _wp2);

	void Reset();

	Segment3f GetSegment();

	obuint32 OnPathThrough() const { return m_OnPathThrough; }
	obuint32 OnPathThroughParam() const { return m_OnPathThroughParam; }

	void PostLoad();

#ifdef ENABLE_REMOTE_DEBUGGING
	void Sync( RemoteLib::DataBuffer & db, bool fullSync );
#endif

	Waypoint();
	Waypoint(const Vector3f &_pos, float _radius, const Vector3f &_face = Vector3f::ZERO);
	virtual ~Waypoint();
protected:

	// For path queries, first for caching.
	Waypoint			*m_Parent;
	float				m_GivenCost;
	float				m_FinalCost;
	float				m_HeuristicCost;
	NavFlags			m_ConnectionFlags;
	int					m_GoalIndex;

	// Waypoint info.
	NavFlags			m_NavigationFlags;
	Vector3f			m_Position;
	Vector3f			m_Facing;
	float				m_Radius;
	obuint32			m_Mark;	
	obuint32			m_UID;
	GameEntity			m_Entity;

	obuint32			m_OnPathThrough;
	obuint32			m_OnPathThroughParam;

	// Other
	String				m_WaypointName;
	ConnectionList		m_Connections;
	PropertyMap			m_PropertyList;

	bool				m_Locked : 1;

	bool				m_NeedsSynced : 1;

	static obuint32		m_NextUID;
private:
};

typedef boost::shared_ptr<Waypoint> WaypointPtr;

#endif