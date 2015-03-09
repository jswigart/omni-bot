////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef __PATHPLANNERRECAST_H__
#define __PATHPLANNERRECAST_H__

#include "PathPlannerBase.h"
#include "RecastInterfaces.h"

#include "DetourTileCache.h"
#include "DetourTileCacheBuilder.h"
#include "DetourNavMesh.h"

#include "CollisionModel.h"

#include "btBulletCollisionCommon.h"

struct aiScene;
struct aiNode;
struct sqlite3;

namespace modeldata
{
	class Scene;
	class Node;
};

struct EnumDef
{
	size_t			mValue;
	const char *	mName;
};

extern const EnumDef sPolyAreas [];
extern const size_t sNumPolyAreas;

extern const EnumDef sPolyFlags [];
extern const size_t sNumPolyFlags;

bool PolyAreaToString( size_t area, std::string & strOut );
bool StringToPolyArea( const std::string & name, size_t & areaOut );
bool PolyFlagsToString( size_t flags, std::string & strOut );
bool StringToPolyFlags( const std::string & name, size_t & flagsOut );

struct OffMeshConnection
{
	static const size_t MaxPoints = 4;

	Vector3f		mEntry;
	Vector3f		mIntermediates[ MaxPoints ];
	Vector3f		mExit;

	size_t			mNumPts;
	float			mRadius;
	NavArea			mAreaType;
	NavAreaFlags	mFlags;
	bool			mBiDir;

	void Render();

	OffMeshConnection()
		: mNumPts( 0 )
		, mRadius( 0.0f )
		, mAreaType( NAVAREA_GROUND )
		, mFlags( NAVFLAGS_NONE )
		, mBiDir( false )
	{
	}
};

//////////////////////////////////////////////////////////////////////////

// class: PathPlannerRecast
//		Path planner interface for the navmesh system for hl2
class PathPlannerRecast : public PathPlannerBase
{
public:
	friend class RecastPathInterface;

	/*enum NavMeshFlags
	{
	NAV_SHOW_COLLISION_MODELS = NUM_BASE_NAVFLAGS,
	};*/

	static const int VERSION_MODELCACHE = 1;

	enum ModelState
	{
		StateUnknown,
		StateNonCollidable,
		StateCollidable,
		StateMoved,
		StateMarkedForDelete,
	};

	bool Init( System & system );
	void Update( System & system );
	void Shutdown();
	bool IsReady() const;

	int GetLatestFileVersion() const;

	bool GetNavFlagByName( const std::string &_flagname, NavFlags &_flag ) const;

	Vector3f GetDisplayPosition( const Vector3f &_pos );

	bool Load( const std::string &_mapname, bool _dl = true );
	bool Save( const std::string &_mapname );
	void Unload();
	bool SetFileComments( const std::string &_text );

	void RegisterGameGoals();

	virtual void RegisterScriptFunctions( gmMachine *a_machine );

	bool GetNavInfo( const Vector3f &pos, obint32 &_id, std::string &_name );

	void AddEntityConnection( const Event_EntityConnection &_conn );
	void RemoveEntityConnection( GameEntity _ent );
	void EntityCreated( const EntityInstance &ei );
	void EntityDeleted( const EntityInstance &ei );

	void BuildConfig( rcConfig & config );
	void BuildNav();
	void BuildNavTile();

	PathInterface * AllocPathInterface( Client * client );

	const char *GetPlannerName() const
	{
		return "Recast Path Planner";
	};
	int GetPlannerType() const
	{
		return NAVID_RECAST;
	};
	
	struct TileRebuild
	{
		int mX, mY;

		TileRebuild( int x = 0, int y = 0 ) : mX( x ), mY( y ) { }
	};
	struct TileAddData
	{
		int					mX, mY;

		unsigned char *		mNavData = NULL;
		int					mNavDataSize = 0;

		TileAddData( int x = 0, int y = 0 )
			: mX( x )
			, mY( y )
			, mNavData( NULL )
			, mNavDataSize( 0 )
		{
		}
	};
	typedef std::vector<TileRebuild> TileRebuildList;
	typedef std::vector<TileAddData> AddTileList;

	bool AsyncGetTileRebuild( TileRebuild & buildTile );

	void MarkTileForBuilding( const dtMeshTile * tile );
	void MarkTileForBuilding( const Vector3f & pos );
	void MarkTileForBuilding( const int tx, const int ty );

	void RasterizeTileLayers( int tax, int ty );

	PathPlannerRecast();
	virtual ~PathPlannerRecast();
protected:
	friend class ToolCreateBounds;
	friend class ToolCreateOffMeshConnection;

	void OpenCachedDatabase();

	void InitCommands();
	void cmdNavSave( const StringVector &_args );
	void cmdNavLoad( const StringVector &_args );
	void cmdNavView( const StringVector &_args );
	void cmdNavViewConnections( const StringVector &_args );

	void cmdBuildNav( const StringVector &_args );
	void cmdBuildNavTile( const StringVector &_args );

	void cmdUndo( const StringVector &_args );
	void cmdNavAddExclusionZone( const StringVector &_args );
	void cmdNavAddOffMeshConnection( const StringVector &_args );
	void cmdCommitPoly( const StringVector &_args );
	void cmdAutoBuildFeatures( const StringVector &_args );
	void cmdSaveToObjFile( const StringVector &_args );
	void cmdModelEnable( const StringVector &_args );
	void cmdModelMover( const StringVector &_args );
	void cmdModelNonSolid( const StringVector &_args );

