////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "VBSPFileImporter.h"
#include "VBSPFileData.h"
#include "pow2assert.h"

#include <boost/lexical_cast.hpp>
#include <boost/filesystem/path.hpp>

namespace MapFormat
{
	using namespace VBSP;

	template<typename T>
	T Squared( T value )
	{
		return value*value;
	}
	
	template <class T>
	T lerp( const T& one, const T& two, float val )
	{
		return one + ( two - one )*val;
	}
	
	// ------------------------------------------------------------------------------------------------
	VBSPFileImporter::VBSPFileImporter()
	{
	}

	// ------------------------------------------------------------------------------------------------
	VBSPFileImporter::~VBSPFileImporter()
	{
	}
		
	// ------------------------------------------------------------------------------------------------
	//	Import method.
	bool VBSPFileImporter::ParseFile( CollisionWorld & scene, const char * memoryBuffer, size_t memoryBufferSize )
	{
		VBSP::VBSPModel bspModel;
		if ( memoryBuffer && bspModel.ReadBsp( memoryBuffer, memoryBufferSize ) )
		{
			CreateSceneGraph( scene, scene.mRootNode, bspModel );
			return true;
		}
		return false;
	}

	static void CreatePolyForPlane( std::vector<Vector3f> & verts, const Vector3f & normal, const float dist )
	{
#define VectorMA( v, s, b, o )    ( ( o )[0] = ( v )[0] + ( b )[0] * ( s ),( o )[1] = ( v )[1] + ( b )[1] * ( s ),( o )[2] = ( v )[2] + ( b )[2] * ( s ) )

		const int POLY_SIZE = 128 * 1024;

		// what's the biggest axis
		float f = 0.0f;
		int b = 0;
		for ( int i = 0; i < 3; ++i )
			if ( fabs( normal[ i ] ) > f )
			{
				b = i;
				f = fabs( normal[ i ] );
			}

		Vector3f up( 0.f, 0.f, 0.f );
		if ( b == 0 || b == 1 )
			up[ 2 ] = 1.0f;
		else
			up[ 0 ] = 1.0f;

		const float dot = up.Dot(normal);
		VectorMA( up, -dot, normal, up );
		up.Normalize();

		Vector3f orig = normal * dist;
		Vector3f right = up.Cross( normal );

		up *= POLY_SIZE;
		right *= POLY_SIZE;

		// make a large plane for this face
		verts.resize( 4 );
		verts[ 0 ] = orig - right + up;
		verts[ 1 ] = orig + right + up;
		verts[ 2 ] = orig + right - up;
		verts[ 3 ] = orig - right - up;
	}

	static void ClipPolyInPlace( std::vector<Vector3f> & verts, const Vector3f & normal, const float dist, float epsilon )
	{
		enum Side
		{
			SIDE_FRONT,
			SIDE_BACK,
			SIDE_ON,
			SIDE_CROSS,
		};

		std::vector<float> vertDistances;
		std::vector<Side> vertSides;
		int sideCounts[ 3 ] = { 0 };

		// determine sides for each point
		for ( size_t i = 0; i < verts.size(); i++ )
		{
			float dot = verts[ i ].Dot( normal );
			dot -= dist;

			vertDistances.push_back( dot );
			if ( dot > epsilon )
				vertSides.push_back( SIDE_FRONT );
			else if ( dot < -epsilon )
				vertSides.push_back( SIDE_BACK );
			else
				vertSides.push_back( SIDE_ON );

			sideCounts[ vertSides[ i ] ]++;
		}

		vertSides.push_back( vertSides[ 0 ] );
		vertDistances.push_back( vertDistances[ 0 ] );

		if ( !sideCounts[ 0 ] )
		{
			verts.resize( 0 );
			return;
		}

		// nothing clipped, input stays the same
		if ( !sideCounts[ 1 ] )
			return;

		Vector3f p1, p2, mid;

		std::vector<Vector3f> newPoly;
		for ( size_t i = 0; i < verts.size(); i++ )
		{
			p1 = verts[ i ];

			if ( vertSides[ i ] == SIDE_ON )
			{
				newPoly.push_back( p1 );
				continue;
			}

			if ( vertSides[ i ] == SIDE_FRONT )
			{
				newPoly.push_back( p1 );
			}

			if ( vertSides[ i + 1 ] == SIDE_ON || vertSides[ i + 1 ] == vertSides[ i ] )
			{
				continue;
			}

			// generate a split point
			p2 = verts[ ( i + 1 ) % verts.size() ];

			float dot = vertDistances[ i ] / ( vertDistances[ i ] - vertDistances[ i + 1 ] );
			for ( int j = 0; j < 3; j++ )
			{   // avoid round off error when possible
				if ( normal[ j ] == 1 )
					mid[ j ] = dist;
				else if ( normal[ j ] == -1 )
					mid[ j ] = -dist;
				else
					mid[ j ] = p1[ j ] + dot * ( p2[ j ] - p1[ j ] );
			}

			newPoly.push_back( mid );
		}

		verts = newPoly;
	}
	
