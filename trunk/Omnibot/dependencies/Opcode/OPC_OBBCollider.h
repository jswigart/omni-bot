///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 *	OPCODE - Optimized Collision Detection
 *	Copyright (C) 2001 Pierre Terdiman
 *	Homepage: http://www.codercorner.com/Opcode.htm
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 *	Contains code for an OBB collider.
 *	\file		OPC_OBBCollider.h
 *	\author		Pierre Terdiman
 *	\date		January, 1st, 2002
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef __OPC_OBBCOLLIDER_H__
#define __OPC_OBBCOLLIDER_H__

	struct OPCODE_API OBBCache : VolumeCache
	{
					OBBCache() : FatCoeff(1.1f)
					{
						FatBox.mCenter.Zero();
						FatBox.mExtents.Zero();
						FatBox.mRot.Identity();
					}

		// Cached faces signature
		IceMaths::OBB				FatBox;		//!< Box used when performing the query resulting in cached faces
		// User settings
		float			FatCoeff;	//!< extents multiplier used to create a fat box
	};

	class OPCODE_API OBBCollider : public VolumeCollider
	{
		public:
		// Constructor / Destructor
											OBBCollider();
		virtual								~OBBCollider();

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Generic collision query for generic OPCODE models. After the call, access the results:
		 *	- with GetContactStatus()
		 *	- with GetNbTouchedPrimitives()
		 *	- with GetTouchedPrimitives()
		 *
		 *	\param		cache			[in/out] a box cache
		 *	\param		box				[in] collision OBB in local space
		 *	\param		model			[in] Opcode model to collide with
		 *	\param		worldb			[in] OBB's world matrix, or null
		 *	\param		worldm			[in] model's world matrix, or null
		 *	\return		true if success
		 *	\warning	SCALE NOT SUPPORTED IN OBB MATRIX. The obb's matrix must contain rotation & translation parts only.
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
							bool			Collide(OBBCache& cache, const IceMaths::OBB& box, const Model& model, const IceMaths::Matrix4x4* worldb=null, const IceMaths::Matrix4x4* worldm=null);

		// Settings

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Settings: select between full box-box tests or "SAT-lite" tests (where Class III axes are discarded)
		 *	\param		flag		[in] true for full tests, false for coarse tests
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		inline_				void			SetFullBoxBoxTest(bool flag)	{ mFullBoxBoxTest = flag;	}

		// Settings

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Validates current settings. You should call this method after all the settings and callbacks have been defined for a collider.
		 *	\return		null if everything is ok, else a string describing the problem
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		override(Collider)	const char*		ValidateSettings();

		protected:
		// Precomputed data
							IceMaths::Matrix3x3		mAR;				//!< Absolute rotation matrix
							IceMaths::Matrix3x3		mRModelToBox;		//!< Rotation from model space to obb space
							IceMaths::Matrix3x3		mRBoxToModel;		//!< Rotation from obb space to model space
							IceMaths::Point			mTModelToBox;		//!< Translation from model space to obb space
							IceMaths::Point			mTBoxToModel;		//!< Translation from obb space to model space

							IceMaths::Matrix3x3		mSRModelToBox;		//!< Scale and Rotation from model space to obb space

							IceMaths::Point			mBoxExtents;
							IceMaths::Point			mB0;				//!< - mTModelToBox + mBoxExtents
							IceMaths::Point			mB1;				//!< - mTModelToBox - mBoxExtents

							float			mBBx1;
							float			mBBy1;
							float			mBBz1;

							float			mBB_1;
							float			mBB_2;
							float			mBB_3;
							float			mBB_4;
							float			mBB_5;
							float			mBB_6;
							float			mBB_7;
							float			mBB_8;
							float			mBB_9;

		// Leaf description
							IceMaths::Point			mLeafVerts[3];		//!< Triangle vertices
		// Settings
							bool			mFullBoxBoxTest;	//!< Perform full BV-BV tests (true) or SAT-lite tests (false)
		// Internal methods
							void			_Collide(const AABBCollisionNode* node);
							void			_Collide(const AABBNoLeafNode* node);
							void			_Collide(const AABBQuantizedNode* node);
							void			_Collide(const AABBQuantizedNoLeafNode* node);
							void			_CollideNoPrimitiveTest(const AABBCollisionNode* node);
							void			_CollideNoPrimitiveTest(const AABBNoLeafNode* node);
							void			_CollideNoPrimitiveTest(const AABBQuantizedNode* node);
							void			_CollideNoPrimitiveTest(const AABBQuantizedNoLeafNode* node);
			// Overlap tests
		inline_				BOOL			OBBContainsBox(const IceMaths::Point& bc, const IceMaths::Point& be);
		inline_				BOOL			BoxBoxOverlap(const IceMaths::Point& extents, const IceMaths::Point& center);
		inline_				BOOL			TriBoxOverlap();
			// Init methods
							BOOL			InitQuery(OBBCache& cache, const IceMaths::OBB& box, const IceMaths::Matrix4x4* worldb=null, const IceMaths::Matrix4x4* worldm=null);
	};

	class OPCODE_API HybridOBBCollider : public OBBCollider
	{
		public:
		// Constructor / Destructor
											HybridOBBCollider();
		virtual								~HybridOBBCollider();

							bool			Collide(OBBCache& cache, const IceMaths::OBB& box, const HybridModel& model, const IceMaths::Matrix4x4* worldb=null, const IceMaths::Matrix4x4* worldm=null);
		protected:
							Container		mTouchedBoxes;
	};

#endif // __OPC_OBBCOLLIDER_H__
