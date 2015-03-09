////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "PathPlannerNavMesh.h"

#include <algorithm>

#include "InterfaceFuncs.h"
#include "IGameManager.h"
#include "IGame.h"
#include "GoalManager.h"
#include "FileSystem.h"
#include "Path.h"
#include "ProtoBufUtility.h"
#include "Client.h"

#include "RenderBuffer.h"
#include "PathPlannerNavMeshPathInterface.h"

#include "clipper.hpp"
#include "polypartition.h"

// Protocol buffers
#pragma warning( push )
#pragma warning(disable:4244)
#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/text_format.h"
#include "google/protobuf/io/zero_copy_stream_impl.h"
#pragma warning( pop )

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
		Utils::PushPointToPlane( vec3d, ns.GetPlane(), -Vector3f::UNIT_Z );
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
		Utils::PushPointToPlane( vec3d, ns.GetPlane(), -Vector3f::UNIT_Z );
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
			subSector.SetPlane( parent.GetPlane() );
			subSector.mParent = &parent;
			ConvertPoly( subSector.GetVertsMutable(), subSector, ply );
			sectorsOut.push_back( subSector );
		}
	}
	else
	{
		EngineFuncs::ConsoleError( va( "Sector %d error building subsectors", parent.mIndex ) );
	}
}

//////////////////////////////////////////////////////////////////////////
namespace NavigationMeshOptions
{
	float CharacterHeight = 64.f;
};
//////////////////////////////////////////////////////////////////////////

const float PathPlannerNavMesh::NavPortal::MAX_COST = 10.0f;

PathPlannerNavMesh::NavPortal::NavPortal()
{
	mSrcSector = NULL;
	mDstSector = NULL;
	mSegment = Segment3f( Vector3f::ZERO, Vector3f::ZERO, 0.0f );
	mSegmentLocal = Segment3f( Vector3f::ZERO, Vector3f::ZERO, 0.0f );
	mSegmentFwd = Vector3f::ZERO;
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

bool PathPlannerNavMesh::Obstacle::IsActive() const
{
	return mActive;
}

PathPlannerNavMesh::Obstacle::Obstacle() 
	: mCost( -1.0f )
	, mExpireTime( -1 )
{
}

PathPlannerNavMesh::Obstacle::~Obstacle()
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
	mArea2d = sector->GetArea2d();
}

PathPlannerNavMesh::RuntimeNavSector::~RuntimeNavSector()
{
}

bool PathPlannerNavMesh::RuntimeNavSector::IsMirroredSector() const
{
	return mSectorData == &mSector->mSectorDataMirrored;
}

bool PathPlannerNavMesh::RuntimeNavSector::IsUsableBy( const Client * client ) const
{
	//if ( mSectorData->has_team() )
	//{
	//	// team restricted, make sure we match
	//	if ( (client->GetTeamFlag() & F_NAV_TEAM1) && mSectorData->team().team1() )
	//		return true;
	//	if ( (client->GetTeamFlag() & F_NAV_TEAM2) && mSectorData->team().team2() )
	//		return true;
	//	if ( (client->GetTeamFlag() & F_NAV_TEAM3) && mSectorData->team().team3() )
	//		return true;
	//	if ( (client->GetTeamFlag() & F_NAV_TEAM4) && mSectorData->team().team4() )
	//		return true;
	//	return false;
	//}
	return true;
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
				mPortals[ i ].mSegmentFwd = mPortals[ i ].mSegment.Direction.UnitCross( Vector3f::UNIT_Z );
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

	// sectors should not connect to other edges, only down to drops, which is calculated in RebuildDropPortals
	if ( !mSectorData->ledge() )
	{
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
						portal.mSegmentFwd = portal.mSegment.Direction.Cross( Vector3f::UNIT_Z );
						portal.mSrcSector = this;
						portal.mDstSector = neighbor;
						
						portal.mSegmentLocal = portal.mSegment;
						portal.mSegmentLocal.P0 = (portal.mSegment.P0 - mSectorWorldTrans) * mSectorWorldRot.Transpose();
						portal.mSegmentLocal.P1 = (portal.mSegment.P1 - mSectorWorldTrans) * mSectorWorldRot.Transpose();
						portal.mSegmentLocal.ComputeCenterDirectionExtent();
						
						// do we need to jump to reach this edge?
						if ( p0zDiff > STEP_HEIGHT && p0zDiff <= JUMP_HEIGHT )
						{
							portal.mFlags.SetFlag( NavPortal::FL_JUMP );
						}

						// mark water edges
						if ( portal.mSrcSector->mSectorData->inwater() )
						{
							portal.mFlags.SetFlag( NavPortal::FL_WATER_SWIM, true );

							if ( !portal.mDstSector->mSectorData->inwater() )
								portal.mFlags.SetFlag( NavPortal::FL_WATER_EXIT, true );
						}

						mPortals.push_back( portal );
					}
				}
			}
		}
	}

	if ( mSectorData->ledge() || mSectorData->inwater() )
	{
		RebuildDropPortals( navmesh );
	}
}

//////////////////////////////////////////////////////////////////////////

