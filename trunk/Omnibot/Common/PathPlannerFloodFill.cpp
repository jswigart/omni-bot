////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "PathPlannerFloodFill.h"
#include "IGameManager.h"
#include "IGame.h"
#include "GoalManager.h"
#include "NavigationFlags.h"
#include "Timer.h"
#include "FileSystem.h"
#include "gmUtilityLib.h"

#include "RenderBuffer.h"

using namespace std;

float g_CharacterHeight = 64.0f;
float g_CharacterCrouchHeight = 48.0f;
float g_CharacterStepHeight = 18.0f;
float g_CharacterJumpHeight = 60.0f;
float g_GridRadius = 16.0f;

//////////////////////////////////////////////////////////////////////////

PathPlannerFloodFill::PathPlannerFloodFill()
{
	// TEMP
	m_PlannerFlags.SetFlag(NAV_VIEW);
	m_PlannerFlags.SetFlag(NAVMESH_STEPPROCESS);

	m_CursorColor = COLOR::BLUE;
}

PathPlannerFloodFill::~PathPlannerFloodFill()
{
	Shutdown();
}

bool PathPlannerFloodFill::Init( System & system )
{
	InitCommands();
	return true;
}

void PathPlannerFloodFill::RegisterNavFlag(const std::string &_name, const NavFlags &_bits)
{
}

void PathPlannerFloodFill::RegisterScriptFunctions(gmMachine *a_machine)
{
}

void PathPlannerFloodFill::Update( System & system )
{
	Prof(PathPlannerFloodFill);

	//UpdateFsm(IGame::GetDeltaTimeSecs());

	if(m_PlannerFlags.CheckFlag(NAV_VIEW))
	{
		//////////////////////////////////////////////////////////////////////////
		Vector3f vLocalPos, vLocalAim, vAimPos, vAimNormal;
		Utils::GetLocalEyePosition(vLocalPos);
		Utils::GetLocalFacing(vLocalAim);
		if(Utils::GetLocalAimPoint(vAimPos, &vAimNormal))
		{
			RenderBuffer::AddLine(vAimPos,
				vAimPos + vAimNormal * 16.f,
				m_CursorColor,
				IGame::GetDeltaTimeSecs()*2.f);

			RenderBuffer::AddLine(vAimPos,
				vAimPos + vAimNormal.Perpendicular() * 16.f, m_CursorColor,
				IGame::GetDeltaTimeSecs()*2.f);
		}
		m_CursorColor = COLOR::BLUE; // back to default

		//////////////////////////////////////////////////////////////////////////

		Vector3List::const_iterator cIt = m_StartPositions.begin();
		for(; cIt != m_StartPositions.end(); ++cIt)
		{
			AABB aabb(Vector3f::ZERO);
			aabb.Expand(Vector3f(g_GridRadius, g_GridRadius, 0.0f));
			aabb.Expand(Vector3f(-g_GridRadius, -g_GridRadius, 0.0f));
			aabb.m_Maxs[2] = g_CharacterHeight - g_CharacterStepHeight;
			aabb.Translate(*cIt);
			RenderBuffer::AddAABB(aabb, COLOR::BLACK);
		}

		_Render();
	}
}

void PathPlannerFloodFill::_Render()
{
	Prof(RenderFloodFill);

	GameEntity ge = Utils::GetLocalEntity();
	if(!ge.IsValid())
		return;
}

void PathPlannerFloodFill::Shutdown()
{
	Unload();
}

bool PathPlannerFloodFill::Load(const std::string &_mapname, bool _dl)
{
	if(_mapname.empty())
		return false;

	/*gmMachine *pM = new gmMachine;
	pM->SetDebugMode(true);
	DisableGCInScope gcEn(pM);

	std::string waypointName		= _mapname + ".nav";

	File InFile;

	char strbuffer[1024] = {};
	sprintf(strbuffer, "user/%s", waypointName.c_str());
	InFile.OpenForRead(strbuffer, File::Binary);
	if(InFile.IsOpen())
	{
	obuint32 fileSize = (obuint32)InFile.FileLength();
	boost::shared_array<char> pBuffer(new char[fileSize+1]);

	InFile.Read(pBuffer.get(), fileSize);
	pBuffer[fileSize] = 0;
	InFile.Close();

	int errors = pM->ExecuteString(pBuffer.get());
	if(errors)
	{
	ScriptManager::LogAnyMachineErrorMessages(pM);
	delete pM;
	return false;
	}
	}*/

	return false;
}

