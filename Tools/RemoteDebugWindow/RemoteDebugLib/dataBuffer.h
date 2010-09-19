#pragma once

#include "Primitives.h"

namespace RemoteLib
{
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
		void endSizeHeader();

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

		bool write( const char * data, uint32 numBytes );
		bool read( char * data, uint32 numBytes );

		bool append( DataBuffer & db );

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
};
