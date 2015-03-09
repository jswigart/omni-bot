////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy: jswigart@gmail.com $
// $LastChangedDate: 2013-05-10 02:12:26 -0500 (Fri, 10 May 2013) $
// $LastChangedRevision: 908 $
//
////////////////////////////////////////////////////////////////////////////////

#include "PathPlannerNavMeshPathInterface.h"
#include "RenderBuffer.h"
#include "Client.h"

//////////////////////////////////////////////////////////////////////////

bool node_compare( const PathFindNavMesh::PlanNode* _node1, const PathFindNavMesh::PlanNode* _node2 )
{
	return _node1->CostFinal > _node2->CostFinal;
}

//////////////////////////////////////////////////////////////////////////

obint32 PathFindNavMesh::PlanNode::Hash() const
{
	return (obint32)Utils::MakeId32(
		(obint16)Sector->mIndex,
		Portal ? (obint16)Portal->mDstSector : (obint16)0xFF );
}

void PathFindNavMesh::PlanNode::Reset()
{
	Sector = 0;
	Parent = 0;

	Portal = 0;

	CostHeuristic = 0.f;
	CostGiven = 0.f;
	CostFinal = 0.f;
}

PathFindNavMesh::NodeList &PathFindNavMesh::GetSolution()
{
	return m_Solution;
}

bool PathFindNavMesh::IsFinished() const
{
	return m_OpenList.empty();
}
bool PathFindNavMesh::FoundGoal() const
{
	return FoundGoalIndex != -1;
}
int PathFindNavMesh::GetGoalIndex() const
{
	return FoundGoalIndex;
}
void PathFindNavMesh::StartNewSearch()
{
	m_CurrentGoals.resize( 0 );

	m_ClosedList.clear();
	m_OpenList.resize( 0 );
	m_Solution.resize( 0 );

	m_UsedNodes = 0;

	FoundGoalIndex = -1;
}
bool PathFindNavMesh::AddStart( const Vector3f &_pos )
{
	PathPlannerNavMesh::RuntimeNavSector *pSector = mNavMesh->GetSectorAt( _pos, 512.f );
	if ( pSector )
	{
		PlanNode *pNode = _GetFreeNode();
		pNode->Parent = 0;
		pNode->Position = _pos;
		pNode->Sector = pSector;
		m_OpenList.push_back( pNode );
		return true;
	}
	return false;
}
bool PathFindNavMesh::AddGoal( const Vector3f &_pos )
{
	GoalLocation gl;
	gl.Position = _pos;
	gl.Sector = mNavMesh->GetSectorAt( _pos, 512.f );
	if ( gl.Sector )
	{
		m_CurrentGoals.push_back( gl );
		return true;
	}
	return false;
}
void PathFindNavMesh::Iterate( int _numsteps )
{
	while ( !IsFinished() && _numsteps-- )
	{
		// get the next node
		PlanNode *pCurNode = _GetNextOpenNode();
		if ( !pCurNode )
		{
			break;
		}

		// Push it on the list so it doesn't get considered again.
		_MarkClosed( pCurNode );

		// Is it the goal?
		GoalLocation *gl = _IsNodeGoalInSector( pCurNode );

		//////////////////////////////////////////////////////////////////////////
		if ( gl != NULL && SquaredLength( gl->Position, pCurNode->Position ) < Mathf::Sqr( 32.f ) )
		{
			FoundGoalIndex = (int)( gl - &m_CurrentGoals[ 0 ] );
			m_OpenList.resize( 0 ); // finished
			m_Solution.resize( 0 );

			// add all the nodes, goal->start
			while ( pCurNode )
			{
				m_Solution.push_back( pCurNode );
				pCurNode = pCurNode->Parent;
			}
			break;
		}
		//////////////////////////////////////////////////////////////////////////

		// Expand the node
		_ExpandNode( pCurNode, gl );
	}
}
void PathFindNavMesh::Render()
{
	for ( int i = 0; i < m_UsedNodes; ++i )
	{
		PlanNode *pNode = &m_Nodes[ i ];

		obColor col = COLOR::BLACK;
		if ( IsOnOpenList( pNode ) != m_OpenList.end() )
			col = COLOR::GREEN;

		if ( pNode->Parent )
			RenderBuffer::AddLine( pNode->Position, pNode->Parent->Position, col, 2.f );
	}
}
PathFindNavMesh::PathFindNavMesh( PathPlannerNavMesh * navmesh )
	: mNavMesh( navmesh )
{
	m_UsedNodes = 0;
}

