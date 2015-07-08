////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#ifndef VBSPFILEDATA_H_INC
#define VBSPFILEDATA_H_INC

#include <vector>

#include "Wm5Mathematics.h"
using namespace Wm5;

#include "CollisionModel.h"

namespace MapFormat
{
	namespace VBSP
	{
		// reference
		// https://developer.valvesoftware.com/wiki/Source_BSP_File_Format

		enum LumpTypes 
		{
			LUMP_ENTITIES = 0,							// Map entities
			LUMP_PLANES = 1,							// 	Plane array
			LUMP_TEXDATA = 2,							// Index to texture names
			LUMP_VERTEXES = 3,							// Vertex array
			LUMP_VISIBILITY = 4,						// Compressed visibility bit arrays
			LUMP_NODES = 5,								// BSP tree nodes
			LUMP_TEXINFO = 6,							// Face texture array
			LUMP_FACES = 7,								// Face array
			LUMP_LIGHTING = 8,							// Lightmap samples
			LUMP_OCCLUSION = 9,							// Occlusion polygons and vertices
			LUMP_LEAFS = 10,							// BSP tree leaf nodes
			LUMP_FACEIDS = 11,							// Correlates between dfaces and Hammer face IDs.Also used as random seed for detail prop placement.
			LUMP_EDGES = 12,							// Edge array
			LUMP_SURFEDGES = 13,						// Index of edges
			LUMP_MODELS = 14,							// Brush models( geometry of brush entities )
			LUMP_WORLDLIGHTS = 15,						// Internal world lights converted from the entity lump
			LUMP_LEAFFACES = 16,						// Index to faces in each leaf
			LUMP_LEAFBRUSHES = 17,						// Index to brushes in each leaf
			LUMP_BRUSHES = 18,							// Brush array
			LUMP_BRUSHSIDES = 19,						// Brushside array
			LUMP_AREAS = 20,							// Area array
			LUMP_AREAPORTALS = 21,						// Portals between areas
			LUMP_PORTALS = 22,							// Source 2004 : Polygons defining the boundary between adjacent leaves ?
			LUMP_UNUSED0 = 22,							// Source 2007
			LUMP_PROPCOLLISION = 22,					// Source 2009 : Static props convex hull lists
			LUMP_CLUSTERS = 23,							// Source 2004 : Leaves that are enterable by the player
			LUMP_UNUSED1 = 23,							// Source 2007 : Unused
			LUMP_PROPHULLS = 23,						// Source 2009 : Static prop convex hulls
			LUMP_PORTALVERTS = 24,						// Source 2004 : Vertices of portal polygons
			LUMP_UNUSED2 = 24,							// Source 2007 : Unused
			LUMP_PROPHULLVERTS = 24,					// Source 2009 : Static prop collision vertices
			LUMP_CLUSTERPORTALS = 25,					// Source 2004 : Polygons defining the boundary between adjacent clusters ?
			LUMP_UNUSED3 = 25,							// Source 2007 : Unused
			LUMP_PROPTRIS = 25,							// Source 2009 : Static prop per hull triangle index start / count
			LUMP_DISP_INFO = 26,							// Displacement surface array
			LUMP_ORIGINALFACES = 27,					// Brush faces array before splitting
			LUMP_PHYSDISP = 28,							// Displacement physics collision data
			LUMP_PHYSCOLLIDE = 29,						// Physics collision data
			LUMP_VERTNORMALS = 30,						// Face plane normals
			LUMP_VERTNORMALINDICES = 31,				// Face plane normal index array
			LUMP_DISP_LIGHTMAP_ALPHAS = 32,				// Displacement lightmap alphas( unused / empty since Source 2006 )
			LUMP_DISP_VERTS = 33,						// Vertices of displacement surface meshes
			LUMP_DISP_LIGHTMAP_SAMPLE_POSITIONS = 34,	// Displacement lightmap sample positions
			LUMP_GAME_LUMP = 35,						// Game - specific data lump
			LUMP_LEAFWATERDATA = 36,					// Data for leaf nodes that are inside water
			LUMP_PRIMITIVES = 37,						// Water polygon data
			LUMP_PRIMVERTS = 38,						// Water polygon vertices
			LUMP_PRIMINDICES = 39,						// Water polygon vertex index array
			LUMP_PAKFILE = 40,							// Embedded uncompressed Zip - format file
			LUMP_CLIPPORTALVERTS = 41,					// Clipped portal polygon vertices
			LUMP_CUBEMAPS = 42,							// env_cubemap location array
			LUMP_TEXDATA_STRING_DATA = 43,				// Texture name data
			LUMP_TEXDATA_STRING_TABLE = 44,				// Index array into texdata string data
			LUMP_OVERLAYS = 45,							// info_overlay data array
			LUMP_LEAFMINDISTTOWATER = 46,				// Distance from leaves to water
			LUMP_FACE_MACRO_TEXTURE_INFO = 47,			// Macro texture info for faces
			LUMP_DISP_TRIS = 48,						// Displacement surface triangles
			LUMP_PHYSCOLLIDESURFACE = 49,				// Source 2004 : Compressed win32 - specific Havok terrain surface collision data.Deprecated and no longer used.
			LUMP_PROP_BLOB = 49,						// Source 2009 : Static prop triangle and string data
			LUMP_WATEROVERLAYS = 50,					// Confirm : info_overlay's on water faces?
			LUMP_LIGHTMAPPAGES = 51,					// Source 2006 : Alternate lightdata implementation for Xbox
			LUMP_LEAF_AMBIENT_INDEX_HDR = 51,			// Source 2007 : Index of LUMP_LEAF_AMBIENT_LIGHTING_HDR
			LUMP_LIGHTMAPPAGEINFOS = 52,				// Source 2006 : Alternate lightdata indices for Xbox
			LUMP_LEAF_AMBIENT_INDEX = 52,				// Source 2007 : Index of LUMP_LEAF_AMBIENT_LIGHTING
			LUMP_LIGHTING_HDR = 53,						// HDR lightmap samples
			LUMP_WORLDLIGHTS_HDR = 54,					// Internal HDR world lights converted from the entity lump
			LUMP_LEAF_AMBIENT_LIGHTING_HDR = 55,		// Confirm : HDR related leaf lighting data ?
			LUMP_LEAF_AMBIENT_LIGHTING = 56,			// Confirm : HDR related leaf lighting data ?
			LUMP_XZIPPAKFILE = 57,						// XZip version of pak file for Xbox.Deprecated.
			LUMP_FACES_HDR = 58,						// HDR maps may have different face data
			LUMP_MAP_FLAGS = 59,						// Extended level - wide flags.Not present in all levels.
			LUMP_OVERLAY_FADES = 60,					// Fade distances for overlays
			LUMP_OVERLAY_SYSTEM_LEVELS = 61,			// System level settings( min / max CPU & GPU to render this overlay )
			LUMP_PHYSLEVEL = 62,						// To do
			LUMP_DISP_MULTIBLEND = 63
		};
		
