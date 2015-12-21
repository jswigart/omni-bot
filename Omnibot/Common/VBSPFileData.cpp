////////////////////////////////////////////////////////////////////////////////
//
// $LastChangedBy$
// $LastChangedDate$
// $LastChangedRevision$
//
////////////////////////////////////////////////////////////////////////////////

#include "VBSPFileData.h"
#include "FileSystem.h"

namespace MapFormat
{
namespace VBSP
{
	template<typename T>
	bool CopyLump( const char * memoryBuffer, size_t memoryBufferSize, LumpData<T> & lst, const lump_t & lump, int bspVersion )
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

	// specialized to handled dleaf_t versioning
	bool CopyLump( const char * memoryBuffer, size_t memoryBufferSize, LumpData<dleaf_t> & lst, const lump_t & lump, int bspVersion )
	{
		size_t stride = sizeof( LumpData<dleaf_t>::value_type );

		// size may vary by bsp version
		switch ( bspVersion )
		{
			case 19:
			{
				const size_t version19TrashSize = 24;
				stride += version19TrashSize;
				break;
			}
		}

		const size_t count = lump.mFileLen / stride;

		lst.Set( &memoryBuffer[ lump.mFileOfs ], count, stride );		
		
		return true;
	}

	bool CopyStringLump( const char * memoryBuffer, size_t memoryBufferSize, std::string & str, const lump_t & lump, int bspVersion )
	{
		if ( (int)memoryBufferSize - lump.mFileOfs < lump.mFileLen )
			return false;

		str.assign( &memoryBuffer[ lump.mFileOfs ], lump.mFileLen );
		return true;
	}

	bool VBSPModel::ParseGameLumps( const char * memoryBuffer, size_t memoryBufferSize, const lump_t & lump, int bspVersion )
	{
		if ( lump.mFileLen == 0 )
			return false;
		
		MemoryReader io( memoryBuffer, memoryBufferSize );

		if ( !io.Seek( lump.mFileOfs ) )
			return false;

		struct dgamelump_t
		{
			int					mId;		
			unsigned short		mFlags;		
			unsigned short		mVersion;	
			int					mFileOffset;
			int					mFilelen;	
		};
		
		int gameLumpCount = 0;
		if ( io.Read( &gameLumpCount, sizeof( gameLumpCount ), 1 ) != 1 )
			return false;

		if ( gameLumpCount > 0 )
		{
			const int sStaticPropLumpId = 'sprp';

			LumpData<dgamelump_t> gameLumps;
			gameLumps.Set( io.Ptr(), gameLumpCount, sizeof( dgamelump_t ) );
			//io.Read( &gameLumps[ 0 ], sizeof( dgamelump_t ), gameLumpCount );
			io.Advance( gameLumpCount * sizeof( dgamelump_t ) );
			
			// we need the static prop info
			for ( size_t i = 0; i < gameLumps.size(); ++i )
			{
				const dgamelump_t & gamelump = gameLumps[ i ];
				if ( gamelump.mId == sStaticPropLumpId )
				{
					if ( !io.Seek( gameLumps[ i ].mFileOffset ) )
						return false;

					int numdicts = 0;
					if ( io.Read( &numdicts, sizeof( numdicts ), 1 ) != 1 )
						continue;

					mStaticPropDict.Set( io.Ptr(), numdicts, sizeof( dgamelump_staticpropdict_t ) );
					if ( !io.Advance( numdicts * sizeof( dgamelump_staticpropdict_t ) ) )
						continue;
					/*mStaticPropDict.resize( numdicts );
					if ( io.Read( &mStaticPropDict[ 0 ], sizeof( dgamelump_staticpropdict_t ), numdicts ) != numdicts )
						continue;*/

					int numleafs = 0;
					if ( io.Read( &numleafs, sizeof( numleafs ), 1 ) != 1 )
						continue;

					mStaticPropLeafs.Set( io.Ptr(), numleafs, sizeof( dgamelump_staticpropleaf_t ) );
					if ( !io.Advance( numleafs * sizeof( dgamelump_staticpropleaf_t ) ) )
						continue;
					/*mStaticPropLeafs.resize( numleafs );
					if ( io.Read( &mStaticPropLeafs[ 0 ], sizeof( dgamelump_staticpropleaf_t ), numleafs ) != numleafs )
						continue;*/

					int numstaticlumps = 0;
					if ( io.Read( &numstaticlumps, sizeof( numstaticlumps ), 1 ) != 1 )
						continue;

					if ( gamelump.mVersion == 4 )
					{
						mStaticPropLumpsV4.Set( io.Ptr(), numstaticlumps, sizeof( dgamelump_staticproplumpv4_t ) );
						if ( !io.Advance( numstaticlumps * sizeof( dgamelump_staticproplumpv4_t ) ) )
							continue;

						/*mStaticPropLumpsV4.resize( numstaticlumps );
						if ( io.Read( &mStaticPropLumpsV4[ 0 ], sizeof( dgamelump_staticproplumpv4_t ), numstaticlumps ) != numstaticlumps )
							continue;*/
					}
					else if ( gameLumps[ i ].mVersion == 5 )
					{
						mStaticPropLumpsV5.Set( io.Ptr(), numstaticlumps, sizeof( dgamelump_staticproplumpv5_t ) );
						if ( !io.Advance( numstaticlumps * sizeof( dgamelump_staticproplumpv5_t ) ) )
							continue;

						/*mStaticPropLumpsV5.resize( numstaticlumps );
						if ( io.Read( &mStaticPropLumpsV5[ 0 ], sizeof( dgamelump_staticproplumpv5_t ), numstaticlumps ) != numstaticlumps )
							continue;*/
					}
					else
					{
						mStaticPropLumps.Set( io.Ptr(), numstaticlumps, sizeof( dgamelump_staticproplump_t ) );
						if ( !io.Advance( numstaticlumps * sizeof( dgamelump_staticproplump_t ) ) )
							continue;

						/*mStaticPropLumps.resize( numstaticlumps );
						if ( io.Read( &mStaticPropLumps[ 0 ], sizeof( dgamelump_staticproplump_t ), numstaticlumps ) != numstaticlumps )
						continue;*/
					}

				}
			}
		}
		return true;
	}

