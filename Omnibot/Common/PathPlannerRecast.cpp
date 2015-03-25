////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include <omp.h>

#include "PathPlannerRecast.h"
#include "IGameManager.h"
#include "IGame.h"
#include "GoalManager.h"
#include "Path.h"
#include "gmUtilityLib.h"
#include "Client.h"
#include "InterfaceFuncs.h"

#include "RenderBuffer.h"

#include "PathPlannerRecastPathInterface.h"

#include <assimp/config.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <DebugDraw.h>

#include <Recast.h>
#include <RecastDebugDraw.h>

#include <DetourCommon.h>
#include <DetourTileCache.h>
#include <DetourTileCacheBuilder.h>
#include <DetourNavMesh.h>
#include <DetourNavMeshBuilder.h>
#include <DetourDebugDraw.h>

#include "recast.pb.h"
#include "modeldata.pb.h"

#include "google/protobuf/text_format.h"
#include "google/protobuf/io/coded_stream.h"
//#include "google/protobuf/io/gzip_stream.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"

#include "fastlz/fastlz.h"

#include "sqlite3.h"

const char * ModelStateName( PathPlannerRecast::ModelState state )
{
	switch ( state )
	{
		case PathPlannerRecast::StateUnknown:
			return "Unknown";
		case PathPlannerRecast::StateNonCollidable:
			return "NonCollidable";
		case PathPlannerRecast::StateCollidable:
			return "Collidable";
		case PathPlannerRecast::StateMoved:
			return "StateMoved";
		case PathPlannerRecast::StateMarkedForDelete:
			return "MarkForDelete";
	}
	return "Unknown";
}

static IceMaths::Matrix4x4 Convert( const aiMatrix4x4 & mat4 )
{
	IceMaths::Matrix4x4 matOut;
	matOut.SetRow( 0, IceMaths::HPoint( mat4.a1, mat4.b1, mat4.c1, mat4.d1 ) );
	matOut.SetRow( 1, IceMaths::HPoint( mat4.a2, mat4.b2, mat4.c2, mat4.d2 ) );
	matOut.SetRow( 2, IceMaths::HPoint( mat4.a3, mat4.b3, mat4.c3, mat4.d3 ) );
	matOut.SetRow( 3, IceMaths::HPoint( mat4.a4, mat4.b4, mat4.c4, mat4.d4 ) );
	return matOut;
}
static Vector3f Convert( const IceMaths::Point & pt )
{
	return Vector3f( pt.x, pt.y, pt.z );
}
static IceMaths::Point Convert( const aiVector3D & pt )
{
	return IceMaths::Point( pt.x, pt.y, pt.z );
}

static Vector3f Convert( const modeldata::Vec3 & vec )
{
	return Vector3f( vec.x(), vec.y(), vec.z() );
}

static Vector3f Convert( const RecastIO::Vec3 & vec )
{
	return Vector3f( vec.x(), vec.y(), vec.z() );
}

IceMaths::Matrix4x4 Convert( const modeldata::Node & node )
{
	IceMaths::Matrix4x4 nodeXform;
	nodeXform.Identity();
	if ( node.has_transformation() )
	{
		const modeldata::Vec4 & r0 = node.transformation().row0();
		const modeldata::Vec4 & r1 = node.transformation().row1();
		const modeldata::Vec4 & r2 = node.transformation().row2();
		const modeldata::Vec4 & r3 = node.transformation().row3();
		nodeXform.SetRow( 0, IceMaths::HPoint( r0.x(), r0.y(), r0.z(), r0.w() ) );
		nodeXform.SetRow( 1, IceMaths::HPoint( r1.x(), r1.y(), r1.z(), r1.w() ) );
		nodeXform.SetRow( 2, IceMaths::HPoint( r2.x(), r2.y(), r2.z(), r2.w() ) );
		nodeXform.SetRow( 3, IceMaths::HPoint( r3.x(), r3.y(), r3.z(), r3.w() ) );
	}
	return nodeXform;
}

void MarkDirtyTiles( const dtMeshTile * tile, void * userdata )
{
	PathPlannerRecast * sys = static_cast<PathPlannerRecast*>( userdata );
	sys->MarkTileForBuilding( tile );
}

class BotAllocator : public MemoryAllocator
{
public:
	virtual char * AllocateMemory( unsigned int numBytes )
	{
		return ( char *)malloc( numBytes );
	}
	virtual void FreeMemory( void * ptr )
	{
		free( ptr );
	}
} allocatorBot;

//////////////////////////////////////////////////////////////////////////

const EnumDef sPolyAreas [] =
{
	{ NAVAREA_GROUND, "ground" },
	{ NAVAREA_WATER, "water" },
	{ NAVAREA_MOVER, "mover" },
	{ NAVAREA_REGION, "region" },	
	{ NAVAREA_JUMP, "jump" },
	{ NAVAREA_LADDER, "ladder" },
	{ NAVAREA_TELEPORT, "teleport" },
};
const size_t sNumPolyAreas = sizeof( sPolyAreas ) / sizeof( sPolyAreas[ 0 ] );

const EnumDef sPolyFlags [] =
{
	{ NAVFLAGS_WALK, "walk" },
	{ NAVFLAGS_DISABLED, "disabled" },
	{ NAVFLAGS_TEAM1_ONLY, "team1" },
	{ NAVFLAGS_TEAM2_ONLY, "team2" },
	{ NAVFLAGS_TEAM3_ONLY, "team3" },
	{ NAVFLAGS_TEAM4_ONLY, "team4" },
	{ NAVFLAGS_SWIM, "swim" },
};
const size_t sNumPolyFlags = sizeof( sPolyFlags ) / sizeof( sPolyFlags[ 0 ] );

bool PolyAreaToString( size_t area, std::string & strOut )
{
	for ( size_t i = 0; i < sNumPolyAreas; ++i )
	{
		if ( sPolyAreas[ i ].mValue == area )
		{
			strOut = sPolyAreas[ i ].mName;
			return true;
		}
	}
	assert( 0 && "Unhandled PolyArea Type" );
	return false;
}

bool StringToPolyArea( const std::string & name, size_t & areaOut )
{
	for ( size_t i = 0; i < sNumPolyAreas; ++i )
	{
		if ( name == sPolyAreas[ i ].mName )
		{
			areaOut = sPolyAreas[ i ].mValue;
			return true;
		}
	}
	assert( 0 && "Unknown PolyArea Type" );
	return false;
}

bool PolyFlagsToString( size_t flags, std::string & strOut )
{
	strOut = "";

	for ( size_t i = 0; i < sNumPolyFlags; ++i )
	{
		if ( ( flags & sPolyFlags[ i ].mValue ) != 0 )
			strOut += va( "%s%s", strOut.empty() ? "" : ", ", sPolyFlags[ i ].mName );
	}

	return( flags == 0 || !strOut.empty() );
}

bool StringToPolyFlags( const std::string & name, size_t & flagsOut )
{
	flagsOut = 0;

	StringVector tokens;
	Utils::Tokenize( name, ",", tokens );
	for ( size_t i = 0; i < tokens.size(); ++i )
	{
		for ( size_t j = 0; j < sNumPolyFlags; ++j )
		{
			if ( tokens[ i ] == sPolyFlags[ j ].mName )
				flagsOut |= sPolyFlags[ j ].mValue;
		}
	}

	return( flagsOut != 0 || name.empty() );
}

//////////////////////////////////////////////////////////////////////////

PathPlannerRecast::RecastSettings::RecastSettings()
{
	AgentHeightStand = 64.f;
	AgentHeightCrouch = 32.f;
	AgentRadius = 14.f;
	AgentMaxClimb = 18.f;

	WalkableSlopeAngle = 45.0f;

	CellSize = 4.0f;
	CellHeight = 4.0f;

	EdgeMaxLen = 1000.0f;
	MaxSimplificationError = 1.f;
	TileSize = 128;
	DetailSampleDist = 6.0f;
	DetailSampleMaxError = 1.0f;
}

//////////////////////////////////////////////////////////////////////////

Vector3f rcToLocal( const float * vec )
{
	return Vector3f( vec[ 0 ], vec[ 2 ], vec[ 1 ] );
}

Vector3f localToRc( const float * vec )
{
	return Vector3f( vec[ 0 ], vec[ 2 ], vec[ 1 ] );
}

struct DebugDraw : public duDebugDraw
{
	DebugDraw() : mVertCount( 0 ), mSizeHint( 1.0f )
	{
	}

	virtual void depthMask( bool state )
	{
		// not supported
	}

	virtual void texture( bool state )
	{
		// not supported
	}

	virtual void begin( duDebugDrawPrimitives prim, float size = 1.0f )
	{
		mActivePrim = prim;
		mSizeHint = size;
		mVertCount = 0;
	}

	virtual void vertex( const float* pos, unsigned int color )
	{
		vertex( pos[ 0 ], pos[ 1 ], pos[ 2 ], color );
	}

	virtual void vertex( const float x, const float y, const float z, unsigned int color )
	{
		mVertCache[ mVertCount++ ] = Vector3f( x, z, y );

		switch ( mActivePrim )
		{
			case DU_DRAW_POINTS:
				if ( mVertCount == 1 )
				{
					int r, g, b, a;
					duRGBASplit( color, r, g, b, a );

					RenderBuffer::AddPoint( mVertCache[ 0 ], obColor( (uint8_t)r, (uint8_t)g, (uint8_t)b, (uint8_t)a ), mSizeHint );
					mVertCount = 0;
				}
				break;
			case DU_DRAW_LINES:
				if ( mVertCount == 2 )
				{
					int r, g, b, a;
					duRGBASplit( color, r, g, b, a );

					RenderBuffer::AddLine( mVertCache[ 0 ], mVertCache[ 1 ], obColor( (uint8_t)r, (uint8_t)g, (uint8_t)b, (uint8_t)a ), mSizeHint );
					mVertCount = 0;
				}
				break;
			case DU_DRAW_TRIS:
				if ( mVertCount == 3 )
				{
					int r, g, b, a;
					duRGBASplit( color, r, g, b, a );

					RenderBuffer::AddTri( mVertCache[ 0 ], mVertCache[ 1 ], mVertCache[ 2 ], obColor( (uint8_t)r, (uint8_t)g, (uint8_t)b, (uint8_t)a ) );
					mVertCount = 0;
				}
				break;
			case DU_DRAW_QUADS:
				if ( mVertCount == 4 )
				{
					int r, g, b, a;
					duRGBASplit( color, r, g, b, a );

					RenderBuffer::AddQuad( mVertCache[ 0 ], mVertCache[ 1 ], mVertCache[ 2 ], mVertCache[ 3 ], obColor( (uint8_t)r, (uint8_t)g, (uint8_t)b, (uint8_t)a ) );
					mVertCount = 0;
				}
				break;
		}
	}

	virtual void vertex( const float* pos, unsigned int color, const float* uv )
	{
		vertex( pos[ 0 ], pos[ 1 ], pos[ 2 ], color );
	}

	virtual void vertex( const float x, const float y, const float z, unsigned int color, const float u, const float v )
	{
		// uvs not supported
		vertex( x, y, z, color );
	}

	virtual void end()
	{
		mVertCount = 0;
	}
private:
	duDebugDrawPrimitives	mActivePrim;
	float					mSizeHint;
	Vector3f				mVertCache[ 4 ];
	int						mVertCount;
} ddraw;

float dtRandom()
{
	return Mathf::UnitRandom();
}

static const int MAX_LAYERS = 32;

//////////////////////////////////////////////////////////////////////////

struct RasterizationContext
{
	RasterizationContext()
		: solid( NULL )
		, triareas( NULL )
		, lset( NULL )
		, chf( NULL )
		, cset( NULL )
		, polymesh( NULL )
		, meshdetail( NULL )
	{
	}

	~RasterizationContext()
	{
		rcFreeHeightField( solid );
		delete [] triareas;
		rcFreeHeightfieldLayerSet( lset );
		rcFreeCompactHeightfield( chf );
		rcFreeContourSet( cset );
		rcFreePolyMesh( polymesh );
		rcFreePolyMeshDetail( meshdetail );
	}

