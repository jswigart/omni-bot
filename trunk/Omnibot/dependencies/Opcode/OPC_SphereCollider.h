///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 *	OPCODE - Optimized Collision Detection
 *	Copyright (C) 2001 Pierre Terdiman
 *	Homepage: http://www.codercorner.com/Opcode.htm
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code for a sphere collider.
 *	\file		OPC_SphereCollider.h
 *	\author		Pierre Terdiman
 *	\date		June, 2, 2001
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef __OPC_SPHERECOLLIDER_H__
#define __OPC_SPHERECOLLIDER_H__

	struct OPCODE_API SphereCache : VolumeCache
	{
					SphereCache() : Center(0.0f,0.0f,0.0f), FatRadius2(0.0f), FatCoeff(1.1f)	{}
					~SphereCache()																{}

		// Cached faces signature
		IceMaths::Point		Center;		//!< Sphere used when performing the query resulting in cached faces
		float		FatRadius2;	//!< Sphere used when performing the query resulting in cached faces
		// User settings
		float		FatCoeff;	//!< mRadius2 multiplier used to create a fat sphere
	};

	class OPCODE_API SphereCollider : public VolumeCollider
	{
		public:
		// Constructor / Destructor
											SphereCollider();
		virtual								~SphereCollider();

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Generic collision query for generic OPCODE models. After the call, access the results:
		 *	- with GetContactStatus()
		 *	- with GetNbTouchedPrimitives()
		 *	- with GetTouchedPrimitives()
		 *
		 *	\param		cache			[in/out] a sphere cache
		 *	\param		sphere			[in] collision sphere in local space
		 *	\param		model			[in] Opcode model to collide with
		 *	\param		worlds			[in] sphere's world matrix, or null
		 *	\param		worldm			[in] model's world matrix, or null
		 *	\return		true if success
		 *	\warning	SCALE NOT SUPPORTED IN SPHERE WORLD MATRIX. The matrix must contain rotation & translation parts only.
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
							bool			Collide(SphereCache& cache, const IceMaths::Sphere& sphere, const Model& model, const IceMaths::Matrix4x4* worlds=null, const IceMaths::Matrix4x4* worldm=null);

		// 
							bool			Collide(SphereCache& cache, const IceMaths::Sphere& sphere, const AABBTree* tree);
		protected:
		// Sphere in model space
							IceMaths::Point			mCenter;			//!< Sphere center
							float			mRadius2;			//!< Sphere radius squared

		// Internal methods
							void			_Collide(const AABBCollisionNode* node);
							void			_Collide(const AABBNoLeafNode* node);
							void			_Collide(const AABBQuantizedNode* node);
							void			_Collide(const AABBQuantizedNoLeafNode* node);
							void			_Collide(const AABBTreeNode* node);
							void			_CollideNoPrimitiveTest(const AABBCollisionNode* node);
							void			_CollideNoPrimitiveTest(const AABBNoLeafNode* node);
							void			_CollideNoPrimitiveTest(const AABBQuantizedNode* node);
							void			_CollideNoPrimitiveTest(const AABBQuantizedNoLeafNode* node);
			// Overlap tests
		inline_				BOOL			SphereContainsBox(const IceMaths::Point& bc, const IceMaths::Point& be);
		inline_				BOOL			SphereAABBOverlap(const IceMaths::Point& center, const IceMaths::Point& extents);
							BOOL			SphereTriOverlap(const IceMaths::Point& vert0, const IceMaths::Point& vert1, const IceMaths::Point& vert2);
			// Init methods
							BOOL			InitQuery(SphereCache& cache, const IceMaths::Sphere& sphere, const IceMaths::Matrix4x4* worlds=null, const IceMaths::Matrix4x4* worldm=null);
	};

	class OPCODE_API HybridSphereCollider : public SphereCollider
	{
		public:
		// Constructor / Destructor
											HybridSphereCollider();
		virtual								~HybridSphereCollider();

							bool			Collide(SphereCache& cache, const IceMaths::Sphere& sphere, const HybridModel& model, const IceMaths::Matrix4x4* worlds=null, const IceMaths::Matrix4x4* worldm=null);
		protected:
							Container		mTouchedBoxes;
	};

#endif // __OPC_SPHERECOLLIDER_H__
