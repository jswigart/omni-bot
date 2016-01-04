#include "CollisionModel.h"
#include "IGame.h"
#include "RenderBuffer.h"
#include "PathPlannerRecast.h"
#include "Wm5IntrSegment3Box3.h"
#include "Wm5DistPoint3Triangle3.h"

#include "VBSPFileImporter.h"
#include "Q3BSPFileImporter.h"
#include "tiny_obj_loader.h"
#include "fastlz/fastlz.h"

#include <boost/shared_array.hpp>
#include <boost/lexical_cast.hpp>

//////////////////////////////////////////////////////////////////////////

const ContentFlagsEnum ContentFlagsEnum::sKeyVal [] =
{
	{ "NONE", CONT_NONE },
	{ "SOLID", CONT_SOLID },
	{ "WATER", CONT_WATER },
	{ "SLIME", CONT_SLIME },
	{ "FOG", CONT_FOG },
	{ "MOVER", CONT_MOVER },
	{ "TRIGGER", CONT_TRIGGER },
	{ "LAVA", CONT_LAVA },
	{ "LADDER", CONT_LADDER },
	{ "TELEPORTER", CONT_TELEPORTER },
	{ "PLAYERCLIP", CONT_PLYRCLIP },
	{ "NONSOLID", CONT_NONSOLID },
	{ "JUMPPAD", CONT_JUMPPAD },
	{ "HITBOX", CONT_HITBOX },
};
const size_t ContentFlagsEnum::sKeyValCount = sizeof( ContentFlagsEnum::sKeyVal ) / sizeof( ContentFlagsEnum::sKeyVal[0] );

//////////////////////////////////////////////////////////////////////////

const SurfaceFlagsEnum SurfaceFlagsEnum::sKeyVal [] =
{
	{ "NONE", SURFACE_NONE },
	{ "NONSOLID", SURFACE_NONSOLID },
	{ "SLICK", SURFACE_SLICK },
	{ "LADDER", SURFACE_LADDER },
	{ "NOFALLDAMAGE", SURFACE_NOFALLDAMAGE },
	{ "SKY", SURFACE_SKY },
	{ "IGNORE", SURFACE_IGNORE },
	{ "NOFOOTSTEP", SURFACE_NOFOOTSTEP },
	{ "NODRAW", SURFACE_NODRAW },
	{ "HITBOX", SURFACE_HITBOX },
};
const size_t SurfaceFlagsEnum::sKeyValCount = sizeof( SurfaceFlagsEnum::sKeyVal ) / sizeof( SurfaceFlagsEnum::sKeyVal[ 0 ] );

//////////////////////////////////////////////////////////////////////////

const ModelStateEnum ModelStateEnum::sKeyVal [] =
{
	{ "Unknown", StateUnknown },
	{ "NonCollidable", StateNonCollidable },
	{ "Collidable", StateCollidable },
	{ "Moved", StateMoved },
	{ "MarkedForDelete", StateMarkedForDelete },
};

const size_t ModelStateEnum::sKeyValCount = sizeof( ModelStateEnum::sKeyVal ) / sizeof( ModelStateEnum::sKeyVal[ 0 ] );

//////////////////////////////////////////////////////////////////////////

static const Material sNullMaterial;

IceMaths::Point Convert( const Vector3f & pt )
{
	return IceMaths::Point( pt.X(), pt.Y(), pt.Z() );
}

const Vector3f ModelTransform::Pos() const
{
	return mPos;
}
const Matrix3f & ModelTransform::Orient() const
{
	return mOrient;
}

void ModelTransform::SetPosition( const Vector3f & pos, bool checkStable )
{
	if ( checkStable && mPos != pos )
		mStableTime = 0.0f;

	mPos = pos;
}

void ModelTransform::SetOrientation( const Matrix3f & orient, bool checkStable )
{
	if ( checkStable && mOrient != orient )
		mStableTime = 0.0f;

	mOrient = orient;
}

void ModelTransform::UpdateStableTime()
{
	mStableTime += IGame::GetDeltaTimeSecs();
}

void ModelTransform::SetTransform( const IceMaths::Matrix4x4 & xform, bool checkStable )
{
	Matrix3f orient;
	orient.SetColumn( 0, Convert( xform.GetRow( 0 ) ) );
	orient.SetColumn( 1, Convert( xform.GetRow( 1 ) ) );
	orient.SetColumn( 2, Convert( xform.GetRow( 2 ) ) );
	SetPosition( Convert( xform.GetTrans() ), checkStable );
	SetOrientation( orient, checkStable );
}


CollisionModel::CollisionModel()
	: mCollisionTree( NULL )
	, mMeshInterface( NULL )
	, mBoundsContents( CONT_NONE )
	, mBoundsSurface( SURFACE_NONE )
	, mModelCrc( 0 )
{
}

CollisionModel::~CollisionModel()
{
	Free();
}

void CollisionModel::Free()
{
	delete mCollisionTree;
	mCollisionTree = NULL;

	delete mMeshInterface;
	mMeshInterface = NULL;

	mVerts.clear();
	mMaterials.clear();
	mTriParms.clear();
}

boost::crc_32_type::value_type CollisionModel::GetModelCrc() const
{
	return mModelCrc;
}

void CollisionModel::UpdateModelCrc()
{
	static const uint8_t MDL_DATA_VERSION = 1;

	boost::crc_32_type mdlcrc;
	mdlcrc.process_byte( MDL_DATA_VERSION );

	if ( mVerts.size() > 0 )
		mdlcrc.process_bytes( &mVerts[ 0 ], sizeof( mVerts[ 0 ] ) * mVerts.size() );
	if ( mTriParms.size() > 0 )
		mdlcrc.process_bytes( &mTriParms[ 0 ], sizeof( mTriParms[ 0 ] ) * mTriParms.size() );

	mModelCrc = mdlcrc.checksum();
}

void CollisionModel::SaveState( RecastIO::NavigationMesh& ioNavmesh )
{
	if ( mMeshInterface == NULL )
		return;

	RecastIO::Model* ioModel = ioNavmesh.add_models();
	ioModel->set_name( GetName() );
	ioModel->set_modelcrc( GetModelCrc() );
	ioModel->set_nummeshtris( mMeshInterface->GetNbTriangles() );
	
	// only save the disabled flags if there are any bits enabled
	for ( size_t i = 0; i < mTriParms.size(); ++i )
	{
		RecastIO::Model_TriParms* triParm = NULL;

		if ( mTriParms[ i ].mSurfaceOverride != 0 )
		{
			if ( !triParm )
			{
				triParm = ioModel->add_triangles();
				triParm->set_tri( i );
			}

			triParm->set_surfaceoverride( mTriParms[ i ].mSurfaceOverride );
			break;
		}
	}
	
	/*for ( size_t i = 0; i < mMaterials.size(); ++i )
	{
		RecastIO::Material* ioMaterial = ioModel->add_material();
		ioMaterial->set_name( mMaterials[ i ].mName );
		if ( ioMaterial->contents() != ( google::protobuf::uint32 )mMaterials[ i ].mContents )
			ioMaterial->set_contents( ( google::protobuf::uint32 )mMaterials[ i ].mContents );
		if ( ioMaterial->surfaceflags() != ( google::protobuf::uint32 )mMaterials[ i ].mSurface )
			ioMaterial->set_surfaceflags( ( google::protobuf::uint32 )mMaterials[ i ].mSurface );
	}

	{
		const size_t dataSize = mVerts.size() * sizeof( IceMaths::Point );
		const size_t bufferSize = (size_t)((double)dataSize * 1.1);
		boost::shared_array<char> compressBuffer( new char[ bufferSize ] );
		const int sizeCompressed = fastlz_compress_level( 2, &mVerts[ 0 ], dataSize, compressBuffer.get() );
		ioModel->mutable_vertices()->assign( compressBuffer.get(), sizeCompressed );
	}
	{
		const size_t dataSize = mMaterialIndices.size() * sizeof( size_t );
		const size_t bufferSize = (size_t)( (double)dataSize * 1.1 );
		boost::shared_array<char> compressBuffer( new char[ bufferSize ] );
		const int sizeCompressed = fastlz_compress_level( 2, &mMaterialIndices[ 0 ], dataSize, compressBuffer.get() );
		ioModel->mutable_materialindices()->assign( compressBuffer.get(), sizeCompressed );
	}*/
}