	dedge_t VBSPModel::GetOrientedEdge( int edgeIndex ) const
	{
		dedge_t edge = mEdges[ std::abs( edgeIndex ) ];
		if ( edgeIndex < 0 )
			std::swap( edge.mVertIndex[ 0 ], edge.mVertIndex[ 1 ] );
		return edge;
	}

	bool VBSPModel::ReadBsp( const char * memoryBuffer, size_t memoryBufferSize )
	{
		rmt_ScopedCPUSample( VBSPModel_ReadBsp );

		const dvbspheader_t* vbspHeader = (const dvbspheader_t*)memoryBuffer;
		
		if ( !vbspHeader->IsValidID() )
		{
			return false;
		}
		
		CopyStringLump( memoryBuffer, memoryBufferSize, mEntityData, vbspHeader->mLumps[ LUMP_ENTITIES ], vbspHeader->mVersion );
		CopyStringLump( memoryBuffer, memoryBufferSize, mTextureStringData, vbspHeader->mLumps[ LUMP_TEXDATA_STRING_DATA ], vbspHeader->mVersion );
		CopyLump( memoryBuffer, memoryBufferSize, mVertices, vbspHeader->mLumps[ LUMP_VERTEXES ], vbspHeader->mVersion );
		CopyLump( memoryBuffer, memoryBufferSize, mPlanes, vbspHeader->mLumps[ LUMP_PLANES ], vbspHeader->mVersion );
		CopyLump( memoryBuffer, memoryBufferSize, mSurfEdges, vbspHeader->mLumps[ LUMP_SURFEDGES ], vbspHeader->mVersion );
		CopyLump( memoryBuffer, memoryBufferSize, mEdges, vbspHeader->mLumps[ LUMP_EDGES ], vbspHeader->mVersion );
		CopyLump( memoryBuffer, memoryBufferSize, mFaces, vbspHeader->mLumps[ LUMP_FACES ], vbspHeader->mVersion );
		CopyLump( memoryBuffer, memoryBufferSize, mOriginalFaces, vbspHeader->mLumps[ LUMP_ORIGINALFACES ], vbspHeader->mVersion );
		CopyLump( memoryBuffer, memoryBufferSize, mBrushes, vbspHeader->mLumps[ LUMP_BRUSHES ], vbspHeader->mVersion );
		CopyLump( memoryBuffer, memoryBufferSize, mBrushSides, vbspHeader->mLumps[ LUMP_BRUSHSIDES ], vbspHeader->mVersion );
		CopyLump( memoryBuffer, memoryBufferSize, mBspNodes, vbspHeader->mLumps[ LUMP_NODES ], vbspHeader->mVersion );
		CopyLump( memoryBuffer, memoryBufferSize, mBspLeafs, vbspHeader->mLumps[ LUMP_LEAFS ], vbspHeader->mVersion );
		CopyLump( memoryBuffer, memoryBufferSize, mBspLeafFaces, vbspHeader->mLumps[ LUMP_LEAFFACES ], vbspHeader->mVersion );
		CopyLump( memoryBuffer, memoryBufferSize, mBspLeafBrushes, vbspHeader->mLumps[ LUMP_LEAFBRUSHES ], vbspHeader->mVersion );
		CopyLump( memoryBuffer, memoryBufferSize, mModels, vbspHeader->mLumps[ LUMP_MODELS ], vbspHeader->mVersion );
		CopyLump( memoryBuffer, memoryBufferSize, mTextureInfo, vbspHeader->mLumps[ LUMP_TEXINFO ], vbspHeader->mVersion );
		CopyLump( memoryBuffer, memoryBufferSize, mTextureData, vbspHeader->mLumps[ LUMP_TEXDATA ], vbspHeader->mVersion );
		CopyLump( memoryBuffer, memoryBufferSize, mTextureStringTable, vbspHeader->mLumps[ LUMP_TEXDATA_STRING_TABLE ], vbspHeader->mVersion );
		CopyLump( memoryBuffer, memoryBufferSize, mDisplacementInfo, vbspHeader->mLumps[ LUMP_DISP_INFO ], vbspHeader->mVersion );
		CopyLump( memoryBuffer, memoryBufferSize, mDisplacementVertices, vbspHeader->mLumps[ LUMP_DISP_VERTS ], vbspHeader->mVersion );
		CopyLump( memoryBuffer, memoryBufferSize, mDisplacementTris, vbspHeader->mLumps[ LUMP_DISP_TRIS ], vbspHeader->mVersion );
		ParseGameLumps( memoryBuffer, memoryBufferSize, vbspHeader->mLumps[ LUMP_GAME_LUMP ], vbspHeader->mVersion );
		
		return true;
	}

}
}
