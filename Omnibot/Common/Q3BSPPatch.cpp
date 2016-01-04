#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

//############################################################################
//##                                                                        ##
//##  PATCH.CPP                                                             ##
//##                                                                        ##
//##  Converts a Quake3 Bezier patch into a hard coded polygon mesh.        ##
//##                                                                        ##
//##  OpenSourced 12/5/2000 by John W. Ratcliff                             ##
//##                                                                        ##
//##  No warranty expressed or implied.                                     ##
//##                                                                        ##
//##  Part of the Q3BSP project, which converts a Quake 3 BSP file into a   ##
//##  polygon mesh.                                                         ##
//############################################################################
//##                                                                        ##
//##  Contact John W. Ratcliff at jratcliff@verant.com                      ##
//############################################################################

#include "Q3BSPPatch.h"

#define LEVEL_WIDTH(lvl) ((1 << (lvl+1)) + 1)
#define MAXMESHLEVEL 5
#define MINDIST (0.4f*0.4f)

Q3BSPPatchVertex operator+( const Q3BSPPatchVertex & v0, const Q3BSPPatchVertex & v1 )
{
	Q3BSPPatchVertex v;
	v.mPos = v0.mPos + v1.mPos;
	v.mTexel1 = v0.mTexel1 + v1.mTexel1;
	v.mTexel2 = v0.mTexel2 + v1.mTexel2;
	return v;
}

Q3BSPPatchVertex operator-( const Q3BSPPatchVertex & v0, const Q3BSPPatchVertex & v1 )
{
	Q3BSPPatchVertex v;
	v.mPos = v0.mPos - v1.mPos;
	v.mTexel1 = v0.mTexel1 - v1.mTexel1;
	v.mTexel2 = v0.mTexel2 - v1.mTexel2;
	return v;
}

Q3BSPPatchVertex operator*( const Q3BSPPatchVertex & v0, float scalar )
{
	Q3BSPPatchVertex v;
	v.mPos = v0.mPos * scalar;
	v.mTexel1 = v0.mTexel1 * scalar;
	v.mTexel2 = v0.mTexel2 * scalar;
	return v;
}

template <class T>
T lerp( const T& one, const T& two, float val )
{
	return one + ( two - one )*val;
}

Q3BSPPatchSurface::Q3BSPPatchSurface( const Q3BSPPatchVertex *cp,
	int npoints,
	int controlx,
	int controly )
{
	int sizex = 0, sizey = 0;
	FindSize( controlx, controly, cp, sizex, sizey );

	int size = sizex*sizey;
	mPoints = new Q3BSPPatchVertex[ size ];

	int stepx = ( sizex - 1 ) / ( controlx - 1 );
	int stepy = ( sizey - 1 ) / ( controly - 1 );
	const Q3BSPPatchVertex *control = cp;

	for ( int y = 0; y < sizey; y += stepy )
	{
		for ( int x = 0; x < sizex; x += stepx )
		{
			int p = y*sizex + x;
			mPoints[ p ] = *control++;
		}
	}


	FillPatch( controlx, controly, sizex, sizey, mPoints );

	mCount = ( sizex - 1 )*( sizey - 1 ) * 6;

	if ( 1 )
	{
		mIndices = new unsigned int[ mCount ];
		unsigned int *foo = mIndices;
		for ( int y = 0; y < sizey - 1; ++y )
		{
			for ( int x = 0; x < sizex - 1; ++x )
			{
				*foo++ = y*sizex + x;
				*foo++ = ( y + 1 )*sizex + x;
				*foo++ = y*sizex + x + 1;

				*foo++ = y*sizex + x + 1;
				*foo++ = ( y + 1 )*sizex + x;
				*foo++ = ( y + 1 )*sizex + x + 1;
			}
		}
		delete [] mIndices;
	}
}

Q3BSPPatchSurface::~Q3BSPPatchSurface()
{
	delete [] mIndices;
	delete [] mPoints;
}


