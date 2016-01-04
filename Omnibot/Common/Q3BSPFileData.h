/*
Open Asset Import Library (assimp)
----------------------------------------------------------------------

Copyright (c) 2006-2008, assimp team
All rights reserved.

Redistribution and use of this software in source and binary forms,
with or without modification, are permitted provided that the
following conditions are met:

* Redistributions of source code must retain the above
copyright notice, this list of conditions and the
following disclaimer.

* Redistributions in binary form must reproduce the above
copyright notice, this list of conditions and the
following disclaimer in the documentation and/or other
materials provided with the distribution.

* Neither the name of the assimp team, nor the names of its
contributors may be used to endorse or promote products
derived from this software without specific prior
written permission of the assimp team.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

----------------------------------------------------------------------
*/
#ifndef Q3BSPFILEDATA_H
#define Q3BSPFILEDATA_H

#include <vector>

#include "Wm5Mathematics.h"
using namespace Wm5;

#include "CollisionModel.h"

namespace MapFormat
{
	namespace Q3BSP
	{
		// reference
		// http://www.mralligator.com/q3/

		enum Lumps
		{
			LUMP_ENTITIES = 0,
			LUMP_TEXTURES,
			LUMP_PLANES,
			LUMP_NODES,
			LUMP_LEAFS,
			LUMP_FACES,
			LUMP_LEAFBRUSHES,
			LUMP_SUBMODELS,
			LUMP_BRUSHES,
			LUMP_BRUSHSIDES,
			LUMP_VERTICES,
			LUMP_MESHVERTICES,
			LUMP_SHADERS,
			LUMP_SURFACES,
			LUMP_LIGHTMAPS,
			LUMP_LIGHTVOLUMES,
			LUMP_VISDATA,

			LUMP_MAX
		};

		static const unsigned int CE_BSP_LIGHTMAPWIDTH = 128;
		static const unsigned int CE_BSP_LIGHTMAPHEIGHT = 128;

		static const unsigned int CE_BSP_LIGHTMAPSIZE = 128 * 128 * 3;	///< = 128( width ) * 128 ( height ) * 3 ( channels / RGB ).
		static const int VERION_Q3LEVEL = 46;						///< Supported version.

		///	Geometric type enumeration
		enum GeoType
		{
			Polygon = 1,
			Patch,
			TriangleMesh,
			Billboard
		};

		template<typename T>
		struct LumpData
		{
			typedef T value_type;

			size_t size() const
			{
				return mCount;
			}

			const T& operator[]( size_t i ) const
			{
				const T* data = (const T*)( (const char*)mArray + mStride * i );
				return *data;
			}

			void Set( const char * data, size_t count, size_t stride )
			{
				mArray = (const T*)data;
				mCount = count;
				mStride = stride;
			}

			LumpData()
				: mArray( NULL )
				, mCount( 0 )
				, mStride( 0 )
			{
			}
		private:
			const T*	mArray;
			size_t		mCount;
			size_t		mStride;
		};

		struct Lump
		{
			int					mFileOfs;
			int					mFileLen;
		};

		///	Fileheader
		struct Header
		{
			char		mIdent[ 4 ];	//!< Should be "IBSP"
			int			mVersion;		//!< 46 for standard levels
			Lump		mLumps[ LUMP_MAX ];

			Header()
			{
				memset( this, 0, sizeof( Header ) );
			}

			bool IsValidID() const
			{
				return (
					mIdent[ 0 ] == 'I' &&
					mIdent[ 1 ] == 'B' &&
					mIdent[ 2 ] == 'S' &&
					mIdent[ 3 ] == 'P' );
			}
		};

		struct Plane
		{
			Vector3f			mNormal;
			float				mDistance;
		};

		///	Vertex of a Q3 level
		struct Vertex
		{
			Vector3f			mPosition;				///< Position of vertex
			Vector2f			mTexCoord;				///< (u,v) Texturecoordinate of detailtexture
			Vector2f			mLightmap;				///< (u,v) Texturecoordinate of lightmap
			Vector3f			mNormal;				///< vertex normale
			unsigned char		mColor[ 4 ];			///< Color in RGBA
		};

