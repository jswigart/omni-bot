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

const Vector3f sExtents = localToRc( Vector3f( 16.f, 16.f, 64.f ) );

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

	mFilter.setAreaCost( NAVFLAGS_CROUCH, 2.0 );
	mFilter.setAreaCost( NAVFLAGS_PRONE, 4.0 );
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

NavAreaFlags RecastPathInterface::GetCurrentAreaFlags() const
{
	return mCurrentAreaFlags;
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

	dtPolyRef currentPoly = 0;
	if ( mCorridor.getPathCount() > 0 )
	{
		Vector3f startPos = localToRc( mSrc );
		if ( !mCorridor.movePosition( startPos, mQuery, &mFilter ) )
		{
			//UpdatePath();
		}
		else
		{
			currentPoly = mCorridor.getFirstPoly();
		}
	}

	if ( currentPoly == 0 )
	{
		dtPolyRef startPoly;
		Vector3f startPos = localToRc( mSrc );
		if ( dtStatusSucceed( mQuery->findNearestPoly( startPos, sExtents, &mFilter, &startPoly, startPos ) ) )
			currentPoly = startPoly;
	}

	mCurrentAreaFlags = NAVFLAGS_NONE;

	if ( currentPoly != 0 )
	{
		navAreaMask areaMask = 0;
		if ( dtStatusSucceed( mNav->mNavMesh->getPolyFlags( currentPoly, &areaMask ) ) )
		{
			mCurrentAreaFlags = ( NavAreaFlags )areaMask;
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
	
	dtPolyRef startPoly, endPoly;
	Vector3f startPos = localToRc( mSrc );
	Vector3f endPos = localToRc( mGoals[ 0 ].mPosition );
	if ( dtStatusSucceed( mQuery->findNearestPoly( startPos, sExtents, &mFilter, &startPoly, startPos ) ) )
	{
		mCorridor.reset( startPoly, startPos );

		if ( dtStatusSucceed( mQuery->findNearestPoly( endPos, sExtents, &mFilter, &endPoly, endPos ) ) )
		{
			int	polyCount = 0;
			dtPolyRef * polys = (dtPolyRef*)StackAlloc( sizeof( dtPolyRef ) * mCorridor.getMaxPathCount() );

			dtStatus status = mQuery->findPath( startPoly, endPoly, startPos, endPos, &mFilter, polys, &polyCount, mCorridor.getMaxPathCount() );
			if ( dtStatusSucceed( status ) )
			{
				if ( dtStatusDetail( status, DT_PARTIAL_RESULT ) )
				{
					mStatus = PATH_NOPATHTOGOAL;
				}
				else
				{
					mStatus = PATH_VALID;
					mCorridor.setCorridor( endPos, polys, polyCount );
				}
				
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
	const size_t num = mCorridor.findCorners( (float*)cornerVerts, cornerFlags, cornerPoly, MaxCorners, mQuery, &mFilter, DT_STRAIGHTPATH_AREA_CROSSINGS );
	for ( size_t i = 0; i < num && numCorners < maxEdges; ++i, ++numCorners )
	{
		NavAreaFlags areaFlags = NAVFLAGS_WALK;

		navAreaMask af = NAVFLAGS_NONE;
		if ( dtStatusSucceed( mNav->mNavMesh->getPolyFlags( cornerPoly[ i ], &af ) ) )
		{
			areaFlags = (NavAreaFlags)( af | areaFlags );
		}

		corners[ numCorners ].mPos = rcToLocal( cornerVerts[ i ] );
		corners[ numCorners ].mAreaMask = areaFlags;
		corners[ numCorners ].mPolyId = cornerPoly[ i ];
		corners[ numCorners ].mIsLink = ( cornerFlags[ i ] & DT_STRAIGHTPATH_OFFMESH_CONNECTION ) != 0;
	}
	return numCorners;
}

bool RecastPathInterface::GetNavLink( uint64_t id, OffMeshConnection& conn ) const
{
	const dtOffMeshConnection* link = mNav->mNavMesh->getOffMeshConnectionByRef( (dtPolyRef)id );
	if ( link )
	{
		if ( link->userId < mNav->mOffMeshConnections.size() )
		{
			conn = mNav->mOffMeshConnections[ link->userId ];
			conn.mPolyId = id;
			return true;
		}
	}
	return false;
}

bool RecastPathInterface::CompleteNavLink( uint64_t id )
{
	dtPolyRef refs[ 2 ];
	Vector3f srcPos, dstPos;
	return mCorridor.moveOverOffmeshConnection( (dtPolyRef)id, refs, srcPos.Ptr(), dstPos.Ptr(), mQuery );
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