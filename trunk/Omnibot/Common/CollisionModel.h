#pragma once
#ifndef COLLISIONMODEL_H
#define COLLISIONMODEL_H

#include <vector>
#include <set>

#include "common.h"

#include "RenderBuffer.h"

#include "Opcode.h"
#include "OPC_IceHook.h"

struct ModelTransform
{
	const Vector3f Pos() const
	{
		return mPos;
	}
	const Matrix3f & Orient() const
	{
		return mOrient;
	}

	void SetPosition( const Vector3f & pos, bool checkStable );
	void SetOrientation( const Matrix3f & orient, bool checkStable );
	void SetTransform( const IceMaths::Matrix4x4 & xform, bool checkStable );

	void UpdateStableTime();
	float GetStableTime() const { return mStableTime; }
	
	static const ModelTransform sIdentity;

	ModelTransform()
		: mPos( Vector3f::ZERO )
		, mOrient( Matrix3f::IDENTITY )
		, mStableTime( std::numeric_limits<float>::max() )
	{
	}
private:
	Vector3f			mPos;
	Matrix3f			mOrient;
	float				mStableTime;
};

struct Material
{
	std::string		mName;
	SurfaceFlags	mSurface;
	ContentFlags	mContents;	
	void *			mUser;

	bool operator==( const Material & other ) const
	{
		return mName == other.mName &&
			mSurface == other.mSurface &&
			mContents == other.mContents &&
			mUser == other.mUser;
	}

	Material() 
		: mSurface( SURFACE_NONE )
		, mContents( CONT_NONE )
		, mUser( NULL )
	{
	}
};

class CollisionModel
{
public:
	CollisionModel();
	~CollisionModel();

	typedef unsigned int CacheId;

	void SetName( const std::string & name )
	{
		mName = name;
	}
	const std::string & GetName()
	{
		return mName;
	}

	void Add( const Vector3List & poly, const Material & material );
	void AddTri( const Vector3f & v0, const Vector3f & v1, const Vector3f & v2, const Material & material );

	bool Build( bool createDefaultIfEmpty );

	struct RayResult
	{
		Vector3f	mHitPos;
		Vector3f	mHitNormal;
		size_t		mHitMaterialIndex;
		size_t		mHitTriangle;
	};
	bool CollideRay( const ModelTransform & mdlXform, RayResult & result, const Vector3f & from, const Vector3f & to, CacheId * lastHit = NULL );

	typedef std::vector<size_t> MaterialIndices;
	typedef std::set<size_t> MaterialSet;
	
	const Material & GetMaterial( size_t materialIndex ) const;

	// all attribs, even duplicates
	bool CollideAABB( const ModelTransform & mdlXform, const AABB & worldaabb, MaterialIndices & hitMaterials );
	bool CollideOBB( const ModelTransform & mdlXform, const Box3f & obb, MaterialIndices & hitMaterial );
	
	// get only unique attribs
	bool CollideAABB( const ModelTransform & mdlXform, const AABB & aabb, MaterialSet & hitAttribs );
	bool CollideOBB( const ModelTransform & mdlXform, const Box3f & obb, MaterialSet & hitAttribs );

	void Free();

	size_t NumTriangles() const { return mVerts.size() / 3; }

	void RenderAxis( const ModelTransform & mdlXform );
	void Render( const ModelTransform & mdlXform, const Vector3f & offset, const obColor & polyColor = COLOR::LIGHT_GREY.fade( 100 ) );
		
	size_t GetNumVerts() const;
	size_t GetNumTris() const;
	
	void GetTriangle( const ModelTransform & mdlXform, size_t triIndex, Vector3f & v0, Vector3f & v1, Vector3f & v2, size_t & materialIndex ) const;
	void GetTriangles( const ModelTransform & mdlXform, RenderBuffer::TriList & triangles, MaterialIndices & materialIndices ) const;
	
	bool IsValid() const;

	const AxisAlignedBox3f & GetLocalAABB() const;
	Box3f GetWorldOBB( const ModelTransform & mdlXform ) const;
private:
	Opcode::Model *					mCollisionTree;
	Opcode::MeshInterface *			mMeshInterface;
	
	AxisAlignedBox3f				mLocalBounds;

	std::string						mName;

	std::vector<IceMaths::Point>	mVerts;
	std::vector<size_t>				mMaterialIndices;
	std::vector<Material>			mMaterials;

	size_t FindOrAddMaterial( const Material & material );
};

typedef boost::shared_ptr<CollisionModel> ModelPtr;

#endif