PathFindNavMesh::PlanNode *PathFindNavMesh::_GetFreeNode()
{
	PlanNode *pNode = &m_Nodes[ m_UsedNodes++ ];
	pNode->Reset();
	return pNode;
}
PathFindNavMesh::PlanNode *PathFindNavMesh::_GetNextOpenNode()
{
	PlanNode *pCurNode = NULL;

	if ( !m_OpenList.empty() )
	{
		pCurNode = m_OpenList.front();
		std::pop_heap( m_OpenList.begin(), m_OpenList.end(), node_compare );
		m_OpenList.pop_back();
	}

	return pCurNode;
}
void PathFindNavMesh::_MarkClosed( PlanNode *_node )
{
	m_ClosedList[ _node->Hash() ] = _node;
}
PathFindNavMesh::NodeClosedList::iterator PathFindNavMesh::IsOnClosedList( PlanNode *_node )
{
	NodeClosedList::iterator it = m_ClosedList.find( _node->Hash() );
	return it;
}
PathFindNavMesh::NodeList::iterator PathFindNavMesh::IsOnOpenList( PlanNode *_node )
{
	float fClosest = 1000000.f;
	NodeList::iterator itRet = m_OpenList.end();

	NodeList::iterator it = m_OpenList.begin(), itEnd = m_OpenList.end();
	for ( ; it != itEnd; ++it )
	{
		if ( ( *it )->Sector == _node->Sector )
		{
			const float fSqDist = SquaredLength( ( *it )->Position, _node->Position );
			if ( fSqDist < fClosest )
			{
				fClosest = fSqDist;
				itRet = it;
			}
		}
	}
	return itRet;
}

void PathFindNavMesh::HeapInsert( NodeList &_wpl, PlanNode *_node )
{
	_wpl.push_back( _node );
	std::push_heap( _wpl.begin(), _wpl.end(), node_compare );
}