	void VBSPFileImporter::ProcessFace( CollisionWorld & scene, ModelPtr geom, const VBSP::VBSPModel & bspmodel, int subModelIndex, const VBSP::dleaf_t & face )
	{
		for ( int b = 0; b < face.mNumleafbrushes; ++b )
		{
			const dleafbrush_t & leafBrush = bspmodel.mBspLeafBrushes[ face.mFirstleafbrush + b ];
			const dbrush_t & brush = bspmodel.mBrushes[ leafBrush.mBrushIndex ];

			for ( int s = 0; s < brush.mNumSides; ++s )
			{
				const dbrushside_t & brushSide = bspmodel.mBrushSides[ brush.mFirstSide + s ];

				// no material means it's disabled(i think)
				if ( brushSide.mTexInfo < 0 )
					continue;

				const dplane_t & plane = bspmodel.mPlanes[ brushSide.mPlaneNum ];

				std::vector<Vector3f> sideVerts;
				CreatePolyForPlane( sideVerts, plane.mNormal, plane.mDistance );

				if ( sideVerts.size() == 0 )
					continue;

				// trim the polygon from the other brushes
				for ( int c = 0; c < brush.mNumSides && !sideVerts.empty(); ++c )
				{
					const dbrushside_t & brushSide2 = bspmodel.mBrushSides[ brush.mFirstSide + c ];

					if ( brushSide.mPlaneNum == brushSide2.mPlaneNum )
						continue;

					const dplane_t & plane2 = bspmodel.mPlanes[ brushSide2.mPlaneNum ^ 1 ];
					ClipPolyInPlace( sideVerts, plane2.mNormal, plane2.mDistance, 0.f );
				}

				if ( sideVerts.size() == 0 )
					continue;

				// if this side is a displacement, then the displacement geometry should be used,
				// not the side itself though the side represents a boundary for the displacement
				if ( brushSide.mDispInfo > 0 )
				{
					// generate faces from displacement info
					const ddispinfo_t & dispInfo = bspmodel.mDisplacementInfo[ brushSide.mDispInfo ];

					const dface_t & displacementFace = bspmodel.mFaces[ dispInfo.mMapFace ];
					displacementFace;

					const int dispSize = ( ( 1 << dispInfo.mPower ) + 1 );
					dispSize;

					for ( int x = 0; x < dispInfo.Height(); ++x )
					{
						for ( int y = 0; y < dispInfo.Width(); ++y )
						{
							const int v = y * dispInfo.Height() + dispInfo.Width();

							const ddispvert_t & v0 = bspmodel.mDisplacementVertices[ dispInfo.mDispVertStart + v + 0 ];
							const ddispvert_t & v1 = bspmodel.mDisplacementVertices[ dispInfo.mDispVertStart + v + 1 ];
							const ddispvert_t & v2 = bspmodel.mDisplacementVertices[ dispInfo.mDispVertStart + v + 2 ];

							// offset the vertices
							Vector3f v0pos = dispInfo.mStartPosition;
							Vector3f v1pos = dispInfo.mStartPosition;
							Vector3f v2pos = dispInfo.mStartPosition;

							// todo: how are the vertices transformed 'across' the displacementFace ?

							// offset the vertices
							v0pos += ( v0.mVecDir * v0.mDist );
							v1pos += ( v1.mVecDir * v1.mDist );
							v2pos += ( v2.mVecDir * v2.mDist );
						}
					}

					continue;
				}

				const dtexinfo_t & faceTexInfo = bspmodel.mTextureInfo[ brushSide.mTexInfo ];
				const dtexdata_t & faceTexData = bspmodel.mTextureData[ faceTexInfo.mTexdata ];
				const dtexstringtable_t & faceTexStrTbl = bspmodel.mTextureStringTable[ faceTexData.mNameStringTableID ];
				const char * faceTexName = &bspmodel.mTextureStringData[ faceTexStrTbl.mStringDataOffset ];

				// generate a custom material for this surface				
				Material surfaceMat;
				surfaceMat.mName = faceTexName;
				surfaceMat.mContents = (ContentFlags)gEngineFuncs->ConvertValue( brush.mContents, IEngineInterface::ConvertContentsFlags, IEngineInterface::ConvertGameToBot );
				surfaceMat.mSurface = (SurfaceFlags)gEngineFuncs->ConvertValue( faceTexInfo.mFlags, IEngineInterface::ConvertSurfaceFlags, IEngineInterface::ConvertGameToBot );
				
				if ( surfaceMat.mSurface & SURFACE_IGNORE )
					continue;

				for ( size_t x = 1; x < sideVerts.size(); ++x )
				{
					const Vector3f vpos[ 3 ] = {
						sideVerts[ 0 ],
						sideVerts[ x - 1 ],
						sideVerts[ x ]
					};
					/*const Vector3f vnor[ 3 ] = {
						plane.mNormal,
						plane.mNormal,
						plane.mNormal,
					};*/

					geom->AddTri( vpos[ 0 ], vpos[ 1 ], vpos[ 2 ], surfaceMat );
				}
			}
		}
	}

