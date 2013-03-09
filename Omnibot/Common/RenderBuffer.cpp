#include "RenderBuffer.h"
#include "Utilities.h"

#ifdef WIN32
#include <windows.h>
#endif

#include <gl/GL.h>
#include <gl/GLU.h>
#include "glext.h"

const float DEFAULT_TIME = 0.01f;

struct VBOVert
{
	float p[3];
	float c[4];
};
typedef std::vector<VBOVert> VBOVerts;

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

typedef std::map<obuint32,VBO> VBOMap;
VBOMap	vbos;

#ifdef WIN32
PFNGLGENBUFFERSARBPROC		glGenBuffersARB = NULL;
PFNGLBINDBUFFERARBPROC		glBindBufferARB = NULL;
PFNGLBUFFERDATAARBPROC		glBufferDataARB = NULL;
PFNGLDELETEBUFFERSARBPROC	glDeleteBuffersARB = NULL;

GLuint						textListBase = 0;
GLYPHMETRICSFLOAT			glyphMetrics[256];
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

RenderBuffer::PointList		RenderBuffer::mPointList;
RenderBuffer::LineList		RenderBuffer::mLineList;
RenderBuffer::TriList		RenderBuffer::mTriList;
RenderBuffer::QuadList		RenderBuffer::mQuadList;
RenderBuffer::CircleList	RenderBuffer::mCircleList;
RenderBuffer::StringList3d	RenderBuffer::mStringList3d;
RenderBuffer::StringList2d	RenderBuffer::mStringList2d;
RenderBuffer::VBOList		RenderBuffer::mVBOList;

void RenderBuffer::Init()
{
#ifdef WIN32
	glGenBuffersARB = (PFNGLGENBUFFERSARBPROC)wglGetProcAddress( "glGenBuffersARB" );
	glBindBufferARB = (PFNGLBINDBUFFERARBPROC)wglGetProcAddress( "glBindBufferARB" );
	glBufferDataARB = (PFNGLBUFFERDATAARBPROC)wglGetProcAddress( "glBufferDataARB" );
	glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)wglGetProcAddress( "glDeleteBuffersARB" );

	const int height = 16;
	HFONT font = CreateFont(height,
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
		FF_DONTCARE|DEFAULT_PITCH,
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

void RenderBuffer::BeginFrame()
{
	mPointList.resize( 0 );
	mLineList.resize( 0 );
	mTriList.resize( 0 );
	mQuadList.resize( 0 );
	mCircleList.resize( 0 );
	mStringList2d.resize( 0 );
	mStringList3d.resize( 0 );
	mVBOList.resize( 0 );
}

void RenderBuffer::AddPoint( const Vector3f & v, const obColor & col, float size )
{
	Point prim;
	prim.v[0] = v;
	prim.c = col;
	prim.pointSize = size;
	mPointList.push_back( prim );
}
void RenderBuffer::AddPoints( const Vector3List & verts, obColor col, float size )
{
	for ( size_t i = 0; i < verts.size(); ++i )
		AddPoint( verts[ i ], col, size );
}
void RenderBuffer::AddArrow( const Vector3f & v0, const Vector3f & v1, const obColor & col, float width )
{
	Line prim;
	prim.v[0] = v0;
	prim.v[1] = v1;
	prim.c = col;
	prim.lineWidth = width;
	mLineList.push_back( prim );

	// arrowheads todo
}
void RenderBuffer::AddLine( const Vector3f & v0, const Vector3f & v1, const obColor & col, float width )
{
	Line prim;
	prim.v[0] = v0;
	prim.v[1] = v1;
	prim.c = col;
	prim.lineWidth = width;
	mLineList.push_back( prim );
}
void RenderBuffer::AddLine( const Vector3List & v, const obColor & col, float width )
{
	Line prim;
	prim.c = col;
	prim.lineWidth = width;

	for ( size_t i = 1; i < v.size(); ++i )
	{
		prim.v[0] = v[ i-1 ];
		prim.v[1] = v[ i ];
		mLineList.push_back( prim );
	}
}
void RenderBuffer::AddTri( const Vector3f & v0, const Vector3f & v1, const Vector3f & v2, const obColor & col )
{
	Triangle prim;
	prim.v[0] = v0;
	prim.v[1] = v1;
	prim.v[2] = v2;
	prim.c = col;
	mTriList.push_back( prim );
}

void RenderBuffer::AddQuad( const Vector3f & v0, const Vector3f & v1, const Vector3f & v2, const Vector3f & v3, const obColor & col )
{
	Quad prim;
	prim.v[0] = v0;
	prim.v[1] = v1;
	prim.v[2] = v2;
	prim.v[3] = v3;
	prim.c = col;
	mQuadList.push_back( prim );
}

void RenderBuffer::AddPolygonFilled( const Vector3List & verts, obColor col )
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
			prim.v[0] = center;
			prim.v[1] = verts[ i-1 ];
			prim.v[2] = verts[ i%verts.size() ];
			mTriList.push_back( prim );
		}
	}
}

