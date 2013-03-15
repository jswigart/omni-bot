////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "PathPlannerNavMesh.h"
#include "ScriptManager.h"
#include "IGameManager.h"
#include "Waypoint.h"
#include "IGame.h"
#include "Client.h"
#include "Timer.h"

#include "RenderBuffer.h"

using namespace std;

const float SNAP_RADIUS = 8.f;

//////////////////////////////////////////////////////////////////////////

void PathPlannerNavMesh::InitCommands()
{
	PathPlannerBase::InitCommands();

	SetEx("nav_save", "Save current navigation to disk",
		this, &PathPlannerNavMesh::cmdNavSave);
	SetEx("nav_load", "Load last saved navigation from disk",
		this, &PathPlannerNavMesh::cmdNavLoad);
	SetEx("nav_view", "Turn on/off navmesh visibility.",
		this, &PathPlannerNavMesh::cmdNavView);
	SetEx("nav_viewconnections", "Turn on/off navmesh connection visibility.",
		this, &PathPlannerNavMesh::cmdNavViewConnections);
	SetEx("nav_viewflags", "Turn on/off navmesh flag visibility.",
		this, &PathPlannerNavMesh::cmdNavViewFlags);

	//////////////////////////////////////////////////////////////////////////
	SetEx("nav_enablestep", "Enable step by step generation process.",
		this, &PathPlannerNavMesh::cmdNavEnableStep);
	SetEx("nav_step", "Step to the next nav process.",
		this, &PathPlannerNavMesh::cmdNavStep);
	//////////////////////////////////////////////////////////////////////////
	SetEx("nav_autofeature", "Automatically waypoints jump pads, teleporters, player spawns.",
		this, &PathPlannerNavMesh::cmdAutoBuildFeatures);

	//////////////////////////////////////////////////////////////////////////
	SetEx("nav_startpoly", "Starts a sector polygon.",
		this, &PathPlannerNavMesh::cmdStartPoly);
	SetEx("nav_undopoly", "Starts a sector polygon.",
		this, &PathPlannerNavMesh::cmdUndoPoly);

	/*SetEx("nav_loadobj", "Loads navmesh from obj file.",
	this, &PathPlannerNavMesh::cmdLoadObj);
	SetEx("nav_loadmap", "Loads navmesh from map file.",
	this, &PathPlannerNavMesh::cmdLoadMap);*/

	SetEx("nav_pp", "Creates a plane from a point and normal.",
		this, &PathPlannerNavMesh::cmdCreatePlanePoly);
	SetEx("nav_sp", "Creates a plane from a point and normal.",
		this, &PathPlannerNavMesh::cmdCreateSlicePoly);
	SetEx("nav_ss", "Slices working sector with the sector being aimed at.",
		this, &PathPlannerNavMesh::cmdCreateSliceSector);

	SetEx("nav_setfield", "Sets a named field on the navigation sector.",
		this, &PathPlannerNavMesh::cmdSetField);
	SetEx("nav_setfieldm", "Sets a named field on the navigation sector.",
		this, &PathPlannerNavMesh::cmdSetField);

	SetEx("nav_commit", "Creates a plane from a point and normal.",
		this, &PathPlannerNavMesh::cmdCommitPoly);
	SetEx("nav_editsector", "Begins editing a sector so further slicing may be done.",
		this, &PathPlannerNavMesh::cmdEditSector);
	SetEx("nav_splitsector", "Splits a sector into 2 sectors along a plane.",
		this, &PathPlannerNavMesh::cmdSplitSector);
	SetEx("nav_groundsector", "Grounds all the vertices in the sector.",
		this, &PathPlannerNavMesh::cmdGroundSector);
	SetEx("nav_updatecontents", "Updates the contents flags of all sectors.",
		this, &PathPlannerNavMesh::cmdUpdateContents);
	SetEx("nav_next", "Steps the current tool to the next operation.",
		this, &PathPlannerNavMesh::cmdNext);

	SetEx("nav_delsector", "Deletes a sector by its id number.",
		this, &PathPlannerNavMesh::cmdDeleteSector);

	SetEx("nav_mirrorsectors", "Mirrors all sectors across a specified axis.",
		this, &PathPlannerNavMesh::cmdMirrorSectors);

	SetEx("sector_setproperty", "Sets a property of the current sector.",
		this, &PathPlannerNavMesh::cmdSectorSetProperty);

	SetEx("sector_createconnections", "Sets a property of the current sector.",
		this, &PathPlannerNavMesh::cmdSectorCreateConnections);

	SetEx("nav_setmapcenter", "Sets the map center.",
		this, &PathPlannerNavMesh::cmdSetMapCenter);

	SetEx("nav_obstacleadd", "Creates an obstacle.",
		this, &PathPlannerNavMesh::cmdObstacleAdd);

	// INFLUENCE MAP EXPERIMENTATION
	SetEx("nav_mapcreate", "Creates an influence map.",
		this, &PathPlannerNavMesh::cmdInfluenceMapCreate);
	SetEx("nav_mapseed", "Adds a seed point to the map for exploration.",
		this, &PathPlannerNavMesh::cmdInfluenceMapSeed);
	SetEx("nav_mapmem", "Shows the memory usage of the map.",
		this, &PathPlannerNavMesh::cmdInfluenceMapMem);
	SetEx("nav_mapsave", "Saves the influence map.",
		this, &PathPlannerNavMesh::cmdInfluenceMapSave);
	SetEx("nav_mapload", "Load the influence map.",
		this, &PathPlannerNavMesh::cmdInfluenceMapLoad);

	SetEx("nav_mapflood", "Load the influence map.",
		this, &PathPlannerNavMesh::cmdInfluenceMapFlood);

	REGISTER_STATE(PathPlannerNavMesh,NoOp);
	REGISTER_STATE(PathPlannerNavMesh,PlaceSector);
	REGISTER_STATE(PathPlannerNavMesh,SliceSector);
	REGISTER_STATE(PathPlannerNavMesh,SliceSectorWithSector);
	REGISTER_STATE(PathPlannerNavMesh,EditSector);
	REGISTER_STATE(PathPlannerNavMesh,SplitSector);
	REGISTER_STATE(PathPlannerNavMesh,TraceSector);
	REGISTER_STATE(PathPlannerNavMesh,GroundSector);
	REGISTER_STATE(PathPlannerNavMesh,CommitSector);
	REGISTER_STATE(PathPlannerNavMesh,MirrorSectors);
	REGISTER_STATE(PathPlannerNavMesh,PlaceBorder);
	REGISTER_STATE(PathPlannerNavMesh,FloodSpanMap);

	SetNextState(NoOp);
}

void PathPlannerNavMesh::cmdNavSave(const StringVector &_args)
{
	if(!m_PlannerFlags.CheckFlag(NAV_VIEW))
		return;

	if(Save(g_EngineFuncs->GetMapName()))
	{
		EngineFuncs::ConsoleMessage("Saved Nav.");
	}
	else
		EngineFuncs::ConsoleError("ERROR Saving Nav.");
}

void PathPlannerNavMesh::cmdNavLoad(const StringVector &_args)
{
	if(!m_PlannerFlags.CheckFlag(NAV_VIEW))
		return;

	if(Load(g_EngineFuncs->GetMapName()))
	{
		EngineFuncs::ConsoleMessage("Loaded Nav.");
	}
	else
		EngineFuncs::ConsoleError("ERROR Loading Nav.");
}

void PathPlannerNavMesh::cmdNavView(const StringVector &_args)
{
	const char *strUsage[] =
	{
		"nav_view enable[bool]",
		"> enable: Enable nav rendering. true/false/on/off/1/0",
	};

	CHECK_NUM_PARAMS(_args, 2, strUsage);
	CHECK_BOOL_PARAM(bEnable, 1, strUsage);

	m_PlannerFlags.SetFlag( NAV_VIEW, bEnable );
}

