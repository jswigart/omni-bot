////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy: jswigart@gmail.com $
// $LastChangedDate: 2013-05-10 02:12:26 -0500 (Fri, 10 May 2013) $
// $LastChangedRevision: 908 $
//
////////////////////////////////////////////////////////////////////////////////

#include <unordered_set>

#include "PathPlannerRecastPathInterface.h"
#include "RenderBuffer.h"
#include "DetourCommon.h"
#include "DetourNavMeshQuery.h"

#include "Client.h"

extern float dtRandom();

Vector3f rcToLocal( const float * vec );
Vector3f localToRc( const float * vec );

//////////////////////////////////////////////////////////////////////////

RecastPathInterfaces RecastPathInterface::sInterfaces;

//////////////////////////////////////////////////////////////////////////

RecastPathInterface::RecastPathInterface( PathPlannerRecast * nav ) 
	: mNav( nav )
	, mStatus( PATH_NONE )
	, mMoveDirection( Vector3f::ZERO )
{
	mQuery = dtAllocNavMeshQuery();
	mCorridor.init( 2048 );
	ReInit();

	sInterfaces.push_back( this );

	mFilter.setAreaCost( NAVFLAGS_CROUCH, 2.0 );
	mFilter.setAreaCost( NAVFLAGS_PRONE, 4.0 );
	mFilter.setAreaCost( NAVFLAGS_PUSHABLE, 10.0 );
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

void RecastPathInterface::UpdateNavFlags( NavFlags includeFlags, NavFlags excludeFlags )
{
	mFilter.setIncludeFlags( includeFlags );
	mFilter.setExcludeFlags( excludeFlags );
}

void RecastPathInterface::UpdateSourcePosition( const Vector3f & srcPos )
{
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
		if ( dtStatusSucceed( mQuery->findNearestPoly( startPos, PathPlannerRecast::sExtents, &mFilter, &startPoly, startPos ) ) )
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
	
	std::vector<dtPolyRef> openlist;
	typedef std::unordered_set<dtPolyRef> ClosedList;
	ClosedList closedlist;

	dtPolyRef startPoly;
	Vector3f startPos = localToRc( mSrc );
	if ( dtStatusSucceed( mQuery->findNearestPoly( startPos, PathPlannerRecast::sExtents, &mFilter, &startPoly, startPos ) ) && startPoly )
	{
		openlist.push_back( startPoly );
	}
	else
	{
		return false;
	}

	while ( !openlist.empty() )
	{
		dtPolyRef polyRef = openlist.back();
		openlist.pop_back();

		closedlist.insert( polyRef );

		unsigned int salt, it, ip;
		mNav->mNavMesh->decodePolyId( polyRef, salt, it, ip );
		
		const dtMeshTile* tile = 0;
		const dtPoly* poly = 0;
		mNav->mNavMesh->getTileAndPolyByRefUnsafe( polyRef, &tile, &poly );

		// Visit linked polygons.
		for ( unsigned int i = poly->firstLink; i != DT_NULL_LINK; i = tile->links[ i ].next )
		{
			const dtPolyRef neiRef = tile->links[ i ].ref;

			// Skip invalid and already visited.
			if ( !neiRef )
				continue;

			// if the teams have already touched this poly, don't explore it again
			ClosedList::iterator it = closedlist.find( neiRef );
			if ( it != closedlist.end() )
				continue;

			openlist.push_back( neiRef );
		}
	}

	if ( closedlist.size() > 0 )
	{
		float areaSum = 0.0f;
		const dtPoly* randPoly = 0;
		dtPolyRef randPolyRef = 0;

		for ( ClosedList::iterator it = closedlist.begin(); it != closedlist.end(); ++it )
		{
			const dtMeshTile* tile = 0;
			const dtPoly* poly = 0;
			mNav->mNavMesh->getTileAndPolyByRefUnsafe( *it, &tile, &poly );

			// Calc area of the polygon.
			float polyArea = 0.0f;
			const dtPoly* p = tile->polys;
			for ( int j = 2; j < p->vertCount; ++j )
			{
				const float* va = &tile->verts[ p->verts[ 0 ] * 3 ];
				const float* vb = &tile->verts[ p->verts[ j - 1 ] * 3 ];
				const float* vc = &tile->verts[ p->verts[ j ] * 3 ];
				polyArea += dtTriArea2D( va, vb, vc );
			}

			// Choose random polygon weighted by area, using reservoi sampling.
			areaSum += polyArea;
			const float u = dtRandom();
			if ( u*areaSum <= polyArea )
			{
				randPoly = p;
				randPolyRef = *it;
			}
		}
		
		Vector3f randomPos;
		if ( dtStatusSucceed( mQuery->findRandomPointInPoly( randPolyRef, dtRandom, randomPos ) ) )
		{
			dest.mPosition = rcToLocal( randomPos );
			mGoals.resize( 0 );
			mGoals.push_back( dest );
			return true;
		}
	}

	return false;
}

void RecastPathInterface::UpdatePath()
{
	mFoundGoalIndex = -1;
	mStatus = PathInterface::PATH_NONE;

	if ( mGoals.empty() )
		return;
	
	dtPolyRef startPoly, endPoly;
	Vector3f startPos = localToRc( mSrc );
	Vector3f endPos = localToRc( mGoals[ 0 ].mPosition );
	if ( dtStatusSucceed( mQuery->findNearestPoly( startPos, PathPlannerRecast::sExtents, &mFilter, &startPoly, startPos ) ) )
	{
		mCorridor.reset( startPoly, startPos );

		if ( dtStatusSucceed( mQuery->findNearestPoly( endPos, PathPlannerRecast::sExtents, &mFilter, &endPoly, endPos ) ) )
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