void PathPlannerNavMesh::RuntimeNavSector::RebuildDropPortals( PathPlannerNavMesh * navmesh )
{
	for ( SectorSet::iterator it = mDropConnections.begin();
		it != mDropConnections.end();
		++it )
	{
		RuntimeNavSector * otherSector = *it;

		// see if the polygons actually overlap
		ClipperLib::Polygons subjectPolys;
		subjectPolys.push_back( ClipperLib::Polygon() );
		ConvertPoly( subjectPolys.back(), mPoly );

		SectorSet dropSectors;

		if ( otherSector->mSubSectors.empty() )
			dropSectors.insert( otherSector );
		else
		{
			for ( size_t i = 0; i < otherSector->mSubSectors.size(); ++i )
			{
				if ( otherSector->mSubSectors[ i ].mCost != NavPortal::MAX_COST )
					dropSectors.insert( &otherSector->mSubSectors[ i ] );
			}
		}

		ClipperLib::Polygon dropPoly;
		for ( SectorSet::iterator it = dropSectors.begin(); it != dropSectors.end(); ++it )
		{
			dropPoly.resize( 0 );

			ConvertPoly( dropPoly, (*it)->mPoly );

			ClipperLib::Clipper clip;
			clip.AddPolygons( subjectPolys, ClipperLib::ptSubject );
			clip.AddPolygon( dropPoly, ClipperLib::ptClip );

			PathPlannerNavMesh::RuntimeSectorList dropSectors;
			if ( clip.Execute( ClipperLib::ctIntersection, ClipperLib::pftNonZero, ClipperLib::pftNonZero ) )
			{
				GenerateFromClipResults( clip, *(*it), 1.0f, dropSectors );

				for ( size_t i = 0; i < dropSectors.size(); ++i )
				{
					const Vector3f dropPos = dropSectors[ i ].CalculateCenter();

					NavPortal portal;
					portal.mSegment.P0 = dropPos;
					portal.mSegment.P1 = dropPos;
					Utils::PushPointToPlane( portal.mSegment.P0, mPlane, -Vector3f::UNIT_Z );

					if ( mSectorData->inwater() )
						portal.mSegment.P0 += Vector3f( 0.0f, 0.0f, 64.0f );

					portal.mSegment.ComputeCenterDirectionExtent();
					portal.mSegmentFwd = portal.mSegment.Direction.Cross( Vector3f::UNIT_Z );
					portal.mSrcSector = this;
					portal.mDstSector = (*it);

					portal.mSegmentLocal = portal.mSegment;
					portal.mSegmentLocal.P0 = (portal.mSegment.P0 - mSectorWorldTrans) * mSectorWorldRot.Transpose();
					portal.mSegmentLocal.P1 = (portal.mSegment.P1 - mSectorWorldTrans) * mSectorWorldRot.Transpose();
					portal.mSegmentLocal.ComputeCenterDirectionExtent();
					
					obTraceResult tr;
					EngineFuncs::TraceLine(tr,
						portal.mSegment.P0 + Vector3f( 0.f, 0.f, 4.f ),
						portal.mSegment.P1 + Vector3f( 0.f, 0.f, 4.f ),
						NULL,TR_MASK_FLOODFILL,-1,False);

					if ( tr.m_Fraction < 1.0f )
						continue;

					// mark water edges
					if ( portal.mSrcSector->mSectorData->inwater() )
					{
						portal.mFlags.SetFlag( NavPortal::FL_WATER_SWIM, true );

						if ( !portal.mDstSector->mSectorData->inwater() )
							portal.mFlags.SetFlag( NavPortal::FL_WATER_EXIT, true );
					}

					// do we need to jump to reach this edge?
					/*if ( p0zDiff > STEP_HEIGHT && p0zDiff <= JUMP_HEIGHT )
					{
						portal.mFlags.SetFlag( NavPortal::FL_JUMP );
					}*/
					
					mPortals.push_back( portal );
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////

static bool CostLessThan( const PathPlannerNavMesh::Obstacle * obs1, const PathPlannerNavMesh::Obstacle * obs2 )
{
	return obs1->mCost < obs2->mCost;
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

		ClipperLib::Polygons	obstaclePolys;
		ObstacleList			activeObstacles;
		
		obstaclePolys.reserve( mObstacles.size() );
		activeObstacles.reserve( mObstacles.size() );

		for ( size_t i = 0; i < mObstacles.size(); ++i )
		{
			if ( mObstacles[ i ]->IsActive() )
			{
				activeObstacles.push_back(  mObstacles[ i ] );
				obstaclePolys.push_back( ClipperLib::Polygon() );
				ConvertPoly( obstaclePolys.back(), mObstacles[ i ]->mPoly );
			}
		}

		if ( obstaclePolys.size() > 0 )
		{
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
				while( o < activeObstacles.size() )
				{
					clip.Clear();
					clip.AddPolygons( subjectPolys, ClipperLib::ptSubject );

					const float setCost = activeObstacles[ o ]->mCost;

					while ( o < activeObstacles.size() )
					{
						if ( setCost == activeObstacles[ o ]->mCost )
						{
							clip.AddPolygon( *(obstaclePolys.begin() + o), ClipperLib::ptClip );
						}
						else
							break;

						++o;
					}

					if ( clip.Execute( ClipperLib::ctIntersection, ClipperLib::pftNonZero, ClipperLib::pftNonZero ) )
					{
						// if there are remaining obstacle sets, we need to cut them from these results
						// if this was the last set, we can add them as-is
						if ( o >= activeObstacles.size() )
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

			Material mat;
			mat.mName = va( "subsector_%d", mIndex );
			mat.mUser = &ns;
			navmesh->mRuntimeSectorCollision[ mIndex ]->Add( ns.mPoly, mat );
		}

		navmesh->mRuntimeSectorCollision[ mIndex ]->Build( false );
	}
	else if ( mSectorData->onmover() )
	{
		Material mat;
		mat.mName = va( "moversector_%d", mIndex );
		mat.mUser = this;
		navmesh->mRuntimeSectorCollision[ mIndex ] = new CollisionModel();
		navmesh->mRuntimeSectorCollision[ mIndex ]->Add( mPoly, mat );
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
	
	mCurrentTool = NULL;
}

PathPlannerNavMesh::~PathPlannerNavMesh()
{
	Shutdown();
}

PathPlannerNavMesh::NavSectorBase::NavSectorBase()
	: mArea2d( 0.0f )
{
}

AxisAlignedBox3f PathPlannerNavMesh::NavSectorBase::CalculateAABB() const
{
	AxisAlignedBox3f aabb;
	aabb.Clear();
	for ( size_t i = 0; i < mPoly.size(); ++i )
		aabb.ExpandPt( mPoly[ i ] );
	return aabb;
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
	dest.mPlane = Utils::CalculatePlane( dest.mPoly );
	dest.CalculateArea();
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

void PathPlannerNavMesh::NavSectorBase::Optimize()
{
	Utils::WeldVertices( mPoly, 1.0f );
}

void PathPlannerNavMesh::NavSectorBase::InitNew( const Vector3f & pos, const Vector3f & normal )
{
	mPoly = Utils::CreatePolygon( pos, normal, 32768.f);
	mPlane = Plane3f( normal, pos );
	CalculateArea();
}

void PathPlannerNavMesh::NavSectorBase::Init( const Vector3List & poly )
{
	mPoly = poly;
	mPlane = Plane3f( poly[ 0 ], poly[ 1 ], poly[ 2 ] );
	CalculateArea();
	CalculatePlane();
}

void PathPlannerNavMesh::NavSectorBase::SetVertZ( size_t i, float z ) 
{
	mPoly[ i ].Z() = z; 
	CalculateArea(); 
	CalculatePlane() ;
}

void PathPlannerNavMesh::NavSectorBase::CalculateArea()
{
	size_t i1, i2;
	mArea2d = 0.0f;
	for ( i1 = 0; i1 < mPoly.size(); i1++ ) {
		i2 = i1+1;
		if(i2 == mPoly.size()) i2 = 0;
		mArea2d += mPoly[i1].X() * mPoly[i2].Y() - mPoly[i1].Y() * mPoly[i2].X();
	}
	mArea2d *= 0.5f;
}

void PathPlannerNavMesh::NavSectorBase::CalculatePlane()
{
	mPlane = Utils::CalculatePlane( mPoly );
}

bool PathPlannerNavMesh::Init( System & system )
{
	InitCommands();
	return true;
}

//static void DrawPoly( bool solid, const PathPlannerNavMesh::RuntimeNavSector & ns, const TPPLPoly & poly, float offset, obColor color, float dur, float vertSize )
//{
//	Vector3List polyList;
//	for ( int i = 0; i < poly.GetNumPoints(); ++i )
//	{
//		Vector3f vec3d( (float)poly[ i ].x, (float)poly[ i ].y, 0.0f );
//		Utils::PushPointToPlane( vec3d, ns.mPlane, -Vector3f::UNIT_Z );
//
//		vec3d.Z() += offset;
//		polyList.push_back( vec3d );
//	}
//
//	if ( solid )
//		RenderBuffer::AddPolygonFilled( polyList, color );
//	else
//		RenderBuffer::AddPolygonSilouette( polyList, color );
//
//	if ( vertSize > 0.0 )
//	{
//		RenderBuffer::AddPoints( polyList, COLOR::MAGENTA, vertSize );
//	}
//}
//
//static void DrawPoly( bool solid, const PathPlannerNavMesh::RuntimeNavSector & ns, const ClipperLib::Polygon & poly, float offset, obColor color, float dur, float vertSize )
//{
//	Vector3List polyList;
//	for ( ClipperLib::Polygon::const_iterator it = poly.begin();
//		it != poly.end();
//		++it )
//	{
//		const ClipperLib::IntPoint & pt = *it;
//		Vector3f vec3d( (float)pt.X, (float)pt.Y, 0.0f );
//
//		Utils::PushPointToPlane( vec3d, ns.mPlane, -Vector3f::UNIT_Z );
//
//		vec3d.Z() += offset;
//		polyList.push_back( vec3d );
//	}
//
//	if ( solid )
//		RenderBuffer::AddPolygonFilled( polyList, color );
//	else
//		RenderBuffer::AddPolygonSilouette( polyList, color );
//
//	if ( vertSize > 0.0 )
//	{
//		RenderBuffer::AddPoints( polyList, COLOR::MAGENTA, vertSize );
//	}
//}

obColor PathPlannerNavMesh::RuntimeNavSector::GetRenderColor() const
{
	const obuint8 alpha = mSectorData->ledge() ? 50 : 100;
	obColor SEC_COLOR = mMirrored ? COLOR::CYAN.fade(alpha) : COLOR::GREEN.fade(alpha);

	if ( mCost != 1.0 )
	{
		SEC_COLOR = obColor::lerp( COLOR::WHITE, COLOR::RED, Mathf::Clamp( mCost / NavPortal::MAX_COST, 0.0f, 1.0f ) ).fade( 180 );
	}

	return SEC_COLOR;
}

void PathPlannerNavMesh::ShowAimSector()
{
	Vector3f eyePos, eyeDir;
	if ( Utils::GetLocalEyePosition( eyePos ) && Utils::GetLocalFacing( eyeDir ) )
	{
		NavCollision nc = FindCollision( eyePos, eyePos + eyeDir * 2048.0f );
		if( nc.DidHit() )
		{
			if ( nc.HitSector() != NULL )
			{
				std::stringstream sectorInfo;

				sectorInfo << "Sector: " << nc.HitSector()->mIndex << std::endl;
				sectorInfo << "Cost: " << nc.HitSector()->mCost << std::endl;
				sectorInfo << "Portals #: " << nc.HitSector()->mPortals.size() << std::endl;

				//nc.HitSector()->IsMirroredSector()
				switch( nc.HitSector()->mSector->mMirror )
				{
				case NavmeshIO::Sector_MirrorDir_MirrorNone:
					break;
				case NavmeshIO::Sector_MirrorDir_MirrorX:
					sectorInfo << "Mirror: X" << std::endl;
					break;
				case NavmeshIO::Sector_MirrorDir_MirrorNX:
					sectorInfo << "Mirror: -X" << std::endl;
					break;
				case NavmeshIO::Sector_MirrorDir_MirrorY:
					sectorInfo << "Mirror: Y" << std::endl;
					break;
				case NavmeshIO::Sector_MirrorDir_MirrorNY:
					sectorInfo << "Mirror: -Y" << std::endl;
					break;
				case NavmeshIO::Sector_MirrorDir_MirrorZ:
					sectorInfo << "Mirror: Z" << std::endl;
					break;
				case NavmeshIO::Sector_MirrorDir_MirrorNZ:
					sectorInfo << "Mirror: -Z" << std::endl;
					break;
				case NavmeshIO::Sector_MirrorDir_ReflectX:
					sectorInfo << "Mirror: Reflect X" << std::endl;
					break;
				case NavmeshIO::Sector_MirrorDir_ReflectY:
					sectorInfo << "Mirror: Reflect Y" << std::endl;
					break;
				case NavmeshIO::Sector_MirrorDir_ReflectZ:
					sectorInfo << "Mirror: Reflect Z" << std::endl;
					break;
				}

				const Vector3f txtPos = nc.HitPosition();
				RenderBuffer::AddString3d( txtPos + Vector3f(0.f,0.f,32.f), COLOR::BLUE, sectorInfo.str().c_str() );
			}
		}
	}
}


static void BuildDisplayString( StringVector & strOut, const google::protobuf::Message & msg0, const google::protobuf::Message & msg1, const std::string & varPath = "" )
{
	using namespace google;

	std::vector<const protobuf::FieldDescriptor*> fields0;
	std::vector<const protobuf::FieldDescriptor*> fields1;
	msg0.GetReflection()->ListFields( msg0, &fields0 );
	msg1.GetReflection()->ListFields( msg1, &fields1 );

	// what fields do the sectors share
	while( fields0.size() > 0 )
	{
		const protobuf::FieldDescriptor * field = fields0.back();

		const std::string fullVarPath = !varPath.empty() ? (varPath + "." + field->lowercase_name()) : field->lowercase_name();

		if ( !field->is_repeated() && !field->options().GetExtension( NavmeshIO::hidden ) )
		{
			if ( field->type() == protobuf::FieldDescriptorProto_Type_TYPE_MESSAGE )
			{			
				const protobuf::Message & childMsg0 = msg0.GetReflection()->GetMessage( msg0, field );
				const protobuf::Message & childMsg1 = msg1.GetReflection()->GetMessage( msg1, field );
				BuildDisplayString( strOut, childMsg0, childMsg1, fullVarPath );
			}
			else
			{
				std::string s = va( "%s - %s",
					fullVarPath.c_str(),
					GetFieldString( msg0, field ).c_str() );

				if ( msg1.GetReflection()->HasField( msg1, field ) )
					s += " <*> ";

				strOut.push_back( s );
			}
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
			const std::string fullVarPath = !varPath.empty() ? (varPath + "." + field->lowercase_name()) : field->lowercase_name();
			if ( field->type() == protobuf::FieldDescriptorProto_Type_TYPE_MESSAGE )
			{
				const protobuf::Message & childMsg0 = msg0.GetReflection()->GetMessage( msg0, field );
				const protobuf::Message & childMsg1 = msg1.GetReflection()->GetMessage( msg1, field );
				BuildDisplayString( strOut, childMsg0, childMsg1, fullVarPath );
			}
			else
			{
				std::string s = va( "%s - %s",
					fullVarPath.c_str(),
					GetFieldString( msg1, field ).c_str() );
				s += " <m> ";

				strOut.push_back( s );
			}
		}

		fields0.erase( std::remove( fields0.begin(), fields0.end(), field ), fields0.end() );
		fields1.erase( std::remove( fields1.begin(), fields1.end(), field ), fields1.end() );
	}
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

	if ( m_PlannerFlags.CheckFlag(NAV_VIEW) && Utils::GetLocalEntity().IsValid() )
	{
		//////////////////////////////////////////////////////////////////////////
		Vector3f aimPos, aimNormal;
		if(Utils::GetLocalAimPoint( aimPos, &aimNormal, TR_MASK_FLOODFILLENT) )
		{
			RenderBuffer::AddLine(aimPos,aimPos + aimNormal * 16.f,m_CursorColor);
			RenderBuffer::AddLine(aimPos,aimPos + aimPos.Perpendicular() * 16.f, m_CursorColor);
		}
		m_CursorColor = COLOR::BLUE; // back to default

		ShowAimSector();
		
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
										
					RenderBuffer::AddPolygonFilled( subSector.GetVerts(), subSector.GetRenderColor() );

					// render edge polys based on edge mask
					for ( size_t p = 0; p < subSector.GetNumVerts(); ++p )
					{
						const Vector3f v0 = subSector.GetVert( p );
						const Vector3f v1 = subSector.GetVert( (p+1) % subSector.GetNumVerts() );

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
				RenderBuffer::AddPolygonFilled( ns.GetVerts(), ns.GetRenderColor() );

				// render edge polys based on edge mask
				for ( size_t p = 0; p < ns.GetNumVerts(); ++p )
				{
					const Vector3f v0 = ns.GetVert( p );
					const Vector3f v1 = ns.GetVert( (p+1) % ns.GetNumVerts() );

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
			RenderBuffer::AddPoints( ns.GetVerts(), ptColor, 5.0f );

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
						
			if ( m_PlannerFlags.CheckFlag( NAVMESH_SHOWOBSTACLES ) )
			{
				for ( size_t o = 0; o < ns.mObstacles.size(); ++o )
				{
					Obstacle * obs = ns.mObstacles[ o ];

					Box3f obb;
					if ( EngineFuncs::EntityWorldOBB( obs->mEntity, obb ) )
						RenderBuffer::AddOBB( obb, COLOR::BLUE );
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
					
					// fields for mirrored
					NavmeshIO::SectorData & data = *sec->mSectorData;
					NavmeshIO::SectorData & otherData = sec->IsMirroredSector() ? sec->mSector->mSectorData : sec->mSector->mSectorDataMirrored;

					StringVector strs;
					BuildDisplayString( strs, data, otherData );
					
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
			/*CollisionModel::MaterialSet skipSet;
			for ( size_t i = 0; i < mRuntimeSectorCollision.size(); ++i )
			{
				if ( mRuntimeSectorCollision[ i ] )
				{
					skipSet.insert( &mRuntimeSectors[ i ] );

					mRuntimeSectorCollision[ i ]->Render( ModelTransform::sIdentity, Vector3f( 0.0f, 0.0f, 16.0f ) );
				}
			}

			mSectorCollision.RenderSkipSet( skipSet, Vector3f( 0.0f, 0.0f, 16.0f ) );*/
		}

		//////////////////////////////////////////////////////////////////////////

		if( mEditSector.GetNumVerts() > 0 )
		{
			RenderBuffer::AddLine(
				mEditSectorStart,
				mEditSectorStart + mEditSectorPlane.Normal * 10.0f,
				COLOR::MAGENTA );

			RenderBuffer::AddPolygonFilled( mEditSector.GetVerts(), COLOR::BLUE.fade(100) );
			if ( mEditSector.IsMirrored() )
			{
				NavSectorBase temp;
				mEditSector.GetMirroredCopy(m_MapCenter, mEditSector.mMirror, temp);
				RenderBuffer::AddPolygonFilled( temp.GetVerts(), COLOR::CYAN.fade(100) );
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
				throw std::exception( "unable to parse navmesh from binary" );
		}
		else if( navFile.OpenForRead( navPathText.c_str(), File::Text ) )
		{
			navFile.ReadWholeFile( fileContents );

			if ( !TextFormat::ParseFromString( fileContents, &nm ) )
				throw std::exception( "unable to parse navmesh from text" );
		} 
		else
		{
			throw std::exception( "unable to open binary or text" );
		}

		m_MapCenter.X() = nm.header().mapcenter().x();
		m_MapCenter.Y() = nm.header().mapcenter().y();
		m_MapCenter.Z() = nm.header().mapcenter().z();

		mNavSectors.clear();
		mNavSectors.reserve( nm.sectors_size() );

		Vector3List vertices;
		for ( int i = 0; i < nm.sectors_size(); ++i )
		{
			vertices.resize( 0 );

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
				vertices.push_back( Vector3f(
					v.position().x(),
					v.position().y(),
					v.position().z() ) );
			}
			sector.Init( vertices );

			sector.Optimize();

			if ( sector.GetNumVerts() < 3 )
				continue;

			// calculate normal
			const Vector3f center = sector.CalculateCenter();
			sector.SetPlane( Plane3f( center, sector.GetVert( 0 ), sector.GetVert( 1 ) ) );

			mNavSectors.push_back( sector );
		}

		if ( System::mInstance->mAnalytics )
			System::mInstance->mAnalytics->AddGameEvent( _mapname.c_str(), "Navmesh:Load" );
	}
	catch (const std::exception & e)
	{
		LOG("PathPlannerNavMesh::Load ERROR: " << e.what() );

		if ( System::mInstance->mAnalytics )
			System::mInstance->mAnalytics->AddQualityEvent( _mapname.c_str(), "Navmesh:LoadFail", e.what() );
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

		for(obuint32 v = 0; v < ns.GetNumVerts(); ++v)
		{
			NavmeshIO::SectorVert * sectorVertOut = sectorOut->add_vertices();
			sectorVertOut->mutable_position()->set_x( ns.GetVert( v ).X() );
			sectorVertOut->mutable_position()->set_y( ns.GetVert( v ).Y() );
			sectorVertOut->mutable_position()->set_z( ns.GetVert( v ).Z() );
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
	catch (const std::exception & e)
	{
		LOG("PathPlannerNavMesh::Save ERROR: " << e.what() );

		if ( System::mInstance->mAnalytics )
			System::mInstance->mAnalytics->AddQualityEvent( _mapname.c_str(), "Navmesh:SaveFail", e.what() );
		return false;
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

//bool PathPlannerNavMesh::GetRandomDestination( Vector3f & dstOut, Client *_client )
//{
//	dstOut = _client->GetPosition();
//
//	if ( mRuntimeSectors.empty() )
//		return false;
//
//	size_t numSectors = 0;
//	float totalArea = 0.0f;
//	int * validSectors = (int*)StackAlloc( mRuntimeSectors.size() * sizeof( int ) );
//	float * validSectorAreas = (float*)StackAlloc( mRuntimeSectors.size() * sizeof( float ) );
//
//	for ( size_t i = 0; i < mRuntimeSectors.size(); ++i )
//	{
//		const PathPlannerNavMesh::RuntimeNavSector & sec = mRuntimeSectors[ i ];
//
//		if ( sec.mSectorData->noroam() )
//			continue;
//
//		if ( sec.IsUsableBy( _client ) )
//		{
//			validSectors[ numSectors ] = i;
//			validSectorAreas[ numSectors ] = sec.GetArea2d();
//			totalArea += validSectorAreas[ numSectors ];
//			numSectors++;
//		}
//	}
//
//	if ( numSectors == 0 )
//		return false;
//	
//	const float r = Mathf::IntervalRandom( 0.0, totalArea );
//	float randAccum = r;
//	for ( size_t i = 0; i < numSectors; ++i )
//	{
//		randAccum -= validSectorAreas[ i ];
//		if ( randAccum <= 0.0f )
//		{
//			const PathPlannerNavMesh::RuntimeNavSector & sec = mRuntimeSectors[ validSectors[ i ] ];
//			dstOut = sec.CalculateCenter(); // todo: random point in sector?
//			return true;
//		}
//	}
//	return false;
//}

//////////////////////////////////////////////////////////////////////////

void PathPlannerNavMesh::Unload()
{
	for ( size_t i = 0; i < mRuntimeSectorCollision.size(); ++i )
		OB_DELETE( mRuntimeSectorCollision[ i ] );

	OB_DELETE( mCurrentTool );

	mSectorCollision.Free();
	mRuntimeSectors.clear();
	mNavSectors.clear();
}

void PathPlannerNavMesh::InitSectors()
{
	// reset all obstacles
	mPendingObstacles.insert( mPendingObstacles.end(), mObstacles.begin(), mObstacles.end() );
	mObstacles.resize( 0 );

	for ( size_t i = 0; i < mObstacles.size(); ++i )
	{
		mObstacles[ i ]->mAffectedSectors.clear();
	}

	mSectorCollision.Free();

	mRuntimeSectors.clear();

	RuntimeSectorList	allSectors;

	// create the active sector list and the collision data at the same time
	// so we can create a proper mapping
	for ( size_t i = 0; i < mNavSectors.size(); ++i )
	{
		RuntimeNavSector runtimeSector( allSectors.size(), &mNavSectors[ i ], &mNavSectors[ i ].mSectorData );
		runtimeSector.Init( mNavSectors[ i ].GetVerts() );
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

		Material mat;
		mat.mName = va( "sector_%d", i );
		mat.mUser = &ns;

		// only add static sector collision for non movable sectors
		if ( !ns.mSectorData->onmover() )
			mSectorCollision.Add( ns.GetVerts(), mat );

		// calculate drop dependencies for ledges
		if ( ns.mSectorData->ledge() || ns.mSectorData->inwater() )
		{
			AxisAlignedBox3f ledgeBounds = ns.CalculateAABB();
			ledgeBounds.Min.Z() -= 10000.0f; // allow any drop down
			
			for ( size_t j = 0; j < mRuntimeSectors.size(); ++j )
			{
				if ( i == j )
					continue;

				RuntimeNavSector & other = mRuntimeSectors[ j ];

				// don't connect ledges to other ledges
				if ( other.mSectorData->ledge() )
					continue;

				const AxisAlignedBox3f otherBounds = other.CalculateAABB();

				if ( !ledgeBounds.TestIntersection( otherBounds ) )
					continue;

				// see if the polygons actually overlap
				ClipperLib::Polygons subjectPolys;
				ClipperLib::Polygons obstaclePolys;

				subjectPolys.push_back( ClipperLib::Polygon() );
				ConvertPoly( subjectPolys.back(), ns.GetVerts() );
				obstaclePolys.push_back( ClipperLib::Polygon() );
				ConvertPoly( obstaclePolys.back(), other.GetVerts() );
				
				ClipperLib::Clipper clip;
				clip.AddPolygons( subjectPolys, ClipperLib::ptSubject );
				clip.AddPolygons( obstaclePolys.begin(), obstaclePolys.end(), ClipperLib::ptClip );

				PathPlannerNavMesh::RuntimeSectorList dropSectors;
				if ( clip.Execute( ClipperLib::ctUnion, ClipperLib::pftNonZero, ClipperLib::pftNonZero ) )
					GenerateFromClipResults( clip, ns, 1.0f, dropSectors );

				if ( !dropSectors.empty() )
				{
					ns.mDropConnections.insert( &other );
					other.mPortalDependencies.insert( &ns );
				}
			}
		}
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
	SectorSet buildPortals;
	
	float minCost = std::numeric_limits<float>::max();
	float maxCost = std::numeric_limits<float>::lowest();
	
	for ( size_t i = 0; i < mRuntimeSectors.size(); ++i )
	{
		RuntimeNavSector & runtimeSector = mRuntimeSectors[ i ];

		minCost = std::min( minCost, runtimeSector.mCost );
		maxCost = std::max( maxCost, runtimeSector.mCost );
	
		if ( !m_PlannerFlags.CheckFlag( NAVMESH_DISABLEOBSTACLES ) )
		{
			for ( size_t o = 0; o < runtimeSector.mObstacles.size(); ++o )
			{
				Obstacle * obs = runtimeSector.mObstacles[ o ];

				Vector3f vel;
				BitFlag64 flags;
				if ( !EngineFuncs::EntityVelocity( obs->mEntity, vel ) || 
					!InterfaceFuncs::GetEntityFlags( obs->mEntity, flags ) )
				{
					obs->mExpireTime = 0;
				}
				else
				{
					const bool nowActive = vel.IsZero() && !flags.CheckFlag( ENT_FLAG_DISABLED );
					if ( obs->mActive != nowActive )
					{
						obs->mActive = nowActive;
						runtimeSector.mUpdateObstacles = true;

						if ( !obs->mActive )
							obs->mExpireTime = 0;
					}
				}

				if ( obs->Expired() )
					runtimeSector.mObstacles.erase( std::remove( runtimeSector.mObstacles.begin(), runtimeSector.mObstacles.end(), obs ), runtimeSector.mObstacles.end() );
			}
		}

		if ( runtimeSector.mUpdateObstacles )
		{
			runtimeSector.RebuildObstacles( this );

			for ( SectorSet::iterator it = runtimeSector.mPortalDependencies.begin();
				it != runtimeSector.mPortalDependencies.end();
				++it )
			{
				buildPortals.insert( *it );
			}
		}

		if ( runtimeSector.mUpdatePortals )
		{
			buildPortals.insert( &runtimeSector );
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
				buildPortals.insert( &subSector );
			}
		}
	}
	
	for ( SectorSet::iterator it = buildPortals.begin(); it != buildPortals.end(); ++it )
	{
		(*it)->RebuildPortals( this );
	}
	
	// todo: invalidate neighboring sectors that connect to this sector
	// because they need to rebuild their sectors as well to create
	// new portals to the new sectors we just built here.
}

//////////////////////////////////////////////////////////////////////////

void PathPlannerNavMesh::UpdateObstacles()
{
	if ( m_PlannerFlags.CheckFlag( NAVMESH_DISABLEOBSTACLES ) )
		return;

	if ( mPendingObstacles.size() > 0 )
	{
		const size_t newObstacleStart = mObstacles.size();

		// make sure there is enough to hold them all
		mObstacles.reserve( mObstacles.size() + mPendingObstacles.size() );

		mObstacles.insert( mObstacles.end(), mPendingObstacles.begin(), mPendingObstacles.end() );
		mPendingObstacles.resize( 0 );

		const size_t newObstacleEnd = mObstacles.size();

		for ( size_t i = newObstacleStart; i < newObstacleEnd; ++i )
		{
			Obstacle * obs = mObstacles[ i ];
			
			if ( !EngineFuncs::EntityPosition( obs->mEntity, obs->mPosition ) )
				return;
			
			// if we don't find a starting sector we can't propagate it correctly
			RuntimeNavSector * obstacleSector = GetSectorAt( obs->mPosition );
			if ( obstacleSector == NULL )
			{
				obs->mExpireTime = 0;
				continue;
			}
			if ( obstacleSector->mParent )
				obstacleSector = obstacleSector->mParent;

			// don't bake moving obstacles
			Vector3f vel = Vector3f::ZERO;
			if ( !EngineFuncs::EntityVelocity( obs->mEntity, vel ) )
			{
				obs->mExpireTime = 0;
				continue;
			}

			if ( !vel.IsZero() )
				continue;

			// calculate the obstacle polygon
			Box3f obb;
			if ( !EngineFuncs::EntityWorldOBB( obs->mEntity, obb ) )
			{
				obs->mExpireTime = 0;
				continue;
			}
			
			enum { NVerts=8 };
			Vector3f vertices[ NVerts ];
			Vector2f vertices2d[ NVerts ];
			obb.ComputeVertices(vertices);
			
			for ( size_t v = 0; v < NVerts; ++v )
				vertices2d[ v ] = Vector2f( vertices[ v ].X(), vertices[ v ].Y() );
			
			Wm5::ConvexHull2f hull2d( 8, vertices2d, Mathf::EPSILON, false, Wm5::Query::QT_REAL );
			
			obs->mPoly.resize( 0 );
			for ( int h = 0; h < hull2d.GetNumSimplices(); ++h )
			{
				const Vector2f hullVec = vertices2d[ hull2d.GetIndices()[ h ] ];
				obs->mPoly.push_back( Vector3f( hullVec.X(), hullVec.Y(), obs->mPosition.Z() ) );
			}
			
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

			// flood fill and propagate clipped obstacles until the whole poly is clipped
			typedef std::list<OpenNode> ExploreOpen;

			ExploreOpen openList;

			openList.push_back( OpenNode( obstacleSector, obs->mPoly ) );

			Vector3List clippedPoly;

			size_t numAffected  = 0;

			while ( !openList.empty() )
			{
				OpenNode node = openList.front();
				openList.pop_front();
				
				// so we don't backtrack to it
				obstacleClosedSet.insert( node.mSector );
				
				// customize the poly for this sector with the clipped poly
				Obstacle * sectorObstacle = new Obstacle;
				*sectorObstacle = *obs;
				obs->mAffectedSectors.push_back( node.mSector );
				sectorObstacle->mAffectedSectors.push_back( node.mSector );
				sectorObstacle->mPoly = node.mPoly;
				node.mSector->mObstacles.push_back( sectorObstacle );
				node.mSector->mUpdateObstacles = true;
				++numAffected;
				
				// attempt to clip and propagate the poly through portals
				//for ( size_t p = 0; p < node.mSector->mPortals.size(); ++p )
				//{
				//	const NavPortal & portal = node.mSector->mPortals[ p ];

				//	if ( obstacleClosedSet.find( portal.mDstSector ) != obstacleClosedSet.end() )
				//		continue;
				//	
				//	clippedPoly = sectorObstacle->mPoly;

				//	// clip the portal normal
				//	clippedPoly = Utils::ClipPolygonToPlanes( clippedPoly, Plane3f( portal.mSegmentFwd, portal.mSegment.Center ) );
				//	if ( clippedPoly.empty() )
				//		continue;

				//	// clip the portal view planes
				//	clippedPoly = Utils::ClipPolygonToPlanes( clippedPoly, Plane3f( obs->mPosition, obs->mPosition + Vector3f::UNIT_Z, portal.mSegment.P0 ) );
				//	if ( clippedPoly.empty() )
				//		continue;
				//	clippedPoly = Utils::ClipPolygonToPlanes( clippedPoly, Plane3f( obs->mPosition, obs->mPosition - Vector3f::UNIT_Z, portal.mSegment.P1 ) );
				//	if ( clippedPoly.empty() )
				//		continue;
				//	
				//	openList.push_back( OpenNode( portal.mDstSector, clippedPoly ) );
				//}
			}

			//EngineFuncs::ConsoleMessage( va( "Obstacle %d affected %d sectors", obs.mHandle, numAffected ) );
		}
	}
	
	for ( size_t i = 0; i < mObstacles.size(); ++i )
	{
		Obstacle * obs = mObstacles[ i ];
		if ( obs->Expired() )
		{
			for ( size_t s = 0; s < obs->mAffectedSectors.size(); ++s )
			{
				RuntimeNavSector * ns = obs->mAffectedSectors[ s ];
				ns->mObstacles.erase( std::remove( ns->mObstacles.begin(), ns->mObstacles.end(), obs ), ns->mObstacles.end() );
				ns->mUpdateObstacles = true;
			}

			OB_DELETE( obs );
			mObstacles.erase( mObstacles.begin() + i );
			--i;
		}
		else
		{
			Vector3f vel;
			BitFlag64 flags;
			if ( !EngineFuncs::EntityVelocity( obs->mEntity, vel ) || 
				!InterfaceFuncs::GetEntityFlags( obs->mEntity, flags ) )
			{
				obs->mExpireTime = 0;
			}
			else
			{
				const bool nowActive = vel.IsZero() && !flags.CheckFlag( ENT_FLAG_DISABLED );
				if ( !nowActive )
				{
					obs->mActive = false;

					mObstacles.erase( mObstacles.begin() + i );
					--i;

					// put it back in the queue for re-adding when it settles again
					mPendingObstacles.push_back( obs );
				}
			}
		}
	}
	
	Vector3f eyePos, eyeDir;
	if ( Utils::GetLocalEyePosition( eyePos ) && Utils::GetLocalFacing( eyeDir ) )
	{
		NavCollision nc = FindCollision( eyePos, eyePos + eyeDir * 2048.0f );
		if( nc.DidHit() )
		{
			const RuntimeNavSector * mySector = nc.HitSector();
			if ( mySector )
			{
				const Vector3f aimPos = nc.HitPosition();
				for ( size_t i = 0; i < mySector->mPortals.size(); ++i )
				{
					const NavPortal & portal =  mySector->mPortals[ i ];

					// clip the portal normal
					RenderBuffer::AddArrow( portal.mSegment.Center, portal.mSegment.Center + portal.mSegmentFwd * 32.0f, COLOR::CYAN );
					RenderBuffer::AddArrow( portal.mSegment.P0, portal.mSegment.P0 + Plane3f( aimPos, aimPos + Vector3f::UNIT_Z, portal.mSegment.P0 ).Normal * 32.0f, COLOR::RED );
					RenderBuffer::AddArrow( portal.mSegment.P1, portal.mSegment.P1 + Plane3f( aimPos, aimPos - Vector3f::UNIT_Z, portal.mSegment.P1 ).Normal * 32.0f, COLOR::BLUE );
				}
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

PathPlannerNavMesh::NavCollision PathPlannerNavMesh::FindCollision( const Vector3f & from, const Vector3f & to, CollisionModel::CacheId * lastHit )
{
	float closestHit = SquaredLength( from, to );

	CollisionModel::RayResult result;
	if ( mSectorCollision.CollideRay( ModelTransform::sIdentity, result, from, to, lastHit ) )
	{
		closestHit = SquaredLength( from, result.mHitPos );
	}

	// try the dynamic collisions
	CollisionModel * closestMdl = NULL;
	for ( size_t i = 0; i < mRuntimeSectorCollision.size(); ++i )
	{
		if ( mRuntimeSectorCollision[ i ] != NULL )
		{
			CollisionModel::RayResult subResult;
			if ( mRuntimeSectorCollision[ i ]->CollideRay( ModelTransform::sIdentity, subResult, from, to, lastHit ) )
			{
				const float subHitDistSq = SquaredLength( from, subResult.mHitPos );
				if ( subHitDistSq < closestHit )
				{
					closestMdl = mRuntimeSectorCollision[ i ];
					closestHit = subHitDistSq;
					result = subResult;
				}
			}
		}
	}

	if ( closestMdl )
	{
		const Material & mat = closestMdl->GetMaterial( result.mHitMaterialIndex );
		return NavCollision( result.mHitPos, result.mHitNormal, static_cast<RuntimeNavSector*>( mat.mUser ) );
	}
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
											const CollisionModel::MaterialSet & attribs,
											const bool staticOnly )
{
	//for ( CollisionModel::MaterialSet::iterator it = attribs.begin(); it != attribs.end(); ++it )
	//{
	//	RuntimeNavSector * ns = static_cast<RuntimeNavSector*>( *it );

	//	if ( staticOnly || mRuntimeSectorCollision[ ns->mIndex ] == NULL )
	//		sectorsOut.push_back( ns ); // no sub collision, so return this sector
	//	else
	//	{
	//		// return sub sector collisions
	//		CollisionModel::MaterialSet subAttribs;
	//		mRuntimeSectorCollision[ ns->mIndex ]->CollideAABB( ModelTransform::sIdentity, aabb, subAttribs );

	//		for ( CollisionModel::MaterialSet::iterator it2 = subAttribs.begin(); it2 != subAttribs.end(); ++it2 )
	//		{
	//			RuntimeNavSector * subSector = static_cast<RuntimeNavSector*>( *it2 );
	//			sectorsOut.push_back( subSector );
	//		}
	//	}
	//}
}

void PathPlannerNavMesh::SectorsFromAttribs( RuntimeSectorRefs & sectorsOut,
											const Box3f & obb,
											const CollisionModel::MaterialSet & attribs,
											const bool staticOnly )
{
	//for ( CollisionModel::MaterialSet::iterator it = attribs.begin(); it != attribs.end(); ++it )
	//{
	//	RuntimeNavSector * ns = static_cast<RuntimeNavSector*>( *it );

	//	if ( staticOnly || mRuntimeSectorCollision[ ns->mIndex ] == NULL )
	//		sectorsOut.push_back( ns ); // no sub collision, so return this sector
	//	else
	//	{
	//		// return sub sector collisions
	//		CollisionModel::MaterialSet subAttribs;
	//		mRuntimeSectorCollision[ ns->mIndex ]->CollideOBB( obb, subAttribs );

	//		for ( CollisionModel::MaterialSet::iterator it2 = subAttribs.begin(); it2 != subAttribs.end(); ++it2 )
	//		{
	//			RuntimeNavSector * subSector = static_cast<RuntimeNavSector*>( *it2 );
	//			sectorsOut.push_back( subSector );
	//		}
	//	}
	//}
}

int PathPlannerNavMesh::FindRuntimeSectors( const AABB & aabb,
										   RuntimeSectorRefs & sectorsOut,
										   const bool staticOnly )
{
	sectorsOut.resize( 0 );

	CollisionModel::MaterialSet attribs;
	mSectorCollision.CollideAABB( ModelTransform::sIdentity, aabb, attribs );
	SectorsFromAttribs( sectorsOut, aabb, attribs, staticOnly );
	return (int)sectorsOut.size();
}

int PathPlannerNavMesh::FindRuntimeSectors( const Box3f & obb,
										   RuntimeSectorRefs & sectorsOut,
										   const bool staticOnly )
{
	sectorsOut.resize( 0 );

	CollisionModel::MaterialSet attribs;
	mSectorCollision.CollideOBB( ModelTransform::sIdentity, obb, attribs );
	SectorsFromAttribs( sectorsOut, obb, attribs, staticOnly );
	return (int)sectorsOut.size();
}

//////////////////////////////////////////////////////////////////////////

void PathPlannerNavMesh::EntityCreated( const EntityInstance &ei )
{
	if ( ei.m_EntityCategory.CheckFlag(ENT_CAT_OBSTACLE) )
	{
		Obstacle * obs = new Obstacle;
		obs->mEntity = ei.m_Entity;
		obs->mCost = NavPortal::MAX_COST; // TODO		
		mPendingObstacles.push_back( obs );

		///////////////////////////////////////////////////////////////////////////////////////
		// test field of view obstacle
		//Matrix3f orient;
		//if ( EngineFuncs::EntityOrientation( ei.m_Entity, orient ) )
		//{
		//	if ( ei.m_EntityClass == 12 || ei.m_EntityClass == 37 ) // sentry, turret
		//	{
		//		Obstacle fovObs;
		//		fovObs.mPosition = obs.mPosition;
		//		fovObs.mEntity = ei.m_Entity;
		//		fovObs.mCost = 9.0f;
		//		fovObs.mPoly.push_back( fovObs.mPosition );
		//		fovObs.mPoly.push_back( fovObs.mPosition + Vector3f( orient.GetColumn( 0 ) ) * 1200.0f + Vector3f( orient.GetColumn( 1 ) ) * 600.0f );
		//		fovObs.mPoly.push_back( fovObs.mPosition + Vector3f( orient.GetColumn( 0 ) ) * 1200.0f - Vector3f( orient.GetColumn( 1 ) ) * 600.0f );
		//		mPendingObstacles.push_back( fovObs );
		//	}

		//}
	}
}

void PathPlannerNavMesh::EntityDeleted( const EntityInstance &ei )
{
	for ( size_t i = 0; i < mPendingObstacles.size(); ++i )
	{
		if ( mPendingObstacles[ i ]->mEntity == ei.m_Entity )
		{
			OB_DELETE( mPendingObstacles[ i ] );

			mPendingObstacles.erase( mPendingObstacles.begin() + i );
			--i;
		}
	}

	for ( size_t i = 0; i < mObstacles.size(); ++i )
	{
		if ( mObstacles[ i ]->mEntity == ei.m_Entity )
		{
			// mark it for immediate removal
			mObstacles[ i ]->mExpireTime = 0;
		}
	}
}

PathInterface * PathPlannerNavMesh::AllocPathInterface( Client * client )
{
	return new NavMeshPathInterface( client, this ); 
}

//////////////////////////////////////////////////////////////////////////