void CollisionModel::LoadState( const RecastIO::Model& ioModel )
{
	if ( ioModel.modelcrc() != GetModelCrc() )
	{
		EngineFuncs::ConsoleError( va( "Navmesh Model %s crc mismatch, ignoring triangle flag overrides", GetName().c_str() ).c_str() );
		return;
	}

	if ( ioModel.nummeshtris() != mMeshInterface->GetNbTriangles() )
	{
		EngineFuncs::ConsoleError( va( "Navmesh Model %s number of triangles changed expected %d, got %d", 
			GetName().c_str(), ioModel.nummeshtris(), mMeshInterface->GetNbTriangles() ).c_str() );
		return;
	}
	
	for ( int i = 0; i < ioModel.triangles_size(); ++i )
	{
		const RecastIO::Model_TriParms& tri = ioModel.triangles( i );
		if ( tri.tri() < mTriParms.size() )
		{
			mTriParms[ tri.tri() ].mSurfaceOverride = (SurfaceFlags)tri.surfaceoverride();
		}
	}
}

const Material & CollisionModel::GetMaterial( size_t materialIndex ) const
{
	return materialIndex < mMaterials.size() ? mMaterials[ materialIndex ] : sNullMaterial;
}

size_t CollisionModel::FindOrAddMaterial( const Material & material )
{
	for ( size_t i = 0; i < mMaterials.size(); ++i )
	{
		if ( mMaterials[ i ] == material )
			return i;
	}

	mMaterials.push_back( material );
	return mMaterials.size() - 1;
}

void CollisionModel::Add( const Vector3List & poly, const Material & material )
{
	if ( poly.size() == 0 )
		return;

	const size_t materialIndex = FindOrAddMaterial( material );

	// generate triangles for the polygon
	for ( size_t v = 2; v < poly.size(); ++v )
	{
		TriParms parms;
		parms.mMaterialIndex = materialIndex;
		mTriParms.push_back( parms );

		mVerts.push_back( IceMaths::Point(
			poly[ 0 ].X(),
			poly[ 0 ].Y(),
			poly[ 0 ].Z() ) );

		mVerts.push_back( IceMaths::Point(
			poly[ v-1 ].X(),
			poly[ v-1 ].Y(),
			poly[ v-1 ].Z() ) );

		mVerts.push_back( IceMaths::Point(
			poly[ v % poly.size() ].X(),
			poly[ v % poly.size() ].Y(),
			poly[ v % poly.size() ].Z() ) );
	}
}

void CollisionModel::AddTri( const Vector3f & v0, const Vector3f & v1, const Vector3f & v2, const Material & material )
{
	const size_t materialIndex = FindOrAddMaterial( material );

	TriParms parms;
	parms.mMaterialIndex = materialIndex;
	mTriParms.push_back( parms );

	mVerts.push_back( IceMaths::Point( v0.X(), v0.Y(), v0.Z() ) );
	mVerts.push_back( IceMaths::Point( v1.X(), v1.Y(), v1.Z() ) );
	mVerts.push_back( IceMaths::Point( v2.X(), v2.Y(), v2.Z() ) );
}

bool CollisionModel::Build( bool createDefaultIfEmpty )
{
	if ( mVerts.size() == 0 )
	{
		if ( !createDefaultIfEmpty )
			return false;
		
		mMaterials.push_back( sNullMaterial );

		TriParms parms;
		parms.mMaterialIndex = 0;

		mVerts.push_back( IceMaths::Point( -10000.0f, -10000.0f, -10000 ) );
		mVerts.push_back( IceMaths::Point( -10000.0f,  10000.0f, -10000 ) );
		mVerts.push_back( IceMaths::Point(  10000.0f,  10000.0f, -10000 ) );
		mTriParms.push_back( parms );

		mVerts.push_back( IceMaths::Point( -10000.0f, -10000.0f, -10000 ) );
		mVerts.push_back( IceMaths::Point(  10000.0f,  10000.0f, -10000 ) );
		mVerts.push_back( IceMaths::Point( 10000.0f,  -10000.0f, -10000 ) );
		mTriParms.push_back( parms );
	}
	
	mLocalBounds.Clear();

	for( size_t i = 0; i < mMaterials.size(); ++i )
		mMaterials[ i ].mNumReferences = 0;

	mMeshInterface = new Opcode::MeshInterface();
	mMeshInterface->SetNbTriangles( mVerts.size() / 3 );
	mMeshInterface->SetNbVertices( mVerts.size() );
	mMeshInterface->SetPointers( NULL, &mVerts[ 0 ] );
	mMeshInterface->SetInterfaceType( Opcode::MESH_TRIANGLE );

	Opcode::OPCODECREATE parms;
	parms.mIMesh = mMeshInterface;
	mCollisionTree = new Opcode::Model();
	if ( mCollisionTree->Build( parms ) )
	{
		for ( size_t i = 0; i < mMeshInterface->GetNbTriangles(); ++i )
		{
			Opcode::VertexPointers vtx;
			mMeshInterface->GetTriangle( vtx, i );
			
			if ( mTriParms[ i ].mMaterialIndex < mMaterials.size() )
				++mMaterials[ mTriParms[ i ].mMaterialIndex ].mNumReferences;

			mLocalBounds.ExpandPt( Convert( *vtx.Vertex[ 0 ] ) );
			mLocalBounds.ExpandPt( Convert( *vtx.Vertex[ 1 ] ) );
			mLocalBounds.ExpandPt( Convert( *vtx.Vertex[ 2 ] ) );
		}

		size_t maxRefs = 0;
		for( size_t i = 0; i < mMaterials.size(); ++i )
		{
			if( mMaterials[ i ].mNumReferences > maxRefs )
			{
				maxRefs = mMaterials[ i ].mNumReferences;
				mBoundsContents = mMaterials[ i ].mContents;
				mBoundsSurface = mMaterials[ i ].mSurface;
			}
		}

		UpdateModelCrc();
		return true;
	}

	Free();
	return false;
}

bool CollisionModel::CollideRay( RayResult & result, const ModelTransform & mdlXform, const Vector3f & from, const Vector3f & dir, float maxDistance, SurfaceFlags ignoreSurfaces )
{
	if ( mCollisionTree != NULL )
	{
		IceMaths::Ray ray;
		ray.mOrig.Set( from.X(), from.Y(), from.Z() );
		ray.mDir.Set( dir.X(), dir.Y(), dir.Z() );
		
		IceMaths::Matrix4x4 xform;
		xform.SetRow( 0, Convert( mdlXform.Orient().GetColumn( 0 ) ) );
		xform.SetRow( 1, Convert( mdlXform.Orient().GetColumn( 1 ) ) );
		xform.SetRow( 2, Convert( mdlXform.Orient().GetColumn( 2 ) ) );
		xform.SetRow( 3, Convert( mdlXform.Pos() ) );

		Opcode::CollisionFaces faces;
		Opcode::RayCollider collider;
		collider.SetCulling( false );
		collider.SetDestination( &faces );
		collider.SetMaxDist( maxDistance );
		collider.SetClosestHit( true );
		collider.Collide( ray, *mCollisionTree, &xform );

		const Opcode::CollisionFace * hitFace = faces.GetFaces();
		if ( hitFace != NULL )
		{
			Opcode::VertexPointers v;
			mCollisionTree->GetMeshInterface()->GetTriangle( v, hitFace->mFaceID );

			IceMaths::Point normal = v.Normal();

			result.mHitPos = from + dir * hitFace->mDistance;
			result.mHitNormal = Vector3f( normal.x, normal.y, normal.z );
			result.mHitTriangle = hitFace->mFaceID;
			result.mHitDistance = hitFace->mDistance;
			return true;
		}
	}
	return false;
}

