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
 *	Contains code for a tree collider.
 *	\file		OPC_TreeCollider.h
 *	\author		Pierre Terdiman
 *	\date		March, 20, 2001
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Include Guard
#ifndef __OPC_TREECOLLIDER_H__
#define __OPC_TREECOLLIDER_H__

	//! This structure holds cached information used by the algorithm.
	//! Two model pointers and two colliding primitives are cached. Model pointers are assigned
	//! to their respective meshes, and the pair of colliding primitives is used for temporal
	//! coherence. That is, in case temporal coherence is enabled, those two primitives are
	//! tested for overlap before everything else. If they still collide, we're done before
	//! even entering the recursive collision code.
	struct OPCODE_API BVTCache : Pair
	{
		//! Constructor
		inline_				BVTCache()
							{
								ResetCache();
								ResetCountDown();
							}

					void	ResetCache()
							{
								Model0			= null;
								Model1			= null;
								id0				= 0;
								id1				= 1;
#ifdef __MESHMERIZER_H__		// Collision hulls only supported within ICE !
								HullTest		= true;
								SepVector.pid	= 0;
								SepVector.qid	= 0;
								SepVector.SV	= Point(1.0f, 0.0f, 0.0f);
#endif // __MESHMERIZER_H__
							}

		inline_		void	ResetCountDown()
							{
#ifdef __MESHMERIZER_H__		// Collision hulls only supported within ICE !
								CountDown		= 50;
#endif // __MESHMERIZER_H__
							}

		const Model*		Model0;	//!< Model for first object
		const Model*		Model1;	//!< Model for second object

#ifdef __MESHMERIZER_H__	// Collision hulls only supported within ICE !
		SVCache				SepVector;
		udword				CountDown;
		bool				HullTest;
#endif // __MESHMERIZER_H__
	};

	class OPCODE_API AABBTreeCollider : public Collider
	{
		public:
		// Constructor / Destructor
											AABBTreeCollider();
		virtual								~AABBTreeCollider();
		// Generic collision query

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Generic collision query for generic OPCODE models. After the call, access the results with:
		 *	- GetContactStatus()
		 *	- GetNbPairs()
		 *	- GetPairs()
		 *
		 *	\param		cache			[in] collision cache for model pointers and a colliding pair of primitives
		 *	\param		world0			[in] world matrix for first object, or null
		 *	\param		world1			[in] world matrix for second object, or null
		 *	\return		true if success
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
							bool			Collide(BVTCache& cache, const IceMaths::Matrix4x4* world0=null, const IceMaths::Matrix4x4* world1=null);

		// Collision queries
							bool			Collide(const AABBCollisionTree* tree0, const AABBCollisionTree* tree1,				const IceMaths::Matrix4x4* world0=null, const IceMaths::Matrix4x4* world1=null, Pair* cache=null);
							bool			Collide(const AABBNoLeafTree* tree0, const AABBNoLeafTree* tree1,					const IceMaths::Matrix4x4* world0=null, const IceMaths::Matrix4x4* world1=null, Pair* cache=null);
							bool			Collide(const AABBQuantizedTree* tree0, const AABBQuantizedTree* tree1,				const IceMaths::Matrix4x4* world0=null, const IceMaths::Matrix4x4* world1=null, Pair* cache=null);
							bool			Collide(const AABBQuantizedNoLeafTree* tree0, const AABBQuantizedNoLeafTree* tree1,	const IceMaths::Matrix4x4* world0=null, const IceMaths::Matrix4x4* world1=null, Pair* cache=null);
		// Settings

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Settings: selects between full box-box tests or "SAT-lite" tests (where Class III axes are discarded)
		 *	\param		flag		[in] true for full tests, false for coarse tests
		 *	\see		SetFullPrimBoxTest(bool flag)
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		inline_				void			SetFullBoxBoxTest(bool flag)			{ mFullBoxBoxTest		= flag;					}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Settings: selects between full triangle-box tests or "SAT-lite" tests (where Class III axes are discarded)
		 *	\param		flag		[in] true for full tests, false for coarse tests
		 *	\see		SetFullBoxBoxTest(bool flag)
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		inline_				void			SetFullPrimBoxTest(bool flag)			{ mFullPrimBoxTest		= flag;					}

		// Stats

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Stats: gets the number of BV-BV overlap tests after a collision query.
		 *	\see		GetNbPrimPrimTests()
		 *	\see		GetNbBVPrimTests()
		 *	\return		the number of BV-BV tests performed during last query
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		inline_				udword			GetNbBVBVTests()				const	{ return mNbBVBVTests;							}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Stats: gets the number of Triangle-Triangle overlap tests after a collision query.
		 *	\see		GetNbBVBVTests()
		 *	\see		GetNbBVPrimTests()
		 *	\return		the number of Triangle-Triangle tests performed during last query
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		inline_				udword			GetNbPrimPrimTests()			const	{ return mNbPrimPrimTests;						}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Stats: gets the number of BV-Triangle overlap tests after a collision query.
		 *	\see		GetNbBVBVTests()
		 *	\see		GetNbPrimPrimTests()
		 *	\return		the number of BV-Triangle tests performed during last query
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		inline_				udword			GetNbBVPrimTests()				const	{ return mNbBVPrimTests;						}

		// Data access

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Gets the number of contacts after a collision query.
		 *	\see		GetContactStatus()
		 *	\see		GetPairs()
		 *	\return		the number of contacts / colliding pairs.
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		inline_				udword			GetNbPairs()					const	{ return mPairs.GetNbEntries()>>1;				}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Gets the pairs of colliding triangles after a collision query.
		 *	\see		GetContactStatus()
		 *	\see		GetNbPairs()
		 *	\return		the list of colliding pairs (triangle indices)
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		inline_				const Pair*		GetPairs()						const	{ return (const Pair*)mPairs.GetEntries();		}

		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/**
		 *	Validates current settings. You should call this method after all the settings and callbacks have been defined for a collider.
		 *	\return		null if everything is ok, else a string describing the problem
		 */
		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		override(Collider)	const char*		ValidateSettings();

		protected:
		// Colliding pairs
							Container		mPairs;				//!< Pairs of colliding primitives
		// User mesh interfaces
					const	MeshInterface*	mIMesh0;			//!< User-defined mesh interface for object0
					const	MeshInterface*	mIMesh1;			//!< User-defined mesh interface for object1
		// Stats
							udword			mNbBVBVTests;		//!< Number of BV-BV tests
							udword			mNbPrimPrimTests;	//!< Number of Primitive-Primitive tests
							udword			mNbBVPrimTests;		//!< Number of BV-Primitive tests
		// Precomputed data
							IceMaths::Matrix3x3		mAR;				//!< Absolute rotation matrix
							IceMaths::Matrix3x3		mR0to1;				//!< Rotation from object0 to object1
							IceMaths::Matrix3x3		mR1to0;				//!< Rotation from object1 to object0
							IceMaths::Point			mT0to1;				//!< Translation from object0 to object1
							IceMaths::Point			mT1to0;				//!< Translation from object1 to object0
		// Precomputed scale data
							IceMaths::Matrix3x3		mSR0to1;			//!< Scale & rotation from object0 to object1 - before scale in object1
							IceMaths::Matrix3x3		mSR1to0;			//!< Scale & rotation from object1 to object0 - before scale in object0
							IceMaths::Point			mScale1;			//!< Scale for object 1
							IceMaths::Point			mScale0;			//!< Scale for object 0
		// Dequantization coeffs
							IceMaths::Point			mCenterCoeff0;
							IceMaths::Point			mExtentsCoeff0;
							IceMaths::Point			mCenterCoeff1;
							IceMaths::Point			mExtentsCoeff1;
		// Leaf description
							IceMaths::Point			mLeafVerts[3];		//!< Triangle vertices
							udword			mLeafIndex;			//!< Triangle index
		// Settings
							bool			mFullBoxBoxTest;	//!< Perform full BV-BV tests (true) or SAT-lite tests (false)
							bool			mFullPrimBoxTest;	//!< Perform full Primitive-BV tests (true) or SAT-lite tests (false)
		// Internal methods

			// Standard AABB trees
							void			_Collide(const AABBCollisionNode* b0, const AABBCollisionNode* b1);
			// Quantized AABB trees
							void			_Collide(const AABBQuantizedNode* b0, const AABBQuantizedNode* b1, const IceMaths::Point& a, const IceMaths::Point& Pa, const IceMaths::Point& b, const IceMaths::Point& Pb);
			// No-leaf AABB trees
							void			_CollideTriBox(const AABBNoLeafNode* b);
							void			_CollideBoxTri(const AABBNoLeafNode* b);
							void			_Collide(const AABBNoLeafNode* a, const AABBNoLeafNode* b);
			// Quantized no-leaf AABB trees
							void			_CollideTriBox(const AABBQuantizedNoLeafNode* b);
							void			_CollideBoxTri(const AABBQuantizedNoLeafNode* b);
							void			_Collide(const AABBQuantizedNoLeafNode* a, const AABBQuantizedNoLeafNode* b);
			// Overlap tests
							void			PrimTest(udword id0, udword id1); // OK
			inline_			void			PrimTestTriIndex(udword id1);     // OK
			inline_			void			PrimTestIndexTri(udword id0);	  // OK

			inline_			BOOL			BoxBoxOverlap(const IceMaths::Point& ea, const IceMaths::Point& ca, const IceMaths::Point& eb, const IceMaths::Point& cb); // OK
			inline_			BOOL			TriBoxOverlap(const IceMaths::Point& center, const IceMaths::Point& extents); // OK
			inline_			BOOL			TriTriOverlap(const IceMaths::Point& V0, const IceMaths::Point& V1, const IceMaths::Point& V2, const IceMaths::Point& U0, const IceMaths::Point& U1, const IceMaths::Point& U2); // OK
			// Init methods
							void			InitQuery(const IceMaths::Matrix4x4* world0=null, const IceMaths::Matrix4x4* world1=null);
							bool			CheckTemporalCoherence(Pair* cache);

		inline_				BOOL			Setup(const MeshInterface* mi0, const MeshInterface* mi1)
											{
												mIMesh0	= mi0;
												mIMesh1	= mi1;

												if(!mIMesh0 || !mIMesh1)	return FALSE;

												return TRUE;
											}
	};

#endif // __OPC_TREECOLLIDER_H__