void PathPlannerNavMesh::cmdNavViewConnections(const StringVector &_args)
{
	const char *strUsage[] =
	{
		"nav_viewconnections enable[bool]",
		"> enable: Enable nav connection rendering. true/false/on/off/1/0",
	};

	CHECK_NUM_PARAMS(_args, 2, strUsage);
	CHECK_BOOL_PARAM(bEnable, 1, strUsage);

	m_PlannerFlags.SetFlag( NAV_VIEWCONNECTIONS, bEnable );
}

void PathPlannerNavMesh::cmdNavViewFlags(const StringVector &_args)
{
	const char *strUsage[] =
	{
		"nav_viewflags enable[bool]",
		"> enable: Enable nav flag. true/false/on/off/1/0",
	};

	CHECK_NUM_PARAMS(_args, 2, strUsage);
	CHECK_BOOL_PARAM(bEnable, 1, strUsage);

	m_PlannerFlags.SetFlag( NAV_VIEWFLAGS, bEnable );
}

void PathPlannerNavMesh::cmdNavEnableStep(const StringVector &_args)
{
	if(!m_PlannerFlags.CheckFlag(NAV_VIEW))
		return;

	const char *strUsage[] =
	{
		"nav_enablestep enable[bool]",
		"> enable: Enable step by step nav generation. true/false/on/off/1/0",
	};

	CHECK_NUM_PARAMS(_args, 2, strUsage);
	CHECK_BOOL_PARAM(bEnable, 1, strUsage);

	m_PlannerFlags.SetFlag( NAVMESH_STEPPROCESS, bEnable );
}

void PathPlannerNavMesh::cmdNavStep(const StringVector &_args)
{
	if(!m_PlannerFlags.CheckFlag(NAV_VIEW))
		return;

	m_PlannerFlags.SetFlag( NAVMESH_TAKESTEP, true );
}

//////////////////////////////////////////////////////////////////////////

extern float g_fBottomWaypointOffset;

void PathPlannerNavMesh::cmdAutoBuildFeatures(const StringVector &_args)
{
	if(!m_PlannerFlags.CheckFlag(NAV_VIEW))
		return;

	const int iMaxFeatures = 128;
	AutoNavFeature features[iMaxFeatures];
	int iNumFeatures = g_EngineFuncs->GetAutoNavFeatures(features, iMaxFeatures);
	for(int i = 0; i < iNumFeatures; ++i)
	{
		const float fTime = 30.f;

		Vector3f vPos(features[i].m_Position);
		Vector3f vFace(features[i].m_Facing);
		Vector3f vTarget(features[i].m_TargetPosition);

		// Adjust for waypoint offsets
		if(!features[i].m_Bounds.IsZero())
		{
			features[i].m_Bounds.CenterBottom(vPos);
			vPos.Z() -= g_fBottomWaypointOffset;
		}
		if(!features[i].m_TargetBounds.IsZero())
		{
			features[i].m_TargetBounds.CenterBottom(vTarget);
			vTarget.Z() -= g_fBottomWaypointOffset;
		}

		//AddFloodStart(vPos);
		//if(vPos != vTarget)
		//{
		//	AddFloodStart(vTarget);
		//	//pFeature->ConnectTo(pTarget);
		//}

		//////////////////////////////////////////////////////////////////////////
		RenderBuffer::AddLine(vPos, vPos+Vector3f::UNIT_Z * 32.f, COLOR::GREEN, fTime);
		if(vPos != vTarget)
		{
			RenderBuffer::AddLine(vPos, vTarget, COLOR::MAGENTA, fTime);
			RenderBuffer::AddLine(vTarget, vTarget+Vector3f::UNIT_Z * 32.f, COLOR::RED, fTime);
		}
		RenderBuffer::AddAABB(features[i].m_Bounds, COLOR::GREEN);
		//////////////////////////////////////////////////////////////////////////
	}
	EngineFuncs::ConsoleMessage(va("Found %d nav features.", iNumFeatures));
}

Vector3f PathPlannerNavMesh::_SectorVertWithin(const Vector3f &_pos1, const Vector3f &_pos2, float _range, bool *_snapped)
{
	Vector3f r = _pos2;

	const float fRangeSq = Mathf::Sqr(_range);
	float fClosest = Utils::FloatMax;

	*_snapped = false;
	for(obuint32 s = 0; s < m_NavSectors.size(); ++s)
	{
		const NavSector &sec = m_NavSectors[s];

		for(obuint32 v = 0; v < sec.m_Boundary.size(); ++v)
		{
			DistPoint3Segment3f dist( sec.m_Boundary[v], Segment3f( _pos1,_pos2 ) );
			const float fLenSq = dist.GetSquared();
			if( fLenSq < fRangeSq && fLenSq < fClosest )
			{
				r = sec.m_Boundary[v];
				fClosest = fLenSq;

				if(_snapped)
					*_snapped = true;
			}
		}
	}
	return r;
}

void PathPlannerNavMesh::cmdStartPoly(const StringVector &_args)
{
	if(!m_PlannerFlags.CheckFlag(NAV_VIEW))
		return;

	m_ToolCancelled = false;
	SetNextState(TraceSector);
}

void PathPlannerNavMesh::cmdUndoPoly(const StringVector &_args)
{
	if(!m_PlannerFlags.CheckFlag(NAV_VIEW))
		return;

	if(!m_WorkingManualSector.empty())
		m_WorkingManualSector.pop_back();

	if(m_WorkingManualSector.empty() && GetCurrentStateId()==TraceSector)
		SetNextState(NoOp);
}

void PathPlannerNavMesh::cmdCreatePlanePoly(const StringVector &_args)
{
	if(!m_PlannerFlags.CheckFlag(NAV_VIEW))
		return;

	if(GetCurrentStateId()!=PlaceSector)
	{
		SetNextState(PlaceSector);
	}
	else
	{
		m_ToolCancelled = false;
		SetNextState(NoOp);
	}
}

void PathPlannerNavMesh::cmdCreateSlicePoly(const StringVector &_args)
{
	if(!m_PlannerFlags.CheckFlag(NAV_VIEW))
		return;

	if(GetCurrentStateId()!=SliceSector)
	{
		SetNextState(SliceSector);
	}
	else
	{
		m_ToolCancelled = false;
		SetNextState(NoOp);
	}
}

void PathPlannerNavMesh::cmdCreateSliceSector(const StringVector &_args)
{
	if(!m_PlannerFlags.CheckFlag(NAV_VIEW))
		return;

	if(GetCurrentStateId()!=SliceSectorWithSector)
	{
		SetNextState(SliceSectorWithSector);
	}
	else
	{
		m_ToolCancelled = false;
		SetNextState(NoOp);
	}
}

void PathPlannerNavMesh::cmdEditSector(const StringVector &_args)
{
	if(!m_PlannerFlags.CheckFlag(NAV_VIEW))
		return;

	m_ToolCancelled = false;
	SetNextState(EditSector);
}

void PathPlannerNavMesh::cmdSplitSector(const StringVector &_args)
{
	if(!m_PlannerFlags.CheckFlag(NAV_VIEW))
		return;

	if(GetCurrentStateId()!=SplitSector)
	{
		SetNextState(SplitSector);
	}
	else
	{
		m_ToolCancelled = false;
		SetNextState(NoOp);
	}
}

void PathPlannerNavMesh::cmdGroundSector(const StringVector &_args)
{
	if(!m_PlannerFlags.CheckFlag(NAV_VIEW))
		return;

	m_ToolCancelled = false;
	SetNextState(GroundSector);
}

