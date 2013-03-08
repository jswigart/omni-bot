#include <memory>
#include <ctype.h>
#include <assert.h>

#include "SystemLib.h"
#include "DataBuffer.h"
#include "Socket.h"

#define VALIDATEBUFFER \
	assert( writePtr <= buffer + maxBufferSize && "write overflow" ); \
	assert( readPtr <= buffer + maxBufferSize && "write overflow" );

namespace RemoteLib
{
	DataBuffer::DataBuffer( char * staticBuffer, int bufferSize, BufferMode mode ) {
		clear();
		init( staticBuffer, bufferSize );
		if ( mode == ModeRead ) {
			writePtr = buffer + bufferSize;
		}
	}

	DataBuffer::DataBuffer( int bufferSize ) {
		clear();
		init( NULL, bufferSize );
	}
	DataBuffer::~DataBuffer() {
		free();
	}
	void DataBuffer::init( char * staticBuffer, int bufferSize ) {
		free();

		maxBufferSize = bufferSize;

		if ( staticBuffer ) {
			isStatic = true;
			buffer = staticBuffer;
		} else {
			isStatic = false;
			buffer = new char[ maxBufferSize ];
		}
		writePtr = readPtr = buffer = buffer;
	}

	void DataBuffer::free() {
		if ( !isStatic ) {
			delete [] buffer;
		}
		writePtr = readPtr = buffer = 0;
		maxBufferSize = 0;
	}

	void DataBuffer::clear() {
		writePtr = readPtr = buffer = NULL;

		writeMode = WriteModeNone;
		readMode = ReadModeNone;
		lastWriteStart = NULL;
		lastReadStart = NULL;
		writeError = 0;
		readError = 0;

		sizeHdr = NULL;

		assertOnError = false;
	}

	int DataBuffer::readFromSocket( TcpSocket & socket ) {
		const int rcvBytesFree = maxBufferSize - ( writePtr - buffer );

		int bytesRead = 0;
		if ( socket.rcvData( bytesRead, writePtr, rcvBytesFree ) ) {
			writePtr += bytesRead;
		}
		VALIDATEBUFFER;
		return bytesRead;
	}

	int DataBuffer::sendToSocket( TcpSocket & socket ) {
		const int maxSendBytes = writePtr - buffer;

		int bytesSent = 0;
		if ( maxSendBytes > 0 && socket.sendData( bytesSent, buffer, maxSendBytes ) ) {
			// get rid of it
			memmove( buffer, buffer + bytesSent, maxSendBytes - bytesSent );
			writePtr -= bytesSent;
		}
		VALIDATEBUFFER;
		return bytesSent;
	}

	uint32 DataBuffer::getBytesWritten() const {
		assert( writePtr >= buffer );
		return writePtr - buffer;
	}

	uint32 DataBuffer::getBytesRead() const {
		assert( readPtr >= buffer );
		return readPtr - buffer;
	}

	uint32 DataBuffer::getReadBytesLeft() const {
		return maxBufferSize - getBytesRead();
	}

	uint32 DataBuffer::getWriteBytesLeft() const {
		return maxBufferSize - getBytesWritten();
	}

	void DataBuffer::collapseToReadPtr() {
		const int memToMove = maxBufferSize - ( readPtr - buffer );
		const int memMoveDist = readPtr - buffer;
		if ( memMoveDist > 0 ) {
			//assert( readPtr > );
			memmove( buffer, readPtr, memToMove );
			readPtr -= memMoveDist;
			writePtr -= memMoveDist;
		}
	}

	bool DataBuffer::startSizeHeader() {
		if ( !checkWrite( sizeof( uint32 ) ) ) { return false; }

		assert( sizeHdr == NULL && isWriting() );
		sizeHdr = (uint32*)writePtr;
		*sizeHdr = 0;
		writePtr += sizeof( uint32 );
		return true;
	}

