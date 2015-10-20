////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "PathPlannerFloodFill.h"
#include "ScriptManager.h"
#include "IGameManager.h"
#include "IGame.h"
#include "Client.h"
#include "Timer.h"

#include "RenderBuffer.h"

using namespace std;

//////////////////////////////////////////////////////////////////////////

void PathPlannerFloodFill::InitCommands()
{
	PathPlannerBase::InitCommands();

	SetEx( "nav_save", "Save current navigation to disk",
		this, &PathPlannerFloodFill::cmdNavSave );
	SetEx( "nav_load", "Load last saved navigation from disk",
		this, &PathPlannerFloodFill::cmdNavLoad );
	SetEx( "nav_view", "Turn on/off navmesh visibility.",
		this, &PathPlannerFloodFill::cmdNavView );
	SetEx( "nav_viewconnections", "Turn on/off navmesh connection visibility.",
		this, &PathPlannerFloodFill::cmdNavViewConnections );

	//////////////////////////////////////////////////////////////////////////
	SetEx( "nav_enablestep", "Enable step by step generation process.",
		this, &PathPlannerFloodFill::cmdNavEnableStep );
	SetEx( "nav_step", "Step to the next nav process.",
		this, &PathPlannerFloodFill::cmdNavStep );
	SetEx( "nav_addfloodstart", "Adds a starting node for the flood fill.",
		this, &PathPlannerFloodFill::cmdAddFloodStart );
	SetEx( "nav_clearfloodstart", "Clear all flood fill starts.",
		this, &PathPlannerFloodFill::cmdClearFloodStarts );
	SetEx( "nav_savefloodstart", "Save all flood fill starts to <mapname>.navstarts.",
		this, &PathPlannerFloodFill::cmdSaveFloodStarts );
	SetEx( "nav_loadfloodstart", "Load all flood fill starts from <mapname>.navstarts.",
		this, &PathPlannerFloodFill::cmdLoadFloodStarts );
	SetEx( "nav_floodfill", "Start the flood fill process.",
		this, &PathPlannerFloodFill::cmdNavMeshFloodFill );
	//////////////////////////////////////////////////////////////////////////
	SetEx( "nav_autofeature", "Automatically waypoints jump pads, teleporters, player spawns.",
		this, &PathPlannerFloodFill::cmdAutoBuildFeatures );

	//////////////////////////////////////////////////////////////////////////

	/*SetEx("nav_loadobj", "Loads navmesh from obj file.",
	this, &PathPlannerFloodFill::cmdLoadObj);
	SetEx("nav_loadmap", "Loads navmesh from map file.",
	this, &PathPlannerFloodFill::cmdLoadMap);*/

	SetEx( "nav_next", "Steps the current tool to the next operation.",
		this, &PathPlannerFloodFill::cmdNext );

	// INFLUENCE MAP EXPERIMENTATION
	SetEx( "nav_mapcreate", "Creates an influence map.",
		this, &PathPlannerFloodFill::cmdInfluenceMapCreate );
	SetEx( "nav_mapseed", "Adds a seed point to the map for exploration.",
		this, &PathPlannerFloodFill::cmdInfluenceMapSeed );
	SetEx( "nav_mapmem", "Shows the memory usage of the map.",
		this, &PathPlannerFloodFill::cmdInfluenceMapMem );
	SetEx( "nav_mapsave", "Saves the influence map.",
		this, &PathPlannerFloodFill::cmdInfluenceMapSave );
	SetEx( "nav_mapload", "Load the influence map.",
		this, &PathPlannerFloodFill::cmdInfluenceMapLoad );

	SetEx( "nav_mapflood", "Load the influence map.",
		this, &PathPlannerFloodFill::cmdInfluenceMapFlood );

	/*REGISTER_STATE(PathPlannerFloodFill,NoOp);

	SetNextState(NoOp);*/
}

void PathPlannerFloodFill::cmdNavSave( const StringVector & args )
{
	if ( mFlags.mViewMode == 0 )
		return;

	if ( Save( gEngineFuncs->GetMapName() ) )
	{
		EngineFuncs::ConsoleMessage( "Saved Nav." );
	}
	else
		EngineFuncs::ConsoleError( "ERROR Saving Nav." );
}

void PathPlannerFloodFill::cmdNavLoad( const StringVector & args )
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