void PathPlannerNavMesh::cmdCommitPoly(const StringVector &_args)
{
	if(!m_PlannerFlags.CheckFlag(NAV_VIEW))
		return;

	m_ToolCancelled = false;
	SetNextState(CommitSector);
}

void PathPlannerNavMesh::cmdDeleteSector(const StringVector &_args)
{
	if(!m_PlannerFlags.CheckFlag(NAV_VIEW))
		return;

	Vector3f vFacing, vPos;
	if(!Utils::GetLocalEyePosition(vPos) || !Utils::GetLocalFacing(vFacing))
	{
		EngineFuncs::ConsoleError("can't get facing or eye position");
		return;
	}

	PathPlannerNavMesh::NavCollision nc = FindCollision(vPos, vFacing*2048.f);
	if(!nc.DidHit() || nc.HitAttrib().Fields.Mirrored)
	{
		EngineFuncs::ConsoleError("can't find sector, aim at a sector and try again.");
		return;
	}

	m_NavSectors.erase(m_NavSectors.begin() + nc.HitAttrib().Fields.SectorId);
	InitSectors();
}

void PathPlannerNavMesh::cmdMirrorSectors(const StringVector &_args)
{
	if(!m_PlannerFlags.CheckFlag(NAV_VIEW))
		return;

	if(_args.size() < 2)
	{
		EngineFuncs::ConsoleError("axis not specified, valid axis are x,y,z,o");
		return;
	}

	bool bNeg = false, bAll = false;
	if(_args[1].find("all") != std::string::npos)
		bAll = true;
	if(_args[1].find('-') != std::string::npos)
		bNeg = true;

	NavmeshIO::Sector_MirrorDir mir = NavmeshIO::Sector_MirrorDir_MirrorNone;

	if(_args[1].find('x') != std::string::npos)
		mir = bNeg ?  NavmeshIO::Sector_MirrorDir_MirrorNX :  NavmeshIO::Sector_MirrorDir_MirrorX;
	if(_args[1].find('y') != std::string::npos)
		mir = bNeg ?  NavmeshIO::Sector_MirrorDir_MirrorNY :  NavmeshIO::Sector_MirrorDir_MirrorY;
	if(_args[1].find('z') != std::string::npos)
		mir = bNeg ?  NavmeshIO::Sector_MirrorDir_MirrorNZ :  NavmeshIO::Sector_MirrorDir_MirrorZ;
	if(_args[1].find('o') != std::string::npos)
		mir = NavmeshIO::Sector_MirrorDir_MirrorNone;

	if(bAll)
	{
		for(obuint32 s = 0; s < m_NavSectors.size(); ++s)
		{
			m_NavSectors[s].mMirror = mir;
		}
	}
	else
	{
		Vector3f vFacing, vPos;
		if(!Utils::GetLocalEyePosition(vPos) || !Utils::GetLocalFacing(vFacing))
		{
			EngineFuncs::ConsoleError("can't get facing or eye position");
			return;
		}

		NavCollision nc = FindCollision(vPos, vPos + vFacing * 2048.f);
		if(!nc.DidHit() || nc.HitAttrib().Fields.Mirrored)
		{
			EngineFuncs::ConsoleError("no sector at cursor");
			SetNextState(NoOp);
			return;
		}
		NavSector *pNavSector = &m_NavSectors[nc.HitAttrib().Fields.SectorId];
		if(!pNavSector)
		{
			EngineFuncs::ConsoleError("can't find sector, aim at a sector and try again.");
			return;
		}
		pNavSector->mMirror = mir;
	}

	InitSectors();
}

void PathPlannerNavMesh::cmdSectorCreateConnections(const StringVector &_args)
{
	return;

	//typedef std::vector<SegmentList> SectorSegments;
	//SectorSegments segs;

	////////////////////////////////////////////////////////////////////////////

	//for(obuint32 i = 0; i < m_ActiveNavSectors.size(); ++i)
	//{
	//	const NavSector &ns = m_ActiveNavSectors[i];
	//	segs.push_back(ns.GetEdgeSegments());
	//}

	//// look for edges that could be connected
	//obuint32 LastPortalSector = (obuint32)-1;

	//for(obuint32 i = 0; i < segs.size(); ++i)
	//{
	//	for(obuint32 j = 0; j < segs.size(); ++j)
	//	{
	//		if(i==j)
	//			continue;

	//		// look for matching segments
	//		for(obuint32 s1 = 0; s1 < segs[i].size(); ++s1)
	//		{
	//			for(obuint32 s2 = 0; s2 < segs[j].size(); ++s2)
	//			{
	//				const Segment3f &sec1 = segs[i][s1];
	//				const Segment3f &sec2 = segs[j][s2];

	//				Segment3f overlap;
	//				if(Utils::GetSegmentOverlap(sec1,sec2,overlap))
	//				{
	//					if(!Utils::TestSegmentForOcclusion(overlap))
	//					{
	//						//RenderBuffer::AddLine(overlap.P1,overlap.Center+Vector3f(0,0,32),COLOR::MAGENTA,10.f);
	//						//RenderBuffer::AddLine(overlap.P0,overlap.Center+Vector3f(0,0,32),COLOR::MAGENTA,10.f);
	//						//RenderBuffer::AddLine(sec2.GetPosEnd()+Vector3f(0,0,32),sec1.GetNegEnd()+Vector3f(0,0,32),COLOR::MAGENTA,10.f);

	//						NavPortal navPortal;
	//						navPortal.m_Segment = overlap;
	//						navPortal.m_DestSector = j;
	//						m_NavPortals.push_back(navPortal);

	//						//////////////////////////////////////////////////////////////////////////
	//						// Update the sector so it knows about its portals
	//						if(LastPortalSector==i)
	//						{
	//							m_ActiveNavSectors[i].m_NumPortals++;
	//						}
	//						else
	//						{
	//							m_ActiveNavSectors[i].m_StartPortal = (int)m_NavPortals.size()-1;
	//							m_ActiveNavSectors[i].m_NumPortals = 1;
	//						}
	//						//////////////////////////////////////////////////////////////////////////

	//						LastPortalSector = i;
	//					}
	//				}
	//			}
	//		}
	//	}
	//}
}

void PathPlannerNavMesh::cmdSetMapCenter(const StringVector &_args)
{
	if(!m_PlannerFlags.CheckFlag(NAV_VIEW))
		return;

	Vector3f vMapCenter;

	bool bError = false;
	switch(_args.size())
	{
	case 2:
		{
			if(_args[1]=="<position>" && Utils::GetLocalPosition(vMapCenter))
			{
			}
			else
			{
				bError = true;
			}
			break;
		}
	case 4:
		{
			if(Utils::ConvertString(_args[1],vMapCenter.X()) &&
				Utils::ConvertString(_args[2],vMapCenter.Y()) &&
				Utils::ConvertString(_args[3],vMapCenter.Z()))
			{
			}
			else
			{
				bError = true;
			}
			break;
		}
	}

	if(bError)
	{
		EngineFuncs::ConsoleError("Invalid Parameters");
		return;
	}

	m_MapCenter = vMapCenter;
	InitSectors();
}

void PathPlannerNavMesh::cmdSectorSetProperty(const StringVector &_args)
{
	if(!m_PlannerFlags.CheckFlag(NAV_VIEW))
		return;

	if(_args.size() < 3)
	{
		EngineFuncs::ConsoleError("sector_setproperty name value");
		return;
	}

	std::string propName = _args[1];
	std::string propValue = _args[2];

	Vector3f vFacing, vPos;
	if(!Utils::GetLocalEyePosition(vPos) || !Utils::GetLocalFacing(vFacing))
	{
		EngineFuncs::ConsoleError("can't get facing or eye position");
		return;
	}

	PathPlannerNavMesh::NavCollision nc = FindCollision(vPos, vFacing*2048.f);
	if(!nc.DidHit() || nc.HitAttrib().Fields.Mirrored)
	{
		EngineFuncs::ConsoleError("can't find sector, aim at a sector and try again.");
		return;
	}

	// TODO:
}

