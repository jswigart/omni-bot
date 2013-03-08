////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "platformspecifics.h"
#include "PathPlannerNavMesh.h"
#include "IGameManager.h"
#include "IGame.h"
#include "GoalManager.h"
#include "NavigationFlags.h"
#include "FileSystem.h"
#include "Path.h"
#include "gmUtilityLib.h"

#include "RenderBuffer.h"

#include "clipper.hpp"
#include "polypartition.h"

// Protocol buffers
#include "google/protobuf/text_format.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"

#include "Opcode.h"
#include "OPC_IceHook.h"

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
		PathPlannerNavMesh::NavSector			*Sector;
		Vector3f								Position;
		PlanNode								*Parent;
		const PathPlannerNavMesh::NavPortal		*Portal;

		int										BoundaryEdge;

		float									CostHeuristic;
		float									CostGiven;
		float									CostFinal;

		obint32 Hash() const
		{
			return (obint32)Utils::MakeId32(
				(obint16)Sector->m_Id,
				Portal ? (obint16)Portal->m_DestSector : (obint16)0xFF);
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
		PathPlannerNavMesh::NavSector *pSector = g_PlannerNavMesh->GetSectorAt(_pos,512.f);
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
		static int NEXT_DRAW = 0;
		if(IGame::GetTime()>NEXT_DRAW)
		{
			NEXT_DRAW = IGame::GetTime() + 2000;
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
	}
	PathFindNavMesh()
	{
		fl.Finished = true;
		m_UsedNodes = 0;
	}
private:
	struct GoalLocation
	{
		PathPlannerNavMesh::NavSector	*Sector;
		Vector3f						Position;
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
	/*static int HashNavNode(const PlanNode *_n1)
	{
	return _n1->Hash;
	}*/

	//typedef boost::fast_pool_allocator< std::pair< const int, PlanNode* >, boost::default_user_allocator_new_delete, boost::details::pool::default_mutex, 769 > HashMapAllocator;
	//typedef FSBAllocator< std::pair< int, PlanNode* > > HashMapAllocator;

#ifdef WIN32
	typedef stdext::hash_compare<int> HashMapCompare;
	typedef stdext::hash_map<int, PlanNode*, HashMapCompare/*, HashMapAllocator*/ > NavHashMap;
#else
	typedef stdext::hash<int> HashMapCompare;
	typedef stdext::hash_map<int, PlanNode*, HashMapCompare, stdext::equal_to<int>/*, HashMapAllocator*/ > NavHashMap;
#endif
	NodeList			m_OpenList;
	NodeList			m_Solution;

	//typedef std::multimap<int,PlanNode*> NodeClosedList; // temp?
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
		//m_ClosedList.insert(std::make_pair(_node->Sector->m_Id,_node));
	}
	NodeClosedList::iterator IsOnClosedList(PlanNode *_node)
	{
		NodeClosedList::iterator it = m_ClosedList.find(_node->Hash());
		return it;
		/*NodeClosedList::iterator it = m_ClosedList.lower_bound(_node->Sector->m_Id);
		NodeClosedList::iterator itEnd = m_ClosedList.upper_bound(_node->Sector->m_Id);
		while(it != itEnd)
		{
		if(SquaredLength(_node->Position,it->second->Position) < 32.f)
		return it;
		++it;
		}
		return m_ClosedList.end();*/
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

		for(int p = _node->Sector->m_StartPortal;
			p < _node->Sector->m_StartPortal+_node->Sector->m_NumPortals;
			++p)
		{
			const PathPlannerNavMesh::NavPortal &portal = g_PlannerNavMesh->m_NavPortals[p];

			//for(int b = 0; b < )

			//////////////////////////////////////////////////////////////////////////
			PlanNode tmpNext;
			tmpNext.Reset();
			tmpNext.Portal = &portal;
			tmpNext.Parent = _node;
			tmpNext.Sector = &g_PlannerNavMesh->m_ActiveNavSectors[portal.m_DestSector];
			tmpNext.Position = portal.m_Segment.Origin; // TODO: branch more

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

PathFindNavMesh g_PathFind;

PathPlannerNavMesh::NavCollision PathPlannerNavMesh::FindCollision(const Vector3f &_from, const Vector3f &_to)
{
	if ( m_CollisionData.m_CollisionTree != NULL )
	{
		Vector3f dir = _to - _from;
		const float distance = dir.Normalize() * 16.0f;

		IceMaths::Ray ray;
		ray.mOrig.Set( _from.x, _from.y, _from.z );
		ray.mDir.Set( dir.x, dir.y, dir.z );

		Opcode::CollisionFaces faces;
		Opcode::RayCollider rayCol;
		rayCol.SetDestination( &faces );
		rayCol.SetMaxDist( distance );
		rayCol.SetClosestHit( true );
		rayCol.Collide( ray, *m_CollisionData.m_CollisionTree );

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

PathPlannerNavMesh::NavSector *PathPlannerNavMesh::GetSectorAt(const Vector3f &_pos, float _distance)
{
	const float CHAR_HALF_HEIGHT = NavigationMeshOptions::CharacterHeight /** 0.5f*/;

	Vector3f raySrc = _pos+Vector3f(0,0,CHAR_HALF_HEIGHT);
	Vector3f rayDst = raySrc - Vector3f::UNIT_Z * _distance;
	NavCollision nc = FindCollision(raySrc, rayDst);
	if(nc.DidHit())
	{
		return &m_ActiveNavSectors[ nc.HitAttrib().Fields.ActiveId ];
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////

PathPlannerNavMesh::PathPlannerNavMesh()
{
	// TEMP
	m_PlannerFlags.SetFlag(NAV_VIEW);
	m_PlannerFlags.SetFlag(NAVMESH_STEPPROCESS);

	m_CursorColor = COLOR::BLUE;

	m_MapCenter = Vector3f::ZERO;

	m_ToolCancelled = false;

	m_SpanMap = NULL;
	m_Influence = NULL;

	g_PlannerNavMesh = this;

	mInfluenceBufferId = 0;
}

PathPlannerNavMesh::~PathPlannerNavMesh()
{
	Shutdown();
	g_PlannerNavMesh = 0;
}

Vector3f PathPlannerNavMesh::NavSector::CalculateCenter() const
{
	Vector3f center = Vector3f::ZERO;
	if ( m_Boundary.size() > 0 )
	{
		center = m_Boundary[ 0 ];
		for ( size_t i = 1; i < m_Boundary.size(); ++i )
			center += m_Boundary[ i ];
		center /= (float)m_Boundary.size();
	}
	return center;
}

PathPlannerNavMesh::NavSector PathPlannerNavMesh::NavSector::GetMirroredCopy(const Vector3f &offset) const
{
	NavSector ns;
	ns.m_Mirror = NavmeshIO::Sector_MirrorDir_MirrorNone;

	Vector3f vAxis;
	switch(m_Mirror)
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
	ns.m_Boundary = m_Boundary;
	for(obuint32 m = 0; m < ns.m_Boundary.size(); ++m)
	{
		ns.m_Boundary[m] -= offset;
		ns.m_Boundary[m] = mat * ns.m_Boundary[m];
		ns.m_Boundary[m] += offset;
	}
	return ns;
}

void PathPlannerNavMesh::NavSector::GetEdgeSegments(SegmentList &_list) const
{
	for(obuint32 m = 0; m < m_Boundary.size()-1; ++m)
	{
		_list.push_back(Utils::MakeSegment(m_Boundary[m],m_Boundary[m+1]));
	}
	_list.push_back(Utils::MakeSegment(m_Boundary[m_Boundary.size()-1],m_Boundary[0]));
}

SegmentList PathPlannerNavMesh::NavSector::GetEdgeSegments() const
{
	SegmentList lst;
	GetEdgeSegments(lst);
	return lst;
}

bool PathPlannerNavMesh::Init()
{
	InitCommands();
	return true;
}

static void DrawPoly( bool solid, const PathPlannerNavMesh::NavSector & ns, const TPPLPoly & poly, float offset, obColor color, float dur, float vertSize )
{
	Vector3List polyList;
	for ( int i = 0; i < poly.GetNumPoints(); ++i )
	{
		Vector3f vec3d( (float)poly[ i ].x, (float)poly[ i ].y, 0.0f );
		ns.m_Plane.DropToPlane( vec3d, -Vector3f::UNIT_Z );
		vec3d.z += offset;
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

static void DrawPoly( bool solid, const PathPlannerNavMesh::NavSector & ns, const ClipperLib::Polygon & poly, float offset, obColor color, float dur, float vertSize )
{
	Vector3List polyList;
	for ( ClipperLib::Polygon::const_iterator it = poly.begin();
		it != poly.end();
		++it )
	{
		const ClipperLib::IntPoint & pt = *it;
		Vector3f vec3d( (float)pt.X, (float)pt.Y, 0.0f );
		ns.m_Plane.DropToPlane( vec3d, -Vector3f::UNIT_Z );
		vec3d.z += offset;
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

void PathPlannerNavMesh::Update()
{
	Prof(PathPlannerNavMesh);

	UpdateFsm(IGame::GetDeltaTimeSecs());

	if(m_PlannerFlags.CheckFlag(NAV_VIEW))
	{
		g_PathFind.Render();

		//////////////////////////////////////////////////////////////////////////
		Vector3f vLocalPos, vLocalAim, vAimPos, vAimNormal;
		Utils::GetLocalEyePosition(vLocalPos);
		Utils::GetLocalFacing(vLocalAim);
		if(Utils::GetLocalAimPoint(vAimPos, &vAimNormal))
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

		NavSector nsMirrored;

		static int iLastSector = -1;

		static int iNextCurSectorTimeUpdate = 0;
		if(iCurrentSector != -1 &&
			(iLastSector != iCurrentSector || IGame::GetTime() >= iNextCurSectorTimeUpdate))
		{
			iLastSector = iCurrentSector;
			iNextCurSectorTimeUpdate = IGame::GetTime() + 500;
			//Utils::DrawPolygon(m_ActiveNavSectors[iCurrentSector].m_Boundary, COLOR::RED, 0.5f, false);
		}

		static int iNextTimeUpdate = 0;
		if(IGame::GetTime() >= iNextTimeUpdate)
		{
			iNextTimeUpdate = IGame::GetTime() + 0;//2000;
			//////////////////////////////////////////////////////////////////////////
			// Draw our nav sectors
			for(obuint32 i = 0; i < m_ActiveNavSectors.size(); ++i)
			{
				const NavSector &ns = m_ActiveNavSectors[i];

				// mirrored sectors are contiguous at the end of
				// the m_ActiveNavSectors list, so draw them differently
				obColor SEC_COLOR = COLOR::GREEN;
				if ( i >= m_NavSectors.size() )
					SEC_COLOR = COLOR::CYAN;

				//////////////////////////////////////////////////////////////////////////
				// Draw sector obstacles
				if ( ns.m_Obstacles.size() > 0 )
				{
					ClipperLib::Polygon sectorPoly;
					for ( size_t b = 0; b < ns.m_Boundary.size(); ++b )
					{
						sectorPoly.push_back( ClipperLib::IntPoint(
							(ClipperLib::long64)ns.m_Boundary[b].x,
							(ClipperLib::long64)ns.m_Boundary[b].y ) );
					}

					ClipperLib::Polygons obstaclePolys;
					for ( size_t o = 0; o < ns.m_Obstacles.size(); ++o )
					{
						obstaclePolys.push_back( ClipperLib::Polygon() );

						ClipperLib::Polygon & obs = obstaclePolys.back();
						for ( size_t ov = 0; ov < ns.m_Obstacles[o].mPolyVerts.size(); ++ov )
						{
							obs.push_back( ClipperLib::IntPoint(
								(ClipperLib::long64)ns.m_Obstacles[o].mPolyVerts[ ov ].x,
								(ClipperLib::long64)ns.m_Obstacles[o].mPolyVerts[ ov ].y ) );
						}
					}

					//////////////////////////////////////////////////////////////////////////
					// generate the union of the obstacles in case there is overlap
					//static bool mergeObstacles = false;
					//if ( mergeObstacles && obstaclePolys.size() > 0 )
					//{
					//	static ClipperLib::ClipType ct = ClipperLib::ctUnion;
					//	static ClipperLib::PolyFillType pft = ClipperLib::pftNonZero;

					//	ClipperLib::Clipper obsUnion;
					//	obsUnion.AddPolygon( obstaclePolys[0], ClipperLib::ptSubject );
					//	obsUnion.AddPolygons( obstaclePolys, ClipperLib::ptClip );
					//	if ( obsUnion.Execute( ct, pft ) )
					//	{
					//		obstaclePolys.resize( 0 );

					//		ClipperLib::Polygon poly;
					//		for ( size_t i = 0; i < obsUnion.GetNumPolysOut(); ++i )
					//		{
					//			bool polyIsHole = false;
					//			if ( obsUnion.GetPolyOut( i, poly, polyIsHole ) )
					//			{
					//				//assert( ClipperLib::Orientation( poly ) );
					//				if ( !ClipperLib::Orientation( poly ) )
					//				{
					//					DrawPoly( true, ns, poly, 12, COLOR::WHITE, 2.0f, 4.0f );
					//				}
					//				obstaclePolys.push_back( poly );
					//			}
					//		}
					//	}
					//}
					//////////////////////////////////////////////////////////////////////////
					assert ( ClipperLib::Orientation( sectorPoly ) );

					static ClipperLib::ClipType ct = ClipperLib::ctDifference;
					static ClipperLib::PolyFillType pft = ClipperLib::pftNonZero;
					static ClipperLib::PolyFillType cft = ClipperLib::pftNonZero;

					ClipperLib::Clipper obsNav;
					obsNav.AddPolygon( sectorPoly, ClipperLib::ptSubject );
					obsNav.AddPolygons( obstaclePolys, ClipperLib::ptClip );
					if ( obsNav.Execute( ct, pft, cft ) )
					{
						typedef std::list<TPPLPoly> PolyList;
						PolyList inputPolys;
						PolyList subPolys;

						// add all the solution polygons to the input polygons
						// of the partition list
						ClipperLib::Polygon poly;
						for ( size_t p = 0; p < obsNav.GetNumPolysOut(); ++p )
						{
							bool polyIsHole = false;
							if ( obsNav.GetPolyOut( p, poly, polyIsHole ) )
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
									ns.m_Plane.DropToPlane( vec3d, -Vector3f::UNIT_Z );
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
								for ( int p = 0; p < ply.GetNumPoints(); ++p )
								{
									Vector3f vec3d( (float)ply[ p ].x, (float)ply[ p ].y, 0.0f );
									ns.m_Plane.DropToPlane( vec3d, -Vector3f::UNIT_Z );
								}

								DrawPoly( true, ns, ply, 0.0f, SEC_COLOR.fade(100), 2.0f, 4.0f );
								DrawPoly( false, ns, ply, 0.0f, SEC_COLOR, 2.0f, 4.0f );
							}
						}
						else
						{
							const NavSector &ns = m_ActiveNavSectors[i];

							RenderBuffer::AddLine(ns.m_Boundary, COLOR::RED.fade( 100 ));

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
					//RenderBuffer::AddPolygonFilled( ns.m_Boundary, SEC_COLOR.fade(100) );
					/*RenderBuffer::AddPolygonSilouette( ns.m_Boundary, SEC_COLOR );
					RenderBuffer::AddPoints( ns.m_Boundary, COLOR::MAGENTA, 5.0f );*/
				}
			}

			if(m_PlannerFlags.CheckFlag(NAV_VIEWCONNECTIONS))
			{
				NavPortalList ::const_iterator pIt = m_NavPortals.begin();
				for(; pIt != m_NavPortals.end(); ++pIt)
				{
					const NavPortal &portal = (*pIt);

					/*RenderBuffer::AddLine(portal.m_Segment.GetPosEnd(),
					portal.m_Segment.Origin+Vector3f(0,0,32),COLOR::MAGENTA,10.f);
					RenderBuffer::AddLine(portal.m_Segment.GetNegEnd(),
					portal.m_Segment.Origin+Vector3f(0,0,32),COLOR::MAGENTA,10.f);*/

					RenderBuffer::AddLine( portal.m_Segment.GetPosEnd(),
						portal.m_Segment.Origin+Vector3f(0,0,32),COLOR::MAGENTA );
					RenderBuffer::AddLine( portal.m_Segment.GetNegEnd(),
						portal.m_Segment.Origin+Vector3f(0,0,32),COLOR::MAGENTA );
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
		}
		//////////////////////////////////////////////////////////////////////////

		if(!m_WorkingSector.m_Boundary.empty())
		{
			//Utils::DrawPolygon(m_WorkingSector.m_Boundary, COLOR::GREEN.fade(100), 0.1f, false);
			RenderBuffer::AddPolygonFilled( m_WorkingSector.m_Boundary, COLOR::GREEN );
			if ( m_WorkingSector.IsMirrored() )
				RenderBuffer::AddPolygonFilled( m_WorkingSector.GetMirroredCopy(m_MapCenter).m_Boundary, COLOR::CYAN );
			//Utils::DrawPolygon(m_WorkingSector.GetMirroredCopy(m_MapCenter).m_Boundary, COLOR::CYAN.fade(100), 0.1f, false);
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

	m_ActiveNavSectors.clear();
	m_NavSectors.clear();

	try
	{
		using namespace google::protobuf;

		const std::string navPathText	= std::string("nav/") + _mapname + ".nm";
		const std::string navPathBinary	= std::string("nav/") + _mapname + ".nmb";

		NavmeshIO::NavigationMesh nm;

		std::string fileContents;

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

		m_MapCenter.x = nm.header().mapcenter().x();
		m_MapCenter.y = nm.header().mapcenter().y();
		m_MapCenter.z = nm.header().mapcenter().z();

		m_NavSectors.clear();
		m_NavSectors.reserve( nm.sectors_size() );

		for ( int i = 0; i < nm.sectors_size(); ++i )
		{
			const NavmeshIO::Sector & s = nm.sectors( i );

			NavSector sector;
			sector.m_Mirror = s.sectormirrored();
			if ( s.has_sectordata() )
			{
			}
			if ( s.has_sectordatamirrored() )
			{
			}

			for ( int i = 0; i < s.vertices_size(); ++i )
			{
				const NavmeshIO::SectorVert & v = s.vertices( i );
				sector.m_Boundary.push_back( Vector3f(
					v.position().x(),
					v.position().y(),
					v.position().z() ) );
			}

			m_NavSectors.push_back( sector );
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
		nm.mutable_header()->mutable_mapcenter()->set_x( m_MapCenter.x );
		nm.mutable_header()->mutable_mapcenter()->set_y( m_MapCenter.y );
		nm.mutable_header()->mutable_mapcenter()->set_z( m_MapCenter.z );
	}

	for(obuint32 s = 0; s < m_NavSectors.size(); ++s)
	{
		const NavSector &ns = m_NavSectors[s];

		NavmeshIO::Sector* sectorOut = nm.add_sectors();
		for(obuint32 v = 0; v < ns.m_Boundary.size(); ++v)
		{
			NavmeshIO::SectorVert * sectorVertOut = sectorOut->add_vertices();
			sectorVertOut->mutable_position()->set_x( ns.m_Boundary[v].x );
			sectorVertOut->mutable_position()->set_y( ns.m_Boundary[v].y );
			sectorVertOut->mutable_position()->set_z( ns.m_Boundary[v].z );

			/*sectorOut->mutable_sectordata()
			sectorOut->mutable_sectordatamirrored()*/
			if ( ns.m_Mirror != NavmeshIO::Sector_MirrorDir_MirrorNone )
			{
				sectorOut->set_sectormirrored( ns.m_Mirror );
				//sectorOut->mutable_sectordatamirrored();
			}
		}
	}

	std::string outBuffer;

	using namespace google::protobuf;
	io::StringOutputStream outputStream( &outBuffer );
	io::CodedOutputStream codedStream( &outputStream );

	std::string serializeBinary, serializeText;

	try
	{
		nm.SerializeToString( &serializeBinary );
		TextFormat::PrintToString( nm, &serializeText );

		const std::string navPathText	= std::string("nav/") + _mapname + ".nm";
		const std::string navPathBinary	= std::string("nav/") + _mapname + ".nmb";

		File outFileBinary;
		if ( outFileBinary.OpenForWrite(navPathBinary.c_str(), File::Binary) )
		{
			outFileBinary.Write( serializeBinary.c_str(), serializeBinary.length(), 1 );
			outFileBinary.Close();
		}

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
	return !m_ActiveNavSectors.empty();
}

bool PathPlannerNavMesh::GetNavFlagByName(const std::string &_flagname, NavFlags &_flag) const
{
	_flag = 0;
	return false;
}

Vector3f PathPlannerNavMesh::GetRandomDestination(Client *_client, const Vector3f &_start, const NavFlags _team)
{
	Vector3f dest = _start;

	if(!m_ActiveNavSectors.empty())
	{
		const NavSector &randSector = m_ActiveNavSectors[rand()%m_ActiveNavSectors.size()];
		dest = Utils::AveragePoint(randSector.m_Boundary);
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
	OB_DELETE( m_Influence );
	OB_DELETE( m_SpanMap );

	m_CollisionData.Free();
}

void PathPlannerNavMesh::InitSectors()
{
	m_CollisionData.Free();

	m_ActiveNavSectors.clear();

	NavSectorList					allSectors;

	std::vector<IceMaths::Point>	vertices;
	std::vector<PolyAttrib>			triSectors;

	// create the active sector list and the collision data at the same time so we can create a proper mapping
	for ( size_t i = 0; i < m_NavSectors.size(); ++i )
	{
		PolyAttrib attr;
		attr.Fields.ActiveId = allSectors.size();
		attr.Fields.SectorId = i;
		attr.Fields.Mirrored = 0;

		allSectors.push_back( m_NavSectors[ i ] );
		allSectors.back().m_Id = attr.Fields.SectorId;

		//////////////////////////////////////////////////////////////////////////
		Vector3f center = allSectors.back().CalculateCenter();
		for ( size_t v = 1; v <= allSectors.back().m_Boundary.size(); ++v )
		{
			triSectors.push_back( attr );

			vertices.push_back( IceMaths::Point(
				center.x, center.y, center.z ) );

			vertices.push_back( IceMaths::Point(
				allSectors.back().m_Boundary[ v-1 ].x,
				allSectors.back().m_Boundary[ v-1 ].y,
				allSectors.back().m_Boundary[ v-1 ].z ) );

			vertices.push_back( IceMaths::Point(
				allSectors.back().m_Boundary[ v % allSectors.back().m_Boundary.size() ].x,
				allSectors.back().m_Boundary[ v % allSectors.back().m_Boundary.size() ].y,
				allSectors.back().m_Boundary[ v % allSectors.back().m_Boundary.size() ].z ) );

			if ( v == 1 )
			{
				allSectors.back().m_Plane = Plane3f(
					center,
					allSectors.back().m_Boundary[ v-1 ],
					allSectors.back().m_Boundary[ v ] );
			}
		}
	}

	// do another pass for the mirrored set
	for ( size_t i = 0; i < m_NavSectors.size(); ++i )
	{
		if ( m_NavSectors[ i ].m_Mirror != NavmeshIO::Sector_MirrorDir_MirrorNone )
		{
			PolyAttrib attr;
			attr.Fields.ActiveId = allSectors.size();
			attr.Fields.SectorId = i;
			attr.Fields.Mirrored = 1;

			allSectors.push_back( m_NavSectors[ i ].GetMirroredCopy( m_MapCenter ) );
			allSectors.back().m_Id = attr.Fields.SectorId;

			//////////////////////////////////////////////////////////////////////////
			Vector3f center = allSectors.back().CalculateCenter();
			for ( size_t v = 1; v <= allSectors.back().m_Boundary.size(); ++v )
			{
				triSectors.push_back( attr );

				vertices.push_back( IceMaths::Point(
					center.x, center.y, center.z ) );

				vertices.push_back( IceMaths::Point(
					allSectors.back().m_Boundary[ v-1 ].x,
					allSectors.back().m_Boundary[ v-1 ].y,
					allSectors.back().m_Boundary[ v-1 ].z ) );

				vertices.push_back( IceMaths::Point(
					allSectors.back().m_Boundary[ v % allSectors.back().m_Boundary.size() ].x,
					allSectors.back().m_Boundary[ v % allSectors.back().m_Boundary.size() ].y,
					allSectors.back().m_Boundary[ v % allSectors.back().m_Boundary.size() ].z ) );

				if ( v == 1 )
				{
					allSectors.back().m_Plane = Plane3f(
						center,
						allSectors.back().m_Boundary[ v-1 ],
						allSectors.back().m_Boundary[ v ] );
				}
			}
			//////////////////////////////////////////////////////////////////////////
		}
	}

	m_ActiveNavSectors.swap( allSectors );

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
		return;
	}
}

void PathPlannerNavMesh::RegisterGameGoals()
{
}

void PathPlannerNavMesh::GetPath(Path &_path, int _smoothiterations)
{
	const float CHAR_HALF_HEIGHT = NavigationMeshOptions::CharacterHeight * 0.75f;

	PathFindNavMesh::NodeList &nl = g_PathFind.GetSolution();

	//////////////////////////////////////////////////////////////////////////
	if(nl.size() > 2)
	{
		for(int i = 0; i < _smoothiterations; ++i)
		{
			bool bSmoothed = false;

			// solution is goal to start
			for(obuint32 n = 1; n < nl.size()-1; ++n)
			{
				PathFindNavMesh::PlanNode *pFrom = nl[n+1];
				PathFindNavMesh::PlanNode *pTo = nl[n-1];
				PathFindNavMesh::PlanNode *pMid = nl[n];
				if(!pMid->Portal /*|| pMid->Portal->m_LinkFlags & teleporter*/)
					continue;

				Segment3f portalSeg = pMid->Portal->m_Segment;
				portalSeg.Extent -= 32.f;
				Segment3f seg = Utils::MakeSegment(pFrom->Position,pTo->Position);
				//DistancePointToLine(_seg1.Origin,_seg2.GetNegEnd(),_seg2.GetPosEnd(),&cp);

				Vector3f intr;
				if(Utils::intersect2D_Segments(seg,portalSeg,&intr))
				{
					// adjust the node position
					if(SquaredLength(intr,pMid->Position) > Mathf::Sqr(16.f))
					{
						//RenderBuffer::AddLine(pMid->Position+Vector3f(0,0,32),intr,COLOR::YELLOW,15.f);
						bSmoothed = true;
						pMid->Position = intr;
					}
				}
			}

			if(!bSmoothed)
				break;
		}
	}
	//////////////////////////////////////////////////////////////////////////

	while(!nl.empty())
	{
		PathFindNavMesh::PlanNode *pn = nl.back();
		Vector3f vNodePos = pn->Position;

		_path.AddPt(vNodePos + Vector3f(0,0,CHAR_HALF_HEIGHT),32.f)
			.NavId(pn->Sector->m_Id)
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

void PathPlannerNavMesh::AddEntityConnection(const Event_EntityConnection &_conn)
{
}

void PathPlannerNavMesh::RemoveEntityConnection(GameEntity _ent)
{
}

Vector3f PathPlannerNavMesh::GetDisplayPosition(const Vector3f &_pos)
{
	return _pos;
}
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////