	rcHeightfield			* solid;
	unsigned char			* triareas;
	rcHeightfieldLayerSet	* lset;
	rcCompactHeightfield	* chf;
	rcContourSet			* cset;
	rcPolyMesh				* polymesh;
	rcPolyMeshDetail		* meshdetail;
};

static bool gAsyncBuildRunning = true;

void AsyncTileBuild( PathPlannerRecast * nav )
{
	while ( gAsyncBuildRunning )
	{
		PathPlannerRecast::TileRebuild buildTile;
		if ( nav->AsyncGetTileRebuild( buildTile ) )
		{
			const int tx = buildTile.mX;
			const int ty = buildTile.mY;

			nav->RasterizeTileLayers( tx, ty );
		}
	}
}

//////////////////////////////////////////////////////////////////////////

PathPlannerRecast::PathPlannerRecast()
	: mNavMesh( NULL )
	, mCurrentTool( NULL )
	, mCacheDb( NULL )
	, mBuildBaseNav( false )
{
	mTileBuildQueue.reserve( 128 );
	mModels.reserve( 2048 );
	//.mPlannerFlags.SetFlag( NAV_VIEW );

	//mCollisionWorld = new btCollisionWorld();
	//mCollisionWorld->setDebugDrawer();
	//mCollisionWorld->setBroadphase( new btDbvtBroadphase() );
}

PathPlannerRecast::~PathPlannerRecast()
{
	Shutdown();
}

int PathPlannerRecast::GetLatestFileVersion() const
{
	return 1; // TODO
}

bool PathPlannerRecast::Init( System & system )
{
	InitCommands();

	OpenCachedDatabase();

	// subtract a thread to try and leave the current core free
	int numBuildThreads = boost::thread::hardware_concurrency() - 1;
	Options::GetValue( "Navigation", "TileBuildThreads", numBuildThreads );
	if ( numBuildThreads < 1 )
		numBuildThreads = 1;

	for ( int i = 0; i < numBuildThreads; ++i )
		mThreadGroup.add_thread( new boost::thread( AsyncTileBuild, this ) );

	EngineFuncs::ConsoleMessage( va( "Created %d threads to rebuild navigation tiles", mThreadGroup.size() ) );

	return true;
}

void PathPlannerRecast::OpenCachedDatabase()
{
	bool cacheToFile = true;
	Options::GetValue( "Navigation", "CacheModelsToFile", cacheToFile );

	fs::path filepath = FileSystem::GetNavFolder() / "_modelcache.db";	
	if ( cacheToFile )
	{
		if ( CheckSqliteError( mCacheDb, sqlite3_open_v2( filepath.string().c_str(), &mCacheDb, SQLITE_OPEN_READWRITE, NULL ) ) == SQLITE_OK )
		{
			// opened existing database, see if we can purge some data
			bool trimOutdatedModels = true;
			if ( Options::GetValue( "Navigation", "TrimOutdatedModels", trimOutdatedModels ) && trimOutdatedModels )
			{
				char * errMsg = NULL;
				sqlite3_exec( mCacheDb, va( "DELETE from modelCache WHERE (version!=%d)", VERSION_MODELCACHE ), 0, 0, &errMsg );
			}
			return;
		}
	}

	// Need to create the database and the tables
	if ( CheckSqliteError( mCacheDb, sqlite3_open_v2( cacheToFile ? filepath.string().c_str() : ":memory:", &mCacheDb, SQLITE_OPEN_CREATE | SQLITE_OPEN_READWRITE, NULL ) ) != SQLITE_OK )
		return;

	sqlite3_stmt * statement = NULL;
	if ( SQLITE_OK == CheckSqliteError( mCacheDb, sqlite3_prepare_v2( mCacheDb, va( "CREATE TABLE modelCache (name TEXT PRIMARY KEY, crc INTEGER, fullSize INTEGER, modeldata BLOB, version INTEGER)" ), -1, &statement, NULL ) ) )
	{
		CheckSqliteError( mCacheDb, sqlite3_step( statement ) );
		CheckSqliteError( mCacheDb, sqlite3_finalize( statement ) );
	}
}

void PathPlannerRecast::RegisterScriptFunctions( gmMachine *a_machine )
{
}

void PathPlannerRecast::MarkTileForBuilding( const Vector3f & pos )
{
	boost::lock_guard<boost::recursive_mutex> lock( mGuardBuildQueue );
	mNavMesh->queryTiles( localToRc( pos ), localToRc( pos ), MarkDirtyTiles, this );
}

void PathPlannerRecast::MarkTileForBuilding( const int tx, const int ty )
{
	for ( size_t j = 0; j < mTileBuildQueue.size(); ++j )
	{
		if ( mTileBuildQueue[ j ].mX == tx && mTileBuildQueue[ j ].mY == ty )
		{
			return;
		}
	}
	mTileBuildQueue.push_back( PathPlannerRecast::TileRebuild( tx, ty ) );
}

void PathPlannerRecast::MarkTileForBuilding( const dtMeshTile * tile )
{
	for ( size_t j = 0; j < mTileBuildQueue.size(); ++j )
	{
		if ( mTileBuildQueue[ j ].mX == tile->header->x &&
			mTileBuildQueue[ j ].mY == tile->header->y )
		{
			return;
		}
	}
	mTileBuildQueue.push_back( PathPlannerRecast::TileRebuild( tile->header->x, tile->header->y ) );
}

bool PathPlannerRecast::AsyncGetTileRebuild( TileRebuild & buildTile )
{
	boost::lock_guard<boost::recursive_mutex> lock( mGuardBuildQueue );
	if ( mTileBuildQueue.size() > 0 )
	{
		buildTile = mTileBuildQueue.front();
		mTileBuildQueue.erase( mTileBuildQueue.begin() );

		EngineFuncs::ConsoleMessage( va( "AsyncTileBuild(%d,%d): %d tiles remaining", buildTile.mX, buildTile.mY, mTileBuildQueue.size() ) );
		return true;
	}
	return false;
}

void PathPlannerRecast::Update( System & system )
{
	Prof( PathPlannerRecast );

	if ( mCurrentTool )
	{
		if ( !mCurrentTool->Update( this ) )
			OB_DELETE( mCurrentTool );
	}

	UpdateModelState();

	if ( mBuildBaseNav )
	{
		boost::lock_guard<boost::recursive_mutex> lock( mGuardBuildQueue );
		if ( mTileBuildQueue.size() == 0 )
		{
			// This navmesh should be the 'world' model only
			if ( Save( gEngineFuncs->GetMapName() ) )
				EngineFuncs::ConsoleMessage( "Saved Base Nav." );
			else
				EngineFuncs::ConsoleError( "ERROR Saving Base Nav." );

			mBuildBaseNav = false;
		}
	}

	if ( mPlannerFlags.CheckFlag( NAV_VIEW ) )
	{
		for ( int i = 0; i < mContext.getLogCount(); ++i )
		{
			const char * logTxt = mContext.getLogText( i );
			EngineFuncs::ConsoleMessage( logTxt );
		}
		mContext.resetLog();
		
		for ( size_t i = 0; i < mExclusionZones.size(); ++i )
			RenderBuffer::AddAABB( mExclusionZones[ i ], COLOR::RED );

		RenderBuffer::AddAABB( mNavigationBounds, COLOR::MAGENTA );

		for ( size_t i = 0; i < mOffMeshConnections.size(); ++i )
			mOffMeshConnections[ i ].Render();

		// surface probe
		int contents = 0, surface = 0;
		Vector3f vAimPt, vAimNormal;
		if ( Utils::GetLocalAimPoint( vAimPt, &vAimNormal, TR_MASK_FLOODFILLENT, &contents, &surface ) )
		{
			obColor clr = COLOR::WHITE;

			if ( surface & SURFACE_LADDER )
			{
				clr = COLOR::ORANGE;
			}
			RenderBuffer::AddLine( vAimPt, vAimPt + vAimNormal * 16.f, clr );
		}

		if ( mNavMesh != NULL )
		{
			Vector3f
				vMins = localToRc( mNavigationBounds.Min ),
				vMaxs = localToRc( mNavigationBounds.Max );

			int gw = 0, gh = 0;
			rcCalcGridSize( vMins, vMaxs, mSettings.CellSize, &gw, &gh );
			const int ts = mSettings.TileSize;
			const int tw = ( gw + ts - 1 ) / ts;
			const int th = ( gh + ts - 1 ) / ts;
			const float s = mSettings.TileSize * mSettings.CellSize;

			duDebugDrawGridXZ( &ddraw,
				vMins[ 0 ],
				vAimPt.Z(),
				vMins[ 2 ],
				tw, th,
				s,
				duRGBA( 0, 0, 0, 64 ),
				1.0f );
		}

		size_t aimedModelIndex, triangleIndex;
		Vector3f aimedPos, aimedNormal;
		if ( GetAimedAtModel( aimedModelIndex, triangleIndex, aimedPos, aimedNormal ) )
		{
			const ModelCache & mdl = mModels[ aimedModelIndex ];
			
			static bool renderMdl = false;
			if ( renderMdl )
				mdl.mModel->Render( mdl.mTransform, Vector3f( 0.0f, 0.0f, 0.0f ) );
			mdl.mModel->RenderAxis( mdl.mTransform );

			Vector3f v0, v1, v2;

			size_t materialIndex;
			mdl.mModel->GetTriangle( mdl.mTransform, triangleIndex, v0, v1, v2, materialIndex );
			RenderBuffer::AddTri( v0, v1, v2, ( mdl.mDisabled ? COLOR::RED : COLOR::GREEN ).fade( 128 ) );

			const Material & mtl = mdl.mModel->GetMaterial( materialIndex );

			std::string contentStr = "cnt: ", surfaceStr = "srf: ";

			if ( mtl.mContents & CONT_SOLID )
				contentStr += " sld";
			if ( mtl.mContents & CONT_WATER )
				contentStr += " wtr";
			if ( mtl.mContents & CONT_SLIME )
				contentStr += " slm";
			if ( mtl.mContents & CONT_FOG )
				contentStr += " fog";
			if ( mtl.mContents & CONT_MOVER )
				contentStr += " mvr";
			if ( mtl.mContents & CONT_TRIGGER )
				contentStr += " trg";
			if ( mtl.mContents & CONT_LAVA )
				contentStr += " lav";
			if ( mtl.mContents & CONT_LADDER )
				contentStr += " ldr";
			if ( mtl.mContents & CONT_TELEPORTER )
				contentStr += " tlp";
			if ( mtl.mContents & CONT_MOVABLE )
				contentStr += " mov";
			if ( mtl.mContents & CONT_PLYRCLIP )
				contentStr += " pclp";

			if ( mtl.mSurface & SURFACE_NONSOLID )
				surfaceStr += " nonsol";
			if ( mtl.mSurface & SURFACE_SLICK )
				surfaceStr += " slick";
			if ( mtl.mSurface & SURFACE_LADDER )
				surfaceStr += " ldr";
			if ( mtl.mSurface & SURFACE_NOFALLDAMAGE )
				surfaceStr += " nofalldam";
			if ( mtl.mSurface & SURFACE_SKY )
				surfaceStr += " sky";
			if ( mtl.mSurface & SURFACE_IGNORE )
				surfaceStr += " ignore";
			if ( mtl.mSurface & SURFACE_NOFOOTSTEP )
				surfaceStr += " nofootstep";
			if ( mtl.mSurface & SURFACE_NODRAW )
				surfaceStr += " nodraw";

			const Vector3f normalOffset = aimedPos + aimedNormal * 24.0f;
			RenderBuffer::AddLine( aimedPos, aimedPos + vAimNormal * 16.0f, COLOR::BLUE );
			RenderBuffer::AddString3d( normalOffset, COLOR::CYAN, va( "%d %s\n%s\n%s\n%s",
				aimedModelIndex, mtl.mName.c_str(), ModelStateName( mdl.mActiveState ), contentStr.c_str(), surfaceStr.c_str() ) );

			const Box3f obb = mdl.mModel->GetWorldOBB( mdl.mTransform );
			const AxisAlignedBox3f & aabb = ComputeAABB( mdl.mModel->GetWorldOBB( mdl.mTransform ) );
			RenderBuffer::AddOBB( obb, COLOR::MAGENTA );
			RenderBuffer::AddAABB( aabb, COLOR::YELLOW );
		}

		static bool showNonWorld = false;
		if ( showNonWorld )
		{
			static float zOffset = 1.0f;
			
			static bool showWorld = false;
			for ( size_t i = showWorld?0:1; i < mModels.size(); ++i )
			{
				EntityInfo entInfo;
				if ( IGame::GetEntityInfo( mModels[ i ].mEntity, entInfo ) )
					mModels[ i ].mModel->Render( mModels[ i ].mTransform, Vector3f( 0.0f, 0.0f, zOffset ), entInfo.mFlags.CheckFlag( ENT_FLAG_COLLIDABLE ) ? COLOR::GREEN.fade( 100 ) : COLOR::ORANGE.fade( 100 ) );
			}
		}

		Vector3f eyePos;
		if ( Utils::GetLocalEyePosition( eyePos ) )
		{
			static float renderRadius = 1024.0f;
			if ( mNavMesh != NULL )
				duDebugDrawNavMeshInRadius( &ddraw, *mNavMesh, DU_DRAWNAVMESH_OFFMESHCONS, localToRc( eyePos ), renderRadius );
		}
	}

	{
		// only modifications to the nav mesh at the end of the frame so the rebuilding
		// can happen asynchronously, but the navmesh usage and updating is done in
		// the main thread
		boost::lock_guard<boost::recursive_mutex> lock( mGuardAddTile );

		for ( size_t i = 0; i < mAddTileQueue.size(); ++i )
		{
			const TileAddData & tile = mAddTileQueue[ i ];

			mNavMesh->removeTile( mNavMesh->getTileRefAt( tile.mX, tile.mY, 0 ), 0, 0 );

			if ( dtStatusFailed( mNavMesh->addTile( tile.mNavData, tile.mNavDataSize, DT_TILE_FREE_DATA, 0, 0 ) ) )
			{
				dtFree( tile.mNavData );
			}
		}

		mAddTileQueue.resize( 0 );
	}
}

