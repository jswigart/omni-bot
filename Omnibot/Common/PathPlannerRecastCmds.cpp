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

bool NavNodeSort( const NodePtr& n0, const NodePtr& n1 )
{
	if ( n0->mSubModel < n1->mSubModel )
		return true;
	else if ( n0->mSubModel > n1->mSubModel )
		return false;

	if ( n0->mStaticModel < n1->mStaticModel )
		return true;
	else if ( n0->mStaticModel > n1->mStaticModel )
		return false;
	else if ( n0->mDisplacement > n1->mDisplacement )
		return false;
	
	return n0->mEntityName < n1->mEntityName;
}

//////////////////////////////////////////////////////////////////////////

void PathPlannerRecast::InitCommands()
{
	PathPlannerBase::InitCommands();

	SetEx( "nav_save", "Save current navigation to disk", this, &PathPlannerRecast::cmdNavSave );
	SetEx( "nav_load", "Load last saved navigation from disk", this, &PathPlannerRecast::cmdNavLoad );
	SetEx( "nav_view", "Turn on/off navmesh visibility.",  this, &PathPlannerRecast::cmdNavView );
	SetEx( "nav_list", "Show all nodes.", this, &PathPlannerRecast::cmdNavList );
	SetEx( "nav_viewmodels", "Turn on/off model info visibility.", this, &PathPlannerRecast::cmdNavViewModels );
	SetEx( "nav_viewconnections", "Turn on/off navmesh connection visibility.",  this, &PathPlannerRecast::cmdNavViewConnections );
	SetEx( "nav_addexclusionzone", "Adds a bounding area to include in navigation.", this, &PathPlannerRecast::cmdNavAddExclusionZone );
	SetEx( "nav_addlink", "Adds an off-mesh connection.", this, &PathPlannerRecast::cmdNavAddLink );
	
	SetEx( "nav_commit", "Completes the current tool.", this, &PathPlannerRecast::cmdCommitPoly );
	//////////////////////////////////////////////////////////////////////////
	SetEx( "nav_build", "Builds navigation for all tiles.", this, &PathPlannerRecast::cmdBuildNav );
	SetEx( "nav_buildtile", "Builds navigation for current tile.", this, &PathPlannerRecast::cmdBuildNavTile );
	//////////////////////////////////////////////////////////////////////////
	SetEx( "nav_autofeature", "Create off-mesh links for jump pads, teleporters, etc.", this, &PathPlannerRecast::cmdAutoBuildFeatures );
	SetEx( "nav_saveobj", "Saves obj file of base mesh.", this, &PathPlannerRecast::cmdSaveToObjFile );

	SetEx( "nav_modelenable", "Disables the currently looked-at model.", this, &PathPlannerRecast::cmdModelEnable );
	SetEx( "nav_modelshape", "Sets the shape mode for the model.", this, &PathPlannerRecast::cmdModelShape );
	SetEx( "nav_modelsolid", "Enables solid flag on look-at model.", this, &PathPlannerRecast::cmdModelSetSolid );
	SetEx( "nav_modeldynamic", "Enables mover flag on look-at model.", this, &PathPlannerRecast::cmdModelDynamic );
	SetEx( "nav_modelsettrisurface", "Enables mover flag on look-at model.", this, &PathPlannerRecast::cmdModelSetTriangleSurface );
	SetEx( "nav_modelsetarea", "Sets an area override for the region baked in by this model.", this, &PathPlannerRecast::cmdModelSetAreaFlag );
	SetEx( "nav_addbuildthread", "Sets an area override for the region baked in by this model.", this, &PathPlannerRecast::cmdNavAddBuildThread );	
}

void PathPlannerRecast::cmdNavSave( const StringVector & args )
{	
	if ( mFlags.mViewMode == 0 )
		return;

	// saves the navigation when the build queue is empty
	mDeferredSaveNav = true;
}

void PathPlannerRecast::cmdNavLoad( const StringVector & args )
{
	if ( mFlags.mViewMode == 0 )
		return;

	if ( Load( gEngineFuncs->GetMapName() ) )
	{
		EngineFuncs::ConsoleMessage( "Loaded Nav." );
	}
	else
		EngineFuncs::ConsoleError( "ERROR Loading Nav." );
}

void PathPlannerRecast::cmdNavView( const StringVector & args )
{
	const char *strUsage [] =
	{
		"nav_view enable[int]",
		"> enable: Enable nav rendering. 0-off,1-show active, 2-show all",
	};

	CHECK_NUM_PARAMS( args, 2, strUsage );
	CHECK_INT_PARAM( viewmode, 1, strUsage );

	mFlags.mViewMode = viewmode;
}