bool PathPlannerFloodFill::Save(const std::string &_mapname)
{
	if(_mapname.empty())
		return false;

	//std::string waypointName		= _mapname + ".nav";
	//std::string navPath	= std::string("nav/") + waypointName;

	/*gmMachine *pM = new gmMachine;
	pM->SetDebugMode(true);
	DisableGCInScope gcEn(pM);

	gmTableObject *pNavTbl = pM->AllocTableObject();
	pM->GetGlobals()->Set(pM, "Navigation", gmVariable(pNavTbl));

	pNavTbl->Set(pM,"MapCenter",gmVariable(Vector3f::ZERO));

	gmTableObject *pSectorsTable = pM->AllocTableObject();
	pNavTbl->Set(pM, "Sectors", gmVariable(pSectorsTable));

	Vector3List vlist;
	vlist.reserve(4);

	gmUtility::DumpTable(pM,waypointName.c_str(),"Navigation",gmUtility::DUMP_ALL);
	delete pM;*/

	return false;
}

bool PathPlannerFloodFill::IsReady() const
{
	return true;//!mRuntimeSectors.empty();
}

bool PathPlannerFloodFill::GetNavFlagByName(const std::string &_flagname, NavFlags &_flag) const
{
	_flag = 0;
	return false;
}

Vector3f PathPlannerFloodFill::GetRandomDestination(Client *_client, const Vector3f &_start, const NavFlags _team)
{
	Vector3f dest = _start;

	/*if(!mRuntimeSectors.empty())
	{
	const NavSector &randSector = mRuntimeSectors[rand()%mRuntimeSectors.size()];
	dest = Utils::AveragePoint(randSector.m_Boundary);
	}*/
	return dest;
}

//////////////////////////////////////////////////////////////////////////

int PathPlannerFloodFill::PlanPathToNearest(Client *_client, const Vector3f &_start, const Vector3List &_goals, const NavFlags &_team)
{
	DestinationVector dst;
	for(obuint32 i = 0; i < _goals.size(); ++i)
		dst.push_back(Destination(_goals[i],32.f));
	return PlanPathToNearest(_client,_start,dst,_team);
}

int PathPlannerFloodFill::PlanPathToNearest(Client *_client, const Vector3f &_start, const DestinationVector &_goals, const NavFlags &_team)
{
	/*g_PathFinder.StartNewSearch();
	g_PathFinder.AddStart(_start);

	for(obuint32 i = 0; i < _goals.size(); ++i)
	g_PathFinder.AddGoal(_goals[i].m_Position);

	while(!g_PathFinder.IsFinished())
	g_PathFinder.Iterate();

	return g_PathFinder.GetGoalIndex();*/
	return -1;
}

void PathPlannerFloodFill::PlanPathToGoal(Client *_client, const Vector3f &_start, const Vector3f &_goal, const NavFlags _team)
{
	DestinationVector dst;
	dst.push_back(Destination(_goal,32.f));
	PlanPathToNearest(_client,_start,dst,_team);
}

bool PathPlannerFloodFill::IsDone() const
{
	//return g_PathFinder.IsFinished();
	return false;
}
bool PathPlannerFloodFill::FoundGoal() const
{
	//return g_PathFinder.FoundGoal();
	return false;
}

void PathPlannerFloodFill::Unload()
{
}

void PathPlannerFloodFill::RegisterGameGoals()
{
}