void PathPlannerNavMesh::cmdObstacleAdd(const StringVector &_args)
{
	if(_args.size() < 3)
	{
		EngineFuncs::ConsoleError("nav_obstacleadd length width");
		return;
	}

	const float len = (float)atof( _args[ 1 ].c_str() );
	const float wid = (float)atof( _args[ 2 ].c_str() );

	if ( len == 0.0 || wid == 0.0 )
		return;

	Vector3f vObsPoint, vObsNormal;
	Utils::GetLocalAimPoint( vObsPoint, &vObsNormal );
	NavSector * sector = GetSectorAt( vObsPoint );
	if ( sector != NULL )
	{
		Obstacle obs;
		obs.mPolyVerts.push_back( vObsPoint + Vector3f( -len, -wid, 0.0f ) );
		obs.mPolyVerts.push_back( vObsPoint + Vector3f(  len, -wid, 0.0f ) );
		obs.mPolyVerts.push_back( vObsPoint + Vector3f(  len,  wid, 0.0f ) );
		obs.mPolyVerts.push_back( vObsPoint + Vector3f( -len,  wid, 0.0f ) );
		sector->m_Obstacles.push_back( obs );
	}
}

void PathPlannerNavMesh::cmdInfluenceMapCreate(const StringVector &_args)
{
	const float fmin = std::numeric_limits<float>::lowest();
	const float fmax = std::numeric_limits<float>::max();

	AABB mapbounds;
	mapbounds.Set( Vector3f( fmax,fmax,fmax ), Vector3f( fmin, fmin, fmin ) );
	for ( NavSectorList::iterator s = m_ActiveNavSectors.begin(); s != m_ActiveNavSectors.end(); ++s)
	{
		for ( Vector3List::iterator v = s->m_Boundary.begin(); v != s->m_Boundary.end(); ++v)
		{
			mapbounds.Expand( *v );
		}
	}

	VectorQueue empty;
	m_SpanFrontier.swap( empty );

	RenderBuffer::StaticBufferDelete( mInfluenceBufferId );
	mInfluenceBufferId = 0;

	OB_DELETE( m_Influence );
	OB_DELETE( m_SpanMap );

	m_SpanMap = new SpanMap();
	m_SpanMap->Init( Vector3f(mapbounds.m_Mins), Vector3f(mapbounds.m_Maxs), 16.0f );

	EngineFuncs::ConsoleMessage(va("Created %d x %d span map",
		m_SpanMap->GetNumCellsX(), m_SpanMap->GetNumCellsY() ) );
}

void PathPlannerNavMesh::cmdInfluenceMapSeed(const StringVector &_args)
{
	/*Vector3f eyePos;
	if ( Utils::GetLocalEyePosition(eyePos) )
	m_SpanFrontier.push( eyePos );*/

	enum { MaxFeatures = 64 };
	AutoNavFeature features[ MaxFeatures ];
	const int numFeatures = g_EngineFuncs->GetAutoNavFeatures( features, MaxFeatures );
	for ( int i = 0; i < numFeatures; ++i )
	{
		m_SpanFrontier.push( features[ i ].m_Position );
		m_SpanFrontier.push( features[ i ].m_TargetPosition );
	}

	if ( m_SpanMap == NULL )
		EngineFuncs::ConsoleMessage( "No Influence Map Created, use nav_mapcreate" );
	else if ( GetCurrentStateId() != FloodSpanMap )
		SetNextState( FloodSpanMap );
}

void PathPlannerNavMesh::cmdInfluenceMapMem(const StringVector &_args)
{
	if ( m_SpanMap != NULL )
	{
		EngineFuncs::ConsoleMessage(va("Influence Map %d x %d ( %s )",
			m_SpanMap->GetNumCellsX(),
			m_SpanMap->GetNumCellsY(),
			Utils::FormatByteString( m_SpanMap->CalculateMemUsage() ).c_str() ) );
	}
}

void PathPlannerNavMesh::cmdInfluenceMapSave(const StringVector &_args)
{
	if ( m_SpanMap != NULL )
	{
		const std::string filePath	= std::string("nav/") + std::string(g_EngineFuncs->GetMapName()) + ".influence";

		std::string data;
		if ( m_SpanMap->Serialize( data ) )
		{
			File f;
			if(f.OpenForWrite( filePath.c_str() ,File::Binary ) )
			{
				f.Write( data.c_str(), data.length() );
			}
		}
	}
}

void PathPlannerNavMesh::cmdInfluenceMapLoad(const StringVector &_args)
{
	if ( m_SpanMap == NULL )
		m_SpanMap = new SpanMap();

	const std::string filePath	= std::string("nav/") + std::string(g_EngineFuncs->GetMapName()) + ".influence";

	m_SpanMap->Clear();

	File f;
	if(!f.OpenForRead( filePath.c_str() ,File::Binary ) )
	{
		EngineFuncs::ConsoleError( va( "Influence Map %s not found", filePath.c_str() ) );
		return;
	}

	std::string data;
	if (!f.ReadWholeFile( data ) )
	{
		EngineFuncs::ConsoleError( va( "Influence Map Read Error %s", filePath.c_str() ) );
		return;
	}

	if ( !m_SpanMap->DeSerialize( data ) )
	{
		EngineFuncs::ConsoleError( va( "Influence Map Parse Error %s", filePath.c_str() ) );
		return;
	}
}

void PathPlannerNavMesh::cmdInfluenceMapFlood(const StringVector &_args)
{
	Vector3f vAimPt;
	if ( !Utils::GetLocalAimPoint( vAimPt ))
		return;
	NavSector * ns = GetSectorAt(vAimPt);
	if ( ns == NULL )
		return;

	m_Influence = m_SpanMap->CreateInfluenceLayer();
	m_Influence->Reset();
	m_Influence->AddSeed( vAimPt, 0.0f );

	Timer t;
	while( !m_Influence->UpdateInfluences( std::numeric_limits<int>::max() ) )
	{
	}

	EngineFuncs::ConsoleError( va( "Influence Flooded in %.3f sec ( %s )",
		t.GetElapsedSeconds(),
		Utils::FormatByteString( m_Influence->CalculateMemUsage() ).c_str() ) );
}

void PathPlannerNavMesh::_BenchmarkPathFinder(const StringVector &_args)
{
	EngineFuncs::ConsoleMessage("-= NavMesh PathFind Benchmark =-");

	double dTimeTaken = 0.0f;
	obint32 iNumSectors = (obint32)m_ActiveNavSectors.size();
	obint32 iNumPaths = iNumSectors * iNumSectors;

	Timer tme;
	tme.Reset();
	for(obint32 w1 = 0; w1 < iNumSectors; ++w1)
	{
		for(obint32 w2 = 0; w2 < iNumSectors; ++w2)
		{
			/*const NavSector &pS1 = m_ActiveNavSectors[w1];
			const NavSector &pS2 = m_ActiveNavSectors[w2];

			PlanPathToGoal(NULL,
			pS1.m_Middle+Vector3f(0,0,NavigationMeshOptions::CharacterHeight),
			pS2.m_Middle+Vector3f(0,0,NavigationMeshOptions::CharacterHeight),
			0);*/
		}
	}
	dTimeTaken = tme.GetElapsedSeconds();

	EngineFuncs::ConsoleMessage(va("generated %d paths in %f seconds: %f paths/sec",
		iNumPaths, dTimeTaken, dTimeTaken != 0.0f ? (float)iNumPaths / dTimeTaken : 0.0f));
}