void PathFindNavMesh::_ExpandNode( PlanNode *_node, GoalLocation *_goal )
{
	if ( _goal )
	{
		PlanNode *pNextNode = _GetFreeNode();
		pNextNode->Portal = 0;
		pNextNode->Parent = _node;
		pNextNode->Sector = _node->Sector;
		pNextNode->Position = _goal->Position; // TODO: branch more
		pNextNode->CostHeuristic = 0;
		pNextNode->CostGiven = _node->CostGiven + Length( _node->Position, pNextNode->Position );
		pNextNode->CostFinal = pNextNode->CostHeuristic + pNextNode->CostGiven;
		HeapInsert( m_OpenList, pNextNode );
		return;
	}

	for ( size_t p = 0; p < _node->Sector->mPortals.size(); ++p )
	{
		const PathPlannerNavMesh::NavPortal &portal = _node->Sector->mPortals[ p ];

		//////////////////////////////////////////////////////////////////////////
		PlanNode tmpNext;
		tmpNext.Reset();
		tmpNext.Portal = &portal;
		tmpNext.Parent = _node;
		tmpNext.Sector = portal.mDstSector;
		tmpNext.Position = portal.mSegment.Center; // TODO: branch more

		//if(m_CurrentGoals.size()==1)
		//	tmpNext.CostHeuristic = Length(m_CurrentGoals.front().Position,tmpNext.Position); // USE IF 1 GOAL!
		//else
		tmpNext.CostHeuristic = 0;

		tmpNext.CostGiven = _node->CostGiven + Length( _node->Position, tmpNext.Position );
		tmpNext.CostFinal = tmpNext.CostHeuristic + tmpNext.CostGiven;

		//////////////////////////////////////////////////////////////////////////

		// Look in closed list for this. If this is better, re-open it
		NodeClosedList::iterator closedIt = IsOnClosedList( &tmpNext );
		if ( closedIt != m_ClosedList.end() )
		{
			PlanNode *OnClosed = closedIt->second;
			if ( OnClosed->CostGiven > tmpNext.CostGiven )
			{
				*OnClosed = tmpNext;

				// and remove from the closed list.
				m_ClosedList.erase( closedIt );
				// ... back into the open list
				HeapInsert( m_OpenList, OnClosed );
			}
			continue;
		}

		// Look in open list for this. If this is better, update it.
		// Check the open list for this node to see if it's better
		NodeList::iterator openIt = IsOnOpenList( &tmpNext );
		if ( openIt != m_OpenList.end() )
		{
			PlanNode *pOnOpen = ( *openIt );
			if ( pOnOpen->CostGiven > tmpNext.CostGiven )
			{
				// Update the open list
				*pOnOpen = tmpNext;

				// Remove it from the open list first.
				//m_OpenList.erase(openIt);
				// ... and re-insert it

				// Since we possibly removed from the middle, we need to fix the heap again.
				//std::make_heap(m_OpenList.begin(), m_OpenList.end(), waypoint_comp);
				std::push_heap( m_OpenList.begin(), openIt + 1, node_compare );
			}
			continue;
		}

		// New node to explore
		PlanNode *pNextNode = _GetFreeNode();
		*pNextNode = tmpNext;
		HeapInsert( m_OpenList, pNextNode );
	}
}
PathFindNavMesh::GoalLocation *PathFindNavMesh::_IsNodeGoalInSector( PlanNode *_node )
{
	for ( obuint32 i = 0; i < m_CurrentGoals.size(); ++i )
	{
		if ( m_CurrentGoals[ i ].Sector == _node->Sector )
		{
			return &m_CurrentGoals[ i ];
		}
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////

NavMeshPathInterface::NavMeshPathInterface( Client * client, PathPlannerNavMesh * nav )
	: mClient( client )
	, mNavMesh( nav )
	, mStatus( PATH_NONE )
	, mMoveDirection( Vector3f::ZERO )
	, mPathFind( nav )
{
}

PathInterface::PathStatus NavMeshPathInterface::GetPathStatus() const
{
	return mStatus;
}

void NavMeshPathInterface::UpdateSourcePosition( const Vector3f & srcPos )
{
	mSrc = srcPos;
	mSrcChanged = true;
}

void NavMeshPathInterface::UpdateGoalPosition( const Vector3f & goal, float radius )
{
	mGoals.resize( 1 );
	mGoals[ 0 ].m_Position = goal;
	mGoals[ 0 ].m_Radius = radius;
}

void NavMeshPathInterface::UpdateGoalPositions( const DestinationVector & goals )
{
	mGoals = goals;
}

bool NavMeshPathInterface::UpdateGoalPositionRandom()
{
	mGoals.resize( 0 );

	Destination dest;
	dest.m_Position = mSrc;
	dest.m_Radius = 4.0f;

	if ( mNavMesh->GetRandomDestination( dest.m_Position, mClient ) )
	{
		mGoals.push_back( dest );
		return true;
	}
	return false;
}

void NavMeshPathInterface::UpdatePath()
{
	mFoundGoalIndex = -1;

	mPath.resize( 0 );

	mStatus = PATH_SEARCHING;

	mPathFind.StartNewSearch();
	mPathFind.AddStart( mSrc );

	for ( obuint32 i = 0; i < mGoals.size(); ++i )
		mPathFind.AddGoal( mGoals[ i ].m_Position );

	// todo: time splice?
	while ( !mPathFind.IsFinished() )
		mPathFind.Iterate();

	mFoundGoalIndex = mPathFind.GetGoalIndex();

	if ( mFoundGoalIndex == -1 )
	{
		mStatus = PATH_NOPATHTOGOAL;
		return;
	}

	// remove all goals but the one we chose
	// in case we need to replan
	mStatus = PATH_VALID;
	mGoals[ 0 ] = mGoals[ mFoundGoalIndex ];
	mGoals.resize( 1 );

	PathFindNavMesh::NodeList &nl = mPathFind.GetSolution();

	while ( !nl.empty() )
	{
		PathFindNavMesh::PlanNode *pn = nl.back();
		Vector3f vNodePos = pn->Position;

		NavFlags navFlags = (NavFlags)0;
		if ( pn->Portal )
		{
			/*static const float pushDist = 32.0f;
			Vector3f portalNormal = pn->Portal->mSegment.Direction.Cross( Vector3f::UNIT_Z );
			vNodePos += portalNormal * pushDist;*/

			/*if ( pn->Portal->mFlags.CheckFlag( PathPlannerNavMesh::NavPortal::FL_JUMP ) )
				navFlags |= F_NAV_JUMPLOW;

				if ( pn->Portal->mFlags.CheckFlag( PathPlannerNavMesh::NavPortal::FL_WATER_SWIM ) )
				navFlags |= F_NAV_INWATER;

				if ( pn->Portal->mFlags.CheckFlag( PathPlannerNavMesh::NavPortal::FL_WATER_EXIT ) )
				navFlags |= F_NAV_CLIMB;*/
		}

		NavMeshNode node;
		node.mPos = pn->Position;
		node.mPortal = pn->Portal;
		node.mFlags = navFlags;
		node.mSector = pn->Sector;
		node.mSectorVersion = pn->Sector->mVersion; // store the 'static' sector version

		mPath.push_back( node );

		nl.pop_back();
	}
}

void NavMeshPathInterface::Cancel()
{
	mStatus = PATH_NONE;
	mMoveDirection = Vector3f::ZERO;
}

void NavMeshPathInterface::UpdateCurrentSector()
{
	Vector3f groundProbeSrc = mSrc + Vector3f( 0.0f, 0.0f, 16.0f );
	Vector3f groundProbeDst = mSrc - Vector3f( 0.0f, 0.0f, 2048.0f );
	PathPlannerNavMesh::NavCollision col = mNavMesh->FindCollision( groundProbeSrc, groundProbeDst, &mCurrentSectorCache );
	mCurrentSector = col.HitSector();

	mPathNodeIndex = 0;

	if ( col.DidHit() )
	{
		bool offPath = mPath.size() > 0;
		bool pathChanged = false;

		for ( size_t i = 0; i < mPath.size(); ++i )
		{
			const NavMeshNode & node = mPath[ i ];
			if ( node.mPortal != NULL )
			{
				// as long as we follow our expected sectors, we're good
				// if we get knocked off or go into an unexpected sector
				// we need to re-path
				if ( mPathNodeIndex == 0 )
				{
					if ( node.mPortal->mSrcSector == mCurrentSector )
					{
						offPath = false;
						mPathNodeIndex = i;
					}
				}

				/*if ( node.mPortal->mDstSector == mCurrentSector )
				{
				mPathNodeIndex = i;
				return;
				}*/

				// path goes through a changed sector, need to repath
				if ( node.mSectorVersion != node.mSector->mVersion )
				{
					pathChanged = true;
				}
			}
		}

		if ( offPath || pathChanged )
			UpdatePath();
	}
}

size_t NavMeshPathInterface::GetPathCorners( PathCorner * corners, size_t maxEdges )
{
	//if ( mSrcChanged )
	{
		UpdateCurrentSector();
		mSrcChanged = false;
	}

	size_t num = 0;

	if ( mPathNodeIndex < mPath.size() )
	{
		Vector3f lastPt = mPath[ mPathNodeIndex ].mPos;

		for ( size_t i = mPathNodeIndex + 0; i < mPath.size() && num < maxEdges; ++i )
		{
			const NavMeshNode & node = mPath[ i ];

			if ( node.mPortal != NULL )
			{
				//corners[ num ].mSrc = node.mPos + node.mPortal->mSegmentFwd * radius;
				//corners[ num ].mDst = node.mPos - node.mPortal->mSegmentFwd * radius;
				//corners[ num ].mFlags = node.mFlags;
				//corners[ num ].mWaterDepth = node.mSector->mSectorData->waterdepth();

				//Utils::PushPointToPlane( corners[ num ].mDst, node.mPortal->mDstSector->GetPlane(), -Vector3f::UNIT_Z );

				//lastPt = corners[ num ].mDst;
				++num;
			}
			else if ( i != mPathNodeIndex )
			{
				//corners[ num ].mSrc = lastPt;
				//corners[ num ].mDst = node.mPos;
				//corners[ num ].mFlags = node.mFlags;
				//corners[ num ].mWaterDepth = node.mSector->mSectorData->waterdepth();

				//lastPt = corners[ num ].mDst;
				++num;
			}
		}
	}
	else
	{
		/*corners[ num ].mSrc = mSrc;
		corners[ num ].mDst = mSrc;
		corners[ num ].mFlags = 0;
		corners[ num ].mWaterDepth = 0.0f;*/
		++num;
	}

	return num;
}

bool NavMeshPathInterface::GetPointAlongPath( float lookAheadDist, Vector3f & ptOut )
{
	if ( mStatus != PATH_VALID )
		return false;

	return true;
}

void NavMeshPathInterface::Render()
{
	//mPathFind.Render();

	enum
	{
		MaxEdges = 64
	};
	PathInterface::PathCorner edges[ MaxEdges ];
	const size_t pathEdgeCount = GetPathCorners( edges, MaxEdges );

	//const Vector3f offs( 0.0f, 0.0f, NavigationMeshOptions::CharacterHeight );

	for ( size_t i = 0; i < pathEdgeCount; ++i )
	{
		RenderBuffer::AddArrow( edges[ i ].mPos, edges[ i ].mPos, COLOR::ORANGE );
	}
}