void PathPlannerFloodFill::GetPath(Path &_path)
{
	//const float CHAR_HALF_HEIGHT = g_CharacterHeight * 0.75f;

	//PathFindFloodFill::NodeList &nl = g_PathFinder.GetSolution();

	//////////////////////////////////////////////////////////////////////////
	//if(nl.size() > 2)
	//{
	//	for(int i = 0; i < _smoothiterations; ++i)
	//	{
	//		bool bSmoothed = false;

	//		// solution is goal to start
	//		for(obuint32 n = 1; n < nl.size()-1; ++n)
	//		{
	//			PathFind::PlanNode *pFrom = nl[n+1];
	//			PathFind::PlanNode *pTo = nl[n-1];
	//			PathFind::PlanNode *pMid = nl[n];
	//			if(!pMid->Portal /*|| pMid->Portal->m_LinkFlags & teleporter*/)
	//				continue;

	//			Segment3f portalSeg = pMid->Portal->m_Segment;
	//			portalSeg.Extent -= 32.f;
	//			Segment3f seg = Utils::MakeSegment(pFrom->Position,pTo->Position);
	//			//DistancePointToLine(_seg1.Origin,_seg2.GetNegEnd(),_seg2.GetPosEnd(),&cp);

	//			Vector3f intr;
	//			if(Utils::intersect2D_Segments(seg,portalSeg,&intr))
	//			{
	//				// adjust the node position
	//				if(SquaredLength(intr,pMid->Position) > Mathf::Sqr(16.f))
	//				{
	//					//RenderBuffer::AddLine(pMid->Position+Vector3f(0,0,32),intr,COLOR::YELLOW,15.f);
	//					bSmoothed = true;
	//					pMid->Position = intr;
	//				}
	//			}
	//		}

	//		if(!bSmoothed)
	//			break;
	//	}
	//}
	//////////////////////////////////////////////////////////////////////////

	//while(!nl.empty())
	//{
	//	Vector3f vNodePos = nl.back()->Position;

	//	_path.AddPt(vNodePos + Vector3f(0,0,CHAR_HALF_HEIGHT),32.f)
	//		/*.Flags(m_Solution.back()->GetNavigationFlags())
	//		.OnPathThrough(m_Solution.back()->OnPathThrough())
	//		.OnPathThroughParam(m_Solution.back()->OnPathThroughParam())*/;

	//	nl.pop_back();
	//}
}

static bool GetGroundPos( const Vector3f & pos, Vector3f & groundPosOut )
{
	obTraceResult tr;
	EngineFuncs::TraceLine(tr,
		pos,
		pos - Vector3f::UNIT_Z * 100000.0f,
		NULL/*&bounds*/,
		TR_MASK_FLOODFILL,
		-1,
		False);
	if ( tr.m_Fraction > 0.0f && tr.m_Fraction < 1.0f )
	{
		groundPosOut = tr.m_Endpos;
		return true;
	}
	return false;
}

static bool GetHeightAtPos( const Vector3f & pos, float & height )
{
	obTraceResult tr;
	EngineFuncs::TraceLine(tr,
		pos + Vector3f( 0.0f, 0.0f, 1.0f ),
		pos + Vector3f::UNIT_Z * 100000.0f,
		NULL/*&bounds*/,
		TR_MASK_FLOODFILL,
		-1,
		False);
	if ( tr.m_Fraction > 0.0f && tr.m_Fraction < 1.0f )
	{
		height = tr.m_Endpos[ 2 ] - pos.Z();
		return true;
	}
	return false;
}