void PathPlannerRecast::cmdNavList( const StringVector & args )
{
	std::vector<NodePtr> nodes;
	std::vector<NodePtr> open;

	open.push_back( mCollision.mRootNode );
	while ( !open.empty() )
	{
		NodePtr n = open.back();
		open.pop_back();

		nodes.push_back( n );

		for ( size_t i = 0; i < n->mChildren.size(); ++i )
		{
			open.push_back( n->mChildren[ i ] );
		}
	}

	std::sort( nodes.begin(), nodes.end(), NavNodeSort );

	EngineFuncs::ConsoleMessage( va( "Nodes(%d)", nodes.size() ).c_str() );
	for ( size_t i = 0; i < nodes.size(); ++i )
	{
		const std::string& mdlName = nodes[ i ]->mModel ? nodes[ i ]->mModel->GetName() : "";

		std::string info;
		if ( nodes[ i ]->mSubModel >= 0 )
			info += va( "submdl(%d) ", nodes[ i ]->mSubModel ).c_str();
		if ( nodes[ i ]->mStaticModel >= 0 )
			info += va( "staticmdl(%d) ", nodes[ i ]->mStaticModel ).c_str();
		if ( nodes[ i ]->mDisplacement >= 0 )
			info += va( "displ(%d) ", nodes[ i ]->mDisplacement ).c_str();
		
		if ( !nodes[ i ]->mEntityName.empty() )
			info += va( "name(%s) ", nodes[ i ]->mEntityName.c_str() ).c_str();
		if ( !mdlName.empty() )
			info += va( "model(%s) ", mdlName.c_str() ).c_str();

		EngineFuncs::ConsoleMessage( va( "%d: %s", i, info.c_str() ).c_str() );
	}
}

void PathPlannerRecast::cmdNavViewModels( const StringVector & args )
{
	const char *strUsage [] =
	{
		"nav_viewmodels enable[int]",
		"> enable: Enable extra model rendering. 0-off,1-show bounds, 2-show axis",
	};

	CHECK_NUM_PARAMS( args, 2, strUsage );
	CHECK_INT_PARAM( viewmode, 1, strUsage );

	mFlags.mViewModels = viewmode;
}

void PathPlannerRecast::cmdNavViewConnections( const StringVector & args )
{
	const char *strUsage [] =
	{
		"nav_viewconnections enable[bool]",
		"> enable: Enable nav connection rendering. true/false/on/off/1/0",
	};

	CHECK_NUM_PARAMS( args, 2, strUsage );
	CHECK_BOOL_PARAM( bEnable, 1, strUsage );

	mFlags.mViewConnections = bEnable;
}

void PathPlannerRecast::cmdBuildNav( const StringVector & args )
{
	if ( mFlags.mViewMode == 0 )
		return;

	BuildNav( false );
}

