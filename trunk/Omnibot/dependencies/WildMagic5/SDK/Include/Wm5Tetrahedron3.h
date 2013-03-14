// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5TETRAHEDRON3_H
#define WM5TETRAHEDRON3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Plane3.h"

namespace Wm5
{

template <typename Real>
class Tetrahedron3
{
public:
    // The tetrahedron is represented as an array of four vertices:
    // V0, V1, V2, and V3.  The vertices are ordered so that the triangular
    // faces are counterclockwise-ordered triangles when viewed by an observer
    // outside the tetrahedron:
    //   face 0 = <V[0],V[2],V[1]>
    //   face 1 = <V[0],V[1],V[3]>
    //   face 2 = <V[0],V[3],V[2]>
    //   face 3 = <V[1],V[2],V[3]>

    // Construction and destruction.
    Tetrahedron3 ();  // uninitialized
    ~Tetrahedron3 ();

    Tetrahedron3 (const Vector3<Real>& v0, const Vector3<Real>& v1,
        const Vector3<Real>& v2, const Vector3<Real>& v3);

    Tetrahedron3 (const Vector3<Real> vertex[4]);

    // Get the vertex indices for the specified face.
    void GetFaceIndices (int face, int index[3]) const;

    // Construct the planes of the faces.  The planes have outer pointing
    // normal vectors.  The plane indexing is the same as the face indexing
    // mentioned previously.
    void GetPlanes (Plane3<Real> plane[4]) const;

    Vector3<Real> V[4];
};

#include "Wm5Tetrahedron3.inl"

typedef Tetrahedron3<float> Tetrahedron3f;
typedef Tetrahedron3<double> Tetrahedron3d;

}

#endif