bool CollisionModel::CollideOBB( const ModelTransform & mdlXform, const Box3f & obb, Indices & hitTriangles )
{
	bool hit = false;
	if ( mCollisionTree != NULL )
	{
		IceMaths::Matrix4x4 xformMdl;
		xformMdl.SetRow( 0, Convert( mdlXform.Orient().GetColumn( 0 ) ) );
		xformMdl.SetRow( 1, Convert( mdlXform.Orient().GetColumn( 1 ) ) );
		xformMdl.SetRow( 2, Convert( mdlXform.Orient().GetColumn( 2 ) ) );
		xformMdl.SetRow( 3, Convert( mdlXform.Pos() ) );

		IceMaths::Matrix3x3 xformObb;
		xformObb.SetRow( 0, IceMaths::Point( obb.Axis[0].X(), obb.Axis[0].Y(), obb.Axis[0].Z() ) );
		xformObb.SetRow( 1, IceMaths::Point( obb.Axis[1].X(), obb.Axis[1].Y(), obb.Axis[1].Z() ) );
		xformObb.SetRow( 2, IceMaths::Point( obb.Axis[2].X(), obb.Axis[2].Y(), obb.Axis[2].Z() ) );

		IceMaths::OBB iceobb(
			IceMaths::Point( obb.Center.X(), obb.Center.Y(), obb.Center.Z() ),
			IceMaths::Point( obb.Extent[0], obb.Extent[1], obb.Extent[2] ),
			xformObb );

		Opcode::OBBCollider collider;
		collider.SetFirstContact( false );

		Opcode::OBBCache cache;
		collider.Collide( cache, iceobb, *mCollisionTree, NULL, &xformMdl );

		for ( udword i = 0; i < collider.GetNbTouchedPrimitives(); ++i )
		{
			const udword primId = collider.GetTouchedPrimitives()[ i ];
			hitTriangles.push_back( primId );
			hit = true;
		}
	}
	return hit;
}
void CollisionModel::GatherTriangles( const GatherParms & parms, const ModelTransform & mdlXform, const Sphere3f & gatherSphere,
										const CollisionTriangle& baseTriangle, GatherData& dataOut )
{
	if( parms.mMode == RecastIO::SHAPE_TRIANGLES )
	{
		if( mCollisionTree != NULL )
		{
			IceMaths::Matrix4x4 xformMdl;
			xformMdl.SetRow( 0, Convert( mdlXform.Orient().GetColumn( 0 ) ) );
			xformMdl.SetRow( 1, Convert( mdlXform.Orient().GetColumn( 1 ) ) );
			xformMdl.SetRow( 2, Convert( mdlXform.Orient().GetColumn( 2 ) ) );
			xformMdl.SetRow( 3, Convert( mdlXform.Pos() ) );

			IceMaths::Sphere icesphere(
				IceMaths::Point( gatherSphere.Center.X(), gatherSphere.Center.Y(), gatherSphere.Center.Z() ),
				gatherSphere.Radius );

			Opcode::SphereCollider collider;
			collider.SetFirstContact( false );

			Opcode::SphereCache cache;
			collider.Collide( cache, icesphere, *mCollisionTree, NULL, &xformMdl );

			// reserve space for as much as all of the touched primitives
			dataOut.mTriangles.reserve( dataOut.mTriangles.size() + collider.GetNbTouchedPrimitives() );

			for( udword i = 0; i < collider.GetNbTouchedPrimitives(); ++i )
			{
				const udword primId = collider.GetTouchedPrimitives()[ i ];
				
				size_t materialIndex = 0;
				CollisionTriangle tri = baseTriangle;
				GetTriangle( primId, mdlXform, tri.mTri.V, tri.mContents, tri.mSurface, materialIndex );

				if ( ( tri.mSurface & parms.mIgnoreSurfaces ) != 0 )
					continue;
				
				dataOut.mTriangles.push_back( tri );
			}
		}
	}
	else if( parms.mMode == RecastIO::SHAPE_OBB )
	{
		if( ( mBoundsSurface & parms.mIgnoreSurfaces ) != 0 )
			return;

		const Box3f worldObb = GetWorldOBB( mdlXform );
		IntrBox3Sphere3f intr( worldObb, gatherSphere );
		if( intr.Test() )
		{
			const Vector3f extAxis0 = worldObb.Extent[ 0 ] * worldObb.Axis[ 0 ];
			const Vector3f extAxis1 = worldObb.Extent[ 1 ] * worldObb.Axis[ 1 ];
			const Vector3f extAxis2 = worldObb.Extent[ 2 ] * worldObb.Axis[ 2 ];

			Vector3f vertex[ 8 ];
			vertex[ 0 ] = worldObb.Center - extAxis0 - extAxis1 - extAxis2;
			vertex[ 1 ] = worldObb.Center + extAxis0 - extAxis1 - extAxis2;
			vertex[ 2 ] = worldObb.Center + extAxis0 + extAxis1 - extAxis2;
			vertex[ 3 ] = worldObb.Center - extAxis0 + extAxis1 - extAxis2;
			vertex[ 4 ] = worldObb.Center - extAxis0 - extAxis1 + extAxis2;
			vertex[ 5 ] = worldObb.Center + extAxis0 - extAxis1 + extAxis2;
			vertex[ 6 ] = worldObb.Center + extAxis0 + extAxis1 + extAxis2;
			vertex[ 7 ] = worldObb.Center - extAxis0 + extAxis1 + extAxis2;

			static const int indices[ 36 ] =
			{
				// bottom
				0, 1, 2,
				0, 2, 3,

				0, 3, 4,
				3, 4, 7,

				0, 1, 4,
				1, 4, 5,

				1, 2, 5,
				2, 5, 6,

				2, 3, 6,
				3, 6, 7,

				// top
				4, 5, 6,
				4, 6, 7,
			};

			for( int i = 0; i < 36; i += 3 )
			{
				CollisionTriangle tri = baseTriangle;
				tri.mTri.V[ 0 ] = vertex[ indices[ i + 0 ] ];
				tri.mTri.V[ 1 ] = vertex[ indices[ i + 1 ] ];
				tri.mTri.V[ 2 ] = vertex[ indices[ i + 2 ] ];
				tri.mContents = mBoundsContents;
				tri.mSurface = mBoundsSurface;
				dataOut.mTriangles.push_back( tri );
			}
		}
	}
}

void CollisionModel::GatherTriangles( const GatherParms & parms, const ModelTransform & mdlXform, const Box3f & gatherObb,
										const CollisionTriangle& baseTriangle, GatherData& dataOut )
{
	assert( this != NULL );

	if( parms.mMode == RecastIO::SHAPE_TRIANGLES )
	{
		IceMaths::Matrix4x4 xformMdl;
		xformMdl.SetRow( 0, Convert( mdlXform.Orient().GetColumn( 0 ) ) );
		xformMdl.SetRow( 1, Convert( mdlXform.Orient().GetColumn( 1 ) ) );
		xformMdl.SetRow( 2, Convert( mdlXform.Orient().GetColumn( 2 ) ) );
		xformMdl.SetRow( 3, Convert( mdlXform.Pos() ) );

		IceMaths::Matrix3x3 xformObb;
		xformObb.SetRow( 0, IceMaths::Point( gatherObb.Axis[ 0 ].X(), gatherObb.Axis[ 0 ].Y(), gatherObb.Axis[ 0 ].Z() ) );
		xformObb.SetRow( 1, IceMaths::Point( gatherObb.Axis[ 1 ].X(), gatherObb.Axis[ 1 ].Y(), gatherObb.Axis[ 1 ].Z() ) );
		xformObb.SetRow( 2, IceMaths::Point( gatherObb.Axis[ 2 ].X(), gatherObb.Axis[ 2 ].Y(), gatherObb.Axis[ 2 ].Z() ) );

		IceMaths::OBB iceobb(
			IceMaths::Point( gatherObb.Center.X(), gatherObb.Center.Y(), gatherObb.Center.Z() ),
			IceMaths::Point( gatherObb.Extent[ 0 ], gatherObb.Extent[ 1 ], gatherObb.Extent[ 2 ] ),
			xformObb );

		Opcode::OBBCollider collider;
		collider.SetFirstContact( false );

		Opcode::OBBCache cache;
		collider.Collide( cache, iceobb, *mCollisionTree, NULL, &xformMdl );

		for( udword i = 0; i < collider.GetNbTouchedPrimitives(); ++i )
		{
			const udword primId = collider.GetTouchedPrimitives()[ i ];

			size_t materialIndex = 0;
			CollisionTriangle tri = baseTriangle;
			GetTriangle( primId, mdlXform, tri.mTri.V, tri.mContents, tri.mSurface, materialIndex );
			
			if ( ( tri.mSurface & parms.mIgnoreSurfaces ) != 0 )
				continue;
			
			tri.mEntity = parms.mEntity;
			dataOut.mTriangles.push_back( tri );
		}
	}
	else if( parms.mMode == RecastIO::SHAPE_OBB )
	{
		if( ( mBoundsSurface & parms.mIgnoreSurfaces ) != 0 )
			return;

		const Box3f worldObb = GetWorldOBB( mdlXform );
		IntrBox3Box3f intr( gatherObb, worldObb );
		if( intr.Test() )
		{			
			const Vector3f extAxis0 = worldObb.Extent[ 0 ] * worldObb.Axis[ 0 ];
			const Vector3f extAxis1 = worldObb.Extent[ 1 ] * worldObb.Axis[ 1 ];
			const Vector3f extAxis2 = worldObb.Extent[ 2 ] * worldObb.Axis[ 2 ];

			Vector3f vertex[ 8 ];
			vertex[ 0 ] = worldObb.Center - extAxis0 - extAxis1 - extAxis2;
			vertex[ 1 ] = worldObb.Center + extAxis0 - extAxis1 - extAxis2;
			vertex[ 2 ] = worldObb.Center + extAxis0 + extAxis1 - extAxis2;
			vertex[ 3 ] = worldObb.Center - extAxis0 + extAxis1 - extAxis2;
			vertex[ 4 ] = worldObb.Center - extAxis0 - extAxis1 + extAxis2;
			vertex[ 5 ] = worldObb.Center + extAxis0 - extAxis1 + extAxis2;
			vertex[ 6 ] = worldObb.Center + extAxis0 + extAxis1 + extAxis2;
			vertex[ 7 ] = worldObb.Center - extAxis0 + extAxis1 + extAxis2;

			//////////////////////////////////////////////////////////////////////////
			CollisionConvex convex;
			convex.mNavFlags = baseTriangle.mNavFlags;
			convex.mContents = mBoundsContents;
			convex.mSurface = mBoundsSurface;
			convex.mVertStart = dataOut.mConvexVertices.size();
			convex.mEntity = parms.mEntity;

			static const int MaxIndices = 32;
			int convexIndices[ MaxIndices ] = {};
			const int nv = ConvexhullXY( vertex, 8, convexIndices, MaxIndices, convex.mHeightMin, convex.mHeightMax );
			if ( nv > 0 )
			{
				for ( int i = 0; i < nv; ++i )
				{
					dataOut.mConvexVertices.push_back( vertex[ convexIndices[ i ] ] );
				}
				convex.mVertEnd = dataOut.mConvexVertices.size();
				dataOut.mConvexShapes.push_back( convex );
			}
			return;
			//////////////////////////////////////////////////////////////////////////

			static const int indices[ 36 ] =
			{
				// bottom
				0, 1, 2,
				0, 2, 3,

				0, 3, 4,
				3, 4, 7,

				0, 1, 4,
				1, 4, 5,

				1, 2, 5,
				2, 5, 6,

				2, 3, 6,
				3, 6, 7,

				// top
				4, 5, 6,
				4, 6, 7,
			};

			for( int i = 0; i < 36; i += 3 )
			{
				CollisionTriangle tri = baseTriangle;
				tri.mTri.V[ 0 ] = vertex[ indices[ i + 0 ] ];
				tri.mTri.V[ 1 ] = vertex[ indices[ i + 1 ] ];
				tri.mTri.V[ 2 ] = vertex[ indices[ i + 2 ] ];
				tri.mContents = mBoundsContents;
				tri.mSurface = mBoundsSurface;
				tri.mEntity = parms.mEntity;
				dataOut.mTriangles.push_back( tri );
			}
		}
	}
}

