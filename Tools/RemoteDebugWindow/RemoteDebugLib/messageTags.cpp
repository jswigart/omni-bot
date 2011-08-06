#include <assert.h>

#include "messageTags.h"
#include <string>

#include "dataBuffer.h"

namespace RemoteLib
{
	RemoteEntInfo::RemoteEntInfo() 
		: serial( 0 )
		, flags( 0 ) {
		name[0] = tooltip[0] = 0;
	}

	void RemoteEntInfo::Free() {
		uint16 ser = serial;
		*this = RemoteEntInfo();
		serial = ser+1;
	}

	void RemoteEntInfo::SetName( const char * newName ) {
		flags |= INFO_SYNC_NAME;
		strncpy( name, newName, BufferSz );
	}
	void RemoteEntInfo::SetToolTip( const char * newTooltip ) {
		flags |= INFO_SYNC_TOOLTIP;
		strncpy( tooltip, newTooltip, BufferSz );
	}
	void RemoteEntInfo::SetImage( const char * newImage ) {
		flags |= INFO_SYNC_IMAGE;
		strncpy( image, newImage, BufferSz );
	}
	void RemoteEntInfo::SetTransform( float newX, float newY, float newYaw ) {
		flags |= INFO_SYNC_TRANSFORM;
		posX = newX;
		posY = newY;
		yaw = newYaw;
	}
	void RemoteEntInfo::SetSize( float newSizeX, float newSizeY ) {
		flags |= INFO_SYNC_SIZE;
		sizeX = newSizeX;
		sizeY = newSizeY;
	}
	
	class RemoteObjectDbImpl {
	public:
		RemoteEntHandle GetHandle( const RemoteEntInfo & info ) {
			return (RemoteEntHandle)((info.serial<<16) | (&info - entities));
		}

		RemoteEntHandle AllocEntity( const char * name, const char * tooltip ) {
			for( uint32 i = 0; i < maxEntities; ++i ) {
				if ( !(entities[ i ].flags & INFO_INUSE) ) {
					entities[ i ].SetName( name );
					entities[ i ].SetToolTip( tooltip );
					return GetHandle( entities[ i ] );
				}
			}
			return (RemoteEntHandle)0;
		}

		void DeleteEntity( RemoteEntHandle hndl ) {
			RemoteEntInfo * info = GetEntityInfo( hndl );
			if ( info ) {
				info->flags |= INFO_DELETE;
			}
		}

		RemoteEntInfo * GetEntityInfo( RemoteEntHandle hndl, bool alloc = false ) {
			const uint16 index = hndl & 0xFF;
			const uint16 serial = hndl >> 16;
			if ( index < maxEntities && entities[ index ].serial == serial ) {
				return &entities[ index ];
			} else if ( alloc ) {
				entities[ index ].Free();
				entities[ index ].serial = serial;
				return &entities[ index ];
			}
			return 0;
		}

		void SyncWrite( DataBuffer & db ) {
			enum { StaticBufferSize = 2048 };
			char staticBuffer[ StaticBufferSize ] = {};

			{
				// send all deletes first
				DataBuffer localDb( staticBuffer, StaticBufferSize );
				
				for( uint32 i = 0; i < maxEntities; ++i ) {
					if ( (entities[ i ].flags & INFO_DELETE) ) {
						localDb.writeInt32( GetHandle( entities[ i ] ) );
					}
				}

				if ( localDb.getBytesWritten() > 0 ) {
					db.beginWrite( DataBuffer::WriteModeAllOrNone );
					db.startSizeHeader();
					db.writeInt32( ID_entityDelete );
					db.writeInt32( localDb.getBytesWritten() );
					db.write( staticBuffer, localDb.getBytesWritten() );
					db.endSizeHeader();
					const uint32 errors = db.endWrite();
					if ( errors == 0 ) {
						for( uint32 i = 0; i < maxEntities; ++i ) {
							entities[ i ].Free();
						}
					}
				}
			}
			
			// sync entity changes
			for( uint32 i = 0; i < maxEntities; ++i ) {
				RemoteEntInfo * info = &entities[ i ];
				if ( info->flags & INFO_SYNC_BITS ) {
					db.beginWrite( DataBuffer::WriteModeAllOrNone );
					db.startSizeHeader();
					db.writeInt32( ID_entityInfo );
					db.writeInt32( GetHandle( entities[ i ] ) );

					if ( info->flags & INFO_SYNC_NAME ) {
						db.writeInt32( ID_entityName );
						db.writeSmallString( info->name );
					}
					if ( info->flags & INFO_SYNC_TOOLTIP ) {
						db.writeInt32( ID_entityTooltip );
						db.writeSmallString( info->tooltip );
					}
					if ( info->flags & INFO_SYNC_IMAGE ) {
						db.writeInt32( ID_entityImage );
						db.writeSmallString( info->image );
					}
					if ( info->flags & INFO_SYNC_TRANSFORM ) {
						db.writeInt32( ID_entityTransform );
						db.writeFloat16( info->posX );
						db.writeFloat16( info->posY );
						db.writeFloat16( info->yaw );
					}
					if ( info->flags & INFO_SYNC_SIZE ) {
						db.writeInt32( ID_entitySize );
						db.writeFloat16( info->sizeX );
						db.writeFloat16( info->sizeY );
					}

					db.endSizeHeader();
					const uint32 errors = db.endWrite();
					if ( errors == 0 ) {
						entities[ i ].flags &= ~INFO_SYNC_BITS;
					}
				}
			}
		}

