////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "PathPlannerRecast.h"
#include "ScriptManager.h"
#include "IGameManager.h"
#include "IGame.h"
#include "Client.h"

using namespace std;

Vector3f localToRc( const float * vec );
Vector3f rcToLocal( const float * vec );

//////////////////////////////////////////////////////////////////////////

void PathPlannerRecast::InitCommands()
{
	PathPlannerBase::InitCommands();

	SetEx( "nav_save", "Save current navigation to disk",
		this, &PathPlannerRecast::cmdNavSave );
	SetEx( "nav_load", "Load last saved navigation from disk",
		this, &PathPlannerRecast::cmdNavLoad );
	SetEx( "nav_view", "Turn on/off navmesh visibility.",
		this, &PathPlannerRecast::cmdNavView );
	SetEx( "nav_viewconnections", "Turn on/off navmesh connection visibility.",
		this, &PathPlannerRecast::cmdNavViewConnections );

	SetEx( "nav_addexclusionzone", "Adds a bounding area to include in navigation.",
		this, &PathPlannerRecast::cmdNavAddExclusionZone );
	SetEx( "nav_connectionadd", "Adds an off-mesh connection.",
		this, &PathPlannerRecast::cmdNavAddOffMeshConnection );	
	SetEx( "nav_commit", "Completes the current tool.",
		this, &PathPlannerRecast::cmdCommitPoly );
	//////////////////////////////////////////////////////////////////////////
	SetEx( "nav_build", "Builds navigation for all tiles.", this, &PathPlannerRecast::cmdBuildNav );
	SetEx( "nav_buildtile", "Builds navigation for current tile.", this, &PathPlannerRecast::cmdBuildNavTile );
	//////////////////////////////////////////////////////////////////////////
	SetEx( "nav_autofeature", "Create off-mesh links for jump pads, teleporters, etc.",
		this, &PathPlannerRecast::cmdAutoBuildFeatures );
	SetEx( "nav_saveobj", "Saves obj file of base mesh.", this, &PathPlannerRecast::cmdSaveToObjFile );

	SetEx( "nav_modelenable", "Disables the currently looked-at model.", this, &PathPlannerRecast::cmdModelEnable );
	SetEx( "nav_modelmover", "Enableds mover flag on look-at model.", this, &PathPlannerRecast::cmdModelMover );
	SetEx( "nav_modelnonsolid", "Enableds non solid flag on look-at model.", this, &PathPlannerRecast::cmdModelNonSolid );
}

void PathPlannerRecast::cmdNavSave( const StringVector &_args )
{
	if ( !m_PlannerFlags.CheckFlag( NAV_VIEW ) )
		return;

	// this should clear the non world models so the tiles that they effect are rebuilt
	mBuildBaseNav = true;
}

void PathPlannerRecast::cmdNavLoad( const StringVector &_args )
{
	if ( !m_PlannerFlags.CheckFlag( NAV_VIEW ) )
		return;

	if ( Load( gEngineFuncs->GetMapName() ) )
	{
		EngineFuncs::ConsoleMessage( "Loaded Nav." );
	}
	else
		EngineFuncs::ConsoleError( "ERROR Loading Nav." );
}

void PathPlannerRecast::cmdNavView( const StringVector &_args )
{
	const char *strUsage [] =
	{
		"nav_view enable[bool]",
		"> enable: Enable nav rendering. true/false/on/off/1/0",
	};

	CHECK_NUM_PARAMS( _args, 2, strUsage );
	CHECK_BOOL_PARAM( bEnable, 1, strUsage );
	m_PlannerFlags.SetFlag( PathPlannerRecast::NAV_VIEW, bEnable != 0 );
}

void PathPlannerRecast::cmdNavViewConnections( const StringVector &_args )
{
	const char *strUsage [] =
	{
		"nav_viewconnections enable[bool]",
		"> enable: Enable nav connection rendering. true/false/on/off/1/0",
	};

	CHECK_NUM_PARAMS( _args, 2, strUsage );
	CHECK_BOOL_PARAM( bEnable, 1, strUsage );
	m_PlannerFlags.SetFlag( PathPlannerRecast::NAV_VIEWCONNECTIONS, bEnable != 0 );
}

