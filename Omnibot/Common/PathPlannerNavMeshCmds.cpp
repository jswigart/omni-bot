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
#include "ProtoBufUtility.h"

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

	SetEx("nav_stats", "Step to the next nav process.",
		this, &PathPlannerNavMesh::cmdNavStats);

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
	SetEx("nav_dupesector", "Begins editing a sector so further slicing may be done.",
		this, &PathPlannerNavMesh::cmdDupeSector);
	SetEx("nav_splitsector", "Splits a sector into 2 sectors along a plane.",
		this, &PathPlannerNavMesh::cmdSplitSector);
	SetEx("nav_groundsector", "Grounds all the vertices in the sector.",
		this, &PathPlannerNavMesh::cmdGroundSector);
	SetEx("nav_updatecontents", "Updates the contents flags of all sectors.",
		this, &PathPlannerNavMesh::cmdUpdateContents);

	SetEx("nav_sectoredgedrop", "Drops the corrent sector edge to the cursor height.",
		this, &PathPlannerNavMesh::cmdSectorEdgeDrop);

	SetEx("nav_portalcreate", "Creates a manual portal edge.",
		this, &PathPlannerNavMesh::cmdPortalCreate);

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
	REGISTER_STATE(PathPlannerNavMesh,MoveSector);
	REGISTER_STATE(PathPlannerNavMesh,SplitSector);
	REGISTER_STATE(PathPlannerNavMesh,TraceSector);
	REGISTER_STATE(PathPlannerNavMesh,GroundSector);
	REGISTER_STATE(PathPlannerNavMesh,CommitSector);
	REGISTER_STATE(PathPlannerNavMesh,MirrorSectors);
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

