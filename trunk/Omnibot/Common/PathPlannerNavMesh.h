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

#include "Omni-Bot_BitFlags.h"
#include "PathPlannerBase.h"
#include "CollisionModel.h"

#include "navmesh.pb.h"

typedef std::vector<Segment3f> SegmentList;

//////////////////////////////////////////////////////////////////////////

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

template< typename T >
class EditTool
{
public:
	EditTool( const char * toolname ) : mToolName( toolname ) {}
	virtual ~EditTool() {}

	virtual bool Enter( T * system ) = 0;
	virtual bool ReEnter( T * system ) { return true; }
	virtual bool Update( T * system ) = 0;
	virtual void Commit( T * system ) = 0;
	virtual void Undo( T * system ) { EngineFuncs::ConsoleError( va( "No Undo functionality for tool %s",mToolName ) ); }
protected:
	std::string		mToolError;
	const char *	mToolName;
};

// class: PathPlannerNavMesh
//		Path planner interface for the navmesh system for hl2
class PathPlannerNavMesh : public PathPlannerBase
{
public:
	struct RuntimeNavSector;
	
	enum NavMeshFlags
	{
		NAVMESH_STEPPROCESS = NUM_BASE_NAVFLAGS,
		NAVMESH_TAKESTEP,
		NAVMESH_SHOWCOLLISION,
	};

	typedef std::vector<RuntimeNavSector*> RuntimeSectorRefs;
	typedef std::vector<const RuntimeNavSector*> RuntimeSectorRefsConst;

	struct Obstacle
	{
		obuint32				mHandle;

		GameEntity				mEntity;
		Vector3f				mPosition;

		Vector3List				mPoly;
		float					mCost; // NavPortal::MAX_COST is blocked
		
		obint32					mExpireTime;
		RuntimeSectorRefs		mAffectedSectors;

		bool operator==( const Obstacle & other )
		{
			return mHandle == other.mHandle;
		}

		bool Expired() const;

		Obstacle();
	};
	typedef std::vector<Obstacle> ObstacleList;
	
	struct NavPortal
	{
		Segment3f			mSegment;
		Segment3f			mSegmentLocal;

		RuntimeNavSector *  mSrcSector;
		RuntimeNavSector *  mDstSector;

		enum PortalFlags
		{
			FL_DISABLED,
			FL_JUMP,
		};

		BitFlag64		mFlags;

		static const float MAX_COST;

		NavPortal();
	};

	struct EdgePortals
	{
		obuint16				mPortalIndex;
		obuint16				mPortalCount;
	};

	typedef std::vector<NavPortal> NavPortalList;
	typedef std::vector<EdgePortals> NavEdgePortals;

	/////////////////////////////////////////////////////////
	struct NavSectorBase
	{
		Vector3List						mPoly;
		Plane3f							mPlane;

		Vector3f CalculateCenter() const;
		void GetMirroredCopy(const Vector3f & mirrorAxis,NavmeshIO::Sector_MirrorDir mirror,NavSectorBase & dest) const;
		void GetEdgeSegments(SegmentList &_list) const;
		void SetPointsToPlane();
	};

	struct NavSector : public NavSectorBase
	{
		NavmeshIO::Sector_MirrorDir		mMirror;
		NavmeshIO::SectorData			mSectorData;
		NavmeshIO::SectorData			mSectorDataMirrored;

		bool IsMirrored() const;

		// Utility functions
		NavSector();
	};

	typedef std::vector<NavSector> NavSectorList;
	typedef std::vector<RuntimeNavSector> RuntimeSectorList;

	struct RuntimeNavSector : public NavSectorBase
	{
		obuint32					mIndex;
		RuntimeNavSector *			mParent;

		Vector3List					mLocalPoly;

		NavSector *					mSector;

		float						mCost;

		// reference to the global sector data for this sector
		NavmeshIO::SectorData *		mSectorData;

		// currently active navigation portals
		NavPortalList				mPortals;

