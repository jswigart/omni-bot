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
#include "IGame.h"
#include "Client.h"
#include "Timer.h"
#include "ProtoBufUtility.h"
#include "RenderBuffer.h"

#include "google/protobuf/descriptor.h"

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
	SetEx("nav_enableobstacles", "Turn on/off runtime obstacles.",
		this, &PathPlannerNavMesh::cmdNavDisableObstacles);
	SetEx("nav_showobstacles", "Turn on/off runtime obstacles rendering.",
		this, &PathPlannerNavMesh::cmdNavShowObstacles);	
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
	SetEx("nav_tracepoly", "Starts a sector polygon.",
		this, &PathPlannerNavMesh::cmdTracePoly);

	/*SetEx("nav_loadobj", "Loads navmesh from obj file.",
	this, &PathPlannerNavMesh::cmdLoadObj);
	SetEx("nav_loadmap", "Loads navmesh from map file.",
	this, &PathPlannerNavMesh::cmdLoadMap);*/

	SetEx("nav_poly", "Creates a plane from a point and normal.",
		this, &PathPlannerNavMesh::cmdCreatePlanePoly);
	SetEx("nav_slice", "Creates a plane from a point and normal.",
		this, &PathPlannerNavMesh::cmdCreateSlicePoly);
	SetEx("nav_sectorslice", "Slices working sector with the sector being aimed at.",
		this, &PathPlannerNavMesh::cmdCreateSliceSector);

	SetEx("nav_setfield", "Sets a named field on the navigation sector.",
		this, &PathPlannerNavMesh::cmdSetField);
	SetEx("nav_setfieldm", "Sets a named field on the navigation sector.",
		this, &PathPlannerNavMesh::cmdSetField);
	SetEx("nav_setfields", "Sets a named field on the navigation sector.",
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

	SetEx("nav_edgeblock", "Toggle flag on sector edge that prevents the edge from generating portals.",
		this, &PathPlannerNavMesh::cmdPortalBlockCreate);

	SetEx("nav_delsector", "Deletes a sector by its id number.",
		this, &PathPlannerNavMesh::cmdDeleteSector);

	SetEx("nav_mirrorsectors", "Mirrors all sectors across a specified axis.",
		this, &PathPlannerNavMesh::cmdMirrorSectors);

	SetEx("sector_createconnections", "Sets a property of the current sector.",
		this, &PathPlannerNavMesh::cmdSectorCreateConnections);

	SetEx("nav_setmapcenter", "Sets the map center.",
		this, &PathPlannerNavMesh::cmdSetMapCenter);

	SetEx("nav_obstacleadd", "Creates an obstacle.",
		this, &PathPlannerNavMesh::cmdObstacleAdd);

	SetEx("nav_saveobj", "Save To .obj file.",
		this, &PathPlannerNavMesh::cmdSaveToObjFile);
	
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

void PathPlannerNavMesh::cmdNavShowObstacles(const StringVector &_args)
{
	const char *strUsage[] =
	{
		"nav_showobstacles enable[bool]",
		"> enable: Render runtime obstacles. true/false/on/off/1/0",
	};

	CHECK_NUM_PARAMS(_args, 2, strUsage);
	CHECK_BOOL_PARAM(bEnable, 1, strUsage);

	m_PlannerFlags.SetFlag( NAVMESH_SHOWOBSTACLES, bEnable );
}

void PathPlannerNavMesh::cmdNavDisableObstacles(const StringVector &_args)
{
	const char *strUsage[] =
	{
		"nav_enableobstacles enable[bool]",
		"> enable: Enable runtime obstacles. true/false/on/off/1/0",
	};

	CHECK_NUM_PARAMS(_args, 2, strUsage);
	CHECK_BOOL_PARAM(bEnable, 1, strUsage);

	m_PlannerFlags.SetFlag( NAVMESH_DISABLEOBSTACLES, bEnable );

	if ( bEnable )
	{
		for ( size_t i = 0; i < mObstacles.size(); ++i )
		{
			mObstacles[ i ]->mActive = false;
		}

		for ( size_t i = 0; i < mRuntimeSectors.size(); ++i )
		{
			mRuntimeSectors[ i ].mUpdateObstacles = true;
		}
	}
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

	size_t dynTris = 0;
	for ( size_t i = 0; i < mRuntimeSectorCollision.size(); ++i )
		if ( mRuntimeSectorCollision[ i ] )
			dynTris += mRuntimeSectorCollision[ i ]->NumTriangles();

	EngineFuncs::ConsoleMessage( "-= Navigation Mesh =-" );
	EngineFuncs::ConsoleMessage( va( "%d Sectors", mNavSectors.size() ) );
	EngineFuncs::ConsoleMessage( va( "%d Total Sectors(with mirrored)", mRuntimeSectors.size() ) );
	EngineFuncs::ConsoleMessage( va( "%d Portals", numPortals ) );
	EngineFuncs::ConsoleMessage( va( "%d Tris in Static NavMesh Collision", mSectorCollision.NumTriangles() ) );
	EngineFuncs::ConsoleMessage( va( "%d Tris in Dynamic NavMesh Collision", dynTris ) );
	
	EngineFuncs::ConsoleMessage( "-= Obstacles =-" );
	EngineFuncs::ConsoleMessage( va( "%d Obstacles", mObstacles.size() ) );

	const float fmin = std::numeric_limits<float>::lowest();
	const float fmax = std::numeric_limits<float>::max();

	AABB mapbounds;
	mapbounds.Set( Vector3f( fmax, fmax, fmax ), Vector3f( fmin, fmin, fmin ) );
	for ( size_t i = 0; i < mRuntimeSectors.size(); ++i )
	{
		const AxisAlignedBox3f aabb = mRuntimeSectors[i].CalculateAABB();
		mapbounds.Expand( aabb.Min );
		mapbounds.Expand( aabb.Max );
	}

	EngineFuncs::ConsoleMessage( va( "Bounds Min( %.4f, %.4f, %.4f )", mapbounds.m_Mins[ 0 ], mapbounds.m_Mins[ 1 ], mapbounds.m_Mins[ 2 ] ) );
	EngineFuncs::ConsoleMessage( va( "Bounds Max( %.4f, %.4f, %.4f )", mapbounds.m_Maxs[ 0 ], mapbounds.m_Maxs[ 1 ], mapbounds.m_Maxs[ 2 ] ) );
}

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
		}
		if(!features[i].m_TargetBounds.IsZero())
		{
			features[i].m_TargetBounds.CenterBottom(vTarget);
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
	for ( size_t s = 0; s < mRuntimeSectors.size(); ++s )
	{
		const RuntimeNavSector & ns = mRuntimeSectors[s];

		for(obuint32 v = 0; v < ns.GetNumVerts(); ++v)
		{
			DistPoint3Segment3f dist( ns.GetVert( v ), Segment3f( _pos1,_pos2 ) );
			const float fLenSq = dist.GetSquared();
			if( fLenSq < fRangeSq && fLenSq < fClosest )
			{
				r = ns.GetVert( v );
				fClosest = fLenSq;

				if(_snapped)
					*_snapped = true;
			}
		}
	}
	return r;
}

