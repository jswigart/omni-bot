#include "messageTags.h"
#include "dataBuffer.h"

namespace RemoteLib
{
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