void PathPlannerRecast::UpdateModelState()
{
	// update the state of models for dynamic navigation rebuiling
	for ( size_t i = 0; i < mModels.size(); ++i )
	{
		ModelCache & mdl = mModels[ i ];

		// default non collidable, unless the game tells us we should be
		ModelState currentState = StateNonCollidable;

		if ( mdl.mActiveState == StateMarkedForDelete )
			currentState = StateMarkedForDelete;
		else if ( mdl.mDisabled )
			currentState = StateNonCollidable;
		else if ( mdl.mBaseStaticMesh )
			currentState = StateCollidable;
		else
		{
			EntityInfo entInfo;
			if ( IGame::GetEntityInfo( mdl.mEntity, entInfo ) )
				currentState = entInfo.mFlags.CheckFlag( ENT_FLAG_COLLIDABLE ) ? StateCollidable : StateNonCollidable;
		}

		mdl.mTransform.UpdateStableTime();

		const AxisAlignedBox3f oldBounds = ComputeAABB( mdl.mModel->GetWorldOBB( mdl.mTransform ) );

		// Is it moving?
		if ( mdl.mEntity.IsValid() )
		{
			const bool checkStable = mdl.mActiveState != StateUnknown;

			Vector3f origin = Vector3f::ZERO;
			if ( EngineFuncs::EntityPosition( mdl.mEntity, origin ) )
				mdl.mTransform.SetPosition( origin, checkStable );

			Vector3f vFwd, vRight, vUp;
			if ( EngineFuncs::EntityOrientation( mdl.mEntity, vFwd, vRight, vUp ) )
				mdl.mTransform.SetOrientation( Matrix3f( vFwd, -vRight, vUp, true ), checkStable );
		}

		if ( mdl.mTransform.GetStableTime() < 1.0f )
			currentState = StateMoved;

		// when building the base navigation, all non world models should stay unknown
		// so they aren't used in the tile building.
		if ( mBuildBaseNav && !mdl.mBaseStaticMesh )
			currentState = StateUnknown;

		if ( mdl.mActiveState != currentState || currentState == StateMarkedForDelete )
		{
			if ( currentState == StateMarkedForDelete )
			{
				EngineFuncs::ConsoleMessage( va( "Deleting model model %d", i ) );
			}
			else if ( mdl.mEntity.IsValid() )
			{
				std::string entName = EngineFuncs::EntityName( mdl.mEntity, "" );
				EngineFuncs::ConsoleMessage( va( "Entity '%s', model %d change state %s->%s", entName.c_str(), i, ModelStateName( mdl.mActiveState ), ModelStateName( currentState ) ) );
			}

			mdl.mActiveState = currentState;

			// which tiles does this model touch? we need to queue them for rebuilding
			const AxisAlignedBox3f bounds = ComputeAABB( mdl.mModel->GetWorldOBB( mdl.mTransform ) );

			// we dirty both the source and destination tiles to account for large movements, teleportations, 
			boost::lock_guard<boost::recursive_mutex> lock( mGuardBuildQueue );
			mNavMesh->queryTiles( localToRc( oldBounds.Min ), localToRc( oldBounds.Max ), MarkDirtyTiles, this );
			mNavMesh->queryTiles( localToRc( bounds.Min ), localToRc( bounds.Max ), MarkDirtyTiles, this );
		}

		if ( currentState == StateMarkedForDelete )
		{
			mdl.mModel.reset();
			mModels.erase( mModels.begin() + i );
			--i;
			continue;
		}
	}
}

void PathPlannerRecast::Shutdown()
{
	gAsyncBuildRunning = false;
	//mThreadGroup.interrupt_all();
	mThreadGroup.join_all();

	Unload();

	sqlite3_close_v2( mCacheDb );
	mCacheDb = NULL;
}

bool PathPlannerRecast::Load( const std::string &_mapname, bool _dl )
{
	Unload();

	const std::string navName = _mapname + _GetNavFileExtension();
	const std::string navPathBinary = std::string( "nav/" ) + navName;
	const std::string navPathText = std::string( "nav/" ) + navName + "_txt";

	bool success = true;

	try
	{
		RecastIO::NavigationMesh ioNavmesh;

		std::string dataIn;

		File fileBin, fileTxt;
		if ( !fileBin.OpenForRead( navPathBinary.c_str(), File::Binary ) ||
			!fileBin.ReadWholeFile( dataIn ) ||
			!ioNavmesh.ParseFromString( dataIn ) )
		{
			dataIn.clear();

			// try to fall back to a text formatted file
			if ( !fileTxt.OpenForRead( navPathText.c_str(), File::Text ) ||
				 !fileTxt.ReadWholeFile( dataIn ) ||
				 !google::protobuf::TextFormat::ParseFromString( dataIn, &ioNavmesh ) )
			{
				throw std::exception( va( "PathPlannerRecast:: Load failed %s", navPathBinary.c_str() ) );
			}
		}

		LoadWorldModel();
		InitNavmesh();
		
		const int fileVersion = ioNavmesh.version();
		fileVersion; // todo: check this?

		mSettings.AgentHeightStand = ioNavmesh.navmeshparams().agentheightstand();
		mSettings.AgentHeightCrouch = ioNavmesh.navmeshparams().agentheightcrouch();
		mSettings.AgentRadius = ioNavmesh.navmeshparams().agentradius();
		mSettings.AgentMaxClimb = ioNavmesh.navmeshparams().agentclimb();
		mSettings.WalkableSlopeAngle = ioNavmesh.navmeshparams().walkslopeangle();
		mSettings.CellSize = ioNavmesh.navmeshparams().cellsize();
		mSettings.CellHeight = ioNavmesh.navmeshparams().cellheight();
		mSettings.EdgeMaxLen = ioNavmesh.navmeshparams().edgemaxlength();
		mSettings.MaxSimplificationError = ioNavmesh.navmeshparams().edgemaxerror();
		mSettings.TileSize = ioNavmesh.navmeshparams().tilesize();
		mSettings.DetailSampleDist = ioNavmesh.navmeshparams().detailsampledist();
		mSettings.DetailSampleMaxError = ioNavmesh.navmeshparams().detailsamplemaxerr();

		for ( int i = 0; i < ioNavmesh.exclusionzone_size(); ++i )
		{
			AxisAlignedBox3f bounds;
			bounds.Min = Convert( ioNavmesh.exclusionzone( i ).mins() );
			bounds.Max = Convert( ioNavmesh.exclusionzone( i ).maxs() );
			mExclusionZones.push_back( bounds );
		}

		for ( int i = 0; i < ioNavmesh.offmeshconnection_size(); ++i )
		{
			const RecastIO::OffMeshConnection & ioConn = ioNavmesh.offmeshconnection( i );

			OffMeshConnection conn;
			conn.mEntry = Convert( ioConn.entrypos() );
			conn.mNumPts = std::min( ioConn.intermediatepos_size(), (int)OffMeshConnection::MaxPoints );
			for ( size_t p = 0; p < conn.mNumPts; ++p )
				conn.mIntermediates[ p ] = Convert( ioConn.intermediatepos( p ) );
			conn.mExit = Convert( ioConn.exitpos() );
			conn.mRadius = ioConn.radius();
			conn.mAreaType = (NavArea)ioConn.areatype();
			conn.mFlags = (NavAreaFlags)ioConn.flags();
			conn.mBiDir = ioConn.bidirectional();
			mOffMeshConnections.push_back( conn );
		}

		for ( int i = 0; i < ioNavmesh.submodelinfo_size(); ++i )
		{
			const RecastIO::SubModel & submdl = ioNavmesh.submodelinfo( i );

			for ( size_t m = 0; m < mModels.size(); ++m )
			{				
				if ( mModels[ m ].mSubModel == submdl.submodelid() )
				{
					if ( submdl.has_disabled() )
						mModels[ m ].mDisabled = submdl.disabled();
					if ( submdl.has_mover() )
						mModels[ m ].mMover = submdl.mover();
					if ( submdl.has_nonsolid() )
						mModels[ m ].mNonSolid = submdl.nonsolid();
				}
			}
		}

		for ( int i = 0; i < ioNavmesh.tiles_size(); ++i )
		{
			const RecastIO::Tile & tile = ioNavmesh.tiles( i );
			const std::string & tileCompressedData = tile.compresseddata();

			unsigned char * decompressedBuffer = new unsigned char[ tile.uncompressedsize() ];
			const int sizeD = fastlz_decompress( tileCompressedData.c_str(), tileCompressedData.size(), decompressedBuffer, tile.uncompressedsize() );
			if ( sizeD == tile.uncompressedsize() )
			{
				mNavMesh->addTile( decompressedBuffer, sizeD, DT_TILE_FREE_DATA, 0, 0 );
			}
			else
			{
				EngineFuncs::ConsoleError( "Invalid Tile Data(compressed size mismatch)" );
			}
		}
	}
	catch ( const std::exception & ex )
	{
		EngineFuncs::ConsoleError( va( "PathPlannerRecast:: Load failed %s", ex.what() ) );
		success = false;

		LoadWorldModel();

		// no matter if we loaded data or not we need to load the world geometry
		// and initialize the navmesh
		InitNavmesh();
	}
	return success;
}

