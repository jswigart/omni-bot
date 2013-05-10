////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "PathPlannerNavMesh.h"

#include <algorithm>

#include "platformspecifics.h"

#include "InterfaceFuncs.h"
#include "IGameManager.h"
#include "IGame.h"
#include "GoalManager.h"
#include "NavigationFlags.h"
#include "FileSystem.h"
#include "Path.h"
#include "ProtoBufUtility.h"

#include "RenderBuffer.h"

#include "clipper.hpp"
#include "polypartition.h"

// Protocol buffers
#include "google/protobuf/text_format.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"

#include "Opcode.h"
#include "OPC_IceHook.h"

static float DROP_HEIGHT = 64.f;
static float STEP_HEIGHT = 20.f;
static float JUMP_HEIGHT = 60.f;
static float LINE_DIST = 2.f;
static float WATER_EXIT = 16.0f;

//////////////////////////////////////////////////////////////////////////

static void ConvertPoly( Vector3List & polyOut, const PathPlannerNavMesh::RuntimeNavSector & ns, const TPPLPoly & poly )
{
	polyOut.reserve( poly.GetNumPoints() );
	for ( int i = 0; i < poly.GetNumPoints(); ++i )
	{
		Vector3f vec3d( (float)poly[ i ].x, (float)poly[ i ].y, 0.0f );
		Utils::PushPointToPlane( vec3d, ns.mPlane, -Vector3f::UNIT_Z );
		polyOut.push_back( vec3d );
	}
}

static void ConvertPoly( Vector3List & polyOut, const PathPlannerNavMesh::RuntimeNavSector & ns, const ClipperLib::Polygon & poly )
{
	polyOut.reserve( poly.size() );
	for ( size_t i = 0; i < poly.size(); ++i )
	{
		const ClipperLib::IntPoint & pt = poly[ i ];
		Vector3f vec3d( (float)pt.X, (float)pt.Y, 0.0f );
		Utils::PushPointToPlane( vec3d, ns.mPlane, -Vector3f::UNIT_Z );
		polyOut.push_back( vec3d );
	}
}

static void ConvertPoly( ClipperLib::Polygon & polyOut, const Vector3List & poly )
{
	polyOut.resize( poly.size() );
	for ( size_t i = 0; i < poly.size(); ++i )
	{
		polyOut[ i ] = ClipperLib::IntPoint(
			(ClipperLib::long64)poly[i].X(),
			(ClipperLib::long64)poly[i].Y() );
	}
}

static void ConvertPoly( TPPLPoly & polyOut, const ClipperLib::Polygon & poly )
{
	polyOut.Init( poly.size() );
	for ( size_t i = 0; i < poly.size(); ++i )
	{
		const ClipperLib::IntPoint & pt = poly[ i ];

		TPPLPoint & tpp = polyOut[ i ];
		tpp.x = (float)pt.X;
		tpp.y = (float)pt.Y;
	}
}

//////////////////////////////////////////////////////////////////////////
namespace NavigationMeshOptions
{
	float CharacterHeight = 64.f;
};
//////////////////////////////////////////////////////////////////////////

PathPlannerNavMesh *g_PlannerNavMesh = 0;

//////////////////////////////////////////////////////////////////////////
// Pathing vars
class PathFindNavMesh
{
public:
	struct PlanNode
	{
		PathPlannerNavMesh::RuntimeNavSector *	Sector;
		Vector3f								Position;
		PlanNode *								Parent;
		const PathPlannerNavMesh::NavPortal *	Portal;
		
		float									CostHeuristic;
		float									CostGiven;
		float									CostFinal;

		obint32 Hash() const
		{
			return (obint32)Utils::MakeId32(
				(obint16)Sector->mIndex,
				Portal ? (obint16)Portal->mDstSector : (obint16)0xFF);
		}

		void Reset()
		{
			Sector = 0;
			Parent = 0;

			Portal = 0;
			
			CostHeuristic = 0.f;
			CostGiven = 0.f;
			CostFinal = 0.f;
		}
	};
	typedef std::vector<PlanNode*> NodeList;

	NodeList &GetSolution() { return m_Solution; }

	bool IsFinished() const
	{
		return m_OpenList.empty();
	}
	bool FoundGoal() const
	{
		return FoundGoalIndex != -1;
	}
	int GetGoalIndex() const
	{
		return FoundGoalIndex;
	}
	void StartNewSearch()
	{
		m_CurrentGoals.resize(0);

		m_ClosedList.clear();
		m_OpenList.resize(0);
		m_Solution.resize(0);

		m_UsedNodes = 0;

		FoundGoalIndex = -1;
	}
	bool AddStart(const Vector3f &_pos)
	{
		PathPlannerNavMesh::RuntimeNavSector *pSector = g_PlannerNavMesh->GetSectorAt(_pos,512.f);
		if(pSector)
		{
			PlanNode *pNode = _GetFreeNode();
			pNode->Parent = 0;
			pNode->Position = _pos;
			pNode->Sector = pSector;
			m_OpenList.push_back(pNode);
			return true;
		}
		return false;
	}
	bool AddGoal(const Vector3f &_pos)
	{
		GoalLocation gl;
		gl.Position = _pos;
		gl.Sector = g_PlannerNavMesh->GetSectorAt(_pos,512.f);
		if(gl.Sector)
		{
			m_CurrentGoals.push_back(gl);
			return true;
		}
		return false;
	}
	void Iterate(int _numsteps = 1)
	{
		while(!IsFinished() && _numsteps--)
		{
			// get the next node
			PlanNode *pCurNode = _GetNextOpenNode();
			if(!pCurNode)
			{
				break;
			}

			// Push it on the list so it doesn't get considered again.
			_MarkClosed(pCurNode);

			// Is it the goal?
			GoalLocation *gl = _IsNodeGoalInSector(pCurNode);

			//////////////////////////////////////////////////////////////////////////
			if(gl != NULL && SquaredLength(gl->Position,pCurNode->Position)<Mathf::Sqr(32.f))
			{
				FoundGoalIndex = (int)(gl - &m_CurrentGoals[0]);
				m_OpenList.resize( 0 ); // finished
				m_Solution.resize(0);

				// add all the nodes, goal->start
				while(pCurNode)
				{
					m_Solution.push_back(pCurNode);
					pCurNode = pCurNode->Parent;
				}
				break;
			}
			//////////////////////////////////////////////////////////////////////////

			// Expand the node
			_ExpandNode(pCurNode,gl);
		}
	}
	void Render()
	{
		for(int i = 0; i < m_UsedNodes; ++i)
		{
			PlanNode *pNode = &m_Nodes[i];

			obColor col = COLOR::BLACK;
			if(IsOnOpenList(pNode) != m_OpenList.end())
				col = COLOR::GREEN;

			if(pNode->Parent)
				RenderBuffer::AddLine(pNode->Position,pNode->Parent->Position,col,2.f);
		}
	}
	PathFindNavMesh()
	{
		m_UsedNodes = 0;
	}
private:
	struct GoalLocation
	{
		PathPlannerNavMesh::RuntimeNavSector	*Sector;
		Vector3f								Position;
	};
	typedef std::vector<GoalLocation> GoalList;
	GoalList	m_CurrentGoals;

	enum		{ MaxNodes=2048 };
	PlanNode	m_Nodes[MaxNodes];
	int			m_UsedNodes;

	static bool NAV_COMP(const PlanNode* _n1, const PlanNode* _n2)
	{
		return _n1->CostFinal > _n2->CostFinal;
	}
#ifdef WIN32
	typedef stdext::hash_compare<int> HashMapCompare;
	typedef stdext::hash_map<int, PlanNode*, HashMapCompare/*, HashMapAllocator*/ > NavHashMap;
#else
	typedef stdext::hash<int> HashMapCompare;
	typedef stdext::hash_map<int, PlanNode*, HashMapCompare, stdext::equal_to<int>/*, HashMapAllocator*/ > NavHashMap;
#endif
	NodeList			m_OpenList;
	NodeList			m_Solution;

	typedef NavHashMap NodeClosedList;
	NavHashMap			m_ClosedList;

	int									FoundGoalIndex;

	PlanNode *_GetFreeNode()
	{
		PlanNode *pNode = &m_Nodes[m_UsedNodes++];
		pNode->Reset();
		return pNode;
	}
	PlanNode *_GetNextOpenNode()
	{
		PlanNode *pCurNode = NULL;

		if(!m_OpenList.empty())
		{
			pCurNode = m_OpenList.front();
			std::pop_heap(m_OpenList.begin(), m_OpenList.end(), NAV_COMP);
			m_OpenList.pop_back();
		}

		return pCurNode;
	}
	void _MarkClosed(PlanNode *_node)
	{
		m_ClosedList[_node->Hash()] = _node;
	}
	NodeClosedList::iterator IsOnClosedList(PlanNode *_node)
	{
		NodeClosedList::iterator it = m_ClosedList.find(_node->Hash());
		return it;
	}
	NodeList::iterator IsOnOpenList(PlanNode *_node)
	{
		float fClosest = 1000000.f;
		NodeList::iterator itRet = m_OpenList.end();

		NodeList::iterator it = m_OpenList.begin(), itEnd = m_OpenList.end();
		for(; it != itEnd; ++it)
		{
			if((*it)->Sector == _node->Sector)
			{
				const float fSqDist = SquaredLength((*it)->Position, _node->Position);
				if(fSqDist < fClosest)
				{
					fClosest = fSqDist;
					itRet = it;
				}
			}
		}
		return itRet;
	}

	static bool node_compare(const PlanNode* _node1, const PlanNode* _node2)
	{
		return _node1->CostFinal > _node2->CostFinal;
	}

	void HeapInsert(NodeList &_wpl, PlanNode *_node)
	{
		_wpl.push_back(_node);
		std::push_heap(_wpl.begin(), _wpl.end(), node_compare);
	}

