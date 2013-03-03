///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 *	OPCODE - Optimized Collision Detection
 *	Copyright (C) 2001 Pierre Terdiman
 *	Homepage: http://www.codercorner.com/Opcode.htm
 *
 *  OPCODE modifications for scaled model support (and other things)
 *  Copyright (C) 2004 Gilvan Maia (gilvan 'at' vdl.ufc.br)
 *	Check http://www.vdl.ufc.br/gilvan/coll/opcode/index.htm for updates.
 *
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code for a ray collider.
 *	\file		OPC_RayCollider.cpp
 *	\author		Pierre Terdiman
 *	\date		June, 2, 2001
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains a ray-vs-tree collider.
 *	This class performs a stabbing query on an AABB tree, i.e. does a ray-mesh collision.
 *
 *	HIGHER DISTANCE BOUND:
 *
 *		If P0 and P1 are two 3D points, let's define:
 *		- d = distance between P0 and P1
 *		- Origin	= P0
 *		- Direction	= (P1 - P0) / d = normalized direction vector
 *		- A parameter t such as a point P on the line (P0,P1) is P = Origin + t * Direction
 *		- t = 0  -->  P = P0
 *		- t = d  -->  P = P1
 *
 *		Then we can define a general "ray" as:
 *
 *			struct Ray
 *			{
 *				Point	Origin;
 *				Point	Direction;
 *			};
 *
 *		But it actually maps three different things:
 *		- a segment,   when 0 <= t <= d
 *		- a half-line, when 0 <= t < +infinity, or -infinity < t <= d
 *		- a line,      when -infinity < t < +infinity
 *
 *		In Opcode, we support segment queries, which yield half-line queries by setting d = +infinity.
 *		We don't support line-queries. If you need them, shift the origin along the ray by an appropriate margin.
 *
 *		In short, the lower bound is always 0, and you can setup the higher bound "d" with RayCollider::SetMaxDist().
 *
 *		Query	|segment			|half-line		|line
 *		--------|-------------------|---------------|----------------
 *		Usages	|-shadow feelers	|-raytracing	|-
 *				|-sweep tests		|-in/out tests	|
 *
 *	FIRST CONTACT:
 *
 *		- You can setup "first contact" mode or "all contacts" mode with RayCollider::SetFirstContact().
 *		- In "first contact" mode we return as soon as the ray hits one face. If can be useful e.g. for shadow feelers, where
 *		you want to know whether the path to the light is free or not (a boolean answer is enough).
 *		- In "all contacts" mode we return all faces hit by the ray.
 *
 *	TEMPORAL COHERENCE:
 *
 *		- You can enable or disable temporal coherence with RayCollider::SetTemporalCoherence().
 *		- It currently only works in "first contact" mode.
 *		- If temporal coherence is enabled, the previously hit triangle is cached during the first query. Then, next queries
 *		start by colliding the ray against the cached triangle. If they still collide, we return immediately.
 *
 *	CLOSEST HIT:
 *
 *		- You can enable or disable "closest hit" with RayCollider::SetClosestHit().
 *		- It currently only works in "all contacts" mode.
 *		- If closest hit is enabled, faces are sorted by distance on-the-fly and the closest one only is reported.
 *
 *	BACKFACE CULLING:
 *
 *		- You can enable or disable backface culling with RayCollider::SetCulling().
 *		- If culling is enabled, ray will not hit back faces (only front faces).
 *		
 *
 *
 *	\class		RayCollider
 *	\author		Pierre Terdiman
 *	\version	1.3
 *	\date		June, 2, 2001
*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	This class describes a face hit by a ray or segment.
 *	This is a particular class dedicated to stabbing queries.
 *
 *	\class		CollisionFace
 *	\author		Pierre Terdiman
 *	\version	1.3
 *	\date		March, 20, 2001
