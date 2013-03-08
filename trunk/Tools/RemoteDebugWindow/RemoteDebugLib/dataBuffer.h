#pragma once

#include "Primitives.h"

namespace RemoteLib
{
	int32 HashString32(const char *_str);

	class TcpSocket;

	class DataBuffer
	{
	public:
		enum BufferMode {
			ModeWrite,
			ModeRead,
		};
		enum WriteMode {
			WriteModeNone,
			WriteModeAllOrNone,
			WriteModePartial,
		};
		enum ReadMode {
			ReadModeNone,
			ReadModeAllOrNone,
			ReadModePartial,
		};
		enum {
			DefaultFloatPrecision = 2
		};

		uint32 beginWrite( WriteMode mode );
		uint32 endWrite();

		uint32 beginRead( ReadMode mode );
		uint32 endRead( bool resetToReadStart = false );

		bool isWriting() const { return writeMode > WriteModeNone; }
		bool isReading() const { return readMode > ReadModeNone; }

		bool startSizeHeader();
		uint32 endSizeHeader();

		int readFromSocket( TcpSocket & socket );
		int sendToSocket( TcpSocket & socket );

		bool peekInt8( int8 & v );
		bool readInt8( int8 & v );
		bool writeInt8( int8 v );

		bool peekInt16( int16 & v );
		bool readInt16( int16 & v );
		bool writeInt16( int16 v );		

		bool peekInt32( int32 & v );
		bool readInt32( int32 & v );
		bool writeInt32( int32 v );

		bool writeFloat16( float v, int precision = DefaultFloatPrecision );
		bool readFloat16( float & v, int precision = DefaultFloatPrecision );
		bool writeFloat32( float v, int precision = DefaultFloatPrecision );
		bool readFloat32( float & v, int precision = DefaultFloatPrecision );

		bool writeString( const char * str );
		bool readString( char * buffer, int maxSize );

		bool writeSmallString( const char * str );
		bool readSmallString( char * buffer, int maxSize );

		bool write( const char * data, uint32 numBytes );
		bool read( char * data, uint32 numBytes );

		bool append( const DataBuffer & db );

		//////////////////////////////////////////////////////////////////////////
		// template to auto convert
		template< typename T >
		bool read8( T & v ) {
			int8 i = 0;
			if ( readInt8( i ) ) {
				v = (T)i;
				return true;
			}
			return false;
		}
		template< typename T >
		bool read16( T & v ) {
			int16 i = 0;
			if ( readInt16( i ) ) {
				v = (T)i;
				return true;
			}
			return false;
		}
		template< typename T >
		bool read32( T & v ) {
			int32 i = 0;
			if ( readInt32( i ) ) {
				v = (T)i;
				return true;
			}
			return false;
		}

		uint32 getBytesWritten() const;
		uint32 getBytesRead() const;

		uint32 getReadBytesLeft() const;
		uint32 getWriteBytesLeft() const;

		void collapseToReadPtr();

		void init( char * staticBuffer, int bufferSize );
		void free();
		void clear();

		void setAssertOnError() { assertOnError = true; }

		bool hasWriteError() const { return writeError > 0; }
		bool hasReadError() const { return readError > 0; }

		bool		canWrite( uint32 bytes );
		bool		canRead( uint32 bytes );

		DataBuffer( int bufferSize = 0 );
		DataBuffer( char * staticBuffer, int bufferSize, BufferMode mode = ModeWrite );
		~DataBuffer();
	private:
		int			maxBufferSize;
		char *		buffer;

		char *		readPtr;
		char *		writePtr;

		// size headers
		uint32 *	sizeHdr;

		// for restoring read/write state
		WriteMode	writeMode;
		ReadMode	readMode;
		char *		lastWriteStart;
		char *		lastReadStart;

		uint32		writeError;
		uint32		readError;

		bool		isStatic;
		bool		assertOnError;