void PathPlannerRecast::cmdBuildNav( const StringVector &_args )
{
	if ( !m_PlannerFlags.CheckFlag( NAV_VIEW ) )
		return;

	BuildNav();
}

void PathPlannerRecast::cmdBuildNavTile( const StringVector &_args )
{
	if ( !m_PlannerFlags.CheckFlag( NAV_VIEW ) )
		return;

	BuildNavTile();
}

class ToolCreateBounds : public EditTool < PathPlannerRecast >
{
public:
	ToolCreateBounds()
		: EditTool( "Create Exclusion Bounds" )
	{
	}

	virtual bool Enter( PathPlannerRecast * system )
	{
		Vector3f localPos;
		Utils::GetLocalPosition( localPos );

		mBounds.Clear();
		mBounds.ExpandPt( localPos );

		return true;
	}
	virtual bool ReEnter( PathPlannerRecast * system )
	{
		system->mExclusionZones.push_back( mBounds );
		return false;
	}
	virtual bool Update( PathPlannerRecast * system )
	{
		Vector3f localPos;
		Utils::GetLocalPosition( localPos );
		mBounds.ExpandPt( localPos );

		RenderBuffer::AddAABB( mBounds, COLOR::RED );

		return true;
	}
	virtual void Commit( PathPlannerRecast * system )
	{
	}

	AxisAlignedBox3f	mBounds;
};

class ToolCreateExclusion : public ToolCreateBounds
{
public:
	ToolCreateExclusion() : ToolCreateBounds()
	{
	}
};

class ToolCreateOffMeshConnection : public EditTool < PathPlannerRecast >
{
public:
	ToolCreateOffMeshConnection( const OffMeshConnection & conn )
		: EditTool( "Create Off Mesh Connection" )
		, mConn( conn )
	{
	}

	virtual bool Enter( PathPlannerRecast * system )
	{
		Utils::GetLocalAimPoint( mConn.mEntry );
		return true;
	}
	virtual bool ReEnter( PathPlannerRecast * system )
	{
		return false;
	}
	virtual bool Update( PathPlannerRecast * system )
	{
		int contents = 0, surface = 0;
		Vector3f vAimPt, vAimNormal;
		Utils::GetLocalAimPoint( vAimPt, &vAimNormal, TR_MASK_FLOODFILLENT, &contents, &surface );

		if ( mConn.mAreaType == NAVAREA_LADDER && ( surface & SURFACE_LADDER ) != 0 )
		{
			if ( mConn.mNumPts == 0 )
			{
				mConn.mIntermediates[ 0 ] = vAimPt;
				mConn.mNumPts = 1;
			}
			else
			{
				mConn.mIntermediates[ 0 ].X() = vAimPt.X();
				mConn.mIntermediates[ 0 ].Y() = vAimPt.Y();
			}
		}

		mConn.mExit = vAimPt;

		if ( mConn.mNumPts > 0 )
		{
			mConn.mIntermediates[ 0 ].Z() = std::max( mConn.mIntermediates[ 0 ].Z(), mConn.mExit.Z() );
		}

		mConn.Render();
		return true;
	}
	virtual void Commit( PathPlannerRecast * system )
	{
		system->mOffMeshConnections.push_back( mConn );
		system->MarkTileForBuilding( mConn.mEntry );
		system->MarkTileForBuilding( mConn.mExit );
	}

	OffMeshConnection mConn;
};

void PathPlannerRecast::cmdNavAddExclusionZone( const StringVector &_args )
{
	if ( !m_PlannerFlags.CheckFlag( NAV_VIEW ) )
		return;

	SetCurrentTool<ToolCreateExclusion>();
}