	void LoadWorldModel();

	void LoadModel( const GameModelInfo & modelInfo, GameEntity entity, const IceMaths::Matrix4x4 & xform, bool baseStaticMesh );

	EditTool<PathPlannerRecast> * mCurrentTool;

	//////////////////////////////////////////////////////////////////////////
	// Friend functions
	friend int GM_CDECL gmfRecastView( gmThread *a_thread );
	friend int GM_CDECL gmfRecastViewConnections( gmThread *a_thread );
	friend int GM_CDECL gmfRecastEnableStep( gmThread *a_thread );
	friend int GM_CDECL gmfRecastStep( gmThread *a_thread );
	friend int GM_CDECL gmfRecastAddFloodSeed( gmThread *a_thread );
	friend int GM_CDECL gmfRecastBuildNavMesh( gmThread *a_thread );
	friend int GM_CDECL gmfRecastFloodFill( gmThread *a_thread );
	friend int GM_CDECL gmfRecastTrimSectors( gmThread *a_thread );

	//////////////////////////////////////////////////////////////////////////
protected:

	//////////////////////////////////////////////////////////////////////////
	// Internal Implementations of base class functionality
	std::string _GetNavFileExtension()
	{
		return ".nm";
	}

private:
	struct RecastSettings
	{
		float	AgentHeightStand;
		float	AgentHeightCrouch;
		float	AgentRadius;
		float	AgentMaxClimb;

		float	WalkableSlopeAngle;

		float	CellSize;
		float	CellHeight;

		float	EdgeMaxLen;
		float	MaxSimplificationError;
		int		TileSize;
		float	DetailSampleDist;
		float	DetailSampleMaxError;
		
		RecastSettings();
	} mSettings;

	typedef std::vector<OffMeshConnection> OffMeshConnections;
	OffMeshConnections			mOffMeshConnections;

	RecastBuildContext			mContext;
	dtNavMesh	*				mNavMesh;
	
	struct ModelCache
	{
		GameEntity			mEntity;
		BitFlag32			mEntityCategory;
		int					mSubModel;

		ModelTransform		mTransform;
		ModelPtr			mModel;

		ModelState			mActiveState;
		
		bool				mBaseStaticMesh;
		
		bool				mDisabled;
		bool				mMover;
		bool				mNonSolid;

		ModelCache()
			: mModel( NULL )
			, mSubModel( -1 )
			, mActiveState( StateUnknown )
			, mBaseStaticMesh( false )
			, mDisabled( false )
			, mMover( false )
			, mNonSolid( false )
		{
		}
		ModelCache( const ModelCache & other )
		{
			mEntity = other.mEntity;
			mModel = other.mModel;
			mSubModel = other.mSubModel;
			mActiveState = other.mActiveState;
			mBaseStaticMesh = other.mBaseStaticMesh;
			mDisabled = other.mDisabled;
			mMover = other.mMover;
			mNonSolid = other.mNonSolid;
		}
		// move ctr
		ModelCache( ModelCache && other )
		{
			mEntity = other.mEntity;
			mModel = other.mModel;
			mSubModel = other.mSubModel;
			mActiveState = other.mActiveState;
			mBaseStaticMesh = other.mBaseStaticMesh;
			mDisabled = other.mDisabled;
			mMover = other.mMover;
			mNonSolid = other.mNonSolid;
		}

		void Free()
		{
			mEntity = GameEntity();
			mModel.reset();
			mActiveState = StateUnknown;
		}
	};

	typedef std::vector<ModelCache> CachedModels;
	CachedModels					mModels;

	btCollisionWorld*				mCollisionWorld;

	TileRebuildList					mTileBuildQueue;
	AddTileList						mAddTileQueue;

	std::vector<AxisAlignedBox3f>	mExclusionZones;

	void InitNavmesh();

	template<typename T, typename... Args>
	void SetCurrentTool( Args... args )
	{
		if ( mCurrentTool != NULL && typeid( *mCurrentTool ) == typeid( T ) )
		{
			if ( !mCurrentTool->ReEnter( this ) )
				OB_DELETE( mCurrentTool );
		}
		else
		{
			OB_DELETE( mCurrentTool );
			mCurrentTool = new T( args... );
			if ( !mCurrentTool->Enter( this ) )
				OB_DELETE( mCurrentTool );
		}
	}

	sqlite3 * mCacheDb;

	boost::recursive_mutex		mGuardBuildQueue;
	boost::recursive_mutex		mGuardAddTile;

	boost::thread_group			mThreadGroup;
	bool						mBuildBaseNav;

	void CreateModels( GameEntity entity, const AABB & localaabb, bool baseStaticMesh );
	void CreateModels( GameEntity entity, const IceMaths::Matrix4x4 & baseXform, const modeldata::Scene & ioScene, const modeldata::Node & ioNode, bool baseStaticMesh );

	void UpdateModelState();

	void CountStats( const modeldata::Scene & ioScene, const modeldata::Node & ioNode, size_t & numTris, size_t & numMeshes );
	void GatherModel( ModelCache & cache, const modeldata::Scene & ioScene, const modeldata::Node & ioNode, const BitFlag32 category, const IceMaths::Matrix4x4 & nodeXform );

	bool GetAimedAtModel( size_t & modelIndex, size_t & triangleIndex, Vector3f & hitPos, Vector3f & hitNormal );
};

#endif
