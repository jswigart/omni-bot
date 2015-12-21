#include "Q3BSPFileImporter.h"
#include "Q3BSPFileData.h"
#include "pow2assert.h"

#include <boost/lexical_cast.hpp>
#include <boost/filesystem/path.hpp>

namespace MapFormat
{
	namespace Q3BSP
	{
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

			const float dot = up.Dot( normal );
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

		// ------------------------------------------------------------------------------------------------
		bool FileImporter::ParseFile( CollisionWorld & scene, const char * memoryBuffer, size_t memoryBufferSize )
		{
			Q3BSP::Model bspModel;
			if ( memoryBuffer && bspModel.ReadBsp( memoryBuffer, memoryBufferSize ) )
			{
				CreateSceneGraph( scene, scene.mRootNode, bspModel );
				return true;
			}
			return false;
		}

		// ------------------------------------------------------------------------------------------------
		void FileImporter::CreateSceneGraph( CollisionWorld & scene, NodePtr & parent, const Q3BSP::Model & bspmodel )
		{
			rmt_ScopedCPUSample( FileImporter_CreateSceneGraph );
			for ( size_t m = 0; m < bspmodel.mSubModels.size(); ++m )
			{
				// store it in the master list
				const std::string modelName = "SubModel:" + boost::lexical_cast<std::string>( m );

				NodePtr node( new Node() );
				node->mSubModel = m;
				node->mModel.reset( new CollisionModel() );
				node->mModel->SetName( modelName );

				WalkNodeTree( scene, node->mModel, bspmodel, m );

				node->mModel->Build( false );

				scene.mCachedModels[ modelName ] = node->mModel;

				// Add a node to reference the loaded models.
				parent->mChildren.push_back( node );
			}

			//GenDisplacements( scene, parent, bspmodel );

			// create nodes for all the static props who are externally referenced models
			//CreateStaticNodes( scene, parent, bspmodel.mStaticPropLumpsV4, bspmodel );
			//CreateStaticNodes( scene, parent, bspmodel.mStaticPropLumpsV5, bspmodel );
			//CreateStaticNodes( scene, parent, bspmodel.mStaticPropLumps, bspmodel );
		}
		
		void FileImporter::WalkNodeTree( CollisionWorld & scene, ModelPtr geom, const Q3BSP::Model & bspmodel, int subModelIndex )
		{
			const SubModel & subModel = bspmodel.mSubModels[ subModelIndex ];
			for ( int b = 0; b < subModel.mBrushIndexNum; ++b )
			{
				const int brushIndex = subModel.mBrushIndexFirst + b;
				const Brushes & bspBrush = bspmodel.mBrushes[ brushIndex ];
				const Texture & bspTexture = bspmodel.mTextures[ bspBrush.mTextureId ];

				// generate a custom material for this surface				
				Material surfaceMat;
				surfaceMat.mName = bspTexture.mName;
				surfaceMat.mContents = (ContentFlags)gEngineFuncs->ConvertValue( bspTexture.mContents, IEngineInterface::ConvertContentsFlags, IEngineInterface::ConvertGameToBot );
				surfaceMat.mSurface = (SurfaceFlags)gEngineFuncs->ConvertValue( bspTexture.mSurfaceFlags, IEngineInterface::ConvertSurfaceFlags, IEngineInterface::ConvertGameToBot );

				if ( surfaceMat.mSurface & SURFACE_IGNORE )
					continue;

				for ( int s = 0; s < bspBrush.mNumSides; ++s )
				{
					const BrushSide & bspSide = bspmodel.mBrushSides[ bspBrush.mFirstSide + s ];
					const Plane & bspPlane = bspmodel.mPlanes[ bspSide.mPlaneIndex ];

					std::vector<Vector3f> sideVerts;
					CreatePolyForPlane( sideVerts, bspPlane.mNormal, bspPlane.mDistance );

					if ( sideVerts.size() == 0 )
						continue;

					// trim the polygon from the other brushes
					for ( int c = 0; c < bspBrush.mNumSides && !sideVerts.empty(); ++c )
					{
						const BrushSide & bspSide2 = bspmodel.mBrushSides[ bspBrush.mFirstSide + c ];

						if ( bspSide.mPlaneIndex == bspSide2.mPlaneIndex )
							continue;

						const Plane & bspPlane2 = bspmodel.mPlanes[ bspSide2.mPlaneIndex ^ 1 ];
						ClipPolyInPlace( sideVerts, bspPlane2.mNormal, bspPlane2.mDistance, 0.f );
					}

					if ( sideVerts.size() == 0 )
						continue;

					for ( size_t x = 1; x < sideVerts.size(); ++x )
					{
						const Vector3f vpos[ 3 ] = {
							sideVerts[ 0 ],
							sideVerts[ x - 1 ],
							sideVerts[ x ]
						};

						geom->AddTri( vpos[ 0 ], vpos[ 1 ], vpos[ 2 ], surfaceMat );
					}
				}
			}

			// find patches
			for ( int s = 0; s < subModel.mSurfaceIndexNum; ++s )
			{
				const Surface & bspSurface = bspmodel.mSurfaces[ subModel.mSurfaceIndexFirst + s ];
				const Texture & bspTexture = bspmodel.mTextures[ bspSurface.mTextureID ];

				Material surfaceMat;
				surfaceMat.mName = bspTexture.mName;
				surfaceMat.mContents = (ContentFlags)gEngineFuncs->ConvertValue( bspTexture.mContents, IEngineInterface::ConvertContentsFlags, IEngineInterface::ConvertGameToBot );
				surfaceMat.mSurface = (SurfaceFlags)gEngineFuncs->ConvertValue( bspTexture.mSurfaceFlags, IEngineInterface::ConvertSurfaceFlags, IEngineInterface::ConvertGameToBot );

				if ( surfaceMat.mSurface & SURFACE_IGNORE )
					continue;

				if ( subModel.mBrushIndexNum == 0 )
					continue;

				if ( bspSurface.mType != Patch )
					continue;
				
				for ( int i = 2; i < bspSurface.mNumVerts; i += 3 )
				{
					const Vertex & v0 = bspmodel.mVertices[ bspSurface.mFirstVert + i - 2 ];
					const Vertex & v1 = bspmodel.mVertices[ bspSurface.mFirstVert + i - 1 ];
					const Vertex & v2 = bspmodel.mVertices[ bspSurface.mFirstVert + i - 0 ];

					/*patchVerts[ i ].mPos = vpos;
					patchVerts[ i ].mTexel1 = coord0;
					patchVerts[ i ].mTexel2 = coord1;*/
					const Vector3f normal = ( v1.mPosition - v0.mPosition ).UnitCross( v2.mPosition - v0.mPosition );

					geom->AddTri( v0.mPosition, v1.mPosition, v2.mPosition, surfaceMat );
				}
			}
		}
	};
}