void RenderBuffer::AddPolygonSilouette( const Vector3List & verts, obColor col )
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

void RenderBuffer::AddCircle( const Vector3f & v, const obColor & col, float radius, const Vector3f & up )
{
	Circle prim;
	prim.v = v;
	prim.c = col;
	prim.radius = radius;
	prim.up = up;
	mCircleList.push_back( prim );
}

void RenderBuffer::AddString3d( const Vector3f & v, const obColor & col, const char * str )
{
	Str3d prim;
	prim.v = v;
	prim.c = col;
	prim.str = str;
	mStringList3d.push_back( prim );
}

void RenderBuffer::AddString2d( const Vector2f & v, const obColor & col, const char * str )
{
	Str2d prim;
	prim.v = v;
	prim.c = col;
	prim.str = str;
	mStringList2d.push_back( prim );
}

void RenderBuffer::AddAABB(const AABB &_aabb, const obColor &_color, AABB::Direction _dir/* = AABB::DIR_ALL*/)
{
	if(g_EngineFuncs->DebugBox(Vector3f::ZERO,Vector3f::ZERO,COLOR::WHITE,0.f))
	{
		if(_dir == AABB::DIR_ALL)
		{
			g_EngineFuncs->DebugBox(_aabb.m_Mins,_aabb.m_Maxs,_color,DEFAULT_TIME);
			return;
		}

		Vector3f vVertex[8] = { Vector3f::ZERO };

		vVertex[0] = Vector3f(_aabb.m_Mins[0], _aabb.m_Mins[1], _aabb.m_Mins[2]);
		vVertex[1] = Vector3f(_aabb.m_Maxs[0], _aabb.m_Mins[1], _aabb.m_Mins[2]);
		vVertex[2] = Vector3f(_aabb.m_Maxs[0], _aabb.m_Maxs[1], _aabb.m_Mins[2]);
		vVertex[3] = Vector3f(_aabb.m_Mins[0], _aabb.m_Maxs[1], _aabb.m_Mins[2]);

		vVertex[4] = Vector3f(_aabb.m_Mins[0], _aabb.m_Mins[1], _aabb.m_Maxs[2]);
		vVertex[5] = Vector3f(_aabb.m_Maxs[0], _aabb.m_Mins[1], _aabb.m_Maxs[2]);
		vVertex[6] = Vector3f(_aabb.m_Maxs[0], _aabb.m_Maxs[1], _aabb.m_Maxs[2]);
		vVertex[7] = Vector3f(_aabb.m_Mins[0], _aabb.m_Maxs[1], _aabb.m_Maxs[2]);

		// Top
		if(_dir == AABB::DIR_TOP || _dir == AABB::DIR_ALL)
		{
			RenderBuffer::AddLine(vVertex[4], vVertex[5], _color, DEFAULT_TIME);
			RenderBuffer::AddLine(vVertex[5], vVertex[6], _color, DEFAULT_TIME);
			RenderBuffer::AddLine(vVertex[6], vVertex[7], _color, DEFAULT_TIME);
			RenderBuffer::AddLine(vVertex[7], vVertex[4], _color, DEFAULT_TIME);
		}

		// Bottom
		if(_dir == AABB::DIR_BOTTOM || _dir == AABB::DIR_ALL)
		{
			RenderBuffer::AddLine(vVertex[0], vVertex[1], _color, DEFAULT_TIME);
			RenderBuffer::AddLine(vVertex[1], vVertex[2], _color, DEFAULT_TIME);
			RenderBuffer::AddLine(vVertex[2], vVertex[3], _color, DEFAULT_TIME);
			RenderBuffer::AddLine(vVertex[3], vVertex[0], _color, DEFAULT_TIME);
		}

		// Sides
		if(_dir == AABB::DIR_ALL)
		{
			RenderBuffer::AddLine(vVertex[4], vVertex[0], _color, DEFAULT_TIME);
			RenderBuffer::AddLine(vVertex[5], vVertex[1], _color, DEFAULT_TIME);
			RenderBuffer::AddLine(vVertex[6], vVertex[2], _color, DEFAULT_TIME);
			RenderBuffer::AddLine(vVertex[7], vVertex[3], _color, DEFAULT_TIME);
		}
	}
}