		struct lump_t
		{
			int			mFileOfs;		// offset into file (bytes)
			int			mFileLen;		// length of lump (bytes)
			int			mVersion;		// lump format version
			char		mFourCC[ 4 ];	// lump ident code
		};

		typedef unsigned char byte;

		///	Fileheader
		static const int HEADER_LUMPS = 64;

		struct dvbspheader_t 
		{
			char		mIdent[ 4 ];	//!< Should be "VBSP"
			int			mVersion;	//!< 46 for standard levels
			lump_t		mLumps[ HEADER_LUMPS ];  // lump directory array
			int			mMapRevision; // the map's revision (iteration, version) number

			dvbspheader_t()
			{
				memset( this, 0, sizeof( dvbspheader_t ) );
			}

			bool IsValidID() const
			{
				return (
					mIdent[ 0 ] == 'V' &&
					mIdent[ 1 ] == 'B' &&
					mIdent[ 2 ] == 'S' &&
					mIdent[ 3 ] == 'P' );
			}
		};
		
		struct dplane_t 
		{
			Vector3f	mNormal;
			float		mDistance;
			int			mType;		// plane axis identifier
		};

		struct dedge_t
		{
			unsigned short	mVertIndex[ 2 ];	// vertex indices
		};

		struct dface_t
		{
			unsigned short		mPlaneNum;		// the plane number
			byte				mSide;			// faces opposite to the node's plane direction
			byte				mOnNode;		// 1 of on node, 0 if in leaf
			int					mFirstEdge;		// index into surfedges
			short				mNumEdges;		// number of surfedges
			short				mTexInfo;		// texture info
			short				mDispInfo;		// displacement info
			short				mSurfaceFogVolumeID;	// ?
			byte				mStyles[ 4 ];		// switchable lighting info
			int					mLightOfs;		// offset into lightmap lump
			float				mArea;			// face area in units^2
			int					mLightmapTextureMinsInLuxels[ 2 ];	// texture lighting info
			int					mLightmapTextureSizeInLuxels[ 2 ];	// texture lighting info
			int					mOrigFace;		// original face this was split from
			unsigned short		mNumPrims;		// primitives
			unsigned short		mFirstPrimID;
			unsigned int		mSmoothingGroups;	// lightmap smoothing group
		};