bool PathPlannerRecast::Save( const std::string &_mapname )
{
	if ( mNavMesh != NULL )
	{
		const std::string navName = _mapname + _GetNavFileExtension();
		const std::string navPathBinary = std::string( "nav/" ) + navName;
		const std::string navPathText = std::string( "nav/" ) + navName + "_txt";

		RecastIO::NavigationMesh ioNavmesh;
		ioNavmesh.set_version( 1 );

		ioNavmesh.mutable_navmeshparams()->set_agentheightstand( mSettings.AgentHeightStand );
		ioNavmesh.mutable_navmeshparams()->set_agentheightcrouch( mSettings.AgentHeightCrouch );
		ioNavmesh.mutable_navmeshparams()->set_agentradius( mSettings.AgentRadius );
		ioNavmesh.mutable_navmeshparams()->set_agentclimb( mSettings.AgentMaxClimb );
		ioNavmesh.mutable_navmeshparams()->set_walkslopeangle( mSettings.WalkableSlopeAngle );
		ioNavmesh.mutable_navmeshparams()->set_cellsize( mSettings.CellSize );
		ioNavmesh.mutable_navmeshparams()->set_cellheight( mSettings.CellHeight );
		ioNavmesh.mutable_navmeshparams()->set_edgemaxlength( mSettings.EdgeMaxLen );
		ioNavmesh.mutable_navmeshparams()->set_edgemaxerror( mSettings.MaxSimplificationError );
		ioNavmesh.mutable_navmeshparams()->set_tilesize( mSettings.TileSize );
		ioNavmesh.mutable_navmeshparams()->set_detailsampledist( mSettings.DetailSampleDist );
		ioNavmesh.mutable_navmeshparams()->set_detailsamplemaxerr( mSettings.DetailSampleMaxError );

		for ( size_t i = 0; i < mExclusionZones.size(); ++i )
		{
			RecastIO::AxisAlignedBounds * bnds = ioNavmesh.add_exclusionzone();
			bnds->mutable_mins()->set_x( mExclusionZones[ i ].Min[ 0 ] );
			bnds->mutable_mins()->set_y( mExclusionZones[ i ].Min[ 1 ] );
			bnds->mutable_mins()->set_z( mExclusionZones[ i ].Min[ 2 ] );
			bnds->mutable_maxs()->set_x( mExclusionZones[ i ].Max[ 0 ] );
			bnds->mutable_maxs()->set_y( mExclusionZones[ i ].Max[ 1 ] );
			bnds->mutable_maxs()->set_z( mExclusionZones[ i ].Max[ 2 ] );
		}

		for ( size_t i = 0; i < mOffMeshConnections.size(); ++i )
		{
			RecastIO::OffMeshConnection * conn = ioNavmesh.add_offmeshconnection();
			conn->mutable_entrypos()->set_x( mOffMeshConnections[ i ].mEntry[ 0 ] );
			conn->mutable_entrypos()->set_y( mOffMeshConnections[ i ].mEntry[ 1 ] );
			conn->mutable_entrypos()->set_z( mOffMeshConnections[ i ].mEntry[ 2 ] );
			conn->mutable_exitpos()->set_x( mOffMeshConnections[ i ].mExit[ 0 ] );
			conn->mutable_exitpos()->set_y( mOffMeshConnections[ i ].mExit[ 1 ] );
			conn->mutable_exitpos()->set_z( mOffMeshConnections[ i ].mExit[ 2 ] );
			conn->set_radius( mOffMeshConnections[ i ].mRadius );
			conn->set_areatype( mOffMeshConnections[ i ].mAreaType );
			conn->set_flags( mOffMeshConnections[ i ].mFlags );

			for ( size_t p = 0; p < mOffMeshConnections[ i ].mNumPts; ++p )
			{
				RecastIO::Vec3 * ioPos = conn->add_intermediatepos();
				ioPos->set_x( mOffMeshConnections[ i ].mIntermediates[ p ].X() );
				ioPos->set_y( mOffMeshConnections[ i ].mIntermediates[ p ].Y() );
				ioPos->set_z( mOffMeshConnections[ i ].mIntermediates[ p ].Z() );
			}
			if ( mOffMeshConnections[ i ].mBiDir != conn->bidirectional() )
				conn->set_bidirectional( true );
		}

		for ( int i = 0; i < mNavMesh->getMaxTiles(); ++i )
		{
			const dtMeshTile* tile = ( (const dtNavMesh *)mNavMesh )->getTile( i );
			if ( tile && tile->header && tile->dataSize )
			{
				// compress the tile data
				const size_t bufferSize = tile->dataSize + (size_t)( tile->dataSize * 0.1 );
				boost::shared_array<char> compressBuffer( new char[ bufferSize ] );
				const int sizeCompressed = fastlz_compress_level( 2, tile->data, tile->dataSize, compressBuffer.get() );

				RecastIO::Tile * ioTile = ioNavmesh.add_tiles();
				ioTile->set_uncompressedsize( tile->dataSize );
				ioTile->set_compresseddata( compressBuffer.get(), sizeCompressed );
			}
		}

		for ( size_t m = 0; m < mModels.size(); ++m )
		{
			if ( mModels[ m ].mSubModel != -1 )
			{
				RecastIO::SubModel * submdl = ioNavmesh.add_submodelinfo();
				submdl->set_submodelid( mModels[ m ].mSubModel );
				submdl->set_disabled( mModels[ m ].mDisabled );
				submdl->set_mover( mModels[ m ].mMover );
				submdl->set_nonsolid( mModels[ m ].mNonSolid );
				//ioNavmesh.add_disabledsubmodels( mModels[ m ].mSubModel );
			}
		}

		try
		{
			/*File outBinary;
			if ( outBinary.OpenForWrite( navPathBinary.c_str(), File::Binary, false ) )
			{
				outBinary.Write( fbbl.GetBufferPointer(), fbbl.GetSize() );
				outBinary.Close();
			}*/

			std::string dataOut;

			// binary file
			if ( ioNavmesh.SerializeToString( &dataOut ) )
			{
				File outBinary;
				if ( outBinary.OpenForWrite( navPathBinary.c_str(), File::Binary, false ) )
				{
					outBinary.Write( dataOut.c_str(), dataOut.length() );
					outBinary.Close();
				}
			}

			if ( google::protobuf::TextFormat::PrintToString( ioNavmesh, &dataOut ) )
			{
				File outText;
				if ( outText.OpenForWrite( navPathText.c_str(), File::Text, false ) )
				{
					outText.Write( dataOut.c_str(), dataOut.length() );
					outText.Close();
				}
			}
		}
		catch ( const std::exception & ex )
		{
			EngineFuncs::ConsoleError( va( "PathPlannerRecast:: Save failed %s", ex.what() ) );
			return false;
		}
		return true;
	}
	return false;
}

bool PathPlannerRecast::IsReady() const
{
	return mNavMesh != NULL;
}

bool PathPlannerRecast::GetNavFlagByName( const std::string &_flagname, NavFlags &_flag ) const
{
	_flag = 0;
	return false;
}

void PathPlannerRecast::Unload()
{
	OB_DELETE( mCurrentTool );

	dtFreeNavMesh( mNavMesh );
	mNavMesh = NULL;

	mExclusionZones.resize( 0 );

	mOffMeshConnections.resize( 0 );

	for ( size_t i = 0; i < mModels.size(); ++i )
		mModels[ i ].Free();

	mModels.resize( 0 );
}

void PathPlannerRecast::RegisterGameGoals()
{
}

//////////////////////////////////////////////////////////////////////////

void PathPlannerRecast::InitNavmesh()
{
	Vector3f
		vMins = localToRc( mNavigationBounds.Min ),
		vMaxs = localToRc( mNavigationBounds.Max );

	dtFreeNavMesh( mNavMesh );
	mNavMesh = dtAllocNavMesh();

	dtNavMeshParams parms;
	rcVcopy( parms.orig, vMins );
	parms.tileWidth = mSettings.TileSize * mSettings.CellSize;
	parms.tileHeight = mSettings.TileSize * mSettings.CellSize;
	parms.maxTiles = (int)( parms.tileWidth * parms.tileHeight * 2.0 );
	parms.maxPolys = 16384;

	mNavMesh->init( &parms );

	for ( size_t i = 0; i < RecastPathInterface::sInterfaces.size(); ++i )
		RecastPathInterface::sInterfaces[ i ]->ReInit();
}

//////////////////////////////////////////////////////////////////////////

void PathPlannerRecast::BuildNav()
{
	InitNavmesh();

	rcConfig cfg;
	BuildConfig( cfg );

	int gw = 0, gh = 0;
	rcCalcGridSize( cfg.bmin, cfg.bmax, mSettings.CellSize, &gw, &gh );
	const int ts = mSettings.TileSize;
	const int tw = ( gw + ts - 1 ) / ts;
	const int th = ( gh + ts - 1 ) / ts;

	// prevent the worker threads from changing anything
	boost::lock_guard<boost::recursive_mutex> lock( mGuardBuildQueue );

	// clear any pending tile rebuilds
	mTileBuildQueue.resize( 0 );

	mBuildBaseNav = true;

	for ( int y = 0; y < th; ++y )
	{
		for ( int x = 0; x < tw; ++x )
		{
			MarkTileForBuilding( x, y );
		}
	}

	// mark all but the world model(0) as unknown so they won't be used in the rebuild process
	for ( size_t i = 1; i < mModels.size(); ++i )
		mModels[ i ].mActiveState = StateUnknown;
}

//////////////////////////////////////////////////////////////////////////

void PathPlannerRecast::BuildConfig( rcConfig & cfg )
{
	memset( &cfg, 0, sizeof( cfg ) );
	cfg.cs = mSettings.CellSize;
	cfg.ch = mSettings.CellHeight;
	cfg.walkableSlopeAngle = mSettings.WalkableSlopeAngle;
	cfg.walkableHeight = (int)ceilf( mSettings.AgentHeightStand / cfg.ch );
	cfg.walkableClimb = (int)floorf( mSettings.AgentMaxClimb / cfg.ch );
	cfg.walkableRadius = (int)ceilf( mSettings.AgentRadius / cfg.cs );
	cfg.maxEdgeLen = (int)( mSettings.EdgeMaxLen / mSettings.CellSize );
	cfg.maxSimplificationError = mSettings.MaxSimplificationError;
	cfg.minRegionArea = rcSqr<int>( 8 ); // Note: area = size*size
	cfg.mergeRegionArea = rcSqr<int>( 20 );	// Note: area = size*size
	cfg.maxVertsPerPoly = DT_VERTS_PER_POLYGON;
	cfg.tileSize = mSettings.TileSize;
	cfg.borderSize = cfg.walkableRadius + 3; // Reserve enough padding.
	cfg.width = cfg.tileSize + cfg.borderSize * 2;
	cfg.height = cfg.tileSize + cfg.borderSize * 2;
	cfg.detailSampleDist = mSettings.DetailSampleDist < 0.9f ? 0 : mSettings.CellSize * mSettings.DetailSampleDist;
	cfg.detailSampleMaxError = cfg.ch * mSettings.DetailSampleMaxError;

	Vector3f
		vMins = localToRc( mNavigationBounds.Min ),
		vMaxs = localToRc( mNavigationBounds.Max );

	rcVcopy( cfg.bmin, vMins );
	rcVcopy( cfg.bmax, vMaxs );
}

//////////////////////////////////////////////////////////////////////////

void PathPlannerRecast::BuildNavTile()
{
	if ( mNavMesh == NULL )
		InitNavmesh();

	Vector3f aimPos;
	if ( Utils::GetLocalAimPoint( aimPos ) )
	{
		aimPos = localToRc( aimPos );

		int tileX = 0, tileY = 0;
		mNavMesh->calcTileLoc( aimPos, &tileX, &tileY );

		MarkTileForBuilding( tileX, tileY );
	}
}

//////////////////////////////////////////////////////////////////////////