	uint32 DataBuffer::endSizeHeader() {
		// doesn't include the size header
		if ( sizeHdr ) {
			const int32 packetSize = (int32)( writePtr - (char*)sizeHdr ) - sizeof( uint32 );
			*sizeHdr = SystemLib::hostToNetByteOrder( packetSize );
			sizeHdr = NULL;
			return (uint32)packetSize;
		}
		return 0;
	}

	bool DataBuffer::peekInt8( int8 & v ) {
		if ( !canRead( sizeof( v ) ) ) { return false; }

		v = *(int8*)readPtr;
		return true;
	}

	bool DataBuffer::readInt8( int8 & v ) {
		if ( !checkRead( sizeof( v ) ) ) { return false; }

		v = *(int8*)readPtr;
		readPtr += sizeof( int8 );

		VALIDATEBUFFER;
		return true;
	}

	bool DataBuffer::writeInt8( int8 v ) {
		if ( !checkWrite( sizeof( v ) ) ) { return false; }

		memcpy( writePtr, &v, sizeof( int8 ) );
		writePtr += sizeof( int8 );

		VALIDATEBUFFER;
		return true;
	}

	bool DataBuffer::peekInt16( int16 & v ) {
		if ( !canRead( sizeof( v ) ) ) { return false; }

		v = SystemLib::netToHostByteOrder( *(int16*)readPtr );
		return true;
	}

	bool DataBuffer::readInt16( int16 & v ) {
		if ( !checkRead( sizeof( v ) ) ) { return false; }

		v = SystemLib::netToHostByteOrder( *(int16*)readPtr );
		readPtr += sizeof( int16 );

		VALIDATEBUFFER;
		return true;
	}

	bool DataBuffer::writeInt16( int16 v ) {
		if ( !checkWrite( sizeof( v ) ) ) { return false; }

		v = SystemLib::hostToNetByteOrder( v );
		*( (int16*)writePtr ) = v;
		writePtr += sizeof( int16 );

		VALIDATEBUFFER;
		return true;
	}

	bool DataBuffer::peekInt32( int32 & v ) {
		if ( !canRead( sizeof( v ) ) ) { return false; }

		v = SystemLib::netToHostByteOrder( *(int32*)readPtr );
		return true;
	}

	bool DataBuffer::readInt32( int32 & v ) {
		if ( !checkRead( sizeof( v ) ) ) { return false; }

		v = SystemLib::netToHostByteOrder( *(int32*)readPtr );
		readPtr += sizeof( int32 );

		VALIDATEBUFFER;
		return true;
	}

	bool DataBuffer::writeInt32( int32 v ) {
		if ( !checkWrite( sizeof( v ) ) ) { return false; }

		v = SystemLib::hostToNetByteOrder( v );
		*( (int32*)writePtr ) = v;
		writePtr += sizeof( int32 );

		VALIDATEBUFFER;
		return true;
	}

	bool DataBuffer::writeFloat16( float v, int precision ) {
		v *= ( precision>0 ? (10 * precision) : 1.0f );
		return writeInt16( (int16)v );
	}

	bool DataBuffer::readFloat16( float & v, int precision ) {
		int16 i = 0;
		if ( readInt16( i ) ) {
			v = (float)i / ( precision>0 ? (10 * precision) : 1.0f );
			return true;
		}
		return false;
	}

	bool DataBuffer::writeFloat32( float v, int precision ) {
		v *= 10 * precision;
		return writeInt32( (int32)v );
	}

	bool DataBuffer::readFloat32( float & v, int precision ) {
		int32 i = 0;
		if ( readInt32( i ) ) {
			v = (float)i / (10 * precision);
			return true;
		}
		return false;
	}

	bool DataBuffer::writeString( const char * str ) {
		const int16 len =(int16)strlen( str ) + 1;
		if ( !checkWrite( len + sizeof( int16 ) ) ) { return false; }

		return writeInt16( len ) && write( str, len );
	}

	bool DataBuffer::readString( char * buffer, int maxSize ) {
		int16 len = 0;
		assert( maxSize >= len ); maxSize;
		return readInt16( len ) && read( buffer, len );
	}