void PathPlannerNavMesh::cmdNavStats(const StringVector &_args)
{
	if(!m_PlannerFlags.CheckFlag(NAV_VIEW))
		return;

	size_t numPortals = 0;
	for ( size_t i = 0; i < mRuntimeSectors.size(); ++i )
		numPortals += mRuntimeSectors[ i ].mPortals.size();

	EngineFuncs::ConsoleMessage( va( "%d Sectors", mNavSectors.size() ) );
	EngineFuncs::ConsoleMessage( va( "%d Total Sectors(with mirrored)", mRuntimeSectors.size() ) );
	EngineFuncs::ConsoleMessage( va( "%d Portals", numPortals ) );
}

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
	for(obuint32 s = 0; s < mNavSectors.size(); ++s)
	{
		const NavSector &sec = mNavSectors[s];

		for(obuint32 v = 0; v < sec.mPoly.size(); ++v)
		{
			DistPoint3Segment3f dist( sec.mPoly[v], Segment3f( _pos1,_pos2 ) );
			const float fLenSq = dist.GetSquared();
			if( fLenSq < fRangeSq && fLenSq < fClosest )
			{
				r = sec.mPoly[v];
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

void PathPlannerNavMesh::cmdDupeSector(const StringVector &_args)
{
	if(!m_PlannerFlags.CheckFlag(NAV_VIEW))
		return;

	Vector3f vLocalPos, vLocalAim;
	if(!Utils::GetLocalEyePosition(vLocalPos) || !Utils::GetLocalFacing(vLocalAim))
	{
		SetNextState(NoOp);
		return;
	}

	NavCollision nc = FindCollision(vLocalPos, vLocalPos + vLocalAim * 1024.f);
	if(!nc.DidHit() && !nc.HitAttrib().Fields.Mirrored)
	{
		EngineFuncs::ConsoleError("No Nav Sector Found");
		return;
	}

	m_WorkingSectorStart = nc.HitPosition();
	m_WorkingSector = mNavSectors[nc.HitAttrib().Fields.SectorId];

	m_ToolCancelled = false;
	SetNextState(MoveSector);
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

	if ( mCurrentTool )
	{
		mCurrentTool->Commit( this );

		// should we always delete the tool?
		OB_DELETE( mCurrentTool );
	}
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

	mNavSectors.erase(mNavSectors.begin() + nc.HitAttrib().Fields.SectorId);
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
		for(obuint32 s = 0; s < mNavSectors.size(); ++s)
		{
			mNavSectors[s].mMirror = mir;
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
		NavSector *pNavSector = &mNavSectors[nc.HitAttrib().Fields.SectorId];
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
	Vector3f vPos;
	if(!Utils::GetLocalEyePosition(vPos))
	{
		EngineFuncs::ConsoleError("can't get facing or eye position");
		return;
	}

	RuntimeNavSector * ns = GetSectorAt( vPos );
	if ( ns )
	{
		ns->BuildSectorPortals( this );
	}

	size_t totalPortals = 0;
	for ( size_t i = 0; i < mRuntimeSectors.size(); ++i )
		totalPortals += mRuntimeSectors[ i ].mPortals.size();

	EngineFuncs::ConsoleMessage( va( "Found %d portals for this sector( total %d )", ns->mPortals.size(), totalPortals ) );
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

	Obstacle obs;
	obs.mPoly.push_back( vObsPoint + Vector3f( -len, -wid, 0.0f ) );
	obs.mPoly.push_back( vObsPoint + Vector3f(  len, -wid, 0.0f ) );
	obs.mPoly.push_back( vObsPoint + Vector3f(  len,  wid, 0.0f ) );
	obs.mPoly.push_back( vObsPoint + Vector3f( -len,  wid, 0.0f ) );
	mPendingObstacles.push_back( obs );
}

void PathPlannerNavMesh::cmdInfluenceMapCreate(const StringVector &_args)
{
	const float fmin = std::numeric_limits<float>::lowest();
	const float fmax = std::numeric_limits<float>::max();

	AABB mapbounds;
	mapbounds.Set( Vector3f( fmax,fmax,fmax ), Vector3f( fmin, fmin, fmin ) );
	for ( RuntimeSectorList::iterator s = mRuntimeSectors.begin(); s != mRuntimeSectors.end(); ++s)
	{
		for ( Vector3List::iterator v = s->mPoly.begin(); v != s->mPoly.end(); ++v)
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
	RuntimeNavSector * ns = GetSectorAt(vAimPt);
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

	// cache all sector center positions so we don't include that as part of the benchmark
	Vector3List sectorCenters;
	sectorCenters.reserve( mRuntimeSectors.size() );
	for(obuint32 i = 0; i < mRuntimeSectors.size(); ++i)
	{
		sectorCenters.push_back( mRuntimeSectors[ i ].CalculateCenter() );
	}

	Timer tme;
	tme.Reset();
	for ( size_t s1 = 0; s1 < sectorCenters.size(); ++s1 )
	{
		for ( size_t s2 = 0; s2 < sectorCenters.size(); ++s2 )
		{
			PlanPathToGoal(NULL,sectorCenters[ s1 ],sectorCenters[ s2 ],0);
		}
	}
	const double dTimeTaken = tme.GetElapsedSeconds();

	EngineFuncs::ConsoleMessage(va("generated %d paths in %f seconds: %f paths/sec",
		(sectorCenters.size()*sectorCenters.size()),
		dTimeTaken,
		dTimeTaken != 0.0f ? (float)(sectorCenters.size()*sectorCenters.size()) / dTimeTaken : 0.0f));
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

	// cache all sector center positions so we don't include that as part of the benchmark
	Vector3List sectorCenters;
	sectorCenters.reserve( mRuntimeSectors.size() );
	for(obuint32 i = 0; i < mRuntimeSectors.size(); ++i)
	{
		sectorCenters.push_back( mRuntimeSectors[ i ].CalculateCenter() );
	}

	Timer tme;

	obuint32 iHits = 0, iMisses = 0;
	tme.Reset();
	for(obuint32 i = 0; i < iNumIterations; ++i)
	{
		for(obuint32 s = 0; s < sectorCenters.size(); ++s)
		{
			RuntimeNavSector * sector = GetSectorAt( sectorCenters[ s ] );
			if ( sector )
				++iHits;
			else
				++iMisses;
		}
	}

	const double dTimeTaken = tme.GetElapsedSeconds();

	EngineFuncs::ConsoleMessage( va( "BenchmarkGetNavPoint() %d calls, %d hits, %d misses : avg %f per second",
		mRuntimeSectors.size() * iNumIterations,
		iHits,
		iMisses,
		dTimeTaken != 0.0f ? ((float)(mRuntimeSectors.size() * iNumIterations) / dTimeTaken) : 0.0f ) );
}

void PathPlannerNavMesh::cmdUpdateContents(const StringVector &_args)
{
	for ( size_t i = 0; i < mRuntimeSectors.size(); ++i )
	{
		RuntimeNavSector & ans = mRuntimeSectors[ i ];
		ans.UpdateAutoFlags();
	}
	InitSectors();
}

class ToolSectorEdgeDrop : public EditTool<PathPlannerNavMesh>
{
	virtual bool Start( PathPlannerNavMesh * system )
	{
		Vector3f aimPt;
		if( !Utils::GetLocalAimPoint( aimPt ) )
			return false;

		mSector = system->GetSectorAtCursor();
		if ( mSector == NULL )
			return false;

		m0 = m1 = 0;

		float nearest = std::numeric_limits<float>::max();
		for ( size_t i = 1; i <= mSector->mPoly.size(); ++i )
		{
			const int s0 = i-1;
			const int s1 = i%mSector->mPoly.size();
			DistPoint3Segment3f dist( aimPt, Segment3f( mSector->mPoly[ s0 ], mSector->mPoly[ s1 ] ) );

			const float d = dist.Get();
			if ( d < nearest )
			{
				nearest = d;
				m0 = s0;
				m1 = s1;
			}
		}

		return true;
	}
	virtual void Update( PathPlannerNavMesh * system )
	{
		Vector3f aimPt;
		if( !Utils::GetLocalAimPoint( aimPt ) )
			return;

		mSector->mPoly[ m0 ].Z() = aimPt.Z();
		mSector->mPoly[ m1 ].Z() = aimPt.Z();

		// update the runtime sectors too
		for ( size_t i = 0; i < system->mRuntimeSectors.size(); ++i )
		{
			if ( system->mRuntimeSectors[ i ].mSector == mSector )
			{
				system->mRuntimeSectors[ i ].mPoly[ m0 ].Z() = aimPt.Z();
				system->mRuntimeSectors[ i ].mPoly[ m1 ].Z() = aimPt.Z();
			}
		}
	}
	virtual void Commit( PathPlannerNavMesh * system )
	{
		for ( size_t i = 0; i < system->mRuntimeSectors.size(); ++i )
		{
			if ( system->mRuntimeSectors[ i ].mSector == mSector )
			{
				system->mRuntimeSectors[ i ].mUpdatePortals = true;
			}
		}
		system->InitSectors();
	}
private:
	PathPlannerNavMesh::NavSector *		mSector;
	size_t								m0,m1;
};

void PathPlannerNavMesh::cmdSectorEdgeDrop(const StringVector &_args)
{
	mCurrentTool = new ToolSectorEdgeDrop;
	if ( !mCurrentTool->Start( this ) )
		OB_DELETE( mCurrentTool );
}

class ToolPortalCreate : public EditTool<PathPlannerNavMesh>
{
	virtual bool Start( PathPlannerNavMesh * system )
	{
		Vector3f aimPt;
		if( !Utils::GetLocalAimPoint( aimPt ) )
			return false;

		/*mSector = system->GetSectorAtCursor();
		if ( mSector == NULL )
		return false;*/

		/*float nearest = std::numeric_limits<float>::max();
		for ( size_t i = 1; i <= mSector->mPoly.size(); ++i )
		{
		const int s0 = i-1;
		const int s1 = i%mSector->mPoly.size();
		DistPoint3Segment3f dist( aimPt, Segment3f( mSector->mPoly[ s0 ], mSector->mPoly[ s1 ] ) );

		const float d = dist.Get();
		if ( d < nearest )
		{
		nearest = d;
		m0 = s0;
		m1 = s1;
		}
		}*/

		return true;
	}
	virtual void Update( PathPlannerNavMesh * system )
	{
	}
	virtual void Commit( PathPlannerNavMesh * system )
	{
	}
private:
};

void PathPlannerNavMesh::cmdPortalCreate(const StringVector &_args)
{
	mCurrentTool = new ToolPortalCreate;
	if ( !mCurrentTool->Start( this ) )
		OB_DELETE( mCurrentTool );
}

void PathPlannerNavMesh::cmdSetField(const StringVector &_args)
{
	using namespace google;

	const NavmeshIO::SectorData & msgDef = NavmeshIO::SectorData::default_instance();

	const bool mirrored = _args.at( 0 ).back() == 'm';

	NavSector * ns = GetSectorAtCursor();
	if ( ns == NULL )
		return;

	NavmeshIO::SectorData & msg = mirrored ? ns->mSectorDataMirrored : ns->mSectorData;

	const protobuf::Descriptor * desc = msg.GetDescriptor();

	if ( _args.size() <= 2 )
	{
		EngineFuncs::ConsoleError( va( "Usage: %s fieldname fieldvalue", _args.at( 0 ).c_str() ) );
		for ( int i = 0; i < desc->field_count(); ++i )
		{
			const protobuf::FieldDescriptor * fieldDesc = desc->field( i );

			const bool hidden = fieldDesc->options().GetExtension( NavmeshIO::hidden );
			if ( hidden )
				continue;

			const std::string doc = fieldDesc->options().GetExtension( NavmeshIO::doc );
			EngineFuncs::ConsoleError( va( "    %s ( %s ) = %s - %s",
				fieldDesc->lowercase_name().c_str(),
				protobuf::FieldDescriptor::kTypeToName[ fieldDesc->type() ],
				GetFieldString( msg, fieldDesc ).c_str(),
				doc.c_str() ) );
		}
		return;
	}

	const bool wasOnMover = msg.onmover();

	const std::string fieldName = Utils::StringToLower( _args.at( 1 ).c_str() );
	const std::string fieldValue = _args.at( 2 ).c_str();

	const protobuf::FieldDescriptor * fieldDesc = desc->FindFieldByLowercaseName( fieldName.c_str() );

	const bool hidden = fieldDesc ? fieldDesc->options().GetExtension( NavmeshIO::hidden ) : false;
	const bool settable = fieldDesc ? fieldDesc->options().GetExtension( NavmeshIO::settable ) : false;
	if ( fieldDesc == NULL )
	{
		EngineFuncs::ConsoleError( va( "Unknown field: %s", _args.at( 1 ).c_str() ) );
		EngineFuncs::ConsoleError( "    If this should be added as a custom field, use nav_addfield instead" );
		return;
	}
	if ( hidden )
	{
		EngineFuncs::ConsoleError( va( "Field: %s is hidden and cannot be set", _args.at( 1 ).c_str() ) );
		return;
	}
	if ( settable )
	{
		EngineFuncs::ConsoleError( va( "Field: %s is not settable", _args.at( 1 ).c_str() ) );
		return;
	}

	const protobuf::Reflection * refl = msg.GetReflection();

	bool unsupported = false;
	try
	{
		switch( fieldDesc->type() )
		{
		case protobuf::FieldDescriptorProto_Type_TYPE_DOUBLE:
			{
				const double val = boost::lexical_cast<double>( _args.at( 2 ).c_str() );
				if ( val == refl->GetDouble( msgDef, fieldDesc ) )
					refl->ClearField( &msg, fieldDesc );
				else
					refl->SetDouble( &msg, fieldDesc, val );
				break;
			}
		case protobuf::FieldDescriptorProto_Type_TYPE_FLOAT:
			{
				const float val = boost::lexical_cast<float>( _args.at( 2 ).c_str() );
				if ( val == refl->GetFloat( msgDef, fieldDesc ) )
					refl->ClearField( &msg, fieldDesc );
				else
					refl->SetFloat( &msg, fieldDesc, val );
				break;
			}
		case protobuf::FieldDescriptorProto_Type_TYPE_INT64:
		case protobuf::FieldDescriptorProto_Type_TYPE_SFIXED64:
		case protobuf::FieldDescriptorProto_Type_TYPE_SINT64:
			{
				const protobuf::int64 val = boost::lexical_cast<protobuf::int64>( _args.at( 2 ).c_str() );
				if ( val == refl->GetInt64( msgDef, fieldDesc ) )
					refl->ClearField( &msg, fieldDesc );
				else
					refl->SetInt64( &msg, fieldDesc, val );
				break;
			}
		case protobuf::FieldDescriptorProto_Type_TYPE_UINT64:
		case protobuf::FieldDescriptorProto_Type_TYPE_FIXED64:
			{
				const protobuf::uint64 val = boost::lexical_cast<protobuf::uint64>( _args.at( 2 ).c_str() );
				if ( val == refl->GetUInt64( msgDef, fieldDesc ) )
					refl->ClearField( &msg, fieldDesc );
				else
					refl->SetUInt64( &msg, fieldDesc, val );
				break;
			}
		case protobuf::FieldDescriptorProto_Type_TYPE_INT32:
		case protobuf::FieldDescriptorProto_Type_TYPE_SFIXED32:
		case protobuf::FieldDescriptorProto_Type_TYPE_SINT32:
			{
				const protobuf::int32 val = boost::lexical_cast<protobuf::int32>( _args.at( 2 ).c_str() );
				if ( val == refl->GetInt32( msgDef, fieldDesc ) )
					refl->ClearField( &msg, fieldDesc );
				else
					refl->SetInt32( &msg, fieldDesc, val );
				break;
			}
		case protobuf::FieldDescriptorProto_Type_TYPE_BOOL:
			{
				const bool val = boost::lexical_cast<bool>( _args.at( 2 ).c_str() );
				if ( val == refl->GetBool( msgDef, fieldDesc ) )
					refl->ClearField( &msg, fieldDesc );
				else
					refl->SetBool( &msg, fieldDesc, val );
				break;
			}
		case protobuf::FieldDescriptorProto_Type_TYPE_UINT32:
		case protobuf::FieldDescriptorProto_Type_TYPE_FIXED32:
			{
				const protobuf::uint32 val = boost::lexical_cast<protobuf::uint32>( _args.at( 2 ).c_str() );
				if ( val == refl->GetUInt32( msgDef, fieldDesc ) )
					refl->ClearField( &msg, fieldDesc );
				else
					refl->SetUInt32( &msg, fieldDesc, val );
				break;
			}
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

				if ( str == refl->GetString( msgDef, fieldDesc ) )
					refl->ClearField( &msg, fieldDesc );
				else
					refl->SetString( &msg, fieldDesc, str );
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
				{
					if ( enumVal == refl->GetEnum( msgDef, fieldDesc ) )
						refl->ClearField( &msg, fieldDesc );
					else
						refl->SetEnum( &msg, fieldDesc, enumVal );
				}
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
			return;
		}

		bool clearMoverState = !msg.onmover();
		if ( !wasOnMover && msg.onmover() )
		{
			for ( size_t i = 0; i < mRuntimeSectors.size(); ++i )
			{
				if ( mRuntimeSectors[ i ].mSectorData == &msg )
				{
					if ( !mRuntimeSectors[ i ].InitSectorTransform() )
					{
						EngineFuncs::ConsoleError( "Unable to Initialize Mover Offset" );
						clearMoverState = true;
					}
				}
			}
		}

		if ( clearMoverState )
		{
			msg.clear_onmover();
			msg.clear_mover();
			msg.clear_localoffsets();
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

	m_WorkingSector.mPoly = Utils::CreatePolygon(
		vPos,
		m_WorkingSectorPlane.Normal,
		32768.f);
}

//////////////////////////////////////////////////////////////////////////

STATE_ENTER(PathPlannerNavMesh, SliceSector)
{
	if(m_WorkingSector.mPoly.empty())
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

	m_WorkingSector.mPoly =
		Utils::ClipPolygonToPlanes( m_WorkingSector.mPoly, clipPlane );
}

//////////////////////////////////////////////////////////////////////////

STATE_ENTER(PathPlannerNavMesh, SliceSectorWithSector)
{
	if(m_WorkingSector.mPoly.empty())
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

	m_WorkingSector.mPoly = Utils::ClipPolygonToPlanes( m_WorkingSector.mPoly, clipPlane );

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
	m_WorkingSector = mNavSectors[nc.HitAttrib().Fields.SectorId];
	mNavSectors.erase(mNavSectors.begin() + nc.HitAttrib().Fields.SectorId);

	InitSectors();

	SetNextState(NoOp);
}

STATE_EXIT(PathPlannerNavMesh, EditSector)
{
}

//////////////////////////////////////////////////////////////////////////

STATE_ENTER(PathPlannerNavMesh, MoveSector)
{
}

STATE_UPDATE(PathPlannerNavMesh, MoveSector)
{
	Vector3f vAimPt, vAimNormal;
	if ( !Utils::GetLocalAimPoint( vAimPt, &vAimNormal ) )
	{
		SetNextState(NoOp);
		return;
	}

	m_WorkingSector.mPlane = Plane3f( m_WorkingSector.mPlane.Normal, vAimPt );
	m_WorkingSector.SetPointsToPlane();
}

STATE_EXIT(PathPlannerNavMesh, MoveSector)
{
	if(m_ToolCancelled)
		return;

	InitSectors();
}

//////////////////////////////////////////////////////////////////////////

STATE_ENTER(PathPlannerNavMesh, SplitSector)
{
	if(m_WorkingSector.mPoly.empty())
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

	Vector3List s1 = Utils::ClipPolygonToPlanes( m_WorkingSector.mPoly, m_WorkingSlicePlane );
	Vector3List s2 = Utils::ClipPolygonToPlanes( m_WorkingSector.mPoly, -m_WorkingSlicePlane );

	// add the 2 new sectors
	NavSector ns = m_WorkingSector;
	if(!s1.empty())
	{
		m_WorkingSector.mPoly = s1;
		mNavSectors.push_back(m_WorkingSector);
	}
	if(!s2.empty())
	{
		m_WorkingSector.mPoly = s2;
		mNavSectors.push_back(m_WorkingSector);
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
			m_WorkingSector.mPoly = m_WorkingManualSector;
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

	NavSector &ns = mNavSectors[nc.HitAttrib().Fields.SectorId];

	obTraceResult tr;
	for(obuint32 i = 0; i < ns.mPoly.size(); ++i)
	{
		Vector3f vPt = ns.mPoly[i];
		EngineFuncs::TraceLine(tr,
			vPt+Vector3f(0.f,0.f,1.f),
			vPt+Vector3f(0.f,0.f,-1024.f),
			NULL,
			TR_MASK_FLOODFILL,
			-1,
			False);

		ns.mPoly[i].Z() = tr.m_Endpos[2];
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
	//////////////////////////////////////////////////////////////////////////

	if ( m_WorkingSector.mPoly.size() > 2 )
	{
		mNavSectors.push_back( m_WorkingSector );
	}
	m_WorkingSector = NavSector();

	//////////////////////////////////////////////////////////////////////////

	if ( m_WorkingManualSector.size() > 2 )
	{
		NavSector ns;
		ns.mPoly = m_WorkingManualSector;
		mNavSectors.push_back(ns);
	}
	m_WorkingManualSector.resize(0);

	InitSectors();

	SetNextState( NoOp );
}

STATE_EXIT(PathPlannerNavMesh, CommitSector)
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