	void VBSPFileImporter::WalkNodeTree( CollisionWorld & scene, ModelPtr geom, const VBSP::VBSPModel & bspmodel, int subModelIndex, int bspNodeIndex )
	{
		if ( bspNodeIndex >= 0 )
		{
			const dnode_t & bspNode = bspmodel.mBspNodes[ bspNodeIndex ];

			// add faces
			/*for ( int f = 0; f < bspNode.mNumFaces; ++f )
			{
				const dface_t & face = bspmodel.mFaces[ bspNode.mFirstFace + f ];
				ProcessFace( bspmodel, subModelIndex, face );
			}*/	

			WalkNodeTree( scene, geom, bspmodel, subModelIndex, bspNode.mChildren[ 0 ] );
			WalkNodeTree( scene, geom, bspmodel, subModelIndex, bspNode.mChildren[ 1 ] );
		}
		else
		{
			const int leafIndex = -1 - bspNodeIndex;
			const dleaf_t & bspLeaf = bspmodel.mBspLeafs[ leafIndex ];
			ProcessFace( scene, geom, bspmodel, subModelIndex, bspLeaf );
		}
	}
	
	inline void SinCos( float radians, float *sine, float *cosine )
	{
		*sine = sin( radians );
		*cosine = cos( radians );
	}

	void CreateMatrix( Matrix3f & matrix, const Vector3f & angles )
	{
		matrix.MakeIdentity();

		enum { PITCH = 0, YAW = 1, ROLL = 2 };
		
		float sr, sp, sy, cr, cp, cy;
		SinCos( Mathf::DegToRad( angles[ YAW ] ), &sy, &cy );
		SinCos( Mathf::DegToRad( angles[ PITCH ] ), &sp, &cp );
		SinCos( Mathf::DegToRad( angles[ ROLL ] ), &sr, &cr );
		
		matrix = Matrix3f(			
			Vector3f( cp*cy, cp*sy, -sp ),
			Vector3f( -( -1 * sr*sp*cy + -1 * cr*-sy ), -( -1 * sr*sp*sy + -1 * cr*cy ), -( -1 * sr*cp ) ),
			Vector3f( cr*sp*cy + -sr*-sy, cr*sp*sy + -sr*cy, cr*cp ),
			true );
	}
	
