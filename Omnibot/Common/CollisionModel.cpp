#include "CollisionModel.h"
#include "RenderBuffer.h"

Vector3f Convert( const IceMaths::Point & pt )
{
	return Vector3f( pt.x, pt.y, pt.z );
}

CollisionModel::CollisionModel()
	: mCollisionTree( NULL )
	, mMeshInterface( NULL )
{
	mTransform.Identity();
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
	mAttribs.clear();
}

void CollisionModel::Add( const Vector3List & poly, void * attrib )
{
	if ( poly.size() == 0 )
		return;

	// generate triangles for the polygon
	for ( size_t v = 2; v < poly.size(); ++v )
	{
		mAttribs.push_back( attrib );

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

bool CollisionModel::Build( bool createDefaultIfEmpty )
{
	if ( mVerts.size() == 0 )
	{
		mVerts.push_back( IceMaths::Point( -10000.0f, -10000.0f, -10000 ) );
		mVerts.push_back( IceMaths::Point( -10000.0f,  10000.0f, -10000 ) );
		mVerts.push_back( IceMaths::Point(  10000.0f,  10000.0f, -10000 ) );
		mAttribs.push_back( 0 );

		mVerts.push_back( IceMaths::Point( -10000.0f, -10000.0f, -10000 ) );
		mVerts.push_back( IceMaths::Point(  10000.0f,  10000.0f, -10000 ) );
		mVerts.push_back( IceMaths::Point( 10000.0f,  -10000.0f, -10000 ) );
		mAttribs.push_back( 0 );
	}

	mMeshInterface = new Opcode::MeshInterface();
	mMeshInterface->SetNbTriangles( mVerts.size() / 3 );
	mMeshInterface->SetNbVertices( mVerts.size() );
	mMeshInterface->SetPointers( NULL, &mVerts[ 0 ] );
	mMeshInterface->SetInterfaceType( Opcode::MESH_TRIANGLE );

	Opcode::OPCODECREATE parms;
	parms.mIMesh = mMeshInterface;
	mCollisionTree = new Opcode::Model();
	if ( mCollisionTree->Build( parms ) )
		return true;

	Free();
	return false;
}

bool CollisionModel::CollideRay( const Vector3f & from, const Vector3f & to, Vector3f & hitPos, Vector3f & hitNormal, void *& hitAttrib )
{
	if ( mCollisionTree != NULL )
	{
		Vector3f dir = to - from;
		const float distance = dir.Normalize();

		IceMaths::Ray ray;
		ray.mOrig.Set( from.X(), from.Y(), from.Z() );
		ray.mDir.Set( dir.X(), dir.Y(), dir.Z() );

		Opcode::CollisionFaces faces;
		Opcode::RayCollider collider;
		collider.SetCulling( false );
		collider.SetDestination( &faces );
		collider.SetMaxDist( distance );
		collider.SetClosestHit( true );
		collider.Collide( ray, *mCollisionTree, &mTransform );

		const Opcode::CollisionFace * hitFace = faces.GetFaces();
		if ( hitFace != NULL )
		{
			Opcode::VertexPointers v;
			mCollisionTree->GetMeshInterface()->GetTriangle( v, hitFace->mFaceID );

			IceMaths::Point normal = v.Normal();

			hitPos = from + dir * hitFace->mDistance;
			hitNormal = Vector3f( normal.x, normal.y, normal.z );
			hitAttrib = mAttribs[ hitFace->mFaceID ];
			return true;
		}
	}
	return false;
}

bool CollisionModel::CollideAABB( const AABB & worldaabb, AttribList & hitAttribs )
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
			hitAttribs.push_back( mAttribs[ primId ] );
			hit = true;
		}
	}
	return hit;
}
bool CollisionModel::CollideOBB( const Box3f & obb, AttribList & hitAttribs )
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
		collider.Collide( cache, iceobb, *mCollisionTree, &mTransform );

		for ( udword i = 0; i < collider.GetNbTouchedPrimitives(); ++i )
		{
			const udword primId = collider.GetTouchedPrimitives()[ i ];
			hitAttribs.push_back( mAttribs[ primId ] );

			hit = true;
		}
	}
	return hit;
}

bool CollisionModel::CollideAABB( const AABB & aabb, AttribSet & hitAttribs )
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
			hitAttribs.insert( mAttribs[ primId ] );
			hit = true;
		}
	}
	return hit;
}
bool CollisionModel::CollideOBB( const Box3f & obb, AttribSet & hitAttribs )
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
			hitAttribs.insert( mAttribs[ primId ] );
			hit = true;
		}
	}
	return hit;
}
void CollisionModel::Render( const Vector3f & offset )
{
	for ( udword i = 0; i < mMeshInterface->GetNbTriangles(); ++i )
	{
		Opcode::VertexPointers vtx;
		mMeshInterface->GetTriangle( vtx, i );

		RenderBuffer::AddTri(
			Convert( *vtx.Vertex[ 0 ] ) + offset,
			Convert( *vtx.Vertex[ 1 ] ) + offset,
			Convert( *vtx.Vertex[ 2 ] ) + offset,
			COLOR::LIGHT_GREY.fade( 100 ) );
		RenderBuffer::AddLine(
			Convert( *vtx.Vertex[ 0 ] ) + offset,
			Convert( *vtx.Vertex[ 1 ] ) + offset,
			COLOR::BLUE );
		RenderBuffer::AddLine(
			Convert( *vtx.Vertex[ 1 ] ) + offset,
			Convert( *vtx.Vertex[ 2 ] ) + offset,
			COLOR::BLUE );
		RenderBuffer::AddLine(
			Convert( *vtx.Vertex[ 0 ] ) + offset,
			Convert( *vtx.Vertex[ 2 ] ) + offset,
			COLOR::BLUE );
	}
}
void CollisionModel::RenderSkipSet( const AttribSet & skip, const Vector3f & offset )
{
	for ( udword i = 0; i < mMeshInterface->GetNbTriangles(); ++i )
	{
		if ( skip.find( mAttribs[ i ] ) != skip.end() )
			continue;

		Opcode::VertexPointers vtx;
		mMeshInterface->GetTriangle( vtx, i );

		RenderBuffer::AddTri(
			Convert( *vtx.Vertex[ 0 ] ) + offset,
			Convert( *vtx.Vertex[ 1 ] ) + offset,
			Convert( *vtx.Vertex[ 2 ] ) + offset,
			COLOR::LIGHT_GREY.fade( 100 ) );
		RenderBuffer::AddLine(
			Convert( *vtx.Vertex[ 0 ] ) + offset,
			Convert( *vtx.Vertex[ 1 ] ) + offset,
			COLOR::BLUE );
		RenderBuffer::AddLine(
			Convert( *vtx.Vertex[ 1 ] ) + offset,
			Convert( *vtx.Vertex[ 2 ] ) + offset,
			COLOR::BLUE );
		RenderBuffer::AddLine(
			Convert( *vtx.Vertex[ 0 ] ) + offset,
			Convert( *vtx.Vertex[ 2 ] ) + offset,
			COLOR::BLUE );
	}
}