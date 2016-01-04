#pragma once

#ifndef RENDERBUFFER_H
#define RENDERBUFFER_H

#include "common.h"

namespace RenderBuffer
{
	struct Point
	{
		Vector3f	v[ 1 ];
		obColor		c;
		float		duration;
	};
	struct Line
	{
		Vector3f	v[ 2 ];
		obColor		c;
		float		duration;
	};
	struct Triangle
	{
		Vector3f	v[ 3 ];
		obColor		c;
		float		duration;
	};
	struct Quad
	{
		Vector3f	v[ 4 ];
		obColor		c;
		float		duration;
	};
	struct Circle
	{
		Vector3f	v;
		obColor		c;
		float		radius;
		Vector3f	up;
		float		duration;
	};
	struct Str2d
	{
		Vector2f		v;
		obColor			c;
		std::string		str;
		float			duration;
	};
	struct Str3d
	{
		Vector3f		v;
		obColor			c;
		std::string		str;
		float			radius;
		float			duration;
	};

	typedef std::vector<Point>		PointList;
	typedef std::vector<Line>		LineList;
	typedef std::vector<Triangle>	TriList;
	typedef std::vector<Quad>		QuadList;
	typedef std::vector<Circle>		CircleList;
	typedef std::vector<Str3d>		StringList3d;
	typedef std::vector<Str2d>		StringList2d;
	typedef std::vector<uint32_t>	VBOList;

	void Init();
	void StartFrame();

	void RenderToOpenGL();
	void RenderToGame();

#define DEFAULT_TIME 0.0

	void AddPoint( const Vector3f & v, const obColor & col, float duration = DEFAULT_TIME );
	void AddPoints( const Vector3List & verts, obColor col, float duration = DEFAULT_TIME );
	void AddArrow( const Vector3f & v0, const Vector3f & v1, const obColor & col, float duration = DEFAULT_TIME );
	void AddLine( const Vector3f & v0, const Vector3f & v1, const obColor & col, float duration = DEFAULT_TIME );
	void AddLine( const Vector3List & v, const obColor & col, float duration = DEFAULT_TIME );
	void AddTri( const Triangle & tri, float duration = DEFAULT_TIME );
	void AddTri( const Vector3f & v0, const Vector3f & v1, const Vector3f & v2, const obColor & col, float duration = DEFAULT_TIME );
	void AddQuad( const Quad & primitive, float duration = DEFAULT_TIME );
	void AddQuad( const Vector3f & v0, const Vector3f & v1, const Vector3f & v2, const Vector3f & v3, const obColor & col, float duration = DEFAULT_TIME );
	void AddPolygonFilled( const Vector3List & verts, obColor col, float duration = DEFAULT_TIME );
	void AddPolygonSilouette( const Vector3List & verts, obColor col, float duration = DEFAULT_TIME );
	void AddCircle( const Vector3f & v, float radius, const obColor & col, const Vector3f & up = Vector3f::UNIT_Z, float duration = DEFAULT_TIME );
	void AddString3d( const Vector3f & v, const obColor & col, const char * str, float duration = DEFAULT_TIME );
	void AddString3dRadius( const Vector3f & v, const obColor & col, float radius, const char * str, float duration = DEFAULT_TIME );
	void AddString2d( const Vector2f & v, const obColor & col, const char * str, float duration = DEFAULT_TIME );
	void AddAABB( const AxisAlignedBox3f &_aabb, const obColor &_color, AABB::Direction _dir = AABB::DIR_ALL, float duration = DEFAULT_TIME );
	void AddAABB( const AABB &_aabb, const obColor &_color, AABB::Direction _dir = AABB::DIR_ALL, float duration = DEFAULT_TIME );
	void AddOBB( const Box3f &_obb, const obColor &_color, AABB::Direction _dir = AABB::DIR_ALL, float duration = DEFAULT_TIME );

#undef DEFAULT_TIME

	bool StaticBufferCreate( uint32_t & bufferId, const TriList & primitives );
	bool StaticBufferCreate( uint32_t & bufferId, const QuadList & primitives );
	void StaticBufferDelete( uint32_t bufferId );
	void StaticBufferDraw( uint32_t bufferId );
};

#endif