		// current obstacles influencing this sector
		ObstacleList				mObstacles;

		RuntimeSectorList			mSubSectors;
		
		obuint32					mVersion;

		Vector3f					mSectorWorldTrans;
		Matrix3f					mSectorWorldRot;

		bool						mMirrored;
		bool						mUpdatePortals;
		bool						mUpdateObstacles;

		bool						IsMirroredSector() const;

		void UpdateAutoFlags();

		// build portals for this sector
		void RebuildPortals( PathPlannerNavMesh * navmesh );
		void RebuildLedgePortals( PathPlannerNavMesh * navmesh );		

		// rebuild subsectors due to change in obstacles
		void RebuildObstacles( PathPlannerNavMesh * navmesh );

		void RenderPortals( const PathPlannerNavMesh * navmesh ) const;

		// movable sectors
		bool InitSectorTransform();
		void UpdateSectorTransform();

		obColor GetRenderColor() const;

		RuntimeNavSector( obuint32 index, NavSector * sector, NavmeshIO::SectorData * data );
		~RuntimeNavSector();
	private:
		RuntimeNavSector(); // not allowed
	};
	/////////////////////////////////////////////////////////

	RuntimeNavSector *GetSectorAt(const Vector3f &_pos, float _distance = 1024.f);

	bool Init( System & system );
	void Update( System & system );
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
	void GetPath(Path &_path);

	virtual void RegisterNavFlag(const std::string &_name, const NavFlags &_bits);
	virtual void RegisterScriptFunctions(gmMachine *a_machine);

	bool GetNavInfo(const Vector3f &pos,obint32 &_id,std::string &_name);

	void AddEntityConnection(const Event_EntityConnection &_conn);
	void RemoveEntityConnection(GameEntity _ent);
	
	void EntityCreated(const EntityInstance &ei);
	void EntityDeleted(const EntityInstance &ei);

	const char *GetPlannerName() const { return "Navigation Mesh Path Planner"; } ;
	int GetPlannerType() const { return NAVID_NAVMESH; };

	PathInterface * AllocPathInterface();

	PathPlannerNavMesh();
	virtual ~PathPlannerNavMesh();
protected:
	void InitCommands();
	void cmdNavSave(const StringVector &_args);
	void cmdNavLoad(const StringVector &_args);
	void cmdNavView(const StringVector &_args);
	void cmdNavViewConnections(const StringVector &_args);
	void cmdNavViewFlags(const StringVector &_args);
	void cmdNavStep(const StringVector &_args);
	void cmdNavStats(const StringVector &_args);
	void cmdNavEnableStep(const StringVector &_args);
	void cmdAutoBuildFeatures(const StringVector &_args);
	void cmdTracePoly(const StringVector &_args);
	void cmdUndo(const StringVector &_args);
	void cmdCreatePlanePoly(const StringVector &_args);
	void cmdCreateSlicePoly(const StringVector &_args);
	void cmdCreateSliceSector(const StringVector &_args);
	void cmdCommitPoly(const StringVector &_args);
	void cmdDeleteSector(const StringVector &_args);
	void cmdMirrorSectors(const StringVector &_args);
	void cmdEditSector(const StringVector &_args);
	void cmdDupeSector(const StringVector &_args);
	void cmdSplitSector(const StringVector &_args);
	void cmdGroundSector(const StringVector &_args);
	void cmdSectorCreateConnections(const StringVector &_args);
	void cmdSetMapCenter(const StringVector &_args);
	void cmdUpdateContents(const StringVector &_args);
	void cmdSectorEdgeDrop(const StringVector &_args);
	void cmdSetField(const StringVector &_args);
	void cmdPortalCreate(const StringVector &_args);
	void cmdPortalBlockCreate(const StringVector &_args);

	void cmdObstacleAdd(const StringVector &_args);

	NavSector * GetSectorAtCursor();