int PathPlannerFloodFill::Process_FloodFill()
{
	Prof(Process_FloodFill);

	enum FloodFillStatus
	{
		Process_PrepareData,
		Process_FloodFill,
		Process_FloodBorder,
		Process_MergeSectors,
		Process_MakeRegions,
		Process_ConnectSectors,
		Process_Cleanup
	};

	//////////////////////////////////////////////////////////////////////////
	static FloodFillStatus status = Process_PrepareData;
	/*if(!m_FloodFillData)
	status = Process_PrepareData;*/

	switch(status)
	{
	case Process_PrepareData:
		{
			mSpanMap.Clear();

			for(obuint32 i = 0; i < m_StartPositions.size(); ++i)
			{
				float gHeight = 0.0f;
				Vector3f gPos;
				if ( GetGroundPos( m_StartPositions[ i ], gPos ) && GetHeightAtPos( gPos, gHeight ) )
				{
					mSpanMap.AddOpenSpan( gPos, gHeight );
				}
			}
			status = Process_FloodFill;

			EngineFuncs::ConsoleMessage("Initializing Flood Fill.");
			break;
		}
	case Process_FloodFill:
		{
			Prof(FloodFill);

			Timer tme;
			while(true)
			{
				//////////////////////////////////////////////////////////////////////////
				//NavNode *pNavNode = _GetNextOpenNode();
				//if(!pNavNode)
				//{
				//	if(!m_PlannerFlags.CheckFlag(NAVMESH_STEPPROCESS) ||
				//		m_PlannerFlags.CheckFlag(NAVMESH_TAKESTEP))
				//	{
				//		m_PlannerFlags.ClearFlag(NAVMESH_TAKESTEP);
				//		EngineFuncs::ConsoleMessage("Tagging node border distance.");
				//		status = Process_FloodBorder;
				//		//status = Process_Recast;
				//	}
				//	break;
				//}
				////////////////////////////////////////////////////////////////////////////
				//// Overflow protection
				//if(m_FloodFillData->m_FaceIndex >= NUM_FLOOD_NODES)
				//{
				//	EngineFuncs::ConsoleError("AutoNav: Out of Nodes but still need to expand.");
				//	status = Process_Cleanup;
				//	break;
				//}
				//// Close this node so it won't be explored again.
				//pNavNode->m_Open = false;

				//_ExpandNode(pNavNode);

				// Time splice.
				if(tme.GetElapsedSeconds() > 0.01)
					break;
			}
			break;
		}
	case Process_FloodBorder:
		{
			Prof(FloodBorder);

			//Timer tme;
			//for(obuint32 i = 0; i < m_FloodFillData->m_NodeIndex; ++i)
			//{
			//	if(m_FloodFillData->m_Nodes[i].m_NearSolid ||
			//		m_FloodFillData->m_Nodes[i].m_NearVoid)
			//	{
			//		m_FloodFillData->m_Nodes[i].m_DistanceFromEdge = 1;
			//	}
			//}

			//int iCurrentNum = 1;
			//bool bDidSomething = true;
			//while(bDidSomething)
			//{
			//	bDidSomething = false;
			//	for(obuint32 i = 0; i < m_FloodFillData->m_NodeIndex; ++i)
			//	{
			//		if(m_FloodFillData->m_Nodes[i].m_DistanceFromEdge == iCurrentNum)
			//		{
			//			for(int side = 0; side < NUM_DIRS; ++side)
			//			{
			//				if(m_FloodFillData->m_Nodes[m_FloodFillData->m_Nodes[i].m_Connections[side].Index].m_DistanceFromEdge == 0)
			//				{
			//					m_FloodFillData->m_Nodes[m_FloodFillData->m_Nodes[i].m_Connections[side].Index].m_DistanceFromEdge = iCurrentNum+1;
			//					bDidSomething = true;
			//				}
			//			}
			//		}
			//	}
			//	++iCurrentNum;
			//}

			//m_FloodFillData->m_Stats.m_TotalTime += tme.GetElapsedSeconds();

			//if(!m_PlannerFlags.CheckFlag(NAVMESH_STEPPROCESS) ||
			//	m_PlannerFlags.CheckFlag(NAVMESH_TAKESTEP))
			//{
			//	m_PlannerFlags.ClearFlag(NAVMESH_TAKESTEP);
			//	EngineFuncs::ConsoleMessage("Merging Sectors.");
			//	//status = Process_MergeSectors;
			//	status = Process_MakeRegions;
			//	m_Sectors.clear();
			//}

			////status = Process_MergeSectors;
			//status = Process_MakeRegions;
			//HandledCells.resize(m_FloodFillData->m_NodeIndex);
			//m_Sectors.clear();

			break;
		}
	case Process_MakeRegions:
		{
			Prof(MakeRegions);

			//static int Cycles = 1;

			//if(Cycles == 0)
			//	break;

			//Cycles--;

			//int LargestDistance = 0;
			//for(obuint32 i = 0; i < m_FloodFillData->m_NodeIndex; ++i)
			//{
			//	m_FloodFillData->m_Nodes[i].m_Region = 0;
			//	if(m_FloodFillData->m_Nodes[i].m_DistanceFromEdge > LargestDistance)
			//	{
			//		LargestDistance = m_FloodFillData->m_Nodes[i].m_DistanceFromEdge;
			//	}
			//}

			//// Mark all the regions to start
			//for(obuint32 i = 0; i < m_FloodFillData->m_NodeIndex; ++i)
			//{
			//	if(m_FloodFillData->m_Nodes[i].m_DistanceFromEdge == LargestDistance)
			//	{
			//		m_FloodFillData->m_Nodes[i].m_Region = -1;
			//	}
			//}

			//// isolate the region ids
			//std::list<obint32> il;

			//int CurrentRegion = 1;
			//for(obuint32 i = 0; i < m_FloodFillData->m_NodeIndex; ++i)
			//{
			//	if(m_FloodFillData->m_Nodes[i].m_Region == -1)
			//	{
			//		m_FloodFillData->m_Nodes[i].m_Region = CurrentRegion;

			//		il.push_back(i);
			//		while(!il.empty())
			//		{
			//			const int CurrentIndex = il.front();
			//			il.pop_front();

			//			for(int side = 0; side < NUM_DIRS; ++side)
			//			{
			//				const int neighbor = m_FloodFillData->m_Nodes[CurrentIndex].m_Connections[side].Index;
			//				if(m_FloodFillData->m_Nodes[neighbor].m_Region==-1)
			//				{
			//					m_FloodFillData->m_Nodes[neighbor].m_Region = CurrentRegion;
			//					il.push_back(neighbor);
			//				}
			//			}
			//		}

			//		CurrentRegion++;
			//	}
			//}

			break;
		}
	case Process_MergeSectors:
		{
			Prof(MergeSectors);

			//Timer tme;

			////////////////////////////////////////////////////////////////////////////
			//for(obuint32 i = 0; i < m_FloodFillData->m_NodeIndex; ++i)
			//{
			//	if(m_FloodFillData->m_Nodes[i].m_NearSolid ||
			//		m_FloodFillData->m_Nodes[i].m_NearVoid ||
			//		HandledCells.test(i))
			//	{
			//		m_FloodFillData->m_Nodes[i].m_DistanceFromEdge = 1;
			//	}
			//}

			//int iCurrentNum = 1;
			//bool bDidSomething = true;
			//while(bDidSomething)
			//{
			//	bDidSomething = false;
			//	for(obuint32 i = 0; i < m_FloodFillData->m_NodeIndex; ++i)
			//	{
			//		if(m_FloodFillData->m_Nodes[i].m_DistanceFromEdge == iCurrentNum)
			//		{
			//			for(int side = 0; side < NUM_DIRS; ++side)
			//			{
			//				if(m_FloodFillData->m_Nodes[m_FloodFillData->m_Nodes[i].m_Connections[side].Index].m_DistanceFromEdge == 0)
			//				{
			//					m_FloodFillData->m_Nodes[m_FloodFillData->m_Nodes[i].m_Connections[side].Index].m_DistanceFromEdge = iCurrentNum+1;
			//					bDidSomething = true;
			//				}
			//			}
			//		}
			//	}
			//	++iCurrentNum;
			//}
			////////////////////////////////////////////////////////////////////////////
			//
			//while(true)
			//{
			//	int iNodeDepth = 0;
			//	int iDeepestNode = -1;
			//	for(obuint32 i = 0; i < m_FloodFillData->m_NodeIndex; ++i)
			//	{
			//		if((m_FloodFillData->m_Nodes[i].m_DistanceFromEdge > iNodeDepth) &&
			//			!m_FloodFillData->m_Nodes[i].m_Sectorized)
			//		{
			//			iDeepestNode = i;
			//			iNodeDepth = m_FloodFillData->m_Nodes[i].m_DistanceFromEdge;
			//		}
			//	}

			//	// Out of nodes!
			//	if(iDeepestNode == -1)
			//	{
			//		EngineFuncs::ConsoleMessage(va("%d Sectors Merged.", m_Sectors.size()));
			//		status = Process_ConnectSectors;
			//		EngineFuncs::ConsoleMessage("Connecting Sectors.");
			//		break;
			//	}

			//	m_FloodFillData->m_Nodes[iDeepestNode].m_Sectorized = true;
			//	_BuildSector(iDeepestNode);

			//	//////////////////////////////////////////////////////////////////////////
			//	// mark all cells
			//	CellSet::iterator cIt = m_Sectors.back().m_ContainingCells.begin();
			//	for(; cIt != m_Sectors.back().m_ContainingCells.end(); ++cIt)
			//	{
			//		HandledCells.set(*cIt,true);
			//	}
			//	//////////////////////////////////////////////////////////////////////////

			//	// Time splice.
			//	if(tme.GetElapsedSeconds() > 0.01)
			//		break;
			//}
			//m_FloodFillData->m_Stats.m_TotalTime += tme.GetElapsedSeconds();
			break;
		}
	case Process_ConnectSectors:
		{
			Prof(ConnectSectors);

			//Timer tme;
			//while(ConnectSector < (int)m_Sectors.size())
			//{
			//	Sector &currentSector = m_Sectors[ConnectSector];

			//	CellSet::iterator cIt = currentSector.m_ContainingCells.begin(),
			//		cItEnd = currentSector.m_ContainingCells.end();
			//	for(; cIt != cItEnd; ++cIt)
			//	{
			//		const int iSectorCell = (*cIt);

			//		// Look for all connections that are part of a different sector
			//		for(int conn = 0; conn < NUM_DIRS; ++conn)
			//		{
			//			const int iConnectedCell = m_FloodFillData->m_Nodes[iSectorCell].m_Connections[conn].Index;

			//			if(iConnectedCell != -1)
			//			{
			//				const int iConnectedSector = m_FloodFillData->m_Nodes[iConnectedCell].m_Sector;
			//				if(iConnectedSector != ConnectSector)
			//				{
			//					// found a connection to another sector.
			//					SectorLink lnk;
			//					lnk.m_Sector = iConnectedSector;
			//					lnk.m_From = m_FloodFillData->m_Nodes[iSectorCell].m_Position;
			//					lnk.m_To = m_FloodFillData->m_Nodes[iConnectedCell].m_Position;
			//					currentSector.m_SectorLinks.push_back(lnk);
			//				}
			//			}
			//		}
			//	}

			//	// next
			//	++ConnectSector;
			//	if(tme.GetElapsedSeconds() > 0.01)
			//		return Function_InProgress;
			//}

			//obuint64 iSize = m_Sectors.size() * sizeof(NavigationMeshFF::NavSector);
			//for(obuint32 s = 0; s < m_Sectors.size(); ++s)
			//	iSize += m_Sectors[s].m_SectorLinks.size() * sizeof(NavigationMeshFF::NavLink);

			//EngineFuncs::ConsoleMessage(va("Approx size of stored Navigation Mesh: %s",
			//	Utils::FormatByteString(iSize).c_str()));

			status = Process_Cleanup;
			break;
		}
	case Process_Cleanup:
		{
			// Print status
			//int iNumGood = 0, iNumBad = 0;
			//for(obuint32 i = 0; i < m_FloodFillData->m_NodeIndex; ++i)
			//{
			//	if(m_FloodFillData->m_Nodes[i].m_ValidPos)
			//		iNumGood++;
			//	else
			//		iNumBad++;
			//}

			//EngineFuncs::ConsoleMessage(va("%d good nodes, %d bad nodes, in %f seconds",
			//	iNumGood,
			//	iNumBad,
			//	m_FloodFillData->m_Stats.m_TotalTime));

			//// Release the node data.
			////m_FloodFillData.reset();

			////////////////////////////////////////////////////////////////////////////

			//std::string strMap = g_EngineFuncs->GetMapName();
			//strMap += ".off";
			//fs::path filepath = Utils::GetNavFolder() / strMap;

			//std::fstream fl;
			//fl.open(filepath.string().c_str(), std::ios_base::out);
			//if(fl.is_open())
			//{
			//	fl << "OFF" << std::endl;

			//	fl << m_FloodFillData->m_VertIndex << " " <<
			//		m_FloodFillData->m_FaceIndex << " " <<
			//		m_FloodFillData->m_FaceIndex * 4 << std::endl << std::endl;

			//	for(obuint32 i = 0; i < m_FloodFillData->m_VertIndex; ++i)
			//	{
			//		fl << m_FloodFillData->m_Vertices[i].X() << " " <<
			//			m_FloodFillData->m_Vertices[i].Y() << " " <<
			//			m_FloodFillData->m_Vertices[i].Z() << std::endl;
			//	}

			//	for(obuint32 i = 0; i < m_FloodFillData->m_FaceIndex; ++i)
			//	{
			//		obColor col = GetFaceColor(m_FloodFillData->m_Faces[i]);

			//		fl << "4 " <<
			//			m_FloodFillData->m_Faces[i].m_Verts[0] << " " <<
			//			m_FloodFillData->m_Faces[i].m_Verts[1] << " " <<
			//			m_FloodFillData->m_Faces[i].m_Verts[2] << " " <<
			//			m_FloodFillData->m_Faces[i].m_Verts[3] << std::endl;
			//		/*" 3 " <<
			//		col.rF() << " " <<
			//		col.gF() << " " <<
			//		col.bF() << " " << std::endl;*/
			//	}
			//	fl.close();

			//	return Function_Finished;
			//}

			//////////////////////////////////////////////////////////////////////////

			return Function_Finished;
		}
	}

	return Function_InProgress;
}

