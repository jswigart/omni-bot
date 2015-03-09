// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5POLYHEDRON3_H
#define WM5POLYHEDRON3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Vector3.h"
#include "Wm5SmartPointer.h"

namespace Wm5
{

template <typename Real>
class Polyhedron3
{
public:
    // Construction and destruction.  The caller is responsible for ensuring
    // that the mesh represents a simple polyhedron (2-manifold with the
    // topology of a sphere, non-self-intersecting).  The triangular faces
    // must be orderd counterclockwise when viewed from outside the mesh.
    //
    // Polyhedron3 objects typically share data from other sources.  The class
    // uses smart pointers (Pointer1) to share the input arrays.  If you
    // do not want the class to delete the arrays, use your own smart pointers
    // for the arrays.

    Polyhedron3 (int numVertices, Vector3<Real>* vertices, int numTriangles,
        int* indices);

    Polyhedron3 (const Polyhedron3& polyhedron);
    virtual ~Polyhedron3 ();

    // Assignment.
    Polyhedron3& operator= (const Polyhedron3& polyhedron);

    // Read-only member access.
    int GetNumVertices () const;
    const Vector3<Real>* GetVertices () const;
    const Vector3<Real>& GetVertex (int i) const;
    int GetNumTriangles () const;
    int GetNumIndices () const;  // = 3*numTriangles
    const int* GetIndices () const;
    const int* GetTriangle (int i) const;

    // Allow vertex modification.  The caller must ensure that the polyhedron
    // remains simple.
    virtual void SetVertex (int i, const Vector3<Real>& vertex);

    // Compute various information about the polyhedron.
    Vector3<Real> ComputeVertexAverage () const;
    Real ComputeSurfaceArea () const;
    Real ComputeVolume () const;

protected:
    int mNumVertices;
    Pointer1<Vector3<Real> > mVertices;
    int mNumTriangles;
    int mNumIndices;
    Pointer1<int> mIndices;
};

#include "Wm5Polyhedron3.inl"

typedef Polyhedron3<float> Polyhedron3f;
typedef Polyhedron3<double> Polyhedron3d;

}

#endif