const AxisAlignedBox3f & CollisionModel::GetLocalAABB() const
{
	return mLocalBounds; 
}

Box3f CollisionModel::GetWorldOBB( const ModelTransform & mdlXform ) const
{
	Box3f worldBounds;
	mLocalBounds.GetCenterExtents( worldBounds.Center, worldBounds.Extent );

	// transform to world
	worldBounds.Axis[ 0 ] = mdlXform.Orient().GetColumn( 0 );
	worldBounds.Axis[ 1 ] = mdlXform.Orient().GetColumn( 1 );
	worldBounds.Axis[ 2 ] = mdlXform.Orient().GetColumn( 2 );
	worldBounds.Center = ( mdlXform.Pos() + mdlXform.Orient() * worldBounds.Center );
	
	return worldBounds;
}

void CollisionModel::RenderAxis( const ModelTransform & mdlXform )
{
	const Vector3f fwd = mdlXform.Orient().GetColumn( 0 );
	const Vector3f right = mdlXform.Orient().GetColumn( 1 );
	const Vector3f up = mdlXform.Orient().GetColumn( 2 );

	RenderBuffer::AddLine( mdlXform.Pos(), mdlXform.Pos() + fwd * 32.0f, COLOR::GREEN );
	RenderBuffer::AddLine( mdlXform.Pos(), mdlXform.Pos() + right * 32.0f, COLOR::BLUE );
	RenderBuffer::AddLine( mdlXform.Pos(), mdlXform.Pos() + up * 32.0f, COLOR::RED );
}

void CollisionModel::Render( const ModelTransform & mdlXform, const Vector3f & offset, const obColor & polyColor )
{
	for ( udword i = 0; i < mMeshInterface->GetNbTriangles(); ++i )
	{
		Vector3f v[ 3 ];
		ContentFlags contents;
		SurfaceFlags surface;
		size_t materialIndex = 0;
		GetTriangle( i, mdlXform, v, contents, surface, materialIndex );

		RenderBuffer::AddTri(
			v[ 0 ] + offset,
			v[ 1 ] + offset,
			v[ 2 ] + offset,
			polyColor );
		RenderBuffer::AddLine(
			v[ 0 ] + offset,
			v[ 1 ] + offset,
			COLOR::BLUE );
		RenderBuffer::AddLine(
			v[ 1 ] + offset,
			v[ 2 ] + offset,
			COLOR::BLUE );
		RenderBuffer::AddLine(
			v[ 0 ] + offset,
			v[ 2 ] + offset,
			COLOR::BLUE );
	}
}

void CollisionModel::RenderInRadius( const Vector3f & eye, float radius, const ModelTransform & mdlXform, const Vector3f & offset, const obColor & polyColor )
{
	CollisionTriangle baseTri;

	GatherData gatherData;

	GatherParms parms;
	GatherTriangles( parms, mdlXform, Sphere3f( eye, radius ), baseTri, gatherData );

	for ( size_t i = 0; i < gatherData.mTriangles.size(); ++i )
	{
		const CollisionTriangle& tri = gatherData.mTriangles[ i ];
		RenderBuffer::AddTri( tri.mTri.V[ 0 ], tri.mTri.V[ 1 ], tri.mTri.V[ 2 ], polyColor );
		RenderBuffer::AddLine( tri.mTri.V[ 0 ], tri.mTri.V[ 1 ], COLOR::BLUE );
		RenderBuffer::AddLine( tri.mTri.V[ 1 ], tri.mTri.V[ 2 ], COLOR::BLUE );
		RenderBuffer::AddLine( tri.mTri.V[ 0 ], tri.mTri.V[ 2 ], COLOR::BLUE );
	}
}

size_t	CollisionModel::GetNumVerts() const
{
	return mMeshInterface ? mMeshInterface->GetNbVertices() : 0;
}

size_t	CollisionModel::GetNumTris() const
{
	return mMeshInterface ? mMeshInterface->GetNbTriangles() : 0;
}

size_t CollisionModel::GetNumMaterials() const
{
	return mMaterials.size();
}

void CollisionModel::GetTriangle( size_t triIndex, const ModelTransform & mdlXform, Vector3f v[ 3 ], ContentFlags& contents, SurfaceFlags& surface, size_t& mtrlIndex ) const
{
	Opcode::VertexPointers vtx;
	mMeshInterface->GetTriangle( vtx, triIndex );

	// convert it to world space
	v[0] = mdlXform.Pos() + mdlXform.Orient() * Convert( *vtx.Vertex[ 0 ] );
	v[1] = mdlXform.Pos() + mdlXform.Orient() * Convert( *vtx.Vertex[ 1 ] );
	v[2] = mdlXform.Pos() + mdlXform.Orient() * Convert( *vtx.Vertex[ 2 ] );

	const TriParms& triParms = mTriParms[ triIndex ];
	const Material& mtrl = GetMaterial( triParms.mMaterialIndex );
	mtrlIndex = triParms.mMaterialIndex;
	contents = mtrl.mContents;
	surface = triParms.mSurfaceOverride ? triParms.mSurfaceOverride : mtrl.mSurface;
}

void CollisionModel::SetSurfaceOverride( size_t triIndex, SurfaceFlags surfaceFlags )
{
	if ( triIndex < mTriParms.size() )
	{
		mTriParms[ triIndex ].mSurfaceOverride = surfaceFlags;
	}
}

bool CollisionModel::IsValid() const
{
	return mCollisionTree != NULL;
}

Node::Node()
{
	Clear();
}

Node::~Node()
{
}

void Node::Clear()
{
	mEntity = GameEntity();
	mEntInfo = EntityInfo();
	mSubModel = -1;
	mStaticModel = -1;
	mDisplacement = -1;
	mActiveState = StateCollidable;
	mActiveModelCrc = 0;
	mNavFlagsActive = NAVFLAGS_NONE;
	mNavFlagsOverride = NAVFLAGS_NONE;
	mTransform = ModelTransform();
	mChildren.clear();
	mModel.reset();
	mShapeMode = RecastIO::SHAPE_TRIANGLES;
	mEnabledDefault = mEnabled = true;
	mSolid = true;
	mDynamic = false;
	mForceRebuild = false;
	mSaveable = true;
	mRuntime = false;
}

