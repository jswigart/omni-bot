////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy: jswigart@gmail.com $
// $LastChangedDate: 2013-05-10 02:12:26 -0500 (Fri, 10 May 2013) $
// $LastChangedRevision: 908 $
//
////////////////////////////////////////////////////////////////////////////////

#include "PathPlannerFloodFillPathInterface.h"
#include "RenderBuffer.h"

//////////////////////////////////////////////////////////////////////////

FloodFillPathInterface::FloodFillPathInterface( PathPlannerFloodFill * nav ) 
	: mNav( nav )
	, mStatus( PATH_NONE )
	, mMoveDirection( Vector3f::ZERO )
{
	mInfluence = mNav->AllocInfluenceMap();
}

PathInterface::PathStatus FloodFillPathInterface::GetPathStatus() const
{
	return mStatus;
}

NavAreaFlags FloodFillPathInterface::GetCurrentAreaFlags() const
{
	return NAVFLAGS_NONE;
}

void FloodFillPathInterface::UpdateNavFlags( NavFlags includeFlags, NavFlags excludeFlags )
{
}

void FloodFillPathInterface::UpdateSourcePosition( const Vector3f & srcPos )
{
	mSrc = srcPos;
}

void FloodFillPathInterface::UpdateGoalPosition( const Vector3f & goal, float radius )
{
	mGoals.resize( 1 );
	mGoals[ 0 ].mPosition = goal;
	mGoals[ 0 ].mRadius = radius;
}

void FloodFillPathInterface::UpdateGoalPositions( const DestinationVector & goals )
{
	mGoals = goals;
}

bool FloodFillPathInterface::UpdateGoalPositionRandom()
{
	mGoals.resize( 0 );

	Destination dest;
	dest.mPosition = mSrc;
	dest.mRadius = 4.0f;

	/*const PathPlannerNavMesh::RuntimeSectorList & sectors = mNavMesh->mRuntimeSectors;
	if ( !sectors.empty() )
	{
		const PathPlannerNavMesh::RuntimeNavSector & sec = sectors[ rand()% sectors.size() ];
		dest.mPosition = sec.CalculateCenter();
	}*/
	mGoals.push_back( dest );
	return true;
}

void FloodFillPathInterface::UpdatePath( bool forceRecalculate )
{
	mFoundGoalIndex = -1;

	mStatus = PATH_SEARCHING;

	//mPath.resize( 0 );
	//mPathFind.StartNewSearch();
	//mPathFind.AddStart( mSrc );

	//for(uint32_t i = 0; i < mGoals.size(); ++i)
	//	mPathFind.AddGoal( mGoals[i].mPosition );

	//// todo: time splice?
	//while(!mPathFind.IsFinished())
	//	mPathFind.Iterate();

	//mFoundGoalIndex = mPathFind.GetGoalIndex();

	//if ( mFoundGoalIndex == -1 )
	//{
	//	mStatus = PATH_NOPATHTOGOAL;
	//	return;
	//}

	//// remove all goals but the one we chose
	//// in case we need to replan
	//mStatus = PATH_VALID;
	//mGoals[ 0 ] = mGoals[ mFoundGoalIndex ];
	//mGoals.resize( 1 );
	//
	//const float CHAR_HALF_HEIGHT = NavigationMeshOptions::CharacterHeight * 0.75f;

	//PathFindNavMesh::NodeList &nl = mPathFind.GetSolution();

	//while(!nl.empty())
	//{
	//	PathFindNavMesh::PlanNode *pn = nl.back();
	//	Vector3f vNodePos = pn->Position;

	//	NavFlags navFlags = (NavFlags)0;
	//	if ( pn->Portal )	
	//	{
	//		/*static const float pushDist = 32.0f;
	//		Vector3f portalNormal = pn->Portal->mSegment.Direction.Cross( Vector3f::UNIT_Z );
	//		vNodePos += portalNormal * pushDist;*/

	//		if ( pn->Portal->mFlags.CheckFlag( PathPlannerNavMesh::NavPortal::FL_JUMP ) )
	//			navFlags |= F_NAV_JUMPLOW;

	//		if ( pn->Portal->mFlags.CheckFlag( PathPlannerNavMesh::NavPortal::FL_WATER_SWIM ) )
	//			navFlags |= F_NAV_INWATER;

	//		if ( pn->Portal->mFlags.CheckFlag( PathPlannerNavMesh::NavPortal::FL_WATER_EXIT ) )
	//			navFlags |= F_NAV_CLIMB;
	//	}

	//	NavMeshNode node;
	//	node.mPos = pn->Position;
	//	node.mPos.Z() += CHAR_HALF_HEIGHT;
	//	node.mPortal = pn->Portal;
	//	node.mFlags = navFlags;
	//	node.mSector = pn->Sector;
	//	node.mSectorVersion = pn->Sector->mVersion; // store the 'static' sector version

	//	mPath.push_back( node );

	//	/*_path.AddPt(vNodePos + Vector3f(0,0,CHAR_HALF_HEIGHT),32.f)
	//	.NavId( pn->Sector->mIndex )
	//	.Flags( navFlags );*/

	//	nl.pop_back();
	//}
}