void RenderBuffer::AddOBB(const Box3f &_obb, const obColor &_color, AABB::Direction _dir)
{
	Vector3f vertices[8];
	_obb.ComputeVertices(vertices);

	// bottom
	if(_dir == AABB::DIR_BOTTOM || _dir == AABB::DIR_ALL)
	{
		RenderBuffer::AddLine(vertices[0], vertices[1], _color);
		RenderBuffer::AddLine(vertices[1], vertices[2], _color);
		RenderBuffer::AddLine(vertices[2], vertices[3], _color);
		RenderBuffer::AddLine(vertices[3], vertices[0], _color);
	}

	// top
	if(_dir == AABB::DIR_TOP || _dir == AABB::DIR_ALL)
	{
		RenderBuffer::AddLine(vertices[4], vertices[5], _color);
		RenderBuffer::AddLine(vertices[5], vertices[6], _color);
		RenderBuffer::AddLine(vertices[6], vertices[7], _color);
		RenderBuffer::AddLine(vertices[7], vertices[4], _color);
	}

	//verts
	if(_dir == AABB::DIR_ALL)
	{
		RenderBuffer::AddLine(vertices[0], vertices[4], _color);
		RenderBuffer::AddLine(vertices[1], vertices[5], _color);
		RenderBuffer::AddLine(vertices[2], vertices[6], _color);
		RenderBuffer::AddLine(vertices[3], vertices[7], _color);
	}
}

//bool RenderBuffer::StaticBufferCreate( obuint32 & bufferId, const TriList & primitives )
//{
//	static int nextBufferId = 0;
//
//	if ( glGenBuffersARB == NULL ||
//		glBufferDataARB == NULL ||
//		glBindBufferARB == NULL ||
//		glDeleteBuffersARB == NULL )
//		return false;
//
//	CheckGLError( "StaticBufferCreate1" );
//
//	StaticBufferDelete( bufferId );
//
//	if ( bufferId == 0 )
//		bufferId = ++nextBufferId;
//
//	glPushClientAttrib( GL_CLIENT_ALL_ATTRIB_BITS );
//
//	VBO vb;
//	vb.mType = GL_TRIANGLES;
//	vb.mNumVertsPerPrimitive = 3;
//	vb.mNumPrimitives = primitives.size();
//
//	glGenBuffersARB( 1, &vb.mBufferId );
//
//	std::vector<VBOVert> verts( vb.mNumPrimitives * vb.mNumVertsPerPrimitive * 4 );
//
//	verts.resize( 0 );
//
//	for ( size_t i = 0; i < primitives.size(); ++i )
//	{
//		const Triangle & q = primitives[ i ];
//		for ( size_t v = 0; v < vb.mNumVertsPerPrimitive; ++v )
//		{
//			VBOVert vert;
//			vert.p[ 0 ] = q.v[ v ].x;
//			vert.p[ 1 ] = q.v[ v ].y;
//			vert.p[ 2 ] = q.v[ v ].z;
//			vert.c[ 0 ] = q.c.rF();
//			vert.c[ 1 ] = q.c.gF();
//			vert.c[ 2 ] = q.c.bF();
//			vert.c[ 3 ] = q.c.aF();
//			verts.push_back( vert );
//			verts.push_back( vert );
//		}
//	}
//
//	const GLsizeiptr vertexBytes = sizeof(VBOVert) * verts.size();
//
//	glBindBufferARB( GL_ARRAY_BUFFER_ARB, vb.mBufferId );
//	glBufferDataARB( GL_ARRAY_BUFFER_ARB, vertexBytes, &verts[ 0 ], GL_STATIC_DRAW_ARB );
//
//	glBindBufferARB( GL_ARRAY_BUFFER_ARB, 0 );
//
//	CheckGLError( "StaticBufferCreate2" );
//
//	vbos.insert( std::make_pair( bufferId, vb ) );
//
//	glPopClientAttrib();
//	return true;
//}