void PathPlannerRecast::cmdBuildNavTile( const StringVector & args )
{
	if ( mFlags.mViewMode == 0 )
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

		if ( mConn.mAreaFlags == NAVFLAGS_LADDER && ( surface & SURFACE_LADDER ) != 0 )
		{
			if ( mConn.mVertices.size() == 0 )
			{
				mConn.mVertices.push_back( vAimPt );
			}
			else
			{
				mConn.mVertices.back().X() = vAimPt.X();
				mConn.mVertices.back().Y() = vAimPt.Y();
			}
		}

		mConn.mExit = vAimPt;

		if ( mConn.mVertices.size() > 0 )
		{
			mConn.mVertices[ 0 ].Z() = std::max( mConn.mVertices[ 0 ].Z(), mConn.mExit.Z() );
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

void PathPlannerRecast::cmdNavAddExclusionZone( const StringVector & args )
{
	if ( mFlags.mViewMode == 0 )
		return;

	SetCurrentTool<ToolCreateExclusion>();
}

void PathPlannerRecast::cmdNavAddLink( const StringVector & args )
{
	if ( mFlags.mViewMode == 0 )
		return;

	OffMeshConnection conn;

	if ( args.size() < 3 )
		goto showUsage;

	if ( !Utils::ConvertString( args[ 1 ], conn.mRadius ) )
		goto showUsage;
	if ( !NavAreaFlagsEnum::ValueForName( args[ 2 ].c_str(), conn.mAreaFlags ) )
		goto showUsage;
	
	SetCurrentTool<ToolCreateOffMeshConnection>( conn );
	return;

showUsage:
	
	std::string flagsStr;
	for ( size_t i = 0; i < NavAreaFlagsEnum::sKeyValCount; ++i )
		flagsStr += va( "%s%s", flagsStr.empty() ? "" : ", ", NavAreaFlagsEnum::sKeyVal[ i ].mName ).c_str();

	EngineFuncs::ConsoleError( "nav_addlink radius[#] type[string] flag(s)[string].." );
	EngineFuncs::ConsoleError( va( "	radius - radius of connection" ).c_str() );
	EngineFuncs::ConsoleError( va( "	flags - combination of (%s)", flagsStr.c_str() ).c_str() );
}

void PathPlannerRecast::cmdAutoBuildFeatures( const StringVector & args )
{
	if ( mFlags.mViewMode == 0 )
		return;

	const int iMaxFeatures = 128;
	AutoNavFeature features[ iMaxFeatures ];
	int iNumFeatures = gEngineFuncs->GetAutoNavFeatures( features, iMaxFeatures );
	for ( int i = 0; i < iNumFeatures; ++i )
	{
		const float fTime = 30.f;

		if ( features[ i ].mEntityInfo.mGroup == ENT_GRP_TELEPORTER )
		{
			OffMeshConnection conn;
			conn.mEntry = Vector3f( features[ i ].mPosition );
			conn.mExit = Vector3f( features[ i ].mTargetPosition );
			conn.mAreaFlags = NAVFLAGS_TELEPORT;
			conn.mRadius = 16.0f;
			conn.mBiDirectional = false;
			mOffMeshConnections.push_back( conn );

			RenderBuffer::AddLine( conn.mEntry, conn.mExit, COLOR::MAGENTA, fTime );
		}
	}
	//EngineFuncs::ConsoleMessage( va( "Found %d nav features.", iNumFeatures ) );
}

void PathPlannerRecast::cmdCommitPoly( const StringVector & args )
{
	if ( mCurrentTool )
	{
		mCurrentTool->Commit( this );

		// should we always delete the tool?
		OB_DELETE( mCurrentTool );
	}
}

//////////////////////////////////////////////////////////////////////////

void PathPlannerRecast::cmdSaveToObjFile( const StringVector & args )
{
	if ( mFlags.mViewMode == 0 )
		return;

	if ( args.size() < 2 )
	{
		EngineFuncs::ConsoleError( "nav_saveobj filename" );
		return;
	}

	//size_t totalTriangles = 0;

	//for ( size_t m = 0; m < mModels.size(); ++m )
	//{
	//	const ModelCache & model = mModels[ m ];

	//	// skip models not currently active
	//	switch ( mModels[ m ].mActiveState )
	//	{
	//		case StateCollidable:
	//			break;
	//		case StateUnknown:
	//		default:
	//			continue;
	//	}

	//	totalTriangles += model.mModel->GetNumTris();
	//}

	//RenderBuffer::TriList triangles;
	//triangles.reserve( totalTriangles );

	//for ( size_t m = 0; m < mModels.size(); ++m )
	//{
	//	const ModelCache & model = mModels[ m ];

	//	// skip models not currently active
	//	switch ( mModels[ m ].mActiveState )
	//	{
	//		case StateCollidable:
	//			break;
	//		case StateUnknown:
	//		default:
	//			continue;
	//	}
	//	
	//	// for now use all triangles for testing
	//	for ( size_t t = 0; t < model.mModel->GetNumTris(); ++t )
	//	{
	//		size_t materialIndex;
	//		RenderBuffer::Triangle tri;
	//		tri.c = COLOR::GREEN;
	//		model.mModel->GetTriangle( model.mTransform, t, tri.v[ 0 ], tri.v[ 1 ], tri.v[ 2 ], materialIndex );

	//		const Material & mat = model.mModel->GetMaterial( materialIndex );
	//		if ( mat.mSurface & ( SURFACE_NONSOLID | SURFACE_IGNORE | SURFACE_SKY ) )
	//			continue;

	//		triangles.push_back( tri );
	//	}
	//}

	//const std::string objfilename = args[ 1 ] + ".obj";
	//const std::string matfilename = args[ 1 ] + ".mat";

	//File objFile, matFile;
	//if ( objFile.OpenForWrite( objfilename.c_str(), File::Text, false ) &&
	//	matFile.OpenForWrite( matfilename.c_str(), File::Text, false ) )
	//{
	//	// material
	//	matFile.WriteString( va( "# Materials Navmesh - %s\n\n", gEngineFuncs->GetMapName() ).c_str() );
	//	matFile.WriteString( "Kd 0.000 1.000 0.000     # green" );

	//	// Mesh
	//	objFile.WriteString( va( "# Navmesh - %s\n\n", gEngineFuncs->GetMapName() ).c_str() );
	//	objFile.WriteString( va( "mltlib %s\n", matfilename.c_str() ).c_str() );
	//	objFile.WriteString( va( "# Vertices %d\n", triangles.size() * 3 ).c_str() );
	//	
	//	for ( size_t t = 0; t < triangles.size(); ++t )
	//	{
	//		for ( size_t i = 0; i < 3; ++i )
	//		{
	//			Vector3f vecRecast = /*localToRc*/( triangles[ t ].v[ i ] );

	//			objFile.WriteString( va( "v %f %f %f\n",
	//				vecRecast.X(),
	//				vecRecast.Y(),
	//				vecRecast.Z() ).c_str() );
	//		}
	//	}

	//	objFile.WriteString( va( "\n# Faces %d\n", triangles.size() ).c_str() );

	//	for ( size_t t = 0; t < triangles.size(); ++t )
	//	{
	//		objFile.WriteString( va( "f %d %d %d\n", t * 3 + 1, t * 3 + 2, t * 3 + 3 ).c_str() );
	//	}
	//}
}

//////////////////////////////////////////////////////////////////////////

void PathPlannerRecast::cmdModelEnable( const StringVector & args )
{
	if ( mFlags.mViewMode == 0 )
		return;

	const char *strUsage [] =
	{
		"nav_modelenable enable[bool] all[bool]<optional>",
		"> enable: Enable/Disable currently viewed model",
	};

	CHECK_NUM_PARAMS( args, 2, strUsage );
	CHECK_BOOL_PARAM( enable, 1, strUsage );
	OPTIONAL_BOOL_PARAM( all, 2, false );

	RayResult result;
	if ( GetAimedAtModel( result, SURFACE_IGNORE ) )
	{
		if ( all )
			mCollision.mRootNode->SetModelEnable( result.mHitNode->mModel, enable );
		else
		{ 
			result.mHitNode->mEnabled = enable;
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void PathPlannerRecast::cmdModelShape( const StringVector & args )
{
	if ( mFlags.mViewMode == 0 )
		return;

	const char *strUsage [] =
	{
		"nav_modelshape mode[string] all[bool]<optional>",
		"> enable: Sets the shape mode of currently viewed model",
	};

	CHECK_NUM_PARAMS( args, 2, strUsage );
	CHECK_STRING_PARAM( mode, 1, strUsage );
	OPTIONAL_BOOL_PARAM( all, 2, false );

	RecastIO::ShapeMode shapemode;
	if ( !RecastIO::ShapeMode_Parse( mode, &shapemode ) )
	{
		std::string validValues;
		for ( int i = 0; i < RecastIO::ShapeMode_descriptor()->value_count(); ++i )
		{
			if ( !validValues.empty() )
				validValues += ", ";
			validValues += RecastIO::ShapeMode_descriptor()->value( i )->name();
		}
		EngineFuncs::ConsoleError( va( "Invalid mode, valid values are: %s", validValues.c_str() ).c_str() );
		return;
	}

	RayResult result;
	if ( GetAimedAtModel( result, SURFACE_IGNORE ) )
	{
		if ( all )
			mCollision.mRootNode->SetModelShapeMode( result.mHitNode->mModel, shapemode );
		else
		{
			result.mHitNode->mShapeMode = shapemode;
			result.mHitNode->mForceRebuild = true;
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void PathPlannerRecast::cmdModelSetSolid( const StringVector & args )
{
	if ( mFlags.mViewMode == 0 )
		return;

	const char *strUsage [] =
	{
		"nav_modelsolid enable[bool] all[bool]<optional>",
		"> enable: Enable/Disable whether model is solid or is marked off as a region",
	};

	CHECK_NUM_PARAMS( args, 2, strUsage );
	CHECK_BOOL_PARAM( enable, 1, strUsage );
	OPTIONAL_BOOL_PARAM( all, 2, false );

	RayResult result;
	if ( GetAimedAtModel( result, SURFACE_IGNORE ) )
	{
		if ( all )
			mCollision.mRootNode->SetModelSolid( result.mHitNode->mModel, enable );
		else
		{
			result.mHitNode->mSolid = enable;
			result.mHitNode->mForceRebuild = true;
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void PathPlannerRecast::cmdModelDynamic( const StringVector & args )
{
	if ( mFlags.mViewMode == 0 )
		return;

	const char *strUsage [] =
	{
		"nav_modeldynamic enable[bool] all[bool]<optional>",
		"> enable: Enable/Disable movement detection of current model to rebake navigation",
	};

	CHECK_NUM_PARAMS( args, 2, strUsage );
	CHECK_BOOL_PARAM( enable, 1, strUsage );
	OPTIONAL_BOOL_PARAM( all, 2, false );

	RayResult result;
	if ( GetAimedAtModel( result, SURFACE_IGNORE ) )
	{
		if ( all )
			mCollision.mRootNode->SetModelDynamic( result.mHitNode->mModel, enable );
		else
		{
			result.mHitNode->mDynamic = enable;
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void PathPlannerRecast::cmdModelSetTriangleSurface( const StringVector & args )
{
	if ( mFlags.mViewMode == 0 )
		return;

	const char *strUsage [] =
	{
		"nav_modelsettrisurface surfaceflag[string]...",
		"> surfaceflag: set override triangle surface flags for aimed at triangle",
	};

	CHECK_NUM_PARAMS( args, 2, strUsage );

	SurfaceFlags overrideFlags = SURFACE_NONE;
	for ( size_t i = 1; i < args.size(); ++i )
	{
		CHECK_STRING_PARAM( surfacename, i, strUsage );

		SurfaceFlags parsed = SURFACE_NONE;
		if ( !SurfaceFlagsEnum::ValueForName( surfacename.c_str(), parsed ) )
		{
			EngineFuncs::ConsoleError( va( "Unknown surface type '%s', options are %s", surfacename.c_str(), SurfaceFlagsEnum::ValidNames().c_str() ).c_str() );
			return;
		}

		overrideFlags = (SurfaceFlags)( overrideFlags | parsed );
	}

	RayResult result;
	if ( GetAimedAtModel( result, SURFACE_IGNORE ) )
	{
		result.mHitNode->mModel->SetSurfaceOverride( result.mHitTriangle, overrideFlags );
		result.mHitNode->mModel->UpdateModelCrc();
	}
}

//////////////////////////////////////////////////////////////////////////

void PathPlannerRecast::cmdModelSetAreaFlag( const StringVector & args )
{
	if ( mFlags.mViewMode == 0 )
		return;

	NavAreaFlags navFlags = NAVFLAGS_NONE;
	RayResult result;

	if ( args.size() < 2 )
		goto showUsage;

	for ( size_t i = 1; i < args.size(); ++i )
	{
		if ( !NavAreaFlagsEnum::ValueForName( args[ i ].c_str(), navFlags ) )
		{
			EngineFuncs::ConsoleError( va( "Unknown flag '%s'", args[ i ].c_str() ).c_str() );
			goto showUsage;
		}
	}
		
	if ( GetAimedAtModel( result, SURFACE_IGNORE ) )
	{
		result.mHitNode->mNavFlagsOverride = navFlags;
	}
	return;

showUsage:

	std::string flagsStr;
	for ( size_t i = 0; i < NavAreaFlagsEnum::sKeyValCount; ++i )
		flagsStr += va( "%s%s", flagsStr.empty() ? "" : ", ", NavAreaFlagsEnum::sKeyVal[ i ].mName ).c_str();

	EngineFuncs::ConsoleError( "nav_modelsetarea flag(s)[string].." );
	EngineFuncs::ConsoleError( va( "	flags - combination of (%s)", flagsStr.c_str() ).c_str() );
}

//////////////////////////////////////////////////////////////////////////

void PathPlannerRecast::cmdNavAddBuildThread( const StringVector & args )
{
	if ( mFlags.mViewMode == 0 )
		return;

	const char *strUsage [] =
	{
		"nav_addbuildthread #",
		"> numthreads: number of threads to add to help build navigation",
	};

	CHECK_NUM_PARAMS( args, 2, strUsage );
	OPTIONAL_INT_PARAM( numTileBuildThreads, 1, 1 );
	
	for ( int i = 0; i < numTileBuildThreads; ++i )
		mThreadGroupBuild.add_thread( new boost::thread( AsyncTileBuild::Run, this, i ) );
}