void PathPlannerRecast::RasterizeTileLayers( int tx, int ty )
{
	RasterizationContext rc;

	rcConfig cfg;
	BuildConfig( cfg );

	// Tile bounds.
	const float tcs = cfg.tileSize * cfg.cs;

	rcConfig tcfg;
	memcpy( &tcfg, &cfg, sizeof( tcfg ) );

	tcfg.bmin[ 0 ] = cfg.bmin[ 0 ] + tx*tcs;
	tcfg.bmin[ 1 ] = cfg.bmin[ 1 ];
	tcfg.bmin[ 2 ] = cfg.bmin[ 2 ] + ty*tcs;
	tcfg.bmax[ 0 ] = cfg.bmin[ 0 ] + ( tx + 1 )*tcs;
	tcfg.bmax[ 1 ] = cfg.bmax[ 1 ];
	tcfg.bmax[ 2 ] = cfg.bmin[ 2 ] + ( ty + 1 )*tcs;

	// border size
	tcfg.bmin[ 0 ] -= tcfg.borderSize * tcfg.cs;
	tcfg.bmin[ 2 ] -= tcfg.borderSize * tcfg.cs;
	tcfg.bmax[ 0 ] += tcfg.borderSize * tcfg.cs;
	tcfg.bmax[ 2 ] += tcfg.borderSize * tcfg.cs;

	// Allocate voxel heightfield where we rasterize our input data to.
	rc.solid = rcAllocHeightfield();
	if ( !rc.solid )
	{
		mContext.log( RC_LOG_ERROR, "buildNavigation: Out of memory 'solid'." );
		return;
	}

	if ( !rcCreateHeightfield( &mContext, *rc.solid, tcfg.width, tcfg.height, tcfg.bmin, tcfg.bmax, tcfg.cs, tcfg.ch ) )
	{
		mContext.log( RC_LOG_ERROR, "buildNavigation: Could not create solid heightfield." );
		return;
	}
		
	size_t tileTriCount = 0;

	for ( size_t m = 0; m < mModels.size(); ++m )
	{
		const ModelCache & model = mModels[ m ];
		
		// skip models not currently active
		switch ( mModels[ m ].mActiveState )
		{
			case StateCollidable:
				break;
			case StateUnknown:
			default:
				continue;
		}

		// accounted for in 2nd pass later
		if ( model.mNonSolid )
			continue;

		// todo: collect only relevant triangles
		//model.mModel->CollideAABB();

		// for now use all triangles for testing
		for ( size_t t = 0; t < model.mModel->GetNumTris(); ++t )
		{
			size_t materialIndex;
			Vector3f verts[ 3 ];
			model.mModel->GetTriangle( model.mTransform, t, verts[ 0 ], verts[ 1 ], verts[ 2 ], materialIndex );
			const Material & mat = model.mModel->GetMaterial( materialIndex );

			if ( mat.mSurface & ( SURFACE_NONSOLID | SURFACE_IGNORE | SURFACE_SKY ) )
				continue;

			NavArea areaType = NAVAREA_GROUND;
#if(0)
			if ( mat.mContents & CONT_WATER )
				continue; // skip to process later as an open region
			if ( model.mEntityCategory.CheckFlag( ENT_CAT_TRIGGER ) )
				continue; // skip to process later as an open region
			if ( model.mEntityCategory.CheckFlag( ENT_CAT_MOVER ) )
				continue; // skip to process later as an open region
#endif
			if ( model.mEntInfo.mCategory.CheckFlag( ENT_CAT_PROP_PUSHABLE ) )
				continue; // skip to process later as an open region
			
			// everything else we can process as solid

			// in game space
			if ( verts[ 0 ].Z() < mNavigationBounds.Min.Z() &&
				verts[ 1 ].Z() < mNavigationBounds.Min.Z() &&
				verts[ 2 ].Z() < mNavigationBounds.Min.Z() )
			{
				continue;
			}

			if ( verts[ 0 ].Z() > mNavigationBounds.Max.Z() &&
				verts[ 1 ].Z() > mNavigationBounds.Max.Z() &&
				verts[ 2 ].Z() > mNavigationBounds.Max.Z() )
			{
				continue;
			}

			// in recast space
			verts[ 0 ] = localToRc( verts[ 0 ] );
			verts[ 1 ] = localToRc( verts[ 1 ] );
			verts[ 2 ] = localToRc( verts[ 2 ] );

			unsigned char areaFlag = 0;
			const int tris[ 3 ] = { 0, 1, 2 };
			rcMarkWalkableTriangles( &mContext,
				tcfg.walkableSlopeAngle,
				(float*)verts, 3,
				tris, 1,
				&areaFlag );

			if ( areaFlag == RC_WALKABLE_AREA && areaType != NAVAREA_GROUND )
				areaFlag = (unsigned char)areaType;
			
			rcRasterizeTriangle( &mContext, verts[ 0 ], verts[ 1 ], verts[ 2 ], areaFlag, *rc.solid );
			++tileTriCount;
		}
	}

	// Once all geometry is rasterized, we do initial pass of filtering to
	// remove unwanted overhangs caused by the conservative rasterization
	// as well as filter spans where the character cannot possibly stand.
	rcFilterLowHangingWalkableObstacles( &mContext, tcfg.walkableClimb, *rc.solid );
	//rcFilterLedgeSpans( &mContext, tcfg.walkableHeight, tcfg.walkableClimb, *rc.solid );
	rcFilterWalkableLowHeightSpans( &mContext, tcfg.walkableHeight, *rc.solid );

	rc.chf = rcAllocCompactHeightfield();
	if ( !rc.chf )
	{
		mContext.log( RC_LOG_ERROR, "buildNavigation: Out of memory 'chf'." );
		return;
	}
	if ( !rcBuildCompactHeightfield( &mContext, tcfg.walkableHeight, tcfg.walkableClimb, *rc.solid, *rc.chf ) )
	{
		mContext.log( RC_LOG_ERROR, "buildNavigation: Could not build compact data." );
		return;
	}

	// Erode the walkable area by agent radius.
	if ( !rcErodeWalkableArea( &mContext, tcfg.walkableRadius, *rc.chf ) )
	{
		mContext.log( RC_LOG_ERROR, "buildNavigation: Could not erode." );
		return;
	}

	// use any non solid geometry for marking additional regions
	for ( size_t m = 0; m < mModels.size(); ++m )
	{
		const ModelCache & model = mModels[ m ];

		switch ( model.mActiveState )
		{
			case StateCollidable:
				break;
			case StateUnknown:
			default:
				continue;
		}

		if ( !model.mNonSolid && !model.mEntInfo.mCategory.CheckFlag( ENT_CAT_PROP_PUSHABLE ) )
			continue;

		for ( size_t t = 0; t < model.mModel->GetNumTris(); ++t )
		{
			size_t materialIndex;
			Vector3f verts[ 3 ];
			model.mModel->GetTriangle( model.mTransform, t, verts[ 0 ], verts[ 1 ], verts[ 2 ], materialIndex );
			const Material & mat = model.mModel->GetMaterial( materialIndex );
			
			if ( mat.mSurface & ( SURFACE_NONSOLID | SURFACE_IGNORE | SURFACE_SKY ) )
				continue;

			NavArea areaType = NAVAREA_GROUND;

			if ( mat.mContents & CONT_WATER )
				areaType = NAVAREA_WATER;
			if ( model.mEntInfo.mCategory.CheckFlag( ENT_CAT_TRIGGER ) )
				areaType = NAVAREA_REGION; // mark off triggers
			if ( model.mEntInfo.mCategory.CheckFlag( ENT_CAT_MOVER ) )
				areaType = NAVAREA_MOVER; // mark off movers
			if ( model.mEntInfo.mCategory.CheckFlag( ENT_CAT_PROP_PUSHABLE ) )
				areaType = NAVAREA_PUSHABLE;

			// only proceed with special areas
			if ( areaType == NAVAREA_GROUND )
				continue;

			// in game space
			if ( verts[ 0 ].Z() < mNavigationBounds.Min.Z() &&
				verts[ 1 ].Z() < mNavigationBounds.Min.Z() &&
				verts[ 2 ].Z() < mNavigationBounds.Min.Z() )
			{
				continue;
			}

			if ( verts[ 0 ].Z() > mNavigationBounds.Max.Z() &&
				verts[ 1 ].Z() > mNavigationBounds.Max.Z() &&
				verts[ 2 ].Z() > mNavigationBounds.Max.Z() )
			{
				continue;
			}

			// in recast space
			verts[ 0 ] = localToRc( verts[ 0 ] );
			verts[ 1 ] = localToRc( verts[ 1 ] );
			verts[ 2 ] = localToRc( verts[ 2 ] );

			float hmin = FLT_MAX;
			float hmax = -FLT_MAX;

			for ( int v = 0; v < 3; ++v )
			{
				hmin = rcMin( hmin, verts[ v ].Y() );
				hmax = rcMax( hmax, verts[ v ].Y() );
			}

			static float maxAdj = 4.0f;
			
			hmin -= mSettings.AgentHeightStand;
			hmax += maxAdj;

			rcMarkConvexPolyArea( &mContext, (float*)verts, 3, hmin, hmax, (unsigned char)areaType, *rc.chf );
		}
	}

	for ( size_t x = 0; x < mExclusionZones.size(); ++x )
	{
		const AxisAlignedBox3f & aabb = mExclusionZones[ x ];

		const Vector3f verts[ 4 ] =
		{
			localToRc( Vector3f( aabb.Min[ 0 ], aabb.Min[ 1 ], aabb.Min[ 2 ] ) ),
			localToRc( Vector3f( aabb.Max[ 0 ], aabb.Min[ 1 ], aabb.Min[ 2 ] ) ),
			localToRc( Vector3f( aabb.Max[ 0 ], aabb.Max[ 1 ], aabb.Min[ 2 ] ) ),
			localToRc( Vector3f( aabb.Min[ 0 ], aabb.Max[ 1 ], aabb.Min[ 2 ] ) ),
		};
		
		rcMarkConvexPolyArea( &mContext, (float*)&verts[0], 4, aabb.Min.Z(), aabb.Max.Z(), RC_NULL_AREA, *rc.chf );
	}

	enum PartitionType
	{
		PARTITION_WATERSHED,
		PARTITION_MONOTONE,
		PARTITION_LAYERS,
	};

	static PartitionType usePartitionType = PARTITION_MONOTONE;

	if ( usePartitionType == PARTITION_WATERSHED )
	{
		// Prepare for region partitioning, by calculating distance field along the walkable surface.
		if ( !rcBuildDistanceField( &mContext, *rc.chf ) )
		{
			mContext.log( RC_LOG_ERROR, "buildNavigation: Could not build distance field." );
			return;
		}

		// Partition the walkable surface into simple regions without holes.
		if ( !rcBuildRegions( &mContext, *rc.chf, tcfg.borderSize, tcfg.minRegionArea, tcfg.mergeRegionArea ) )
		{
			mContext.log( RC_LOG_ERROR, "buildNavigation: Could not build watershed regions." );
			return;
		}
	}
	else if ( usePartitionType == PARTITION_MONOTONE )
	{
		// Partition the walkable surface into simple regions without holes.
		// Monotone partitioning does not need distance field.
		if ( !rcBuildRegionsMonotone( &mContext, *rc.chf, tcfg.borderSize, tcfg.minRegionArea, tcfg.mergeRegionArea ) )
		{
			mContext.log( RC_LOG_ERROR, "buildNavigation: Could not build monotone regions." );
			return;
		}
	}
	else // PARTITION_LAYERS
	{
		// Partition the walkable surface into simple regions without holes.
		if ( !rcBuildLayerRegions( &mContext, *rc.chf, tcfg.borderSize, tcfg.minRegionArea ) )
		{
			mContext.log( RC_LOG_ERROR, "buildNavigation: Could not build layer regions." );
			return;
		}
	}
	
	// Create contours.
	rc.cset = rcAllocContourSet();
	if ( !rc.cset )
	{
		mContext.log( RC_LOG_ERROR, "buildNavigation: Out of memory 'cset'." );
		return;
	}
	if ( !rcBuildContours( &mContext, *rc.chf, tcfg.maxSimplificationError, tcfg.maxEdgeLen, *rc.cset ) )
	{
		mContext.log( RC_LOG_ERROR, "buildNavigation: Could not create contours." );
		return;
	}

	if ( rc.cset->nconts == 0 )
	{
		return;
	}

	// Build polygon navmesh from the contours.
	rc.polymesh = rcAllocPolyMesh();
	if ( !rc.polymesh )
	{
		mContext.log( RC_LOG_ERROR, "buildNavigation: Out of memory 'pmesh'." );
		return;
	}
	if ( !rcBuildPolyMesh( &mContext, *rc.cset, tcfg.maxVertsPerPoly, *rc.polymesh ) )
	{
		mContext.log( RC_LOG_ERROR, "buildNavigation: Could not triangulate contours." );
		return;
	}

	// Build detail mesh.
	rc.meshdetail = rcAllocPolyMeshDetail();
	if ( !rc.meshdetail )
	{
		mContext.log( RC_LOG_ERROR, "buildNavigation: Out of memory 'dmesh'." );
		return;
	}

	if ( !rcBuildPolyMeshDetail( &mContext, *rc.polymesh, *rc.chf,
		tcfg.detailSampleDist, tcfg.detailSampleMaxError,
		*rc.meshdetail ) )
	{
		mContext.log( RC_LOG_ERROR, "buildNavigation: Could build polymesh detail." );
		return;
	}

	rcFreeCompactHeightfield( rc.chf );
	rc.chf = 0;
	rcFreeContourSet( rc.cset );
	rc.cset = 0;

	if ( tcfg.maxVertsPerPoly <= DT_VERTS_PER_POLYGON )
	{
		//if ( rc.polymesh->nverts >= 0xffff )
		//{
		//	// The vertex indices are ushorts, and cannot point to more than 0xffff vertices.
		//	mContext.log(RC_LOG_ERROR, "Too many vertices per tile %d (max: %d).", rc.polymesh->nverts, 0xffff);
		//	return 0;
		//}

		// Update poly flags from areas.
		for ( int i = 0; i < rc.polymesh->npolys; ++i )
		{
			if ( rc.polymesh->areas[ i ] == RC_WALKABLE_AREA )
				rc.polymesh->areas[ i ] = NAVAREA_GROUND;

			if ( rc.polymesh->areas[ i ] == NAVAREA_GROUND ||
				rc.polymesh->areas[ i ] == NAVAREA_MOVER )
			{
				rc.polymesh->flags[ i ] = NAVFLAGS_WALK;
			}
			else if ( rc.polymesh->areas[ i ] == NAVAREA_WATER )
			{
				rc.polymesh->flags[ i ] = NAVFLAGS_SWIM;
			}
		}

		struct ConnPts
		{
			Vector3f	mStart;
			Vector3f	mEnd;
		};

		std::vector<ConnPts>			offMeshPoints( mOffMeshConnections.size() );
		std::vector<float>				offMeshRadius( mOffMeshConnections.size() );
		std::vector<unsigned char>		offMeshConDir( mOffMeshConnections.size() );
		std::vector<unsigned char>		offMeshConAreas( mOffMeshConnections.size() );
		std::vector<unsigned short>		offMeshConFlags( mOffMeshConnections.size() );
		std::vector<unsigned int>		offMeshUserIds( mOffMeshConnections.size() );

		for ( size_t c = 0; c < mOffMeshConnections.size(); ++c )
		{
			const OffMeshConnection & conn = mOffMeshConnections[ c ];
			offMeshPoints[ c ].mStart = localToRc( conn.mEntry );
			offMeshPoints[ c ].mEnd = localToRc( conn.mExit );
			offMeshRadius[ c ] = conn.mRadius;
			offMeshConDir[ c ] = conn.mBiDir ? DT_OFFMESH_CON_BIDIR : 0;
			offMeshConAreas[ c ] = (unsigned char)conn.mAreaType;
			offMeshConFlags[ c ] = (unsigned short)conn.mFlags;
			offMeshUserIds[ c ] = c;
		}

		dtNavMeshCreateParams params;
		memset( &params, 0, sizeof( params ) );
		params.verts = rc.polymesh->verts;
		params.vertCount = rc.polymesh->nverts;
		params.polys = rc.polymesh->polys;
		params.polyAreas = rc.polymesh->areas;
		params.polyFlags = rc.polymesh->flags;
		params.polyCount = rc.polymesh->npolys;
		params.nvp = rc.polymesh->nvp;
		params.detailMeshes = rc.meshdetail->meshes;
		params.detailVerts = rc.meshdetail->verts;
		params.detailVertsCount = rc.meshdetail->nverts;
		params.detailTris = rc.meshdetail->tris;
		params.detailTriCount = rc.meshdetail->ntris;
		if ( mOffMeshConnections.size() > 0 )
		{
			params.offMeshConVerts = offMeshPoints[ 0 ].mStart;
			params.offMeshConRad = &offMeshRadius[ 0 ];
			params.offMeshConDir = &offMeshConDir[ 0 ];
			params.offMeshConAreas = &offMeshConAreas[ 0 ];
			params.offMeshConFlags = &offMeshConFlags[ 0 ];
			params.offMeshConUserID = &offMeshUserIds[ 0 ];
			params.offMeshConCount = mOffMeshConnections.size();
		}
		params.walkableHeight = mSettings.AgentHeightStand;
		params.walkableRadius = mSettings.AgentRadius;
		params.walkableClimb = mSettings.AgentMaxClimb;
		params.tileX = tx;
		params.tileY = ty;
		params.tileLayer = 0;
		rcVcopy( params.bmin, rc.polymesh->bmin );
		rcVcopy( params.bmax, rc.polymesh->bmax );
		params.cs = tcfg.cs;
		params.ch = tcfg.ch;
		params.buildBvTree = true;

		TileAddData add;
		add.mX = tx;
		add.mY = ty;
		
		if ( !dtCreateNavMeshData( &params, &add.mNavData, &add.mNavDataSize ) )
		{
			mContext.log( RC_LOG_ERROR, "Could not build Detour navmesh." );
			return;
		}

		{
			boost::lock_guard<boost::recursive_mutex> lock( mGuardAddTile );
			
			mAddTileQueue.push_back( add );

			/*mNavMesh->removeTile( mNavMesh->getTileRefAt( tx, ty, 0 ), 0, 0 );

			if ( dtStatusFailed( mNavMesh->addTile( navData, navDataSize, DT_TILE_FREE_DATA, 0, 0 ) ) )
			{
				dtFree( navData );
			}*/
		}
	}
}