*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	This class is a dedicated collection of CollisionFace.
 *
 *	\class		CollisionFaces
 *	\author		Pierre Terdiman
 *	\version	1.3
 *	\date		March, 20, 2001
*/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Precompiled Header
#include "Stdafx.h"

using namespace Opcode;
using namespace IceMaths;

// When this macro is set, the overlap tests considers an scaling on AABBs/tris.
// This means that the ray/line is not entirely in the model's local space when collision
// tests take places.
// #define OPC_RAYCOLLIDER_SCALE_BEFORE_OVERLAP

#include "OPC_RayAABBOverlap.h"
#include "OPC_RayTriOverlap.h"

#define SET_CONTACT(prim_index, flag)											\
	mNbIntersections++;															\
	/* Set contact status */													\
	mFlags |= flag;																\
	/* In any case the contact has been found and recorded in mStabbedFace  */	\
	mStabbedFace.mFaceID = prim_index;

#ifdef OPC_RAYHIT_CALLBACK

	#define HANDLE_CONTACT(prim_index, flag)													\
		SET_CONTACT(prim_index, flag)															\
																								\
		if(mHitCallback)	(mHitCallback)(mStabbedFace, mUserData);

	#define UPDATE_CACHE					\
		if(cache && GetContactStatus())		\
		{									\
			*cache	= mStabbedFace.mFaceID;	\
		}
#else

	#define HANDLE_CONTACT(prim_index, flag)													\
		SET_CONTACT(prim_index, flag)															\
																								\
		/* Now we can also record it in mStabbedFaces if available */							\
		if(mStabbedFaces)																		\
		{																						\
			/* If we want all faces or if that's the first one we hit */						\
			if(!mClosestHit || !mStabbedFaces->GetNbFaces())									\
			{																					\
				mStabbedFaces->AddFace(mStabbedFace);											\
			}																					\
			else																				\
			{																					\
				/* We only keep closest hit */													\
				CollisionFace* Current = const_cast<CollisionFace*>(mStabbedFaces->GetFaces());	\
				if(Current && mStabbedFace.mDistance<Current->mDistance)						\
				{																				\
					*Current = mStabbedFace;													\
				}																				\
			}																					\
		}

	#define UPDATE_CACHE												\
		if(cache && GetContactStatus() && mStabbedFaces)				\
		{																\
			const CollisionFace* Current = mStabbedFaces->GetFaces();	\
			if(Current)	*cache	= Current->mFaceID;						\
			else		*cache	= INVALID_ID;							\
		}
#endif

#define SEGMENT_PRIM(prim_index, flag)														\
	/* Request vertices from the app */														\
	VertexPointers VP;	mIMesh->GetTriangle(VP, prim_index);								\
																							\
	/* Perform ray-tri overlap test and return */											\
	if(RayTriOverlap(*VP.Vertex[0], *VP.Vertex[1], *VP.Vertex[2]))							\
	{																						\
		/* Intersection point is valid if dist < segment's length */						\
		/* We know dist>0 so we can use integers */											\
		if(IR(mStabbedFace.mDistance)<IR(mMaxDist))											\
		{																					\
			HANDLE_CONTACT(prim_index, flag)												\
		}																					\
	}

#define RAY_PRIM(prim_index, flag)															\
	/* Request vertices from the app */														\
	VertexPointers VP;	mIMesh->GetTriangle(VP, prim_index);								\
																							\
	/* Perform ray-tri overlap test and return */											\
	if(RayTriOverlap(*VP.Vertex[0], *VP.Vertex[1], *VP.Vertex[2]))							\
	{																						\
		HANDLE_CONTACT(prim_index, flag)													\
	}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Constructor.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
RayCollider::RayCollider() :
	mNbRayBVTests		(0),
	mNbRayPrimTests		(0),
	mNbIntersections	(0),
	mCulling			(true),
#ifdef OPC_RAYHIT_CALLBACK
	mHitCallback		(null),
	mUserData			(0),
