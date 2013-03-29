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
//////////////////////////////////////////////////////////////////////////

void PathPlannerNavMesh::CollisionData::Free()
{
	OB_DELETE( m_CollisionTree );
	OB_DELETE( m_MeshInterface );
	OB_ARRAY_DELETE( m_Verts );
	OB_ARRAY_DELETE( m_TriSectorMap );
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

		int										BoundaryEdge;

		float									CostHeuristic;
		float									CostGiven;
		float									CostFinal;

		obint32 Hash() const
		{
			return (obint32)Utils::MakeId32(
				(obint16)Sector->mIndex,
				Portal ? (obint16)Portal->mDestSector : (obint16)0xFF);
		}

		void Reset()
		{
			Sector = 0;
			Parent = 0;

			Portal = 0;

			BoundaryEdge = -1;

			CostHeuristic = 0.f;
			CostGiven = 0.f;
			CostFinal = 0.f;
		}
	};
	typedef std::vector<PlanNode*> NodeList;

	NodeList &GetSolution() { return m_Solution; }

	bool IsFinished() const
	{
		return fl.Finished;
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

		fl.Finished = 0;

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
		while(!fl.Finished && _numsteps--)
		{
			// get the next node
			PlanNode *pCurNode = _GetNextOpenNode();
			if(!pCurNode)
			{
				fl.Finished = true;
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
				fl.Finished = true;

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
		fl.Finished = true;
		m_UsedNodes = 0;
	}
private:
	/*std::vector<float>		mCostG;
	std::vector<float>		mCostF;
	std::vector<int>		mOpen;*/

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

	struct
	{
		obuint32							Finished : 1;
	} fl;

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

			//for(int b = 0; b < )

			//////////////////////////////////////////////////////////////////////////
			PlanNode tmpNext;
			tmpNext.Reset();
			tmpNext.Portal = &portal;
			tmpNext.Parent = _node;
			tmpNext.Sector = &g_PlannerNavMesh->mRuntimeSectors[portal.mDestSector];
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

PathPlannerNavMesh::RuntimeNavSector::RuntimeNavSector( obuint32 index, NavSector * sector, NavmeshIO::SectorData * data )
	: mIndex( index )
	, mSector( sector )
	, mSectorData( data )
	, mUpdatePortals( true )
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

		Vector3f moverPos;
		Vector3f moverFwd, moverRight, moverUp;
		good &= EngineFuncs::EntityPosition(
			mover,
			moverPos );
		good &= EngineFuncs::EntityOrientation(
			mover,
			moverFwd,
			moverRight,
			moverUp );

		if ( good )
		{
			const Matrix3f moverOrient( moverRight, moverFwd, moverUp, true );

			mSectorData->mutable_mover()->set_id( mover.AsInt() );

			mLocalPoly.resize( mPoly.size() );
			mSectorData->clear_localoffsets();
			for ( size_t i = 0; i < mPoly.size(); ++i )
			{
				mLocalPoly[ i ] = (mPoly[ i ] - moverPos) * moverOrient.Transpose();

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

		Vector3f moverPos;
		Vector3f moverFwd, moverRight, moverUp;
		if ( mover.IsValid() &&
			EngineFuncs::EntityPosition( mover, moverPos ) &&
			EngineFuncs::EntityOrientation(
			mover,
			moverFwd,
			moverRight,
			moverUp ) )
		{
			const Matrix3f moverOrient( moverRight, moverFwd, moverUp, true );

			for ( size_t i = 0; i < mPoly.size(); ++i )
				mPoly[ i ] = moverPos + mLocalPoly[ i ] * moverOrient;
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

void PathPlannerNavMesh::RuntimeNavSector::BuildSectorPortals( PathPlannerNavMesh * navmesh )
{
	mPortals.resize( 0 );

	SegmentList edges;
	GetEdgeSegments( edges );

	const float MAX_STEPUP = std::max( JUMP_HEIGHT, mSectorData->waterdepth() + WATER_EXIT );

	for ( size_t e = 0; e < edges.size(); ++e )
	{
		const Segment3f & seg0 = edges[ e ];

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

		RuntimeSectorRefList nearbySectors;
		navmesh->FindRuntimeSectors( obb, nearbySectors );

		// search all sectors for neighboring connections
		for ( size_t n = 0; n < nearbySectors.size(); ++n )
		{
			RuntimeNavSector * neighbor = nearbySectors[ n ];

			if ( this == neighbor )
				continue;

			const Vector3List & edges1 = neighbor->mPoly;
			for ( size_t nb = 1; nb <= edges1.size(); ++nb )
			{
				Segment3f seg1( edges1[ nb-1 ], edges1[ nb % edges1.size() ] );

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
					portal.mDestSector = neighbor->mIndex;

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
}

//////////////////////////////////////////////////////////////////////////

PathPlannerNavMesh::PathPlannerNavMesh()
{
	// TEMP
	m_PlannerFlags.SetFlag(NAV_VIEW);
	m_PlannerFlags.SetFlag(NAV_VIEWCONNECTIONS);
	m_PlannerFlags.SetFlag(NAVMESH_STEPPROCESS);
	m_PlannerFlags.SetFlag(NAV_VIEWFLAGS);

	m_CursorColor = COLOR::BLUE;

	m_MapCenter = Vector3f::ZERO;

	m_ToolCancelled = false;

	m_SpanMap = NULL;
	m_Influence = NULL;

	g_PlannerNavMesh = this;

	mInfluenceBufferId = 0;

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

void PathPlannerNavMesh::RegisterScriptFunctions(gmMachine *a_machine)
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
	/*if ( solid )
	Utils::DrawPolygon( polyList, color, dur );
	else
	RenderBuffer::AddLine( polyList, color, dur, vertSize );*/

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
	/*if ( solid )
	Utils::DrawPolygon( polyList, color, dur );
	else
	RenderBuffer::AddLine( polyList, color, dur, vertSize );*/

	if ( solid )
		RenderBuffer::AddPolygonFilled( polyList, color );
	else
		RenderBuffer::AddPolygonSilouette( polyList, color );

	if ( vertSize > 0.0 )
	{
		RenderBuffer::AddPoints( polyList, COLOR::MAGENTA, vertSize );
	}
}

void PathPlannerNavMesh::Update( System & system )
{
	Prof(PathPlannerNavMesh);

	if ( mCurrentTool )
		mCurrentTool->Update( this );

	UpdateFsm(IGame::GetDeltaTimeSecs());

	UpdateMoverSectors();
	UpdatePortals();
	UpdateObstacles();

	if(m_PlannerFlags.CheckFlag(NAV_VIEW))
	{
		g_PathFind.Render();

		//////////////////////////////////////////////////////////////////////////
		Vector3f vLocalPos, vLocalAim, vAimPos, vAimNormal;
		Utils::GetLocalEyePosition(vLocalPos);
		Utils::GetLocalFacing(vLocalAim);
		if(Utils::GetLocalAimPoint(vAimPos, &vAimNormal, TR_MASK_FLOODFILLENT))
		{
			RenderBuffer::AddLine(vAimPos,
				vAimPos + vAimNormal * 16.f,
				m_CursorColor);

			RenderBuffer::AddLine(vAimPos,
				vAimPos + vAimNormal.Perpendicular() * 16.f, m_CursorColor);

			/*RenderBuffer::AddLine(vAimPos,
			vAimPos + vAimNormal * 16.f,
			m_CursorColor,
			IGame::GetDeltaTimeSecs()*2.f);

			RenderBuffer::AddLine(vAimPos,
			vAimPos + vAimNormal.Perpendicular() * 16.f, m_CursorColor,
			IGame::GetDeltaTimeSecs()*2.f);*/
		}
		m_CursorColor = COLOR::BLUE; // back to default
		//////////////////////////////////////////////////////////////////////////
		obint32 iCurrentSector = -1;
		NavCollision nc = FindCollision(vLocalPos, vLocalPos + vLocalAim * 1024.f);
		if(nc.DidHit())
		{
			iCurrentSector = nc.HitAttrib().Fields.ActiveId;
		}
		//////////////////////////////////////////////////////////////////////////

		bool influenceDone = true;
		if ( m_Influence )
		{
			static int iterations = 200;
			influenceDone = m_Influence->UpdateInfluences( iterations );
		}

		//////////////////////////////////////////////////////////////////////////
		// Draw our nav sectors
		for ( obuint32 i = 0; i < mRuntimeSectors.size(); ++i )
		{
			const RuntimeNavSector &ns = mRuntimeSectors[i];

			// mirrored sectors are contiguous at the end of
			// the mRuntimeSectors list, so draw them differently
			obColor SEC_COLOR = COLOR::GREEN;
			if ( i >= mNavSectors.size() )
				SEC_COLOR = COLOR::CYAN;

			//////////////////////////////////////////////////////////////////////////
			// Draw sector obstacles
			if ( ns.mObstacles.size() > 0 )
			{
				ClipperLib::Polygon		sectorPoly;
				ClipperLib::Polygons	obstaclePolys;

				for ( size_t b = 0; b < ns.mPoly.size(); ++b )
				{
					sectorPoly.push_back( ClipperLib::IntPoint(
						(ClipperLib::long64)ns.mPoly[b].X(),
						(ClipperLib::long64)ns.mPoly[b].Y() ) );
				}

				for ( size_t o = 0; o < ns.mObstacles.size(); ++o )
				{
					obstaclePolys.push_back( ClipperLib::Polygon() );

					ClipperLib::Polygon & obs = obstaclePolys.back();
					for ( size_t ov = 0; ov < ns.mObstacles[o].mPoly.size(); ++ov )
					{
						obs.push_back( ClipperLib::IntPoint(
							(ClipperLib::long64)ns.mObstacles[o].mPoly[ ov ].X(),
							(ClipperLib::long64)ns.mObstacles[o].mPoly[ ov ].Y() ) );
					}
				}

				assert ( ClipperLib::Orientation( sectorPoly ) );

				static ClipperLib::ClipType ct = ClipperLib::ctDifference;
				static ClipperLib::PolyFillType pft = ClipperLib::pftNonZero;
				static ClipperLib::PolyFillType cft = ClipperLib::pftNonZero;

				ClipperLib::Clipper clipNav;
				clipNav.AddPolygon( sectorPoly, ClipperLib::ptSubject );
				clipNav.AddPolygons( obstaclePolys, ClipperLib::ptClip );
				if ( clipNav.Execute( ct, pft, cft ) )
				{
					typedef std::list<TPPLPoly> PolyList;
					PolyList inputPolys;
					PolyList subPolys;

					// add all the solution polygons to the input polygons
					// of the partition list
					ClipperLib::Polygon poly;
					for ( size_t p = 0; p < clipNav.GetNumPolysOut(); ++p )
					{
						bool polyIsHole = false;
						if ( clipNav.GetPolyOut( p, poly, polyIsHole ) )
						{
							inputPolys.push_back( TPPLPoly() );
							inputPolys.back().Init( poly.size() );

							Vector3List drawList;

							for ( size_t j = 0; j < poly.size(); ++j )
							{
								const ClipperLib::IntPoint & pt = poly[ j ];

								TPPLPoint & tpp = inputPolys.back()[ j ];
								tpp.x = (float)pt.X;
								tpp.y = (float)pt.Y;

								//////////////////////////////////////////////////////////////////////////
								Vector3f vec3d( (float)tpp.x, (float)tpp.y, 0.0f );

								Utils::PushPointToPlane( vec3d, ns.mPlane, -Vector3f::UNIT_Z );

								drawList.push_back( vec3d );
								//////////////////////////////////////////////////////////////////////////
							}

							inputPolys.back().SetHole( polyIsHole );
							inputPolys.back().SetOrientation( polyIsHole ? TPPL_CW : TPPL_CCW );
						}
					}

					TPPLPartition partition;
					if ( partition.ConvexPartition_HM( &inputPolys, &subPolys ) != 0 )
					{
						// translate it back to sector space
						for ( PolyList::iterator it = subPolys.begin();
							it != subPolys.end();
							++it )
						{
							const TPPLPoly & ply = *it;
							DrawPoly( true, ns, ply, 0.0f, SEC_COLOR.fade(100), 2.0f, 4.0f );
							DrawPoly( false, ns, ply, 0.0f, SEC_COLOR, 2.0f, 4.0f );
						}
					}
					else
					{
						const RuntimeNavSector &ns = mRuntimeSectors[i];

						RenderBuffer::AddLine(ns.mPoly, COLOR::RED.fade( 100 ));

						// Draw the error input poly line
						for ( PolyList::iterator p = inputPolys.begin();
							p != inputPolys.end();
							++p )
						{
							DrawPoly( false, ns, (*p), 6.0f, COLOR::MAGENTA, 2.0f, 4.0f );
						}
					}
				}
			}
			else
			{
				RenderBuffer::AddPolygonFilled( ns.mPoly, SEC_COLOR.fade(100) );
				RenderBuffer::AddPolygonSilouette( ns.mPoly, SEC_COLOR );

				// render the points, colored by whether this sector is mirrored
				const obColor ptColor = ( ns.mSector->mMirror != NavmeshIO::Sector_MirrorDir_MirrorNone ) ?
					COLOR::CYAN : COLOR::MAGENTA;
				RenderBuffer::AddPoints( ns.mPoly, ptColor, 5.0f );
			}

			if ( m_PlannerFlags.CheckFlag( NAV_VIEWCONNECTIONS ) )
			{
				//ConnectTest();

				for ( size_t p = 0; p < ns.mPortals.size(); ++p )
				{
					const NavPortal &portal = ns.mPortals[ p ];

					const Vector3f fwd = ( portal.mSegment.P1 - portal.mSegment.P0 ).UnitCross( Vector3f::UNIT_Z );

					RenderBuffer::AddLine(
						portal.mSegment.P0 + Vector3f(0.0f,0.0f,2.0f),
						portal.mSegment.P1 + Vector3f(0.0f,0.0f,2.0f),
						COLOR::MAGENTA );

					const RuntimeNavSector & destSector = mRuntimeSectors[ portal.mDestSector ];

					obColor portalColor = COLOR::BLUE;
					if ( portal.mFlags.CheckFlag( NavPortal::FL_DISABLED ) )
						portalColor = COLOR::RED;
					else if ( portal.mFlags.CheckFlag( NavPortal::FL_JUMP ) )
						portalColor = COLOR::CYAN;

					Vector3f vP0 = portal.mSegment.Center;
					Vector3f vP1 = vP0;

					Utils::PushPointToPlane( vP1, destSector.mPlane, -Vector3f::UNIT_Z );

					RenderBuffer::AddLine( vP0, vP1,portalColor );
					vP0 = vP1;
					vP1 = destSector.CalculateCenter();
					RenderBuffer::AddArrow( vP0, vP1,portalColor );
				}
			}

			if ( m_PlannerFlags.CheckFlag( NAV_VIEWFLAGS ) )
			{
				using namespace google;
				const protobuf::Reflection * refl = ns.mSectorData->GetReflection();

				// fields for mirrored
				NavmeshIO::SectorData & data = *ns.mSectorData;
				NavmeshIO::SectorData & otherData =
					ns.IsMirroredSector() ? ns.mSector->mSectorData : ns.mSector->mSectorDataMirrored;

				std::vector<const protobuf::FieldDescriptor*> fields0;
				std::vector<const protobuf::FieldDescriptor*> fields1;

				refl->ListFields( data, &fields0 );
				refl->ListFields( otherData, &fields1 );

				StringVector strs;

				// what fields do the sectors share
				while( fields0.size() > 0 )
				{
					const protobuf::FieldDescriptor * field = fields0.back();

					if ( !field->is_repeated() )
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

					if ( !field->is_repeated() )
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

				/*std::string strData = ns.mSectorData->ShortDebugString();*/
				RenderBuffer::AddString3dRadius(
					ns.CalculateCenter() + Vector3f(0.f,0.f,32.f),
					COLOR::BLUE, 1024.f, strData.c_str() );
			}
		}

		//////////////////////////////////////////////////////////////////////////
		if ( m_SpanMap != NULL )
		{
			struct RenderSpanCell : SpanMap::RenderFunctor
			{
			public:
				RenderSpanCell( RenderBuffer::QuadList & lst )
					: mList( lst ) { }

				virtual void RenderCell( const Vector3f & pos, float cellSize, float influenceRatio )
				{
					static obuint8 alpha = 255;

					RenderBuffer::Quad q;
					q.v[ 0 ] = pos + Vector3f( -cellSize, -cellSize, 0.0f );
					q.v[ 1 ] = pos + Vector3f(  cellSize, -cellSize, 0.0f );
					q.v[ 2 ] = pos + Vector3f(  cellSize,  cellSize, 0.0f );
					q.v[ 3 ] = pos + Vector3f( -cellSize,  cellSize, 0.0f );
					q.c = GetCoolWarmColor( influenceRatio ).fade( alpha );
					mList.push_back( q );
				}

				RenderBuffer::QuadList & mList;
			};

			float influenceMinWeight = 0.0f;
			float influenceMaxWeight = 1.0f;
			if ( m_Influence )
			{
				m_Influence->GetWeightRange( influenceMinWeight, influenceMaxWeight );
			}

			if ( mInfluenceBufferId == 0 && m_SpanMap->GetNumSpans() > 0 )
			{
				RenderBuffer::QuadList prims;
				prims.reserve( m_SpanMap->GetNumSpans() * 2 );

				RenderSpanCell renderCb( prims );
				m_SpanMap->RenderWithCallback( renderCb, influenceDone ? m_Influence : NULL );

				RenderBuffer::StaticBufferCreate( mInfluenceBufferId, prims );
			}

			if ( mInfluenceBufferId != 0 )
			{
				RenderBuffer::StaticBufferDraw( mInfluenceBufferId );
			}
		}
		//////////////////////////////////////////////////////////////////////////
		if(!m_WorkingSector.mPoly.empty())
		{
			RenderBuffer::AddLine(
				m_WorkingSectorStart,
				m_WorkingSectorStart + m_WorkingSectorPlane.Normal * 10.0f,
				COLOR::MAGENTA );

			RenderBuffer::AddPolygonFilled( m_WorkingSector.mPoly, COLOR::BLUE.fade(100) );
			if ( m_WorkingSector.IsMirrored() )
			{
				NavSectorBase temp;
				m_WorkingSector.GetMirroredCopy(m_MapCenter, m_WorkingSector.mMirror, temp);
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

	OB_DELETE( mCurrentTool );
	mRuntimeSectors.clear();
	mNavSectors.clear();

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

			for ( int i = 0; i < s.vertices_size(); ++i )
			{
				const NavmeshIO::SectorVert & v = s.vertices( i );
				sector.mPoly.push_back( Vector3f(
					v.position().x(),
					v.position().y(),
					v.position().z() ) );
			}

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

int PathPlannerNavMesh::PlanPathToNearest(Client *_client, const Vector3f &_start, const DestinationVector &_goals, const NavFlags &_team)
{
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
	OB_DELETE( m_Influence );
	OB_DELETE( m_SpanMap );

	m_CollisionData.Free();
}

void PathPlannerNavMesh::InitSectors()
{
	m_CollisionData.Free();

	mRuntimeSectors.clear();

	RuntimeSectorList	allSectors;

	std::vector<IceMaths::Point>	vertices;
	std::vector<PolyAttrib>			triSectors;

	// create the active sector list and the collision data at the same time
	// so we can create a proper mapping
	for ( size_t i = 0; i < mNavSectors.size(); ++i )
	{
		PolyAttrib attr;
		attr.Fields.ActiveId = allSectors.size();
		attr.Fields.SectorId = i;
		attr.Fields.Mirrored = 0;

		RuntimeNavSector runtimeSector( allSectors.size(), &mNavSectors[ i ], &mNavSectors[ i ].mSectorData );
		runtimeSector.mPoly = mNavSectors[ i ].mPoly;

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

		//////////////////////////////////////////////////////////////////////////
		Vector3f center = allSectors.back().CalculateCenter();
		for ( size_t v = 1; v <= allSectors.back().mPoly.size(); ++v )
		{
			triSectors.push_back( attr );

			vertices.push_back( IceMaths::Point(
				center.X(), center.Y(), center.Z() ) );

			vertices.push_back( IceMaths::Point(
				allSectors.back().mPoly[ v-1 ].X(),
				allSectors.back().mPoly[ v-1 ].Y(),
				allSectors.back().mPoly[ v-1 ].Z() ) );

			vertices.push_back( IceMaths::Point(
				allSectors.back().mPoly[ v % allSectors.back().mPoly.size() ].X(),
				allSectors.back().mPoly[ v % allSectors.back().mPoly.size() ].Y(),
				allSectors.back().mPoly[ v % allSectors.back().mPoly.size() ].Z() ) );

			if ( v == 1 )
			{
				allSectors.back().mPlane = Plane3f(
					center,
					allSectors.back().mPoly[ v-1 ],
					allSectors.back().mPoly[ v ] );
			}
		}
	}

	// do another pass for the mirrored set
	for ( size_t i = 0; i < mNavSectors.size(); ++i )
	{
		if ( mNavSectors[ i ].mMirror != NavmeshIO::Sector_MirrorDir_MirrorNone )
		{
			PolyAttrib attr;
			attr.Fields.ActiveId = allSectors.size();
			attr.Fields.SectorId = i;
			attr.Fields.Mirrored = 1;

			RuntimeNavSector runtimeSector( allSectors.size(), &mNavSectors[ i ], &mNavSectors[ i ].mSectorDataMirrored );
			mNavSectors[ i ].GetMirroredCopy( m_MapCenter, mNavSectors[ i ].mMirror, runtimeSector );

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

			//////////////////////////////////////////////////////////////////////////
			Vector3f center = allSectors.back().CalculateCenter();
			for ( size_t v = 1; v <= allSectors.back().mPoly.size(); ++v )
			{
				triSectors.push_back( attr );

				vertices.push_back( IceMaths::Point(
					center.X(), center.Y(), center.Z() ) );

				vertices.push_back( IceMaths::Point(
					allSectors.back().mPoly[ v-1 ].X(),
					allSectors.back().mPoly[ v-1 ].Y(),
					allSectors.back().mPoly[ v-1 ].Z() ) );

				vertices.push_back( IceMaths::Point(
					allSectors.back().mPoly[ v % allSectors.back().mPoly.size() ].X(),
					allSectors.back().mPoly[ v % allSectors.back().mPoly.size() ].Y(),
					allSectors.back().mPoly[ v % allSectors.back().mPoly.size() ].Z() ) );

				if ( v == 1 )
				{
					allSectors.back().mPlane = Plane3f(
						center,
						allSectors.back().mPoly[ v-1 ],
						allSectors.back().mPoly[ v ] );
				}
			}
			//////////////////////////////////////////////////////////////////////////
		}
	}

	// make the new set of sectors the active set
	mRuntimeSectors.swap( allSectors );

	if ( vertices.size() == 0 )
	{
		PolyAttrib attr;
		attr.Fields.ActiveId = 0;
		attr.Fields.SectorId = 0;
		attr.Fields.Mirrored = 0;

		// just make a single huge quad
		vertices.push_back( IceMaths::Point( -10000.0f, -10000.0f, -10000 ) );
		vertices.push_back( IceMaths::Point( -10000.0f,  10000.0f, -10000 ) );
		vertices.push_back( IceMaths::Point(  10000.0f,  10000.0f, -10000 ) );
		triSectors.push_back( attr );

		vertices.push_back( IceMaths::Point( -10000.0f, -10000.0f, -10000 ) );
		vertices.push_back( IceMaths::Point(  10000.0f,  10000.0f, -10000 ) );
		vertices.push_back( IceMaths::Point( 10000.0f,  -10000.0f, -10000 ) );
		triSectors.push_back( attr );
	}

	// initialize the opcode collision for navigation geometry probes
	m_CollisionData.m_Verts = new IceMaths::Point[ vertices.size() ];
	m_CollisionData.m_TriSectorMap = new PolyAttrib[ triSectors.size() ];

	memcpy( m_CollisionData.m_Verts, &vertices[0], sizeof(IceMaths::Point) * vertices.size() );
	memcpy( m_CollisionData.m_TriSectorMap, &triSectors[0], sizeof(unsigned int) * triSectors.size() );

	m_CollisionData.m_MeshInterface = new Opcode::MeshInterface();
	m_CollisionData.m_MeshInterface->SetNbTriangles( vertices.size() / 3 );
	m_CollisionData.m_MeshInterface->SetNbVertices( vertices.size() );
	m_CollisionData.m_MeshInterface->SetPointers( NULL, m_CollisionData.m_Verts );
	m_CollisionData.m_MeshInterface->SetInterfaceType( Opcode::MESH_TRIANGLE );

	Opcode::OPCODECREATE parms;
	parms.mIMesh = m_CollisionData.m_MeshInterface;
	m_CollisionData.m_CollisionTree = new Opcode::Model();
	if ( !m_CollisionData.m_CollisionTree->Build( parms ) )
	{
		m_CollisionData.Free();
	}
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

		_path.AddPt(vNodePos + Vector3f(0,0,CHAR_HALF_HEIGHT),32.f)
			.NavId(pn->Sector->mIndex)
			/*.Flags(m_Solution.back()->GetNavigationFlags())
			.OnPathThrough(m_Solution.back()->OnPathThrough())
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

void PathPlannerNavMesh::AddEntityConnection(const Event_EntityConnection &_conn)
{
}

//////////////////////////////////////////////////////////////////////////

void PathPlannerNavMesh::RemoveEntityConnection(GameEntity _ent)
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

void PathPlannerNavMesh::UpdatePortals()
{
	for ( size_t i = 0; i < mRuntimeSectors.size(); ++i )
	{
		RuntimeNavSector & runtimeSector = mRuntimeSectors[ i ];
		if ( runtimeSector.mUpdatePortals )
		{
			runtimeSector.BuildSectorPortals( this );

			// update the connections for this portal
			runtimeSector.mUpdatePortals = false;
		}
	}
}

void PathPlannerNavMesh::UpdateObstacles()
{
	RuntimeSectorRefList nearbySectors;

	for ( size_t i = 0; i < mPendingObstacles.size(); ++i )
	{
		const Obstacle & obs = mPendingObstacles[ i ];

		AABB obsBounds;
		obsBounds.m_Mins[ 0 ] = std::numeric_limits<float>::max();
		obsBounds.m_Mins[ 1 ] = std::numeric_limits<float>::max();
		obsBounds.m_Mins[ 2 ] = std::numeric_limits<float>::max();
		obsBounds.m_Maxs[ 0 ] = std::numeric_limits<float>::lowest();
		obsBounds.m_Maxs[ 1 ] = std::numeric_limits<float>::lowest();
		obsBounds.m_Maxs[ 2 ] = std::numeric_limits<float>::lowest();

		for ( size_t v = 0; v < obs.mPoly.size(); ++v )
			obsBounds.Expand( obs.mPoly[ v ] );

		obsBounds.ExpandAxis( 2, 32.f );

		obsBounds.Expand( 128.0f );

		FindRuntimeSectors( obsBounds, nearbySectors );

		for ( size_t o = 0; o < nearbySectors.size(); ++o )
			nearbySectors[ o ]->mObstacles.push_back( obs );
	}

	mPendingObstacles.resize( 0 );
}

PathPlannerNavMesh::RuntimeNavSector *PathPlannerNavMesh::GetSectorAt(const Vector3f &_pos, float _distance)
{
	const float CHAR_HALF_HEIGHT = NavigationMeshOptions::CharacterHeight /** 0.5f*/;

	Vector3f raySrc = _pos+Vector3f(0,0,CHAR_HALF_HEIGHT);
	Vector3f rayDst = raySrc - Vector3f::UNIT_Z * _distance;
	NavCollision nc = FindCollision(raySrc, rayDst);
	if(nc.DidHit())
	{
		return &mRuntimeSectors[ nc.HitAttrib().Fields.ActiveId ];
	}
	return NULL;
}

PathPlannerNavMesh::NavCollision PathPlannerNavMesh::FindCollision(const Vector3f &_from, const Vector3f &_to)
{
	if ( m_CollisionData.m_CollisionTree != NULL )
	{
		Vector3f dir = _to - _from;
		const float distance = dir.Normalize();

		IceMaths::Ray ray;
		ray.mOrig.Set( _from.X(), _from.Y(), _from.Z() );
		ray.mDir.Set( dir.X(), dir.Y(), dir.Z() );

		Opcode::CollisionFaces faces;
		Opcode::RayCollider collider;
		collider.SetCulling( false );
		collider.SetDestination( &faces );
		collider.SetMaxDist( distance );
		collider.SetClosestHit( true );
		collider.Collide( ray, *m_CollisionData.m_CollisionTree );

		//udword		mFaceID;				//!< Index of touched face
		//float		mDistance;				//!< Distance from collider to hitpoint
		//float		mU, mV;
		const Opcode::CollisionFace * hitFace = faces.GetFaces();
		if ( hitFace != NULL )
		{
			Opcode::VertexPointers v;
			m_CollisionData.m_CollisionTree->GetMeshInterface()->GetTriangle( v, hitFace->mFaceID );

			/*Vector3List pts( 3 );
			pts.push_back( Vector3f( v.Vertex[ 0 ]->x, v.Vertex[ 0 ]->y, v.Vertex[ 0 ]->z ) );
			pts.push_back( Vector3f( v.Vertex[ 1 ]->x, v.Vertex[ 1 ]->y, v.Vertex[ 1 ]->z ) );
			pts.push_back( Vector3f( v.Vertex[ 2 ]->x, v.Vertex[ 2 ]->y, v.Vertex[ 2 ]->z ) );
			Utils::DrawPolygon( pts, COLOR::MAGENTA, 2.0 );*/

			const PolyAttrib faceAttrib = m_CollisionData.m_TriSectorMap[ hitFace->mFaceID ];

			IceMaths::Point normal = v.Normal();

			return NavCollision( true,
				_from + dir * hitFace->mDistance,
				Vector3f( normal.x, normal.y, normal.z ),
				faceAttrib );
		}
	}
	return NavCollision(false);
}

PathPlannerNavMesh::NavSector * PathPlannerNavMesh::GetSectorAtCursor()
{
	Vector3f vFacing, vPos;
	if ( !Utils::GetLocalEyePosition( vPos ) || !Utils::GetLocalFacing( vFacing ) )
		return NULL;

	NavCollision nc = FindCollision( vPos, vPos + vFacing * 2048.f );
	if( !nc.DidHit() )
		return NULL;

	return &mNavSectors[ nc.HitAttrib().Fields.SectorId ];
}

int PathPlannerNavMesh::FindRuntimeSectors( const AABB & aabb, std::vector<RuntimeNavSector*> & sectorsOut )
{
	sectorsOut.resize( 0 );

	if ( m_CollisionData.m_CollisionTree != NULL )
	{
		Opcode::CollisionAABB colaabb;
		colaabb.SetMinMax(
			IceMaths::Point( aabb.m_Mins ),
			IceMaths::Point( aabb.m_Maxs ) );

		Opcode::AABBCollider collider;
		collider.SetFirstContact( false );

		Opcode::AABBCache boxCache;
		collider.Collide( boxCache, colaabb, *m_CollisionData.m_CollisionTree );

		typedef std::set<RuntimeNavSector*> SectorSet;
		SectorSet	sectorSet;

		for ( udword i = 0; i < collider.GetNbTouchedPrimitives(); ++i )
		{
			const udword primId = collider.GetTouchedPrimitives()[ i ];

			const PolyAttrib faceAttrib = m_CollisionData.m_TriSectorMap[ primId ];
			sectorSet.insert( &mRuntimeSectors[ faceAttrib.Fields.ActiveId ] );
		}

		for ( SectorSet::iterator it = sectorSet.begin();
			it != sectorSet.end();
			++it )
		{
			sectorsOut.push_back( *it );
		}
	}
	return (int)sectorsOut.size();
}

int PathPlannerNavMesh::FindRuntimeSectors( const Box3f & obb, std::vector<RuntimeNavSector*> & sectorsOut )
{
	sectorsOut.resize( 0 );

	if ( m_CollisionData.m_CollisionTree != NULL )
	{
		IceMaths::Matrix3x3 rot;
		rot.SetRow( 0, IceMaths::Point( obb.Axis[0].X(), obb.Axis[0].Y(), obb.Axis[0].Z() ) );
		rot.SetRow( 1, IceMaths::Point( obb.Axis[1].X(), obb.Axis[1].Y(), obb.Axis[1].Z() ) );
		rot.SetRow( 2, IceMaths::Point( obb.Axis[2].X(), obb.Axis[2].Y(), obb.Axis[2].Z() ) );

		IceMaths::OBB iceobb(
			IceMaths::Point( obb.Center.X(), obb.Center.Y(), obb.Center.Z() ),
			IceMaths::Point( obb.Extent[0], obb.Extent[1], obb.Extent[2] ),
			rot );

		Opcode::OBBCollider collider;
		collider.SetFirstContact( false );

		Opcode::OBBCache cache;
		collider.Collide( cache, iceobb, *m_CollisionData.m_CollisionTree );

		typedef std::set<RuntimeNavSector*> SectorSet;
		SectorSet	sectorSet;

		for ( udword i = 0; i < collider.GetNbTouchedPrimitives(); ++i )
		{
			const udword primId = collider.GetTouchedPrimitives()[ i ];

			const PolyAttrib faceAttrib = m_CollisionData.m_TriSectorMap[ primId ];
			sectorSet.insert( &mRuntimeSectors[ faceAttrib.Fields.ActiveId ] );
		}

		for ( SectorSet::iterator it = sectorSet.begin();
			it != sectorSet.end();
			++it )
		{
			sectorsOut.push_back( *it );
		}
	}
	return (int)sectorsOut.size();
}

void PathPlannerNavMesh::ConnectTest()
{
	Vector3f vFacing, vEyePos;
	if( !Utils::GetLocalEyePosition( vEyePos ) || !Utils::GetLocalFacing( vFacing ) )
		return;

	Vector3f vAimPos, vAimNormal;
	if( !Utils::GetLocalAimPoint( vAimPos, &vAimNormal ) )
		return;

	RuntimeNavSector * sector = GetSectorAt( vEyePos );
	if ( sector != NULL )
	{
		sector->BuildSectorPortals( this );
		return;

		sector->mPortals.resize( 0 );

		// look for all neighbors
		const Vector3List & edges0 = sector->mPoly;
		for ( size_t b = 1; b <= edges0.size(); ++b )
		{
			Segment3f seg0( edges0[ b-1 ], edges0[ b % edges0.size() ] );

			{
				DistPoint3Segment3f segDist( vAimPos, seg0 );
				if ( segDist.Get() > 32.0f )
					continue;
			}

			Line3f line0;
			line0.FromPoints( seg0.P0, seg0.P1 );

			const float MAX_STEPUP = std::max( JUMP_HEIGHT, sector->mSectorData->waterdepth() + WATER_EXIT );

			RuntimeSectorRefList nearbySectors;

			Box3f edgeBox;
			edgeBox.Center = vAimPos;

			AABB probeBox( vAimPos );
			probeBox.Expand( 64.0f );
			probeBox.m_Maxs[ 2 ] += MAX_STEPUP;

			FindRuntimeSectors( probeBox, nearbySectors );

			// search all sectors for neighboring connections
			for ( size_t n = 0; n < nearbySectors.size(); ++n )
			{
				RuntimeNavSector * neighbor = nearbySectors[ n ];

				if ( sector == neighbor )
					continue;

				const Vector3List & edges1 = neighbor->mPoly;
				for ( size_t nb = 1; nb <= edges1.size(); ++nb )
				{
					Segment3f seg1( edges1[ nb-1 ], edges1[ nb % edges1.size() ] );

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
					if ( d0.GetLineParameter() > seg0.Extent && d1.GetLineParameter() > seg0.Extent )
						continue;
					if ( d0.GetLineParameter() < -seg0.Extent && d1.GetLineParameter() < -seg0.Extent )
						continue;

					// pull the points into the segment space
					const float d0Segment = Mathf::Clamp( d0.GetLineParameter(), -seg0.Extent, seg0.Extent );
					const float d1Segment = Mathf::Clamp( d1.GetLineParameter(), -seg0.Extent, seg0.Extent );

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
						portal.mDestSector = neighbor->mIndex;

						// do we need to jump to reach this edge?
						if ( p0zDiff > STEP_HEIGHT && p0zDiff <= JUMP_HEIGHT )
						{
							portal.mFlags.SetFlag( NavPortal::FL_JUMP );
						}
						sector->mPortals.push_back( portal );
					}
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////