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
#include "DetourDebugDraw.h"

#include "Client.h"

static float WALL_RADIUS = 16.0f;

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

	mHitPos = Vector3f::ZERO;
	mHitNormal = Vector3f::ZERO;
	mHitDist = WALL_RADIUS;
}

RecastPathInterface::~RecastPathInterface()
{
	dtFreeNavMeshQuery( mQuery );

	sInterfaces.erase( std::remove( sInterfaces.begin(), sInterfaces.end(), this ), sInterfaces.end() );
}

void RecastPathInterface::ReInit()
{
	mQuery->init( mNav->mNavMesh, 2048 );
	mStatus = PATH_NONE;
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
		UpdatePath( true );

	mSrc = srcPos;

	Vector3f rcSrc = localToRc( mSrc );

	dtPolyRef currentPoly = 0;
	if ( mCorridor.getPathCount() > 0 )
	{	
		if ( !mCorridor.movePosition( rcSrc, mQuery, &mFilter ) )
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
		if ( dtStatusSucceed( mQuery->findNearestPoly( rcSrc, PathPlannerRecast::sExtents, &mFilter, &startPoly, rcSrc ) ) )
			currentPoly = startPoly;
	}

	mCurrentAreaFlags = NAVFLAGS_NONE;

	if ( currentPoly != 0 )
	{
		// cache the wall distance
		if ( dtStatusSucceed( mQuery->findDistanceToWall( currentPoly, rcSrc, WALL_RADIUS, &mFilter, &mHitDist, mHitPos, mHitNormal ) ) )
		{
			mHitPos = rcToLocal( mHitPos );
			mHitNormal = rcToLocal( mHitNormal );
		}
		else
		{
			mHitDist = WALL_RADIUS;
		}

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

void RecastPathInterface::UpdatePath( bool forceRecalculate )
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
		// go to the nearest if multiple goals are provided
		if ( mGoals.size() > 1 )
		{
			std::vector<dtMultiPathGoal> goals( mGoals.size() );

			for ( size_t i = 0; i < mGoals.size(); ++i )
			{
				dtMultiPathGoal& goal = goals[ i ];

				dtVcopy( goal.mDest, localToRc( mGoals[ i ].mPosition ) );
				if ( dtStatusFailed( mQuery->findNearestPoly( goal.mDest, PathPlannerRecast::sExtents, &mFilter, &goal.mDestPoly, goal.mDest ) ) )
					goal.mDestPoly = 0;
			}

			if ( dtStatusFailed( mQuery->findMultiPath( startPoly, startPos, &mFilter, &goals[ 0 ], goals.size() ) ) )
				return;

			// use the best one as the goal
			mFoundGoalIndex = 0;
			for ( size_t i = 1; i < goals.size(); ++i )
			{
				if ( goals[ i ].mNavDist < goals[ mFoundGoalIndex ].mNavDist )
					mFoundGoalIndex = i;
			}

			endPoly = goals[ mFoundGoalIndex ].mDestPoly;
			dtVcopy( endPos, goals[ mFoundGoalIndex ].mDest );
		}
		else if ( !dtStatusSucceed( mQuery->findNearestPoly( endPos, PathPlannerRecast::sExtents, &mFilter, &endPoly, endPos ) ) )
		{
			return;
		}
		else
		{
			mFoundGoalIndex = 0;
		}
		
		// try to re-use current path state
		//if ( forceRecalculate || mCorridor.getPathCount() == 0 )
			mCorridor.reset( startPoly, startPos );
		//else
		//{
		//	// do we need to recalculate the path?
		//	if ( mCorridor.moveTargetPosition( endPos, mQuery, &mFilter ) )
		//	{
		//		mStatus = PATH_VALID;
		//		return;
		//	}
		//}

		int	polyCount = 0;
		dtPolyRef * polys = (dtPolyRef*)StackAlloc( sizeof( dtPolyRef ) * mCorridor.getMaxPathCount() );
#if(0)
		dtStatus status = mQuery->findPath( startPoly, endPoly, startPos, endPos, &mFilter, polys, &polyCount, mCorridor.getMaxPathCount() );
#else
		dtStatus status = mQuery->initSlicedFindPath( startPoly, endPoly, startPos, endPos, &mFilter, DT_FINDPATH_ANY_ANGLE );
		if ( status & DT_IN_PROGRESS )
		{
			do 
			{
				status = mQuery->updateSlicedFindPath( 10000000, NULL );
			}
			while ( status & DT_IN_PROGRESS );
		}

		status = mQuery->finalizeSlicedFindPath( polys, &polyCount, mCorridor.getMaxPathCount() );
#endif

		static bool showCorridor = false;
		if ( showCorridor )
		{
			static float duration = 2.0f;

			DebugDraw ddraw( duration );
			duDebugDrawCorridor( &ddraw, *mQuery->getAttachedNavMesh(), &mCorridor, duRGBA( 0, 255, 0, 128 ) );
		}

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

void RecastPathInterface::Cancel()
{
	mStatus = PATH_NONE;
	mMoveDirection = Vector3f::ZERO;
}

size_t RecastPathInterface::GatherPolyEntities( dtPolyRef poly, GameEntity entities[], size_t maxEntities )
{
	navAreaMask af = NAVFLAGS_NONE;
	if ( !dtStatusSucceed( mNav->mNavMesh->getPolyFlags( poly, &af ) ) )
		return 0;

	int entIndex = 0;

	// if the area references entities, we can provide them as well
	if ( ( af & NAVFLAGS_ENTITYREF_MASK ) != 0 )
	{
		const dtMeshTile* tile = static_cast<const dtNavMesh*>( mNav->mNavMesh )->getTileByRef( poly );
		if ( tile != NULL )
		{
			navAreaMask ref = ( af & NAVFLAGS_ENTITYREF_MASK ) >> 32;

			// area may reference multiple user refs, so grab them all
			int refIndex = 0;
			while ( ref != 0 && entIndex < PathCorner::MAX_ENTITIES )
			{
				if ( ref & 1 )
				{
					const dtUserRef& uref = tile->userRefs[ refIndex++ ];

					// dont include it more than once
					bool exists = false;
					for ( int i = 0; i < entIndex && !exists; ++i )
					{ 
						if ( entities[ i ].AsInt() == uref.id )
							exists = true;
					}

					if ( !exists )
					{
						entities[ entIndex++ ].FromInt( uref.id );
					}
				}

				++refIndex;
				ref = ref >> 1;
			}
		}
	}
	return entIndex;
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

		GatherPolyEntities( cornerPoly[ i ], corners[ numCorners ].mEntities, PathCorner::MAX_ENTITIES );

		corners[ numCorners ].mPos = rcToLocal( cornerVerts[ i ] );
		corners[ numCorners ].mAreaMask = areaFlags;
		corners[ numCorners ].mPolyId = cornerPoly[ i ];
		corners[ numCorners ].mIsLink = ( cornerFlags[ i ] & DT_STRAIGHTPATH_OFFMESH_CONNECTION ) != 0;
	}
	return numCorners;
}

size_t RecastPathInterface::FindAreaEntitiesInRadius( const Vector3f& pos, float radius, GameEntity entities [], size_t maxEntities )
{
	dtPolyRef startPoly;
	Vector3f startPos = localToRc( pos );

	size_t cnt = 0;
	if ( dtStatusSucceed( mQuery->findNearestPoly( startPos, PathPlannerRecast::sExtents, &mFilter, &startPoly, startPos ) ) )
	{
		static const int MAX_RESULTS = 512;
		dtPolyRef resultRefs[ MAX_RESULTS ];
		dtPolyRef resultParents[ MAX_RESULTS ];
		float resultCosts[ MAX_RESULTS ];

		int resultCount = 0;
		if ( dtStatusSucceed( mQuery->findPolysAroundCircle( startPoly, startPos, radius, &mFilter, resultRefs, resultParents, resultCosts, &resultCount, MAX_RESULTS ) ) )
		{
			for ( int i = 0; i < resultCount; ++i )
			{
				cnt += GatherPolyEntities( resultRefs[ i ], &entities[ cnt ], maxEntities - cnt );
			}
		}
	}
	return cnt;
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

bool RecastPathInterface::GetNearestWall( Vector3f& wallPos, Vector3f& wallNormal, float& wallDist )
{
	if ( mHitDist < WALL_RADIUS )
	{
		wallPos = mHitPos;
		wallNormal = mHitNormal;
		wallDist = mHitDist;
		return true;
	}
	return false;
}

bool RecastPathInterface::NavTrace( NavTraceResult& result, const Vector3f& start, const Vector3f& end )
{
	dtPolyRef startPoly;
	Vector3f startPos = localToRc( start );
	Vector3f endPos = localToRc( end );
	if ( dtStatusSucceed( mQuery->findNearestPoly( startPos, PathPlannerRecast::sExtents, &mFilter, &startPoly, startPos ) ) )
	{
		dtRaycastHit hit;
		if ( dtStatusSucceed( mQuery->raycast( startPoly, startPos, endPos, &mFilter, DT_RAYCAST_USE_COSTS, &hit, NULL ) ) )
		{
			result.t = hit.t;
			result.pos = rcToLocal( hit.endPos );
			result.normal = rcToLocal( hit.hitNormal );
			return true;
		}
	}
	return true;
}