	void _ExpandNode(PlanNode *_node, GoalLocation *_goal)
	{
		if(_goal)
		{
			PlanNode *pNextNode = _GetFreeNode();
			pNextNode->Portal = 0;
			pNextNode->Parent = _node;
			pNextNode->Sector = _node->Sector;
			pNextNode->Position = _goal->Position; // TODO: branch more
			pNextNode->CostHeuristic = 0;
			pNextNode->CostGiven = _node->CostGiven + Length(_node->Position, pNextNode->Position);
			pNextNode->CostFinal = pNextNode->CostHeuristic + pNextNode->CostGiven;
			HeapInsert(m_OpenList, pNextNode);
			return;
		}

		for ( size_t p = 0; p < _node->Sector->mPortals.size(); ++p)
		{
			const PathPlannerNavMesh::NavPortal &portal = _node->Sector->mPortals[p];

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

			tmpNext.CostGiven = _node->CostGiven + Length(_node->Position, tmpNext.Position);
			tmpNext.CostFinal = tmpNext.CostHeuristic + tmpNext.CostGiven;

			//////////////////////////////////////////////////////////////////////////

			// Look in closed list for this. If this is better, re-open it
			NodeClosedList::iterator closedIt = IsOnClosedList(&tmpNext);
			if(closedIt != m_ClosedList.end())
			{
				PlanNode *OnClosed = closedIt->second;
				if(OnClosed->CostGiven > tmpNext.CostGiven)
				{
					*OnClosed = tmpNext;

					// and remove from the closed list.
					m_ClosedList.erase(closedIt);
					// ... back into the open list
					HeapInsert(m_OpenList, OnClosed);
				}
				continue;
			}

			// Look in open list for this. If this is better, update it.
			// Check the open list for this node to see if it's better
			NodeList::iterator openIt = IsOnOpenList(&tmpNext);
			if(openIt != m_OpenList.end())
			{
				PlanNode *pOnOpen = (*openIt);
				if(pOnOpen->CostGiven > tmpNext.CostGiven)
				{
					// Update the open list
					*pOnOpen = tmpNext;

					// Remove it from the open list first.
					//m_OpenList.erase(openIt);
					// ... and re-insert it

					// Since we possibly removed from the middle, we need to fix the heap again.
					//std::make_heap(m_OpenList.begin(), m_OpenList.end(), waypoint_comp);
					std::push_heap(m_OpenList.begin(), openIt+1, node_compare);
				}
				continue;
			}

			// New node to explore
			PlanNode *pNextNode = _GetFreeNode();
			*pNextNode = tmpNext;
			HeapInsert(m_OpenList, pNextNode);
		}
	}
	GoalLocation *_IsNodeGoalInSector(PlanNode *_node)
	{
		for(obuint32 i = 0; i < m_CurrentGoals.size(); ++i)
		{
			if(m_CurrentGoals[i].Sector == _node->Sector)
			{
				return &m_CurrentGoals[i];
			}
		}
		return NULL;
	}
};

PathFindNavMesh g_PathFind; // TEMP


class NavMeshPathInterface : public PathInterface
{
public:
	NavMeshPathInterface( PathPlannerNavMesh * nav ) 
		: mNavMesh( nav )
		, mStatus( PATH_NONE )
		, mMoveDirection( Vector3f::ZERO )
	{
	}

	virtual PathStatus GetPathStatus() const
	{
		return mStatus;
	}

	virtual void UpdateSourcePosition( const Vector3f & srcPos )
	{
		mSrc = srcPos;
		// update and re-funnel path
	}

	virtual void UpdateGoalPositions( const DestinationVector & goals )
	{
		mGoals = goals;
		//re-plan path
	}

	virtual void UpdateGoalPositionRandom()
	{
		mGoals.resize( 0 );

		Destination dest;
		dest.m_Position = mSrc;
		dest.m_Radius = 4.0f;

		const PathPlannerNavMesh::RuntimeSectorList & sectors = mNavMesh->mRuntimeSectors;
		if ( !sectors.empty() )
		{
			const PathPlannerNavMesh::RuntimeNavSector & sec = sectors[ rand()% sectors.size() ];
			dest.m_Position = sec.CalculateCenter();
		}
		mGoals.push_back( dest );
	}

	virtual void UpdatePath()
	{
		//repath
	}

	virtual void Cancel()
	{
		mStatus = PATH_NONE;
		mMoveDirection = Vector3f::ZERO;
	}

	virtual size_t GetNextMoveEdges( PathEdge * corners, size_t maxEdges )
	{
		return 0;
	}
	
	virtual bool GetPointAlongPath( float lookAheadDist, Vector3f & ptOut )	
	{
		if ( mStatus != PATH_VALID )
			return false;

		return true;
	}

	virtual void Render() 
	{

	}
private:
	PathPlannerNavMesh *mNavMesh;
	PathStatus			mStatus;

	Vector3f			mSrc;
	DestinationVector	mGoals;

	Vector3f			mMoveDirection;
};

const float PathPlannerNavMesh::NavPortal::MAX_COST = 10.0f;

PathPlannerNavMesh::NavPortal::NavPortal()
{
	mSrcSector = NULL;
	mDstSector = NULL;
	mFlags.ClearAll();
}

//////////////////////////////////////////////////////////////////////////

PathPlannerNavMesh::NavSector::NavSector()
	: mMirror( NavmeshIO::Sector_MirrorDir_MirrorNone )
{
}

bool PathPlannerNavMesh::NavSector::IsMirrored() const
{
	return mMirror != NavmeshIO::Sector_MirrorDir_MirrorNone;
}

//////////////////////////////////////////////////////////////////////////

bool PathPlannerNavMesh::Obstacle::Expired() const
{
	if ( mExpireTime >= 0 && IGame::GetTime() > mExpireTime )
		return true;
	return false;
}

PathPlannerNavMesh::Obstacle::Obstacle() 
	: mCost( -1.0f )
	, mExpireTime( -1 )
{
}

//////////////////////////////////////////////////////////////////////////

PathPlannerNavMesh::RuntimeNavSector::RuntimeNavSector( obuint32 index, NavSector * sector, NavmeshIO::SectorData * data )
	: mIndex( index )
	, mParent( NULL )
	, mSector( sector )
	, mSectorData( data )
	, mCost( 1.0f )
	, mVersion( 0 )
	, mMirrored( false )
	, mUpdatePortals( true )
	, mUpdateObstacles( false )
{
	mSectorWorldTrans = Vector3f::ZERO;
	mSectorWorldRot.MakeIdentity();
}

PathPlannerNavMesh::RuntimeNavSector::~RuntimeNavSector()
{
}

bool PathPlannerNavMesh::RuntimeNavSector::IsMirroredSector() const
{
	return mSectorData == &mSector->mSectorDataMirrored;
}

bool PathPlannerNavMesh::RuntimeNavSector::InitSectorTransform()
{
	const Vector3f center = CalculateCenter();

	mSectorData->clear_mover();

	GameEntity mover = InterfaceFuncs::GetMoverAt( center );
	if ( mover.IsValid() )
	{
		bool good = true;

		Vector3f moverFwd, moverRight, moverUp;
		good &= EngineFuncs::EntityPosition(
			mover,
			mSectorWorldTrans );
		good &= EngineFuncs::EntityOrientation(
			mover,
			moverFwd,
			moverRight,
			moverUp );

		if ( good )
		{
			mSectorWorldRot = Matrix3f( moverRight, moverFwd, moverUp, true );

			mSectorData->mutable_mover()->set_id( mover.AsInt() );

			mLocalPoly.resize( mPoly.size() );
			mSectorData->clear_localoffsets();
			for ( size_t i = 0; i < mPoly.size(); ++i )
			{
				mLocalPoly[ i ] = (mPoly[ i ] - mSectorWorldTrans) * mSectorWorldRot.Transpose();

				NavmeshIO::SectorVert * vert = mSectorData->add_localoffsets();
				vert->mutable_position()->set_x( mLocalPoly[ i ].X() );
				vert->mutable_position()->set_y( mLocalPoly[ i ].Y() );
				vert->mutable_position()->set_z( mLocalPoly[ i ].Z() );
			}
			
			UpdateSectorTransform();
			return true;
		}
	}
	return false;
}

void PathPlannerNavMesh::RuntimeNavSector::UpdateSectorTransform()
{
	if ( mSectorData->onmover() )
	{
		GameEntity mover;
		mover.FromInt( mSectorData->mover().id() );

		Vector3f moverFwd, moverRight, moverUp;
		if ( mover.IsValid() &&
			EngineFuncs::EntityPosition( mover, mSectorWorldTrans ) &&
			EngineFuncs::EntityOrientation(
			mover,
			moverFwd,
			moverRight,
			moverUp ) )
		{
			mSectorWorldRot = Matrix3f( moverRight, moverFwd, moverUp, true );

			for ( size_t i = 0; i < mPoly.size(); ++i )
				mPoly[ i ] = mSectorWorldTrans + mLocalPoly[ i ] * mSectorWorldRot;
			
			for ( size_t i = 0; i < mPortals.size(); ++i )
			{
				mPortals[ i ].mSegment.P0 = mSectorWorldTrans + mPortals[ i ].mSegmentLocal.P0 * mSectorWorldRot;
				mPortals[ i ].mSegment.P1 = mSectorWorldTrans + mPortals[ i ].mSegmentLocal.P1 * mSectorWorldRot;
				mPortals[ i ].mSegment.ComputeCenterDirectionExtent();
			}
		}
		else
		{
			const Vector3f center = CalculateCenter();
			RenderBuffer::AddString3d(
				center + Vector3f(0.f,0.f,32.f),
				COLOR::RED,
				"Invalid Mover Entity" );
		}
	}
	else
	{
		Vector3List empty;
		mLocalPoly.swap( empty );
	}
}