		void SyncRead( DataBuffer & db ) {
			db.setAssertOnError();
			while( db.getReadBytesLeft() >= 4 ) {
				const uint32 baseBytes = db.getBytesRead();
				db.beginRead( RemoteLib::DataBuffer::ReadModeAllOrNone );

				int32 tagId = 0, blockSize = 0;
				db.readInt32( blockSize );

				if ( !db.canRead( blockSize ) ) {
					db.endRead( true );
					break;
				}

				db.readInt32( tagId );

				switch( tagId )
				{
				case RemoteLib::ID_entityDelete:
					{
						int32 bytes = 0;
						db.readInt32( bytes );
						assert( bytes % sizeof(uint32) == 0 );
						const uint32 numDeletes = bytes / sizeof(uint32);
						for ( uint32 i = 0; i < numDeletes; ++i ) {
							int32 hndl = 0;
							db.readInt32( hndl );
							RemoteEntInfo * info = GetEntityInfo( (RemoteEntHandle)hndl );
							if ( info ) {
								info->flags |= INFO_DELETE;
							}
						}
						break;
					}
				case RemoteLib::ID_entityInfo:
					{
						int32 infoHandle = 0;
						db.readInt32( infoHandle );

						RemoteEntInfo * info = GetEntityInfo( infoHandle, true );

						while( db.getBytesRead() - baseBytes < (uint32)blockSize+4 ) {
							int32 subTag = 0;
							db.readInt32( subTag );
							switch ( subTag ) 
							{
							case ID_entityName:
								info->flags |= INFO_SYNC_NAME;
								db.readSmallString( info->name, RemoteEntInfo::BufferSz );
								break;
							case ID_entityTooltip:
								info->flags |= INFO_SYNC_TOOLTIP;
								db.readSmallString( info->tooltip, RemoteEntInfo::BufferSz );
								break;
							case ID_entityImage:
								info->flags |= INFO_SYNC_IMAGE;
								db.readSmallString( info->image, RemoteEntInfo::BufferSz );
								break;
							case ID_entityTransform:
								info->flags |= INFO_SYNC_TRANSFORM;
								db.readFloat16( info->posX );
								db.readFloat16( info->posY );
								db.readFloat16( info->yaw );
								break;
							case ID_entitySize:
								info->flags |= INFO_SYNC_SIZE;
								db.readFloat16( info->sizeX );
								db.readFloat16( info->sizeY );
								break;
							}
						}
						break;
					}
				default:
					assert( 0 && "unhandled message type" );
					break;
				}

				const uint32 blockBytesRead = db.getBytesRead() - (uint32)baseBytes;
				blockBytesRead;
				assert( blockBytesRead == (uint32)blockSize+4 && "databuffer read error" );
				db.endRead();
			}
		}

		RemoteObjectDbImpl( uint32 maxEntities )
			: maxEntities( maxEntities ) {
			entities = new RemoteEntInfo[ maxEntities ];
		}
	private:
		const uint32		maxEntities;
		RemoteEntInfo *		entities;
	};

	//////////////////////////////////////////////////////////////////////////