void PathPlannerFloodFill::cmdNavView( const StringVector & args )
{
	const char *strUsage [] =
	{
		"nav_view enable[bool]",
		"> enable: Enable nav rendering. true/false/on/off/1/0",
	};

	CHECK_NUM_PARAMS( args, 2, strUsage );
	CHECK_BOOL_PARAM( bEnable, 1, strUsage );
	ScriptManager::GetInstance()->ExecuteStringLogged( va( "Nav.EnableView( %s );", bEnable ? "true" : "false" ).c_str() );
}

void PathPlannerFloodFill::cmdNavViewConnections( const StringVector & args )
{
	const char *strUsage [] =
	{
		"nav_viewconnections enable[bool]",
		"> enable: Enable nav connection rendering. true/false/on/off/1/0",
	};

	CHECK_NUM_PARAMS( args, 2, strUsage );
	CHECK_BOOL_PARAM( bEnable, 1, strUsage );
	ScriptManager::GetInstance()->ExecuteStringLogged( va( "Nav.EnableViewConnection( %s );", bEnable ? "true" : "false" ).c_str() );
}

void PathPlannerFloodFill::cmdNavEnableStep( const StringVector & args )
{
	if ( mFlags.mViewMode == 0 )
		return;

	const char *strUsage [] =
	{
		"nav_enablestep enable[bool]",
		"> enable: Enable step by step nav generation. true/false/on/off/1/0",
	};

	CHECK_NUM_PARAMS( args, 2, strUsage );
	CHECK_BOOL_PARAM( bEnable, 1, strUsage );
	ScriptManager::GetInstance()->ExecuteStringLogged( va( "Nav.EnableStep( %s );",	bEnable ? "true" : "false" ).c_str() );
}

void PathPlannerFloodFill::cmdNavStep( const StringVector & args )
{
	if ( mFlags.mViewMode == 0 )
		return;
	ScriptManager::GetInstance()->ExecuteStringLogged( "Nav.Step();" );
}

void PathPlannerFloodFill::cmdAddFloodStart( const StringVector & args )
{
	if ( mFlags.mViewMode == 0 )
		return;


	Vector3f vPosition;
	if ( SUCCESS( gEngineFuncs->GetEntityPosition( Utils::GetLocalEntity(), vPosition ) ) )
	{
		AddFloodStart( vPosition );
	}
}

void PathPlannerFloodFill::cmdClearFloodStarts( const StringVector & args )
{
	if ( mFlags.mViewMode == 0 )
		return;


	ClearFloodStarts();
}

void PathPlannerFloodFill::cmdSaveFloodStarts( const StringVector & args )
{
	if ( mFlags.mViewMode == 0 )
		return;

	SaveFloodStarts();
}

void PathPlannerFloodFill::cmdLoadFloodStarts( const StringVector & args )
{
	if ( mFlags.mViewMode == 0 )
		return;

	LoadFloodStarts();
}

PathPlannerFloodFill::FloodFillOptions::FloodFillOptions()
{
	mCharacterHeight = 64.0f;
	mCharacterCrouchHeight = 48.0f;
	mCharacterStepHeight = 18.0f;
	mCharacterJumpHeight = 60.0f;
	mGridRadius = 16.0f;
}

void PathPlannerFloodFill::cmdNavMeshFloodFill( const StringVector & args )
{
	if ( mFlags.mViewMode == 0 )
		return;

	const char *strUsage [] =
	{
		"nav_floodfill gridradius[#] stepheight[#] jumpheight[#] characterheight[#] charactercrouchheight[#]",
		"> gridradius: radius of the grid cell to test with",
		"> stepheight: the height an entity can step before being blocked by an edge",
		"> jumpheight: the height an entity can jump before being blocked by an edge",
		"> charactercrouchheight: the height the character",
		"> characterheight: the height the character",
	};

	PRINT_USAGE( strUsage );

	FloodFillOptions options;

	OPTIONAL_FLOAT_PARAM( fGridRadius, 1, options.mGridRadius );
	OPTIONAL_FLOAT_PARAM( fStepHeight, 2, options.mCharacterStepHeight );
	OPTIONAL_FLOAT_PARAM( fJumpHeight, 3, options.mCharacterJumpHeight );
	OPTIONAL_FLOAT_PARAM( fCrouchHeight, 4, options.mCharacterCrouchHeight );
	OPTIONAL_FLOAT_PARAM( fCharacterHeight, 5, options.mCharacterHeight );

	FloodFill( options );
}

//////////////////////////////////////////////////////////////////////////