void PathPlannerFloodFill::AddFloodStart(const Vector3f &_vec)
{
	m_StartPositions.push_back(_vec);
	EngineFuncs::ConsoleMessage("Added Flood Fill Start");
}

void PathPlannerFloodFill::ClearFloodStarts()
{
	EngineFuncs::ConsoleMessage(va("Clearing %d flood start nodes.", m_StartPositions.size()));
	m_StartPositions.clear();
}

void PathPlannerFloodFill::SaveFloodStarts()
{
	std::string strMap = g_EngineFuncs->GetMapName();
	strMap += ".navstarts";

	char strBuffer[1024] = {};
	sprintf(strBuffer, "nav/%s", strMap.c_str());

	File f;
	f.OpenForWrite(strBuffer, File::Text);
	if(f.IsOpen())
	{
		f.WriteInt32((obuint32)m_StartPositions.size());
		Vector3List::const_iterator cIt = m_StartPositions.begin();
		for(; cIt != m_StartPositions.end(); ++cIt)
		{
			f.WriteFloat((*cIt).X());
			f.WriteFloat((*cIt).Y());
			f.WriteFloat((*cIt).Z());
			f.WriteNewLine();
		}
		f.Close();
	}
	EngineFuncs::ConsoleMessage(va("Saved %d nav starts from %s",
		m_StartPositions.size(), strMap.c_str()));
}