void PathPlannerNavMesh::_BenchmarkGetNavPoint(const StringVector &_args)
{
	obuint32 iNumIterations = 1;
	if(_args.size() > 1)
	{
		iNumIterations = atoi(_args[1].c_str());
		if(iNumIterations <= 0)
			iNumIterations = 1;
	}

	EngineFuncs::ConsoleMessage("-= NavMesh GetNavPoint Benchmark  =-");

	/*double dTimeTaken = 0.0f;
	obuint32 iNumWaypoints = m_ActiveNavSectors.size();
	Timer tme;

	obuint32 iHits = 0, iMisses = 0;
	tme.Reset();
	for(obuint32 i = 0; i < iNumIterations; ++i)
	{
	for(obuint32 w1 = 0; w1 < iNumWaypoints; ++w1)
	{
	NavSector *pSector = m_ActiveNavSectors[w1];

	Waypoint *pClosest = _GetClosestWaypoint(pWaypoint->GetPosition(), (NavFlags)0, true);
	if(pClosest)
	++iHits;
	else
	++iMisses;
	}
	}

	dTimeTaken = tme.GetElapsedSeconds();

	EngineFuncs::ConsoleMessage("_GetClosest() %d calls, %d hits, %d misses : avg %f per second",
	iNumWaypoints * iNumIterations,
	iHits,
	iMisses,
	dTimeTaken != 0.0f ? ((float)(iNumWaypoints * iNumIterations) / dTimeTaken) : 0.0f);	*/
}

void PathPlannerNavMesh::cmdNext(const StringVector &_args)
{
	if(!m_PlannerFlags.CheckFlag(NAV_VIEW))
		return;
}

void PathPlannerNavMesh::cmdUpdateContents(const StringVector &_args)
{
	/*CONT_SOLID		= (1<<0),
	CONT_WATER		= (1<<1),
	CONT_SLIME		= (1<<2),
	CONT_FOG		= (1<<3),
	CONT_MOVER		= (1<<4),
	CONT_TRIGGER	= (1<<5),
	CONT_LAVA		= (1<<6),
	CONT_LADDER		= (1<<7),
	CONT_TELEPORTER = (1<<8),
	CONT_MOVABLE	= (1<<9),
	CONT_PLYRCLIP	= (1<<10),*/

	for ( size_t i = 0; i < m_ActiveNavSectors.size(); ++i )
	{
		const NavSector & ans = m_ActiveNavSectors[ i ];
		const Vector3f sectorCenter = ans.CalculateCenter();

		const bool mirroredSector = ans.mId >= (int)m_NavSectors.size();
		NavSector & baseSector = m_NavSectors[ ans.mId % m_NavSectors.size() ];

		NavmeshIO::SectorData & sectorData = mirroredSector ?
			baseSector.mSectorDataMirrored :
		baseSector.mSectorData;

		const int contents = g_EngineFuncs->GetPointContents( sectorCenter );

		// set the flags
		sectorData.set_inwater( (contents & CONT_WATER)!=0 );

		if ( sectorData.inwater() )
		{
		}
	}
}

static std::string GetDisplayValue( const google::protobuf::Message & msg, const google::protobuf::FieldDescriptor * fieldDesc )
{
	using namespace google;

	try
	{
		switch( fieldDesc->type() )
		{
		case protobuf::FieldDescriptorProto_Type_TYPE_DOUBLE:
			{
				return boost::lexical_cast<std::string>( msg.GetReflection()->GetDouble( msg, fieldDesc ) );
			}
		case protobuf::FieldDescriptorProto_Type_TYPE_FLOAT:
			return boost::lexical_cast<std::string>( msg.GetReflection()->GetFloat( msg, fieldDesc ) );
		case protobuf::FieldDescriptorProto_Type_TYPE_INT64:
		case protobuf::FieldDescriptorProto_Type_TYPE_SFIXED64:
		case protobuf::FieldDescriptorProto_Type_TYPE_SINT64:
			return boost::lexical_cast<std::string>( msg.GetReflection()->GetInt64( msg, fieldDesc ) );
		case protobuf::FieldDescriptorProto_Type_TYPE_UINT64:
		case protobuf::FieldDescriptorProto_Type_TYPE_FIXED64:
			return boost::lexical_cast<std::string>( msg.GetReflection()->GetUInt64( msg, fieldDesc ) );
		case protobuf::FieldDescriptorProto_Type_TYPE_INT32:
		case protobuf::FieldDescriptorProto_Type_TYPE_SFIXED32:
		case protobuf::FieldDescriptorProto_Type_TYPE_SINT32:
			return boost::lexical_cast<std::string>( msg.GetReflection()->GetInt32( msg, fieldDesc ) );
		case protobuf::FieldDescriptorProto_Type_TYPE_BOOL:
			return boost::lexical_cast<std::string>( msg.GetReflection()->GetBool( msg, fieldDesc ) );
		case protobuf::FieldDescriptorProto_Type_TYPE_UINT32:
		case protobuf::FieldDescriptorProto_Type_TYPE_FIXED32:
			return boost::lexical_cast<std::string>( msg.GetReflection()->GetUInt32( msg, fieldDesc ) );
		case protobuf::FieldDescriptorProto_Type_TYPE_STRING:
			return msg.GetReflection()->GetString( msg, fieldDesc );
		case protobuf::FieldDescriptorProto_Type_TYPE_GROUP:
			return "GROUP";
		case protobuf::FieldDescriptorProto_Type_TYPE_MESSAGE:
			return "MESSAGE";
		case protobuf::FieldDescriptorProto_Type_TYPE_BYTES:
			return "BYTES";
		case protobuf::FieldDescriptorProto_Type_TYPE_ENUM:
			{
				const protobuf::EnumValueDescriptor * enumVal = msg.GetReflection()->GetEnum( msg, fieldDesc );
				return enumVal->name();
			}
		}
	}
	catch ( const boost::bad_lexical_cast & ex )
	{
		ex;
		/*EngineFuncs::ConsoleError(
		va( "Can't convert '%s' to appropriate type %s",
		_args.at( 2 ).c_str(),
		protobuf::FieldDescriptor::kTypeToName[ fieldDesc->type() ] ) );*/
	}
	return "";
}

