#include "CollisionModel.h"
#include "IGame.h"
#include "RenderBuffer.h"

static const Material sNullMaterial;

Vector3f Convert( const IceMaths::Point & pt )
{
	return Vector3f( pt.x, pt.y, pt.z );
}

IceMaths::Point Convert( const Vector3f & pt )
{
	return IceMaths::Point( pt.X(), pt.Y(), pt.Z() );
}

const ModelTransform ModelTransform::sIdentity;

void ModelTransform::UpdateStableTime()
{
	mStableTime += IGame::GetDeltaTimeSecs();
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
	mMaterialIndices.clear();
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
		mMaterialIndices.push_back( materialIndex );

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

	mMaterialIndices.push_back( materialIndex );
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

		mVerts.push_back( IceMaths::Point( -10000.0f, -10000.0f, -10000 ) );
		mVerts.push_back( IceMaths::Point( -10000.0f,  10000.0f, -10000 ) );
		mVerts.push_back( IceMaths::Point(  10000.0f,  10000.0f, -10000 ) );
		mMaterialIndices.push_back( 0 );

		mVerts.push_back( IceMaths::Point( -10000.0f, -10000.0f, -10000 ) );
		mVerts.push_back( IceMaths::Point(  10000.0f,  10000.0f, -10000 ) );
		mVerts.push_back( IceMaths::Point( 10000.0f,  -10000.0f, -10000 ) );
		mMaterialIndices.push_back( 0 );
	}

	mLocalBounds.Clear();

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

			mLocalBounds.ExpandPt( Convert( *vtx.Vertex[ 0 ] ) );
			mLocalBounds.ExpandPt( Convert( *vtx.Vertex[ 1 ] ) );
			mLocalBounds.ExpandPt( Convert( *vtx.Vertex[ 2 ] ) );
		}
		return true;
	}

	Free();
	return false;
}

bool CollisionModel::CollideRay( const ModelTransform & mdlXform, RayResult & result, const Vector3f & from, const Vector3f & to, CacheId * lastHit )
{
	if ( mCollisionTree != NULL )
	{
		Vector3f dir = to - from;
		const float distance = dir.Normalize();

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
		collider.SetMaxDist( distance );
		collider.SetClosestHit( true );
		collider.Collide( ray, *mCollisionTree, &xform, lastHit );

		const Opcode::CollisionFace * hitFace = faces.GetFaces();
		if ( hitFace != NULL )
		{
			Opcode::VertexPointers v;
			mCollisionTree->GetMeshInterface()->GetTriangle( v, hitFace->mFaceID );

			IceMaths::Point normal = v.Normal();

			result.mHitPos = from + dir * hitFace->mDistance;
			result.mHitNormal = Vector3f( normal.x, normal.y, normal.z );
			result.mHitMaterialIndex = mMaterialIndices[ hitFace->mFaceID ];
			result.mHitTriangle = hitFace->mFaceID;
			return true;
		}
	}
	return false;
}

bool CollisionModel::CollideAABB( const ModelTransform & mdlXform, const AABB & worldaabb, MaterialIndices & hitMaterials )
{
	bool hit = false;
	if ( mCollisionTree != NULL )
	{
		Opcode::CollisionAABB colaabb;
		colaabb.SetMinMax(
			IceMaths::Point( worldaabb.m_Mins ),
			IceMaths::Point( worldaabb.m_Maxs ) );

		Opcode::AABBCollider collider;
		collider.SetFirstContact( false );

		Opcode::AABBCache boxCache;
		collider.Collide( boxCache, colaabb, *mCollisionTree );

		for ( udword i = 0; i < collider.GetNbTouchedPrimitives(); ++i )
		{
			const udword primId = collider.GetTouchedPrimitives()[ i ];
			hitMaterials.push_back( mMaterialIndices[ primId ] );
			hit = true;
		}
	}
	return hit;
}
bool CollisionModel::CollideOBB( const ModelTransform & mdlXform, const Box3f & obb, MaterialIndices & hitMaterials )
{
	bool hit = false;
	if ( mCollisionTree != NULL )
	{
		IceMaths::Matrix4x4 xform;
		xform.SetRow( 0, Convert( mdlXform.Orient().GetColumn( 0 ) ) );
		xform.SetRow( 1, Convert( mdlXform.Orient().GetColumn( 1 ) ) );
		xform.SetRow( 2, Convert( mdlXform.Orient().GetColumn( 2 ) ) );
		xform.SetRow( 3, Convert( mdlXform.Pos() ) );

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
		collider.Collide( cache, iceobb, *mCollisionTree, &xform );

		for ( udword i = 0; i < collider.GetNbTouchedPrimitives(); ++i )
		{
			const udword primId = collider.GetTouchedPrimitives()[ i ];
			hitMaterials.push_back( mMaterialIndices[ primId ] );
			hit = true;
		}
	}
	return hit;
}

