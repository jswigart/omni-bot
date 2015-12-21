#ifndef PATCH_H

#define PATCH_H

//############################################################################
//##                                                                        ##
//##  PATCH.H                                                               ##
//##                                                                        ##
//##  Converts a Quake3 Bezier patch into a hard coded polygon mesh.        ##
//##                                                                        ##
//##  OpenSourced 12/5/2000 by John W. Ratcliff                             ##
//##                                                                        ##
//##  No warranty expressed or implied.                                     ##
//##                                                                        ##
//##  Part of the Q3BSP project, which converts a Quake 3 BSP file into a   ##
//##  polygon mesh.                                                         ##
//##				                                                        ##
//##  Modifed by Jeremy Swigart                                             ##
//############################################################################
//##                                                                        ##
//##  Contact John W. Ratcliff at jratcliff@verant.com                      ##
//############################################################################

struct Q3BSPPatchVertex
{
	aiVector3D	mPos;
	aiVector2D	mTexel1, mTexel2;
};

class Q3BSPPatchSurface
{
public:
	Q3BSPPatchSurface( const Q3BSPPatchVertex *control_points, int npoints, int controlx, int controly );
	~Q3BSPPatchSurface();

	const Q3BSPPatchVertex * GetVerts( void ) const
	{
		return mPoints;
	};
	const unsigned short * GetIndices( void ) const
	{
		return mIndices;
	};
	int GetIndiceCount( void ) const
	{
		return mCount;
	};

private:
	bool FindSize( int controlx, int controly, const Q3BSPPatchVertex *cp, int &sizex, int &sizey ) const;
	void FillPatch( int controlx, int controly, int sizex, int sizey, Q3BSPPatchVertex *points );
	void FillCurve( int numcp, int size, int stride, Q3BSPPatchVertex *p );

	int FindLevel( const aiVector3D &cv0, const aiVector3D &cv1, const aiVector3D &cv2 ) const;

	int                  mCount;
	Q3BSPPatchVertex    *mPoints;       // vertices produced.
	unsigned short      *mIndices;      // indices into those vertices
};

#endif
