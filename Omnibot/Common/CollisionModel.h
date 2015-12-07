#pragma once
#ifndef COLLISIONMODEL_H
#define COLLISIONMODEL_H

#include <vector>
#include <set>

#include "common.h"

#include "RenderBuffer.h"

#include "Opcode.h"
#include "OPC_IceHook.h"

#include "PathPlannerBase.h"
#include "recast.pb.h"
#include "modeldata.pb.h"

#pragma warning( push )
#pragma warning( disable: 4245 4244 )
#include <boost/crc.hpp>
#pragma warning( pop )

typedef EnumerationValues<ContentFlags> ContentFlagsEnum;
typedef EnumerationValues<SurfaceFlags> SurfaceFlagsEnum;

struct Node;
typedef std::shared_ptr<Node> NodePtr;

class PathPlannerRecast;

struct ModelTransform
{
	const Vector3f Pos() const;
	const Matrix3f & Orient() const;

	void SetPosition( const Vector3f & pos, bool checkStable );
	void SetOrientation( const Matrix3f & orient, bool checkStable );
	void SetTransform( const IceMaths::Matrix4x4 & xform, bool checkStable );

	void UpdateStableTime();
	float GetStableTime() const { return mStableTime; }
	
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
	unsigned int	mNumReferences;

	bool operator==( const Material & other ) const
	{
		return mName == other.mName &&
			mSurface == other.mSurface &&
			mContents == other.mContents;
	}

	Material() 
		: mSurface( SURFACE_NONE )
		, mContents( CONT_NONE )
		, mNumReferences( 0 )
	{
	}
};

struct GatherParms
{
	GameEntity				mEntity;
	RecastIO::ShapeMode		mMode;
	int						mIgnoreSurfaces;

	GatherParms()
		: mMode( RecastIO::SHAPE_TRIANGLES )
		, mIgnoreSurfaces( 0 )
	{
	}
};

struct CollisionTriangle
{
	Triangle3f		mTri;
	ContentFlags	mContents;
	SurfaceFlags	mSurface;
	NavAreaFlags	mNavFlags;
	GameEntity		mEntity;
};

struct CollisionConvex
{
	size_t			mVertStart;
	size_t			mVertEnd;
	ContentFlags	mContents;
	SurfaceFlags	mSurface;
	NavAreaFlags	mNavFlags;
	float			mHeightMin;
	float			mHeightMax;
	GameEntity		mEntity;
};

struct GatherData
{
	std::vector<CollisionTriangle>	mTriangles;
	std::vector<Vector3f>			mConvexVertices;
	std::vector<CollisionConvex>	mConvexShapes;
};

struct RayResult
{
	Vector3f		mHitPos;
	Vector3f		mHitNormal;
	float			mHitDistance;
	size_t			mHitTriangle;

	NodePtr			mHitNode;
	int				mModelIndex;

	RayResult() 
		: mHitDistance( Mathf::MAX_REAL )
		, mModelIndex( -1 )
	{
	}
};

struct TriParms
{
	size_t			mMaterialIndex;
	SurfaceFlags 	mSurfaceOverride;

	TriParms()
		: mMaterialIndex( 0 )
		, mSurfaceOverride( SURFACE_NONE )
	{
	}
};

typedef std::vector<size_t> Indices;

class CollisionModel
{
public:
	CollisionModel();
	~CollisionModel();
	
	void SetName( const std::string & name )
	{
		mName = name;
	}
	const std::string & GetName()
	{
		return mName;
	}

	void SaveState( RecastIO::NavigationMesh& ioNavmesh );
	void LoadState( const RecastIO::Model& ioModel );

	void Add( const Vector3List & poly, const Material & material );
	void AddTri( const Vector3f & v0, const Vector3f & v1, const Vector3f & v2, const Material & material );

	bool Build( bool createDefaultIfEmpty );
		
	bool CollideRay( RayResult & result, const ModelTransform & mdlXform, const Vector3f & from, const Vector3f & dir, float maxDistance, SurfaceFlags ignoreSurfaces );
		
	const Material & GetMaterial( size_t materialIndex ) const;

	// all attribs, even duplicates
	bool CollideOBB( const ModelTransform & mdlXform, const Box3f & obb, Indices & hitTriangles );
	
	void GatherTriangles( const GatherParms & parms, const ModelTransform & mdlXform, const Box3f & gatherObb, 
		const CollisionTriangle& baseTriangle, GatherData& dataOut );

	void GatherTriangles( const GatherParms & parms, const ModelTransform & mdlXform, const Sphere3f & gatherSphere,
		const CollisionTriangle& baseTriangle, GatherData& dataOut );

	void Free();

	size_t NumTriangles() const { return mVerts.size() / 3; }

	void RenderAxis( const ModelTransform & mdlXform );
	void Render( const ModelTransform & mdlXform, const Vector3f & offset, const obColor & polyColor );
	void RenderInRadius( const Vector3f & eye, float radius, const ModelTransform & mdlXform, const Vector3f & offset, const obColor & polyColor );
		
	size_t GetNumVerts() const;
	size_t GetNumTris() const;
	size_t GetNumMaterials() const;
	