	template<typename T>
	void CreateStaticNodes( CollisionWorld & scene, NodePtr & parent, const LumpData<T> & list, const VBSP::VBSPModel & bspmodel )
	{
		DefaultMemoryAllocator defaultAllocator;

		for ( size_t i = 0; i < list.size(); ++i )
		{
			const T & lump = list[ i ];

			const dgamelump_staticpropdict_t & staticDictEntry = bspmodel.mStaticPropDict[ lump.mPropType ];

			Matrix3f orient;
			CreateMatrix( orient, lump.mAngles );

			NodePtr node( new Node() );
			node->mStaticModel = i;
			node->mTransform.SetPosition( lump.mOrigin, false );
			node->mTransform.SetOrientation( orient, false );
			
			// has this model already been loaded?
			CollisionWorld::ModelCache::iterator it = scene.mCachedModels.find( staticDictEntry.mName );
			if ( it != scene.mCachedModels.end() )
			{
				node->mModel = it->second;
			}
			else
			{
				// load the model
				const boost::filesystem::path ext = boost::filesystem::path( staticDictEntry.mName ).extension();

				GameModelInfo modelInfo;
				strncpy( modelInfo.mModelName, staticDictEntry.mName, sizeof( modelInfo.mModelName ) );
				strncpy( modelInfo.mModelType, ext.string().c_str() + 1, ext.string().length() );
				gEngineFuncs->GetModel( modelInfo, defaultAllocator );

				// no idea what that is
				if ( modelInfo.mDataBuffer == NULL )
					continue;

				Material defaultMaterial;
				defaultMaterial.mName = modelInfo.mModelName;
				defaultMaterial.mContents = CONT_SOLID;

				node->mModel = scene.ParseModelData( modelInfo, defaultMaterial );
				node->mModel->SetName( modelInfo.mModelName );

				defaultAllocator.FreeMemory( modelInfo.mDataBuffer );
			}

			parent->mChildren.push_back( node );
		}
	}