	//////////////////////////////////////////////////////////////////////////
	friend class NavMeshPathInterface;
	friend class PathFindNavMesh;
	friend class ToolSectorEdgeDrop;
	friend class ToolPortalCreate;
	friend class ToolPortalEdgeMask;
	friend class ToolCreateSectorPoly;
	friend class ToolCreateSlicePoly;
	friend class ToolSplitSector;
	friend class ToolTraceSector;
	friend class ToolMoveSector;
protected:
	//////////////////////////////////////////////////////////////////////////
	class NavCollision
	{
	public:
		bool DidHit() const { return mHitSector != NULL; }
		RuntimeNavSector * HitSector() { return mHitSector; }
		const Vector3f & HitPosition() const { return mHitPosition; }
		const Vector3f & HitNormal() const { return mHitNormal; }

		NavCollision( const Vector3f & pos = Vector3f::ZERO, const Vector3f &normal = Vector3f::ZERO, RuntimeNavSector * hitSector = NULL )
			: mHitPosition( pos )
			, mHitNormal( normal )
			, mHitSector( hitSector )
		{
		}
	private:
		RuntimeNavSector *	mHitSector;
		Vector3f			mHitPosition;
		Vector3f			mHitNormal;
		obuint32			mActiveSectorId;
	};

	NavCollision FindCollision(const Vector3f &_from, const Vector3f &_to);
	int FindRuntimeSectors( const AABB & aabb,
		RuntimeSectorRefs & sectorsOut,
		const bool staticOnly );
	int FindRuntimeSectors( const Box3f & obb,
		RuntimeSectorRefs & sectorsOut,
		const bool staticOnly );

	//////////////////////////////////////////////////////////////////////////
	Vector3f				m_MapCenter;

	typedef std::vector<CollisionModel*> SectorCollisionList;

	NavSectorList			mNavSectors;
	RuntimeSectorList		mRuntimeSectors;
	SectorCollisionList		mRuntimeSectorCollision;
	ObstacleList			mPendingObstacles;
	ObstacleList			mDynamicObstacles;
	ObstacleList			mObstacles;
	
	CollisionModel			mSectorCollision;

	//////////////////////////////////////////////////////////////////////////
	// Current tool variables
	obColor				m_CursorColor;
	NavSector			mEditSector;
	Vector3f			mEditSectorStart;
	Vector3f			mEditSectorNormal;

	Plane3f				mEditSectorPlane;
	Plane3f				mEditSlicePlane;

	EditTool<PathPlannerNavMesh> * mCurrentTool;

	gmGCRoot<gmUserObject> mNavRef;

	void InitSectors();

	Vector3f _SectorVertWithin(const Vector3f &_pos1, const Vector3f &_pos2, float _range, bool *_snapped = NULL);

	void SectorsFromAttribs( RuntimeSectorRefs & sectorsOut,
		const AABB & aabb,
		const CollisionModel::AttribSet & attribs,
		const bool staticOnly );

	void SectorsFromAttribs( RuntimeSectorRefs & sectorsOut,
		const Box3f & obb,
		const CollisionModel::AttribSet & attribs,
		const bool staticOnly );

	//////////////////////////////////////////////////////////////////////////
	// Internal Implementations of base class functionality
	std::string _GetNavFileExtension() { return ".nav"; }
	virtual void _BenchmarkPathFinder(const StringVector &_args);
	virtual void _BenchmarkGetNavPoint(const StringVector &_args);

	void UpdateMoverSectors();
	void UpdateRuntimeSectors();
	void UpdateObstacles();

	template<typename T>
	void SetCurrentTool()
	{
		if ( mCurrentTool != NULL && typeid( *mCurrentTool ) == typeid( T ) )
		{
			if ( !mCurrentTool->ReEnter( this ) )
				OB_DELETE( mCurrentTool );
		}
		else
		{
			OB_DELETE( mCurrentTool );
			mCurrentTool = new T;
			if ( !mCurrentTool->Enter( this ) )
				OB_DELETE( mCurrentTool );
		}
	}
};

#endif