void Node::Init( PathPlannerRecast * planner )
{
	// Look up by submodel first
	if ( mSubModel != -1 )
	{
		gEngineFuncs->GetEntityForMapModel( mSubModel, mEntity );
	}

	if ( mEntity.IsValid() )
	{
		// Cache the name
		mEntityName = EngineFuncs::EntityName( mEntity, "<unknown>" );

		if ( !IGame::GetEntityInfo( mEntity, mEntInfo ) )
		{
			EngineFuncs::ConsoleMessage( va( "Entity '%s' unknown type, ignoring", mEntityName.c_str() ).c_str() );
			mEnabled = false;
		}

		if ( mEntInfo.mCategory.CheckFlag( ENT_CAT_DYNAMIC_NAV ) )
			mDynamic = true;

		if ( !mEntInfo.mCategory.CheckFlag( ENT_CAT_OBSTACLE ) )
		{
			EngineFuncs::ConsoleMessage( va( "Entity '%s' not an obstacle", mEntityName.c_str() ).c_str() );
			mEnabled = false;
		}

		if ( mEntInfo.mCategory.CheckFlag( ENT_CAT_PROP_PUSHABLE ) )
		{
			mShapeMode = RecastIO::SHAPE_OBB;
			mSolid = false;
		}

		if ( mEnabled )
			EngineFuncs::ConsoleMessage( va( "Entity '%s' is an obstacle (mdl %s)", mEntityName.c_str(), mModel->GetName().c_str() ).c_str() );
	}
	else if ( mStaticModel < 0 )
	{
		mEnabledDefault = mEnabled = false;
	}
	else if ( mDisplacement >= 0 )
	{
		// default all displacements to false
		// since they tend to be very high poly
		mEnabledDefault = mEnabled = false;
	}

	// Update the children
	for ( size_t i = 0; i < mChildren.size(); ++i )
	{
		mChildren[ i ]->Init( planner );
	}
}

void Node::UpdateModelState( PathPlannerRecast * planner, bool forcePositionUpdate )
{
	if ( mModel != NULL )
	{
		bool rebuildForced = false;
		bool rebuildModelChanged = false;
		bool rebuildNavFlagChanged = false;

		NavAreaFlags currentNavFlags = mEntInfo.mNavFlags;
		
		if ( mNavFlagsOverride != NAVFLAGS_NONE )
			currentNavFlags = mNavFlagsOverride;

		// by default, models with no entity are collidable
		ModelState currentState = StateNonCollidable;
		if ( mRuntime && !mEntity.IsValid() )
			currentState = StateNonCollidable;
		if ( mActiveState == StateMarkedForDelete )
			currentState = StateMarkedForDelete;
		else if ( !mEnabled )
			currentState = StateNonCollidable;
		else if ( mForceRebuild )
		{
			rebuildForced = true;
			mForceRebuild = false;
		}
		else if ( mActiveModelCrc != mModel->GetModelCrc() )
		{
			rebuildModelChanged = true;
		}
		else if ( currentNavFlags != mNavFlagsActive )
		{
			rebuildNavFlagChanged = true;
		}
		else if ( mEntity.IsValid() )
		{
			if ( IGame::GetEntityInfo( mEntity, mEntInfo ) )
			{
				if ( mEntInfo.mFlags.CheckFlag( ENT_FLAG_COLLIDABLE ) )
				{
					if ( !mEntInfo.mFlags.CheckFlag( ENT_FLAG_HELDBYPLAYER ) &&
						!mEntInfo.mFlags.CheckFlag( ENT_FLAG_INVEHICLE ) )
					{
						currentState = StateCollidable;
					}
				}
			}
			else
				currentState = StateNonCollidable;
		}
		else
		{
			currentState = StateCollidable;
		}

		mTransform.UpdateStableTime();

		const AxisAlignedBox3f oldBounds = ComputeAABB( mModel->GetWorldOBB( mTransform ) );

		// Is it moving?
		if ( mDynamic || forcePositionUpdate )
		{
			if ( mEntity.IsValid() )
			{
				const bool checkStable = mActiveState != StateUnknown;

				Vector3f origin = Vector3f::ZERO;
				if ( EngineFuncs::EntityPosition( mEntity, origin ) )
					mTransform.SetPosition( origin, checkStable );

				Vector3f vFwd, vRight, vUp;
				if ( EngineFuncs::EntityOrientation( mEntity, vFwd, vRight, vUp ) )
					mTransform.SetOrientation( Matrix3f( vFwd, -vRight, vUp, true ), checkStable );
			}

			if ( mTransform.GetStableTime() < 1.0f )
				currentState = StateMoved;
		}
		
		if ( rebuildForced || rebuildModelChanged || rebuildNavFlagChanged || mActiveState != currentState || currentState == StateMarkedForDelete )
		{
			if ( currentState == StateMarkedForDelete )
			{
				EngineFuncs::ConsoleMessage( va( "Deleting node model %s", mModel->GetName().c_str() ).c_str() );
			}
			else if ( rebuildForced )
			{
				EngineFuncs::ConsoleMessage( va( "Entity '%s', rebuild forced", mEntityName.c_str(), mSubModel ).c_str() );
			}
			else if ( rebuildModelChanged )
			{
				EngineFuncs::ConsoleMessage( va( "Entity '%s', model %d crc changed", mEntityName.c_str(), mSubModel ).c_str() );
			}
			else if ( rebuildNavFlagChanged )
			{
				EngineFuncs::ConsoleMessage( va( "Entity '%s', nav flag changed", mEntityName.c_str() ).c_str() );
			}
			else if ( mEntity.IsValid() )
			{
				std::string oldState, newState;
				ModelStateEnum::NameForValue( mActiveState, oldState );
				ModelStateEnum::NameForValue( currentState, newState );

				EngineFuncs::ConsoleMessage( va( "Entity '%s', model %d change state %s->%s",
					mEntityName.c_str(),
					mSubModel,
					oldState.c_str(),
					newState.c_str() ).c_str() );
			}

			mActiveState = currentState;
			mActiveModelCrc = mModel->GetModelCrc();
			mNavFlagsActive = currentNavFlags;

			// which tiles does this model touch? we need to queue them for rebuilding
			const AxisAlignedBox3f bounds = ComputeAABB( mModel->GetWorldOBB( mTransform ) );

			// we dirty both the source and destination tiles to account for large movements, teleportations, 
			planner->MarkTileForBuilding( oldBounds, bounds );
		}
	}

	// Update the children
	for ( size_t i = 0; i < mChildren.size(); ++i )
	{
		mChildren[ i ]->UpdateModelState( planner, forcePositionUpdate );

		if ( mChildren[ i ]->mActiveState == StateMarkedForDelete )
		{
			mChildren.erase( mChildren.begin() + i );
			--i;
		}
	}
}

bool Node::CollideSegmentNearest( RayResult & result, const Segment3f & segment, SurfaceFlags ignoreSurfaces, bool onlyCollidable )
{
	if ( mModel != NULL )
	{
		if ( !onlyCollidable || mActiveState == StateCollidable )
		{
			const float maxLength = result.mHitDistance < Mathf::MAX_REAL ?
				result.mHitDistance :
				segment.Extent * 2.0f;

			const Box3f worldObb = mModel->GetWorldOBB( mTransform );
			IntrSegment3Box3f intr( Segment3f( segment.P0, segment.P1 ), worldObb, false );
			if ( intr.Find() )
			{
				RayResult hitResult;
				if ( mModel->CollideRay( hitResult, mTransform, segment.P0, segment.Direction, maxLength, ignoreSurfaces ) )
				{
					if ( hitResult.mHitDistance < result.mHitDistance )
					{
						result = hitResult;
						result.mHitNode = shared_from_this();
					}
				}
			}
		}
	}
	
	for ( size_t i = 0; i < mChildren.size(); ++i )
	{
		mChildren[ i ]->CollideSegmentNearest( result, segment, ignoreSurfaces, onlyCollidable );
	}

	return result.mHitDistance < Mathf::MAX_REAL;
}

