////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy: jswigart@gmail.com $
// $LastChangedDate: 2013-05-10 02:12:26 -0500 (Fri, 10 May 2013) $
// $LastChangedRevision: 908 $
//
////////////////////////////////////////////////////////////////////////////////

#include "PathPlannerRecastPathInterface.h"
#include "RenderBuffer.h"

#include "DetourNavMeshQuery.h"

#include "Client.h"

extern float dtRandom();

Vector3f rcToLocal( const float * vec );
Vector3f localToRc( const float * vec );

//////////////////////////////////////////////////////////////////////////

RecastPathInterfaces RecastPathInterface::sInterfaces;

//////////////////////////////////////////////////////////////////////////

RecastPathInterface::RecastPathInterface( Client * client, PathPlannerRecast * nav ) 
	: mClient( client )
	, mNav( nav )
	, mStatus( PATH_NONE )
	, mMoveDirection( Vector3f::ZERO )
{
	mQuery = dtAllocNavMeshQuery();
	mCorridor.init( 2048 );
	ReInit();

	sInterfaces.push_back( this );
}

RecastPathInterface::~RecastPathInterface()
{
	dtFreeNavMeshQuery( mQuery );

	sInterfaces.erase( std::remove( sInterfaces.begin(), sInterfaces.end(), this ), sInterfaces.end() );
}

void RecastPathInterface::ReInit()
{
	mQuery->init( mNav->mNavMesh, 2048 );
	mStatus = PathInterface::PATH_NONE;
}

PathInterface::PathStatus RecastPathInterface::GetPathStatus() const
{
	return mStatus;
}

void RecastPathInterface::UpdateNavFlags( NavFlags & includeFlags, NavFlags & excludeFlags )
{
	mFilter.setIncludeFlags( (unsigned short)includeFlags );
	mFilter.setExcludeFlags( (unsigned short)excludeFlags );
}

void RecastPathInterface::UpdateSourcePosition( const Vector3f & srcPos )
{
	NavFlags includeFlags = NAVFLAGS_WALK;
	NavFlags excludeFlags = NAVFLAGS_NONE;
	mClient->GetNavFlags( includeFlags, excludeFlags );
	UpdateNavFlags( includeFlags, excludeFlags );

	if ( !mCorridor.isValid( 32, mQuery, &mFilter ) )
		UpdatePath();

	mSrc = srcPos;
	if ( mCorridor.getPathCount() > 0 )
	{
		Vector3f startPos = localToRc( mSrc );
		if ( !mCorridor.movePosition( startPos, mQuery, &mFilter ) )
		{
			//UpdatePath();
		}
	}
}

void RecastPathInterface::UpdateGoalPosition( const Vector3f & goal, float radius )
{
	mGoals.resize( 1 );
	mGoals[ 0 ].mPosition = goal;
	mGoals[ 0 ].mRadius = radius;
}

void RecastPathInterface::UpdateGoalPositions( const DestinationVector & goals )
{
	mGoals = goals;
}

bool RecastPathInterface::UpdateGoalPositionRandom()
{
	Destination dest;
	dest.mPosition = mSrc;
	dest.mRadius = 4.0f;
	
	Vector3f randomPos;
	dtPolyRef randomPoly;
	if ( dtStatusSucceed( mQuery->findRandomPoint( &mFilter, dtRandom, &randomPoly, randomPos ) ) )
	{
		dest.mPosition = rcToLocal( randomPos );
		mGoals.resize( 0 );
		mGoals.push_back( dest );
		return true;
	}
	return false;
}

