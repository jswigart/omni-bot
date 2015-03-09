#include "RenderBuffer.h"
#include "Utilities.h"

#ifdef WIN32
#include <windows.h>
#endif

#include <float.h>

#include <gl/GL.h>
#include <gl/GLU.h>
#include "glext.h"

//////////////////////////////////////////////////////////////////
static const float VIEW_DISTANCE = 1024.0f;
static const float DEFAULT_TIME = 0.0f;
static const float TEXT_SCALE = 10.0f;
static const float TEXT_DIST = 2048.0f;
//////////////////////////////////////////////////////////////////

static int nextBufferId = 0;

struct VBOVert
{
	float p[ 3 ];
	float c[ 4 ];

	void SetPos( float _x, float _y, float _z )
	{
		p[ 0 ] = _x;
		p[ 1 ] = _y;
		p[ 2 ] = _z;
	}
	void SetColor( const obColor col )
	{
		c[ 0 ] = col.rF();
		c[ 1 ] = col.gF();
		c[ 2 ] = col.bF();
		c[ 3 ] = col.aF();
	}
};

typedef std::vector<unsigned int>	Indices;

// partition for rendering acceleration
struct Chunk
{
	AxisAlignedBox3f	mAABB;
	Indices				mIndices;
};

typedef std::vector<VBOVert>		VBOVerts;
typedef std::vector<Chunk>			Chunks;

struct VBO
{
	VBO()
		: mBufferId( 0 )
	{
	}
	~VBO()
	{
	}

	GLuint		mBufferId;
	GLenum		mType;
	GLuint		mNumPrimitives;
	GLuint		mNumVertsPerPrimitive;
	VBOVerts	mVerts;
};

typedef std::multimap<obuint32, VBO> VBOMap;
VBOMap	vbos;

#ifdef WIN32
PFNGLGENBUFFERSARBPROC		glGenBuffersARB = NULL;
PFNGLBINDBUFFERARBPROC		glBindBufferARB = NULL;
PFNGLBUFFERDATAARBPROC		glBufferDataARB = NULL;
PFNGLDELETEBUFFERSARBPROC	glDeleteBuffersARB = NULL;

GLuint						textListBase = 0;
GLYPHMETRICSFLOAT			glyphMetrics[ 256 ];
#endif

// helpers to minimize state changes
static void CheckPointSize( float & current, const float expected )
{
	if ( current != expected )
	{
		glPointSize( expected );
		current = expected;
	}
}
static void CheckLineWidth( float & current, const float expected )
{
	if ( current != expected )
	{
		glLineWidth( expected );
		current = expected;
	}
}
static void CheckColor( obColor & current, const obColor expected )
{
	if ( current != expected )
	{
		glColor4ub( expected.r(), expected.g(), expected.b(), expected.a() );
		current = expected;
	}
}

static void CheckGLError( const char * msg )
{
	GLenum err = GL_NO_ERROR;
	while ( ( err = glGetError() ) != GL_NO_ERROR )
		OutputDebugString( va( "OpenGL Error(%s) - %d\n", msg, err ) );
}

namespace RenderBuffer
{
	static PointList		mPointList;
	static LineList			mLineList;
	static TriList			mTriList;
	static CircleList		mCircleList;
	static StringList2d		mStringList2d;
	static StringList3d		mStringList3d;
	static VBOList			mVBOList;


	void Init()
	{
#ifdef WIN32
		glGenBuffersARB = (PFNGLGENBUFFERSARBPROC)wglGetProcAddress( "glGenBuffersARB" );
		glBindBufferARB = (PFNGLBINDBUFFERARBPROC)wglGetProcAddress( "glBindBufferARB" );
		glBufferDataARB = (PFNGLBUFFERDATAARBPROC)wglGetProcAddress( "glBufferDataARB" );
		glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)wglGetProcAddress( "glDeleteBuffersARB" );

		const int height = 16;
		HFONT font = CreateFont( height,
			0,
			0,
			0,
			FW_BOLD, //m_bold? FW_BOLD : FW_NORMAL,
			0, //m_italic? TRUE : FALSE,
			0, //m_underline? TRUE : FALSE,
			0, //m_strokeout? TRUE : FALSE,
			ANSI_CHARSET,
			OUT_TT_PRECIS,
			CLIP_DEFAULT_PRECIS,
			ANTIALIASED_QUALITY,
			FF_DONTCARE | DEFAULT_PITCH,
			"Courier New" );

		HDC hDC = wglGetCurrentDC();

		HFONT oldfont = (HFONT)SelectObject( hDC, font );

		textListBase = glGenLists( 256 );
		if ( FALSE == wglUseFontOutlines( hDC, 0, 255, textListBase, 0.0f, 0.0f, WGL_FONT_POLYGONS, glyphMetrics ) )
		{
			glDeleteLists( textListBase, 256 );
			textListBase = 0;
		}
		/*if ( FALSE == wglUseFontBitmaps( hDC, 0, 255, textListBase ) )
		{
		glDeleteLists( textListBase, 256 );
		textListBase = 0;
		}*/