void PathPlannerFloodFill::cmdAutoBuildFeatures( const StringVector & args )
{
	if ( mFlags.mViewMode == 0 )
		return;

	const int iMaxFeatures = 128;
	AutoNavFeature features[ iMaxFeatures ];
	int iNumFeatures = gEngineFuncs->GetAutoNavFeatures( features, iMaxFeatures );
	for ( int i = 0; i < iNumFeatures; ++i )
	{
		Vector3f vPos( features[ i ].mPosition );
		Vector3f vFace( features[ i ].mFacing );
		Vector3f vTarget( features[ i ].mTargetPosition );

		// Adjust for waypoint offsets
		if ( !features[ i ].mBounds.IsZero() )
		{
			features[ i ].mBounds.CenterBottom( vPos );
		}
		if ( !features[ i ].mTargetBounds.IsZero() )
		{
			features[ i ].mTargetBounds.CenterBottom( vTarget );
		}

		AddFloodStart( vPos );
		if ( vPos != vTarget )
		{
			AddFloodStart( vTarget );
			//pFeature->ConnectTo(pTarget);
		}

		//////////////////////////////////////////////////////////////////////////
		RenderBuffer::AddLine( vPos, vPos + Vector3f::UNIT_Z * 32.f, COLOR::GREEN );
		if ( vPos != vTarget )
		{
			RenderBuffer::AddLine( vPos, vTarget, COLOR::MAGENTA );
			RenderBuffer::AddLine( vTarget, vTarget + Vector3f::UNIT_Z * 32.f, COLOR::RED );
		}
		RenderBuffer::AddAABB( features[ i ].mBounds, COLOR::GREEN );
		//////////////////////////////////////////////////////////////////////////
	}
	EngineFuncs::ConsoleMessage( va( "Found %d nav features.", iNumFeatures ).c_str() );
}

void PathPlannerFloodFill::_BenchmarkPathFinder( const StringVector & args )
{
	EngineFuncs::ConsoleMessage( "-= FloodFill PathFind Benchmark =-" );

	double dTimeTaken = 0.0f;
	int32_t iNumSectors = 0;//(int32_t)mRuntimeSectors.size();
	int32_t iNumPaths = iNumSectors * iNumSectors;

	Timer tme;
	tme.Reset();
	for ( int32_t w1 = 0; w1 < iNumSectors; ++w1 )
	{
		for ( int32_t w2 = 0; w2 < iNumSectors; ++w2 )
		{
			/*const NavSector &pS1 = mRuntimeSectors[w1];
			const NavSector &pS2 = mRuntimeSectors[w2];

			PlanPathToGoal(NULL,
			pS1.mMiddle+Vector3f(0,0,NavigationMeshOptions::CharacterHeight),
			pS2.mMiddle+Vector3f(0,0,NavigationMeshOptions::CharacterHeight),
			0);*/
		}
	}
	dTimeTaken = tme.GetElapsedSeconds();

	EngineFuncs::ConsoleMessage( va( "generated %d paths in %f seconds: %f paths/sec",
		iNumPaths, dTimeTaken, dTimeTaken != 0.0f ? (float)iNumPaths / dTimeTaken : 0.0f ).c_str() );
}

void PathPlannerFloodFill::_BenchmarkGetNavPoint( const StringVector & args )
{
	uint32_t iNumIterations = 1;
	if ( args.size() > 1 )
	{
		iNumIterations = atoi( args[ 1 ].c_str() );
		if ( iNumIterations <= 0 )
			iNumIterations = 1;
	}

	EngineFuncs::ConsoleMessage( "-= FloodFill GetNavPoint Benchmark  =-" );

	/*double dTimeTaken = 0.0f;
	uint32_t iNumWaypoints = mRuntimeSectors.size();
	Timer tme;

	uint32_t iHits = 0, iMisses = 0;
	tme.Reset();
	for(uint32_t i = 0; i < iNumIterations; ++i)
	{
	for(uint32_t w1 = 0; w1 < iNumWaypoints; ++w1)
	{
	NavSector *pSector = mRuntimeSectors[w1];

	Waypoint *pClosest = _GetClosestWaypoint(pWaypoint->GetPosition(), (NavFlags)0, true);
	if(pClosest)
	++iHits;
	else
	++iMisses;
	}
	}

	dTimeTaken = tme.GetElapsedSeconds();

	EngineFuncs::ConsoleMessage("_GetClosest() %d calls, %d hits, %d misses : avg %f per second",
	iNumWaypoints * iNumIterations,
	iHits,
	iMisses,
	dTimeTaken != 0.0f ? ((float)(iNumWaypoints * iNumIterations) / dTimeTaken) : 0.0f);	*/
}