void Node::GatherTriangles( const GatherParms & parms, const Box3f & gatherObb, GatherData& dataOut )
{
	if ( mModel != NULL && mEnabled && mActiveState == StateCollidable )
	{
		CollisionTriangle baseTri;
		if ( mNavFlagsOverride != NAVFLAGS_NONE )
			baseTri.mNavFlags = mNavFlagsOverride;
		else if ( mEntInfo.mNavFlags != NAVFLAGS_NONE )
			baseTri.mNavFlags = mEntInfo.mNavFlags;
		else
			baseTri.mNavFlags = mSolid ? NAVFLAGS_WALK : NAVFLAGS_NONE;
		
		//if ( mEntInfo.mCategory.CheckFlag( ENT_CAT_TRIGGER ) )
		//	baseTri.mNavFlags = (NavAreaFlags)( baseTri.mNavFlags | NAVFLAGS_REGION ); // mark off triggers
		if ( mEntInfo.mCategory.CheckFlag( ENT_CAT_MOVER ) )
			baseTri.mNavFlags = (NavAreaFlags)( baseTri.mNavFlags | NAVFLAGS_MOVER ); // mark off movers
		if ( mEntInfo.mCategory.CheckFlag( ENT_CAT_PROP_PUSHABLE ) )
			baseTri.mNavFlags = (NavAreaFlags)( baseTri.mNavFlags | NAVFLAGS_PUSHABLE );

		GatherParms nodeParms = parms;
		nodeParms.mMode = mShapeMode;
		nodeParms.mEntity = mEntity;
		mModel->GatherTriangles( nodeParms, mTransform, gatherObb, baseTri, dataOut );
	}

	// gather from the children
	for ( size_t i = 0; i < mChildren.size(); ++i )
	{
		mChildren[ i ]->GatherTriangles( parms, gatherObb, dataOut );
	}
}

void Node::GatherTriangles( const GatherParms & parms, const Sphere3f & gatherSphere, GatherData& dataOut )
{
	if( mModel != NULL && mEnabled && mActiveState == StateCollidable )
	{
		CollisionTriangle baseTri;
		if ( mNavFlagsOverride != NAVFLAGS_NONE )
			baseTri.mNavFlags = mNavFlagsOverride;
		else if ( mEntInfo.mNavFlags != NAVFLAGS_NONE )
			baseTri.mNavFlags = mEntInfo.mNavFlags;
		else
			baseTri.mNavFlags = mSolid ? NAVFLAGS_WALK : NAVFLAGS_NONE;

		//if ( mEntInfo.mCategory.CheckFlag( ENT_CAT_TRIGGER ) )
		//	baseTri.mNavFlags = (NavAreaFlags)( baseTri.mNavFlags | NAVFLAGS_REGION ); // mark off triggers
		if ( mEntInfo.mCategory.CheckFlag( ENT_CAT_MOVER ) )
			baseTri.mNavFlags = (NavAreaFlags)( baseTri.mNavFlags | NAVFLAGS_MOVER ); // mark off movers
		if ( mEntInfo.mCategory.CheckFlag( ENT_CAT_PROP_PUSHABLE ) )
			baseTri.mNavFlags = (NavAreaFlags)( baseTri.mNavFlags | NAVFLAGS_PUSHABLE );

		GatherParms nodeParms = parms;
		nodeParms.mMode = mShapeMode;
		nodeParms.mEntity = mEntity;
		mModel->GatherTriangles( nodeParms, mTransform, gatherSphere, baseTri, dataOut );
	}

	// gather from the children
	for( size_t i = 0; i < mChildren.size(); ++i )
	{
		mChildren[ i ]->GatherTriangles( parms, gatherSphere, dataOut );
	}
}

void Node::UpdateWorldBounds( AxisAlignedBox3f & worldBounds )
{
	if ( mModel != NULL )
	{
		worldBounds.ExpandAABB( ComputeAABB( mModel->GetWorldOBB( mTransform ) ) );
	}

	for ( size_t i = 0; i < mChildren.size(); ++i )
	{
		mChildren[ i ]->UpdateWorldBounds( worldBounds );
	}
}
void Node::SetModelShapeMode( const ModelPtr & mdl, RecastIO::ShapeMode mode )
{
	if( mModel == mdl )
	{
		mShapeMode = mode;
		mForceRebuild = true;
	}

	for( size_t i = 0; i < mChildren.size(); ++i )
	{
		mChildren[ i ]->SetModelShapeMode( mdl, mode );
	}
}
void Node::SetModelEnable( const ModelPtr & mdl, bool en )
{
	if ( mModel == mdl )
	{
		mEnabled = en;
	}

	for ( size_t i = 0; i < mChildren.size(); ++i )
	{
		mChildren[ i ]->SetModelEnable( mdl, en );
	}
}
void Node::SetModelSolid( const ModelPtr & mdl, bool en )
{
	if ( mModel == mdl )
	{
		mSolid = en;
	}

	for ( size_t i = 0; i < mChildren.size(); ++i )
	{
		mChildren[ i ]->SetModelSolid( mdl, en );
	}
}
void Node::SetModelDynamic( const ModelPtr & mdl, bool en )
{
	if ( mModel == mdl )
	{
		mDynamic = en;
	}

	for ( size_t i = 0; i < mChildren.size(); ++i )
	{
		mChildren[ i ]->SetModelDynamic( mdl, en );
	}
}
void Node::RenderModel( const obColor & polyColor )
{
	if ( mModel != NULL )
		mModel->Render( mTransform, Vector3f::ZERO, polyColor );

	for ( size_t i = 0; i < mChildren.size(); ++i )
	{
		mChildren[ i ]->RenderModel( polyColor );
	}
}
void Node::RenderInRadius( const Vector3f & eye, float distance, const obColor & polyColor )
{
	if ( mModel != NULL )
		mModel->RenderInRadius( eye, distance, mTransform, Vector3f::ZERO, polyColor );

	for ( size_t i = 0; i < mChildren.size(); ++i )
	{
		mChildren[ i ]->RenderInRadius( eye, distance, polyColor );
	}
}
void Node::FindNodeWithSubModel( int subModelId, NodePtr & node )
{
	if ( mSubModel == subModelId )
	{
		node = shared_from_this();
		return;
	}

	for ( size_t i = 0; i < mChildren.size(); ++i )
	{
		mChildren[ i ]->FindNodeWithSubModel( subModelId, node );
		if ( node )
			return;
	}
}
void Node::FindNodeWithStaticModel( int staticModelId, NodePtr & node )
{
	if ( mStaticModel == staticModelId )
	{
		node = shared_from_this();
		return;
	}

	for ( size_t i = 0; i < mChildren.size(); ++i )
	{
		mChildren[ i ]->FindNodeWithStaticModel( staticModelId, node );
		if ( node )
			return;
	}
}
void Node::FindNodeWithDisplacement( int displacementId, NodePtr & node )
{
	if ( mDisplacement == displacementId )
	{
		node = shared_from_this();
		return;
	}

	for ( size_t i = 0; i < mChildren.size(); ++i )
	{
		mChildren[ i ]->FindNodeWithDisplacement( displacementId, node );
		if ( node )
			return;
	}
}
void Node::FindNodeWithEntity( GameEntity entity, NodePtr & node )
{
	if ( mEntity == entity )
	{
		node = shared_from_this();
		return;
	}

	for ( size_t i = 0; i < mChildren.size(); ++i )
	{
		mChildren[ i ]->FindNodeWithEntity( entity, node );
		if ( node )
			return;
	}
}

void Node::FindNodeWithName( const std::string& entName, NodePtr & node )
{
	if ( mEntityName == entName )
	{
		node = shared_from_this();
		return;
	}

	for ( size_t i = 0; i < mChildren.size(); ++i )
	{
		mChildren[ i ]->FindNodeWithName( entName, node );
		if ( node )
			return;
	}
}

void Node::RenderAxis()
{
	if ( mModel != NULL )
		mModel->RenderAxis( mTransform );

	for ( size_t i = 0; i < mChildren.size(); ++i )
	{
		mChildren[ i ]->RenderAxis();
	}
}

