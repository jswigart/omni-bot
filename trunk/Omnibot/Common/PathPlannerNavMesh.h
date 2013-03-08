////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __PATHPLANNERNAVMESH_H__
#define __PATHPLANNERNAVMESH_H__

#include <queue>

#include "PathPlannerBase.h"
#include "InternalFsm.h"
#include "SpanHeightMap.h"

#include "navmesh.pb.h"

typedef std::vector<Segment3f> SegmentList;

//////////////////////////////////////////////////////////////////////////

enum ToolStateNavMesh
{
	NoOp,
	PlaceSector,
	SliceSector,
	SliceSectorWithSector,
	EditSector,
	SplitSector,
	TraceSector,
	GroundSector,
	CommitSector,
	MirrorSectors,
	PlaceBorder,
	FloodSpanMap,
	NumToolStates
};

namespace NavigationMeshOptions
{
	extern float CharacterHeight;
};

namespace Opcode
{
	class Model;
	class MeshInterface;
}

namespace IceMaths
{
	class Point;
}

// class: PathPlannerNavMesh
//		Path planner interface for the navmesh system for hl2
class PathPlannerNavMesh : public PathPlannerBase, public InternalFSM<PathPlannerNavMesh,NumToolStates>
{
public:
	enum NavMeshFlags
	{
		NAVMESH_STEPPROCESS = NUM_BASE_NAVFLAGS,
		NAVMESH_TAKESTEP,
	};

	enum NodeDirection
	{
		NORTH,
		SOUTH,
		EAST,
		WEST,
		NUM_DIRS
	};

	//////////////////////////////////////////////////////////////////////////
	struct Obstacle
	{
		Vector3List	mPolyVerts;
	};
	typedef std::vector<Obstacle> ObstacleList;

	struct NavSector
	{
		int								m_Id;
		int								m_StartPortal;
		int								m_NumPortals;

		Vector3List						m_Boundary;
		NavmeshIO::Sector_MirrorDir		m_Mirror;

		Plane3f							m_Plane;

		ObstacleList					m_Obstacles;

		bool IsMirrored() const
		{
			return m_Mirror != NavmeshIO::Sector_MirrorDir_MirrorNone;
		}

		NavSector()
			: m_Id(0)
			, m_StartPortal(0)
			, m_NumPortals(0)
			, m_Mirror( NavmeshIO::Sector_MirrorDir_MirrorNone )
		{
		}

		Vector3f CalculateCenter() const;
		NavSector GetMirroredCopy(const Vector3f &offset) const;
		SegmentList GetEdgeSegments() const;
		void GetEdgeSegments(SegmentList &_list) const;
	};
	struct NavPortal
	{
		Segment3f		m_Segment;

		int				m_DestSector;
		int				m_DestPortal;

		BitFlag64		m_LinkFlags;
	};

	NavSector *GetSectorAt(const Vector3f &_pos, float _distance = 1024.f);

	bool Init();
	void Update();
	void Shutdown();
	bool IsReady() const;

	int GetLatestFileVersion() const { return 1; }

	Vector3f GetRandomDestination(Client *_client, const Vector3f &_start, const NavFlags _team);

	void PlanPathToGoal(Client *_client, const Vector3f &_start, const Vector3f &_goal, const NavFlags _team);
	int PlanPathToNearest(Client *_client, const Vector3f &_start, const Vector3List &_goals, const NavFlags &_team);
	int PlanPathToNearest(Client *_client, const Vector3f &_start, const DestinationVector &_goals, const NavFlags &_team);

	bool GetNavFlagByName(const std::string &_flagname, NavFlags &_flag) const;

	Vector3f GetDisplayPosition(const Vector3f &_pos);

	bool IsDone() const;
	bool FoundGoal() const;
	bool Load(const std::string &_mapname, bool _dl = true);
	bool Save(const std::string &_mapname);
	void Unload();
	bool SetFileComments(const std::string &_text);

	void RegisterGameGoals();
	void GetPath(Path &_path, int _smoothiterations);

	virtual void RegisterNavFlag(const std::string &_name, const NavFlags &_bits) {}

	void RegisterScriptFunctions(gmMachine *a_machine);

	bool GetNavInfo(const Vector3f &pos,obint32 &_id,std::string &_name);

	void AddEntityConnection(const Event_EntityConnection &_conn);
	void RemoveEntityConnection(GameEntity _ent);

	const char *GetPlannerName() const { return "Navigation Mesh Path Planner"; } ;
	int GetPlannerType() const { return NAVID_NAVMESH; };

	PathPlannerNavMesh();
	virtual ~PathPlannerNavMesh();
protected:

	void InitCommands();
	void cmdNavSave(const StringVector &_args);
	void cmdNavLoad(const StringVector &_args);
	void cmdNavView(const StringVector &_args);
	void cmdNavViewConnections(const StringVector &_args);
	void cmdNavStep(const StringVector &_args);
	void cmdNavEnableStep(const StringVector &_args);
	void cmdAutoBuildFeatures(const StringVector &_args);
	void cmdStartPoly(const StringVector &_args);
	void cmdUndoPoly(const StringVector &_args);
	/*void cmdLoadObj(const StringVector &_args);
	void cmdLoadMap(const StringVector &_args);*/
	void cmdCreatePlanePoly(const StringVector &_args);
	void cmdCreateSlicePoly(const StringVector &_args);
	void cmdCreateSliceSector(const StringVector &_args);
	void cmdCommitPoly(const StringVector &_args);
	void cmdDeleteSector(const StringVector &_args);
	void cmdMirrorSectors(const StringVector &_args);
	void cmdSectorSetProperty(const StringVector &_args);
	void cmdEditSector(const StringVector &_args);
	void cmdSplitSector(const StringVector &_args);
	void cmdGroundSector(const StringVector &_args);
	void cmdSectorCreateConnections(const StringVector &_args);
	void cmdSetMapCenter(const StringVector &_args);

