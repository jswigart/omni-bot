#include "primitives.h"

#ifndef __MESSAGETAGS_H__
#define __MESSAGETAGS_H__

#define REMOTELIB_MAKE_ID32( a, b, c, d )  ( ((d)<<24) | ((c)<<16) | ((b)<<8) | (a))

namespace RemoteLib
{
	enum MsgTagId 
	{
		ID_configName = REMOTELIB_MAKE_ID32('C','O','N','F'),
		ID_ack = REMOTELIB_MAKE_ID32('A','C','K','0'),
		ID_delete = REMOTELIB_MAKE_ID32('D','E','L','T'),
		ID_treeNode = REMOTELIB_MAKE_ID32('T','R','E','N'),
		ID_circle = REMOTELIB_MAKE_ID32('C','I','R','C'),
		ID_ellipse = REMOTELIB_MAKE_ID32('E','L','I','P'),
		ID_line = REMOTELIB_MAKE_ID32('L','I','N','E'),
		ID_obb = REMOTELIB_MAKE_ID32('O','B','O','X'),
		ID_image = REMOTELIB_MAKE_ID32('I','M','A','G'),
		ID_token = REMOTELIB_MAKE_ID32('T','O','K','S'),
		ID_tokenEnd = REMOTELIB_MAKE_ID32('T','O','K','E'),
	};

	//////////////////////////////////////////////////////////////////////////
	class DataBuffer;
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// line
	bool PackLine( DataBuffer & db,
		const char * group,
		const char * name,
		float x0, float y0,
		float x1, float y1,
		int8 r, int8 g, int8 b, int8 a );

	bool UnPackLine( DataBuffer & db,
		char * group, int groupBufferSize,
		char * name, int nameBufferSize,
		float &x0, float &y0,
		float &x1, float &y1,
		int8 & r, int8 & g, int8 & b, int8 & a );
	//////////////////////////////////////////////////////////////////////////
	// oriented rect
	bool PackRect( DataBuffer & db,
		const char * group,
		const char * name,
		float x, float y,
		float w, float h,
		float yaw,
		int8 r, int8 g, int8 b, int8 a );

	bool UnPackRect( DataBuffer & db,
		char * group, int groupBufferSize,
		char * name, int nameBufferSize,
		float &x, float &y,
		float &w, float &h,
		float &yaw,
		int8 & r, int8 & g, int8 & b, int8 & a );
	//////////////////////////////////////////////////////////////////////////
	// ellipse
	bool PackEllipse( DataBuffer & db,
		const char * group,
		const char * name,
		float x, float y,
		float sizex, float sizey, 
		float pieAngle, float yaw,
		int8 r, int8 g, int8 b, int8 a );

	bool UnpackEllipse( DataBuffer & db,
		char * group, int groupBufferSize,
		char * name, int nameBufferSize,
		float &x, float &y,
		float &sizex, float &sizey, 
		float &pieAngle, float &yaw,
		int8 & r, int8 & g, int8 & b, int8 & a );
	//////////////////////////////////////////////////////////////////////////
	// circle
	bool PackCircle( DataBuffer & db,
		const char * group,
		const char * name,
		float x, float y, float radius,
		int8 r, int8 g, int8 b, int8 a );

	bool UnpackCircle( DataBuffer & db,
		char * group, int groupBufferSize,
		char * name, int nameBufferSize,
		float & x, float & y, float & radius,
		int8 & r, int8 & g, int8 & b, int8 & a );
	//////////////////////////////////////////////////////////////////////////
	// image
	bool PackImage( DataBuffer & db,
		const char * group,
		const char * name,
		const char * imgFile,
		float x, float y, 
		float sizex, float sizey, float yaw,
		int8 r, int8 g, int8 b, int8 a );
	bool UnpackImage( DataBuffer & db, 
		char * group, int groupBufferSize,
		char * name, int nameBufferSize,
		char * file, int fileBufferSize,
		float & x, float & y, 
		float & sizex, float & sizey, float & yaw, 
		int8 & r, int8 & g, int8 & b, int8 & a );
};

#endif