void Node::RenderWorldBounds()
{
	if ( mModel != NULL )
	{ 
		obColor activeColor = COLOR::CYAN;
		switch ( mActiveState )
		{
			case StateNonCollidable:
				activeColor = COLOR::ORANGE;
				break;
			case StateCollidable:
				activeColor = COLOR::GREEN;
				break;
			case StateMoved:
				activeColor = COLOR::YELLOW;
				break;
			case StateUnknown:
			case StateMarkedForDelete:
				activeColor = COLOR::GREY;
				break;
		}

		if ( !mEnabled )
			activeColor = COLOR::RED.fade( 100 );

		const Box3f worldObb = mModel->GetWorldOBB( mTransform );
		RenderBuffer::AddOBB( worldObb, activeColor );
	}

	for ( size_t i = 0; i < mChildren.size(); ++i )
	{
		mChildren[ i ]->RenderWorldBounds();
	}
}
void Node::SaveState( RecastIO::NavigationMesh & ioNavmesh )
{
	RecastIO::NodeState nodeState;

	if ( !mEntityName.empty() )
		nodeState.set_name( mEntityName );

	if ( mSubModel >= 0 )
		nodeState.set_submodelid( mSubModel );
	else if ( mStaticModel >= 0 )
		nodeState.set_staticmodelid( mStaticModel );
	else if ( mDisplacement >= 0 )
		nodeState.set_displacementid( mDisplacement );	
	
	bool dataSet = false;
	if ( mEnabledDefault != mEnabled )
	{
		nodeState.set_enabled( mEnabled );
		dataSet = true;
	}
	if ( nodeState.solid() != mSolid )
	{
		nodeState.set_solid( mSolid );
		dataSet = true;
	}
	if ( nodeState.dynamic() != mDynamic )
	{
		nodeState.set_dynamic( mDynamic );
		dataSet = true;
	}
	if ( nodeState.shapemode() != mShapeMode )
	{
		nodeState.set_shapemode( mShapeMode );
		dataSet = true;
	}
	if ( nodeState.activemodelcrc() != mActiveModelCrc )
	{
		nodeState.set_activemodelcrc( mActiveModelCrc );
		dataSet = true;
	}
	if ( nodeState.navflagoverride() != mNavFlagsOverride )
	{
		nodeState.set_navflagoverride( mNavFlagsOverride );
		dataSet = true;
	}

	// if all the fields are defaulted don't save it out
	if ( dataSet )
	{
		if ( nodeState.has_name() || nodeState.has_submodelid() || nodeState.has_staticmodelid() || nodeState.has_displacementid() )
		{
			ioNavmesh.add_nodestate()->CopyFrom( nodeState );
		}
		else
		{
			std::vector<const google::protobuf::FieldDescriptor*> fields;
			nodeState.GetReflection()->ListFields( nodeState, &fields );

			std::string fieldStr;
			for ( size_t i = 0; i < fields.size(); ++i )
			{ 
				if ( !fieldStr.empty() )
					fieldStr += ", ";
				fieldStr += fields[i]->camelcase_name();
			}
			
			EngineFuncs::ConsoleError( va( "Unable to save model node state(no unique identifer), fields set %s", fieldStr.c_str() ).c_str() );
		}
	}
}
void Node::LoadState( const RecastIO::NodeState & nodeState )
{
	mEnabled = nodeState.enabled();
	mSolid = nodeState.solid();
	mDynamic = nodeState.dynamic();
	mShapeMode = nodeState.shapemode();
	mActiveModelCrc = mModel ? mModel->GetModelCrc() : 0;
	mNavFlagsOverride = (NavAreaFlags)nodeState.navflagoverride();
	mEntityName = nodeState.name();
}

void Node::BuildScene( modeldata::Scene & scene, modeldata::Node & node )
{
	Vector3f eulerXYZ = Vector3f::ZERO;
	mTransform.Orient().ExtractEulerXYX( eulerXYZ.X(), eulerXYZ.Y(), eulerXYZ.Z() );
	if ( eulerXYZ.X() != 0.0 )
		node.mutable_eulerrotation()->set_x( eulerXYZ.X() );
	if ( eulerXYZ.Y() != 0.0 )
		node.mutable_eulerrotation()->set_y( eulerXYZ.Y() );
	if ( eulerXYZ.Z() != 0.0 )
		node.mutable_eulerrotation()->set_z( eulerXYZ.Z() );
	
	if ( mTransform.Pos().X() != 0.0 )
		node.mutable_translation()->set_x( mTransform.Pos().X() );
	if ( mTransform.Pos().Y() != 0.0 )
		node.mutable_translation()->set_y( mTransform.Pos().Y() );
	if ( mTransform.Pos().Z() != 0.0 )
		node.mutable_translation()->set_z( mTransform.Pos().Z() );
	
	if ( mModel != NULL )
	{		
		node.set_meshname( mModel->GetName() );
	}

	// gather from the children
	for ( size_t i = 0; i < mChildren.size(); ++i )
	{
		mChildren[ i ]->BuildScene( scene, *node.add_children() );
	}
}

CollisionWorld::CollisionWorld()
{
}

CollisionWorld::~CollisionWorld()
{
}

void CollisionWorld::Clear()
{
	mRootNode.reset( new Node() );
	
	mCachedModels.clear();
}

AxisAlignedBox3f CollisionWorld::CalcWorldAABB() const
{
	AxisAlignedBox3f aabb;
	aabb.Clear();

	mRootNode->UpdateWorldBounds( aabb );

	return aabb;
}

NodePtr CollisionWorld::LoadModelIntoWorld( const GameEntity entity, const GameModelInfo & modelInfo, const EntityInfo & entInfo )
{
	MapFormat::VBSPFileImporter vbsp;
	if ( vbsp.ParseFile( *this, modelInfo.mDataBuffer, modelInfo.mDataBufferSize ) )
		return NodePtr();

	MapFormat::Q3BSP::FileImporter bsp;
	if ( bsp.ParseFile( *this, modelInfo.mDataBuffer, modelInfo.mDataBufferSize ) )
		return NodePtr();
	
	if ( !stricmp( modelInfo.mModelType, "submodel" ) )
	{
		const std::string entityName = EngineFuncs::EntityName( entity, va( "noname(%d.%d)", entity.GetIndex(), entity.GetSerial() ).c_str() );

		try
		{
			const int subModelIndex = boost::lexical_cast <int>( modelInfo.mModelName );

			NodePtr subModelNode;
			mRootNode->FindNodeWithSubModel( subModelIndex, subModelNode );
			if ( subModelNode )
			{
				subModelNode->mEntity = entity;
				return subModelNode;
			}

			EngineFuncs::ConsoleError( va( "Entity '%s' references submodel '%d' is invalid.", entityName.c_str(), subModelIndex ).c_str() );
		}
		catch ( const boost::bad_lexical_cast & ex )
		{
			EngineFuncs::ConsoleError( ( "Submodel '%s' is invalid (%s).", modelInfo.mModelName, ex.what() ) );
		}
	}
	else if ( entInfo.mFlags.CheckFlag( ENT_FLAG_USEBOUNDS ) && !modelInfo.mAABB.IsZero() )
	{
		return CreateNodeForEntityBounds( entity, modelInfo, entInfo );
	}
	else if ( modelInfo.mDataBuffer != NULL )
	{
		return CreateNodeForEntityModel( entity, modelInfo, entInfo );
	}
	else if ( !modelInfo.mAABB.IsZero() )
	{		
		return CreateNodeForEntityBounds( entity, modelInfo, entInfo );
	}
	return NodePtr();
}

NodePtr CollisionWorld::CreateNodeForEntityModel( const GameEntity entity, const GameModelInfo & modelInfo, const EntityInfo & entInfo )
{
	Vector3f origin = Vector3f::ZERO;
	if ( !EngineFuncs::EntityPosition( entity, origin ) )
		return NodePtr();

	Vector3f vFwd, vRight, vUp;
	if ( !EngineFuncs::EntityOrientation( entity, vFwd, vRight, vUp ) )
		return NodePtr();
	
	const std::string entName = EngineFuncs::EntityName( entity, "" );

	NodePtr node;
	mRootNode->FindNodeWithName( entName, node );
	if ( !node )
	{
		node.reset( new Node );

		Material mtrl;
		mtrl.mName = modelInfo.mModelName;
		mtrl.mSurface = SURFACE_NONE;
		mtrl.mContents = CONT_PLYRCLIP;
		node->mModel = ParseModelData( modelInfo, mtrl );
		if ( node->mModel == NULL )
			return NodePtr();

		node->mModel->SetName( modelInfo.mModelName );
		mRootNode->mChildren.emplace_back( node );
	}
		
	node->mEntity = entity;
	node->mEntityName = entName;	
	node->mTransform.SetPosition( origin, false );
	node->mTransform.SetOrientation( Matrix3f( vFwd, -vRight, vUp, true ), false );
	node->mSaveable = entInfo.mFlags.CheckFlag( ENT_FLAG_SAVENAV );
	node->mRuntime = true;

	if ( entInfo.mCategory.CheckFlag( ENT_CAT_DYNAMIC_NAV ) )
		node->mDynamic = true;

	if ( entInfo.mCategory.CheckFlag( ENT_CAT_PROP_PUSHABLE ) )
	{
		node->mShapeMode = RecastIO::SHAPE_OBB;
		node->mSolid = false;
	}

	return node;
}