	void cmdObstacleAdd(const StringVector &_args);

	void cmdInfluenceMapCreate(const StringVector &_args);
	void cmdInfluenceMapSeed(const StringVector &_args);
	void cmdInfluenceMapMem(const StringVector &_args);
	void cmdInfluenceMapSave(const StringVector &_args);
	void cmdInfluenceMapLoad(const StringVector &_args);
	void cmdInfluenceMapFlood(const StringVector &_args);

	void cmdNext(const StringVector &_args);

	//////////////////////////////////////////////////////////////////////////
	// Friend functions
	friend int GM_CDECL gmfNavMeshView(gmThread *a_thread);
	friend int GM_CDECL gmfNavMeshViewConnections(gmThread *a_thread);
	friend int GM_CDECL gmfNavMeshEnableStep(gmThread *a_thread);
	friend int GM_CDECL gmfNavMeshStep(gmThread *a_thread);

	//////////////////////////////////////////////////////////////////////////

	friend class PathFindNavMesh;
protected:

	//////////////////////////////////////////////////////////////////////////
	struct AttribFields
	{
		obuint32		Mirrored : 1;
		obuint32		ActiveId : 12;
		obuint32		SectorId : 12;
		obuint32		UnUsed : 7;
	};
	union PolyAttrib
	{
		obuint32		Attrib;
		AttribFields	Fields;

		PolyAttrib() : Attrib( 0 ) {}
	};
	//////////////////////////////////////////////////////////////////////////
	class NavCollision
	{
	public:

		bool DidHit() const { return m_HitSomething; }
		const PolyAttrib HitAttrib() const { return m_HitAttrib; }
		const Vector3f &HitPosition() const { return m_HitPosition; }
		const Vector3f &HitNormal() const { return m_HitNormal; }

		NavCollision(bool _hit, const Vector3f &_pos = Vector3f::ZERO, const Vector3f &_normal = Vector3f::ZERO, PolyAttrib _attrib = PolyAttrib() )
			: m_HitPosition(_pos)
			, m_HitNormal(_normal)
			, m_HitSomething(_hit)
			, m_HitAttrib( _attrib )
		{
		}
	private:
		Vector3f	m_HitPosition;
		Vector3f	m_HitNormal;
		obuint32	m_ActiveSectorId;
		PolyAttrib	m_HitAttrib;
		bool		m_HitSomething : 1;

		NavCollision();
	};

	NavCollision FindCollision(const Vector3f &_from, const Vector3f &_to);

	//////////////////////////////////////////////////////////////////////////
	Vector3f			m_MapCenter;

	typedef std::vector<NavSector> NavSectorList;
	NavSectorList		m_NavSectors;
	NavSectorList		m_ActiveNavSectors;

	typedef std::vector<NavPortal> NavPortalList;
	NavPortalList		m_NavPortals;
	//////////////////////////////////////////////////////////////////////////
	// Tool state machine
	STATE_PROTOTYPE(NoOp);
	STATE_PROTOTYPE(PlaceSector);
	STATE_PROTOTYPE(SliceSector);
	STATE_PROTOTYPE(SliceSectorWithSector);
	STATE_PROTOTYPE(EditSector);
	STATE_PROTOTYPE(SplitSector);
	STATE_PROTOTYPE(TraceSector);
	STATE_PROTOTYPE(GroundSector);
	STATE_PROTOTYPE(CommitSector);
	STATE_PROTOTYPE(MirrorSectors);
	STATE_PROTOTYPE(PlaceBorder);
	STATE_PROTOTYPE(FloodSpanMap);

	//////////////////////////////////////////////////////////////////////////
	// Current tool variables
	obColor				m_CursorColor;
	NavSector			m_WorkingSector;
	Vector3f			m_WorkingSectorStart;
	Vector3f			m_WorkingSectorNormal;

	Vector3List			m_WorkingManualSector;

	Plane3f				m_WorkingSectorPlane;
	Plane3f				m_WorkingSlicePlane;

	bool				m_ToolCancelled;

	typedef SpanHeightMap<obuint8> SpanMap;
	typedef std::queue<Vector3f> VectorQueue;

	SpanMap *				m_SpanMap;
	SpanMap::InfluenceMap *	m_Influence;
	VectorQueue				m_SpanFrontier;
	struct CollisionData
	{
		CollisionData()
			: m_CollisionTree( NULL )
			, m_MeshInterface( NULL )
			, m_Verts( NULL )
			, m_TriSectorMap( NULL )
		{
		}
		~CollisionData()
		{
			Free();
		}
		void Free();

		Opcode::Model *				m_CollisionTree;
		Opcode::MeshInterface *		m_MeshInterface;
		IceMaths::Point *			m_Verts;
		PolyAttrib *				m_TriSectorMap;
	}	m_CollisionData;

	obuint32	mInfluenceBufferId;

	void InitSectors();

	Vector3f _SectorVertWithin(const Vector3f &_pos1, const Vector3f &_pos2, float _range, bool *_snapped = NULL);

	//////////////////////////////////////////////////////////////////////////
	// Internal Implementations of base class functionality
	std::string _GetNavFileExtension() { return ".nav"; }
	virtual void _BenchmarkPathFinder(const StringVector &_args);
	virtual void _BenchmarkGetNavPoint(const StringVector &_args);
};

#endif