		struct dbrush_t
		{
			int					mFirstSide;		// first brushside
			int					mNumSides;		// number of brushsides
			int					mContents;		// contents flags
		};

		struct dbrushside_t
		{
			unsigned short		mPlaneNum;	// facing out of the leaf
			short				mTexInfo;	// texture info
			short				mDispInfo;	// displacement info
			short				mBevel;		// is the side a bevel plane?
		};
		struct dnode_t
		{
			int					mPlaneNum;	// index into plane array
			int					mChildren[ 2 ];	// negative numbers are -(leafs + 1), not nodes
			short				mMins[ 3 ];	// for frustum culling
			short				mMaxs[ 3 ];
			unsigned short		mFirstFace;	// index into face array
			unsigned short		mNumFaces;	// counting both sides
			short				mArea;		// If all leaves below this node are in the same area, then
			// this is the area index. If not, this is -1.
			short				mPadding;	// pad to 32 bytes length
		};
		struct dleaf_t
		{
			int					mContents;		// OR of all brushes (not needed?)
			short				mCluster;		// cluster this leaf is in
			short				mAarea : 9;			// area this leaf is in
			short				mFlags : 7;		// flags
			short				mMins[ 3 ];		// for frustum culling
			short				mMaxs[ 3 ];
			unsigned short		mFirstleafface;		// index into leaffaces
			unsigned short		mNumleaffaces;
			unsigned short		mFirstleafbrush;		// index into leafbrushes
			unsigned short		mNumleafbrushes;
			short				mLeafWaterDataID;	// -1 for not in water

			//!!! NOTE: for maps of version 19 or lower uncomment this block
			/*
			CompressedLightCube	ambientLighting;	// Precaculated light info for entities 24 bytes.
			short			padding;		// padding to 4-byte boundary
			*/
		};
		struct dtexinfo_t
		{
			float				mTextureVecs[ 2 ][ 4 ];		// [s/t][xyz offset]
			float				mLightmapVecs[ 2 ][ 4 ];	// [s/t][xyz offset] - length is in units of texels/area
			int					mFlags;						// miptex flags	overrides
			int					mTexdata;					// Pointer to texture name, size, etc.
		};

		struct dtexdata_t
		{
			Vector3f			mReflectivity;		// RGB reflectivity
			int					mNameStringTableID;	// index into TexdataStringTable
			int					mWidth, mHeight;		// source image
			int					mViewWidth;
			int					mViewHeight;
		};

		struct dtexstringtable_t
		{
			int					mStringDataOffset;		// offset into string table
		};