class ToolTraceSector : public EditTool<PathPlannerNavMesh>
{
public:
	ToolTraceSector() : EditTool( "Trace Sector" ) {}

	virtual bool Enter( PathPlannerNavMesh * system )
	{
		Vector3f aimPt;
		if ( !Utils::GetLocalAimPoint( aimPt ) )
			return false;

		mPoly.clear();
		mPoly.push_back( aimPt);
		mPoly.push_back( aimPt );
		return true;
	}
	virtual bool ReEnter( PathPlannerNavMesh * system )
	{
		Vector3f aimPt;
		if ( !Utils::GetLocalAimPoint( aimPt ) )
			return false;

		// add a new vert
		mPoly.push_back( aimPt );
		return true;
	}
	virtual bool Update( PathPlannerNavMesh * system )
	{
		if ( mPoly.size() == 0 )
			return false;

		Vector3f eyePos, aimPt;
		if ( !Utils::GetLocalEyePosition( eyePos ) || !Utils::GetLocalAimPoint( aimPt ) )
			return false;

		bool bSnapped = false;
		if ( Length( aimPt, mPoly[0] ) <= SNAP_RADIUS)
		{
			bSnapped = true;
			aimPt = mPoly[0];
		}
		else
			aimPt = system->_SectorVertWithin( eyePos, aimPt, SNAP_RADIUS, &bSnapped);

		if(bSnapped)
			system->m_CursorColor = COLOR::ORANGE;

		mPoly[ mPoly.size() - 1 ] = aimPt;

		RenderBuffer::AddPolygonSilouette( mPoly, COLOR::YELLOW );
		return true;
	}
	virtual void Commit( PathPlannerNavMesh * system )
	{
		Utils::WeldVertices( mPoly, 1.0f );
		if ( mPoly.size() >= 3 )
		{
			PathPlannerNavMesh::NavSector ns;
			ns.Init( mPoly );
			system->mNavSectors.push_back( ns );
			system->InitSectors();
		}
	}
	void Undo( PathPlannerNavMesh * system )
	{
		if ( mPoly.size() > 0 )
			mPoly.pop_back();
	}
private:
	Vector3List			mPoly;
};

void PathPlannerNavMesh::cmdTracePoly(const StringVector &_args)
{
	if(!m_PlannerFlags.CheckFlag(NAV_VIEW))
		return;

	SetCurrentTool<ToolTraceSector>();
}

void PathPlannerNavMesh::cmdUndo(const StringVector &_args)
{
	if ( mCurrentTool )
		mCurrentTool->Undo( this );
}

class ToolCreateSectorPoly : public EditTool<PathPlannerNavMesh>
{
public:
	ToolCreateSectorPoly() : EditTool( "Polygon Create" ) {}

	virtual bool Enter( PathPlannerNavMesh * system )
	{
		Vector3f aimPt, aimNormal;
		if( !Utils::GetLocalAimPoint( aimPt, &aimNormal ) )
			return false;

		// don't allow inverted polygons
		if ( aimNormal.Z() < 0.0 )
			aimNormal = -aimNormal;

		system->mEditSectorStart = aimPt;
		system->mEditSectorPlane = Plane3f( aimNormal, aimPt );

		return true;
	}
	virtual bool ReEnter( PathPlannerNavMesh * system )
	{
		Vector3f aimPt;
		if( !Utils::GetLocalAimPoint( aimPt ) )
			return false;
		
		system->mEditSector.InitNew( aimPt, system->mEditSectorPlane.Normal );

		// we want it to cancel the tool here to use the tool as a toggle
		return false;
	}
	virtual bool Update( PathPlannerNavMesh * system )
	{
		Vector3f aimPt;
		if( !Utils::GetLocalAimPoint( aimPt ) )
			return false;

		system->mEditSectorPlane = Plane3f( system->mEditSectorPlane.Normal, aimPt );

		Vector3List poly = Utils::CreatePolygon(
			aimPt,
			system->mEditSectorPlane.Normal,
			32768.f);

		RenderBuffer::AddPolygonFilled( poly, COLOR::GREEN.fade(100) );

		return true;
	}
	virtual void Commit( PathPlannerNavMesh * system )
	{
		ReEnter( system );
		/*Vector3f aimPt;
		if( !Utils::GetLocalAimPoint( aimPt ) )
		return;

		system->mEditSector.mPoly = Utils::CreatePolygon(
		aimPt,
		system->mEditSectorPlane.Normal,
		32768.f);*/
	}
};