bool RenderBuffer::StaticBufferCreate( obuint32 & bufferId, const QuadList & primitives )
{
	static int nextBufferId = 0;

	if ( glGenBuffersARB == NULL ||
		glBufferDataARB == NULL ||
		glBindBufferARB == NULL ||
		glDeleteBuffersARB == NULL )
		return false;

	CheckGLError( "StaticBufferCreate1" );

	StaticBufferDelete( bufferId );

	if ( bufferId == 0 )
		bufferId = ++nextBufferId;

	glPushClientAttrib( GL_CLIENT_ALL_ATTRIB_BITS );

	VBO vb;
	vb.mType = GL_QUADS;
	vb.mNumVertsPerPrimitive = 4;
	vb.mNumPrimitives = primitives.size();
	vb.mVerts.reserve( vb.mNumPrimitives * vb.mNumVertsPerPrimitive );

	for ( size_t i = 0; i < primitives.size(); ++i )
	{
		const Quad & q = primitives[ i ];
		for ( size_t v = 0; v < vb.mNumVertsPerPrimitive; ++v )
		{
			VBOVert vert;
			vert.p[ 0 ] = q.v[ v ].x;
			vert.p[ 1 ] = q.v[ v ].y;
			vert.p[ 2 ] = q.v[ v ].z;
			vert.c[ 0 ] = q.c.rF();
			vert.c[ 1 ] = q.c.gF();
			vert.c[ 2 ] = q.c.bF();
			vert.c[ 3 ] = q.c.aF();
			vb.mVerts.push_back( vert );

			assert( InRangeT( vert.p[ 0 ], -30000.0f, 30000.0f ) );
			assert( InRangeT( vert.p[ 1 ], -30000.0f, 30000.0f ) );
			assert( InRangeT( vert.p[ 2 ], -30000.0f, 30000.0f ) );

			assert( InRangeT( vert.c[ 0 ], 0.0f, 1.0f ) );
			assert( InRangeT( vert.c[ 1 ], 0.0f, 1.0f ) );
			assert( InRangeT( vert.c[ 2 ], 0.0f, 1.0f ) );
			assert( InRangeT( vert.c[ 3 ], 0.0f, 1.0f ) );
		}
	}

	const GLsizeiptr vertexBytes = sizeof(VBOVert) * vb.mVerts.size();

	glGenBuffersARB( 1, &vb.mBufferId );
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, vb.mBufferId );
	glBufferDataARB( GL_ARRAY_BUFFER_ARB, vertexBytes, &vb.mVerts[ 0 ], GL_STATIC_DRAW_ARB );
	glBindBufferARB( GL_ARRAY_BUFFER_ARB, 0 );

	CheckGLError( "StaticBufferCreate2" );

	vbos.insert( std::make_pair( bufferId, vb ) );

	glPopClientAttrib();
	return true;
}

void RenderBuffer::StaticBufferDelete( obuint32 bufferId )
{
	if ( glDeleteBuffersARB == NULL )
		return;

	VBOMap::iterator it = vbos.find( bufferId );
	if ( it != vbos.end() )
	{
		glDeleteBuffersARB( 1, &it->second.mBufferId );
		vbos.erase( it );
	}
}

void RenderBuffer::StaticBufferDraw( obuint32 bufferId )
{
	mVBOList.push_back( bufferId );
}

