#include "primitives.h"

#ifndef __MESSAGETAGS_H__
#define __MESSAGETAGS_H__

#define REMOTELIB_MAKE_ID32( a, b, c, d )  ( ((a)<<24) | ((b)<<16) | ((c)<<8) | (d))

namespace RemoteLib
{
	//////////////////////////////////////////////////////////////////////////      
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
		ID_qmlEntity = REMOTELIB_MAKE_ID32('Q','M','L','E'),

		ID_entityInfo = REMOTELIB_MAKE_ID32('E','N','T','I'),
			ID_entityName = REMOTELIB_MAKE_ID32('E','N','A','M'),
			ID_entityTooltip = REMOTELIB_MAKE_ID32('E','T','T','P'),
			ID_entityImage = REMOTELIB_MAKE_ID32('E','I','M','G'),
			ID_entityTransform = REMOTELIB_MAKE_ID32('E','T','F','M'),
			ID_entitySize = REMOTELIB_MAKE_ID32('E','S','I','Z'),
		ID_entityDelete = REMOTELIB_MAKE_ID32('E','N','T','D'),
	};

	//////////////////////////////////////////////////////////////////////////
	class DataBuffer;
	//////////////////////////////////////////////////////////////////////////

	class RemoteObjectDbImpl;

	typedef int32 RemoteEntHandle;

	enum EntInfoFlags {
		INFO_INUSE			= (1<<0),
		INFO_DELETE			= (1<<1),
		INFO_SYNC_NAME		= (1<<2),
		INFO_SYNC_TOOLTIP	= (1<<3),
		INFO_SYNC_IMAGE		= (1<<4),
		INFO_SYNC_TRANSFORM	= (1<<5),
		INFO_SYNC_SIZE		= (1<<6),

		INFO_SYNC_BITS		= INFO_SYNC_NAME|INFO_SYNC_TOOLTIP|INFO_SYNC_IMAGE|INFO_SYNC_TRANSFORM|INFO_SYNC_SIZE,
	};

	class RemoteEntInfo {
	public:
		friend class RemoteObjectDbImpl;

		void SetName( const char * newName );
		void SetToolTip( const char * newTooltip );
		void SetImage( const char * newImage );
		void SetTransform( float newX, float newY, float newYaw );
		void SetSize( float newSizeX, float newSizeY );

		void Free();

		RemoteEntInfo();
	private:
		enum	{ BufferSz = 64 };
		uint32	flags;
		uint16	serial;

		char	name[BufferSz];
		char	tooltip[BufferSz];
		char	image[BufferSz];

		float	posX, posY;
		float	sizeX, sizeY;
		float	yaw;
	};

	class RemoteObjectDb {
	public:
		
		RemoteEntHandle AllocEntity( const char * name, const char * tooltip );
		void DeleteEntity( RemoteEntHandle hndl );

		RemoteEntInfo * GetEntityInfo( RemoteEntHandle hndl );

		RemoteObjectDb( uint32 maxEntities );
		~RemoteObjectDb();
	private:
		RemoteObjectDbImpl * impl;
	};

	//////////////////////////////////////////////////////////////////////////

	class Line {
	public:
		static const uint32 TagId = REMOTELIB_MAKE_ID32('L','I','N','E');

		Line( const float * pos, const float * size, uint32 clr );

		bool Send( DataBuffer & db );
	private:
		float x, y;
		float sx, sy;
		uint32 color;
	};

	//////////////////////////////////////////////////////////////////////////

	class Entity {
	public:
		static const uint32 TagId = REMOTELIB_MAKE_ID32('E','N','T','T');

		Entity( uint32 entId, const float * pos, const float * size, uint32 clr );

		bool Send( DataBuffer & db );
	private:
		uint32 id;
		float x, y;
		float sx, sy;
		uint32 color;
	};

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