void FloodFillPathInterface::Cancel()
{
	mStatus = PATH_NONE;
	mMoveDirection = Vector3f::ZERO;
}

size_t FloodFillPathInterface::GetPathCorners( PathCorner * corners, size_t maxEdges )
{
	size_t num = 0;

	//if ( mSrcChanged )
	/*{
		UpdateCurrentSector();
		mSrcChanged = false;
	}
	
	const float radius = 32.0f;

	if ( mPathNodeIndex < mPath.size() )
	{
		Vector3f lastPt = mPath[ mPathNodeIndex ].mPos;

		for ( size_t i = mPathNodeIndex + 0; i < mPath.size() && num < maxEdges; ++i )
		{
			const NavMeshNode & node = mPath[ i ];

			if ( node.mPortal != NULL )
			{
				corners[ num ].mSrc = node.mPos + node.mPortal->mSegmentFwd * radius;
				corners[ num ].mDst = node.mPos - node.mPortal->mSegmentFwd * radius;
				corners[ num ].mFlags = node.mFlags;
				corners[ num ].mWaterDepth = node.mSector->mSectorData->waterdepth();

				Utils::PushPointToPlane( corners[ num ].mDst, node.mPortal->mDstSector->mPlane, -Vector3f::UNIT_Z );

				lastPt = corners[ num ].mDst;
				++num;
			}
			else if ( i != mPathNodeIndex )
			{
				corners[ num ].mSrc = lastPt;
				corners[ num ].mDst = node.mPos;
				corners[ num ].mFlags = node.mFlags;
				corners[ num ].mWaterDepth = node.mSector->mSectorData->waterdepth();

				lastPt = corners[ num ].mDst;
				++num;
			}
		}
	}
	else
	{
		corners[ num ].mSrc = mSrc;
		corners[ num ].mDst = mSrc;
		corners[ num ].mFlags = 0;
		corners[ num ].mWaterDepth = 0.0f;
		++num;
	}*/

	return num;
}

bool FloodFillPathInterface::GetNavLink( uint64_t id, OffMeshConnection& conn ) const
{
	return false;
}

bool FloodFillPathInterface::CompleteNavLink( uint64_t id )
{
	return true;
}

void FloodFillPathInterface::Render() 
{
	//mPathFind.Render();

	enum { MaxEdges = 64 };
	PathInterface::PathCorner edges[ MaxEdges ];
	const size_t pathEdgeCount = GetPathCorners( edges, MaxEdges );

	//const Vector3f offs( 0.0f, 0.0f, NavigationMeshOptions::CharacterHeight );

	for ( size_t i = 1; i < pathEdgeCount; ++i )
	{
		RenderBuffer::AddArrow( edges[ i-1 ].mPos, edges[ i ].mPos, COLOR::ORANGE );
	}
}

bool FloodFillPathInterface::GetNearestWall( Wm5::Vector3f &, Wm5::Vector3f &, float & )
{
	return false;
}