void PathPlannerFloodFill::cmdNext( const StringVector & args )
{
	if ( mFlags.mViewMode == 0 )
		return;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void PathPlannerFloodFill::cmdInfluenceMapCreate( const StringVector & args )
{
	AABB mapbounds;
	mapbounds.Set( Vector3f( -4400, -2000, -800 ), Vector3f( 4400, 2000, 230 ) );

	VectorQueue empty;
	mSpanFrontier.swap( empty );

	RenderBuffer::StaticBufferDelete( mInfluenceBufferId );
	mInfluenceBufferId = 0;

	OB_DELETE( mInfluence );

	mSpanMap.Clear();
	mSpanMap.Init( Vector3f( mapbounds.mMins ), Vector3f( mapbounds.mMaxs ), 16.0f );

	EngineFuncs::ConsoleMessage( va( "Created %d x %d span map",
		mSpanMap.GetNumCellsX(), mSpanMap.GetNumCellsY() ).c_str() );
}

void PathPlannerFloodFill::cmdInfluenceMapSeed( const StringVector & args )
{
	/*Vector3f eyePos;
	if ( Utils::GetLocalEyePosition(eyePos) )
	mSpanFrontier.push( eyePos );*/

	enum
	{
		MaxFeatures = 64
	};
	AutoNavFeature features[ MaxFeatures ];
	const int numFeatures = gEngineFuncs->GetAutoNavFeatures( features, MaxFeatures );
	for ( int i = 0; i < numFeatures; ++i )
	{
		mSpanFrontier.push( features[ i ].mPosition );
		mSpanFrontier.push( features[ i ].mTargetPosition );
	}
}

void PathPlannerFloodFill::cmdInfluenceMapMem( const StringVector & args )
{
	EngineFuncs::ConsoleMessage( va( "Influence Map %d x %d ( %s )",
		mSpanMap.GetNumCellsX(),
		mSpanMap.GetNumCellsY(),
		Utils::FormatByteString( mSpanMap.CalculateMemUsage() ).c_str() ).c_str() );
}

void PathPlannerFloodFill::cmdInfluenceMapSave( const StringVector & args )
{
	const std::string filePath = std::string( "nav/" ) + std::string( gEngineFuncs->GetMapName() ) + ".influence";

	std::string data;
	if ( mSpanMap.Serialize( data ) )
	{
		File f;
		if ( f.OpenForWrite( filePath.c_str(), File::Binary ) )
		{
			f.Write( data.c_str(), data.length() );
		}
	}
}

void PathPlannerFloodFill::cmdInfluenceMapLoad( const StringVector & args )
{
	const std::string filePath = std::string( "nav/" ) + std::string( gEngineFuncs->GetMapName() ) + ".influence";

	mSpanMap.Clear();

	File f;
	if ( !f.OpenForRead( filePath.c_str(), File::Binary ) )
	{
		EngineFuncs::ConsoleError( va( "Influence Map %s not found", filePath.c_str() ).c_str() );
		return;
	}

	std::string data;
	if ( !f.ReadWholeFile( data ) )
	{
		EngineFuncs::ConsoleError( va( "Influence Map Read Error %s", filePath.c_str() ).c_str() );
		return;
	}

	if ( !mSpanMap.DeSerialize( data ) )
	{
		EngineFuncs::ConsoleError( va( "Influence Map Parse Error %s", filePath.c_str() ).c_str() );
		return;
	}
}

void PathPlannerFloodFill::cmdInfluenceMapFlood( const StringVector & args )
{
	Vector3f vAimPt;
	if ( !Utils::GetLocalAimPoint( vAimPt ) )
		return;

	if ( mInfluence == NULL )
		mInfluence = AllocInfluenceMap();

	mInfluence->Reset();
	mInfluence->AddSeed( vAimPt, 0.0f );

	Timer t;

	// do an immediate full update
	while ( !mInfluence->UpdateInfluences( std::numeric_limits<int>::max() ) )
	{
	}

	mUpdateInfluenceBuffer = true;

	EngineFuncs::ConsoleError( va( "Influence Flooded in %.3f sec ( %s )",
		t.GetElapsedSeconds(),
		Utils::FormatByteString( mInfluence->CalculateMemUsage() ).c_str() ).c_str() );
}