	bool DataBuffer::writeSmallString( const char * str ) {
		const int8 len =(int8)strlen( str ) + 1;
		if ( !checkWrite( len + sizeof( int8 ) ) ) { return false; }

		return writeInt8( len ) && write( str, len );
	}

	bool DataBuffer::readSmallString( char * buffer, int maxSize ) {
		int8 len = 0;
		assert( maxSize >= len ); maxSize;
		return readInt8( len ) && read( buffer, len );
	}

	bool DataBuffer::write( const char * data, uint32 numBytes ) {
		if ( !checkWrite( numBytes ) ) { return false; }

		memcpy( writePtr, data, numBytes );
		writePtr += numBytes;

		VALIDATEBUFFER;
		return true;
	}

	bool DataBuffer::read( char * data, uint32 numBytes ) {
		if ( !checkRead( numBytes ) ) { return false; }

		memcpy( data, readPtr, numBytes );
		readPtr += numBytes;

		VALIDATEBUFFER;
		return true;
	}

	bool DataBuffer::append( const DataBuffer & db ) {
		if ( isWriting() ) {
			return write( db.readPtr, db.getBytesWritten() );
		} else {
			beginWrite( WriteModeAllOrNone );
			bool ok = write( db.readPtr, db.getBytesWritten() );
			endWrite();
			return ok;
		}
	}

	uint32 DataBuffer::beginWrite( WriteMode mode ) {
		assert( !isWriting() );
		writeMode = mode;
		writeError = false;
		lastWriteStart = writePtr;
		return getWriteBytesLeft();
	}

	uint32 DataBuffer::endWrite() {
		assert( isWriting() );
		const uint32 ret = writeError;
		if ( writeError ) {
			writePtr = lastWriteStart;
		}
		writeMode = WriteModeNone;
		writeError = 0;
		lastWriteStart = NULL;
		return ret;
	}

	uint32 DataBuffer::beginRead( ReadMode mode ) {
		assert( !isReading() );
		readMode = mode;
		readError = 0;
		lastReadStart = readPtr;
		return getReadBytesLeft();
	}

	uint32 DataBuffer::endRead( bool resetToReadStart ) {
		assert( isReading() );
		const uint32 ret = readError;
		if ( resetToReadStart || readError ) {
			readPtr = lastReadStart;
		}
		readMode = ReadModeNone;
		readError = 0;
		lastReadStart = NULL;
		return ret;
	}

	bool DataBuffer::canWrite( uint32 bytes ) {
		assert( isWriting() );
		if ( writeError == 0 && isWriting() ) {
			if ( getWriteBytesLeft() >= bytes ) {
				return true;
			}
		}
		return false;
	}

	bool DataBuffer::canRead( uint32 bytes ) {
		assert( isReading() );
		if ( readError == 0 && isReading() ) {
			if ( getReadBytesLeft() >= bytes ) {
				return true;
			}
		}
		return false;
	}

	bool DataBuffer::checkWrite( uint32 bytes ) {
		if ( !canWrite( bytes ) ) {
			assert( !assertOnError && "checkWrite" );
			writeError += bytes;
			return false;
		}
		return true;
	}

	bool DataBuffer::checkRead( uint32 bytes ) {
		if ( !canRead( bytes ) ) {
			assert( !assertOnError && "checkWrite" );
			readError += bytes;
			return false;
		}
		return true;
	}

	/*
	* FNV-1a hash 32 bit http://www.isthe.com/chongo/tech/comp/fnv/
	*/
	int32 HashString32(const char *_str)
	{
		const int32 FNV_32_PRIME = ((int32)0x01000193);
		const int32 FNV1A_32_INIT = ((int32)0x811c9dc5);

		const char *s = _str;
		int32 hval = FNV1A_32_INIT;
		while (*s)
		{
			char c = (char)tolower(*s++);
			hval ^= (int32)c;
			hval *= FNV_32_PRIME;
		}
		return hval;
	}
};