		struct dmodel_t
		{
			Vector3f			mMins, mMaxs;			// bounding box
			Vector3f			mOrigin;				// for sounds or lights
			int					mHeadNode;				// index into node array
			int					mFirstFace;
			int					mNumFaces;				// index into face array
		};

		///	Vertex of a Q3 level
		struct dvertex_t
		{
			Vector3f			mPosition;	///< Position of vertex			
		};
		
		struct dleafface_t
		{
			unsigned short		mFaceIndex;
		};
		struct dleafbrush_t
		{
			unsigned short		mBrushIndex;
		};

		struct dispsubneighbor_t
		{
			unsigned short		mNeighbor;			// Indexes into ddispinfos, 0xFFFF is none
			unsigned char		mNeighborOrient;	// (CCW) rotation of the neighbor
			unsigned char		mSpan;				// Where the neighbor fits onto this side of our displacement.
			unsigned char		mNeighborSpan;		// Where we fit onto our neighbor.
		};
		struct dispneighbor_t
		{
			dispsubneighbor_t	mSubNeighbor[ 2 ];
		};
		struct dispcornerneighbors_t
		{
			unsigned short	mNeighbors[ 4 ];		// indices of neighbors
			unsigned char	mNumNeighbors;
		};
		struct ddispinfo_t
		{
			int SubDivs() const
			{
				return ( ( 1 << mPower ) );
			}
			int Height() const
			{
				return ( ( 1 << mPower ) + 1 );
			}
			int Width() const
			{
				return ( ( 1 << mPower ) + 1 );
			}
			int NumTris() const 
			{
				return ( ( Height() - 1 ) * ( Width() - 1 ) * 2 );
			}

			Vector3f				mStartPosition;					// start position used for orientation
			int						mDispVertStart;					// Index into LUMP_DISP_VERTS. (2^mPower 1)^2
			int						mDispTriStart;					// Index into LUMP_DISP_TRIS. 2x(2^mPower)^2
			int						mPower;							// power - indicates size of surface (2^power	1)
			int						mMinTess;						// minimum tesselation allowed
			float					mSmoothingAngle;				// lighting smoothing angle
			int						mContents;						// surface contents
			unsigned short			mMapFace;						// Which map face this displacement comes from.
			int						mLightmapAlphaStart;			// Index into ddisplightmapalpha.
			int						mLightmapSamplePositionStart;	// Index into LUMP_DISP_LIGHTMAP_SAMPLE_POSITIONS.
			dispneighbor_t			mEdgeNeighbors[ 4 ];			// Indexed by NEIGHBOREDGE_ defines.
			dispcornerneighbors_t	mCornerNeighbors[ 4 ];			// Indexed by CORNER_ defines.
			unsigned int			mAllowedVerts[ 10 ];			// active verticies
		};

		struct ddispvert_t
		{
			Vector3f				mVecDir;						// Vector field defining displacement volume.
			float					mDist;							// Displacement distances.
			float					mAlpha;							// "per vertex" alpha values.
		};
		struct ddisptri_t
		{
			unsigned short			mTags;							// Displacement triangle tags.
		};

		struct dgamelump_staticpropdict_t
		{
			enum
			{
				NameLength = 128
			};
			char			mName[ NameLength ];
		};

		struct dgamelump_staticpropleaf_t
		{
			unsigned short	mLeafId;
		};

		struct dgamelump_color32
		{
			char r, g, b, a;
		};

		struct dgamelump_staticproplumpv4_t
		{
			Vector3f			mOrigin;
			Vector3f			mAngles;
			unsigned short		mPropType;
			unsigned short		mFirstLeaf;
			unsigned short		mLeafCount;
			unsigned char		mSolid;
			unsigned char		mFlags;
			int					mSkin;
			float				mFadeMinDist;
			float				mFadeMaxDist;
			Vector3f			mLightingOrigin;
		};

