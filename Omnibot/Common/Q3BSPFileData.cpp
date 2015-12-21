
#include "Q3BSPFileData.h"

namespace MapFormat
{
	namespace Q3BSP
	{
		template<typename T>
		bool CopyLump( const char * memoryBuffer, size_t memoryBufferSize, LumpData<T> & lst, const Lump & lump, int bspVersion )
		{
			if ( lump.mFileLen > 0 )
			{
				const size_t stride = sizeof( LumpData<T>::value_type );
				const size_t count = lump.mFileLen / stride;

				lst.Set( &memoryBuffer[ lump.mFileOfs ], count, stride );
				return true;
			}
			return false;
		}
		
		bool CopyStringLump( const char * memoryBuffer, size_t memoryBufferSize, std::string & str, const Lump & lump, int bspVersion )
		{
			if ( (int)memoryBufferSize - lump.mFileOfs < lump.mFileLen )
				return false;

			str.assign( &memoryBuffer[ lump.mFileOfs ], lump.mFileLen );
			return true;
		}

		bool Model::ReadBsp( const char * memoryBuffer, size_t memoryBufferSize )
		{
			rmt_ScopedCPUSample( VBSPModel_ReadBsp );

			const Header* bspHeader = (const Header*)memoryBuffer;
			if ( !bspHeader->IsValidID() )
			{
				return false;
			}
			
			CopyStringLump( memoryBuffer, memoryBufferSize, mEntityData, bspHeader->mLumps[ LUMP_ENTITIES ], bspHeader->mVersion );
			CopyLump( memoryBuffer, memoryBufferSize, mTextures, bspHeader->mLumps[ LUMP_TEXTURES ], bspHeader->mVersion );
			CopyLump( memoryBuffer, memoryBufferSize, mPlanes, bspHeader->mLumps[ LUMP_PLANES ], bspHeader->mVersion );
			CopyLump( memoryBuffer, memoryBufferSize, mBspNodes, bspHeader->mLumps[ LUMP_NODES ], bspHeader->mVersion );
			CopyLump( memoryBuffer, memoryBufferSize, mBspLeafs, bspHeader->mLumps[ LUMP_LEAFS ], bspHeader->mVersion );
			CopyLump( memoryBuffer, memoryBufferSize, mBspLeafFaces, bspHeader->mLumps[ LUMP_FACES ], bspHeader->mVersion );
			CopyLump( memoryBuffer, memoryBufferSize, mBspLeafBrushes, bspHeader->mLumps[ LUMP_LEAFBRUSHES ], bspHeader->mVersion );
			CopyLump( memoryBuffer, memoryBufferSize, mSubModels, bspHeader->mLumps[ LUMP_SUBMODELS ], bspHeader->mVersion );
			CopyLump( memoryBuffer, memoryBufferSize, mBrushes, bspHeader->mLumps[ LUMP_BRUSHES ], bspHeader->mVersion );
			CopyLump( memoryBuffer, memoryBufferSize, mBrushSides, bspHeader->mLumps[ LUMP_BRUSHSIDES ], bspHeader->mVersion );
			CopyLump( memoryBuffer, memoryBufferSize, mVertices, bspHeader->mLumps[ LUMP_VERTICES ], bspHeader->mVersion );
			CopyLump( memoryBuffer, memoryBufferSize, mIndices, bspHeader->mLumps[ LUMP_MESHVERTICES ], bspHeader->mVersion );
			//kShaders
			CopyLump( memoryBuffer, memoryBufferSize, mSurfaces, bspHeader->mLumps[ LUMP_SURFACES ], bspHeader->mVersion );
			CopyLump( memoryBuffer, memoryBufferSize, mLightmaps, bspHeader->mLumps[ LUMP_LIGHTMAPS ], bspHeader->mVersion );
		
			return true;
		}
	}
}