	void GenDisplacements( CollisionWorld & scene, NodePtr & parent, const VBSP::VBSPModel & bspmodel )
	{
		static bool enableIsolation = false;
		static size_t isolateDisplacement = 0;

		for ( size_t i = 0; i < bspmodel.mDisplacementInfo.size(); ++i )
		{
			const ddispinfo_t & dispInfo = bspmodel.mDisplacementInfo[ i ];

			const dface_t & displacementFace = bspmodel.mFaces[ dispInfo.mMapFace ];

			if ( displacementFace.mNumEdges != 4 || ( enableIsolation && isolateDisplacement != i ) )
				continue;

			const dtexinfo_t & faceTexInfo = bspmodel.mTextureInfo[ displacementFace.mTexInfo ];
			const dtexdata_t & faceTexData = bspmodel.mTextureData[ faceTexInfo.mTexdata ];
			const dtexstringtable_t & faceTexStrTbl = bspmodel.mTextureStringTable[ faceTexData.mNameStringTableID ];
			const char * faceTexName = &bspmodel.mTextureStringData[ faceTexStrTbl.mStringDataOffset ];

			// generate a custom material for this surface				
			Material surfaceMat;
			surfaceMat.mName = faceTexName;
			surfaceMat.mContents = (ContentFlags)gEngineFuncs->ConvertValue( dispInfo.mContents, IEngineInterface::ConvertContentsFlags, IEngineInterface::ConvertGameToBot );
			surfaceMat.mSurface = (SurfaceFlags)gEngineFuncs->ConvertValue( faceTexInfo.mFlags, IEngineInterface::ConvertSurfaceFlags, IEngineInterface::ConvertGameToBot );
			
			if ( surfaceMat.mSurface & SURFACE_IGNORE )
				continue;

			const int edge0Index = bspmodel.mSurfEdges[ displacementFace.mFirstEdge + 0 ];
			const int edge1Index = bspmodel.mSurfEdges[ displacementFace.mFirstEdge + 1 ];
			const int edge2Index = bspmodel.mSurfEdges[ displacementFace.mFirstEdge + 2 ];
			const int edge3Index = bspmodel.mSurfEdges[ displacementFace.mFirstEdge + 3 ];

			const dedge_t & edge0 = bspmodel.GetOrientedEdge( edge0Index );
			const dedge_t & edge1 = bspmodel.GetOrientedEdge( edge1Index );
			const dedge_t & edge2 = bspmodel.GetOrientedEdge( edge2Index );
			const dedge_t & edge3 = bspmodel.GetOrientedEdge( edge3Index );

			const dvertex_t & dispFaceV0 = bspmodel.mVertices[ edge0.mVertIndex[ 0 ] ];
			const dvertex_t & dispFaceV1 = bspmodel.mVertices[ edge1.mVertIndex[ 0 ] ];
			const dvertex_t & dispFaceV2 = bspmodel.mVertices[ edge2.mVertIndex[ 0 ] ];
			const dvertex_t & dispFaceV3 = bspmodel.mVertices[ edge3.mVertIndex[ 0 ] ];

			const Vector3f originalNormal = Normalize( ( dispFaceV1.mPosition - dispFaceV0.mPosition ).Cross( dispFaceV2.mPosition - dispFaceV0.mPosition ) );

			const dedge_t * windEdge0 = NULL;
			const dedge_t * windEdge1 = NULL;

			// find the starting edge for the winding
			const float tolerance = 0.1f;
			if ( ( dispFaceV0.mPosition - dispInfo.mStartPosition ).Length() < tolerance )
			{
				windEdge0 = &edge3;
				windEdge1 = &edge1;
			}
			else if ( ( dispFaceV1.mPosition - dispInfo.mStartPosition ).Length() < tolerance )
			{
				windEdge0 = &edge0;
				windEdge1 = &edge2;
			}
			else if ( ( dispFaceV2.mPosition - dispInfo.mStartPosition ).Length() < tolerance )
			{
				windEdge0 = &edge1;
				windEdge1 = &edge3;
			}
			else if ( ( dispFaceV3.mPosition - dispInfo.mStartPosition ).Length() < tolerance )
			{
				windEdge0 = &edge2;
				windEdge1 = &edge0;
			}
			else
			{
				POW2_ASSERT( 0 && "Unexpected Start Position" );
				continue;
			}

			const Vector3f side0[ 2 ] =
			{
				bspmodel.mVertices[ windEdge0->mVertIndex[ 1 ] ].mPosition,
				bspmodel.mVertices[ windEdge0->mVertIndex[ 0 ] ].mPosition,
			};
			const Vector3f side1[ 2 ] =
			{
				bspmodel.mVertices[ windEdge1->mVertIndex[ 0 ] ].mPosition,
				bspmodel.mVertices[ windEdge1->mVertIndex[ 1 ] ].mPosition,
			};

			const float sideFracX = 1.0f / dispInfo.SubDivs();
			const float sideFracY = 1.0f / dispInfo.SubDivs();

			const int height = dispInfo.Height();
			const int width = dispInfo.Width();

			const Vector3f sZero( 0.f, 0.f, 0.f );

			const std::string modelName = "Displacement:" + boost::lexical_cast<std::string>( i );

			// generate a new model/node for the displacement
			NodePtr node( new Node() );
			node->mDisplacement = i;
			node->mModel.reset( new CollisionModel() );
			node->mModel->SetName( modelName );

			for ( int x = 1; x < height; ++x )
			{
				for ( int y = 1; y < width; ++y )
				{
					const Vector3f sPosMin = lerp( side0[ 0 ], side0[ 1 ], (float)( x - 1 )*sideFracX );
					const Vector3f sPosMax = lerp( side0[ 0 ], side0[ 1 ], (float)( x - 0 )*sideFracX );
					const Vector3f dPosMin = lerp( side1[ 0 ], side1[ 1 ], (float)( x - 1 )*sideFracX );
					const Vector3f dPosMax = lerp( side1[ 0 ], side1[ 1 ], (float)( x - 0 )*sideFracX );

					Vector3f v0pos = lerp( sPosMin, dPosMin, (float)( y - 1 )*sideFracY );
					Vector3f v1pos = lerp( sPosMin, dPosMin, (float)( y - 0 )*sideFracY );
					Vector3f v2pos = lerp( sPosMax, dPosMax, (float)( y - 0 )*sideFracY );
					Vector3f v3pos = lerp( sPosMax, dPosMax, (float)( y - 1 )*sideFracY );

					const ddispvert_t & v0 = bspmodel.mDisplacementVertices[ dispInfo.mDispVertStart + ( x - 1 ) + ( y - 1 ) * width ];
					const ddispvert_t & v1 = bspmodel.mDisplacementVertices[ dispInfo.mDispVertStart + ( x - 1 ) + ( y - 0 ) * width ];
					const ddispvert_t & v2 = bspmodel.mDisplacementVertices[ dispInfo.mDispVertStart + ( x - 0 ) + ( y - 0 ) * width ];
					const ddispvert_t & v3 = bspmodel.mDisplacementVertices[ dispInfo.mDispVertStart + ( x - 0 ) + ( y - 1 ) * width ];

					// offset the vertices
					/*Vector3f v0pos = dispInfo.mStartPosition;
					Vector3f v1pos = dispInfo.mStartPosition;
					Vector3f v2pos = dispInfo.mStartPosition;*/

					// offset the vertices
					const Vector3f offsetDirV0 = v0.mVecDir != sZero ? v0.mVecDir : originalNormal;
					const Vector3f offsetDirV1 = v1.mVecDir != sZero ? v1.mVecDir : originalNormal;
					const Vector3f offsetDirV2 = v2.mVecDir != sZero ? v2.mVecDir : originalNormal;
					const Vector3f offsetDirV3 = v3.mVecDir != sZero ? v3.mVecDir : originalNormal;

					POW2_ASSERT( offsetDirV0 != sZero );
					POW2_ASSERT( offsetDirV1 != sZero );
					POW2_ASSERT( offsetDirV2 != sZero );
					POW2_ASSERT( offsetDirV3 != sZero );

					v0pos += ( offsetDirV0 * v0.mDist );
					v1pos += ( offsetDirV1 * v1.mDist );
					v2pos += ( offsetDirV2 * v2.mDist );
					v3pos += ( offsetDirV3 * v3.mDist );

					node->mModel->AddTri( v0pos, v1pos, v2pos, surfaceMat );
					node->mModel->AddTri( v0pos, v2pos, v3pos, surfaceMat );
				}
			}

			node->mModel->Build( false );

			// Add it to the hierarchy
			scene.mCachedModels[ modelName ] = node->mModel;

			// Add it to its own node
			parent->mChildren.push_back( node );
		}
	}