void PathPlannerFloodFill::LoadFloodStarts()
{
	std::string strMap = g_EngineFuncs->GetMapName();
	strMap += ".navstarts";

	char strBuffer[1024] = {};
	sprintf(strBuffer, "nav/%s", strMap.c_str());

	File f;
	f.OpenForRead(strBuffer, File::Text);
	if(f.IsOpen())
	{
		obuint32 iNumPositions = 0;
		f.ReadInt32(iNumPositions);

		m_StartPositions.resize(0);
		m_StartPositions.reserve(iNumPositions);

		Vector3f vPos;
		for(obuint32 i = 0; i < iNumPositions; ++i)
		{
			f.ReadFloat(vPos.X());
			f.ReadFloat(vPos.Y());
			f.ReadFloat(vPos.Z());
			m_StartPositions.push_back( vPos );
		}
		f.Close();
	}
	EngineFuncs::ConsoleMessage(va("Loaded %d nav starts from %s",
		m_StartPositions.size(), strMap.c_str()));
}

void PathPlannerFloodFill::FloodFill(const FloodFillOptions &_options)
{
	if( System::mInstance->mGame->RemoveUpdateFunction("NavMesh_FloodFill"))
		return;

	FunctorPtr f(new ObjFunctor<PathPlannerFloodFill>(this, &PathPlannerFloodFill::Process_FloodFill));

	System::mInstance->mGame->AddUpdateFunction("NavMesh_FloodFill", f);
}

//////////////////////////////////////////////////////////////////////////

bool PathPlannerFloodFill::GetNavInfo(const Vector3f &pos,obint32 &_id,std::string &_name)
{
	return false;
}

void PathPlannerFloodFill::AddEntityConnection(const Event_EntityConnection &_conn)
{
}

void PathPlannerFloodFill::RemoveEntityConnection(GameEntity _ent)
{
}

Vector3f PathPlannerFloodFill::GetDisplayPosition(const Vector3f &_pos)
{
	return _pos;
}