#else
	mClosestHit			(false),
	mStabbedFaces		(null),
#endif
	mMaxDist			(MAX_FLOAT)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Destructor.
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
RayCollider::~RayCollider()
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Validates current settings. You should call this method after all the settings and callbacks have been defined.
 *	\return		null if everything is ok, else a string describing the problem
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char* RayCollider::ValidateSettings()
{
	if(mMaxDist<0.0f)											return "Higher distance bound must be positive!";
	if(TemporalCoherenceEnabled() && !FirstContactEnabled())	return "Temporal coherence only works with ""First contact"" mode!";
#ifndef OPC_RAYHIT_CALLBACK
	if(mClosestHit && FirstContactEnabled())					return "Closest hit doesn't work with ""First contact"" mode!";
	if(TemporalCoherenceEnabled() && mClosestHit)				return "Temporal coherence can't guarantee to report closest hit!";
#endif
	if(SkipPrimitiveTests())									return "SkipPrimitiveTests not possible for RayCollider ! (not implemented)";
	return null;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Generic stabbing query for generic OPCODE models. After the call, access the results:
 *	- with GetContactStatus()
 *	- in the user-provided destination array
 *
 *	\param		world_ray		[in] stabbing ray in world space
 *	\param		model			[in] Opcode model to collide with
 *	\param		world			[in] model's world matrix, or null
 *	\param		cache			[in] a possibly cached face index, or null
 *	\return		true if success
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool RayCollider::Collide(const IceMaths::Ray& world_ray, const Model& model, const IceMaths::Matrix4x4* world, udword* cache)
{
	// Checkings
	if(!Setup(&model))	return false;

	// Init collision query
	float maxDistanceBkp = mMaxDist;	
	Point originBkp = mOrigin;
	Point dirBkp = mDir;

	if(InitQuery(world_ray, world, cache))
	{
		mMaxDist = maxDistanceBkp;
		mDir = dirBkp;
		mOrigin = originBkp;
		return true;
	}

	if(!model.HasLeafNodes())
	{
		if(model.IsQuantized())
		{
			const AABBQuantizedNoLeafTree* Tree = (const AABBQuantizedNoLeafTree*)model.GetTree();

			// Setup dequantization coeffs
			mCenterCoeff	= Tree->mCenterCoeff;
			mExtentsCoeff	= Tree->mExtentsCoeff;

			// Perform stabbing query
			if(IR(mMaxDist)!=IEEE_MAX_FLOAT)	_SegmentStab(Tree->GetNodes());
			else								_RayStab(Tree->GetNodes());
		}
		else
		{
			const AABBNoLeafTree* Tree = (const AABBNoLeafTree*)model.GetTree();

			// Perform stabbing query
			if(IR(mMaxDist)!=IEEE_MAX_FLOAT)	_SegmentStab(Tree->GetNodes());
			else								_RayStab(Tree->GetNodes());
		}
	}
	else
	{
		if(model.IsQuantized())
		{
			const AABBQuantizedTree* Tree = (const AABBQuantizedTree*)model.GetTree();

			// Setup dequantization coeffs
			mCenterCoeff	= Tree->mCenterCoeff;
			mExtentsCoeff	= Tree->mExtentsCoeff;

			// Perform stabbing query
			if(IR(mMaxDist)!=IEEE_MAX_FLOAT)	_SegmentStab(Tree->GetNodes());
			else								_RayStab(Tree->GetNodes());
		}
		else
		{
			const AABBCollisionTree* Tree = (const AABBCollisionTree*)model.GetTree();

			// Perform stabbing query
			if(IR(mMaxDist)!=IEEE_MAX_FLOAT)	_SegmentStab(Tree->GetNodes());
			else								_RayStab(Tree->GetNodes());
		}
	}

        // reverts max distance, etc
	mMaxDist = maxDistanceBkp;
	mDir = dirBkp;
	mOrigin = originBkp;

	// Update cache if needed
	UPDATE_CACHE
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Initializes a stabbing query :
 *	- reset stats & contact status
 *	- compute ray in local space
 *	- check temporal coherence
 *
 *	\param		world_ray	[in] stabbing ray in world space
 *	\param		world		[in] object's world matrix, or null
 *	\param		face_id		[in] index of previously stabbed triangle
 *	\return		TRUE if we can return immediately
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL RayCollider::InitQuery(const IceMaths::Ray& world_ray, const IceMaths::Matrix4x4* world, udword* face_id)
{
	// Reset stats & contact status
	Collider::InitQuery();

	mNbRayBVTests		= 0;
	mNbRayPrimTests		= 0;
	mNbIntersections	= 0;

#ifndef OPC_RAYHIT_CALLBACK
	if(mStabbedFaces)	mStabbedFaces->Reset();
#endif

	// Compute ray in local space
	// The (Origin/Dir) form is needed for the ray-triangle test anyway (even for segment tests)
	if(world)
	{
#ifdef OPC_RAYCOLLIDER_SCALE_BEFORE_OVERLAP
		// Matrix normalization & scaling stripping		
		Matrix4x4 normWorldm;
		NormalizePRSMatrix( normWorldm, mLocalScale, *world );
		
		// Invert model matrix
		Matrix3x3 InvWorld = normWorldm;
		mDir = InvWorld * world_ray.mDir;

		Matrix4x4 World;
		InvertPRMatrix(World, normWorldm);
		mOrigin = world_ray.mOrig * World;
#else
		// Now we are a much better code to get the ray in local space.
		// Some notes about this new code:
		//	- faster, because we don't need to compute square roots anymore;
		//  - faster yet, because the number of divisions is even smaller now;
		//  - the intersection tests are robust enough to handle rays with non-unit direction vectors;
		//  - matrices are less subject to FPU errors, because I don't like square root;
		//	 (it seems to introduce errors, because it cuts number precision by a half when
		//	  stripping the matrix scale off)
		//  - the code is shorter and easier to maintain;  :P
		#pragma message(" >> Using new code for ray collision")		

		// first, invert the world matrix and transform the ray's origin
		Matrix4x4 World;
		InvertPRSMatrix(World, *world);
		mOrigin = world_ray.mOrig * World;

		// second, transform the ray's direction
		Matrix3x3 InvWorld = World;
		mDir = world_ray.mDir * InvWorld;
#endif
	}
	else
	{
          	mLocalScale.Set(1.0f,1.0f,1.0f);
		mDir	= world_ray.mDir;
		mOrigin	= world_ray.mOrig;
	}

	// 4) Special case: 1-triangle meshes [Opcode 1.3]
	if(mCurrentModel && mCurrentModel->HasSingleNode())
	{
		// We simply perform the BV-Prim overlap test each time. We assume single triangle has index 0.
		if(!SkipPrimitiveTests())
		{
			// Perform overlap test between the unique triangle and the ray (and set contact status if needed)
			SEGMENT_PRIM(udword(0), OPC_CONTACT)

			// Return immediately regardless of status
			return TRUE;
		}
	}

	// Check temporal coherence :

	// Test previously colliding primitives first
	if(TemporalCoherenceEnabled() && FirstContactEnabled() && face_id && *face_id!=INVALID_ID)
	{
#ifdef OLD_CODE
#ifndef OPC_RAYHIT_CALLBACK
		if(!mClosestHit)
#endif
		{
			// Request vertices from the app
			VertexPointers VP;
			mIMesh->GetTriangle(VP, *face_id);
			// Perform ray-cached tri overlap test
			if(RayTriOverlap(*VP.Vertex[0], *VP.Vertex[1], *VP.Vertex[2]))
			{
				// Intersection point is valid if:
				// - distance is positive (else it can just be a face behind the orig point)
				// - distance is smaller than a given max distance (useful for shadow feelers)
//				if(mStabbedFace.mDistance>0.0f && mStabbedFace.mDistance<mMaxDist)
				if(IR(mStabbedFace.mDistance)<IR(mMaxDist))	// The other test is already performed in RayTriOverlap
				{
					// Set contact status
					mFlags |= OPC_TEMPORAL_CONTACT;

					mStabbedFace.mFaceID = *face_id;

#ifndef OPC_RAYHIT_CALLBACK
					if(mStabbedFaces)	mStabbedFaces->AddFace(mStabbedFace);
#endif
					return TRUE;
				}
			}
		}
#else
		// New code
		// We handle both Segment/ray queries with the same segment code, and a possible infinite limit
		SEGMENT_PRIM(*face_id, OPC_TEMPORAL_CONTACT)

		// Return immediately if possible
		if(GetContactStatus())	return TRUE;
#endif
	}

	// Precompute data (moved after temporal coherence since only needed for ray-AABB)
	if(IR(mMaxDist)!=IEEE_MAX_FLOAT)
	{
		// For Segment-AABB overlap
		mData = 0.5f * mDir * mMaxDist;
		mData2 = mOrigin + mData;

		// Precompute mFDir;
		mFDir.x = fabsf(mData.x);
		mFDir.y = fabsf(mData.y);
		mFDir.z = fabsf(mData.z);
	}
	else
	{
		// For Ray-AABB overlap
//		udword x = SIR(mDir.x)-1;
//		udword y = SIR(mDir.y)-1;
//		udword z = SIR(mDir.z)-1;
//		mData.x = FR(x);
//		mData.y = FR(y);
//		mData.z = FR(z);

		// Precompute mFDir;
		mFDir.x = fabsf(mDir.x);
		mFDir.y = fabsf(mDir.y);
		mFDir.z = fabsf(mDir.z);
	}

	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Stabbing query for vanilla AABB trees.
 *	\param		world_ray		[in] stabbing ray in world space
 *	\param		tree			[in] AABB tree
 *	\param		box_indices		[out] indices of stabbed boxes
 *	\return		true if success
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool RayCollider::Collide(const IceMaths::Ray& world_ray, const AABBTree* tree, Container& box_indices)
{
	// ### bad design here

	// This is typically called for a scene tree, full of -AABBs-, not full of triangles.
	// So we don't really have "primitives" to deal with. Hence it doesn't work with
	// "FirstContact" + "TemporalCoherence".
	ASSERT( !(FirstContactEnabled() && TemporalCoherenceEnabled()) );

	// Checkings
	if(!tree)					return false;

	// Init collision query
	// Basically this is only called to initialize precomputed data
	if(InitQuery(world_ray))	return true;

	// Perform stabbing query
	if(IR(mMaxDist)!=IEEE_MAX_FLOAT)	_SegmentStab(tree, box_indices);
	else								_RayStab(tree, box_indices);

	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Recursive stabbing query for normal AABB trees.
 *	\param		node	[in] current collision node
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RayCollider::_SegmentStab(const AABBCollisionNode* node)
{
	// Perform Segment-AABB overlap test
	if(!SegmentAABBOverlap(node->mAABB.mCenter, node->mAABB.mExtents))	return;

	if(node->IsLeaf())
	{
		SEGMENT_PRIM(node->GetPrimitive(), OPC_CONTACT)
	}
	else
	{
		_SegmentStab(node->GetPos());

		if(ContactFound()) return;

		_SegmentStab(node->GetNeg());
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Recursive stabbing query for quantized AABB trees.
 *	\param		node	[in] current collision node
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RayCollider::_SegmentStab(const AABBQuantizedNode* node)
{
	// Dequantize box
	const QuantizedAABB& Box = node->mAABB;
	const IceMaths::Point Center(float(Box.mCenter[0]) * mCenterCoeff.x, float(Box.mCenter[1]) * mCenterCoeff.y, float(Box.mCenter[2]) * mCenterCoeff.z);
	const IceMaths::Point Extents(float(Box.mExtents[0]) * mExtentsCoeff.x, float(Box.mExtents[1]) * mExtentsCoeff.y, float(Box.mExtents[2]) * mExtentsCoeff.z);

	// Perform Segment-AABB overlap test
	if(!SegmentAABBOverlap(Center, Extents))	return;

	if(node->IsLeaf())
	{
		SEGMENT_PRIM(node->GetPrimitive(), OPC_CONTACT)
	}
	else
	{
		_SegmentStab(node->GetPos());

		if(ContactFound()) return;

		_SegmentStab(node->GetNeg());
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Recursive stabbing query for no-leaf AABB trees.
 *	\param		node	[in] current collision node
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RayCollider::_SegmentStab(const AABBNoLeafNode* node)
{
	// Perform Segment-AABB overlap test
	if(!SegmentAABBOverlap(node->mAABB.mCenter, node->mAABB.mExtents))	return;

	if(node->HasPosLeaf())
	{
		SEGMENT_PRIM(node->GetPosPrimitive(), OPC_CONTACT)
	}
	else _SegmentStab(node->GetPos());

	if(ContactFound()) return;

	if(node->HasNegLeaf())
	{
		SEGMENT_PRIM(node->GetNegPrimitive(), OPC_CONTACT)
	}
	else _SegmentStab(node->GetNeg());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Recursive stabbing query for quantized no-leaf AABB trees.
 *	\param		node	[in] current collision node
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RayCollider::_SegmentStab(const AABBQuantizedNoLeafNode* node)
{
	// Dequantize box
	const QuantizedAABB& Box = node->mAABB;
	const IceMaths::Point Center(float(Box.mCenter[0]) * mCenterCoeff.x, float(Box.mCenter[1]) * mCenterCoeff.y, float(Box.mCenter[2]) * mCenterCoeff.z);
	const IceMaths::Point Extents(float(Box.mExtents[0]) * mExtentsCoeff.x, float(Box.mExtents[1]) * mExtentsCoeff.y, float(Box.mExtents[2]) * mExtentsCoeff.z);

	// Perform Segment-AABB overlap test
	if(!SegmentAABBOverlap(Center, Extents))	return;

	if(node->HasPosLeaf())
	{
		SEGMENT_PRIM(node->GetPosPrimitive(), OPC_CONTACT)
	}
	else _SegmentStab(node->GetPos());

	if(ContactFound()) return;

	if(node->HasNegLeaf())
	{
		SEGMENT_PRIM(node->GetNegPrimitive(), OPC_CONTACT)
	}
	else _SegmentStab(node->GetNeg());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Recursive stabbing query for vanilla AABB trees.
 *	\param		node		[in] current collision node
 *	\param		box_indices	[out] indices of stabbed boxes
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RayCollider::_SegmentStab(const AABBTreeNode* node, Container& box_indices)
{
	// Test the box against the segment
   IceMaths::Point Center, Extents;
	node->GetAABB()->GetCenter(Center);
	node->GetAABB()->GetExtents(Extents);
	if(!SegmentAABBOverlap(Center, Extents))	return;

	if(node->IsLeaf())
	{
		box_indices.Add(node->GetPrimitives(), node->GetNbPrimitives());
	}
	else
	{
		_SegmentStab(node->GetPos(), box_indices);
		_SegmentStab(node->GetNeg(), box_indices);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Recursive stabbing query for normal AABB trees.
 *	\param		node	[in] current collision node
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RayCollider::_RayStab(const AABBCollisionNode* node)
{
	// Perform Ray-AABB overlap test
	if(!RayAABBOverlap(node->mAABB.mCenter, node->mAABB.mExtents))	return;

	if(node->IsLeaf())
	{
		RAY_PRIM(node->GetPrimitive(), OPC_CONTACT)
	}
	else
	{
		_RayStab(node->GetPos());

		if(ContactFound()) return;

		_RayStab(node->GetNeg());
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Recursive stabbing query for quantized AABB trees.
 *	\param		node	[in] current collision node
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RayCollider::_RayStab(const AABBQuantizedNode* node)
{
	// Dequantize box
	const QuantizedAABB& Box = node->mAABB;
	const IceMaths::Point Center(float(Box.mCenter[0]) * mCenterCoeff.x, float(Box.mCenter[1]) * mCenterCoeff.y, float(Box.mCenter[2]) * mCenterCoeff.z);
	const IceMaths::Point Extents(float(Box.mExtents[0]) * mExtentsCoeff.x, float(Box.mExtents[1]) * mExtentsCoeff.y, float(Box.mExtents[2]) * mExtentsCoeff.z);

	// Perform Ray-AABB overlap test
	if(!RayAABBOverlap(Center, Extents))	return;

	if(node->IsLeaf())
	{
		RAY_PRIM(node->GetPrimitive(), OPC_CONTACT)
	}
	else
	{
		_RayStab(node->GetPos());

		if(ContactFound()) return;

		_RayStab(node->GetNeg());
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Recursive stabbing query for no-leaf AABB trees.
 *	\param		node	[in] current collision node
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RayCollider::_RayStab(const AABBNoLeafNode* node)
{
	// Perform Ray-AABB overlap test
	if(!RayAABBOverlap(node->mAABB.mCenter, node->mAABB.mExtents))	return;

	if(node->HasPosLeaf())
	{
		RAY_PRIM(node->GetPosPrimitive(), OPC_CONTACT)
	}
	else _RayStab(node->GetPos());

	if(ContactFound()) return;

	if(node->HasNegLeaf())
	{
		RAY_PRIM(node->GetNegPrimitive(), OPC_CONTACT)
	}
	else _RayStab(node->GetNeg());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Recursive stabbing query for quantized no-leaf AABB trees.
 *	\param		node	[in] current collision node
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RayCollider::_RayStab(const AABBQuantizedNoLeafNode* node)
{
	// Dequantize box
	const QuantizedAABB& Box = node->mAABB;
	const IceMaths::Point Center(float(Box.mCenter[0]) * mCenterCoeff.x, float(Box.mCenter[1]) * mCenterCoeff.y, float(Box.mCenter[2]) * mCenterCoeff.z);
	const IceMaths::Point Extents(float(Box.mExtents[0]) * mExtentsCoeff.x, float(Box.mExtents[1]) * mExtentsCoeff.y, float(Box.mExtents[2]) * mExtentsCoeff.z);

	// Perform Ray-AABB overlap test
	if(!RayAABBOverlap(Center, Extents))	return;

	if(node->HasPosLeaf())
	{
		RAY_PRIM(node->GetPosPrimitive(), OPC_CONTACT)
	}
	else _RayStab(node->GetPos());

	if(ContactFound()) return;

	if(node->HasNegLeaf())
	{
		RAY_PRIM(node->GetNegPrimitive(), OPC_CONTACT)
	}
	else _RayStab(node->GetNeg());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Recursive stabbing query for vanilla AABB trees.
 *	\param		node		[in] current collision node
 *	\param		box_indices	[out] indices of stabbed boxes
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void RayCollider::_RayStab(const AABBTreeNode* node, Container& box_indices)
{
	// Test the box against the ray
	IceMaths::Point Center, Extents;
	node->GetAABB()->GetCenter(Center);
	node->GetAABB()->GetExtents(Extents);
	if(!RayAABBOverlap(Center, Extents))	return;

	if(node->IsLeaf())
	{
		mFlags |= OPC_CONTACT;
		box_indices.Add(node->GetPrimitives(), node->GetNbPrimitives());
	}
	else
	{
		_RayStab(node->GetPos(), box_indices);
		_RayStab(node->GetNeg(), box_indices);
	}
}
