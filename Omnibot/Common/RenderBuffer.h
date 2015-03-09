#pragma once

#ifndef RENDERBUFFER_H
#define RENDERBUFFER_H

#include "common.h"

namespace RenderBuffer
{
	struct Point
	{
		Vector3f	v[1];
		obColor		c;
		float		pointSize;
	};
	struct Line
	{
		Vector3f	v[2];
		obColor		c;
		float		lineWidth;
	};
	struct Triangle
	{
		Vector3f	v[3];
		obColor		c;
	};
	struct Quad
	{
		Vector3f	v[4];
		obColor		c;
	};
	struct Circle
	{
		Vector3f	v;
		obColor		c;
		float		radius;
		Vector3f	up;
	};
	struct Str2d
	{
		Vector2f		v;
		obColor			c;
		std::string		str;
	};
	struct Str3d
	{
		Vector3f		v;
		obColor			c;
		std::string		str;
		float			radius;
	};

	typedef std::vector<Point>		PointList;
	typedef std::vector<Line>		LineList;
	typedef std::vector<Triangle>	TriList;
	typedef std::vector<Quad>		QuadList;
	typedef std::vector<Circle>		CircleList;
	typedef std::vector<Str3d>		StringList3d;
	typedef std::vector<Str2d>		StringList2d;
	typedef std::vector<obuint32>	VBOList;

	void Init();
	void BeginFrame();

	void RenderToOpenGL();
	void RenderToGame();
	
	void AddPoint( const Vector3f & v, const obColor & col, float size = 1.0f );
	void AddPoints( const Vector3List & verts, obColor col, float size = 1.0f );
	void AddArrow( const Vector3f & v0, const Vector3f & v1, const obColor & col, float width = 1.0f );
	void AddLine( const Vector3f & v0, const Vector3f & v1, const obColor & col, float width = 1.0f );
	void AddLine( const Vector3List & v, const obColor & col, float width = 1.0f );
	void AddTri( const Triangle & tri );
	void AddTri( const Vector3f & v0, const Vector3f & v1, const Vector3f & v2, const obColor & col );
	void AddQuad( const Quad & primitive );
	void AddQuad( const Vector3f & v0, const Vector3f & v1, const Vector3f & v2, const Vector3f & v3, const obColor & col );
	void AddPolygonFilled( const Vector3List & verts, obColor col );
	void AddPolygonSilouette( const Vector3List & verts, obColor col );
	void AddCircle( const Vector3f & v, float radius, const obColor & col, const Vector3f & up = Vector3f::UNIT_Z );
	void AddString3d( const Vector3f & v, const obColor & col, const char * str );
	void AddString3dRadius( const Vector3f & v, const obColor & col, float radius, const char * str );
	void AddString2d( const Vector2f & v, const obColor & col, const char * str );
	void AddAABB(const AxisAlignedBox3f &_aabb, const obColor &_color, AABB::Direction _dir = AABB::DIR_ALL);
	void AddAABB(const AABB &_aabb, const obColor &_color, AABB::Direction _dir = AABB::DIR_ALL);
	void AddOBB(const Box3f &_obb, const obColor &_color, AABB::Direction _dir = AABB::DIR_ALL);
	
	bool StaticBufferCreate( obuint32 & bufferId, const TriList & primitives );
	bool StaticBufferCreate( obuint32 & bufferId, const QuadList & primitives );
	void StaticBufferDelete( obuint32 bufferId );
	void StaticBufferDraw( obuint32 bufferId );
};

#endif