bool Q3BSPPatchSurface::FindSize( int controlx, int controly, const Q3BSPPatchVertex *cp, int &sizex, int &sizey ) const
{
	/* Find non-coincident pairs in u direction */

	bool found = false;

	const Q3BSPPatchVertex *a = 0;
	const Q3BSPPatchVertex *b = 0;

	for ( int v = 0; v < controly; v++ )
	{
		for ( int u = 0; u < controlx; u += 2 )
		{
			a = &cp[ v * controlx + u ];
			b = &cp[ v * controlx + u + 2 ];

			if ( a->mPos.X() != b->mPos.X() ||
				a->mPos.Y() != b->mPos.Y() ||
				a->mPos.Z() != b->mPos.Z() )
			{
				found = true;
				break;
			}
		}
		if ( found ) break;
	}

	if ( !found )
	{
		printf( "Bad mesh control points\n" );
		return false;
	}

	/* Find subdivision level in u */
	int levelx = FindLevel( a[ 0 ].mPos, a[ 1 ].mPos, b[ 0 ].mPos );
	sizex = ( LEVEL_WIDTH( levelx ) - 1 ) * ( ( controlx - 1 ) / 2 ) + 1;


	for ( int u = 0; u < controlx; u++ )
	{
		for ( int v = 0; v < controly; v += 2 )
		{

			a = &cp[ v * controlx + u ];
			b = &cp[ ( ( v + 2 ) * controlx ) + u ];

			if ( a->mPos.X() != b->mPos.X() ||
				a->mPos.Y() != b->mPos.Y() ||
				a->mPos.Z() != b->mPos.Z() )
			{
				found = true;
				break;
			}
		}
		if ( found ) break;
	}

	if ( !found )
	{
		printf( "Bad mesh control points\n" );
		return false;
	}

	/* Find subdivision level in u */
	int levely = FindLevel( a[ 0 ].mPos, a[ 1 ].mPos, b[ 0 ].mPos );
	sizey = ( LEVEL_WIDTH( levely ) - 1 ) * ( ( controly - 1 ) / 2 ) + 1;

	return true;
}


int Q3BSPPatchSurface::FindLevel( const Vector3f &cv0, const Vector3f &cv1, const Vector3f &cv2 ) const
{
	int level;
	Vector3f a, b, dist;
	Vector3f v0 = cv0; // init control points.
	Vector3f v1 = cv1;
	Vector3f v2 = cv2;

	/* Subdivide on the left until tolerance is reached */
	for ( level = 0; level < MAXMESHLEVEL - 1; level++ )
	{
		/* Subdivide on the left */
		a = lerp( v0, v1, 0.5f );
		b = lerp( v1, v2, 0.5f );
		v2 = lerp( a, b, 0.5f );

		/* Find distance moved */
		dist = v2 - v1;

		float dist2 = dist.X()*dist.X() + dist.Y()*dist.Y() + dist.Z()*dist.Z();
		if ( dist2 < MINDIST )
			break;

		/* Insert new middle vertex */
		v1 = a;
	}

	return level;
}

void Q3BSPPatchSurface::FillPatch( int controlx, int controly, int sizex, int sizey, Q3BSPPatchVertex *p )
{
	int stepx = ( sizex - 1 ) / ( controlx - 1 );
	for ( int u = 0; u < sizex; u += stepx )
	{
		FillCurve( controly, sizey, sizex, p + u );
	}
	for ( int v = 0; v < sizey; v++ )
	{
		FillCurve( controlx, sizex, 1, p + v * sizex );
	}
}

void Q3BSPPatchSurface::FillCurve( int numcp, int size, int stride, Q3BSPPatchVertex *p )
{
	int step, halfstep, i, mid;
	Q3BSPPatchVertex a, b;

	step = ( size - 1 ) / ( numcp - 1 );

	while ( step > 0 )
	{
		halfstep = step / 2;
		for ( i = 0; i < size - 1; i += step * 2 )
		{
			mid = ( i + step )*stride;
			a = lerp( p[ i*stride ], p[ mid ], 0.5f );
			b = lerp( p[ mid ], p[ ( i + step * 2 )*stride ], 0.5f );
			p[ mid ] = lerp( a, b, 0.5f );

			//	    vec_avg(p[i*stride], p[mid], a);
			//	    vec_avg(p[mid], p[(i+step*2)*stride], b);
			//	    vec_avg(a, b, p[mid]);

			if ( halfstep > 0 )
			{
				p[ ( i + halfstep )*stride ] = a;
				p[ ( i + 3 * halfstep )*stride ] = b;
			}
		}
		step /= 2;
	}
}