	RemoteObjectDb::RemoteObjectDb( uint32 maxEntities ) {
		impl = new RemoteObjectDbImpl( maxEntities );
	}

	RemoteObjectDb::~RemoteObjectDb() {
		delete impl;
	}

	RemoteEntHandle RemoteObjectDb::AllocEntity( const char * name, const char * tooltip ) {
		return impl->AllocEntity( name, tooltip );
	}

	void RemoteObjectDb::DeleteEntity( RemoteEntHandle hndl ) {
		impl->DeleteEntity( hndl );
	}

	RemoteEntInfo * RemoteObjectDb::GetEntityInfo( RemoteEntHandle hndl ) {
		return impl->GetEntityInfo( hndl );
	}

	//////////////////////////////////////////////////////////////////////////

	Line::Line( const float * pos, const float * size, uint32 clr )
		: x( pos[ 0 ] )
		, y( pos[ 1 ] )
		, sx( size[ 0 ] )
		, sy( size[ 1 ] )
		, color( clr ) {
	}

	bool Line::Send( DataBuffer & db ) {
		db;
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// line
	bool PackLine( DataBuffer & db,
		const char * group,
		const char * name,
		float x0, float y0,
		float x1, float y1,
		int8 r, int8 g, int8 b, int8 a ) {
			db.writeInt32( RemoteLib::ID_line );
			db.writeSmallString( group );
			db.writeSmallString( name );
			db.writeFloat16( x0, 0 );
			db.writeFloat16( y0, 0 );
			db.writeFloat16( x1, 0 );
			db.writeFloat16( y1, 0 );
			db.writeInt8( r );
			db.writeInt8( g );
			db.writeInt8( b );
			db.writeInt8( a );
			return !db.hasWriteError();
	}

	bool UnPackLine( DataBuffer & db,
		char * group, int groupBufferSize,
		char * name, int nameBufferSize,
		float & x0, float & y0,
		float & x1, float & y1,
		int8 & r, int8 & g, int8 & b, int8 & a ) {
			db.readSmallString( group, groupBufferSize );
			db.readSmallString( name, nameBufferSize );
			db.readFloat16( x0, 0 );
			db.readFloat16( y0, 0 );
			db.readFloat16( x1, 0 );
			db.readFloat16( y1, 0 );
			db.readInt8( r );
			db.readInt8( g );
			db.readInt8( b );
			db.readInt8( a );
			return !db.hasReadError();
	}
	//////////////////////////////////////////////////////////////////////////
	// oriented rect
	bool PackRect( DataBuffer & db,
		const char * group,
		const char * name,
		float x, float y,
		float w, float h,
		float yaw,
		int8 r, int8 g, int8 b, int8 a ) {
			db.writeInt32( RemoteLib::ID_obb );
			db.writeSmallString( group );
			db.writeSmallString( name );
			db.writeFloat16( x, 0 );
			db.writeFloat16( y, 0 );
			db.writeFloat16( w, 0 );
			db.writeFloat16( h, 0 );
			db.writeFloat16( yaw, 0 );
			db.writeInt8( r );
			db.writeInt8( g );
			db.writeInt8( b );
			db.writeInt8( a );
			return !db.hasWriteError();
	}

	bool UnPackRect( DataBuffer & db,
		char * group, int groupBufferSize,
		char * name, int nameBufferSize,
		float &x, float &y,
		float &w, float &h,
		float &yaw,
		int8 & r, int8 & g, int8 & b, int8 & a ) {
			db.readSmallString( group, groupBufferSize );
			db.readSmallString( name, nameBufferSize );
			db.readFloat16( x, 0 );
			db.readFloat16( y, 0 );
			db.readFloat16( w, 0 );
			db.readFloat16( h, 0 );
			db.readFloat16( yaw, 0 );
			db.readInt8( r );
			db.readInt8( g );
			db.readInt8( b );
			db.readInt8( a );
			return !db.hasReadError();
	}
	//////////////////////////////////////////////////////////////////////////
	// ellipse
	bool PackEllipse( DataBuffer & db,
		const char * group,
		const char * name,
		float x, float y,
		float sizex, float sizey, 
		float pieAngle, float yaw,
		int8 r, int8 g, int8 b, int8 a ) {
			db.writeInt32( RemoteLib::ID_ellipse );
			db.writeSmallString( group );
			db.writeSmallString( name );
			db.writeFloat16( x, 0 );
			db.writeFloat16( y, 0 );
			db.writeFloat16( sizex, 0 );
			db.writeFloat16( sizey, 0 );
			db.writeFloat16( pieAngle, 0 );
			db.writeFloat16( yaw, 0 );
			db.writeInt8( r );
			db.writeInt8( g );
			db.writeInt8( b );
			db.writeInt8( a );
			return !db.hasWriteError();
	}

	bool UnpackEllipse( DataBuffer & db,
		char * group, int groupBufferSize,
		char * name, int nameBufferSize,
		float &x, float &y,
		float &sizex, float &sizey, 
		float &pieAngle, float &yaw,
		int8 & r, int8 & g, int8 & b, int8 & a ) {
			db.readSmallString( group, groupBufferSize );
			db.readSmallString( name, nameBufferSize );
			db.readFloat16( x, 0 );
			db.readFloat16( y, 0 );
			db.readFloat16( sizex, 0 );
			db.readFloat16( sizey, 0 );
			db.readFloat16( pieAngle, 0 );
			db.readFloat16( yaw, 0 );
			db.readInt8( r );
			db.readInt8( g );
			db.readInt8( b );
			db.readInt8( a );
			return !db.hasReadError();
	}
	//////////////////////////////////////////////////////////////////////////
	bool PackCircle( DataBuffer & db,
		const char * group,
		const char * name,
		float x, float y, float radius,
		int8 r, int8 g, int8 b, int8 a ) {
			db.writeInt32( RemoteLib::ID_ellipse );
			db.writeSmallString( group );
			db.writeSmallString( name );
			db.writeFloat16( x, 0 );
			db.writeFloat16( y, 0 );
			db.writeFloat16( radius, 0 );
			db.writeInt8( r );
			db.writeInt8( g );
			db.writeInt8( b );
			db.writeInt8( a );
			return !db.hasWriteError();
	}

	bool UnpackCircle( DataBuffer & db,
		char * group, int groupBufferSize,
		char * name, int nameBufferSize,
		float & x, float & y, float & radius,
		int8 & r, int8 & g, int8 & b, int8 & a ) {
			db.readSmallString( group, groupBufferSize );
			db.readSmallString( name, nameBufferSize );
			db.readFloat16( x, 0 );
			db.readFloat16( y, 0 );
			db.readFloat16( radius, 0 );
			db.readInt8( r );
			db.readInt8( g );
			db.readInt8( b );
			db.readInt8( a );
			return !db.hasReadError();
	}
	//////////////////////////////////////////////////////////////////////////
	// Image
	bool PackImage( DataBuffer & db,
		const char * group,
		const char * name,
		const char * imgFile,
		float x, float y, 
		float sizex, float sizey, float yaw,
		int8 r, int8 g, int8 b, int8 a )  {
			db.writeInt32( RemoteLib::ID_image );
			db.writeSmallString( group );
			db.writeSmallString( name );
			db.writeSmallString( imgFile );
			db.writeFloat16( x, 0 );
			db.writeFloat16( y, 0 );
			db.writeFloat16( sizex, 0 );
			db.writeFloat16( sizey, 0 );
			db.writeFloat16( yaw, 0 );
			db.writeInt8( r );
			db.writeInt8( g );
			db.writeInt8( b );
			db.writeInt8( a );
			return !db.hasWriteError();
	}

	bool UnpackImage( DataBuffer & db, 
		char * group, int groupBufferSize,
		char * name, int nameBufferSize,
		char * file, int fileBufferSize,
		float & x, float & y, 
		float & sizex, float & sizey, float & yaw, 
		int8 & r, int8 & g, int8 & b, int8 & a ) {
			db.readSmallString( group, groupBufferSize );
			db.readSmallString( name, nameBufferSize );
			db.readSmallString( file, fileBufferSize );
			db.readFloat16( x, 0 );
			db.readFloat16( y, 0 );
			db.readFloat16( sizex, 0 );
			db.readFloat16( sizey, 0 );
			db.readFloat16( yaw, 0 );
			db.readInt8( r );
			db.readInt8( g );
			db.readInt8( b );
			db.readInt8( a );
			return !db.hasReadError();
	}
};