	// ------------------------------------------------------------------------------------------------
	//	Creates all assimp nodes.
	void VBSPFileImporter::CreateSceneGraph( CollisionWorld & scene, NodePtr & parent, const VBSP::VBSPModel & bspmodel )
	{
		rmt_ScopedCPUSample( VBSPFileImporter_CreateSceneGraph );
		for ( size_t m = 0; m < bspmodel.mModels.size(); ++m )
		{
			const dmodel_t & subModel = bspmodel.mModels[ m ];
			
			// store it in the master list
			const std::string modelName = "SubModel:" + boost::lexical_cast<std::string>( m );
			
			NodePtr node( new Node() );
			node->mSubModel = m;
			node->mModel.reset( new CollisionModel() );
			node->mModel->SetName( modelName );

			WalkNodeTree( scene, node->mModel, bspmodel, m, subModel.mHeadNode );
			
			node->mModel->Build( false );

			scene.mCachedModels[ modelName ] = node->mModel;

			// Add a node to reference the loaded models.
			parent->mChildren.push_back( node );
		}

		GenDisplacements( scene, parent, bspmodel );

		// create nodes for all the static props who are externally referenced models
		CreateStaticNodes( scene, parent, bspmodel.mStaticPropLumpsV4, bspmodel );
		CreateStaticNodes( scene, parent, bspmodel.mStaticPropLumpsV5, bspmodel );
		CreateStaticNodes( scene, parent, bspmodel.mStaticPropLumps, bspmodel );
	}
	
	// ------------------------------------------------------------------------------------------------

}