		struct dgamelump_staticproplumpv5_t
		{
			Vector3f			mOrigin;
			Vector3f			mAngles;
			unsigned short		mPropType;
			unsigned short		mFirstLeaf;
			unsigned short		mLeafCount;
			unsigned char		mSolid;
			unsigned char		mFlags;
			int					mSkin;
			float				mFadeMinDist;
			float				mFadeMaxDist;
			Vector3f			mLightingOrigin;
			float				mflForcedFadeScale;
		};

		struct dgamelump_staticproplump_t
		{
			Vector3f			mOrigin;
			Vector3f			mAngles;
			unsigned short		mPropType;
			unsigned short		mFirstLeaf;
			unsigned short		mLeafCount;
			unsigned char		mSolid;
			unsigned char		mFlags;
			int					mSkin;
			float				mFadeMinDist;
			float				mFadeMaxDist;
			Vector3f			mLightingOrigin;
			float				mflForcedFadeScale;
			unsigned short		mnMinDXLevel;
			unsigned short		mnMaxDXLevel;
		};

		struct MemoryReader
		{
			bool Seek( size_t offset )
			{
				if ( offset < mBufferSize )
				{
					mReadCursor = offset;
					return true;
				}
				return false;
			}

			bool Advance( size_t offset )
			{
				if ( mReadCursor + offset < mBufferSize )
				{
					mReadCursor += offset;
					return true;
				}
				return false;
			}

			size_t Read( void* dest, size_t sz, size_t count )
			{
				if ( mReadCursor + sz*count < mBufferSize )
				{
					memcpy( dest, &mBufferBase[ mReadCursor ], sz * count );
					mReadCursor += sz*count;
					return true;
				}
				return false;
			}

			const char * Ptr() const
			{
				return &mBufferBase[ mReadCursor ];
			}

			MemoryReader( const char * buffer, size_t bufferSize )
				: mBufferBase( buffer )
				, mBufferSize( bufferSize )
				, mReadCursor( 0 )
			{
			}
		private:
			const char *	mBufferBase;
			const size_t	mBufferSize;

			// volatile state
			size_t			mReadCursor;

			MemoryReader&operator=( const MemoryReader& other );
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

		struct VBSPModel
		{
			LumpData<dplane_t>							mPlanes;
			LumpData<dvertex_t>							mVertices;
			LumpData<dedge_t>							mEdges;
			LumpData<dface_t>							mFaces;
			LumpData<dface_t>							mOriginalFaces;
			LumpData<int>								mSurfEdges;
			LumpData<dbrush_t>							mBrushes;
			LumpData<dbrushside_t>						mBrushSides;
			LumpData<dnode_t>							mBspNodes;
			LumpData<dleaf_t>							mBspLeafs;
			LumpData<dleafface_t>						mBspLeafFaces;
			LumpData<dleafbrush_t>						mBspLeafBrushes;
			LumpData<dmodel_t>							mModels;
			LumpData<dtexinfo_t>						mTextureInfo;
			LumpData<dtexdata_t>						mTextureData;
			LumpData<dtexstringtable_t>					mTextureStringTable;

			LumpData<ddispinfo_t>						mDisplacementInfo;
			LumpData<ddispvert_t>						mDisplacementVertices;
			LumpData<ddisptri_t>						mDisplacementTris;
			
			LumpData<dgamelump_staticpropdict_t>		mStaticPropDict;
			LumpData<dgamelump_staticpropleaf_t>		mStaticPropLeafs;
			LumpData<dgamelump_staticproplumpv4_t>		mStaticPropLumpsV4;
			LumpData<dgamelump_staticproplumpv5_t>		mStaticPropLumpsV5;
			LumpData<dgamelump_staticproplump_t>		mStaticPropLumps;

			std::string									mTextureStringData;
			std::string									mEntityData;
			std::string									mModelName;
			
			// if the edge index is negative, the indices of the result will be swapped
			dedge_t GetOrientedEdge( int edgeIndex ) const;

			bool ReadBsp( const char * memoryBuffer, size_t memoryBufferSize );
		private:
			bool ParseGameLumps( const char * memoryBuffer, size_t memoryBufferSize, const lump_t & lump, int bspVersion );
		};

	}
}

#endif
