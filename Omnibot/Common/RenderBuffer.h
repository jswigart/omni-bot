#pragma once

#ifndef RENDERBUFFER_H
#define RENDERBUFFER_H

#include "common.h"

class RenderBuffer
{
public:
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
	};

	typedef std::vector<Point>		PointList;
	typedef std::vector<Line>		LineList;
	typedef std::vector<Circle>		CircleList;
	typedef std::vector<Str3d>		StringList3d;
	typedef std::vector<Str2d>		StringList2d;
	typedef std::vector<obuint32>	VBOList;

	static void Init();
	static void BeginFrame();

	static void RenderToOpenGL();

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

	typedef std::vector<Triangle>	TriList;
	typedef std::vector<Quad>		QuadList;

	static void AddPoint( const Vector3f & v, const obColor & col, float size = 1.0f );
	static void AddPoints( const Vector3List & verts, obColor col, float size = 1.0f );
	static void AddArrow( const Vector3f & v0, const Vector3f & v1, const obColor & col, float width = 1.0f );
	static void AddLine( const Vector3f & v0, const Vector3f & v1, const obColor & col, float width = 1.0f );
	static void AddLine( const Vector3List & v, const obColor & col, float width = 1.0f );
	static void AddTri( const Vector3f & v0, const Vector3f & v1, const Vector3f & v2, const obColor & col );
	static void AddQuad( const Vector3f & v0, const Vector3f & v1, const Vector3f & v2, const Vector3f & v3, const obColor & col );
	static void AddPolygonFilled( const Vector3List & verts, obColor col );
	static void AddPolygonSilouette( const Vector3List & verts, obColor col );
	static void AddCircle( const Vector3f & v, const obColor & col, float radius, const Vector3f & up = Vector3f::UNIT_Z );
	static void AddString3d( const Vector3f & v, const obColor & col, const char * str );
	static void AddString2d( const Vector2f & v, const obColor & col, const char * str );
	static void AddAABB(const AABB &_aabb, const obColor &_color, AABB::Direction _dir = AABB::DIR_ALL);
	static void AddOBB(const Box3f &_obb, const obColor &_color, AABB::Direction _dir = AABB::DIR_ALL);

	//static bool StaticBufferCreate( obuint32 & bufferId, const TriList & primitives );
	static bool StaticBufferCreate( obuint32 & bufferId, const QuadList & primitives );
	static void StaticBufferDelete( obuint32 bufferId );
	static void StaticBufferDraw( obuint32 bufferId );
private:
	static PointList		mPointList;
	static LineList			mLineList;
	static TriList			mTriList;
	static QuadList			mQuadList;
	static CircleList		mCircleList;
	static StringList2d		mStringList2d;
	static StringList3d		mStringList3d;
	static VBOList			mVBOList;
};

#endif