void PathPlannerNavMesh::cmdCreatePlanePoly(const StringVector &_args)
{
	if(!m_PlannerFlags.CheckFlag(NAV_VIEW))
		return;

	SetCurrentTool<ToolCreateSectorPoly>();
}

class ToolCreateSlicePoly : public EditTool<PathPlannerNavMesh>
{
public:
	ToolCreateSlicePoly() : EditTool( "Polygon Slice" ) {}

	virtual bool Enter( PathPlannerNavMesh * system )
	{
		if(system->mEditSector.GetNumVerts() == 0)
		{
			EngineFuncs::ConsoleError("No Active Sector");
			return false;
		}

		Vector3f aimPt, aimNormal;
		if( !Utils::GetLocalAimPoint( aimPt, &aimNormal ) )
			return false;

		// don't allow inverted polygons
		if ( aimNormal.Z() < 0.0 )
			aimNormal = -aimNormal;

		system->mEditSlicePlane = Plane3f( aimNormal, aimPt );

		return true;
	}
	virtual bool ReEnter( PathPlannerNavMesh * system )
	{
		const Plane3f clipPlane = ( system->mEditSlicePlane.WhichSide( system->mEditSectorStart ) > 0 ) ?
			-system->mEditSlicePlane : system->mEditSlicePlane;

		system->mEditSector.Init( Utils::ClipPolygonToPlanes( system->mEditSector.GetVerts(), clipPlane ) );

		// we want it to cancel the tool here to use the tool as a toggle
		return false;
	}
	virtual bool Update( PathPlannerNavMesh * system )
	{
		Vector3f aimPt, eyePos;
		if( !Utils::GetLocalAimPoint( aimPt ) || !Utils::GetLocalEyePosition( eyePos ) )
			return false;

		bool bSnapped = false;
		aimPt = system->_SectorVertWithin( aimPt, aimPt, SNAP_RADIUS, &bSnapped );
		if(bSnapped)
			system->m_CursorColor = COLOR::ORANGE;

		system->mEditSlicePlane = Plane3f( system->mEditSlicePlane.Normal, aimPt );

		Vector3List poly = Utils::CreatePolygon(
			aimPt,
			system->mEditSlicePlane.Normal,
			32768.f);

		RenderBuffer::AddPolygonFilled( poly, COLOR::RED.fade(100) );
		return true;
	}
	virtual void Commit( PathPlannerNavMesh * system )
	{
		ReEnter( system );
		/*const Plane3f clipPlane = ( system->mEditSlicePlane.WhichSide( system->mEditSectorStart ) > 0 ) ?
		-system->mEditSlicePlane : system->mEditSlicePlane;

		system->mEditSector.mPoly =
		Utils::ClipPolygonToPlanes( system->mEditSector.mPoly, clipPlane );*/
	}
};

class ToolMoveSector : public EditTool<PathPlannerNavMesh>
{
public:
	ToolMoveSector() : EditTool( "Move Sector" ) {}

	virtual bool Enter( PathPlannerNavMesh * system )
	{
		Vector3f eyePos, eyeDir;
		if ( !Utils::GetLocalEyePosition( eyePos ) || !Utils::GetLocalFacing( eyeDir ) )
			return false;

		PathPlannerNavMesh::NavCollision nc = system->FindCollision( eyePos, eyePos + eyeDir * 1024.f);
		if ( !nc.DidHit() || !nc.HitSector()->mMirrored )
		{
			EngineFuncs::ConsoleError("No Nav Sector Found");
			return false;
		}

		PathPlannerNavMesh::RuntimeNavSector * ns = nc.HitSector();
		system->mEditSectorStart = nc.HitPosition();
		system->mEditSector = *ns->mSector;

		system->mNavSectors.erase(system->mNavSectors.begin() + ( ns->mSector - &system->mNavSectors[ 0 ] ));

		system->InitSectors();

		return true;
	}
	virtual bool Update( PathPlannerNavMesh * system )
	{
		Vector3f aimPt;
		if( !Utils::GetLocalAimPoint( aimPt ) )
			return false;

		system->mEditSector.InitNew( aimPt, system->mEditSector.GetPlane().Normal );
		system->mEditSector.SetPointsToPlane();

		return true;
	}
	virtual void Commit( PathPlannerNavMesh * system )
	{
		system->InitSectors();
	}
};

void PathPlannerNavMesh::cmdCreateSlicePoly(const StringVector &_args)
{
	if(!m_PlannerFlags.CheckFlag(NAV_VIEW))
		return;

	SetCurrentTool<ToolCreateSlicePoly>();
}