void RenderBuffer::RenderToOpenGL()
{
	Vector3f vEyePos;
	if ( !Utils::GetLocalEyePosition( vEyePos ) )
		return;

	/*static float renderDist = 2048;
	const float dSq = renderDist * renderDist;*/

	glPushAttrib( GL_ALL_ATTRIB_BITS );

	glDisable( GL_TEXTURE_2D );
	glDisable( GL_LIGHTING );
	glDisable( GL_CULL_FACE );
	glDisable( GL_ALPHA_TEST );

	glEnable( GL_BLEND );
	glEnable( GL_DEPTH_TEST );

	glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc( GL_LEQUAL );

	obColor col = COLOR::CYAN;
	CheckColor( col, COLOR::WHITE );

	if ( mPointList.size() > 0 )
	{
		float sz = 0.0;
		CheckPointSize( sz, 1.0 );

		glBegin( GL_POINTS );
		for( size_t i = 0; i < mPointList.size(); ++i )
		{
			const Point & prim = mPointList[ i ];
			CheckPointSize( sz, prim.pointSize );
			CheckColor( col, prim.c );
			glVertex3fv( prim.v[0] );
		}
		glEnd();
	}

	if ( mLineList.size() > 0 )
	{
		float sz = 0.0;
		CheckLineWidth( sz, 1.0 );

		glBegin( GL_LINES );
		for( size_t i = 0; i < mLineList.size(); ++i )
		{
			const Line & prim = mLineList[ i ];
			CheckLineWidth( sz, prim.lineWidth );
			CheckColor( col, prim.c );
			glVertex3fv( prim.v[0] );
			glVertex3fv( prim.v[1] );
		}
		glEnd();
	}

	if ( mTriList.size() > 0 )
	{
		glBegin( GL_TRIANGLES );
		for( size_t i = 0; i < mTriList.size(); ++i )
		{
			const Triangle & prim = mTriList[ i ];
			CheckColor( col, prim.c );
			glVertex3fv( prim.v[0] );
			glVertex3fv( prim.v[1] );
			glVertex3fv( prim.v[2] );
		}
		glEnd();
	}

	if ( mQuadList.size() > 0 )
	{
		glBegin( GL_QUADS );
		for( size_t i = 0; i < mQuadList.size(); ++i )
		{
			const Quad & prim = mQuadList[ i ];
			CheckColor( col, prim.c );
			glVertex3fv( prim.v[0] );
			glVertex3fv( prim.v[1] );
			glVertex3fv( prim.v[2] );
			glVertex3fv( prim.v[3] );
		}
		glEnd();
	}

	if ( mCircleList.size() > 0 )
	{
		GLUquadric * q = gluNewQuadric();
		gluQuadricDrawStyle( q, GLU_SILHOUETTE );
		for( size_t i = 0; i < mCircleList.size(); ++i )
		{
			const Circle & prim = mCircleList[ i ];
			CheckColor( col, prim.c );
			gluDisk( q, 0, prim.radius, 12, 1 );
		}
		gluDeleteQuadric( q );
	}

	if ( mStringList3d.size() > 0 && textListBase != 0 )
	{
		static const float textScale = 10.0f;
		static const float textDistance = 2048.0f;

		glPushMatrix();
		glPushAttrib( GL_LIST_BIT ); // save the base list
		glListBase( textListBase );

		for( size_t i = 0; i < mStringList3d.size(); ++i )
		{
			const Str3d & prim = mStringList3d[ i ];

			if ( ( vEyePos - prim.v ).Length() > textDistance )
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

			for ( size_t l = 0; l < lines.size(); ++l )
			{
				const float height = lineHeights[ l ] * textScale;
				const float width = lineWidths[ l ] * textScale;

				glPushMatrix();

				// calculate a bill board matrix so the text always faces the camera
				Vector3f look = vEyePos - prim.v;
				look.z = 0.0f;
				look.Normalize();
				Vector3f right = Vector3f::UNIT_Z.Cross( look );

				const GLfloat bb[16] =
				{
					right.x,	right.y,	right.z,	0.0f,
					0.0f,		0.0f,		1.0f,		0.0f,
					look.x,		look.y,		look.z,		0.0f,
					prim.v.x,	prim.v.y,	prim.v.z,	1.0f
				};
				glMultMatrixf( bb );
				glTranslatef(
					- width * 0.5f,
					(blockHeight*0.5f) - (float)l * height,
					0.0f );
				glScalef( textScale, textScale, textScale );
				glCallLists( lines[ l ].length(), GL_UNSIGNED_BYTE, lines[ l ].c_str() );
				glPopMatrix();
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

		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glDisableClientState(GL_INDEX_ARRAY);
		glDisableClientState(GL_EDGE_FLAG_ARRAY);
		glDisableClientState(GL_FOG_COORD_ARRAY);
		glDisableClientState(GL_SECONDARY_COLOR_ARRAY);

		const size_t offsetP = offsetof( VBOVert, p );
		const size_t offsetC = offsetof( VBOVert, c );

		for ( size_t i = 0; i < mVBOList.size(); ++i )
		{
			VBOMap::const_iterator it = vbos.find( mVBOList[ i ] );
			if ( it != vbos.end() )
			{
				glEnableClientState( GL_VERTEX_ARRAY );
				glEnableClientState( GL_COLOR_ARRAY );

				glBindBufferARB( GL_ARRAY_BUFFER_ARB, it->second.mBufferId );

				CheckGLError( "PreRender" );

				/*const GLvoid * vertPtr = &it->second.mVerts[ 0 ].p;
				const GLvoid * colrPtr = &it->second.mVerts[ 0 ].c;
				glVertexPointer( 3, GL_FLOAT, sizeof(VBOVert), vertPtr );
				glColorPointer( 4, GL_FLOAT, sizeof(VBOVert), colrPtr );*/

				glVertexPointer( 3, GL_FLOAT, sizeof(VBOVert), (GLvoid*)offsetP );
				glColorPointer( 4, GL_FLOAT, sizeof(VBOVert), (GLvoid*)offsetC );
				
				CheckGLError( "PreRender" );

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
				glDrawArrays( it->second.mType, 0, it->second.mNumPrimitives * it->second.mNumVertsPerPrimitive );

				// unbind the buffer
				glBindBufferARB( GL_ARRAY_BUFFER_ARB, 0 );

				CheckGLError( "PostRender" );

				glDisableClientState( GL_VERTEX_ARRAY );
				glDisableClientState( GL_COLOR_ARRAY );
			}
		}

		glPopClientAttrib();
	}

	glPopAttrib();
}