void PathPlannerRecast::cmdNavAddOffMeshConnection( const StringVector &_args )
{
	if ( !m_PlannerFlags.CheckFlag( NAV_VIEW ) )
		return;
	
	OffMeshConnection conn;

	if ( _args.size() < 3 )
		goto showUsage;
	
	float radius = 0.0f;
	size_t area = 0;
	size_t flags = 0;
	if ( !Utils::ConvertString( _args[ 1 ], radius ) )
		goto showUsage;
	if ( !StringToPolyArea( _args[ 2 ], area ) )
		goto showUsage;
	if ( !StringToPolyFlags( _args[ 3 ], flags ) )
		goto showUsage;

	conn.mRadius = radius;
	conn.mAreaType = (NavArea)area;
	conn.mFlags = (NavAreaFlags)flags;
	SetCurrentTool<ToolCreateOffMeshConnection>( conn );
	return;

showUsage:
	std::string areasStr, flagsStr;
	for ( size_t i = 0; i < sNumPolyAreas; ++i )
		areasStr += va( "%s%s", areasStr.empty() ? "" : ", ", sPolyAreas[ i ].mName );
	for ( size_t i = 0; i < sNumPolyFlags; ++i )
		flagsStr += va( "%s%s", flagsStr.empty() ? "" : ", ", sPolyFlags[ i ].mName );

	EngineFuncs::ConsoleError( "nav_addconnection radius[#] type[string] flag(s)[string]..." );
	EngineFuncs::ConsoleError( va( "	radius - radius of connection" ) );
	EngineFuncs::ConsoleError( va( "	type - one of (%s)", areasStr.c_str() ) );
	EngineFuncs::ConsoleError( va( "	flags - combination of (%s)", flagsStr.c_str() ) );
}

void PathPlannerRecast::cmdAutoBuildFeatures( const StringVector &_args )
{
	if ( !m_PlannerFlags.CheckFlag( NAV_VIEW ) )
		return;

	const int iMaxFeatures = 128;
	AutoNavFeature features[ iMaxFeatures ];
	int iNumFeatures = gEngineFuncs->GetAutoNavFeatures( features, iMaxFeatures );
	for ( int i = 0; i < iNumFeatures; ++i )
	{
		const float fTime = 30.f;

		if ( features[ i ].m_Type == ENT_CLASS_GENERIC_TELEPORTER )
		{
			OffMeshConnection conn;
			conn.mEntry = Vector3f( features[ i ].m_Position );
			conn.mExit = Vector3f( features[ i ].m_TargetPosition );
			conn.mAreaType = NAVAREA_TELEPORT;
			conn.mFlags = NAVFLAGS_NONE;
			conn.mRadius = 16.0f;
			conn.mBiDir = false;
			mOffMeshConnections.push_back( conn );

			RenderBuffer::AddLine( conn.mEntry, conn.mExit, COLOR::MAGENTA, fTime );
		}
	}
	//EngineFuncs::ConsoleMessage( va( "Found %d nav features.", iNumFeatures ) );
}

void PathPlannerRecast::cmdCommitPoly( const StringVector &_args )
{
	if ( !m_PlannerFlags.CheckFlag( NAV_VIEW ) )
		return;

	if ( mCurrentTool )
	{
		mCurrentTool->Commit( this );

		// should we always delete the tool?
		OB_DELETE( mCurrentTool );
	}
}

//////////////////////////////////////////////////////////////////////////