void RecastPathInterface::UpdatePath()
{
	NavFlags includeFlags = NAVFLAGS_WALK;
	NavFlags excludeFlags = NAVFLAGS_NONE;
	mClient->GetNavFlags( includeFlags, excludeFlags );
	UpdateNavFlags( includeFlags, excludeFlags );

	mFoundGoalIndex = -1;
	mStatus = PathInterface::PATH_NONE;

	if ( mGoals.empty() )
		return;

	const Vector3f extents = localToRc( Vector3f( 16.f, 16.f, 64.f ) );

	dtPolyRef startPoly, endPoly;
	Vector3f startPos = localToRc( mSrc );
	Vector3f endPos = localToRc( mGoals[ 0 ].mPosition );
	if ( dtStatusSucceed( mQuery->findNearestPoly( startPos, extents, &mFilter, &startPoly, startPos ) ) )
	{
		mCorridor.reset( startPoly, startPos );

		if ( dtStatusSucceed( mQuery->findNearestPoly( endPos, extents, &mFilter, &endPoly, endPos ) ))
		{
			int	polyCount = 0;
			dtPolyRef * polys = (dtPolyRef*)StackAlloc( sizeof( dtPolyRef ) * mCorridor.getMaxPathCount() );

			if ( dtStatusSucceed( mQuery->findPath( startPoly, endPoly, startPos, endPos, &mFilter, polys, &polyCount, mCorridor.getMaxPathCount() ) ) )
			{
				mStatus = PATH_VALID;
				mCorridor.setCorridor( endPos, polys, polyCount );
			}
		}
	}
}

void RecastPathInterface::Cancel()
{
	mStatus = PATH_NONE;
	mMoveDirection = Vector3f::ZERO;
}

size_t RecastPathInterface::GetPathCorners( PathCorner * corners, size_t maxEdges )
{
	if ( mCorridor.getPathCount() == 0 )
		return 0;

	enum { MaxCorners = 64 };
	Vector3f cornerVerts[ MaxCorners ];
	unsigned char cornerFlags[ MaxCorners ];
	dtPolyRef cornerPoly[ MaxCorners ];

	size_t numCorners = 0;
	const size_t num = mCorridor.findCorners( (float*)cornerVerts, cornerFlags, cornerPoly, MaxCorners, mQuery, &mFilter );
	for ( size_t i = 0; i < num && numCorners < maxEdges; ++i, ++numCorners )
	{
		NavArea polyArea = NAVAREA_GROUND;
		NavAreaFlags polyFlags = NAVFLAGS_WALK;

		if ( cornerFlags[ i ] & DT_STRAIGHTPATH_OFFMESH_CONNECTION )
			polyFlags = NAVFLAGS_PATH_LINK;
		if ( cornerFlags[ i ] & DT_STRAIGHTPATH_START )
			polyFlags = (NavAreaFlags)(polyFlags | NAVFLAGS_PATH_START);
		if ( cornerFlags[ i ] & DT_STRAIGHTPATH_END )
			polyFlags = (NavAreaFlags)(polyFlags | NAVFLAGS_PATH_GOAL);

		unsigned char area = 0;
		unsigned short flags = 0;
		if ( dtStatusSucceed( mNav->mNavMesh->getPolyArea( cornerPoly[ i ], &area ) ) &&
			dtStatusSucceed( mNav->mNavMesh->getPolyFlags( cornerPoly[ i ], &flags ) ) )
		{
			polyArea = (NavArea)area;
			polyFlags = (NavAreaFlags)( polyFlags | flags );
		}

		corners[ numCorners ].mPos = rcToLocal( cornerVerts[ i ] );
		corners[ numCorners ].mArea = polyArea;
		corners[ numCorners ].mFlags = polyFlags;
	}
	return numCorners;
}

bool RecastPathInterface::GetPointAlongPath( float lookAheadDist, Vector3f & ptOut )	
{
	if ( mStatus != PATH_VALID )
		return false;

	return true;
}

void RecastPathInterface::Render() 
{
	//mPathFind.Render();

	enum { MaxEdges = 64 };
	PathInterface::PathCorner edges[ MaxEdges ];
	const size_t pathEdgeCount = GetPathCorners( edges, MaxEdges );

	// corners don't include the source position, so render that first
	Vector3f lastPt = mSrc;
	for ( size_t i = 0; i < pathEdgeCount; ++i )
	{
		RenderBuffer::AddArrow( lastPt, edges[ i ].mPos, COLOR::ORANGE );
		lastPt = edges[ i ].mPos;
	}
}