void PathPlannerNavMesh::cmdCreateSliceSector(const StringVector &_args)
{
	/*if(!m_PlannerFlags.CheckFlag(NAV_VIEW))
	return;

	if(GetCurrentStateId()!=SliceSectorWithSector)
	{
	SetNextState(SliceSectorWithSector);
	}
	else
	{
	m_ToolCancelled = false;
	SetNextState(NoOp);
	}*/
}

void PathPlannerNavMesh::cmdEditSector(const StringVector &_args)
{
	if(!m_PlannerFlags.CheckFlag(NAV_VIEW))
		return;

	Vector3f eyePos, eyeDir;
	if ( !Utils::GetLocalEyePosition( eyePos ) || !Utils::GetLocalFacing( eyeDir ) )
		return;

	NavCollision nc = FindCollision( eyePos, eyePos + eyeDir * 1024.f );
	if ( !nc.DidHit() || nc.HitSector()->mMirrored )
	{
		EngineFuncs::ConsoleError("No Nav Sector Found");
		return;
	}

	RuntimeNavSector * ns = nc.HitSector();

	mEditSectorStart = nc.HitPosition();
	mEditSector = *ns->mSector;
	mNavSectors.erase(mNavSectors.begin() + ( ns->mSector - &mNavSectors[ 0 ] ));

	InitSectors();
}

void PathPlannerNavMesh::cmdDupeSector(const StringVector &_args)
{
	if(!m_PlannerFlags.CheckFlag(NAV_VIEW))
		return;

	SetCurrentTool<ToolMoveSector>();
}

class ToolSplitSector : public EditTool<PathPlannerNavMesh>
{
public:
	ToolSplitSector() : EditTool( "Split Sector" ) {}

	virtual bool Enter( PathPlannerNavMesh * system )
	{
		if ( system->mEditSector.GetNumVerts() == 0 )
		{
			EngineFuncs::ConsoleError("No Active Sector");
			return false;
		}

		Vector3f aimPos, aimNormal;
		if ( !Utils::GetLocalAimPoint( aimPos, &aimNormal ) )
			return false;

		system->mEditSlicePlane = Plane3f( aimNormal, aimPos );

		return true;
	}
	virtual bool Update( PathPlannerNavMesh * system )
	{
		Vector3f aimPt, eyePos;
		if( !Utils::GetLocalAimPoint( aimPt ) || !Utils::GetLocalEyePosition( eyePos ) )
			return false;

		system->mEditSlicePlane = Plane3f( system->mEditSlicePlane.Normal, aimPt );

		Vector3List slicePoly = Utils::CreatePolygon(
			aimPt,
			system->mEditSlicePlane.Normal,
			1024.f);

		RenderBuffer::AddPolygonFilled( slicePoly, COLOR::RED.fade(100) );

		return true;
	}
	virtual void Commit( PathPlannerNavMesh * system )
	{
		Vector3List s1 = Utils::ClipPolygonToPlanes( system->mEditSector.GetVerts(), system->mEditSlicePlane );
		Vector3List s2 = Utils::ClipPolygonToPlanes( system->mEditSector.GetVerts(), -system->mEditSlicePlane );

		Utils::WeldVertices( s1, 1.0f );
		Utils::WeldVertices( s2, 1.0f );

		// add the 2 new sectors
		if( s1.size() >= 3 )
		{
			PathPlannerNavMesh::NavSector sec = system->mEditSector;
			sec.Init( s1 );
			system->mNavSectors.push_back( sec );
		}
		if( s2.size() >= 3 )
		{
			PathPlannerNavMesh::NavSector sec = system->mEditSector;
			sec.Init( s2 );
			system->mNavSectors.push_back( sec );
		}

		system->mEditSector = PathPlannerNavMesh::NavSector();

		system->InitSectors();
	}
};

void PathPlannerNavMesh::cmdSplitSector(const StringVector &_args)
{
	if(!m_PlannerFlags.CheckFlag(NAV_VIEW))
		return;

	SetCurrentTool<ToolSplitSector>();
}

void PathPlannerNavMesh::cmdGroundSector(const StringVector &_args)
{
	if(!m_PlannerFlags.CheckFlag(NAV_VIEW))
		return;

	Vector3f eyePos, eyeDir;

	if(!Utils::GetLocalEyePosition( eyePos ) || !Utils::GetLocalFacing( eyeDir ))
		return;

	NavCollision nc = FindCollision( eyePos, eyePos + eyeDir * 1024.f );
	if ( !nc.DidHit() || nc.HitSector()->mMirrored )
	{
		EngineFuncs::ConsoleError("No Nav Sector Found");
		return;
	}

	RuntimeNavSector * ns = nc.HitSector();

	obTraceResult tr;
	for ( obuint32 i = 0; i < ns->GetNumVerts(); ++i )
	{
		Vector3f vPt = ns->mSector->GetVert( i );
		EngineFuncs::TraceLine(tr,
			vPt+Vector3f(0.f,0.f,1.f),
			vPt+Vector3f(0.f,0.f,-1024.f),
			NULL,
			TR_MASK_FLOODFILL,
			-1,
			False);

		ns->mSector->SetVertZ( i, tr.m_Endpos[2] );
	}

	InitSectors();
}