		// sets error state
		bool		checkWrite( uint32 bytes );
		bool		checkRead( uint32 bytes );
	};
	//////////////////////////////////////////////////////////////////////////
	template< int size >
	class DataBufferStatic : public DataBuffer {
	public:
		DataBufferStatic() : DataBuffer( buffer, size ) { }
	private:
		char buffer[ size ];
	};
	//////////////////////////////////////////////////////////////////////////
	template< int _size_ = 64 >
	class SyncSnapshot {
	public:
		enum valType {
			VAL_NULL,
			VAL_FLOAT,
			VAL_INT,
			VAL_STRING,
		};

		bool Sync( const char * name, float val, DataBuffer & db, bool force = false ) {
			int i = 0;
			for ( ; i < numRecords; ++i ) {
				if ( buffer[ i ].name == name ) {
					assert( buffer[ i ].type == VAL_FLOAT );
					if ( force || buffer[ i ].data.fval != val ) {
						// need to sync
						break;
					} else {
						// dont need to update
						return true;
					}
				}
			}
			if ( i == numRecords ) {
				if ( numRecords < _size_ ) {
					numRecords++;
				} else {
					assert( 0 && "out of records!" );
					return false;
				}
			}
			if ( i < numRecords ) {
				buffer[ i ].name = name;
				buffer[ i ].type = VAL_FLOAT;
				buffer[ i ].data.fval = val;
				return db.writeSmallString( name ) && db.writeFloat32( val );
			}
			return false;
		}
		bool Sync( const char * name, int32 val, DataBuffer & db, bool force = false ) {
			int i = 0;
			for ( ; i < numRecords; ++i ) {
				if ( buffer[ i ].name == name ) {
					assert( buffer[ i ].type == VAL_INT );
					if ( force || buffer[ i ].data.ival != val ) {
						// need to sync
						break;
					} else {
						// dont need to update
						return true;
					}
				}
			}
			if ( i == numRecords ) {
				if ( numRecords < _size_ ) {
					numRecords++;
				} else {
					assert( 0 && "out of records!" );
					return false;
				}
			}
			if ( i < numRecords ) {
				buffer[ i ].name = name;
				buffer[ i ].type = VAL_INT;
				buffer[ i ].data.ival = val;
				return db.writeSmallString( name ) && db.writeInt32( val );
			}
			return false;
		}
		bool Sync( const char * name, const char * val, DataBuffer & db, bool force = false ) {
			// hash std::string
			const int32 stringHash = HashString32( val );
			
			int i = 0;
			for ( ; i < numRecords; ++i ) {
				if ( buffer[ i ].name == name ) {
					assert( buffer[ i ].type == VAL_STRING );
					if ( force || buffer[ i ].data.sval != stringHash ) {
						// need to sync
						break;
					} else {
						// dont need to update
						return true;
					}
				}
			}
			if ( i == numRecords ) {
				if ( numRecords < _size_ ) {
					numRecords++;
				} else {
					assert( 0 && "out of records!" );
					return false;
				}
			}
			if ( i < numRecords ) {
				buffer[ i ].name = name;
				buffer[ i ].type = VAL_STRING;
				buffer[ i ].data.sval = stringHash;
				return db.writeSmallString( name ) && db.writeSmallString( val );
			}
			return false;
		}
		void Clear() { numRecords = 0; }
		SyncSnapshot() : numRecords( 0 ) {
			for ( int i = 0; i < _size_; ++i ) {
				buffer[ i ].type = VAL_NULL;
				buffer[ i ].name = NULL;
				buffer[ i ].data.ival = 0;
			}
		}
	private:
		struct values_t {
			union {
				float		fval;
				int32		ival;
				int32		sval;
			} data;
			
			valType			type;
			const char *	name;
			values_t() 
				: name( NULL ) {
					data.ival = 0;
			}
		};

		values_t		buffer[ _size_ ];
		int				numRecords;
	};
};