void PathPlannerNavMesh::RuntimeNavSector::UpdateAutoFlags()
{
	const Vector3f center = CalculateCenter();

	// reset the fields we're going to auto detect
	mSectorData->clear_inwater();
	mSectorData->clear_waterdepth();

	const int contents = g_EngineFuncs->GetPointContents( center );

	// set the flags
	const NavmeshIO::SectorData & def = NavmeshIO::SectorData::default_instance();

	const bool inwater = (contents & CONT_WATER)!=0;

	if ( def.inwater() != inwater )
		mSectorData->set_inwater( inwater );

	if ( mSectorData->inwater() )
	{
		// find the water depth over the sector
		obTraceResult tr;
		EngineFuncs::TraceLine(tr,
			center + Vector3f( 0.f, 0.f, 1.f ),
			center + Vector3f( 0.f, 0.f, 1024.f ),
			NULL,TR_MASK_FLOODFILL,-1,False);

		EngineFuncs::TraceLine(tr,
			Vector3f( tr.m_Endpos ) + Vector3f( 0.f, 0.f, -1.f),
			Vector3f( tr.m_Endpos ) + Vector3f( 0.f, 0.f, -1024.f),
			NULL,TR_MASK_WATER,-1,False);

		if ( tr.m_Fraction < 1.0f )
		{
			mSectorData->set_waterdepth( tr.m_Endpos[2] - center.Z() );
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void PathPlannerNavMesh::RuntimeNavSector::RenderPortals( const PathPlannerNavMesh * navmesh ) const
{
	const Vector3f sectorCenter = CalculateCenter();

	for ( size_t p = 0; p < mPortals.size(); ++p )
	{
		const NavPortal &portal = mPortals[ p ];

		const Vector3f fwd = ( portal.mSegment.P1 - portal.mSegment.P0 ).UnitCross( Vector3f::UNIT_Z );

		RenderBuffer::AddLine(
			portal.mSegment.P0 + Vector3f(0.0f,0.0f,2.0f),
			portal.mSegment.P1 + Vector3f(0.0f,0.0f,2.0f),
			COLOR::MAGENTA );

		const RuntimeNavSector * destSector = portal.mDstSector;

		obColor portalColor = COLOR::BLUE;
		if ( portal.mFlags.CheckFlag( NavPortal::FL_DISABLED ) )
			portalColor = COLOR::RED;
		else if ( portal.mFlags.CheckFlag( NavPortal::FL_JUMP ) )
			portalColor = COLOR::CYAN;

		Vector3f vP0 = portal.mSegment.Center;
		Vector3f vP1 = vP0;

		RenderBuffer::AddArrow( sectorCenter, vP0, portalColor );

		Utils::PushPointToPlane( vP1, destSector->mPlane, -Vector3f::UNIT_Z );

		RenderBuffer::AddLine( vP0, vP1, portalColor );
		/*vP0 = vP1;
		vP1 = destSector->CalculateCenter();
		RenderBuffer::AddArrow( vP0, vP1, portalColor );*/
	}
}

//////////////////////////////////////////////////////////////////////////

void PathPlannerNavMesh::RuntimeNavSector::RebuildPortals( PathPlannerNavMesh * navmesh )
{
	mUpdatePortals = false;
	
	mPortals.resize( 0 );

	SegmentList edges0, edges1;
	GetEdgeSegments( edges0 );
	
	const float MAX_STEPUP = std::max( JUMP_HEIGHT, mSectorData->waterdepth() + WATER_EXIT );

	for ( size_t e = 0; e < edges0.size(); ++e )
	{
		if ( mSectorData->edgeportalmask() & (1<<e) )
			continue;

		const Segment3f & seg0 = edges0[ e ];

		Line3f line0;
		line0.FromPoints( seg0.P0, seg0.P1 );

		// the box height is adjusted for proximity with both climb/jumpable sectors
		// as well as sectors that may be jumped down onto
		Box3f obb;
		obb.Center = seg0.Center;
		obb.Center.Z() += (MAX_STEPUP - DROP_HEIGHT) * 0.5f;
		obb.Axis[ 0 ] = seg0.Direction.Cross( Vector3f::UNIT_Z );
		obb.Axis[ 1 ] = seg0.Direction;
		obb.Axis[ 2 ] = obb.Axis[ 0 ].Cross( obb.Axis[ 1 ] );
		obb.Extent[ 0 ] = 8;
		obb.Extent[ 1 ] = seg0.Extent;
		obb.Extent[ 2 ] = (MAX_STEPUP - -DROP_HEIGHT) * 0.5f;

		//RenderBuffer::AddOBB( obb, COLOR::ORANGE );

		RuntimeSectorRefs nearbySectors;
		navmesh->FindRuntimeSectors( obb, nearbySectors, false );

		// search all sectors for neighboring connections
		for ( size_t n = 0; n < nearbySectors.size(); ++n )
		{
			RuntimeNavSector * neighbor = nearbySectors[ n ];

			if ( this == neighbor || neighbor->mCost >= NavPortal::MAX_COST )
				continue;
			
			// don't connect to it if it has sub sectors, which means
			// it is subdivided as well
			if ( neighbor->mSubSectors.size() > 0 )
				continue;
			
			neighbor->GetEdgeSegments( edges1 );
			for ( size_t nb = 0; nb < edges1.size(); ++nb )
			{
				const Segment3f & seg1 = edges1[ nb ];

				DistPoint3Line3f d0( seg1.P0, line0 );
				DistPoint3Line3f d1( seg1.P1, line0 );

				d0.GetSquared();
				d1.GetSquared();

				/*RenderBuffer::AddLine(
				d0.GetClosestPoint1() + Vector3f( 0.0f, 0.0f, 2.0f ),
				d0.GetClosestPoint0() + Vector3f( 0.0f, 0.0f, 2.0f ),
				COLOR::MAGENTA );

				RenderBuffer::AddLine(
				d1.GetClosestPoint1() + Vector3f( 0.0f, 0.0f, 2.0f ),
				d1.GetClosestPoint0() + Vector3f( 0.0f, 0.0f, 2.0f ),
				COLOR::RED );*/

				// the lines must overlap
				/*if ( d0.GetLineParameter() > seg0.Extent && d1.GetLineParameter() > seg0.Extent )
				continue;
				if ( d0.GetLineParameter() < -seg0.Extent && d1.GetLineParameter() < -seg0.Extent )
				continue;*/

				// pull the points into the segment space
				const float d0Segment = Mathf::Clamp( d0.GetLineParameter(), -seg0.Extent, seg0.Extent );
				const float d1Segment = Mathf::Clamp( d1.GetLineParameter(), -seg0.Extent, seg0.Extent );

				if ( Mathf::FAbs( d0Segment - d1Segment ) < 8.0 )
					continue;

				const Vector3f p0diff = d0.GetClosestPoint1() - d0.GetClosestPoint0();
				const Vector3f p1diff = d1.GetClosestPoint1() - d1.GetClosestPoint0();

				const float p0zDiff = seg1.P0.Z() - d0.GetClosestPoint1().Z();
				const float p1zDiff = seg1.P1.Z() - d0.GetClosestPoint1().Z();

				// are these lines are close enough
				if ( p0diff.Length2d() < LINE_DIST && p1diff.Length2d() < LINE_DIST &&
					p0zDiff <= MAX_STEPUP && p1zDiff <= MAX_STEPUP &&
					p0zDiff >= -DROP_HEIGHT && p1zDiff >= -DROP_HEIGHT )
				{
					NavPortal portal;
					portal.mSegment.P0 = line0.Origin + d0Segment * line0.Direction;
					portal.mSegment.P1 = line0.Origin + d1Segment * line0.Direction;
					portal.mSegment.ComputeCenterDirectionExtent();
					portal.mSrcSector = this;
					portal.mDstSector = neighbor;

					portal.mSegmentLocal = portal.mSegment;
					portal.mSegmentLocal.P0 = (portal.mSegment.P0 - mSectorWorldTrans) * mSectorWorldRot.Transpose();
					portal.mSegmentLocal.P1 = (portal.mSegment.P1 - mSectorWorldTrans) * mSectorWorldRot.Transpose();

					// do we need to jump to reach this edge?
					if ( p0zDiff > STEP_HEIGHT && p0zDiff <= JUMP_HEIGHT )
					{
						portal.mFlags.SetFlag( NavPortal::FL_JUMP );
					}
					mPortals.push_back( portal );
				}
			}
		}
	}

	if ( mSectorData->ledge() )
	{
		RebuildLedgePortals( navmesh );
	}
}

//////////////////////////////////////////////////////////////////////////

void PathPlannerNavMesh::RuntimeNavSector::RebuildLedgePortals( PathPlannerNavMesh * navmesh )
{
}

//////////////////////////////////////////////////////////////////////////

static bool CostLessThan( const PathPlannerNavMesh::Obstacle & obs1, const PathPlannerNavMesh::Obstacle & obs2 )
{
	return obs1.mCost < obs2.mCost;
}

//////////////////////////////////////////////////////////////////////////

static void GenerateFromClipResults( const ClipperLib::Clipper & clip,
									PathPlannerNavMesh::RuntimeNavSector & parent,
									const float cost,
									PathPlannerNavMesh::RuntimeSectorList & sectorsOut )
{
	typedef std::list<TPPLPoly> PolyList;
	PolyList inputPolys;
	PolyList convexPolys;

	// add all the solution polygons to the input polygons
	// of the partition list
	ClipperLib::Polygon poly;
	for ( size_t i = 0; i < clip.GetNumPolysOut(); ++i )
	{
		bool polyIsHole = false;
		if ( clip.GetPolyOut( i, poly, polyIsHole ) )
		{
			inputPolys.push_back( TPPLPoly() );

			ConvertPoly( inputPolys.back(), poly );
			inputPolys.back().SetHole( polyIsHole );
			inputPolys.back().SetOrientation( polyIsHole ? TPPL_CW : TPPL_CCW );
		}
	}

	TPPLPartition partition;
	if ( partition.ConvexPartition_HM( &inputPolys, &convexPolys ) != 0 )
	{
		// translate it back to sector space
		for ( PolyList::iterator it = convexPolys.begin(); it != convexPolys.end(); ++it )
		{
			const TPPLPoly & ply = *it;
			PathPlannerNavMesh::RuntimeNavSector subSector( parent.mIndex, parent.mSector, parent.mSectorData );
			subSector.mCost = cost;
			subSector.mPlane = parent.mPlane;
			subSector.mParent = &parent;
			ConvertPoly( subSector.mPoly, subSector, ply );
			sectorsOut.push_back( subSector );
		}
	}
	else
	{
		EngineFuncs::ConsoleError( va( "Sector %d error building subsectors", parent.mIndex ) );
	}
}

//////////////////////////////////////////////////////////////////////////

void PathPlannerNavMesh::RuntimeNavSector::RebuildObstacles( PathPlannerNavMesh * navmesh )
{
	mUpdateObstacles = false;

	// build a new list and swap it at the end
	// (calling resize requires an accessible default ctr)
	RuntimeSectorList newSectors;

	if ( mObstacles.size() > 0 )
	{
		// sort the obstacles from least to highest cost values for processing
		std::sort( mObstacles.begin(), mObstacles.end(), CostLessThan );
		
		// the first operation is removing ALL obstacle footprints from the sector
		ClipperLib::Polygons subjectPolys;
		subjectPolys.push_back( ClipperLib::Polygon() );
		ConvertPoly( subjectPolys.back(), mPoly );
		assert ( ClipperLib::Orientation( subjectPolys.back() ) );

		ClipperLib::Polygons obstaclePolys;
		for ( size_t i = 0; i < mObstacles.size(); ++i )
		{
			obstaclePolys.push_back( ClipperLib::Polygon() );
			ConvertPoly( obstaclePolys.back(), mObstacles[ i ].mPoly );
		}

		ClipperLib::Clipper clip;
		clip.AddPolygons( subjectPolys, ClipperLib::ptSubject );
		clip.AddPolygons( obstaclePolys.begin(), obstaclePolys.end(), ClipperLib::ptClip );
		
		// first we get the sectors MINUS all the obstacles
		if ( clip.Execute( ClipperLib::ctDifference, ClipperLib::pftNonZero, ClipperLib::pftNonZero ) )
			GenerateFromClipResults( clip, *this, 1.0f, newSectors );

		// generate the full obstacle set as unified subsectors, or as discrete obstacle cost sets
		static const bool GENERATE_SIMPLE_OBSTACLESET = false;
		if ( GENERATE_SIMPLE_OBSTACLESET )
		{
			if ( clip.Execute( ClipperLib::ctIntersection, ClipperLib::pftNonZero, ClipperLib::pftNonZero ) )
				GenerateFromClipResults( clip, *this, FLT_MAX, newSectors );
		}
		else
		{
			// for each set of obstacles, we need to find the union of the sector polygons 
			// and the obstacle set MINUS, the set of greater costs

			size_t o = 0;
			while( o < mObstacles.size() )
			{
				clip.Clear();
				clip.AddPolygons( subjectPolys, ClipperLib::ptSubject );

				const float setCost = mObstacles[ o ].mCost;

				while ( o < mObstacles.size() )
				{
					if ( setCost == mObstacles[ o ].mCost )
						clip.AddPolygon( *(obstaclePolys.begin() + o), ClipperLib::ptClip );
					else
						break;

					++o;
				}
				
				if ( clip.Execute( ClipperLib::ctIntersection, ClipperLib::pftNonZero, ClipperLib::pftNonZero ) )
				{
					// if there are remaining obstacle sets, we need to cut them from these results
					// if this was the last set, we can add them as-is
					if ( o >= mObstacles.size() )
					{
						GenerateFromClipResults( clip, *this, setCost, newSectors );
					}
					else
					{
						ClipperLib::Polygons obstacleSetPolys;
						obstacleSetPolys.resize( clip.GetNumPolysOut() );
						for ( size_t i = 0; i < clip.GetNumPolysOut(); ++i )
						{				
							bool isHole;
							clip.GetPolyOut( i, obstacleSetPolys[ i ], isHole );
						}

						clip.Clear();
						clip.AddPolygons( obstacleSetPolys, ClipperLib::ptSubject );
						clip.AddPolygons( obstaclePolys.begin() + o, obstaclePolys.end(), ClipperLib::ptClip );

						if ( clip.Execute( ClipperLib::ctDifference, ClipperLib::pftNonZero, ClipperLib::pftNonZero ) )
							GenerateFromClipResults( clip, *this, setCost, newSectors );
					}
				}
			}
		}
	}

	mSubSectors.clear();
	mSubSectors.swap( newSectors );

	OB_DELETE( navmesh->mRuntimeSectorCollision[ mIndex ] );

	// if it has a subsector, or it's a dynamic sector
	// we should create a collision model for it
	if ( mSubSectors.size() > 0 )
	{
		navmesh->mRuntimeSectorCollision[ mIndex ] = new CollisionModel();

		for ( size_t i = 0; i < mSubSectors.size(); ++i )
		{
			RuntimeNavSector & ns = mSubSectors[ i ];
			navmesh->mRuntimeSectorCollision[ mIndex ]->Add( ns.mPoly, &ns );
		}

		navmesh->mRuntimeSectorCollision[ mIndex ]->Build( false );
	}
	else if ( mSectorData->onmover() )
	{
		navmesh->mRuntimeSectorCollision[ mIndex ] = new CollisionModel();
		navmesh->mRuntimeSectorCollision[ mIndex ]->Add( mPoly, this );
		navmesh->mRuntimeSectorCollision[ mIndex ]->Build( false );
	}

	// sector changed, update version to force portal rebuilding of any that reference this sector
	++mVersion;
}

//////////////////////////////////////////////////////////////////////////

PathPlannerNavMesh::PathPlannerNavMesh()
{
	// TEMP
	m_PlannerFlags.SetFlag(NAV_VIEW);
	m_PlannerFlags.SetFlag(NAV_VIEWCONNECTIONS);
	m_PlannerFlags.SetFlag(NAVMESH_STEPPROCESS);
	m_PlannerFlags.SetFlag(NAV_VIEWFLAGS);
	//m_PlannerFlags.SetFlag(NAVMESH_SHOWCOLLISION); // temp

	m_CursorColor = COLOR::BLUE;

	m_MapCenter = Vector3f::ZERO;

	g_PlannerNavMesh = this;

	mCurrentTool = NULL;
}

PathPlannerNavMesh::~PathPlannerNavMesh()
{
	Shutdown();
	g_PlannerNavMesh = 0;
}

Vector3f PathPlannerNavMesh::NavSectorBase::CalculateCenter() const
{
	Vector3f center = Vector3f::ZERO;
	if ( mPoly.size() > 0 )
	{
		center = mPoly[ 0 ];
		for ( size_t i = 1; i < mPoly.size(); ++i )
			center += mPoly[ i ];
		center /= (float)mPoly.size();
	}
	return center;
}

void PathPlannerNavMesh::NavSectorBase::GetMirroredCopy(
	const Vector3f & mirrorAxis,
	NavmeshIO::Sector_MirrorDir mirror,
	NavSectorBase & dest) const
{
	if ( mirror == NavmeshIO::Sector_MirrorDir_ReflectX ||
		mirror == NavmeshIO::Sector_MirrorDir_ReflectY ||
		mirror == NavmeshIO::Sector_MirrorDir_ReflectZ )
	{
		const int ax = mirror - NavmeshIO::Sector_MirrorDir_ReflectX;
		for(obuint32 m = 0; m < dest.mPoly.size(); ++m)
		{
			dest.mPoly[m] -= mPoly[m];
			dest.mPoly[m][ ax ] = mirrorAxis[ ax ] + ( mirrorAxis[ ax ] - mPoly[m][ax] );
		}
		return;
	}

	Vector3f vAxis;
	switch( mirror )
	{
	case NavmeshIO::Sector_MirrorDir_MirrorX:
		vAxis = Vector3f::UNIT_X;
		break;
	case NavmeshIO::Sector_MirrorDir_MirrorNX:
		vAxis = -Vector3f::UNIT_X;
		break;
	case NavmeshIO::Sector_MirrorDir_MirrorY:
		vAxis = Vector3f::UNIT_Y;
		break;
	case NavmeshIO::Sector_MirrorDir_MirrorNY:
		vAxis = -Vector3f::UNIT_Y;
		break;
	case NavmeshIO::Sector_MirrorDir_MirrorZ:
		vAxis = Vector3f::UNIT_Z;
		break;
	case NavmeshIO::Sector_MirrorDir_MirrorNZ:
		vAxis = -Vector3f::UNIT_Z;
		break;
	case NavmeshIO::Sector_MirrorDir_MirrorNone:
		break;
	case NavmeshIO::Sector_MirrorDir_ReflectX:
	case NavmeshIO::Sector_MirrorDir_ReflectY:
	case NavmeshIO::Sector_MirrorDir_ReflectZ:
		return;
	}

	Matrix3f mat(vAxis, Mathf::DegToRad(180.0f));
	dest.mPoly = mPoly;
	for(obuint32 m = 0; m < dest.mPoly.size(); ++m)
	{
		dest.mPoly[m] -= mirrorAxis;
		dest.mPoly[m] = mat * dest.mPoly[m];
		dest.mPoly[m] += mirrorAxis;
	}
	dest.mPlane.Normal = mat * dest.mPlane.Normal;
}

void PathPlannerNavMesh::NavSectorBase::GetEdgeSegments(SegmentList &_list) const
{
	_list.reserve( mPoly.size() );
	_list.resize( 0 );
	for ( size_t i = 1; i <= mPoly.size(); ++i )
	{
		_list.push_back( Segment3f( mPoly[ i-1 ], mPoly[ i % mPoly.size() ] ) );
	}
}

void PathPlannerNavMesh::NavSectorBase::SetPointsToPlane()
{
	for ( size_t i = 0; i < mPoly.size(); ++i )
	{
		Utils::PushPointToPlane( mPoly[ i ], mPlane, -Vector3f::UNIT_Z );
	}
}

bool PathPlannerNavMesh::Init( System & system )
{
	InitCommands();
	return true;
}

void PathPlannerNavMesh::RegisterNavFlag(const std::string &_name, const NavFlags &_bits)
{
}

static void DrawPoly( bool solid, const PathPlannerNavMesh::RuntimeNavSector & ns, const TPPLPoly & poly, float offset, obColor color, float dur, float vertSize )
{
	Vector3List polyList;
	for ( int i = 0; i < poly.GetNumPoints(); ++i )
	{
		Vector3f vec3d( (float)poly[ i ].x, (float)poly[ i ].y, 0.0f );
		Utils::PushPointToPlane( vec3d, ns.mPlane, -Vector3f::UNIT_Z );

		vec3d.Z() += offset;
		polyList.push_back( vec3d );
	}

	if ( solid )
		RenderBuffer::AddPolygonFilled( polyList, color );
	else
		RenderBuffer::AddPolygonSilouette( polyList, color );

	if ( vertSize > 0.0 )
	{
		RenderBuffer::AddPoints( polyList, COLOR::MAGENTA, vertSize );
	}
}

static void DrawPoly( bool solid, const PathPlannerNavMesh::RuntimeNavSector & ns, const ClipperLib::Polygon & poly, float offset, obColor color, float dur, float vertSize )
{
	Vector3List polyList;
	for ( ClipperLib::Polygon::const_iterator it = poly.begin();
		it != poly.end();
		++it )
	{
		const ClipperLib::IntPoint & pt = *it;
		Vector3f vec3d( (float)pt.X, (float)pt.Y, 0.0f );

		Utils::PushPointToPlane( vec3d, ns.mPlane, -Vector3f::UNIT_Z );

		vec3d.Z() += offset;
		polyList.push_back( vec3d );
	}

	if ( solid )
		RenderBuffer::AddPolygonFilled( polyList, color );
	else
		RenderBuffer::AddPolygonSilouette( polyList, color );

	if ( vertSize > 0.0 )
	{
		RenderBuffer::AddPoints( polyList, COLOR::MAGENTA, vertSize );
	}
}

obColor PathPlannerNavMesh::RuntimeNavSector::GetRenderColor() const
{
	obColor SEC_COLOR = mMirrored ? COLOR::CYAN.fade(100) : COLOR::GREEN.fade(100);

	if ( mCost != 1.0 )
	{
		SEC_COLOR = obColor::lerp( COLOR::WHITE, COLOR::RED, Mathf::Clamp( mCost / NavPortal::MAX_COST, 0.0f, 1.0f ) ).fade( 180 );
	}

	return SEC_COLOR;
}

void PathPlannerNavMesh::Update( System & system )
{
	Prof(PathPlannerNavMesh);

	if ( mCurrentTool )
	{
		if ( !mCurrentTool->Update( this ) )
			OB_DELETE( mCurrentTool );
	}

	UpdateObstacles();
	UpdateRuntimeSectors();

	UpdateMoverSectors();

	if(m_PlannerFlags.CheckFlag(NAV_VIEW))
	{
		g_PathFind.Render();

		//////////////////////////////////////////////////////////////////////////
		Vector3f eyePos, eyeNorm;
		if(Utils::GetLocalAimPoint(eyePos, &eyeNorm, TR_MASK_FLOODFILLENT))
		{
			RenderBuffer::AddLine(eyePos,eyePos + eyeNorm * 16.f,m_CursorColor);
			RenderBuffer::AddLine(eyePos,eyePos + eyePos.Perpendicular() * 16.f, m_CursorColor);
		}
		m_CursorColor = COLOR::BLUE; // back to default

		//////////////////////////////////////////////////////////////////////////
		// Draw our nav sectors
		for ( size_t i = 0; i < mRuntimeSectors.size(); ++i )
		{
			const RuntimeNavSector &ns = mRuntimeSectors[i];

			const Vector3f nsCenter = ns.CalculateCenter();

			// mirrored sectors are contiguous at the end of
			// the mRuntimeSectors list, so draw them differently
			if ( ns.mSubSectors.size() > 0 )
			{
				for ( size_t s = 0; s < ns.mSubSectors.size(); ++s )
				{
					const RuntimeNavSector & subSector = ns.mSubSectors[ s ];
										
					RenderBuffer::AddPolygonFilled( subSector.mPoly, subSector.GetRenderColor() );

					// render edge polys based on edge mask
					for ( size_t p = 0; p < subSector.mPoly.size(); ++p )
					{
						const Vector3f v0 = subSector.mPoly[ p ];
						const Vector3f v1 = subSector.mPoly[ (p+1) % subSector.mPoly.size() ];

						if ( ns.mSectorData->edgeportalmask() & (1<<p) )
						{
							RenderBuffer::AddTri( v0, v1, ns.CalculateCenter(), COLOR::RED.fade(100) );
							continue;
						}

						RenderBuffer::AddLine( v0, v1, COLOR::GREEN );
					}
				}
			}
			else
			{
				RenderBuffer::AddPolygonFilled( ns.mPoly, ns.GetRenderColor() );

				// render edge polys based on edge mask
				for ( size_t p = 0; p < ns.mPoly.size(); ++p )
				{
					const Vector3f v0 = ns.mPoly[ p ];
					const Vector3f v1 = ns.mPoly[ (p+1) % ns.mPoly.size() ];

					if ( ns.mSectorData->edgeportalmask() & (1<<p) )
					{
						RenderBuffer::AddTri( v0, v1, ns.CalculateCenter(), COLOR::RED.fade(100) );
					}

					RenderBuffer::AddLine( v0, v1, COLOR::GREEN );
				}
			}

			// render the points, colored by whether this sector is mirrored
			const obColor ptColor = ( ns.mSector->mMirror != NavmeshIO::Sector_MirrorDir_MirrorNone ) ?
				COLOR::CYAN : COLOR::MAGENTA;
			RenderBuffer::AddPoints( ns.mPoly, ptColor, 5.0f );

			if ( m_PlannerFlags.CheckFlag( NAV_VIEWCONNECTIONS ) )
			{
				// if the sector has a subsector our top level portals are irrelevant
				if ( ns.mSubSectors.size() == 0 )
					ns.RenderPortals( this );
				else
				{
					for ( size_t s = 0; s < ns.mSubSectors.size(); ++s )
						ns.mSubSectors[ s ].RenderPortals( this );
				}
			}

			if ( m_PlannerFlags.CheckFlag( NAV_VIEWFLAGS ) )
			{
				using namespace google;

				RuntimeSectorRefsConst sectorRefs;
				if ( ns.mSubSectors.size() > 0 )
				{
					for ( size_t s = 0; s < ns.mSubSectors.size(); ++s )
						sectorRefs.push_back( &ns.mSubSectors[ s ] );
				}
				else
				{
					sectorRefs.push_back( &ns );
				}

				for ( size_t s = 0; s < sectorRefs.size(); ++s )
				{
					const RuntimeNavSector * sec = sectorRefs[ s ];

					const protobuf::Reflection * refl = sec->mSectorData->GetReflection();

					// fields for mirrored
					NavmeshIO::SectorData & data = *sec->mSectorData;
					NavmeshIO::SectorData & otherData = sec->IsMirroredSector() ? sec->mSector->mSectorData : sec->mSector->mSectorDataMirrored;

					std::vector<const protobuf::FieldDescriptor*> fields0;
					std::vector<const protobuf::FieldDescriptor*> fields1;

					refl->ListFields( data, &fields0 );
					refl->ListFields( otherData, &fields1 );

					StringVector strs;

					// what fields do the sectors share
					while( fields0.size() > 0 )
					{
						const protobuf::FieldDescriptor * field = fields0.back();

						if ( !field->is_repeated() && !field->options().GetExtension( NavmeshIO::hidden ) )
						{
							std::string s = va( "%s - %s",
								field->lowercase_name().c_str(),
								GetFieldString( data, field ).c_str() );

							if ( refl->HasField( otherData, field ) )
								s += " <+m> ";

							strs.push_back( s );
						}

						fields0.erase( std::remove( fields0.begin(), fields0.end(), field ), fields0.end() );
						fields1.erase( std::remove( fields1.begin(), fields1.end(), field ), fields1.end() );
					}

					// what's in the mirrored and not this one
					while( fields1.size() > 0 )
					{
						const protobuf::FieldDescriptor * field = fields1.front();

						if ( !field->is_repeated() && !field->options().GetExtension( NavmeshIO::hidden ) )
						{
							std::string s = va( "%s - %s",
								field->lowercase_name().c_str(),
								GetFieldString( otherData, field ).c_str() );
							s += " <m> ";

							strs.push_back( s );
						}

						fields0.erase( std::remove( fields0.begin(), fields0.end(), field ), fields0.end() );
						fields1.erase( std::remove( fields1.begin(), fields1.end(), field ), fields1.end() );
					}

					std::sort( strs.begin(), strs.end(), std::less<std::string>() );

					std::string strData;
					for ( size_t s = 0; s < strs.size(); ++s )
					{
						strData += strs[ s ];
						strData += "\n";
					}

					RenderBuffer::AddString3dRadius(
						sec->CalculateCenter() + Vector3f(0.f,0.f,32.f),
						COLOR::BLUE, 1024.f, strData.c_str() );
				}
			}
		}

		if ( m_PlannerFlags.CheckFlag( NAVMESH_SHOWCOLLISION ) )
		{
			CollisionModel::AttribSet skipSet;
			for ( size_t i = 0; i < mRuntimeSectorCollision.size(); ++i )
			{
				if ( mRuntimeSectorCollision[ i ] )
				{
					skipSet.insert( &mRuntimeSectors[ i ] );

					mRuntimeSectorCollision[ i ]->Render( Vector3f( 0.0f, 0.0f, 16.0f ) );
				}
			}

			mSectorCollision.RenderSkipSet( skipSet, Vector3f( 0.0f, 0.0f, 16.0f ) );
		}

		//////////////////////////////////////////////////////////////////////////

		if(!mEditSector.mPoly.empty())
		{
			RenderBuffer::AddLine(
				mEditSectorStart,
				mEditSectorStart + mEditSectorPlane.Normal * 10.0f,
				COLOR::MAGENTA );

			RenderBuffer::AddPolygonFilled( mEditSector.mPoly, COLOR::BLUE.fade(100) );
			if ( mEditSector.IsMirrored() )
			{
				NavSectorBase temp;
				mEditSector.GetMirroredCopy(m_MapCenter, mEditSector.mMirror, temp);
				RenderBuffer::AddPolygonFilled( temp.mPoly, COLOR::CYAN.fade(100) );
			}
		}
	}
}

void PathPlannerNavMesh::Shutdown()
{
	Unload();
}

bool PathPlannerNavMesh::Load(const std::string &_mapname, bool _dl)
{
	if(_mapname.empty())
		return false;

	Unload();

	try
	{
		using namespace google::protobuf;

		const std::string navPathText	= std::string("nav/") + _mapname + ".nm";
		const std::string navPathBinary	= std::string("nav/") + _mapname + ".nmb";

		NavmeshIO::NavigationMesh nm;

		std::string fileContents;

		/*const obint64 modBin = FileSystem::FileModifiedTime( filePath( "%s", navPathBinary.c_str() ) );
		const obint64 modTxt = FileSystem::FileModifiedTime( filePath( "%s", navPathText.c_str() ) );*/

		File navFile;
		if ( navFile.OpenForRead( navPathBinary.c_str(), File::Binary) )
		{
			navFile.ReadWholeFile( fileContents );

			if ( !nm.ParseFromString( fileContents ) )
				return false;
		}
		else if( navFile.OpenForRead( navPathText.c_str(), File::Text ) )
		{
			navFile.ReadWholeFile( fileContents );

			if ( !TextFormat::ParseFromString( fileContents, &nm ) )
				return false;
		} else
		{
			return false;
		}

		m_MapCenter.X() = nm.header().mapcenter().x();
		m_MapCenter.Y() = nm.header().mapcenter().y();
		m_MapCenter.Z() = nm.header().mapcenter().z();

		mNavSectors.clear();
		mNavSectors.reserve( nm.sectors_size() );

		for ( int i = 0; i < nm.sectors_size(); ++i )
		{
			const NavmeshIO::Sector & s = nm.sectors( i );

			NavSector sector;
			sector.mMirror = s.sectormirrored();
			sector.mSectorData = s.sectordata();
			sector.mSectorDataMirrored = s.sectordatamirrored();

			ClearDefaultedValues( sector.mSectorData );
			ClearDefaultedValues( sector.mSectorDataMirrored );

			for ( int i = 0; i < s.vertices_size(); ++i )
			{
				const NavmeshIO::SectorVert & v = s.vertices( i );
				sector.mPoly.push_back( Vector3f(
					v.position().x(),
					v.position().y(),
					v.position().z() ) );
			}

			Utils::WeldVertices( sector.mPoly, 1.0f );

			if ( sector.mPoly.size() <= 3 )
				continue;

			// calculate normal
			const Vector3f center = sector.CalculateCenter();
			sector.mPlane = Plane3f( center, sector.mPoly[ 0 ], sector.mPoly[ 1 ] );

			mNavSectors.push_back( sector );
		}
	}
	catch (std::exception* e)
	{
		LOG("PathPlannerNavMesh::Load ERROR: " << e->what() );
		return false;
	}

	InitSectors();

	return true;
}

bool PathPlannerNavMesh::Save(const std::string &_mapname)
{
	if(_mapname.empty())
		return false;

	NavmeshIO::NavigationMesh nm;
	nm.mutable_header(); // header is required

	if ( !m_MapCenter.IsZero() )
	{
		nm.mutable_header()->mutable_mapcenter()->set_x( m_MapCenter.X() );
		nm.mutable_header()->mutable_mapcenter()->set_y( m_MapCenter.Y() );
		nm.mutable_header()->mutable_mapcenter()->set_z( m_MapCenter.Z() );
	}

	for(obuint32 s = 0; s < mNavSectors.size(); ++s)
	{
		const NavSector &ns = mNavSectors[s];

		NavmeshIO::Sector* sectorOut = nm.add_sectors();

		if ( ns.IsMirrored() )
			sectorOut->set_sectormirrored( ns.mMirror );
		else
			sectorOut->clear_sectormirrored();

		*sectorOut->mutable_sectordata() = ns.mSectorData;
		*sectorOut->mutable_sectordatamirrored() = ns.mSectorDataMirrored;

		for(obuint32 v = 0; v < ns.mPoly.size(); ++v)
		{
			NavmeshIO::SectorVert * sectorVertOut = sectorOut->add_vertices();
			sectorVertOut->mutable_position()->set_x( ns.mPoly[v].X() );
			sectorVertOut->mutable_position()->set_y( ns.mPoly[v].Y() );
			sectorVertOut->mutable_position()->set_z( ns.mPoly[v].Z() );
		}
	}

	std::string outBuffer;

	using namespace google::protobuf;
	io::StringOutputStream outputStream( &outBuffer );
	io::CodedOutputStream codedStream( &outputStream );

	std::string serializeBinary, serializeText;

	try
	{
		ClearDefaultedValues( nm );

		nm.SerializeToString( &serializeBinary );
		TextFormat::PrintToString( nm, &serializeText );

		const std::string navPathText	= std::string("nav/") + _mapname + ".nm";
		const std::string navPathBinary	= std::string("nav/") + _mapname + ".nmb";

		/*File outFileBinary;
		if ( outFileBinary.OpenForWrite(navPathBinary.c_str(), File::Binary) )
		{
		outFileBinary.Write( serializeBinary.c_str(), serializeBinary.length(), 1 );
		outFileBinary.Close();
		}*/

		File outFileText;
		if( outFileText.OpenForWrite(navPathText.c_str(), File::Text) )
		{
			outFileText.Write( serializeText.c_str(), serializeText.length(), 1 );
			outFileText.Close();
		}
	}
	catch (std::exception* e)
	{
		LOG("PathPlannerNavMesh::Save ERROR: " << e->what() );
	}

	return true;
}

bool PathPlannerNavMesh::IsReady() const
{
	return !mRuntimeSectors.empty();
}

bool PathPlannerNavMesh::GetNavFlagByName(const std::string &_flagname, NavFlags &_flag) const
{
	_flag = 0;
	return false;
}

Vector3f PathPlannerNavMesh::GetRandomDestination(Client *_client, const Vector3f &_start, const NavFlags _team)
{
	Vector3f dest = _start;

	if(!mRuntimeSectors.empty())
	{
		const RuntimeNavSector &randSector = mRuntimeSectors[rand()%mRuntimeSectors.size()];
		dest = Utils::AveragePoint(randSector.mPoly);
	}
	return dest;
}

//////////////////////////////////////////////////////////////////////////

int PathPlannerNavMesh::PlanPathToNearest(Client *_client, const Vector3f &_start, const Vector3List &_goals, const NavFlags &_team)
{
	DestinationVector dst;
	for(obuint32 i = 0; i < _goals.size(); ++i)
		dst.push_back(Destination(_goals[i],32.f));
	return PlanPathToNearest(_client,_start,dst,_team);
}

//struct PathNode
//{
//	const PathPlannerNavMesh::RuntimeNavSector *	mSector;
//	const PathPlannerNavMesh::NavPortal *			mPortal;
//
//	int												mParent;
//
//	PathNode() : mSector( NULL ), mPortal( NULL ), mParent( 0 ) {}
//};
//
//std::vector<float>			mCostG;
//std::vector<float>			mCostF;
//std::vector<PathNode*>		mSearch;
//Pool<PathNode*>				mFreeList;
//std::vector<PathNode*>		mOpen;
//
//static bool PathNodeCompare(const PathNode * n1, const PathNode * n2)
//{
//	return mCostF[ n1.mSector->mIndex ] > mCostF[ n2.mSector->mIndex ];
//}

int PathPlannerNavMesh::PlanPathToNearest(Client *_client, const Vector3f &_start, const DestinationVector &_goals, const NavFlags &_team)
{
	/*
	// prepare the query
	mFreeList.PreAlloc( mRuntimeSectors.size() );

	// return to the free list
	mFreeList.Free( mOpen );

	mCostG.resize( mRuntimeSectors.size() );
	mCostF.resize( mRuntimeSectors.size() );
	mOpen.resize( mRuntimeSectors.size() );
	mSearch.resize( mRuntimeSectors.size() );

	std::fill( mCostG.begin(), mCostG.end(), -1.0f );
	std::fill( mCostF.begin(), mCostF.end(), -1.0f );
	std::fill( mOpen.begin(), mOpen.end(), NULL );
	std::fill( mSearch.begin(), mSearch.end(), NULL );

	std::vector<RuntimeNavSector*> goalSectors;
	goalSectors.reserve( _goals.size() );

	RuntimeNavSector * startSector = GetSectorAt( _start );
	if ( startSector == NULL )
	return -1;

	for ( size_t i = 0; i < _goals.size(); ++i )
	{
	RuntimeNavSector * goalSector = GetSectorAt( _goals[ i ].m_Position );
	goalSectors.push_back( goalSector );
	}

	if ( goalSectors.size() == 0 )
	return -1;

	PathNode * startNode = mFreeList.Alloc();

	node->mSector = startSector;
	node->mPortal = NULL; // no portal at the starting point

	mOpen.push_back();
	std::push_heap( mOpen.begin(),  mOpen.end(), PathNodeCompare );

	mOpen[ startSector.mIndex ] = startSector;
	mCostG[ startSector.mIndex ] = 0.0f;
	mCostF[ startSector.mIndex ] = goalSectors.size() == 1 ? Length( _start, _goals[ 0 ].m_Position ) : 0.0f;

	// do the search
	while ( !mOpen.empty() )
	{
	PathNode * curNode = mOpen.front();
	std::pop_heap( mOpen.begin(), mOpen.end() );
	mOpen.pop_back();

	// is this a goal
	for ( size_t g = 0; g < goalSectors.size(); ++g )
	{
	if ( goalSectors[ g ] == curNode )
	{
	mSearch[ curNode->mSector->mIndex ]

	return g;
	}
	}

	// expand the node
	for ( size_t i = 0; i < curNode->mSector->mPortals.size(); ++i )
	{
	const PathPlannerNavMesh::NavPortal & prtl = curNode->mSector->mPortals[ i ];
	const float newG = mCostG[ curNode->mSector->mIndex ] +
	Length( curNode->mPortal->mSegment.Center, prtl.mSegment.Center );
	const float newF = newG + (goalSectors.size() == 1) ?
	Length( prtl.mSegment.Center, _goals[ 0 ].m_Position ) : 0.0f;
	}
	}

	return -1;*/

	g_PathFind.StartNewSearch();
	g_PathFind.AddStart(_start);

	for(obuint32 i = 0; i < _goals.size(); ++i)
		g_PathFind.AddGoal(_goals[i].m_Position);

	while(!g_PathFind.IsFinished())
		g_PathFind.Iterate();

	return g_PathFind.GetGoalIndex();
}

void PathPlannerNavMesh::PlanPathToGoal(Client *_client, const Vector3f &_start, const Vector3f &_goal, const NavFlags _team)
{
	DestinationVector dst;
	dst.push_back(Destination(_goal,32.f));
	PlanPathToNearest(_client,_start,dst,_team);
}

bool PathPlannerNavMesh::IsDone() const
{
	return g_PathFind.IsFinished();
}
bool PathPlannerNavMesh::FoundGoal() const
{
	return g_PathFind.FoundGoal();
}

void PathPlannerNavMesh::Unload()
{
	OB_DELETE( mCurrentTool );

	mSectorCollision.Free();
	mRuntimeSectors.clear();
	mNavSectors.clear();
}

void PathPlannerNavMesh::InitSectors()
{
	mSectorCollision.Free();

	mRuntimeSectors.clear();

	RuntimeSectorList	allSectors;

	// create the active sector list and the collision data at the same time
	// so we can create a proper mapping
	for ( size_t i = 0; i < mNavSectors.size(); ++i )
	{
		RuntimeNavSector runtimeSector( allSectors.size(), &mNavSectors[ i ], &mNavSectors[ i ].mSectorData );
		runtimeSector.mPoly = mNavSectors[ i ].mPoly;
		runtimeSector.mPlane = Utils::CalculatePlane( runtimeSector.mPoly );
		runtimeSector.mLocalPoly.resize( 0 );
		for ( int i = 0; i < runtimeSector.mSectorData->localoffsets_size(); ++i )
		{
			const NavmeshIO::SectorVert & v = runtimeSector.mSectorData->localoffsets( i );
			runtimeSector.mLocalPoly.push_back( Vector3f(
				v.position().x(),
				v.position().y(),
				v.position().z() ) );
		}

		runtimeSector.UpdateSectorTransform();
		runtimeSector.UpdateAutoFlags();

		allSectors.push_back( runtimeSector );
	}

	// do another pass for the mirrored set
	for ( size_t i = 0; i < mNavSectors.size(); ++i )
	{
		if ( mNavSectors[ i ].mMirror != NavmeshIO::Sector_MirrorDir_MirrorNone )
		{
			RuntimeNavSector runtimeSector( allSectors.size(), &mNavSectors[ i ], &mNavSectors[ i ].mSectorDataMirrored );
			runtimeSector.mMirrored = true;
			mNavSectors[ i ].GetMirroredCopy( m_MapCenter, mNavSectors[ i ].mMirror, runtimeSector );
			runtimeSector.mPlane = Utils::CalculatePlane( runtimeSector.mPoly );

			runtimeSector.mLocalPoly.resize( 0 );
			for ( int i = 0; i < runtimeSector.mSectorData->localoffsets_size(); ++i )
			{
				const NavmeshIO::SectorVert & v = runtimeSector.mSectorData->localoffsets( i );
				runtimeSector.mLocalPoly.push_back( Vector3f(
					v.position().x(),
					v.position().y(),
					v.position().z() ) );
			}

			runtimeSector.UpdateSectorTransform();
			runtimeSector.UpdateAutoFlags();

			allSectors.push_back( runtimeSector );
		}
	}

	// make the new set of sectors the active set
	mRuntimeSectors.swap( allSectors );

	// we don't want to include dynamic geometry in the static collision
	for ( size_t i = 0; i < mRuntimeSectors.size(); ++i )
	{
		RuntimeNavSector & ns = mRuntimeSectors[ i ];
		ns.mUpdateObstacles = true;
		ns.mUpdatePortals = true;

		// only add static sector collision for non movable sectors
		if ( !ns.mSectorData->onmover() )
			mSectorCollision.Add( ns.mPoly, &ns );
	}

	// allocate enough slots to hold a potential custom collision model
	// for each sector. custom models are created whenever a sector
	// is subdivided by obstacles and needs a temporary collision model
	for ( size_t i = 0; i < mRuntimeSectorCollision.size(); ++i )
		OB_DELETE( mRuntimeSectorCollision[ i ] );
	mRuntimeSectorCollision.resize( mRuntimeSectors.size() );

	if ( !mSectorCollision.Build( true ) )
		mSectorCollision.Free();
}

void PathPlannerNavMesh::RegisterGameGoals()
{
}

void PathPlannerNavMesh::GetPath(Path &_path)
{
	const float CHAR_HALF_HEIGHT = NavigationMeshOptions::CharacterHeight * 0.75f;

	PathFindNavMesh::NodeList &nl = g_PathFind.GetSolution();

	while(!nl.empty())
	{
		PathFindNavMesh::PlanNode *pn = nl.back();
		Vector3f vNodePos = pn->Position;

		NavFlags navFlags = (NavFlags)0;
		if ( pn->Portal )	
		{
			static const float pushDist = 32.0f;
			Vector3f portalNormal = pn->Portal->mSegment.Direction.Cross( Vector3f::UNIT_Z );
			vNodePos += portalNormal * pushDist;

			if ( pn->Portal->mFlags.CheckFlag( NavPortal::FL_JUMP ) )
				navFlags |= F_NAV_JUMPLOW;
		}
		
		_path.AddPt(vNodePos + Vector3f(0,0,CHAR_HALF_HEIGHT),32.f)
			.NavId( pn->Sector->mIndex )
			.Flags( navFlags )
			/*.OnPathThrough(m_Solution.back()->OnPathThrough())
			.OnPathThroughParam(m_Solution.back()->OnPathThroughParam())*/;

		nl.pop_back();
	}
}

//////////////////////////////////////////////////////////////////////////

bool PathPlannerNavMesh::GetNavInfo(const Vector3f &pos,obint32 &_id,std::string &_name)
{
	return false;
}

//////////////////////////////////////////////////////////////////////////

void PathPlannerNavMesh::AddEntityConnection( const Event_EntityConnection &_conn )
{
	/*struct Event_EntityConnection
	{
		GameEntity	m_EntitySrc;
		GameEntity	m_EntityDst;

		float		m_SrcPos[3];
		float		m_DstPos[3];

		BitFlag32	m_Team;
		float		m_Radius;
		float		m_Cost;

		Event_EntityConnection()
			: m_Radius(0.f)
			, m_Cost(0.f)
		{
			for ( int i = 0; i < 3; ++i )
				m_SrcPos[ i ] = m_DstPos[ i ] = 0.0f;
		}
	};*/

}

//////////////////////////////////////////////////////////////////////////

void PathPlannerNavMesh::RemoveEntityConnection( GameEntity _ent )
{
}

//////////////////////////////////////////////////////////////////////////

Vector3f PathPlannerNavMesh::GetDisplayPosition(const Vector3f &_pos)
{
	return _pos;
}

//////////////////////////////////////////////////////////////////////////

void PathPlannerNavMesh::UpdateMoverSectors()
{
	for ( size_t i = 0; i < mRuntimeSectors.size(); ++i )
	{
		mRuntimeSectors[ i ].UpdateSectorTransform();
	}
}

//////////////////////////////////////////////////////////////////////////

void PathPlannerNavMesh::UpdateRuntimeSectors()
{
	// accumulate and rebuild all the portals after all the
	// obstacles have been built, so all the areas are available
	// to find new sector edges
	RuntimeSectorRefs buildPortals;

	float minCost = std::numeric_limits<float>::max();
	float maxCost = std::numeric_limits<float>::lowest();

	for ( size_t i = 0; i < mRuntimeSectors.size(); ++i )
	{
		RuntimeNavSector & runtimeSector = mRuntimeSectors[ i ];

		minCost = std::min( minCost, runtimeSector.mCost );
		maxCost = std::max( maxCost, runtimeSector.mCost );

		if ( runtimeSector.mUpdateObstacles )
		{
			runtimeSector.RebuildObstacles( this );
		}
		if ( runtimeSector.mUpdatePortals )
		{
			buildPortals.push_back( &runtimeSector );
		}

		for ( size_t s = 0; s < runtimeSector.mSubSectors.size(); ++s )
		{
			RuntimeNavSector & subSector = runtimeSector.mSubSectors[ s ];

			minCost = std::min( minCost, subSector.mCost );
			maxCost = std::max( maxCost, subSector.mCost );

			if ( subSector.mUpdateObstacles )
			{
				subSector.RebuildObstacles( this );
			}
			if ( subSector.mUpdatePortals )
			{
				buildPortals.push_back( &subSector );
			}
		}
	}
	
	for ( size_t i = 0; i < buildPortals.size(); ++i )
	{
		buildPortals[ i ]->RebuildPortals( this );
	}

	// todo: invalidate neighboring sectors that connect to this sector
	// because they need to rebuild their sectors as well to create
	// new portals to the new sectors we just built here.
}

//////////////////////////////////////////////////////////////////////////

void PathPlannerNavMesh::UpdateObstacles()
{
	static obuint32 nextHandle = 0;

	// see if we can activate any dynamic obstacles
	for ( size_t i = 0; i < mDynamicObstacles.size(); ++i )
	{
		const Obstacle & dynObs = mDynamicObstacles[ i ];

		Vector3f velocity = Vector3f::ZERO;
		if ( !EngineFuncs::EntityVelocity( dynObs.mEntity, velocity ) || !velocity.IsZero() )
			continue;

		Box3f obb;
		if ( !EngineFuncs::EntityWorldOBB( dynObs.mEntity, obb ) )
			continue;

		const float radii[] = 
		{
			10.0f,
			30.0f,
			60.0f
		};
		const float cost[] = 
		{
			9.5f,
			6.0f,
			3.0f
		};

		for ( int x = 0; x < 3; ++x )
		{
			obb.Extent[ 0 ] = radii[x];
			obb.Extent[ 1 ] = radii[x];
			obb.Extent[ 2 ] = radii[x];

			Obstacle obs;
			obs.mPosition = obb.Center;
			obs.mEntity = dynObs.mEntity;
			obs.mCost = cost[x];

			Vector3f vertices[8];
			obb.ComputeVertices(vertices);
			obs.mPoly.resize( 0 );
			obs.mPoly.push_back( vertices[ 0 ] );
			obs.mPoly.push_back( vertices[ 1 ] );
			obs.mPoly.push_back( vertices[ 2 ] );
			obs.mPoly.push_back( vertices[ 3 ] );
			mPendingObstacles.push_back( obs );
		}

		/*obb.Extent[ 0 ] = 50;
		obb.Extent[ 1 ] = 50;
		obb.Extent[ 2 ] = 50;

		Obstacle obs;
		obs.mPosition = obb.Center;
		obs.mEntity = dynObs.mEntity;
		obs.mCost = dynObs.mCost;

		Vector3f vertices[8];
		obb.ComputeVertices(vertices);
		obs.mPoly.push_back( vertices[ 0 ] );
		obs.mPoly.push_back( vertices[ 1 ] );
		obs.mPoly.push_back( vertices[ 2 ] );
		obs.mPoly.push_back( vertices[ 3 ] );
		mPendingObstacles.push_back( obs );*/

		mDynamicObstacles.erase( mDynamicObstacles.begin() + i );
		--i;
	}

	if ( mPendingObstacles.size() > 0 )
	{
		const size_t newObstacleStart = mObstacles.size();

		mObstacles.reserve( mObstacles.size() + mPendingObstacles.size() );
		mObstacles.insert( mObstacles.end(), mPendingObstacles.begin(), mPendingObstacles.end() );
		mPendingObstacles.resize( 0 );

		const size_t newObstacleEnd = mObstacles.size();

		for ( size_t i = newObstacleStart; i < newObstacleEnd; ++i )
		{
			Obstacle & obs = mObstacles[ i ];
			obs.mHandle = ++nextHandle;
			
			// if we don't find a starting sector we can't propagate it correctly
			RuntimeNavSector * obstacleSector = GetSectorAt( obs.mPosition );
			if ( obstacleSector == NULL )
			{
				obs.mExpireTime = 0;
				continue;
			}
			if ( obstacleSector->mParent )
				obstacleSector = obstacleSector->mParent;

			typedef std::set<RuntimeNavSector*> SectorSet;
			SectorSet obstacleClosedSet;

			struct OpenNode
			{
				RuntimeNavSector *	mSector;
				Vector3List			mPoly;

				OpenNode( RuntimeNavSector * sector, const Vector3List & poly ) 
					: mSector( sector )
					, mPoly( poly ) 
				{
				}
			};

			// flood fill and propagate clipped obtacles until the whole poly is clipped
			typedef std::list<OpenNode> ExploreOpen;

			ExploreOpen openList;

			openList.push_back( OpenNode( obstacleSector, obs.mPoly ) );

			Vector3List clippedPoly;

			size_t numAffected  = 0;

			while ( !openList.empty() )
			{
				OpenNode node = openList.front();
				openList.pop_front();
				
				// so we don't backtrack to it
				obstacleClosedSet.insert( node.mSector );
				
				// customize the poly for this sector with the clipped poly
				Obstacle sectorObstacle = obs;
				obs.mAffectedSectors.push_back( node.mSector );
				sectorObstacle.mAffectedSectors.push_back( node.mSector );
				sectorObstacle.mPoly = node.mPoly;
				node.mSector->mObstacles.push_back( sectorObstacle );
				node.mSector->mUpdateObstacles = true;
				++numAffected;
				
				// attempt to clip and propagate the poly through portals
				for ( size_t p = 0; p < node.mSector->mPortals.size(); ++p )
				{
					const NavPortal & portal = node.mSector->mPortals[ p ];

					if ( obstacleClosedSet.find( portal.mDstSector ) != obstacleClosedSet.end() )
						continue;

					const Vector3f portalNormal = portal.mSegment.Direction.Cross( Vector3f::UNIT_Z );

					clippedPoly = sectorObstacle.mPoly;

					// clip the portal normal
					clippedPoly = Utils::ClipPolygonToPlanes( clippedPoly, Plane3f( portalNormal, portal.mSegment.Center ) );
					if ( clippedPoly.empty() )
						continue;

					// clip the portal view planes
					clippedPoly = Utils::ClipPolygonToPlanes( clippedPoly, Plane3f( obs.mPosition, obs.mPosition + Vector3f::UNIT_Z, portal.mSegment.P0 ) );
					if ( clippedPoly.empty() )
						continue;
					clippedPoly = Utils::ClipPolygonToPlanes( clippedPoly, Plane3f( obs.mPosition, obs.mPosition - Vector3f::UNIT_Z, portal.mSegment.P1 ) );
					if ( clippedPoly.empty() )
						continue;
					
					openList.push_back( OpenNode( portal.mDstSector, clippedPoly ) );
				}
			}

			//EngineFuncs::ConsoleMessage( va( "Obstacle %d affected %d sectors", obs.mHandle, numAffected ) );
		}
	}
	
	for ( size_t i = 0; i < mObstacles.size(); ++i )
	{
		Obstacle & obs = mObstacles[ i ];
		if ( obs.Expired() )
		{
			for ( size_t s = 0; s < obs.mAffectedSectors.size(); ++s )
			{
				RuntimeNavSector * ns = obs.mAffectedSectors[ s ];
				ns->mObstacles.erase( std::remove( ns->mObstacles.begin(), ns->mObstacles.end(), obs ), ns->mObstacles.end() );				
				ns->mUpdateObstacles = true;
			}

			mObstacles.erase( mObstacles.begin() + i );
			--i;
		}
	}


	Vector3f aimPos;
	if ( Utils::GetLocalAimPoint( aimPos ) )
	{
		const RuntimeNavSector * mySector = GetSectorAt( aimPos );
		if ( mySector )
		{
			for ( size_t i = 0; i < mySector->mPortals.size(); ++i )
			{
				const NavPortal & portal =  mySector->mPortals[ i ];

				const Vector3f portalNormal = portal.mSegment.Direction.Cross( Vector3f::UNIT_Z );

				// clip the portal normal
				RenderBuffer::AddArrow( portal.mSegment.Center, portal.mSegment.Center + portalNormal * 32.0f, COLOR::CYAN );
				RenderBuffer::AddArrow( portal.mSegment.P0, portal.mSegment.P0 + Plane3f( aimPos, aimPos + Vector3f::UNIT_Z, portal.mSegment.P0 ).Normal * 32.0f, COLOR::RED );
				RenderBuffer::AddArrow( portal.mSegment.P1, portal.mSegment.P1 + Plane3f( aimPos, aimPos - Vector3f::UNIT_Z, portal.mSegment.P1 ).Normal * 32.0f, COLOR::BLUE );
			}
		}
	}
}

PathPlannerNavMesh::RuntimeNavSector *PathPlannerNavMesh::GetSectorAt(const Vector3f &_pos, float _distance)
{
	const float CHAR_HALF_HEIGHT = NavigationMeshOptions::CharacterHeight /** 0.5f*/;

	Vector3f raySrc = _pos+Vector3f(0,0,CHAR_HALF_HEIGHT);
	Vector3f rayDst = raySrc - Vector3f::UNIT_Z * _distance;
	NavCollision nc = FindCollision(raySrc, rayDst);
	if ( nc.DidHit() )
	{
		return nc.HitSector();
	}
	return NULL;
}

PathPlannerNavMesh::NavCollision PathPlannerNavMesh::FindCollision(const Vector3f & from, const Vector3f & to)
{
	Vector3f hitPos, hitNormal;
	void * hitAttrib = NULL;

	float closestHit = SquaredLength( from, to );
	if ( mSectorCollision.CollideRay( from, to, hitPos, hitNormal, hitAttrib ) )
	{
		closestHit = SquaredLength( from, hitPos );
	}

	// try the dynamic collisions
	for ( size_t i = 0; i < mRuntimeSectorCollision.size(); ++i )
	{
		if ( mRuntimeSectorCollision[ i ] != NULL )
		{
			Vector3f subHitPos, subHitNormal;
			void * subHitAttrib = NULL;

			if ( mRuntimeSectorCollision[ i ]->CollideRay( from, to, subHitPos, subHitNormal, subHitAttrib ) )
			{
				const float subHitDistSq = SquaredLength( from, subHitPos );
				if ( subHitDistSq < closestHit )
				{
					closestHit = subHitDistSq;
					hitPos = subHitPos;
					hitNormal = subHitNormal;
					hitAttrib = subHitAttrib;
				}
			}
		}
	}

	if ( hitAttrib )
		return NavCollision( hitPos, hitNormal, static_cast<RuntimeNavSector*>( hitAttrib ) );
	else
		return NavCollision();
}

PathPlannerNavMesh::NavSector * PathPlannerNavMesh::GetSectorAtCursor()
{
	Vector3f vFacing, vPos;
	if ( !Utils::GetLocalEyePosition( vPos ) || !Utils::GetLocalFacing( vFacing ) )
		return NULL;

	NavCollision nc = FindCollision( vPos, vPos + vFacing * 2048.f );
	if( !nc.DidHit() )
		return NULL;

	return nc.HitSector()->mSector;
}

void PathPlannerNavMesh::SectorsFromAttribs( RuntimeSectorRefs & sectorsOut,
											const AABB & aabb,
											const CollisionModel::AttribSet & attribs,
											const bool staticOnly )
{
	for ( CollisionModel::AttribSet::iterator it = attribs.begin(); it != attribs.end(); ++it )
	{
		RuntimeNavSector * ns = static_cast<RuntimeNavSector*>( *it );

		if ( staticOnly || mRuntimeSectorCollision[ ns->mIndex ] == NULL )
			sectorsOut.push_back( ns ); // no sub collision, so return this sector
		else
		{
			// return sub sector collisions
			CollisionModel::AttribSet subAttribs;
			mRuntimeSectorCollision[ ns->mIndex ]->CollideAABB( aabb, subAttribs );

			for ( CollisionModel::AttribSet::iterator it2 = subAttribs.begin(); it2 != subAttribs.end(); ++it2 )
			{
				RuntimeNavSector * subSector = static_cast<RuntimeNavSector*>( *it2 );
				sectorsOut.push_back( subSector );
			}
		}
	}
}

void PathPlannerNavMesh::SectorsFromAttribs( RuntimeSectorRefs & sectorsOut,
											const Box3f & obb,
											const CollisionModel::AttribSet & attribs,
											const bool staticOnly )
{
	for ( CollisionModel::AttribSet::iterator it = attribs.begin(); it != attribs.end(); ++it )
	{
		RuntimeNavSector * ns = static_cast<RuntimeNavSector*>( *it );

		if ( staticOnly || mRuntimeSectorCollision[ ns->mIndex ] == NULL )
			sectorsOut.push_back( ns ); // no sub collision, so return this sector
		else
		{
			// return sub sector collisions
			CollisionModel::AttribSet subAttribs;
			mRuntimeSectorCollision[ ns->mIndex ]->CollideOBB( obb, subAttribs );

			for ( CollisionModel::AttribSet::iterator it2 = subAttribs.begin(); it2 != subAttribs.end(); ++it2 )
			{
				RuntimeNavSector * subSector = static_cast<RuntimeNavSector*>( *it2 );
				sectorsOut.push_back( subSector );
			}
		}
	}
}

int PathPlannerNavMesh::FindRuntimeSectors( const AABB & aabb,
										   RuntimeSectorRefs & sectorsOut,
										   const bool staticOnly )
{
	sectorsOut.resize( 0 );

	CollisionModel::AttribSet attribs;
	mSectorCollision.CollideAABB( aabb, attribs );
	SectorsFromAttribs( sectorsOut, aabb, attribs, staticOnly );
	return (int)sectorsOut.size();
}

int PathPlannerNavMesh::FindRuntimeSectors( const Box3f & obb,
										   RuntimeSectorRefs & sectorsOut,
										   const bool staticOnly )
{
	sectorsOut.resize( 0 );

	CollisionModel::AttribSet attribs;
	mSectorCollision.CollideOBB( obb, attribs );
	SectorsFromAttribs( sectorsOut, obb, attribs, staticOnly );
	return (int)sectorsOut.size();
}

//////////////////////////////////////////////////////////////////////////

void PathPlannerNavMesh::EntityCreated( const EntityInstance &ei )
{
	if ( ei.m_EntityCategory.CheckFlag(ENT_CAT_OBSTACLE) && ei.m_EntityCategory.CheckFlag(ENT_CAT_STATIC) )
	{
		Obstacle obs;
		obs.mEntity = ei.m_Entity;
		obs.mCost = NavPortal::MAX_COST; // TODO

		if ( !EngineFuncs::EntityPosition( obs.mEntity, obs.mPosition ) )
			return;

		Box3f obb;
		if ( !EngineFuncs::EntityWorldOBB( obs.mEntity, obb ) )
			return;

		Vector3f vertices[8];
		obb.ComputeVertices(vertices);
		obs.mPoly.push_back( vertices[ 0 ] );
		obs.mPoly.push_back( vertices[ 1 ] );
		obs.mPoly.push_back( vertices[ 2 ] );
		obs.mPoly.push_back( vertices[ 3 ] );
		
		mPendingObstacles.push_back( obs );
		
		///////////////////////////////////////////////////////////////////////////////////////
		// test field of view obstacle
		Matrix3f orient;
		if ( EngineFuncs::EntityOrientation( ei.m_Entity, orient ) )
		{
			if ( ei.m_EntityClass == 12 || ei.m_EntityClass == 37 ) // sentry, turret
			{
				Obstacle fovObs;
				fovObs.mPosition = obs.mPosition;
				fovObs.mEntity = ei.m_Entity;
				fovObs.mCost = 9.0f;
				fovObs.mPoly.push_back( fovObs.mPosition );
				fovObs.mPoly.push_back( fovObs.mPosition + Vector3f( orient.GetColumn( 0 ) ) * 1200.0f + Vector3f( orient.GetColumn( 1 ) ) * 600.0f );
				fovObs.mPoly.push_back( fovObs.mPosition + Vector3f( orient.GetColumn( 0 ) ) * 1200.0f - Vector3f( orient.GetColumn( 1 ) ) * 600.0f );
				mPendingObstacles.push_back( fovObs );
			}
			
		}	
	}
	// TEMP
	if ( ei.m_EntityClass == 32 ) // pipe
	{
		Obstacle obs;
		obs.mEntity = ei.m_Entity;
		obs.mCost = 4.0f;
		mDynamicObstacles.push_back( obs );
	}

}

void PathPlannerNavMesh::EntityDeleted( const EntityInstance &ei )
{
	for ( size_t i = 0; i < mDynamicObstacles.size(); ++i )
	{
		if ( mDynamicObstacles[ i ].mEntity == ei.m_Entity )
		{
			mDynamicObstacles.erase( mDynamicObstacles.begin() + i );
			--i;
		}
	}

	for ( size_t i = 0; i < mPendingObstacles.size(); ++i )
	{
		if ( mPendingObstacles[ i ].mEntity == ei.m_Entity )
		{
			mPendingObstacles.erase( mPendingObstacles.begin() + i );
			--i;
		}
	}

	for ( size_t i = 0; i < mObstacles.size(); ++i )
	{
		if ( mObstacles[ i ].mEntity == ei.m_Entity )
		{
			// mark it for immediate removal
			mObstacles[ i ].mExpireTime = 0;
		}
	}
}

PathInterface * PathPlannerNavMesh::AllocPathInterface()
{
	return new NavMeshPathInterface( this ); 
}

//////////////////////////////////////////////////////////////////////////