NodePtr CollisionWorld::CreateNodeForEntityBounds( const GameEntity entity, const GameModelInfo & modelInfo, const EntityInfo & entInfo )
{
	Vector3f origin = Vector3f::ZERO;
	if ( !EngineFuncs::EntityPosition( entity, origin ) )
		return NodePtr();

	Vector3f vFwd, vRight, vUp;
	if ( !EngineFuncs::EntityOrientation( entity, vFwd, vRight, vUp ) )
		return NodePtr();

	const std::string entName = EngineFuncs::EntityName( entity, "" );

	NodePtr node;
	mRootNode->FindNodeWithName( entName, node );
	if ( !node )
	{
		node.reset( new Node );

		Material mtrl;
		mtrl.mName = modelInfo.mModelName;
		mtrl.mSurface = SURFACE_NONE;
		mtrl.mContents = CONT_PLYRCLIP;
		node->mModel = CreateModelData( modelInfo.mAABB, mtrl );
		if ( node->mModel == NULL )
			return NodePtr();

		node->mModel->SetName( modelInfo.mModelName );
		mRootNode->mChildren.emplace_back( node );
	}

	node->mEntity = entity;
	node->mEntityName = entName;
	node->mTransform.SetPosition( origin, false );
	node->mTransform.SetOrientation( Matrix3f( vFwd, -vRight, vUp, true ), false );
	node->mSaveable = entInfo.mFlags.CheckFlag( ENT_FLAG_SAVENAV );
	node->mRuntime = true;
	
	if ( entInfo.mCategory.CheckFlag( ENT_CAT_DYNAMIC_NAV ) )
		node->mDynamic = true;

	if ( entInfo.mCategory.CheckFlag( ENT_CAT_PROP_PUSHABLE ) )
	{
		node->mShapeMode = RecastIO::SHAPE_OBB;
		node->mSolid = false;
	}

	return node;
}

ModelPtr CollisionWorld::CreateModelData( const AABB & localaabb, const Material & defaultMaterial )
{
	const Vector3f extAxis0 = localaabb.GetAxisLength( 0 ) * 0.5f * Vector3f::UNIT_X;
	const Vector3f extAxis1 = localaabb.GetAxisLength( 1 ) * 0.5f * Vector3f::UNIT_Y;
	const Vector3f extAxis2 = localaabb.GetAxisLength( 2 ) * 0.5f * Vector3f::UNIT_Z;

	Vector3f center;
	localaabb.CenterPoint( center );

	Vector3f vertex[ 8 ];
	vertex[ 0 ] = center - extAxis0 - extAxis1 - extAxis2;
	vertex[ 1 ] = center + extAxis0 - extAxis1 - extAxis2;
	vertex[ 2 ] = center + extAxis0 + extAxis1 - extAxis2;
	vertex[ 3 ] = center - extAxis0 + extAxis1 - extAxis2;
	vertex[ 4 ] = center - extAxis0 - extAxis1 + extAxis2;
	vertex[ 5 ] = center + extAxis0 - extAxis1 + extAxis2;
	vertex[ 6 ] = center + extAxis0 + extAxis1 + extAxis2;
	vertex[ 7 ] = center - extAxis0 + extAxis1 + extAxis2;/**/

	static const int indices[ 36 ] =
	{
		// bottom
		0, 1, 2,
		0, 2, 3,

		0, 3, 4,
		3, 4, 7,

		0, 1, 4,
		1, 4, 5,

		1, 2, 5,
		2, 5, 6,

		2, 3, 6,
		3, 6, 7,

		// top
		4, 5, 6,
		4, 6, 7,
	};

	ModelPtr mdl( new CollisionModel() );

	for ( int i = 0; i < 36; i += 3 )
	{
		mdl->AddTri(
			vertex[ indices[ i + 0 ] ],
			vertex[ indices[ i + 1 ] ],
			vertex[ indices[ i + 2 ] ],
			defaultMaterial );
	}

	mdl->Build( false );
	return mdl;
}

ModelPtr CollisionWorld::ParseModelData( const GameModelInfo & modelInfo, const Material & defaultMaterial )
{
	ModelPtr mdl;

	if ( !stricmp( modelInfo.mModelType, "obj" ) )
	{
		// load it pre-parsed from the cache if we can
		CollisionWorld::ModelCache::iterator it = mCachedModels.find( modelInfo.mModelName );
		if ( it != mCachedModels.end() && it->second->IsValid() )
			return it->second;

		// Otherwise build it from scratch
		mdl.reset( new CollisionModel );
		
		tinyobj::NullMtrlReader mtrlReader;

		std::istringstream istr( modelInfo.mDataBuffer );

		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		const std::string err = tinyobj::LoadObj( shapes, materials, istr, mtrlReader );
		if ( err.empty() )
		{
			Material objMtrl;
			objMtrl.mName = modelInfo.mModelName;
			objMtrl.mContents = CONT_SOLID;

			for ( size_t s = 0; s < shapes.size(); ++s )
			{
				const tinyobj::shape_t& shape = shapes[ s ];

				for ( size_t i = 2; i < shape.mesh.indices.size(); i += 3 )
				{
					const unsigned int i0 = shape.mesh.indices[ i - 2 ];
					const unsigned int i1 = shape.mesh.indices[ i - 1 ];
					const unsigned int i2 = shape.mesh.indices[ i - 0 ];

					const Vector3f v0(
						shape.mesh.positions[ 3 * i0 + 0 ],
						shape.mesh.positions[ 3 * i0 + 1 ],
						shape.mesh.positions[ 3 * i0 + 2 ] );
					const Vector3f v1(
						shape.mesh.positions[ 3 * i1 + 0 ],
						shape.mesh.positions[ 3 * i1 + 1 ],
						shape.mesh.positions[ 3 * i1 + 2 ] );
					const Vector3f v2(
						shape.mesh.positions[ 3 * i2 + 0 ],
						shape.mesh.positions[ 3 * i2 + 1 ],
						shape.mesh.positions[ 3 * i2 + 2 ] );

					mdl->AddTri( v0, v1, v2, objMtrl );
				}
			}

			mdl->Build( false );

			mCachedModels[ modelInfo.mModelName ] = mdl;
		}
	}
	return mdl;
}

void CollisionWorld::DeleteNodeForEntity( GameEntity entity )
{
	DeleteNodeForEntity_r( entity, mRootNode );
}

void CollisionWorld::DeleteNodeForEntity_r( GameEntity entity, NodePtr & node )
{
	for ( size_t i = 0; i < node->mChildren.size(); ++i )
	{
		if ( node->mChildren[ i ]->mEntity == entity )
		{
			node->mChildren[ i ]->mActiveState = StateMarkedForDelete;
		}
		else
		{
			DeleteNodeForEntity_r( entity, node->mChildren[ i ] );
		}
	}
}

void CollisionWorld::BuildScene( modeldata::Scene & scene )
{
	for ( ModelCache::iterator it = mCachedModels.begin(); it != mCachedModels.end(); ++it )
	{
		const size_t numTris = it->second->GetNumTris();
		if ( numTris == 0 )
			continue;

		std::vector<Vector3f> vertices;
		std::vector<IceMaths::IndexedTriangle> triangles;
		std::vector<uint16_t> materialIndices;

		vertices.reserve( numTris * 3 );
		triangles.reserve( numTris );
		materialIndices.reserve( numTris );
		
		for ( size_t t = 0; t < numTris; ++t )
		{
			Vector3f v[ 3 ];
			ContentFlags contents;
			SurfaceFlags surface;
			size_t materialIndex = 0;
			it->second->GetTriangle( t, ModelTransform(), v, contents, surface, materialIndex );

			triangles.push_back( IceMaths::IndexedTriangle( vertices.size() + 0, vertices.size() + 1, vertices.size() + 2 ) );
			vertices.push_back( v[ 0 ] );
			vertices.push_back( v[ 1 ] );
			vertices.push_back( v[ 2 ] );
			materialIndices.push_back( (uint16_t)materialIndex );
		}

		modeldata::Mesh * ioMesh = scene.add_meshes();
		ioMesh->set_name( it->first );

		for ( size_t m = 0; m < it->second->GetNumMaterials(); ++m )
		{
			modeldata::Material * ioMtrl = ioMesh->add_materials();

			const Material& mtrl = it->second->GetMaterial( m );
			ioMtrl->set_name( mtrl.mName.c_str() );
			ioMtrl->set_contents( mtrl.mContents );
			ioMtrl->set_surfaceflags( mtrl.mSurface );
		}

		ioMesh->mutable_vertices()->insert( 0, (const char*)&vertices[ 0 ], sizeof( Vector3f ) * vertices.size() );
		ioMesh->mutable_faces()->insert( 0, (const char*)&triangles[ 0 ], sizeof( IceMaths::IndexedTriangle ) * triangles.size() );
		ioMesh->mutable_materialindices()->insert( 0, (const char*)&materialIndices[ 0 ], sizeof( uint16_t ) * materialIndices.size() );
	}
	
	if ( mRootNode )
		mRootNode->BuildScene( scene, *scene.mutable_rootnode() );	
}