void PathPlannerRecast::cmdSaveToObjFile( const StringVector &_args )
{
	if ( _args.size() < 2 )
	{
		EngineFuncs::ConsoleError( "nav_saveobj filename" );
		return;
	}
	
	size_t totalTriangles = 0;

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

		totalTriangles += model.mModel->GetNumTris();
	}

	RenderBuffer::TriList triangles;
	triangles.reserve( totalTriangles );

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
		
		// for now use all triangles for testing
		for ( size_t t = 0; t < model.mModel->GetNumTris(); ++t )
		{
			size_t materialIndex;
			RenderBuffer::Triangle tri;
			tri.c = COLOR::GREEN;
			model.mModel->GetTriangle( model.mTransform, t, tri.v[ 0 ], tri.v[ 1 ], tri.v[ 2 ], materialIndex );

			const Material & mat = model.mModel->GetMaterial( materialIndex );
			if ( mat.mSurface & ( SURFACE_NONSOLID | SURFACE_IGNORE | SURFACE_SKY ) )
				continue;

			triangles.push_back( tri );
		}
	}

	const std::string objfilename = _args[ 1 ] + ".obj";
	const std::string matfilename = _args[ 1 ] + ".mat";

	File objFile, matFile;
	if ( objFile.OpenForWrite( objfilename.c_str(), File::Text, false ) &&
		matFile.OpenForWrite( matfilename.c_str(), File::Text, false ) )
	{
		// material
		matFile.WriteString( va( "# Materials Navmesh - %s\n\n", gEngineFuncs->GetMapName() ).c_str() );
		matFile.WriteString( "Kd 0.000 1.000 0.000     # green" );

		// Mesh
		objFile.WriteString( va( "# Navmesh - %s\n\n", gEngineFuncs->GetMapName() ).c_str() );
		objFile.WriteString( va( "mltlib %s\n", matfilename.c_str() ).c_str() );
		objFile.WriteString( va( "# Vertices %d\n", triangles.size() * 3 ).c_str() );
		
		for ( size_t t = 0; t < triangles.size(); ++t )
		{
			for ( size_t i = 0; i < 3; ++i )
			{
				Vector3f vecRecast = /*localToRc*/( triangles[ t ].v[ i ] );

				objFile.WriteString( va( "v %f %f %f\n",
					vecRecast.X(),
					vecRecast.Y(),
					vecRecast.Z() ).c_str() );
			}
		}

		objFile.WriteString( va( "\n# Faces %d\n", triangles.size() ).c_str() );

		for ( size_t t = 0; t < triangles.size(); ++t )
		{
			objFile.WriteString( va( "f %d %d %d\n", t * 3 + 1, t * 3 + 2, t * 3 + 3 ).c_str() );
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void PathPlannerRecast::cmdModelEnable( const StringVector &_args )
{
	const char *strUsage [] =
	{
		"nav_modelenable enable[bool]",
		"> enable: Enable/Disable currently viewed model",
	};

	CHECK_NUM_PARAMS( _args, 2, strUsage );
	CHECK_BOOL_PARAM( enable, 1, strUsage );

	size_t modelIndex, triangleIndex;
	Vector3f aimPos, aimNormal;
	if ( GetAimedAtModel( modelIndex, triangleIndex, aimPos, aimNormal ) )
	{
		ModelCache & mdl = mModels[ modelIndex ];
		mdl.mDisabled = !enable;
	}
}

//////////////////////////////////////////////////////////////////////////

void PathPlannerRecast::cmdModelMover( const StringVector &_args )
{
	const char *strUsage [] =
	{
		"nav_modelmover enable[bool]",
		"> enable: Enable/Disable movement detection of current model to rebake navigation",
	};

	CHECK_NUM_PARAMS( _args, 2, strUsage );
	CHECK_BOOL_PARAM( enable, 1, strUsage );

	size_t modelIndex, triangleIndex;
	Vector3f aimPos, aimNormal;
	if ( GetAimedAtModel( modelIndex, triangleIndex, aimPos, aimNormal ) )
	{
		ModelCache & mdl = mModels[ modelIndex ];
		mdl.mMover = enable;
	}
}

//////////////////////////////////////////////////////////////////////////

void PathPlannerRecast::cmdModelNonSolid( const StringVector &_args )
{
	const char *strUsage [] =
	{
		"nav_modelnonsolid enable[bool]",
		"> enable: Enable/Disable whether model is solid or is marked off as a region",
	};

	CHECK_NUM_PARAMS( _args, 2, strUsage );
	CHECK_BOOL_PARAM( enable, 1, strUsage );

	size_t modelIndex, triangleIndex;
	Vector3f aimPos, aimNormal;
	if ( GetAimedAtModel( modelIndex, triangleIndex, aimPos, aimNormal ) )
	{
		ModelCache & mdl = mModels[ modelIndex ];
		mdl.mNonSolid = enable;
	}
}