void PathPlannerNavMesh::cmdCommitPoly(const StringVector &_args)
{
	if(!m_PlannerFlags.CheckFlag(NAV_VIEW))
		return;

	if ( mCurrentTool )
	{
		mCurrentTool->Commit( this );

		// should we always delete the tool?
		OB_DELETE( mCurrentTool );
	}
	else
	{
		mEditSector.Optimize();

		if ( mEditSector.GetNumVerts() > 2 )
		{
			mNavSectors.push_back( mEditSector );
			InitSectors();
		}
		mEditSector = NavSector();
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
	if ( !nc.DidHit() || nc.HitSector()->mMirrored )
	{
		EngineFuncs::ConsoleError("can't find sector, aim at a sector and try again.");
		return;
	}

	RuntimeNavSector * ns = nc.HitSector();

	mNavSectors.erase( mNavSectors.begin() + ( ns->mSector - &mNavSectors[ 0 ] ) );

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
		if ( !nc.DidHit() || nc.HitSector()->mMirrored )
		{
			EngineFuncs::ConsoleError("no sector at cursor");
			return;
		}
		RuntimeNavSector * ns = nc.HitSector();
		if ( ns == NULL )
		{
			EngineFuncs::ConsoleError("can't find sector, aim at a sector and try again.");
			return;
		}
		ns->mSector->mMirror = mir;
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
		ns->RebuildPortals( this );
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

void PathPlannerNavMesh::cmdObstacleAdd(const StringVector &_args)
{
	if(_args.size() < 3)
	{
		EngineFuncs::ConsoleError("nav_obstacleadd length width cost");
		return;
	}

	const float len = (float)atof( _args[ 1 ].c_str() );
	const float wid = (float)atof( _args[ 2 ].c_str() );
	const float cost = (float)atof( _args[ 3 ].c_str() );

	if ( len == 0.0 || wid == 0.0 )
		return;

	Vector3f vObsPoint, vObsNormal;
	Utils::GetLocalAimPoint( vObsPoint, &vObsNormal );
	
	// build a random polygon
	Plane3f p = Plane3f( Vector3f::UNIT_Z, vObsPoint );

	const int numPoints = Mathf::IntervalRandomInt( 3, 10 );

	Quaternionf q;
	q.FromAxisAngle( Vector3f::UNIT_Z, Mathf::DegToRad( 360.0f / (float)numPoints ) );

	Vector3f v = Vector3f::UNIT_X;

	Obstacle * obs = new Obstacle;
	obs->mPoly.push_back( vObsPoint + v * Mathf::IntervalRandom( 32.0f, 64.0f ) );
	for ( int i = 0; i < numPoints - 1; ++i )
	{
		v = q.Rotate(v);
		obs->mPoly.push_back( vObsPoint + v * Mathf::IntervalRandom( 32.0f, 64.0f ) );
	}
	
	obs->mPosition = vObsPoint;
	obs->mCost = cost;
	obs->mExpireTime = IGame::GetTime() + 5000;
	mPendingObstacles.push_back( obs );
}

void PathPlannerNavMesh::_BenchmarkPathFinder(const StringVector &_args)
{
	//EngineFuncs::ConsoleMessage("-= NavMesh PathFind Benchmark =-");

	//// cache all sector center positions so we don't include that as part of the benchmark
	//Vector3List sectorCenters;
	//sectorCenters.reserve( mRuntimeSectors.size() );
	//for( size_t i = 0; i < mRuntimeSectors.size(); ++i)
	//{
	//	sectorCenters.push_back( mRuntimeSectors[ i ].CalculateCenter() );
	//}

	//Timer tme;
	//tme.Reset();
	//for ( size_t s1 = 0; s1 < sectorCenters.size(); ++s1 )
	//{
	//	for ( size_t s2 = 0; s2 < sectorCenters.size(); ++s2 )
	//	{
	//		PlanPathToGoal(NULL,sectorCenters[ s1 ],sectorCenters[ s2 ],0);
	//	}
	//}
	//const double dTimeTaken = tme.GetElapsedSeconds();

	//EngineFuncs::ConsoleMessage(va("generated %d paths in %f seconds: %f paths/sec",
	//	(sectorCenters.size()*sectorCenters.size()),
	//	dTimeTaken,
	//	dTimeTaken != 0.0f ? (float)(sectorCenters.size()*sectorCenters.size()) / dTimeTaken : 0.0f));
}

void PathPlannerNavMesh::_BenchmarkGetNavPoint(const StringVector &_args)
{
	//obuint32 iNumIterations = 1;
	//if(_args.size() > 1)
	//{
	//	iNumIterations = atoi(_args[1].c_str());
	//	if(iNumIterations <= 0)
	//		iNumIterations = 1;
	//}

	//EngineFuncs::ConsoleMessage("-= NavMesh GetNavPoint Benchmark  =-");

	//// cache all sector center positions so we don't include that as part of the benchmark
	//Vector3List sectorCenters;
	//sectorCenters.reserve( mRuntimeSectors.size() );
	//for(obuint32 i = 0; i < mRuntimeSectors.size(); ++i)
	//{
	//	sectorCenters.push_back( mRuntimeSectors[ i ].CalculateCenter() );
	//}

	//Timer tme;

	//obuint32 iHits = 0, iMisses = 0;
	//tme.Reset();
	//for(obuint32 i = 0; i < iNumIterations; ++i)
	//{
	//	for(obuint32 s = 0; s < sectorCenters.size(); ++s)
	//	{
	//		RuntimeNavSector * sector = GetSectorAt( sectorCenters[ s ] );
	//		if ( sector )
	//			++iHits;
	//		else
	//			++iMisses;
	//	}
	//}

	//const double dTimeTaken = tme.GetElapsedSeconds();

	//EngineFuncs::ConsoleMessage( va( "BenchmarkGetNavPoint() %d calls, %d hits, %d misses : avg %f per second",
	//	mRuntimeSectors.size() * iNumIterations,
	//	iHits,
	//	iMisses,
	//	dTimeTaken != 0.0f ? ((float)(mRuntimeSectors.size() * iNumIterations) / dTimeTaken) : 0.0f ) );
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
public:
	ToolSectorEdgeDrop() : EditTool( "Sector Edge Drop" ) {}

	virtual bool Enter( PathPlannerNavMesh * system )
	{
		Vector3f aimPt;
		if( !Utils::GetLocalAimPoint( aimPt ) )
			return false;

		mSector = system->GetSectorAtCursor();
		if ( mSector == NULL )
			return false;

		m0 = m1 = 0;

		float nearest = std::numeric_limits<float>::max();
		for ( size_t i = 1; i <= mSector->GetNumVerts(); ++i )
		{
			const int s0 = i-1;
			const int s1 = i%mSector->GetNumVerts();
			DistPoint3Segment3f dist( aimPt, Segment3f( mSector->GetVert( s0 ), mSector->GetVert( s1 ) ) );

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
	virtual bool Update( PathPlannerNavMesh * system )
	{
		Vector3f aimPt;
		if( !Utils::GetLocalAimPoint( aimPt ) )
			return false;

		mSector->SetVertZ( m0, aimPt.Z() );
		mSector->SetVertZ( m1, aimPt.Z() );

		// update the runtime sectors too
		for ( size_t i = 0; i < system->mRuntimeSectors.size(); ++i )
		{
			if ( system->mRuntimeSectors[ i ].mSector == mSector )
			{
				system->mRuntimeSectors[ i ].SetVertZ( m0, aimPt.Z() );
				system->mRuntimeSectors[ i ].SetVertZ( m1, aimPt.Z() );
			}
		}
		return true;
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
	SetCurrentTool<ToolSectorEdgeDrop>();
}

class ToolPortalCreate : public EditTool<PathPlannerNavMesh>
{
public:
	ToolPortalCreate() : EditTool( "Create Portal" ) {}

	virtual bool Enter( PathPlannerNavMesh * system )
	{
		Vector3f aimPt;
		if( !Utils::GetLocalAimPoint( aimPt ) )
			return false;

		return true;
	}
	virtual bool Update( PathPlannerNavMesh * system )
	{
		return true;
	}
	virtual void Commit( PathPlannerNavMesh * system )
	{
	}
private:
};

void PathPlannerNavMesh::cmdPortalCreate(const StringVector &_args)
{
	SetCurrentTool<ToolPortalCreate>();
}

class ToolPortalEdgeMask : public EditTool<PathPlannerNavMesh>
{
public:
	ToolPortalEdgeMask() : EditTool( "PortalEdgeMask" ) {}

	virtual bool Enter( PathPlannerNavMesh * system )
	{
		Vector3f eyePos, eyeDir;
		if( !Utils::GetLocalEyePosition( eyePos ) )
			return false;
		if( !Utils::GetLocalFacing( eyeDir ) )
			return false;

		PathPlannerNavMesh::NavCollision nc = system->FindCollision( eyePos, eyePos + eyeDir * 2048.f );
		if( !nc.DidHit() )
			return false;

		mSector = nc.HitSector();

		if ( mSector == NULL )
			return false;

		return true;
	}
	virtual bool Update( PathPlannerNavMesh * system )
	{
		Vector3f eyePos, eyeDir;
		if( !Utils::GetLocalEyePosition( eyePos ) )
			return false;
		if( !Utils::GetLocalFacing( eyeDir ) )
			return false;

		for ( size_t i = 0; i < mSector->GetNumVerts(); ++i )
		{
			const size_t i0 = i;
			const size_t i1 = (i+1) % mSector->GetNumVerts();
			IntrRay2Segment2f intr(
				Ray2f( To2d( eyePos ), To2d( eyeDir ) ),
				Segment2f( Vector2f( To2d( mSector->GetVert( i0 ) ) ), Vector2f( To2d( mSector->GetVert( i1 ) ) ) ) );

			if ( intr.Test() )
			{
				const bool edgedisabled = ( mSector->mSectorData->edgeportalmask() & (1<<i) ) != 0;

				RenderBuffer::AddTri(
					mSector->GetVert( i0 ) + Vector3f(0.0f,0.0f,4.0f),
					mSector->GetVert( i1 ) + Vector3f(0.0f,0.0f,4.0f),
					mSector->CalculateCenter(),
					edgedisabled ? COLOR::GREEN.fade(100) : COLOR::RED.fade(100) );

				mMask = ( 1<<i );
				break;
			}
		}
		return true;
	}
	virtual void Commit( PathPlannerNavMesh * system )
	{
		const bool edgedisabled = ( mSector->mSectorData->edgeportalmask() & mMask ) != 0;
		if ( edgedisabled )
			mSector->mSectorData->set_edgeportalmask( mSector->mSectorData->edgeportalmask() & ~mMask );
		else
			mSector->mSectorData->set_edgeportalmask( mSector->mSectorData->edgeportalmask() | mMask );

		ClearDefaultedValues( *mSector->mSectorData );

		mSector->mUpdatePortals = true;
	}
private:
	PathPlannerNavMesh::RuntimeNavSector *		mSector;
	obuint32									mMask;
};

void PathPlannerNavMesh::cmdPortalBlockCreate(const StringVector &_args)
{
	SetCurrentTool<ToolPortalEdgeMask>();
}

static void PrintSetFieldUsage( const google::protobuf::Message & msg, const std::string & varpath = "" )
{
	using namespace google;

	for ( int i = 0; i < msg.GetDescriptor()->field_count(); ++i )
	{
		const protobuf::FieldDescriptor * fieldDesc = msg.GetDescriptor()->field( i );

		const bool hidden = fieldDesc->options().GetExtension( NavmeshIO::hidden );
		if ( hidden )
			continue;

		std::string fullvarname = varpath;
		if ( !fullvarname.empty() )
			fullvarname += ".";
		fullvarname += fieldDesc->lowercase_name();

		if ( fieldDesc->type() == protobuf::FieldDescriptorProto_Type_TYPE_MESSAGE )
		{
			PrintSetFieldUsage( msg.GetReflection()->GetMessage( msg, fieldDesc ), fullvarname );
			continue;
		}
		else if ( fieldDesc->type() == protobuf::FieldDescriptorProto_Type_TYPE_GROUP )
		{
			continue;
		}
		else if ( fieldDesc->type() == protobuf::FieldDescriptorProto_Type_TYPE_BYTES )
		{
			continue;
		}

		const std::string doc = fieldDesc->options().GetExtension( NavmeshIO::doc );
		EngineFuncs::ConsoleError( va( "%s ( %s ) = %s - %s",
			fullvarname.c_str(),
			fieldDesc->type_name(),
			GetFieldString( msg, fieldDesc ).c_str(),
			doc.c_str() ) );
	}
}

void PathPlannerNavMesh::cmdSetField(const StringVector &_args)
{
	using namespace google;
	
	const bool both = _args.at( 0 ).back() == 's';
	if ( both )
	{
		// easiest to just execute the command multiple times
		// than to adapt the function to be able to do both
		StringVector cmd = _args;
		cmd[ 0 ] = "nav_setfield";
		cmdSetField( cmd );
		cmd[ 0 ] = "nav_setfieldm";
		cmdSetField( cmd );
		return;
	}

	const bool mirrored = _args.at( 0 ).back() == 'm';
	
	NavSector * ns = GetSectorAtCursor();
	if ( ns == NULL )
	{
		EngineFuncs::ConsoleError( "No Sector found. Aim at a sector to set field values." );
		return;
	}

	NavmeshIO::SectorData * msgSectorData = mirrored ? &ns->mSectorDataMirrored : &ns->mSectorData;
	protobuf::Message * msg = msgSectorData;

	if ( _args.size() <= 2 )
	{
		EngineFuncs::ConsoleError( va( "Usage: %s fieldname fieldvalue", _args.at( 0 ).c_str() ) );
		PrintSetFieldUsage( *msg );
		return;
	}

	const bool wasOnMover = msgSectorData->onmover();

	const std::string fieldName = Utils::StringToLower( _args.at( 1 ).c_str() );

	const protobuf::Descriptor * msgDesc = msg->GetDescriptor();
	const protobuf::Reflection * refl = msg->GetReflection();
	const protobuf::FieldDescriptor * fieldDesc = NULL;

	// handle setting nested field values
	StringVector tokens;
	Utils::Tokenize( fieldName, ".", tokens );
	for ( size_t t = 0; t < tokens.size() && msgDesc != NULL; ++t )
	{
		fieldDesc = msgDesc->FindFieldByLowercaseName( tokens[ t ] );
		if ( fieldDesc == NULL )
		{
			EngineFuncs::ConsoleError( va( "Unknown field: '%s'", tokens[ t ].c_str() ) );
			EngineFuncs::ConsoleError( "    If this should be added as a custom field, use nav_addfield instead" );
			return;
		}

		if ( fieldDesc->type() == protobuf::FieldDescriptorProto_Type_TYPE_MESSAGE )
		{
			msg = refl->MutableMessage( msg, fieldDesc );
			msgDesc = msg ? msg->GetDescriptor() : NULL;
			refl = msg ? msg->GetReflection() : NULL;

			if ( msg == NULL )
			{
				EngineFuncs::ConsoleError( va( "Unknown field: '%s' in sub message '%s'",
					tokens[ t ].c_str(),
					fieldDesc->lowercase_name() ) );
				return;
			}
		}
	}

	const std::string fieldValue = _args.at( 2 ).c_str();

	const bool hidden = fieldDesc ? fieldDesc->options().GetExtension( NavmeshIO::hidden ) : false;
	const bool settable = fieldDesc ? fieldDesc->options().GetExtension( NavmeshIO::settable ) : false;
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

	try
	{
		switch( fieldDesc->type() )
		{
		case protobuf::FieldDescriptorProto_Type_TYPE_DOUBLE:
			{
				const double val = boost::lexical_cast<double>( _args.at( 2 ).c_str() );
				refl->SetDouble( msg, fieldDesc, val );
				break;
			}
		case protobuf::FieldDescriptorProto_Type_TYPE_FLOAT:
			{
				const float val = boost::lexical_cast<float>( _args.at( 2 ).c_str() );
				refl->SetFloat( msg, fieldDesc, val );
				break;
			}
		case protobuf::FieldDescriptorProto_Type_TYPE_INT64:
		case protobuf::FieldDescriptorProto_Type_TYPE_SFIXED64:
		case protobuf::FieldDescriptorProto_Type_TYPE_SINT64:
			{
				const protobuf::int64 val = boost::lexical_cast<protobuf::int64>( _args.at( 2 ).c_str() );
				refl->SetInt64( msg, fieldDesc, val );
				break;
			}
		case protobuf::FieldDescriptorProto_Type_TYPE_UINT64:
		case protobuf::FieldDescriptorProto_Type_TYPE_FIXED64:
			{
				const protobuf::uint64 val = boost::lexical_cast<protobuf::uint64>( _args.at( 2 ).c_str() );
				refl->SetUInt64( msg, fieldDesc, val );
				break;
			}
		case protobuf::FieldDescriptorProto_Type_TYPE_INT32:
		case protobuf::FieldDescriptorProto_Type_TYPE_SFIXED32:
		case protobuf::FieldDescriptorProto_Type_TYPE_SINT32:
			{
				const protobuf::int32 val = boost::lexical_cast<protobuf::int32>( _args.at( 2 ).c_str() );
				refl->SetInt32( msg, fieldDesc, val );
				break;
			}
		case protobuf::FieldDescriptorProto_Type_TYPE_BOOL:
			{
				const bool val = boost::lexical_cast<bool>( _args.at( 2 ).c_str() );
				refl->SetBool( msg, fieldDesc, val );
				break;
			}
		case protobuf::FieldDescriptorProto_Type_TYPE_UINT32:
		case protobuf::FieldDescriptorProto_Type_TYPE_FIXED32:
			{
				const protobuf::uint32 val = boost::lexical_cast<protobuf::uint32>( _args.at( 2 ).c_str() );
				refl->SetUInt32( msg, fieldDesc, val );
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
				
				refl->SetString( msg, fieldDesc, str );
				break;
			}
		case protobuf::FieldDescriptorProto_Type_TYPE_GROUP:
			EngineFuncs::ConsoleError( va( "Can't set message field type 'Group'" ) );
			return;
		case protobuf::FieldDescriptorProto_Type_TYPE_MESSAGE:
			EngineFuncs::ConsoleError( va( "Can't set message field type 'Message', use dot(.) syntax to set child fields directly." ) );
			return;
		case protobuf::FieldDescriptorProto_Type_TYPE_BYTES:
			EngineFuncs::ConsoleError( va( "Can't set message field type 'bytes'" ) );
			return;
		case protobuf::FieldDescriptorProto_Type_TYPE_ENUM:
			{
				const protobuf::EnumValueDescriptor * enumVal = fieldDesc->enum_type()->FindValueByName( _args.at( 2 ).c_str() );
				if ( enumVal != NULL )
				{
					refl->SetEnum( msg, fieldDesc, enumVal );
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

		ClearDefaultedValues( *msg );

		bool clearMoverState = !msgSectorData->onmover();
		if ( !wasOnMover && msgSectorData->onmover() )
		{
			for ( size_t i = 0; i < mRuntimeSectors.size(); ++i )
			{
				if ( mRuntimeSectors[ i ].mSectorData == msgSectorData )
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
			msgSectorData->clear_onmover();
			msgSectorData->clear_mover();
			msgSectorData->clear_localoffsets();
		}
	}
	catch ( const boost::bad_lexical_cast & ex )
	{
		EngineFuncs::ConsoleError(
			va( "Can't convert '%s' to appropriate type %s ( %s )",
			_args.at( 2 ).c_str(),
			fieldDesc->type_name(),
			ex.what() ) );
	}
}

//////////////////////////////////////////////////////////////////////////

void PathPlannerNavMesh::cmdSaveToObjFile(const StringVector &_args) 
{
	if(_args.size() < 2)
	{
		EngineFuncs::ConsoleError("nav_saveobj filename");
		return;
	}

	RenderBuffer::TriList triangles;
	CollisionModel::MaterialIndices attribs;
	mSectorCollision.GetTriangles( ModelTransform::sIdentity, triangles, attribs );

	const std::string objfilename = _args[ 1 ] + ".obj";
	const std::string matfilename = _args[ 1 ] + ".mat";

	File objFile, matFile;
	if ( objFile.OpenForWrite( objfilename.c_str(), File::Text, false ) &&
		matFile.OpenForWrite( matfilename.c_str(), File::Text, false ))
	{
		// material
		matFile.WriteString( va( "# Materials Navmesh - %s\n\n", g_EngineFuncs->GetMapName() ).c_str() );
		matFile.WriteString( "Kd 0.000 1.000 0.000     # green" );

		// Mesh
		objFile.WriteString( va( "# Navmesh - %s\n\n", g_EngineFuncs->GetMapName() ).c_str() );		
		objFile.WriteString( va( "mltlib %s\n", matfilename.c_str() ).c_str() );
		objFile.WriteString( va( "# Vertices %d\n", triangles.size() * 3 ).c_str() );		
		for ( size_t t = 0; t < triangles.size(); ++t )
		{
			for ( size_t i = 0; i < 3; ++i )
			{
				objFile.WriteString( va( "v %f %f %f\n", 
					triangles[ t ].v[i].X(), 
					triangles[ t ].v[i].Y(), 
					triangles[ t ].v[i].Z() ).c_str() );
			}
		}

		objFile.WriteString( va( "\n# Faces %d\n", triangles.size() ).c_str() );

		for ( size_t t = 0; t < triangles.size(); ++t )
		{
			objFile.WriteString( va( "f %d %d %d\n", t*3+1,t*3+2,t*3+3 ).c_str() );
		}
	}
}