		SelectObject( hDC, oldfont );
		DeleteObject( font );
#endif
	}

	void BeginFrame()
	{
		mPointList.resize( 0 );
		mLineList.resize( 0 );
		mTriList.resize( 0 );
		mCircleList.resize( 0 );
		mStringList2d.resize( 0 );
		mStringList3d.resize( 0 );
		mVBOList.resize( 0 );
	}

	void AddPoint( const Vector3f & v, const obColor & col, float size )
	{
		Point prim;
		prim.v[ 0 ] = v;
		prim.c = col;
		prim.pointSize = size;
		mPointList.push_back( prim );
	}
	void AddPoints( const Vector3List & verts, obColor col, float size )
	{
		for ( size_t i = 0; i < verts.size(); ++i )
			AddPoint( verts[ i ], col, size );
	}
	void AddArrow( const Vector3f & v0, const Vector3f & v1, const obColor & col, float width )
	{
		OBASSERT( v0.IsValid(), "Invalid Vector" );
		OBASSERT( v1.IsValid(), "Invalid Vector" );

		Line prim;
		prim.v[ 0 ] = v0;
		prim.v[ 1 ] = v1;
		prim.c = col;
		prim.lineWidth = width;
		mLineList.push_back( prim );

		Vector3f dir = v1 - v0;
		dir.Normalize();
		Vector3f side = dir.Cross( Vector3f::UNIT_Z );

		prim.v[ 0 ] = v1 - dir * 8 - side * 4;
		mLineList.push_back( prim );

		prim.v[ 0 ] = v1 - dir * 8 + side * 4;
		mLineList.push_back( prim );

		// arrowheads todo
	}
	void AddLine( const Vector3f & v0, const Vector3f & v1, const obColor & col, float width )
	{
		OBASSERT( v0.IsValid(), "Invalid Vector" );
		OBASSERT( v1.IsValid(), "Invalid Vector" );

		Line prim;
		prim.v[ 0 ] = v0;
		prim.v[ 1 ] = v1;
		prim.c = col;
		prim.lineWidth = width;
		mLineList.push_back( prim );
	}
	void AddLine( const Vector3List & v, const obColor & col, float width )
	{
		Line prim;
		prim.c = col;
		prim.lineWidth = width;

		for ( size_t i = 1; i < v.size(); ++i )
		{
			prim.v[ 0 ] = v[ i - 1 ];
			prim.v[ 1 ] = v[ i ];
			mLineList.push_back( prim );
		}
	}
	void AddTri( const Triangle & tri )
	{
		mTriList.push_back( tri );
	}
	void AddTri( const Vector3f & v0, const Vector3f & v1, const Vector3f & v2, const obColor & col )
	{
		Triangle prim;
		prim.v[ 0 ] = v0;
		prim.v[ 1 ] = v1;
		prim.v[ 2 ] = v2;
		prim.c = col;
		mTriList.push_back( prim );
	}
	void AddQuad( const Quad & primitive )
	{
		Triangle prim;
		prim.c = primitive.c;

		prim.v[ 0 ] = primitive.v[ 0 ];
		prim.v[ 1 ] = primitive.v[ 1 ];
		prim.v[ 2 ] = primitive.v[ 2 ];
		mTriList.push_back( prim );

		prim.v[ 0 ] = primitive.v[ 0 ];
		prim.v[ 1 ] = primitive.v[ 2 ];
		prim.v[ 2 ] = primitive.v[ 3 ];
		mTriList.push_back( prim );
	}
	void AddQuad( const Vector3f & v0, const Vector3f & v1, const Vector3f & v2, const Vector3f & v3, const obColor & col )
	{
		Triangle prim;
		prim.c = col;

		prim.v[ 0 ] = v0;
		prim.v[ 1 ] = v1;
		prim.v[ 2 ] = v2;
		mTriList.push_back( prim );

		prim.v[ 0 ] = v0;
		prim.v[ 1 ] = v2;
		prim.v[ 2 ] = v3;
		mTriList.push_back( prim );
	}

	void AddPolygonFilled( const Vector3List & verts, obColor col )
	{
		if ( verts.size() > 1 )
		{
			Triangle prim;
			prim.c = col;

			// use the center of the poly as a pivot for the polygon
			// to avoid degenerates
			Vector3f center = verts[ 0 ];
			for ( size_t i = 1; i < verts.size(); ++i )
				center += verts[ i ];
			center /= (float)verts.size();

			for ( size_t i = 1; i <= verts.size(); ++i )
			{
				prim.v[ 0 ] = center;
				prim.v[ 1 ] = verts[ i - 1 ];
				prim.v[ 2 ] = verts[ i%verts.size() ];
				mTriList.push_back( prim );
			}
		}
	}

	void AddPolygonSilouette( const Vector3List & verts, obColor col )
	{
		if ( verts.size() > 1 )
		{
			Line prim;
			prim.c = col;
			prim.v[ 0 ] = verts[ verts.size() - 1 ];
			prim.v[ 1 ] = verts[ 0 ];
			mLineList.push_back( prim );
			for ( size_t i = 1; i < verts.size(); ++i )
			{
				prim.v[ 0 ] = verts[ i - 1 ];
				prim.v[ 1 ] = verts[ i ];
				mLineList.push_back( prim );
			}
		}
	}

	void AddCircle( const Vector3f & v, float radius, const obColor & col, const Vector3f & up )
	{
		Circle prim;
		prim.v = v;
		prim.c = col;
		prim.radius = radius;
		prim.up = up;
		mCircleList.push_back( prim );
	}

	void AddString3d( const Vector3f & v, const obColor & col, const char * str )
	{
		Str3d prim;
		prim.v = v;
		prim.c = col;
		prim.str = str;
		prim.radius = TEXT_DIST;
		mStringList3d.push_back( prim );
	}

	void AddString3dRadius( const Vector3f & v, const obColor & col, float radius, const char * str )
	{
		Str3d prim;
		prim.v = v;
		prim.c = col;
		prim.str = str;
		prim.radius = radius;
		mStringList3d.push_back( prim );
	}

	void AddString2d( const Vector2f & v, const obColor & col, const char * str )
	{
		Str2d prim;
		prim.v = v;
		prim.c = col;
		prim.str = str;
		mStringList2d.push_back( prim );
	}
	void AddAABB( const AxisAlignedBox3f &_aabb, const obColor &_color, AABB::Direction _dir/* = AABB::DIR_ALL*/ )
	{
		AABB aabb;
		aabb.Set( _aabb.Min, _aabb.Max );
		AddAABB( aabb, _color, _dir );
	}
	void AddAABB( const AABB &_aabb, const obColor &_color, AABB::Direction _dir/* = AABB::DIR_ALL*/ )
	{
		//if(g_EngineFuncs->DebugBox(Vector3f::ZERO,Vector3f::ZERO,COLOR::WHITE,0.f))
		//{
		//	if(_dir == AABB::DIR_ALL)
		//	{
		//		g_EngineFuncs->DebugBox(_aabb.m_Mins,_aabb.m_Maxs,_color,DEFAULT_TIME);
		//		return;
		//	}

		Vector3f vVertex[ 8 ] = { Vector3f::ZERO };

		vVertex[ 0 ] = Vector3f( _aabb.m_Mins[ 0 ], _aabb.m_Mins[ 1 ], _aabb.m_Mins[ 2 ] );
		vVertex[ 1 ] = Vector3f( _aabb.m_Maxs[ 0 ], _aabb.m_Mins[ 1 ], _aabb.m_Mins[ 2 ] );
		vVertex[ 2 ] = Vector3f( _aabb.m_Maxs[ 0 ], _aabb.m_Maxs[ 1 ], _aabb.m_Mins[ 2 ] );
		vVertex[ 3 ] = Vector3f( _aabb.m_Mins[ 0 ], _aabb.m_Maxs[ 1 ], _aabb.m_Mins[ 2 ] );

		vVertex[ 4 ] = Vector3f( _aabb.m_Mins[ 0 ], _aabb.m_Mins[ 1 ], _aabb.m_Maxs[ 2 ] );
		vVertex[ 5 ] = Vector3f( _aabb.m_Maxs[ 0 ], _aabb.m_Mins[ 1 ], _aabb.m_Maxs[ 2 ] );
		vVertex[ 6 ] = Vector3f( _aabb.m_Maxs[ 0 ], _aabb.m_Maxs[ 1 ], _aabb.m_Maxs[ 2 ] );
		vVertex[ 7 ] = Vector3f( _aabb.m_Mins[ 0 ], _aabb.m_Maxs[ 1 ], _aabb.m_Maxs[ 2 ] );

		// Top
		if ( _dir == AABB::DIR_TOP || _dir == AABB::DIR_ALL )
		{
			AddLine( vVertex[ 4 ], vVertex[ 5 ], _color, DEFAULT_TIME );
			AddLine( vVertex[ 5 ], vVertex[ 6 ], _color, DEFAULT_TIME );
			AddLine( vVertex[ 6 ], vVertex[ 7 ], _color, DEFAULT_TIME );
			AddLine( vVertex[ 7 ], vVertex[ 4 ], _color, DEFAULT_TIME );
		}

		// Bottom
		if ( _dir == AABB::DIR_BOTTOM || _dir == AABB::DIR_ALL )
		{
			AddLine( vVertex[ 0 ], vVertex[ 1 ], _color, DEFAULT_TIME );
			AddLine( vVertex[ 1 ], vVertex[ 2 ], _color, DEFAULT_TIME );
			AddLine( vVertex[ 2 ], vVertex[ 3 ], _color, DEFAULT_TIME );
			AddLine( vVertex[ 3 ], vVertex[ 0 ], _color, DEFAULT_TIME );
		}

		// Sides
		if ( _dir == AABB::DIR_ALL )
		{
			AddLine( vVertex[ 4 ], vVertex[ 0 ], _color, DEFAULT_TIME );
			AddLine( vVertex[ 5 ], vVertex[ 1 ], _color, DEFAULT_TIME );
			AddLine( vVertex[ 6 ], vVertex[ 2 ], _color, DEFAULT_TIME );
			AddLine( vVertex[ 7 ], vVertex[ 3 ], _color, DEFAULT_TIME );
		}
	}

	void AddOBB( const Box3f &_obb, const obColor &_color, AABB::Direction _dir )
	{
		Vector3f vertices[ 8 ];
		_obb.ComputeVertices( vertices );

		// bottom
		if ( _dir == AABB::DIR_BOTTOM || _dir == AABB::DIR_ALL )
		{
			AddLine( vertices[ 0 ], vertices[ 1 ], _color );
			AddLine( vertices[ 1 ], vertices[ 2 ], _color );
			AddLine( vertices[ 2 ], vertices[ 3 ], _color );
			AddLine( vertices[ 3 ], vertices[ 0 ], _color );
		}

		// top
		if ( _dir == AABB::DIR_TOP || _dir == AABB::DIR_ALL )
		{
			AddLine( vertices[ 4 ], vertices[ 5 ], _color );
			AddLine( vertices[ 5 ], vertices[ 6 ], _color );
			AddLine( vertices[ 6 ], vertices[ 7 ], _color );
			AddLine( vertices[ 7 ], vertices[ 4 ], _color );
		}

		//verts
		if ( _dir == AABB::DIR_ALL )
		{
			AddLine( vertices[ 0 ], vertices[ 4 ], _color );
			AddLine( vertices[ 1 ], vertices[ 5 ], _color );
			AddLine( vertices[ 2 ], vertices[ 6 ], _color );
			AddLine( vertices[ 3 ], vertices[ 7 ], _color );
		}
	}

	bool StaticBufferCreate( obuint32 & bufferId, const TriList & primitives )
	{
		StaticBufferDelete( bufferId );

		if ( bufferId == 0 )
			bufferId = ++nextBufferId;
		else
			StaticBufferDelete( bufferId );

		const size_t maxPrimitives = 5000;
		for ( size_t p = 0; p < primitives.size(); p += maxPrimitives )
		{
			const size_t primCount = std::min( maxPrimitives, primitives.size() - p );

			VBOMap::iterator it = vbos.insert( std::make_pair( bufferId, VBO() ) );
			VBO & vb = it->second;
			vb.mType = GL_TRIANGLES;
			vb.mNumVertsPerPrimitive = 3;
			vb.mNumPrimitives = primCount;
			vb.mVerts.reserve( vb.mNumPrimitives * vb.mNumVertsPerPrimitive );

			for ( size_t i = 0; i < primCount; ++i )
			{
				const Triangle & q = primitives[ p + i ];

				VBOVert vert;
				vert.SetColor( q.c );

				vert.SetPos( q.v[ 0 ].X(), q.v[ 0 ].Y(), q.v[ 0 ].Z() );
				vb.mVerts.push_back( vert );
				vert.SetPos( q.v[ 1 ].X(), q.v[ 1 ].Y(), q.v[ 1 ].Z() );
				vb.mVerts.push_back( vert );
				vert.SetPos( q.v[ 2 ].X(), q.v[ 2 ].Y(), q.v[ 2 ].Z() );
				vb.mVerts.push_back( vert );
			}

			if ( glGenBuffersARB != NULL &&
				glBufferDataARB != NULL &&
				glBindBufferARB != NULL &&
				glDeleteBuffersARB != NULL )
			{
				CheckGLError( "StaticBufferCreate1" );

				glPushClientAttrib( GL_CLIENT_ALL_ATTRIB_BITS );
				const GLsizeiptr vertexBytes = sizeof( VBOVert ) * vb.mVerts.size();

				glGenBuffersARB( 1, &vb.mBufferId );
				glBindBufferARB( GL_ARRAY_BUFFER_ARB, vb.mBufferId );
				glBufferDataARB( GL_ARRAY_BUFFER_ARB, vertexBytes, &vb.mVerts[ 0 ], GL_STATIC_DRAW_ARB );
				glBindBufferARB( GL_ARRAY_BUFFER_ARB, 0 );

				glPopClientAttrib();

				CheckGLError( "StaticBufferCreate2" );
			}
			else
			{
				vb.mBufferId = 0;
			}
		}

		/*
		VBOMap::iterator it = vbos.insert( std::make_pair( bufferId, VBO() ) );
		VBO & vb = it->second;
		vb.mType = GL_TRIANGLES;
		vb.mNumVertsPerPrimitive = 3;
		vb.mNumPrimitives = primitives.size();
		vb.mVerts.reserve( vb.mNumPrimitives * vb.mNumVertsPerPrimitive );

		for ( size_t i = 0; i < primitives.size(); ++i )
		{
		const Triangle & q = primitives[ i ];

		VBOVert vert;
		vert.SetColor( q.c );

		vert.SetPos( q.v[ 0 ].X(), q.v[ 0 ].Y(), q.v[ 0 ].Z() );
		vb.mVerts.push_back( vert );
		vert.SetPos( q.v[ 1 ].X(), q.v[ 1 ].Y(), q.v[ 1 ].Z() );
		vb.mVerts.push_back( vert );
		vert.SetPos( q.v[ 2 ].X(), q.v[ 2 ].Y(), q.v[ 2 ].Z() );
		vb.mVerts.push_back( vert );
		}

		if ( glGenBuffersARB != NULL &&
		glBufferDataARB != NULL &&
		glBindBufferARB != NULL &&
		glDeleteBuffersARB != NULL )
		{
		CheckGLError( "StaticBufferCreate1" );

		glPushClientAttrib( GL_CLIENT_ALL_ATTRIB_BITS );
		const GLsizeiptr vertexBytes = sizeof(VBOVert) * vb.mVerts.size();

		glGenBuffersARB( 1, &vb.mBufferId );
		glBindBufferARB( GL_ARRAY_BUFFER_ARB, vb.mBufferId );
		glBufferDataARB( GL_ARRAY_BUFFER_ARB, vertexBytes, &vb.mVerts[ 0 ], GL_STATIC_DRAW_ARB );
		glBindBufferARB( GL_ARRAY_BUFFER_ARB, 0 );

		glPopClientAttrib();

		CheckGLError( "StaticBufferCreate2" );
		}
		else
		{
		vb.mBufferId = 0;
		}
		*/
		return true;
	}

	bool StaticBufferCreate( obuint32 & bufferId, const QuadList & primitives )
	{
		StaticBufferDelete( bufferId );

		if ( bufferId == 0 )
			bufferId = ++nextBufferId;
		else
			StaticBufferDelete( bufferId );

		const size_t maxPrimitives = 5000;
		for ( size_t p = 0; p < primitives.size(); p += maxPrimitives )
		{
			const size_t primCount = std::min( maxPrimitives, primitives.size() - p );

			VBOMap::iterator it = vbos.insert( std::make_pair( bufferId, VBO() ) );
			VBO & vb = it->second;
			vb.mType = GL_TRIANGLES;
			vb.mNumVertsPerPrimitive = 3;
			vb.mNumPrimitives = primCount * 2;
			vb.mVerts.reserve( vb.mNumPrimitives * vb.mNumVertsPerPrimitive );

			for ( size_t i = 0; i < primCount; ++i )
			{
				const Quad & q = primitives[ p + i ];

				VBOVert vert;
				vert.SetColor( q.c );

				vert.SetPos( q.v[ 0 ].X(), q.v[ 0 ].Y(), q.v[ 0 ].Z() );
				vb.mVerts.push_back( vert );
				vert.SetPos( q.v[ 1 ].X(), q.v[ 1 ].Y(), q.v[ 1 ].Z() );
				vb.mVerts.push_back( vert );
				vert.SetPos( q.v[ 2 ].X(), q.v[ 2 ].Y(), q.v[ 2 ].Z() );
				vb.mVerts.push_back( vert );

				vert.SetPos( q.v[ 0 ].X(), q.v[ 0 ].Y(), q.v[ 0 ].Z() );
				vb.mVerts.push_back( vert );
				vert.SetPos( q.v[ 2 ].X(), q.v[ 2 ].Y(), q.v[ 2 ].Z() );
				vb.mVerts.push_back( vert );
				vert.SetPos( q.v[ 3 ].X(), q.v[ 3 ].Y(), q.v[ 3 ].Z() );
				vb.mVerts.push_back( vert );
			}

			if ( glGenBuffersARB != NULL &&
				glBufferDataARB != NULL &&
				glBindBufferARB != NULL &&
				glDeleteBuffersARB != NULL )
			{
				CheckGLError( "StaticBufferCreate1" );

				glPushClientAttrib( GL_CLIENT_ALL_ATTRIB_BITS );
				const GLsizeiptr vertexBytes = sizeof( VBOVert ) * vb.mVerts.size();

				glGenBuffersARB( 1, &vb.mBufferId );
				glBindBufferARB( GL_ARRAY_BUFFER_ARB, vb.mBufferId );
				glBufferDataARB( GL_ARRAY_BUFFER_ARB, vertexBytes, &vb.mVerts[ 0 ], GL_STATIC_DRAW_ARB );
				glBindBufferARB( GL_ARRAY_BUFFER_ARB, 0 );

				glPopClientAttrib();

				CheckGLError( "StaticBufferCreate2" );
			}
			else
			{
				vb.mBufferId = 0;
			}
		}

		/*
		VBOMap::iterator it = vbos.insert( std::make_pair( bufferId, VBO() ) );
		VBO & vb = it->second;
		vb.mType = GL_TRIANGLES;
		vb.mNumVertsPerPrimitive = 3;
		vb.mNumPrimitives = primitives.size() * 2;
		vb.mVerts.reserve( vb.mNumPrimitives * vb.mNumVertsPerPrimitive );

		for ( size_t i = 0; i < primitives.size(); ++i )
		{
		const Quad & q = primitives[ i ];

		VBOVert vert;
		vert.SetColor( q.c );

		vert.SetPos( q.v[ 0 ].X(), q.v[ 0 ].Y(), q.v[ 0 ].Z() );
		vb.mVerts.push_back( vert );
		vert.SetPos( q.v[ 1 ].X(), q.v[ 1 ].Y(), q.v[ 1 ].Z() );
		vb.mVerts.push_back( vert );
		vert.SetPos( q.v[ 2 ].X(), q.v[ 2 ].Y(), q.v[ 2 ].Z() );
		vb.mVerts.push_back( vert );

		vert.SetPos( q.v[ 0 ].X(), q.v[ 0 ].Y(), q.v[ 0 ].Z() );
		vb.mVerts.push_back( vert );
		vert.SetPos( q.v[ 2 ].X(), q.v[ 2 ].Y(), q.v[ 2 ].Z() );
		vb.mVerts.push_back( vert );
		vert.SetPos( q.v[ 3 ].X(), q.v[ 3 ].Y(), q.v[ 3 ].Z() );
		vb.mVerts.push_back( vert );
		}

		if ( glGenBuffersARB != NULL &&
		glBufferDataARB != NULL &&
		glBindBufferARB != NULL &&
		glDeleteBuffersARB != NULL )
		{
		CheckGLError( "StaticBufferCreate1" );

		glPushClientAttrib( GL_CLIENT_ALL_ATTRIB_BITS );
		const GLsizeiptr vertexBytes = sizeof(VBOVert) * vb.mVerts.size();

		glGenBuffersARB( 1, &vb.mBufferId );
		glBindBufferARB( GL_ARRAY_BUFFER_ARB, vb.mBufferId );
		glBufferDataARB( GL_ARRAY_BUFFER_ARB, vertexBytes, &vb.mVerts[ 0 ], GL_STATIC_DRAW_ARB );
		glBindBufferARB( GL_ARRAY_BUFFER_ARB, 0 );

		glPopClientAttrib();

		CheckGLError( "StaticBufferCreate2" );
		}
		else
		{
		vb.mBufferId = 0;
		}*/

		return true;
	}

	void StaticBufferDelete( obuint32 bufferId )
	{
		VBOMap::_Pairii itR = vbos.equal_range( bufferId );
		for ( VBOMap::iterator it = itR.first; it != itR.second; ++it )
		{
			if ( glDeleteBuffersARB != NULL )
				glDeleteBuffersARB( 1, &it->second.mBufferId );
		}
		vbos.erase( itR.first, itR.second );
	}

	void StaticBufferDraw( obuint32 bufferId )
	{
		mVBOList.push_back( bufferId );
	}

	void RenderToOpenGL()
	{
		Vector3f vEyePos;
		if ( !Utils::GetLocalEyePosition( vEyePos ) )
			return;

		CheckGLError( "StartRenderToOpenGL" );

		/*static float renderDist = 2048;
		const float dSq = renderDist * renderDist;*/

		CheckGLError( "State0" );

		glPushAttrib( GL_ALL_ATTRIB_BITS );
		glDisable( GL_TEXTURE_2D );
		glDisable( GL_LIGHTING );
		glDisable( GL_ALPHA_TEST );
		glEnable( GL_BLEND );
		glEnable( GL_DEPTH_TEST );
		glDisable( GL_CULL_FACE );

		CheckGLError( "State1" );

		//glEnable( GL_CULL_FACE );
		//glCullFace( GL_BACK );
		//glFrontFace( GL_CCW );
		//glPolygonMode( GL_FRONT, GL_FILL );

		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		glDepthFunc( GL_LEQUAL );

		CheckGLError( "State2" );

		obColor col = COLOR::CYAN;
		CheckColor( col, COLOR::WHITE );

		for ( size_t i = 0; i < mPointList.size(); ++i )
		{
			const Point & prim = mPointList[ i ];
			AddLine(
				prim.v[ 0 ],
				prim.v[ 0 ] + Vector3f( 0.0f, 0.0f, prim.pointSize ),
				prim.c,
				prim.pointSize );
		}

		if ( mLineList.size() > 0 )
		{
			float sz = 0.0;
			CheckLineWidth( sz, 1.0 );

			glBegin( GL_LINES );
			for ( size_t i = 0; i < mLineList.size(); ++i )
			{
				const Line & prim = mLineList[ i ];
				CheckLineWidth( sz, prim.lineWidth );
				CheckColor( col, prim.c );
				glVertex3fv( prim.v[ 0 ] );
				glVertex3fv( prim.v[ 1 ] );
			}
			glEnd();
		}

		if ( mTriList.size() > 0 )
		{
			glBegin( GL_TRIANGLES );
			for ( size_t i = 0; i < mTriList.size(); ++i )
			{
				const Triangle & prim = mTriList[ i ];
				CheckColor( col, prim.c );
				glVertex3fv( prim.v[ 0 ] );
				glVertex3fv( prim.v[ 1 ] );
				glVertex3fv( prim.v[ 2 ] );
			}
			glEnd();
		}

		if ( mCircleList.size() > 0 )
		{
			GLUquadric * q = gluNewQuadric();
			gluQuadricDrawStyle( q, GLU_SILHOUETTE );
			for ( size_t i = 0; i < mCircleList.size(); ++i )
			{
				const Circle & prim = mCircleList[ i ];
				CheckColor( col, prim.c );
				glPushMatrix();
				glTranslatef( prim.v.X(), prim.v.Y(), prim.v.Z() );
				gluDisk( q, 0, prim.radius, 12, 1 );
				glPopMatrix();
			}
			gluDeleteQuadric( q );
		}

		if ( mStringList3d.size() > 0 && textListBase != 0 )
		{
			glPushMatrix();
			glPushAttrib( GL_LIST_BIT ); // save the base list
			glListBase( textListBase );

			for ( size_t i = 0; i < mStringList3d.size(); ++i )
			{
				const Str3d & prim = mStringList3d[ i ];

				if ( ( vEyePos - prim.v ).Length() > TEXT_DIST )
					continue;

				CheckColor( col, prim.c );

				StringVector lines;
				Utils::Tokenize( prim.str, "\n", lines );

				std::vector<float> lineWidths;
				std::vector<float> lineHeights;

				// what's the largest line height
				float blockHeight = 0.0f;
				for ( size_t l = 0; l < lines.size(); ++l )
				{
					float lineWidth = 0.0f;
					float lineHeight = 0.0f;
					for ( size_t j = 0; j < lines[ l ].length(); ++j )
					{
						lineWidth += glyphMetrics[ lines[ l ][ j ] ].gmfCellIncX;
						lineHeight = std::max( lineHeight, glyphMetrics[ lines[ l ][ j ] ].gmfBlackBoxY );
					}

					blockHeight += lineHeight;

					lineWidths.push_back( lineWidth );
					lineHeights.push_back( lineHeight );
				}

				assert( lineWidths.size() == lines.size() );
				assert( lineHeights.size() == lines.size() );

				float yoffs = 0.0f;
				for ( size_t l = 0; l < lines.size(); ++l )
				{
					const float height = lineHeights[ l ] * TEXT_SCALE;
					const float width = lineWidths[ l ] * TEXT_SCALE;

					glPushMatrix();

					// calculate a bill board matrix so the text always faces the camera
					Vector3f look = vEyePos - prim.v;
					look.Z() = 0.0f;
					look.Normalize();
					Vector3f right = Vector3f::UNIT_Z.Cross( look );

					const GLfloat bb[ 16 ] =
					{
						right.X(), right.Y(), right.Z(), 0.0f,
						0.0f, 0.0f, 1.0f, 0.0f,
						look.X(), look.Y(), look.Z(), 0.0f,
						prim.v.X(), prim.v.Y(), prim.v.Z(), 1.0f
					};
					glMultMatrixf( bb );
					glTranslatef(
						-width * 0.5f,
						( blockHeight*0.5f ) - yoffs,
						0.0f );
					glScalef( TEXT_SCALE, TEXT_SCALE, TEXT_SCALE );
					glCallLists( lines[ l ].length(), GL_UNSIGNED_BYTE, lines[ l ].c_str() );
					glPopMatrix();

					yoffs += height;
				}
			}
			glPopMatrix();
			glPopAttrib(); // list base
		}

		//mStringList2d TODO

		static bool renderVBOs = true;
		if ( renderVBOs && mVBOList.size() > 0 )
		{
			glPushClientAttrib( GL_CLIENT_ALL_ATTRIB_BITS );

			CheckGLError( "ClientState" );
			glEnableClientState( GL_VERTEX_ARRAY );
			glEnableClientState( GL_COLOR_ARRAY );

			glDisableClientState( GL_NORMAL_ARRAY );
			glDisableClientState( GL_INDEX_ARRAY );
			glDisableClientState( GL_TEXTURE_COORD_ARRAY );
			glDisableClientState( GL_EDGE_FLAG_ARRAY );

			const size_t offsetP = offsetof( VBOVert, p );
			const size_t offsetC = offsetof( VBOVert, c );

			for ( size_t i = 0; i < mVBOList.size(); ++i )
			{
				VBOMap::_Pairii itR = vbos.equal_range( mVBOList[ i ] );
				for ( VBOMap::iterator it = itR.first; it != itR.second; ++it )
				{
					CheckGLError( "BindBuffer0" );

					glBindBufferARB( GL_ARRAY_BUFFER_ARB, it->second.mBufferId );

					CheckGLError( "BindBuffer0" );

					/*const GLvoid * vertPtr = &it->second.mVerts[ 0 ].p;
					const GLvoid * colrPtr = &it->second.mVerts[ 0 ].c;
					glVertexPointer( 3, GL_FLOAT, sizeof(VBOVert), vertPtr );
					glColorPointer( 4, GL_FLOAT, sizeof(VBOVert), colrPtr );*/

					glVertexPointer( 3, GL_FLOAT, sizeof( VBOVert ), (GLvoid*)offsetP );
					glColorPointer( 4, GL_FLOAT, sizeof( VBOVert ), (GLvoid*)offsetC );

					CheckGLError( "VertexPtr" );

					///////////////////////////////////////
					/*GLint arrayBind, colorBind, vertexBind,
					normalBind, elementBind, textureBind,
					indexBind, edgeflagBind,
					color2Bind, fogBind;
					glGetIntegerv( GL_ARRAY_BUFFER_BINDING, &arrayBind );
					glGetIntegerv( GL_COLOR_ARRAY_BUFFER_BINDING, &colorBind );
					glGetIntegerv( GL_VERTEX_ARRAY_BUFFER_BINDING, &vertexBind );
					glGetIntegerv( GL_NORMAL_ARRAY_BUFFER_BINDING, &normalBind );
					glGetIntegerv( GL_ELEMENT_ARRAY_BUFFER_BINDING, &elementBind );
					glGetIntegerv( GL_TEXTURE_COORD_ARRAY_BUFFER_BINDING, &textureBind );
					glGetIntegerv( GL_INDEX_ARRAY_BUFFER_BINDING, &indexBind );
					glGetIntegerv( GL_EDGE_FLAG_ARRAY_BUFFER_BINDING, &edgeflagBind );
					glGetIntegerv( GL_SECONDARY_COLOR_ARRAY_BUFFER_BINDING, &color2Bind );
					glGetIntegerv( GL_FOG_COORDINATE_ARRAY_BUFFER_BINDING, &fogBind );

					OutputDebugString( va( "bind: %d %d %d %d %d %d %d %d %d %d\n",
					arrayBind, colorBind, vertexBind,
					normalBind, elementBind, textureBind,
					indexBind, edgeflagBind,
					color2Bind, fogBind ) );*/
					///////////////////////////////////////

					// whole batch
					/*OutputDebugString( va( "glDrawArrays(%d, %d, %d) %d\n",
					it->second.mType,
					0,
					it->second.mNumVertsPerPrimitive * it->second.mNumPrimitives,
					it->second.mVerts.size() ) );*/

					glDrawArrays( it->second.mType, 0, it->second.mNumVertsPerPrimitive * it->second.mNumPrimitives );

					/*const size_t maxPrimitives = 5000;
					for ( size_t p = 0; p < it->second.mNumPrimitives; p += maxPrimitives )
					{
					const size_t primBatch = std::min( it->second.mNumPrimitives - p, maxPrimitives );

					CheckGLError( "DrawArrays0" );

					OutputDebugString( va( "glDrawArrays(%d, %d, %d) %d\n",
					it->second.mType,
					p * it->second.mNumVertsPerPrimitive,
					primBatch * it->second.mNumVertsPerPrimitive,
					it->second.mNumVertsPerPrimitive * it->second.mNumPrimitives ) );

					glDrawArrays(
					it->second.mType,
					p * it->second.mNumVertsPerPrimitive,
					primBatch * it->second.mNumVertsPerPrimitive );

					CheckGLError( "DrawArrays1" );
					}*/


					// unbind the buffer
					glBindBufferARB( GL_ARRAY_BUFFER_ARB, 0 );

					CheckGLError( "BindBuffer1" );

					CheckGLError( "PostRender" );
				}
			}

			glPopClientAttrib();
		}

		glPopAttrib();

		//CheckGLError( "EndRenderToOpenGL" );
	}

	void RenderToGame()
	{
		Vector3f vEyePos;
		if ( !Utils::GetLocalEyePosition( vEyePos ) )
			return;

		AxisAlignedBox3f viewBox;
		viewBox.Clear();
		viewBox.ExpandPt( vEyePos );
		viewBox.Extend( VIEW_DISTANCE*0.5f, VIEW_DISTANCE*0.5f, VIEW_DISTANCE*0.5f );

		// vbos first bc this populates the other primitive lists
		/*for ( size_t i = 0; i < mVBOList.size(); ++i )
		{
		VBOMap::const_iterator it = vbos.find( mVBOList[ i ] );
		if ( it == vbos.end() )
		continue;

		const VBO & vbo = it->second;

		for ( size_t c = 0; c < vbo.mChunks.size(); ++c )
		{
		const Chunk & ch = vbo.mChunks[ c ];
		if ( ch.mAABB.TestIntersection( viewBox ) )
		{
		AABB b( ch.mAABB.Min, ch.mAABB.Max );
		b.m_Mins[ 2 ] = b.m_Maxs[ 2 ] = vEyePos.Z();

		AddAABB( b, COLOR::CYAN, AABB::DIR_BOTTOM );

		for ( size_t x = 0; x < ch.mIndices.size(); ++x )
		{
		switch ( vbo.mType )
		{
		case GL_POINTS:
		AddPoint(
		vbo.mVerts[ ch.mIndices[ x ] + 0 ].p,
		vbo.mVerts[ ch.mIndices[ x ] + 0 ].c );
		break;
		case GL_LINES:
		AddLine(
		vbo.mVerts[ ch.mIndices[ x ] + 0 ].p,
		vbo.mVerts[ ch.mIndices[ x ] + 1 ].p,
		vbo.mVerts[ ch.mIndices[ x ] + 0 ].c );
		break;
		case GL_TRIANGLES:
		AddTri(
		vbo.mVerts[ ch.mIndices[ x ] + 0 ].p,
		vbo.mVerts[ ch.mIndices[ x ] + 1 ].p,
		vbo.mVerts[ ch.mIndices[ x ] + 2 ].p,
		vbo.mVerts[ ch.mIndices[ x ] + 0 ].c );
		break;
		default:
		assert( 0 && "Unhandled Primitive" );
		}
		}
		}
		}
		}*/

		const float cullDistSq = VIEW_DISTANCE*VIEW_DISTANCE;

		for ( size_t i = 0; i < mPointList.size(); ++i )
		{
			const Point & prim = mPointList[ i ];

			if ( ( prim.v[ 0 ] - vEyePos ).SquaredLength() > cullDistSq )
				continue;

			g_EngineFuncs->DebugLine(
				prim.v[ 0 ],
				prim.v[ 0 ] + Vector3f( 0.0f, 0.0f, prim.pointSize ),
				prim.c,
				DEFAULT_TIME );
		}

		for ( size_t i = 0; i < mLineList.size(); ++i )
		{
			const Line & prim = mLineList[ i ];

			DistPoint3Segment3f dist( vEyePos, Segment3f( prim.v[ 0 ], prim.v[ 1 ] ) );
			if ( dist.GetSquared() > cullDistSq )
				continue;

			g_EngineFuncs->DebugLine(
				prim.v[ 0 ],
				prim.v[ 1 ],
				prim.c,
				DEFAULT_TIME );
		}

		int renderedTris = 0;
		for ( size_t i = 0; i < mTriList.size(); ++i )
		{
			const Triangle & prim = mTriList[ i ];

			/*DistPoint3Triangle3f dist( vEyePos, Triangle3f( prim.v[0],prim.v[1],prim.v[2] ) );
			if ( dist.GetSquared() > cullDistSq )
			continue;*/
			static bool noDepth = false;
			const int renderFlags = noDepth ? IEngineInterface::DR_NODEPTHTEST : 0;

			g_EngineFuncs->DebugPolygon( (const obVec3 *)prim.v, 3, prim.c, DEFAULT_TIME, renderFlags );
			++renderedTris;
		}

		for ( size_t i = 0; i < mCircleList.size(); ++i )
		{
			const Circle & prim = mCircleList[ i ];
			g_EngineFuncs->DebugRadius( prim.v, prim.radius, prim.c, DEFAULT_TIME );
		}

		for ( size_t i = 0; i < mStringList3d.size(); ++i )
		{
			const Str3d & prim = mStringList3d[ i ];

			if ( ( vEyePos - prim.v ).Length() > TEXT_DIST )
				continue;

			g_EngineFuncs->PrintScreenText( prim.v, DEFAULT_TIME, prim.c, prim.str.c_str() );
		}
	}
};
