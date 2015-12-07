////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "PathPlannerBase.h"
#include "FileSystem.h"
#include "IGame.h"

#include "RenderBuffer.h"

//////////////////////////////////////////////////////////////////////////

const NavAreaFlagsEnum NavAreaFlagsEnum::sKeyVal [] =
{
	{ "none", NAVFLAGS_NONE },
	{ "walk", NAVFLAGS_WALK },
	{ "crouch", NAVFLAGS_CROUCH },
	{ "prone", NAVFLAGS_PRONE },
	{ "team1", NAVFLAGS_TEAM1_ONLY },
	{ "team2", NAVFLAGS_TEAM2_ONLY },
	{ "team3", NAVFLAGS_TEAM3_ONLY },
	{ "team4", NAVFLAGS_TEAM4_ONLY },
	{ "water", NAVFLAGS_WATER },
	{ "jump", NAVFLAGS_JUMP },
	{ "ladder", NAVFLAGS_LADDER },
	{ "teleport", NAVFLAGS_TELEPORT },
	{ "door", NAVFLAGS_DOOR },
	{ "rocketjump", NAVFLAGS_ROCKETJUMP },
	{ "pushable", NAVFLAGS_PUSHABLE },
	{ "mover", NAVFLAGS_MOVER },
	{ "jumppad", NAVFLAGS_JUMPPAD },
	{ "jetpack", NAVFLAGS_JETPACK },
	{ "longjump", NAVFLAGS_LONGJUMP },
	{ "destructible", NAVFLAGS_DESTRUCTIBLE },
	{ "jumppad", NAVFLAGS_JUMPPAD },
	{ "disabled", NAVFLAGS_DISABLED },
	{ "allteams", NAVFLAGS_ALLTEAMS },
};

const size_t NavAreaFlagsEnum::sKeyValCount = sizeof( NavAreaFlagsEnum::sKeyVal ) / sizeof( NavAreaFlagsEnum::sKeyVal[ 0 ] );

//////////////////////////////////////////////////////////////////////////

PathInterface::PathInterface()
{
}

PathInterface::~PathInterface()
{
}

//////////////////////////////////////////////////////////////////////////

OffMeshConnection::OffMeshConnection()
	: mRadius( 0.0f )
	, mAreaFlags( NAVFLAGS_WALK )
	, mBiDirectional( false )
	, mPolyId( 0 )
{
}

void OffMeshConnection::Render()
{
	std::string areaStr, flagStr;
	NavAreaFlagsEnum::NameForValue( mAreaFlags, flagStr );

	Vector3f lastPt = mEntry;
	for ( size_t i = 0; i < mVertices.size(); ++i )
	{
		RenderBuffer::AddLine( lastPt, mVertices[ i ], COLOR::GREEN );
		lastPt = mVertices[ i ];
	}

	RenderBuffer::AddCircle( mEntry, mRadius, COLOR::GREEN );
	RenderBuffer::AddCircle( mExit, mRadius, COLOR::GREEN );
	RenderBuffer::AddLine( lastPt, mExit, COLOR::GREEN );
	RenderBuffer::AddString3d( mEntry + Vector3f( 0.f, 0.f, 40.f ), COLOR::BLUE, va( "%s (%s)", areaStr.c_str(), flagStr.c_str() ).c_str() );
	RenderBuffer::AddString3d( mExit + Vector3f( 0.f, 0.f, 40.f ), COLOR::BLUE, va( "%s (%s)", areaStr.c_str(), flagStr.c_str() ).c_str() );
}

//////////////////////////////////////////////////////////////////////////

PathPlannerBase::PathPlannerBase()
{
	mNavigationBounds.Min = Vector3f( -1000.0f, -1000.0f, -1000.0f );
	mNavigationBounds.Max = Vector3f( 1000.0f, 1000.0f, 1000.0f );
}

PathPlannerBase::~PathPlannerBase()
{
}

void PathPlannerBase::InitCommands()
{
	SetEx( "nav_benchmarkpathfinder", "Render a failed path by its index.", this, &PathPlannerBase::cmdBenchmarkPathFind );
	SetEx( "nav_benchmarkgetnavpoint", "Render a failed path by its index.", this, &PathPlannerBase::cmdBenchmarkGetNavPoint );
	SetEx( "nav_resaveall", "Re-save all nav files to the newest file format.", this, &PathPlannerBase::cmdResaveNav );
}

void PathPlannerBase::_BenchmarkPathFinder( const StringVector & args )
{
	EngineFuncs::ConsoleMessage( "Benchmark Not Implemented!" );
}
void PathPlannerBase::_BenchmarkGetNavPoint( const StringVector & args )
{
	EngineFuncs::ConsoleMessage( "Benchmark Not Implemented!" );
}

void PathPlannerBase::cmdBenchmarkPathFind( const StringVector & args )
{
	/*if ( mFlags.mViewMode != 0 )
		return;*/

	_BenchmarkPathFinder( args );
}

void PathPlannerBase::cmdBenchmarkGetNavPoint( const StringVector & args )
{
	/*if ( mFlags.mViewMode != 0 )
		return;*/

	_BenchmarkGetNavPoint( args );
}

void PathPlannerBase::cmdResaveNav( const StringVector & args )
{
	/*if ( mFlags.mViewMode != 0 )
		return;*/

	DirectoryList wpFiles;
	FileSystem::FindAllFiles( "nav/", wpFiles, va( ".*%s", _GetNavFileExtension().c_str() ).c_str() );
	for ( uint32_t i = 0; i < wpFiles.size(); ++i )
	{
		const std::string &mapname = fs::basename( wpFiles[ i ] );

		bool bGood = false;
		if ( Load( mapname ) )
		{
			bGood = true;
			Save( mapname );
		}

		EngineFuncs::ConsoleMessage( va( "Resaving %s, %s", mapname.c_str(), bGood ? "success" : "failed" ).c_str() );
	}

	// reload current map wps
	Load( std::string( gEngineFuncs->GetMapName() ) );
}

bool PathPlannerBase::Load( bool _dl )
{
	return Load( gEngineFuncs->GetMapName(), _dl );
}

void PathPlannerBase::EntityCreated( const EntityInstance &ei )
{
}

void PathPlannerBase::EntityDeleted( const EntityInstance &ei )
{
}

PathInterface * PathPlannerBase::AllocPathInterface()
{
	return NULL;
}

const AxisAlignedBox3f & PathPlannerBase::GetNavigationBounds() const
{
	return mNavigationBounds;
}