		///	A face in bsp format info
		struct Surface
		{
			int					mTextureID;					///< Index in texture array
			int					mEffect;					///< Index in effectarray (-1 = no effect)
			int					mType;						///< 1=Polygon, 2=Patch, 3=Mesh, 4=Billboard
			int					mFirstVert;					///< Start index of polygon
			int					mNumVerts;					///< Number of vertices
			int					mFirstIndex;				///< Index of first mesh vertex
			int					mNumIndices;				///< Anzahl der Meshvertices
			int					mLightmapID;				///< Index to the lightmap array
			int					mLMapCorner[ 2 ];			///< Die Ecke der Lightmap in der Textur
			int					mLMapSize[ 2 ];				///< Size of the lightmap stored on the texture
			Vector3f			mLMapPos;					///< 3D-Ursprung der Lightmap
			Vector3f			mLMapVecs[ 2 ];				///< 3D-s-t-Vektoren
			Vector3f			mNormal;					///< Polygonnormale
			int					mPatchWidth;
			int					mPatchHeight;
		};

		/// A quake3 texture name.
		struct Texture
		{
			char				mName[ 64 ];
			int					mSurfaceFlags;
			int					mContents;
		};

		///	A lightmap of the level, size 128 x 128, RGB components.
		struct Lightmap
		{
			unsigned char bLMapData[ CE_BSP_LIGHTMAPSIZE ];
			Lightmap()
			{
				memset( bLMapData, 0, CE_BSP_LIGHTMAPSIZE );
			}
		};

		struct SubModel
		{
			Vector3f			mMins;
			Vector3f			mMaxs;
			int					mSurfaceIndexFirst;
			int					mSurfaceIndexNum;
			int					mBrushIndexFirst;
			int					mBrushIndexNum;
		};

		struct Brushes
		{
			int					mFirstSide;
			int					mNumSides;
			int					mTextureId;
		};

		struct BrushSide
		{
			int					mPlaneIndex;
			int					mTextureId;
		};

		struct BspNode
		{
			int					mPlaneNum;			// Plane index.
			int					mChildren[ 2 ];		// Children indices. Negative numbers are leaf indices: -(leaf+1).
			int					mMins[ 3 ];			// Integer bounding box min coord.
			int					mMaxs[ 3 ];			// Integer bounding box max coord.
		};

		struct BspLeaf
		{
			int					mCluster;			// Visdata cluster index.
			int					mArea;				// Areaportal area.
			int					mMins[ 3 ];			// Integer bounding box min coord.
			int					mMaxs[ 3 ];			// Integer bounding box max coord.
			unsigned int		mFirstleafface;		// index into leaffaces
			unsigned int		mNumleaffaces;
			unsigned int		mFirstleafbrush;	// index into leafbrushes
			unsigned int		mNumleafbrushes;
		};

		struct LeafFace
		{
			int					mFaceIndex;
		};

		struct LeafBrush
		{
			int					mBrushIndex;
		};

		struct SubPatch
		{
			std::vector<size_t>	mIndices;
			int					mLightmapId;
		};

		struct Model
		{
			LumpData<Lump>				mLumps;
			LumpData<Plane>				mPlanes;
			LumpData<Vertex>			mVertices;
			LumpData<Surface>			mSurfaces;
			LumpData<int>				mIndices;
			LumpData<Texture>			mTextures;
			LumpData<Lightmap>			mLightmaps;
			LumpData<SubModel>			mSubModels;
			LumpData<Brushes>			mBrushes;
			LumpData<BrushSide>			mBrushSides;
			LumpData<BspNode>			mBspNodes;
			LumpData<BspLeaf>			mBspLeafs;
			LumpData<LeafFace>			mBspLeafFaces;
			LumpData<LeafBrush>			mBspLeafBrushes;
			std::string					mEntityData;
			std::string					mModelName;

			/*size_t countVertices( const std::vector<size_t> & faceIndices ) const;
			size_t countFaces( const std::vector<size_t> & faceIndices ) const;
			size_t countTriangles( const std::vector<size_t> & faceIndices ) const;*/
			bool ReadBsp( const char * memoryBuffer, size_t memoryBufferSize );
		};
	};
};

#endif