	void GetTriangle( size_t triIndex, const ModelTransform & mdlXform, Vector3f v[3], ContentFlags& contents, SurfaceFlags& surface, size_t& mtrlIndex ) const;
	
	void SetSurfaceOverride( size_t triIndex, SurfaceFlags surfaceFlags );

	bool IsValid() const;
	
	const AxisAlignedBox3f & GetLocalAABB() const;
	Box3f GetWorldOBB( const ModelTransform & mdlXform ) const;

	boost::crc_32_type::value_type GetModelCrc() const;

	void UpdateModelCrc();
private:
	Opcode::Model *					mCollisionTree;
	Opcode::MeshInterface *			mMeshInterface;
	
	AxisAlignedBox3f				mLocalBounds;
	ContentFlags					mBoundsContents;
	SurfaceFlags					mBoundsSurface;

	std::string						mName;

	std::vector<IceMaths::Point>	mVerts;
	std::vector<TriParms>			mTriParms;
	std::vector<Material>			mMaterials;	
	
	size_t FindOrAddMaterial( const Material & material );

	boost::crc_32_type::value_type	mModelCrc;
};

typedef std::shared_ptr<CollisionModel> ModelPtr;

enum ModelState
{
	StateUnknown,
	StateNonCollidable,
	StateCollidable,
	StateMoved,
	StateMarkedForDelete,
};

typedef EnumerationValues<ModelState> ModelStateEnum;

struct Node : public std::enable_shared_from_this<Node>
{
	GameEntity						mEntity;
	EntityInfo						mEntInfo;
	int								mSubModel;
	int								mStaticModel;
	int								mDisplacement;
	std::string						mEntityName;
	ModelState						mActiveState;
	NavAreaFlags					mNavFlagsActive;
	NavAreaFlags					mNavFlagsOverride;
	boost::crc_32_type::value_type	mActiveModelCrc;

	ModelTransform					mTransform;
	std::vector<NodePtr>			mChildren;
	ModelPtr						mModel;

	RecastIO::ShapeMode				mShapeMode;
	bool							mEnabledDefault : 1;
	bool							mEnabled : 1;
	bool							mSolid : 1;
	bool							mDynamic : 1;
	bool							mForceRebuild : 1;
	bool							mSaveable : 1; // save to file
	bool							mRuntime : 1; // save to file

	void Init( PathPlannerRecast * planner );
	void UpdateModelState( PathPlannerRecast * planner, bool forcePositionUpdate );

	void SaveState( RecastIO::NavigationMesh & ioNavmesh );
	void LoadState( const RecastIO::NodeState & nodeState );

	void Clear();
	
	bool CollideSegmentNearest( RayResult & result, const Segment3f & segment, SurfaceFlags ignoreSurfaces, bool onlyCollidable );
		
	void GatherTriangles( const GatherParms & parms, const Box3f & gatherObb, GatherData& dataOut );
	void GatherTriangles( const GatherParms & parms, const Sphere3f & gatherSphere, GatherData& dataOut );

	void RenderModel( const obColor & polyColor );
	void RenderInRadius( const Vector3f & eye, float distance, const obColor & polyColor );

	void RenderAxis();
	void RenderWorldBounds();
	
	void UpdateWorldBounds( AxisAlignedBox3f & worldBounds );

	void SetModelShapeMode( const ModelPtr & mdl, RecastIO::ShapeMode mode );
	void SetModelEnable( const ModelPtr & mdl, bool en );
	void SetModelSolid( const ModelPtr & mdl, bool en );
	void SetModelDynamic( const ModelPtr & mdl, bool en );

	void FindNodeWithSubModel( int subModel, NodePtr & node );
	void FindNodeWithStaticModel( int staticModelId, NodePtr & node );
	void FindNodeWithDisplacement( int displacementId, NodePtr & node );
	void FindNodeWithEntity( GameEntity entity, NodePtr & node );
	void FindNodeWithName( const std::string& entName, NodePtr & node );
	
	void BuildScene( modeldata::Scene & scene, modeldata::Node & node );

	Node();
	virtual ~Node();
};

struct CollisionWorld
{
	NodePtr							mRootNode;

	// holds the authoritative model information for all models
	// referenced by other nodes in the graph
	typedef std::map<std::string, ModelPtr> ModelCache;
	ModelCache						mCachedModels;

	void Clear();

	AxisAlignedBox3f CalcWorldAABB() const;

	NodePtr LoadModelIntoWorld( const GameEntity entity, const GameModelInfo & modelInfo, const EntityInfo & entInfo );

	ModelPtr CreateModelData( const AABB & localaabb, const Material & defaultMaterial );
	ModelPtr ParseModelData( const GameModelInfo & modelInfo, const Material & defaultMaterial );
	
	void DeleteNodeForEntity( GameEntity entity );

	void BuildScene( modeldata::Scene & scene );

	CollisionWorld();
	virtual ~CollisionWorld();
private:
	NodePtr CreateNodeForEntityModel( const GameEntity entity, const GameModelInfo & modelInfo, const EntityInfo & entInfo );
	NodePtr CreateNodeForEntityBounds( const GameEntity entity, const GameModelInfo & modelInfo, const EntityInfo & entInfo );
	
	void DeleteNodeForEntity_r( GameEntity entity, NodePtr & node );
};

#endif