void PathPlannerNavMesh::cmdSetField(const StringVector &_args)
{
	Vector3f vFacing, vPos;
	if ( !Utils::GetLocalEyePosition( vPos ) || !Utils::GetLocalFacing( vFacing ) )
	{
		EngineFuncs::ConsoleError("can't get facing or eye position");
		SetNextState(NoOp);
		return;
	}

	NavCollision nc = FindCollision( vPos, vPos + vFacing * 2048.f );
	if( !nc.DidHit() )
	{
		EngineFuncs::ConsoleError("no sector at cursor");
		SetNextState(NoOp);
		return;
	}

	using namespace google;

	const bool mirrored = _args.at( 0 ).back() == 'm';
	NavSector & ns = m_NavSectors[ nc.HitAttrib().Fields.SectorId ];
	NavmeshIO::SectorData & sectorData = mirrored ? ns.mSectorDataMirrored : ns.mSectorData;

	const protobuf::Descriptor * desc = sectorData.GetDescriptor();

	if ( _args.size() <= 2 )
	{
		EngineFuncs::ConsoleError( va( "Usage: %s fieldname fieldvalue", _args.at( 0 ).c_str() ) );
		for ( int i = 0; i < desc->field_count(); ++i )
		{
			const protobuf::FieldDescriptor * fieldDesc = desc->field( i );

			const std::string doc = fieldDesc->options().GetExtension( NavmeshIO::doc );
			EngineFuncs::ConsoleError( va( "    %s ( %s ) = %s - %s",
				fieldDesc->lowercase_name().c_str(),
				protobuf::FieldDescriptor::kTypeToName[ fieldDesc->type() ],
				GetDisplayValue( sectorData, fieldDesc ).c_str(),
				doc.c_str() ) );
		}
		return;
	}

	const std::string fieldName = Utils::StringToLower( _args.at( 1 ).c_str() );
	const std::string fieldValue = _args.at( 2 ).c_str();

	const protobuf::FieldDescriptor * fieldDesc = desc->FindFieldByLowercaseName( fieldName.c_str() );
	if ( fieldDesc == NULL )
	{
		EngineFuncs::ConsoleError( va( "Unknown field: %s", _args.at( 1 ).c_str() ) );
		EngineFuncs::ConsoleError( "    If this should be added as a custom field, use nav_addfield instead" );
		return;
	}

	bool unsupported = false;
	try
	{
		switch( fieldDesc->type() )
		{
		case protobuf::FieldDescriptorProto_Type_TYPE_DOUBLE:
			sectorData.GetReflection()->SetDouble(
				&sectorData, fieldDesc, boost::lexical_cast<double>(  _args.at( 2 ).c_str() ) );
			break;
		case protobuf::FieldDescriptorProto_Type_TYPE_FLOAT:
			sectorData.GetReflection()->SetFloat(
				&sectorData, fieldDesc, boost::lexical_cast<float>(  _args.at( 2 ).c_str() ) );
			break;
		case protobuf::FieldDescriptorProto_Type_TYPE_INT64:
		case protobuf::FieldDescriptorProto_Type_TYPE_SFIXED64:
		case protobuf::FieldDescriptorProto_Type_TYPE_SINT64:
			sectorData.GetReflection()->SetInt64(
				&sectorData, fieldDesc, boost::lexical_cast<protobuf::int64>(  _args.at( 2 ).c_str() ) );
			break;
		case protobuf::FieldDescriptorProto_Type_TYPE_UINT64:
		case protobuf::FieldDescriptorProto_Type_TYPE_FIXED64:
			sectorData.GetReflection()->SetUInt64(
				&sectorData, fieldDesc, boost::lexical_cast<protobuf::uint64>(  _args.at( 2 ).c_str() ) );
			break;
		case protobuf::FieldDescriptorProto_Type_TYPE_INT32:
		case protobuf::FieldDescriptorProto_Type_TYPE_SFIXED32:
		case protobuf::FieldDescriptorProto_Type_TYPE_SINT32:
			sectorData.GetReflection()->SetInt32(
				&sectorData, fieldDesc, boost::lexical_cast<protobuf::int32>(  _args.at( 2 ).c_str() ) );
			break;
		case protobuf::FieldDescriptorProto_Type_TYPE_BOOL:
			sectorData.GetReflection()->SetBool(
				&sectorData, fieldDesc, boost::lexical_cast<bool>(  _args.at( 2 ).c_str() ) );
			break;
		case protobuf::FieldDescriptorProto_Type_TYPE_UINT32:
		case protobuf::FieldDescriptorProto_Type_TYPE_FIXED32:
			sectorData.GetReflection()->SetUInt32( &sectorData, fieldDesc, boost::lexical_cast<protobuf::uint32>(  _args.at( 2 ).c_str() ) );
			break;
		case protobuf::FieldDescriptorProto_Type_TYPE_STRING:
			{
				std::string str = _args.at( 2 );
				// parse multi word strings if in quotes
				if ( str[ 0 ] == '\'' || str[ 0 ] == '\"' )
				{
					for( size_t a = 3; a < _args.size(); ++a )
					{
						str += " ";
						str += _args.at( a );
					}
				}
				while ( str.size() > 0 && ( str.front() == '\'' || str.front() == '\\"' ) )
					str.erase( str.begin() );
				while ( str.size() > 0 && ( str.back() == '\'' || str.back() == '\\"' ) )
					str.pop_back();
				sectorData.GetReflection()->SetString( &sectorData, fieldDesc, str );
				break;
			}
		case protobuf::FieldDescriptorProto_Type_TYPE_GROUP:
			unsupported = true;
			break;
		case protobuf::FieldDescriptorProto_Type_TYPE_MESSAGE:
			unsupported = true;
			break;
		case protobuf::FieldDescriptorProto_Type_TYPE_BYTES:
			unsupported = true;
			break;
		case protobuf::FieldDescriptorProto_Type_TYPE_ENUM:
			{
				const protobuf::EnumValueDescriptor * enumVal = fieldDesc->enum_type()->FindValueByName( _args.at( 2 ).c_str() );
				if ( enumVal != NULL )
					sectorData.GetReflection()->SetEnum( &sectorData, fieldDesc, enumVal );
				else
				{
					EngineFuncs::ConsoleError( va( "Invalid enumeration( '%s' ) value %s",
						fieldDesc->enum_type()->name().c_str(),
						_args.at( 2 ).c_str() ) );
					EngineFuncs::ConsoleError( "Valid Values Are:" );
					for ( int i = 0; i < fieldDesc->enum_type()->value_count(); ++i )
					{
						EngineFuncs::ConsoleError(
							va( "    %s", fieldDesc->enum_type()->value( i )->name().c_str() ) );
					}
				}
				break;
			}
		}

		if ( unsupported )
		{
			EngineFuncs::ConsoleError(
				va( "Can't set unsupported message field type '%s'",
				protobuf::FieldDescriptor::kTypeToName[ fieldDesc->type() ] ) );
		}
	}
	catch ( const boost::bad_lexical_cast & ex )
	{
		EngineFuncs::ConsoleError(
			va( "Can't convert '%s' to appropriate type %s ( %s )",
			_args.at( 2 ).c_str(),
			protobuf::FieldDescriptor::kTypeToName[ fieldDesc->type() ],
			ex.what() ) );
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

STATE_ENTER(PathPlannerNavMesh, NoOp)
{
}

STATE_UPDATE(PathPlannerNavMesh, NoOp)
{
}

STATE_EXIT(PathPlannerNavMesh, NoOp)
{
}

//////////////////////////////////////////////////////////////////////////

STATE_ENTER(PathPlannerNavMesh, PlaceSector)
{
	Vector3f vPos, vNormal;
	if(!Utils::GetLocalAimPoint(vPos, &vNormal))
		return;

	m_WorkingSectorStart = vPos;
	m_WorkingSectorPlane = Plane3f( vNormal, vPos );
}

STATE_UPDATE(PathPlannerNavMesh, PlaceSector)
{
	Vector3f vPos, vNormal;
	if(!Utils::GetLocalAimPoint(vPos, &vNormal))
		return;

	m_WorkingSectorPlane = Plane3f( m_WorkingSectorPlane.Normal, vPos );

	Vector3List poly = Utils::CreatePolygon(
		vPos,
		m_WorkingSectorPlane.Normal,
		32768.f);

	RenderBuffer::AddPolygonFilled( poly, COLOR::GREEN.fade(100) );
}

STATE_EXIT(PathPlannerNavMesh, PlaceSector)
{
	if(m_ToolCancelled)
	{
		m_WorkingSector = NavSector();
		return;
	}

	Vector3f vPos, vNormal;
	if(!Utils::GetLocalAimPoint(vPos, &vNormal))
		return;

	m_WorkingSector.m_Boundary = Utils::CreatePolygon(
		vPos,
		m_WorkingSectorPlane.Normal,
		32768.f);
}

//////////////////////////////////////////////////////////////////////////

STATE_ENTER(PathPlannerNavMesh, SliceSector)
{
	if(m_WorkingSector.m_Boundary.empty())
	{
		EngineFuncs::ConsoleError("No Active Sector");
		SetNextState(NoOp);
	}

	Vector3f vPos, vNormal;
	if(!Utils::GetLocalAimPoint(vPos, &vNormal))
		return;

	m_WorkingSlicePlane = Plane3f( vNormal, vPos );
}

STATE_UPDATE(PathPlannerNavMesh, SliceSector)
{
	Vector3f vEye, vPos, vNormal;
	if(!Utils::GetLocalAimPoint(vPos, &vNormal) ||
		!Utils::GetLocalEyePosition(vEye))
		return;

	bool bSnapped = false;
	vPos = _SectorVertWithin(vEye, vPos, SNAP_RADIUS, &bSnapped);
	if(bSnapped)
		m_CursorColor = COLOR::ORANGE;

	m_WorkingSlicePlane = Plane3f( m_WorkingSlicePlane.Normal, vPos );

	Vector3List slicePoly = Utils::CreatePolygon(
		vPos,
		m_WorkingSlicePlane.Normal,
		1024.f);

	RenderBuffer::AddPolygonFilled( slicePoly, COLOR::GREEN.fade(100) );
}

STATE_EXIT(PathPlannerNavMesh, SliceSector)
{
	if(m_ToolCancelled)
		return;

	const Plane3f clipPlane = ( m_WorkingSlicePlane.WhichSide( m_WorkingSectorStart ) > 0 ) ?
		-m_WorkingSlicePlane : m_WorkingSlicePlane;

	m_WorkingSector.m_Boundary =
		Utils::ClipPolygonToPlanes( m_WorkingSector.m_Boundary, clipPlane );
}

//////////////////////////////////////////////////////////////////////////

STATE_ENTER(PathPlannerNavMesh, SliceSectorWithSector)
{
	if(m_WorkingSector.m_Boundary.empty())
	{
		EngineFuncs::ConsoleError("No Active Sector");
		SetNextState(NoOp);
		return;
	}

	//////////////////////////////////////////////////////////////////////////
	Vector3f vFacing, vPos;
	if(!Utils::GetLocalEyePosition(vPos) || !Utils::GetLocalFacing(vFacing))
	{
		EngineFuncs::ConsoleError("can't get facing or eye position");
		SetNextState(NoOp);
		return;
	}

	NavCollision nc = FindCollision(vPos, vPos + vFacing * 2048.f);
	if(!nc.DidHit())
	{
		EngineFuncs::ConsoleError("no sector at cursor");
		SetNextState(NoOp);
		return;
	}
	//////////////////////////////////////////////////////////////////////////

	m_WorkingSlicePlane = Plane3f( nc.HitNormal(), nc.HitPosition() );

	const Plane3f clipPlane = ( m_WorkingSlicePlane.WhichSide( m_WorkingSectorStart ) > 0 ) ?
		-m_WorkingSlicePlane : m_WorkingSlicePlane;

	m_WorkingSector.m_Boundary = Utils::ClipPolygonToPlanes( m_WorkingSector.m_Boundary, clipPlane );

	SetNextState(NoOp);
}

STATE_UPDATE(PathPlannerNavMesh, SliceSectorWithSector)
{
}

STATE_EXIT(PathPlannerNavMesh, SliceSectorWithSector)
{
}

//////////////////////////////////////////////////////////////////////////

STATE_ENTER(PathPlannerNavMesh, EditSector)
{
}

STATE_UPDATE(PathPlannerNavMesh, EditSector)
{
	Vector3f vLocalPos, vLocalAim;

	if(!Utils::GetLocalEyePosition(vLocalPos) ||
		!Utils::GetLocalFacing(vLocalAim))
	{
		SetNextState(NoOp);
		return;
	}

	NavCollision nc = FindCollision(vLocalPos, vLocalPos + vLocalAim * 1024.f);
	if(!nc.DidHit() && !nc.HitAttrib().Fields.Mirrored)
	{
		EngineFuncs::ConsoleError("No Nav Sector Found");
		SetNextState(NoOp);
		return;
	}

	m_WorkingSectorStart = nc.HitPosition();
	m_WorkingSector = m_NavSectors[nc.HitAttrib().Fields.SectorId];
	m_NavSectors.erase(m_NavSectors.begin() + nc.HitAttrib().Fields.SectorId);

	InitSectors();

	SetNextState(NoOp);
}

STATE_EXIT(PathPlannerNavMesh, EditSector)
{
}

//////////////////////////////////////////////////////////////////////////

STATE_ENTER(PathPlannerNavMesh, SplitSector)
{
	if(m_WorkingSector.m_Boundary.empty())
	{
		EngineFuncs::ConsoleError("No Active Sector");
		SetNextState(NoOp);
	}

	Vector3f vPos, vNormal;
	if(!Utils::GetLocalAimPoint(vPos, &vNormal))
		return;

	m_WorkingSlicePlane = Plane3f( vNormal, vPos );
}

STATE_UPDATE(PathPlannerNavMesh, SplitSector)
{
	Vector3f vPos, vNormal;
	if(!Utils::GetLocalAimPoint(vPos, &vNormal))
		return;

	m_WorkingSlicePlane = Plane3f( m_WorkingSlicePlane.Normal, vPos );

	Vector3List slicePoly = Utils::CreatePolygon(
		vPos,
		m_WorkingSlicePlane.Normal,
		1024.f);

	RenderBuffer::AddPolygonFilled( slicePoly, COLOR::RED.fade(100) );
}

STATE_EXIT(PathPlannerNavMesh, SplitSector)
{
	if(m_ToolCancelled)
		return;

	Vector3List s1 = Utils::ClipPolygonToPlanes( m_WorkingSector.m_Boundary, m_WorkingSlicePlane );
	Vector3List s2 = Utils::ClipPolygonToPlanes( m_WorkingSector.m_Boundary, -m_WorkingSlicePlane );

	// add the 2 new sectors
	NavSector ns = m_WorkingSector;
	if(!s1.empty())
	{
		m_WorkingSector.m_Boundary = s1;
		m_NavSectors.push_back(m_WorkingSector);
	}
	if(!s2.empty())
	{
		m_WorkingSector.m_Boundary = s2;
		m_NavSectors.push_back(m_WorkingSector);
	}

	m_WorkingSector = NavSector();

	InitSectors();
}

//////////////////////////////////////////////////////////////////////////

STATE_ENTER(PathPlannerNavMesh, TraceSector)
{
	Vector3f vEye, vAimPos;
	Utils::GetLocalEyePosition(vEye);
	Utils::GetLocalAimPoint(vAimPos);

	// see if this closes the loop.
	if(m_WorkingManualSector.size()>2)
	{
		if(Length(vAimPos,m_WorkingManualSector[0]) <= SNAP_RADIUS)
		{
			m_WorkingManualSector.pop_back(); // remove the last pt
			vAimPos = m_WorkingManualSector[0];
			m_WorkingSector.m_Boundary = m_WorkingManualSector;
			m_WorkingManualSector.resize(0);
			SetNextState(NoOp);
			return;
		}
	}

	vAimPos = _SectorVertWithin(vEye, vAimPos, SNAP_RADIUS);

	if(m_WorkingManualSector.empty())
	{
		m_WorkingManualSector.push_back(vAimPos);
		m_WorkingManualSector.push_back(vAimPos);
	}
	else
	{
		m_WorkingManualSector.push_back(vAimPos);
	}
}

STATE_UPDATE(PathPlannerNavMesh, TraceSector)
{
	if(m_WorkingManualSector.empty())
		return;

	Vector3f vEye, vAimPos;
	Utils::GetLocalEyePosition(vEye);
	Utils::GetLocalAimPoint(vAimPos);

	bool bSnapped = false;

	// see if we can snap to the start.
	if(Length(vAimPos,m_WorkingManualSector[0]) <= SNAP_RADIUS)
	{
		m_CursorColor = COLOR::ORANGE;
		vAimPos = m_WorkingManualSector[0];
	}
	else
		vAimPos = _SectorVertWithin(vEye, vAimPos,SNAP_RADIUS,&bSnapped);

	if(bSnapped)
		m_CursorColor = COLOR::ORANGE;

	// update the last point
	m_WorkingManualSector[m_WorkingManualSector.size()-1] = vAimPos;

	RenderBuffer::AddPolygonSilouette( m_WorkingManualSector, COLOR::GREEN );
}

STATE_EXIT(PathPlannerNavMesh, TraceSector)
{
	if(m_ToolCancelled)
	{
		m_WorkingManualSector.resize(0);
		return;
	}
}

//////////////////////////////////////////////////////////////////////////

STATE_ENTER(PathPlannerNavMesh, GroundSector)
{
}

STATE_UPDATE(PathPlannerNavMesh, GroundSector)
{
	Vector3f vLocalPos, vLocalAim;

	if(!Utils::GetLocalEyePosition(vLocalPos) ||
		!Utils::GetLocalFacing(vLocalAim))
	{
		SetNextState(NoOp);
		return;
	}

	NavCollision nc = FindCollision(vLocalPos, vLocalPos + vLocalAim * 1024.f);
	if(!nc.DidHit() && !nc.HitAttrib().Fields.Mirrored)
	{
		EngineFuncs::ConsoleError("No Nav Sector Found");
		SetNextState(NoOp);
		return;
	}

	NavSector &ns = m_NavSectors[nc.HitAttrib().Fields.SectorId];

	obTraceResult tr;
	for(obuint32 i = 0; i < ns.m_Boundary.size(); ++i)
	{
		Vector3f vPt = ns.m_Boundary[i];
		EngineFuncs::TraceLine(tr,
			vPt+Vector3f(0.f,0.f,1.f),
			vPt+Vector3f(0.f,0.f,-1024.f),
			NULL,
			TR_MASK_FLOODFILL,
			-1,
			False);

		ns.m_Boundary[i].Z() = tr.m_Endpos[2];
	}

	InitSectors();

	SetNextState(NoOp);
}

STATE_EXIT(PathPlannerNavMesh, GroundSector)
{
}

//////////////////////////////////////////////////////////////////////////

STATE_ENTER(PathPlannerNavMesh, CommitSector)
{
}

STATE_UPDATE(PathPlannerNavMesh, CommitSector)
{
	bool bInitSectors = false;

	//////////////////////////////////////////////////////////////////////////

	if(m_WorkingSector.m_Boundary.size() > 2)
	{
		m_NavSectors.push_back(m_WorkingSector);
		bInitSectors = true;
	}
	m_WorkingSector = NavSector();

	//////////////////////////////////////////////////////////////////////////

	if(m_WorkingManualSector.size() > 2)
	{
		NavSector ns;
		ns.m_Boundary = m_WorkingManualSector;
		m_NavSectors.push_back(ns);
		bInitSectors = true;
	}
	m_WorkingManualSector.resize(0);

	if(bInitSectors)
	{
		InitSectors();
	}
}

STATE_EXIT(PathPlannerNavMesh, CommitSector)
{
}

//////////////////////////////////////////////////////////////////////////

STATE_ENTER(PathPlannerNavMesh, PlaceBorder)
{
}

STATE_UPDATE(PathPlannerNavMesh, PlaceBorder)
{
}

STATE_EXIT(PathPlannerNavMesh, PlaceBorder)
{
}

//////////////////////////////////////////////////////////////////////////

STATE_ENTER(PathPlannerNavMesh, MirrorSectors)
{
}

STATE_UPDATE(PathPlannerNavMesh, MirrorSectors)
{
}

STATE_EXIT(PathPlannerNavMesh, MirrorSectors)
{
}

//////////////////////////////////////////////////////////////////////////

const float SpanHeightMin = 32.0f;
const float SpanStepHeight = 32.0f;

static bool TestForValidNode( Vector3f & spanPos, float & spanHeight )
{
	const Vector3f dn0(0.f,0.f,SpanStepHeight);
	const Vector3f dn1(0.f,0.f,-1024.f);

	const Vector3f up0(0.f,0.f,1.f);
	const Vector3f up1(0.f,0.f,1024.f);

	obTraceResult tr;
	EngineFuncs::TraceLine(tr,spanPos+dn0,spanPos+dn1,NULL,TR_MASK_FLOODFILL,-1,False);
	if ( tr.m_Fraction < 1.0f )
	{
		if ( tr.m_Normal[ 2 ] < 0.707f )
			return false;

		spanPos = tr.m_Endpos;
		EngineFuncs::TraceLine(tr,spanPos+up0,spanPos+up1,NULL,TR_MASK_FLOODFILL,-1,False);

		spanHeight = (spanPos+up1).Z() - spanPos.Z();
		if ( tr.m_Fraction < 1.0f )
			spanHeight = tr.m_Endpos[2] - spanPos.Z();

		if ( spanHeight > SpanHeightMin )
		{
			return true;
		}
	}
	return false;
}

STATE_ENTER(PathPlannerNavMesh, FloodSpanMap)
{
	EngineFuncs::ConsoleMessage( "Flooding Influence Map..." );
	m_ToolCancelled = false;
}

STATE_UPDATE(PathPlannerNavMesh, FloodSpanMap)
{
	if ( m_SpanFrontier.empty() )
		SetNextState( NoOp );

	const float cs = m_SpanMap->GetCellSize();

	const Vector3f step[] =
	{
		Vector3f( -cs, 0.0f, SpanStepHeight ),
		Vector3f(  cs, 0.0f, SpanStepHeight ),
		Vector3f( 0.0f, -cs, SpanStepHeight ),
		Vector3f( 0.0f,  cs, SpanStepHeight ),
	};
	const int stepdirs = sizeof(step) / sizeof(step[0]);

	static int maxIterations = std::numeric_limits<int>::max();
	int iterations = maxIterations;
	while ( !m_SpanFrontier.empty() && (iterations--) > 0 )
	{
		Vector3f spanPos = m_SpanFrontier.front();
		m_SpanFrontier.pop();

		float spanHeight = 0.0f;
		if ( TestForValidNode( spanPos, spanHeight ) && m_SpanMap->AddOpenSpan( spanPos, spanHeight ) )
		{
			//RenderBuffer::AddCircle( spanPos, 16.0f, COLOR::GREEN, 0.1f );

			for ( int i = 0; i < stepdirs; ++i )
			{
				Vector3f expandPos = spanPos + step[ i ];

				obTraceResult tr;
				EngineFuncs::TraceLine(tr,
					spanPos + Vector3f(0,0,4),
					expandPos,
					NULL,
					TR_MASK_FLOODFILL,
					-1,
					False);

				if ( tr.m_Fraction == 1.0f )
					m_SpanFrontier.push( expandPos );
			}
		}
	}
}

STATE_EXIT(PathPlannerNavMesh, FloodSpanMap)
{
	if ( m_ToolCancelled )
	{
		OB_DELETE( m_SpanMap );
		OB_DELETE( m_Influence );
		return;
	}

	unsigned int numIndices = m_SpanMap->IndexSpanNodes();

	EngineFuncs::ConsoleMessage(va("Finalize %d x %d ( %d nodes ) span map( %s )",
		m_SpanMap->GetNumCellsX(),
		m_SpanMap->GetNumCellsY(),
		numIndices,
		Utils::FormatByteString( m_SpanMap->CalculateMemUsage() ).c_str() ) );
}