bool CollisionModel::CollideAABB( const ModelTransform & mdlXform, const AABB & aabb, MaterialSet & hitAttribs )
{
	bool hit = false;
	if ( mCollisionTree != NULL )
	{
		Opcode::CollisionAABB colaabb;
		colaabb.SetMinMax(
			IceMaths::Point( aabb.m_Mins ),
			IceMaths::Point( aabb.m_Maxs ) );

		Opcode::AABBCollider collider;
		collider.SetFirstContact( false );

		Opcode::AABBCache boxCache;
		collider.Collide( boxCache, colaabb, *mCollisionTree );

		for ( udword i = 0; i < collider.GetNbTouchedPrimitives(); ++i )
		{
			const udword primId = collider.GetTouchedPrimitives()[ i ];			
			hitAttribs.insert( mMaterialIndices[ primId ] );
			hit = true;
		}
	}
	return hit;
}
bool CollisionModel::CollideOBB( const ModelTransform & mdlXform, const Box3f & obb, MaterialSet & hitAttribs )
{
	bool hit = false;
	if ( mCollisionTree != NULL )
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
		collider.Collide( cache, iceobb, *mCollisionTree );

		for ( udword i = 0; i < collider.GetNbTouchedPrimitives(); ++i )
		{
			const udword primId = collider.GetTouchedPrimitives()[ i ];
			hitAttribs.insert( mMaterialIndices[ primId ] );
			hit = true;
		}
	}
	return hit;
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

	RenderBuffer::AddLine( mdlXform.Pos(), mdlXform.Pos() + fwd * 128.0f, COLOR::GREEN );
	RenderBuffer::AddLine( mdlXform.Pos(), mdlXform.Pos() + right * 128.0f, COLOR::BLUE );
	RenderBuffer::AddLine( mdlXform.Pos(), mdlXform.Pos() + up * 128.0f, COLOR::RED );
}

void CollisionModel::Render( const ModelTransform & mdlXform, const Vector3f & offset, const obColor & polyColor )
{
	for ( udword i = 0; i < mMeshInterface->GetNbTriangles(); ++i )
	{
		Vector3f v0, v1, v2;
		size_t materialIndex;
		GetTriangle( mdlXform, i, v0, v1, v2, materialIndex );
				
		RenderBuffer::AddTri(
			v0 + offset,
			v1 + offset,
			v2 + offset,
			polyColor );
		RenderBuffer::AddLine(
			v0 + offset,
			v1 + offset,
			COLOR::BLUE );
		RenderBuffer::AddLine(
			v1 + offset,
			v2 + offset,
			COLOR::BLUE );
		RenderBuffer::AddLine(
			v0 + offset,
			v2 + offset,
			COLOR::BLUE );
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

void CollisionModel::GetTriangle( const ModelTransform & mdlXform, size_t triIndex, Vector3f & v0, Vector3f & v1, Vector3f & v2, size_t & materialIndex ) const
{
	Opcode::VertexPointers vtx;
	mMeshInterface->GetTriangle( vtx, triIndex );

	// convert it to world space
	v0 = mdlXform.Pos() + mdlXform.Orient() * Convert( *vtx.Vertex[ 0 ] );
	v1 = mdlXform.Pos() + mdlXform.Orient() * Convert( *vtx.Vertex[ 1 ] );
	v2 = mdlXform.Pos() + mdlXform.Orient() * Convert( *vtx.Vertex[ 2 ] );
	materialIndex = mMaterialIndices[ triIndex ];
}

void CollisionModel::GetTriangles( const ModelTransform & mdlXform, RenderBuffer::TriList & triangles, MaterialIndices & triangleAttribs ) const
{
	triangles.resize( mMeshInterface->GetNbTriangles() );
	triangleAttribs.resize( mMeshInterface->GetNbTriangles() );

	for ( size_t i = 0; i < mMeshInterface->GetNbTriangles(); ++i )
	{
		GetTriangle( mdlXform, i, triangles[ i ].v[ 0 ], triangles[ i ].v[ 1 ], triangles[ i ].v[ 2 ], triangleAttribs[ i ] );
		triangles[ i ].c = COLOR::GREEN;
	}
}

bool CollisionModel::IsValid() const
{
	return mCollisionTree != NULL;
}
