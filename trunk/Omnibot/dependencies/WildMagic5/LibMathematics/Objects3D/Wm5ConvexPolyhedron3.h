// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5CONVEXPOLYHEDRON3_H
#define WM5CONVEXPOLYHEDRON3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Polyhedron3.h"

namespace Wm5
{

template <typename Real>
class ConvexPolyhedron3 : public Polyhedron3<Real>
{
public:
    // Construction and destruction.  The caller is responsible for ensuring
    // that the mesh represents a convex polyhedron.  The triangular faces
    // must be ordered counterclockwise when viewed from outside the mesh.
    //
    // Polyhedron3 objects typically share data from other sources.  The class
    // uses smart pointers (Pointer1) to share the input arrays.  If you
    // do not want the class to delete the arrays, use your own smart pointers
    // for the arrays.
    //
    // ConvexPolyhedron3 stores the planes associated with the faces; the
    // plane normals are inner pointing.  The planes may be supplied to the
    // constructor, in which case a smart pointer is used to share them.  If
    // they are not passed ('planes' is null), the class automatically
    // generates them.

    // The plane representation is Dot(N,X) = c.  The 'first' of the pair is
    // a unit-length normal N to the plane.  The 'second' of the pair is the
    // plane constant c.  X is any point on the plane.
    typedef typename std::pair<Vector3<Real>, Real> NCPlane;

    ConvexPolyhedron3 (int numVertices, Vector3<Real>* vertices,
        int numTriangles, int* indices, NCPlane* planes);

    ConvexPolyhedron3 (const ConvexPolyhedron3& polyhedron);
    virtual ~ConvexPolyhedron3 ();

    // Assignment.
    ConvexPolyhedron3& operator= (const ConvexPolyhedron3& polyhedron);

    // Read-only member access.
    const NCPlane* GetPlanes () const;
    const NCPlane& GetPlane (int i) const;

    // Allow vertex modification.  The caller must ensure that the polyhedron
    // remains convex.  After modifying as many vertices as you like, call
    // UpdatePlanes().  The modifications are all done via SetVertex, and the
    // planes are updated only for those triangles sharing the modified
    // vertices.
    virtual void SetVertex (int i, const Vector3<Real>& vertex);
    void UpdatePlanes ();

    // Test for convexity.  This function will iterate over the faces of the
    // polyhedron and verify for each face that the polyhedron vertices are
    // all on the nonnegative side of the facet plane.  A signed distance test
    // is used, so a vertex is on the wrong side of a plane (for convexity)
    // when its signed distance satisfies d < 0.  Numerical round-off errors
    // can generate incorrect convexity tests, so a small negative threshold
    // t may be passed to this function, in which case the distance test
    // becomes d < t < 0.
    bool IsConvex (Real threshold = (Real)0) const;

    // Point-in-polyhedron test, performed by which-side queries between the
    // point and the planes of the faces, an O(n) algorithm for n vertices.
    // This is not the optimal algorithm.  TODO:  Move the BSP-based algorithm
    // to this class.  It is an O(log n) algorithm.
    bool Contains (const Vector3<Real>& p, Real threshold = (Real)0)
        const;

protected:
    using Polyhedron3<Real>::mNumVertices;
    using Polyhedron3<Real>::mVertices;
    using Polyhedron3<Real>::mNumTriangles;
    using Polyhedron3<Real>::mNumIndices;
    using Polyhedron3<Real>::mIndices;

    // Support for efficient updating of facet planes.  The set stores the
    // indices for those triangles that share modified vertices.
    void UpdatePlane (int i, const Vector3<Real>& average);

    // The number of planes is the number of triangles.
    Pointer1<NCPlane> mPlanes;

    // The sharing triangles processed by UpdatePlane.
    std::set<int> mSharingTriangles;
};

#include "Wm5ConvexPolyhedron3.inl"

typedef ConvexPolyhedron3<float> ConvexPolyhedron3f;
typedef ConvexPolyhedron3<double> ConvexPolyhedron3d;

}

#endif