//////////////////////////////////////////////////////////////////////////

bool PathPlannerRecast::GetNavInfo( const Vector3f &pos, int32_t &_id, std::string &_name )
{
	return false;
}

void PathPlannerRecast::AddEntityConnection( const Event_EntityConnection &_conn )
{
}

void PathPlannerRecast::RemoveEntityConnection( GameEntity _ent )
{
}

Vector3f PathPlannerRecast::GetDisplayPosition( const Vector3f &_pos )
{
	return _pos;
}

void PathPlannerRecast::CountStats( const modeldata::Scene & ioScene, const modeldata::Node & ioNode, size_t & numTris, size_t & numMeshes )
{
	// count all children
	for ( int i = 0; i < ioNode.children_size(); ++i )
	{
		CountStats( ioScene, ioNode.children( i ), numTris, numMeshes );
	}

	for ( int m = 0; m < ioNode.meshindex_size(); ++m )
	{
		const modeldata::Mesh & ioMesh = ioScene.meshes( ioNode.meshindex( m ) );
		numTris += ( ioMesh.faces().size() / sizeof( IceMaths::IndexedTriangle ) );
	}
}

void PathPlannerRecast::GatherModel( ModelCache & cache, const modeldata::Scene & ioScene, const modeldata::Node & ioNode, const IceMaths::Matrix4x4 & nodeXform )
{
	for ( int i = 0; i < ioNode.children_size(); ++i )
	{
		GatherModel( cache, ioScene, ioNode.children( i ), nodeXform );
	}

	for ( int m = 0; m < ioNode.meshindex_size(); ++m )
	{
		const modeldata::Mesh & ioMesh = ioScene.meshes( ioNode.meshindex( m ) );
		const auto materialIndex = ioMesh.materialindex();
		const modeldata::Material & ioMaterial = ioScene.materials( materialIndex );

		const int sf = ioMaterial.surfaceflags();
		const int cf = ioMaterial.contents();

		Material mtl;
		mtl.mName = ioMaterial.name();
		mtl.mSurface = (SurfaceFlags)gEngineFuncs->ConvertValue( sf, IEngineInterface::ConvertSurfaceFlags, IEngineInterface::ConvertGameToBot );
		mtl.mContents = (ContentFlags)gEngineFuncs->ConvertValue( cf, IEngineInterface::ConvertContentsFlags, IEngineInterface::ConvertGameToBot );
		
		// just skip some face types entirely
		if ( mtl.mSurface & ( SURFACE_NONSOLID | SURFACE_IGNORE | SURFACE_SKY ) )
			continue;

		const Vector3f * vertices = reinterpret_cast<const Vector3f*>( &ioMesh.vertices()[ 0 ] );

		const std::string & facesBytes = ioMesh.faces();
		const size_t numFaces = facesBytes.size() / sizeof( IceMaths::IndexedTriangle );

		const IceMaths::IndexedTriangle * faces = reinterpret_cast<const IceMaths::IndexedTriangle*>( &facesBytes[ 0 ] );
		
		for ( size_t i = 0; i < numFaces; ++i )
		{
			const Vector3f & ioV0 = vertices[ faces[ i ].mVRef[ 0 ] ];
			const Vector3f & ioV1 = vertices[ faces[ i ].mVRef[ 1 ] ];
			const Vector3f & ioV2 = vertices[ faces[ i ].mVRef[ 2 ] ];

			/*bool excluded = false;
			for ( size_t x = 0; x < mExclusionZones.size() && !excluded; ++x )
			{
			Triangle3f intrTri( ioV0, ioV1, ioV2 );
			Box3f intrBox;

			IntrTriangle3Box3f intr( intrTri, intrBox );
			excluded = intr.Test();
			}

			if ( excluded )
			continue;*/

			const aiVector3D v0( ioV0.X(), ioV0.Y(), ioV0.Z() );
			const aiVector3D v1( ioV1.X(), ioV1.Y(), ioV1.Z() );
			const aiVector3D v2( ioV2.X(), ioV2.Y(), ioV2.Z() );

			const Vector3f p0 = IceMaths::TransformPoint4x3( Convert( v0 ), nodeXform );
			const Vector3f p1 = IceMaths::TransformPoint4x3( Convert( v1 ), nodeXform );
			const Vector3f p2 = IceMaths::TransformPoint4x3( Convert( v2 ), nodeXform );

			// if no inclusion zones are set, we include everything by default
			cache.mModel->AddTri( p0, p1, p2, mtl );
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void PathPlannerRecast::CreateModels( GameEntity entity, const IceMaths::Matrix4x4 & baseXform, const modeldata::Scene & ioScene, const modeldata::Node & ioNode, bool baseStaticMesh )
{
	// accumulate the triangles associated with this node
	// so we can preallocate the vertex arrays
	size_t numTriangles = 0;
	size_t numMeshes = 0;
	CountStats( ioScene, ioNode, numTriangles, numMeshes );
	
	EntityInfo entInfo;

	if ( numTriangles > 0 )
	{
		IceMaths::Matrix4x4 nodeXform = Convert( ioNode );
				
		if ( ioNode.has_submodel() )
		{
			if ( !SUCCESS( gEngineFuncs->GetEntityForMapModel( ioNode.submodel(), entity ) ) )
				return;
		}

		if ( entity.IsValid() && IGame::GetEntityInfo( entity, entInfo ) )
		{
			const std::string entname = EngineFuncs::EntityName( entity, "<unknown>" );

			if ( !entInfo.mCategory.CheckFlag( ENT_CAT_OBSTACLE ) )
			{
				EngineFuncs::ConsoleMessage( va( "Entity '%s' not an obstacle", entname.c_str() ) );
				return;
			}

			EngineFuncs::ConsoleMessage( va( "Entity '%s' is an obstacle", entname.c_str() ) );
		}

		mModels.reserve( mModels.size() + numMeshes );

		mModels.push_back( ModelCache() );
		
		ModelCache & cache = mModels.back();
		cache.mEntity = entity;
		cache.mEntInfo = entInfo;
		cache.mSubModel = ioNode.has_submodel() ? ioNode.submodel() : -1;
		cache.mModel.reset( new CollisionModel() );
		cache.mActiveState = baseStaticMesh ? StateCollidable : StateUnknown;
		cache.mBaseStaticMesh = baseStaticMesh;

		// preallocate attributes list to limit dynamic resizing
		GatherModel( cache, ioScene, ioNode, IceMaths::Matrix4x4() );

		cache.mModel->Build( false );
		cache.mTransform.SetTransform( baseXform * nodeXform, false );
	}

	// see if we have any external references
	for ( int m = 0; m < ioNode.meshindex_size(); ++m )
	{
		const modeldata::Mesh & ioMesh = ioScene.meshes( ioNode.meshindex( m ) );

		// is this an external mesh reference?
		if ( ioMesh.externalref() )
		{
			const fs::path ext = fs::path( ioMesh.name() ).extension();
			if ( !ext.empty() )
			{
				fs::path mdlname( ioMesh.name() );

				GameModelInfo modelInfo;
				strncpy( modelInfo.mModelName, mdlname.string().c_str(), sizeof( modelInfo.mModelName ) );
				strncpy( modelInfo.mModelType, ext.string().c_str() + 1, ext.string().length() );

				gEngineFuncs->GetModel( modelInfo, allocatorBot );
				if ( modelInfo.mDataBuffer != NULL )
				{
					const IceMaths::Matrix4x4 nodeXform = Convert( ioNode );
					LoadModel( modelInfo, entity, nodeXform, true );
				}
				else if ( !modelInfo.mAABB.IsZero() )
				{
					CreateModels( GameEntity(), modelInfo.mAABB, true );
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void PathPlannerRecast::CreateModels( GameEntity entity, const AABB & localaabb, bool baseStaticMesh )
{
	Vector3f origin = Vector3f::ZERO;
	if ( !EngineFuncs::EntityPosition( entity, origin ) )
		return;

	Vector3f vFwd, vRight, vUp;
	if ( !EngineFuncs::EntityOrientation( entity, vFwd, vRight, vUp ) )
		return;

	const Vector3f extAxis0 = localaabb.GetAxisLength( 0 ) * 0.5f * vFwd;
	const Vector3f extAxis1 = localaabb.GetAxisLength( 1 ) * 0.5f * vRight;
	const Vector3f extAxis2 = localaabb.GetAxisLength( 2 ) * 0.5f * vUp;

	Vector3f center;
	localaabb.CenterPoint( center );

	Vector3f vertex[ 8 ];
	vertex[ 0 ] = center - extAxis0 - extAxis1 - extAxis2;
	vertex[ 1 ] = center + extAxis0 - extAxis1 - extAxis2;
	vertex[ 2 ] = center + extAxis0 + extAxis1 - extAxis2;
	vertex[ 3 ] = center - extAxis0 + extAxis1 - extAxis2;
	vertex[ 4 ] = center - extAxis0 - extAxis1 + extAxis2;
	vertex[ 5 ] = center + extAxis0 - extAxis1 + extAxis2;
	vertex[ 6 ] = center + extAxis0 + extAxis1 + extAxis2;
	vertex[ 7 ] = center - extAxis0 + extAxis1 + extAxis2;/**/
	
	mModels.push_back( ModelCache() );

	ModelCache & cache = mModels.back();
	cache.mEntity = entity;	
	cache.mModel.reset( new CollisionModel() );
	cache.mActiveState = StateUnknown;
	cache.mBaseStaticMesh = baseStaticMesh;
	
	static const int indices[ 36 ] =
	{
		// bottom
		0, 1, 2,
		0, 2, 3,

		0, 3, 4,
		3, 4, 7,

		0, 1, 4,
		1, 4, 5,

		1, 2, 5,
		2, 5, 6,

		2, 3, 6,
		3, 6, 7,

		// top
		4, 5, 6,
		4, 6, 7,
	};

	Material mat;
	mat.mName = va( "entity_%s", EngineFuncs::EntityName( entity ).c_str() );
	mat.mSurface = SURFACE_NONE;
	mat.mContents = CONT_PLYRCLIP;

	// build a model from the bounding box
	for ( int i = 0; i < 36; i += 3 )
	{
		// if no inclusion zones are set, we include everything by default
		cache.mModel->AddTri(
			vertex[ indices[ i + 0 ] ],
			vertex[ indices[ i + 1 ] ],
			vertex[ indices[ i + 2 ] ],
			mat );
	}

	cache.mModel->Build( false );
	cache.mTransform.SetPosition( origin, false );
	cache.mTransform.SetOrientation( Matrix3f( vFwd, -vRight, vUp, true ), false );
}

PathInterface * PathPlannerRecast::AllocPathInterface( Client * client )
{
	return new RecastPathInterface( client, this );
}

static void BuildNodeTree( aiNode * sceneNode, modeldata::Node * ioNode, const IceMaths::Matrix4x4 & xform )
{
	ioNode->set_name( sceneNode->mName.C_Str() );
	if ( !xform.IsIdentity() )
	{
		ioNode->mutable_transformation()->mutable_row0()->set_x( xform.GetRow( 0 ).x );
		ioNode->mutable_transformation()->mutable_row0()->set_y( xform.GetRow( 0 ).y );
		ioNode->mutable_transformation()->mutable_row0()->set_z( xform.GetRow( 0 ).z );
		ioNode->mutable_transformation()->mutable_row0()->set_w( xform.GetRow( 0 ).w );
		ioNode->mutable_transformation()->mutable_row1()->set_x( xform.GetRow( 1 ).x );
		ioNode->mutable_transformation()->mutable_row1()->set_y( xform.GetRow( 1 ).y );
		ioNode->mutable_transformation()->mutable_row1()->set_z( xform.GetRow( 1 ).z );
		ioNode->mutable_transformation()->mutable_row1()->set_w( xform.GetRow( 1 ).w );
		ioNode->mutable_transformation()->mutable_row2()->set_x( xform.GetRow( 2 ).x );
		ioNode->mutable_transformation()->mutable_row2()->set_y( xform.GetRow( 2 ).y );
		ioNode->mutable_transformation()->mutable_row2()->set_z( xform.GetRow( 2 ).z );
		ioNode->mutable_transformation()->mutable_row2()->set_w( xform.GetRow( 2 ).w );
		ioNode->mutable_transformation()->mutable_row3()->set_x( xform.GetRow( 3 ).x );
		ioNode->mutable_transformation()->mutable_row3()->set_y( xform.GetRow( 3 ).y );
		ioNode->mutable_transformation()->mutable_row3()->set_z( xform.GetRow( 3 ).z );
		ioNode->mutable_transformation()->mutable_row3()->set_w( xform.GetRow( 3 ).w );
	}

	if ( sceneNode->mMetaData )
	{
		aiString value;
		if ( sceneNode->mMetaData->Get( aiString( "submodel" ), value ) )
			ioNode->set_submodel( atoi( value.C_Str() ) );
	}

	for ( unsigned int i = 0; i < sceneNode->mNumMeshes; ++i )
	{
		ioNode->add_meshindex( sceneNode->mMeshes[ i ] );
	}

	for ( unsigned int i = 0; i < sceneNode->mNumChildren; ++i )
	{
		aiNode * childSceneNode = sceneNode->mChildren[ i ];

		IceMaths::Matrix4x4	xform = Convert( childSceneNode->mTransformation );
		modeldata::Node * childIONode = ioNode->add_children();
		BuildNodeTree( childSceneNode, childIONode, xform );
	}
}

void PathPlannerRecast::LoadModel( const GameModelInfo & modelInfo, GameEntity entity, const IceMaths::Matrix4x4 & xform, bool baseStaticMesh )
{
	LOGFUNC;
	
	Timer loadTimer;
	loadTimer.Reset();

	const uint32_t fileCrc = FileSystem::CalculateCrc( modelInfo.mDataBuffer, modelInfo.mDataBufferSize );

	LOG( "LoadModel " << modelInfo.mModelName );
	LOG( "LoadModel calculate crc [" << va( "0x%08x", fileCrc ) << "] in " << loadTimer.GetElapsedSeconds() << " seconds" );

	static bool loadFromCache = true;

	bool cachedFileLoaded = false;

	loadTimer.Reset();

	// first check if it's a model we can instance
	for ( size_t i = 0; i < mModels.size(); ++i )
	{
		if ( mModels[ i ].mModel->GetName() == modelInfo.mModelName )
		{
			EngineFuncs::ConsoleMessage( va("Instanceable model '%s'\n", modelInfo.mModelName) );
#if(0)
			mModels.push_back( ModelCache() );

			ModelCache & cache = mModels.back();
			cache.mEntity = entity;
			cache.mModel = mModels[ i ].mModel;
			cache.mActiveState = baseStaticMesh ? StateCollidable : StateUnknown;
			cache.mBaseStaticMesh = baseStaticMesh;

			cache.mTransform.SetTransform( nodeXform, false );
			return;
#endif
		}
	}
	
	modeldata::Scene ioScene;
	if ( mCacheDb != NULL && loadFromCache )
	{
		sqlite3_stmt * statement = NULL;
		if ( SQLITE_OK == CheckSqliteError( mCacheDb,
			sqlite3_prepare_v2( mCacheDb,
			"SELECT fullSize, modelData FROM modelCache WHERE ( name=? AND crc=? AND version=? )",
			-1, &statement, NULL ) ) )
		{
			sqlite3_bind_text( statement, 1, modelInfo.mModelName, strlen( modelInfo.mModelName ), NULL );
			sqlite3_bind_int( statement, 2, fileCrc );
			sqlite3_bind_int( statement, 3, VERSION_MODELCACHE );
		}

		if ( SQLITE_ROW == CheckSqliteError( mCacheDb, sqlite3_step( statement ) ) )
		{
			const int dbFileSize = sqlite3_column_int( statement, 0 );
			const void * dbFileData = sqlite3_column_blob( statement, 1 );
			const int dbFileDataSize = sqlite3_column_bytes( statement, 1 );

			boost::shared_array<char> decompressBuffer( new char[ dbFileSize ] );
			const int sizeD = fastlz_decompress( dbFileData, dbFileDataSize, decompressBuffer.get(), dbFileSize );

			if ( ( dbFileSize == sizeD ) && ioScene.ParseFromArray( decompressBuffer.get(), sizeD ) )
			{
				LOG( "LoadModel loaded from cache db " << loadTimer.GetElapsedSeconds() << " seconds" );
				cachedFileLoaded = true;
			}
		}

		CheckSqliteError( mCacheDb, sqlite3_finalize( statement ) );
	}

	// Make a cached version of the file to 
	if ( !cachedFileLoaded )
	{
		loadTimer.Reset();

		//const int ignoreSurfaces = (SurfaceFlags)gEngineFuncs->ConvertValue( SURFACE_IGNORE | SURFACE_NONSOLID, IEngineInterface::ConvertSurfaceFlags, IEngineInterface::ConvertBotToGame );
		//const int onlyContents = (ContentFlags)gEngineFuncs->ConvertValue( CONT_SOLID | CONT_PLYRCLIP | CONT_WATER | CONT_TRIGGER, IEngineInterface::ConvertContentsFlags, IEngineInterface::ConvertBotToGame );

		aiPropertyStore * props = NULL;// aiCreatePropertyStore();

		/*aiSetImportPropertyInteger( props, AI_CONFIG_IMPORT_Q3BSP_ALLOW_0_CONTENTS, 1 );
		aiSetImportPropertyInteger( props, AI_CONFIG_IMPORT_Q3BSP_ALLOW_0_SURFACES, 1 );

		aiSetImportPropertyInteger( props, AI_CONFIG_IMPORT_Q3BSP_FILTER_CONTENTS, onlyContents );
		aiSetImportPropertyInteger( props, AI_CONFIG_IMPORT_Q3BSP_IGNORE_SURFACES, ignoreSurfaces );*/

		const unsigned int flags = aiProcess_Triangulate /*| aiProcess_JoinIdenticalVertices*/;
		const aiScene * scene = aiImportFileFromMemoryWithProperties( (const char*)modelInfo.mDataBuffer, modelInfo.mDataBufferSize, flags, modelInfo.mModelType, props );
		if ( scene && scene->mRootNode )
		{
			IceMaths::Matrix4x4	nodeXform = Convert( scene->mRootNode->mTransformation );
			nodeXform.Scale( modelInfo.mScale[ 0 ], modelInfo.mScale[ 1 ], modelInfo.mScale[ 2 ] );

			// cache the materials
			ioScene.mutable_materials()->Reserve( scene->mNumMaterials );
			for ( unsigned int m = 0; m < scene->mNumMaterials; ++m )
			{
				aiMaterial * sceneMaterial = scene->mMaterials[ m ];

				modeldata::Material * ioMaterial = ioScene.add_materials();

				aiString matName;
				int sf = 0, cf = 0;

				sceneMaterial->Get( AI_MATKEY_NAME, matName );
				ioMaterial->set_name( matName.C_Str() );

				if ( sceneMaterial->Get( "SURFACEFLAGS", 0, 0, sf ) == aiReturn_SUCCESS )
					ioMaterial->set_surfaceflags( sf );
				if ( sceneMaterial->Get( "CONTENTFLAGS", 0, 0, cf ) == aiReturn_SUCCESS )
					ioMaterial->set_contents( cf );
			}

			// cache the meshes
			ioScene.mutable_meshes()->Reserve( scene->mNumMeshes );
			for ( unsigned int m = 0; m < scene->mNumMeshes; ++m )
			{
				aiMesh * sceneMesh = scene->mMeshes[ m ];
				modeldata::Mesh * ioMesh = ioScene.add_meshes();
				
				if ( sceneMesh->mName.length > 0 )
					ioMesh->set_name( sceneMesh->mName.C_Str() );
				
				ioMesh->set_materialindex( sceneMesh->mMaterialIndex );

				// meshes may have no vertices or faces if they are external mesh references
				if ( sceneMesh->mNumVertices > 0 )
				{
					std::vector<Vector3f> vertices( sceneMesh->mNumVertices );
					for ( unsigned int v = 0; v < sceneMesh->mNumVertices; ++v )
					{
						vertices[ v ] = Vector3f( sceneMesh->mVertices[ v ].x, sceneMesh->mVertices[ v ].y, sceneMesh->mVertices[ v ].z );
					}
					ioMesh->mutable_vertices()->insert( 0, (const char*)&vertices[ 0 ], sizeof( Vector3f ) * vertices.size() );
				}
				else
				{
					ioMesh->set_externalref( true );
				}
				
				if ( sceneMesh->mNumFaces > 0 )
				{
					std::vector<IceMaths::IndexedTriangle> faces;
					faces.reserve( sceneMesh->mNumFaces );

					for ( unsigned int f = 0; f < sceneMesh->mNumFaces; ++f )
					{
						const aiFace & meshFace = sceneMesh->mFaces[ f ];

						assert( meshFace.mNumIndices == 3 && "Unhandled PolyArea Type" );
						if ( meshFace.mNumIndices == 3 )
							faces.emplace_back( IceMaths::IndexedTriangle( meshFace.mIndices[ 0 ], meshFace.mIndices[ 1 ], meshFace.mIndices[ 2 ] ) );
					}
					ioMesh->mutable_faces()->insert( 0, (const char*)&faces[ 0 ], sizeof( IceMaths::IndexedTriangle ) * faces.size() );
				}
				else
				{
					ioMesh->set_externalref( true );
				}
			}
			
			// build the node tree
			scene->mRootNode->mName.Set( "root" );
			modeldata::Node * rootNode = ioScene.mutable_rootnode();
			BuildNodeTree( scene->mRootNode, rootNode, nodeXform );

			aiReleaseImport( scene );
		}
		aiReleasePropertyStore( props );

		if ( !ioScene.IsInitialized() )
		{
			EngineFuncs::ConsoleError( va( "Error Loading Model '%s'", modelInfo.mModelName ) );
			return;
		}
		
		// Save the cached file out for later loading
		std::string cachedFileData;
		if ( mCacheDb && ioScene.SerializeToString( &cachedFileData ) )
		{
			const size_t bufferSize = cachedFileData.size() + (size_t)( cachedFileData.size() * 0.1 );
			boost::shared_array<char> compressBuffer( new char[ bufferSize ] );
			const int sizeCompressed = fastlz_compress_level( 2, cachedFileData.c_str(), cachedFileData.size(), compressBuffer.get() );

			sqlite3_stmt * statement = NULL;
			if ( SQLITE_OK == CheckSqliteError( mCacheDb, sqlite3_prepare_v2( mCacheDb, "INSERT into modelCache ( name, crc, fullSize, modeldata, version ) VALUES( ?, ?, ?, ?, ? )", -1, &statement, NULL ) ) )
			{
				sqlite3_bind_text( statement, 1, modelInfo.mModelName, strlen( modelInfo.mModelName ), NULL );
				sqlite3_bind_int( statement, 2, fileCrc );
				sqlite3_bind_int( statement, 3, cachedFileData.size() );
				sqlite3_bind_blob( statement, 4, compressBuffer.get(), sizeCompressed, NULL );
				sqlite3_bind_int( statement, 5, VERSION_MODELCACHE );
				
				const int err = sqlite3_step( statement );
				if ( CheckSqliteError( mCacheDb, err ) != SQLITE_DONE )
				{
					EngineFuncs::ConsoleError( va( "Error Saving to model cache db '%s'", modelInfo.mModelName ) );
				}
			}
			CheckSqliteError( mCacheDb, sqlite3_finalize( statement ) );
		}
	}

	// Load from the custom format
	if ( ioScene.IsInitialized() )
	{
		// Send the data to the analytics viewer about this model
		if ( System::mInstance->mAnalytics && System::mInstance->mAnalytics->IsNetworkActive() )
		{
			std::string cachedFileData;
			if ( ioScene.SerializeToString( &cachedFileData ) )
			{
				Analytics::MessageUnion msgUnion;
				msgUnion.set_timestamp( 0 );

				Analytics::SystemModelData* mdlData = msgUnion.mutable_systemmodeldata();
				mdlData->set_compressiontype( Analytics::Compression_FastLZ );

				// compress the tile data
				/*const size_t bufferSize = cachedFileData.size() + (size_t)( cachedFileData.size() * 0.1 );
				boost::shared_array<char> compressBuffer( new char[ bufferSize ] );
				const int sizeCompressed = fastlz_compress_level( 2, cachedFileData.c_str(), cachedFileData.size(), compressBuffer.get() );
				*/

				mdlData->set_modelname( modelInfo.mModelName );
				mdlData->set_modelbytes( cachedFileData  );
				if ( msgUnion.IsInitialized() )
					System::mInstance->mAnalytics->AddEvent( msgUnion );
			}
		}

		Timer loadTimer;
		loadTimer.Reset();

		const modeldata::Node & ioNode = ioScene.rootnode();

		for ( int n = 0; n < ioNode.children_size(); ++n )
		{
			const modeldata::Node & childNode = ioNode.children( n );

			CreateModels( entity, xform, ioScene, childNode, baseStaticMesh );
		}

		LOG( "LoadWorldModel generated models in " << loadTimer.GetElapsedSeconds() << " seconds" );
	}
}

void PathPlannerRecast::LoadWorldModel()
{
	LOGFUNC;

	mNavigationBounds.Clear();

	GameModelInfo modelInfo;
	gEngineFuncs->GetWorldModel( modelInfo, allocatorBot );

	if ( modelInfo.mDataBuffer == NULL )
		return;

	LoadModel( modelInfo, GameEntity(), IceMaths::Matrix4x4(), true );
	
	// calculate the world bounds
	mNavigationBounds.Clear();
	for ( size_t i = 0; i < mModels.size(); ++i )
	{
		if ( mModels[ i ].mModel->IsValid() )
			mNavigationBounds.ExpandAABB( ComputeAABB( mModels[ i ].mModel->GetWorldOBB( mModels[ i ].mTransform ) ) );
	}

	allocatorBot.FreeMemory( modelInfo.mDataBuffer );
}

//////////////////////////////////////////////////////////////////////////

void PathPlannerRecast::EntityCreated( const EntityInstance &ei )
{
	if ( ei.mEntInfo.mCategory.CheckFlag( ENT_CAT_OBSTACLE ) )
	{
		for ( size_t i = 0; i < mModels.size(); ++i )
		{
			if ( mModels[ i ].mEntity == ei.mEntity )
				return;
		}

		Timer loadTimer;
		loadTimer.Reset();

		GameModelInfo modelInfo;
		gEngineFuncs->GetEntityModel( ei.mEntity, modelInfo, allocatorBot );

		if ( modelInfo.mDataBuffer != NULL )
		{
			LoadModel( modelInfo, ei.mEntity, IceMaths::Matrix4x4(), false );
			allocatorBot.FreeMemory( modelInfo.mDataBuffer );
		}
		else if ( !modelInfo.mAABB.IsZero() )
		{
			CreateModels( ei.mEntity, modelInfo.mAABB, false );
		}
	}
}

void PathPlannerRecast::EntityDeleted( const EntityInstance &ei )
{
	for ( size_t i = 0; i < mModels.size(); ++i )
	{
		if ( mModels[ i ].mEntity == ei.mEntity )
		{
			mModels[ i ].mActiveState = StateMarkedForDelete;
		}
	}
}

bool PathPlannerRecast::GetAimedAtModel( size_t & modelIndex, size_t & triangleIndex, Vector3f & hitPos, Vector3f & hitNormal )
{
	Vector3f rayStart, rayDir;
	if ( Utils::GetLocalEyePosition( rayStart ) && Utils::GetLocalFacing( rayDir ) )
	{
		float closestDist = FLT_MAX;
		size_t closestIndex = 0;
		CollisionModel::RayResult bestResult;

		for ( size_t i = 0; i < mModels.size(); ++i )
		{
			CollisionModel::RayResult result;
			if ( mModels[ i ].mModel->CollideRay( mModels[ i ].mTransform, result, rayStart, rayStart + rayDir * 4096.0f ) )
			{
				const float dist = ( rayStart - result.mHitPos ).Length();
				if ( dist < closestDist )
				{
					closestDist = dist;
					closestIndex = i;
					bestResult = result;
				}
			}
		}

		if ( closestDist < FLT_MAX )
		{
			modelIndex = closestIndex;
			triangleIndex = bestResult.mHitTriangle;
			hitPos = bestResult.mHitPos;
			hitNormal = bestResult.mHitNormal;
			return true;
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////

void OffMeshConnection::Render()
{
	std::string areaStr, flagStr;
	PolyAreaToString( mAreaType, areaStr );
	PolyFlagsToString( mFlags, flagStr );

	Vector3f lastPt = mEntry;
	for ( size_t i = 0; i < mNumPts; ++i )
	{
		RenderBuffer::AddLine( lastPt, mIntermediates[ i ], COLOR::GREEN );
		lastPt = mIntermediates[ i ];
	}

	RenderBuffer::AddCircle( mEntry, mRadius, COLOR::GREEN );
	RenderBuffer::AddCircle( mExit, mRadius, COLOR::GREEN );
	RenderBuffer::AddLine( lastPt, mExit, COLOR::GREEN );
	RenderBuffer::AddString3d( mEntry + Vector3f( 0.f, 0.f, 40.f ), COLOR::BLUE, va( "%s (%s)", areaStr.c_str(), flagStr.c_str() ) );
	RenderBuffer::AddString3d( mExit + Vector3f( 0.f, 0.f, 40.f ), COLOR::BLUE, va( "%s (%s)", areaStr.c_str(), flagStr.c_str() ) );
}
