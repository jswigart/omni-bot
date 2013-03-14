// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.0 (2010/01/01)

#ifndef WM5CONVEXPOLYGON2_H
#define WM5CONVEXPOLYGON2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Polygon2.h"
#include <set>

namespace Wm5
{

template <typename Real>
class ConvexPolygon2 : public Polygon2<Real>
{
public:
    // Construction and destruction.  The caller is responsible for ensuring
    // that the array of vertices represents a convex polygon with
    // counterclockwise order.
    //
    // Polygon2 objects typically share data from other sources.  The class
    // uses smart pointers (Pointer1) to share the input arrays.  If you
    // do not want the class to delete the arrays, use your own smart
    // pointer for the arrays.
    //
    // ConvexPolygon2 stores the lines associated with the edges; the line
    // normals are inner pointing.  The lines may be supplied to the
    // constructor, in which case a smart pointer is used to share them.  If
    // they are not passed ('lines' is null), the class automatically
    // generates them.

    // The line representation Dot(N,X) = c.  The 'first' of the pair is a
    // unit-length normal N to the line.  The 'second' of the pair is the line
    // constant c.  X is any point on the line.
    typedef typename std::pair<Vector2<Real>, Real> NCLine;

    ConvexPolygon2 (int numVertices, Vector2<Real>* vertices, NCLine* lines);
    ConvexPolygon2 (const ConvexPolygon2& polygon);
    virtual ~ConvexPolygon2 ();

    // Assignment.
    ConvexPolygon2& operator= (const ConvexPolygon2& polygon);

    // Read-only member access.
    const NCLine* GetLines () const;
    const NCLine& GetLine (int i) const;

    // Allow vertex modification.  The caller must ensure that the polygon
    // remains convex.  After modifying as many vertices as you like, call
    // UpdateLines().  The modifications are all done via SetVertex, and the
    // lines are updated only for those edges sharing the modified vertices.
    virtual void SetVertex (int i, const Vector2<Real>& vertex);
    void UpdateLines ();

    // Test for convexity.  This function will iterate over the edges of the
    // polygon and verify for each edge that the polygon vertices are all on
    // the nonnegative side of the edge line.  A signed distance test is used,
    // so a vertex is on the wrong side of a line (for convexity) when its
    // signed distance satisfies d < 0.  Numerical round-off errors can
    // generate incorrect convexity tests, so a small negative threshold
    // t may be passed to this function, in which case the distance test
    // becomes d < t < 0.
    bool IsConvex (Real threshold = (Real)0) const;

    // Point-in-polygon test, performed by which-side queries between the
    // point and the lines of the edges, an O(n) algorithm for n vertices.
    // This is not the optimal algorithm.  TODO:  Move the binary-search-based
    // algorithm to this class.  It is an O(log n) algorithm.
    bool Contains (const Vector2<Real>& p, Real threshold = (Real)0)
        const;

protected:
    using Polygon2<Real>::mNumVertices;
    using Polygon2<Real>::mVertices;
    using Polygon2<Real>::ComputeVertexAverage;

    // Support for efficient updating of edge lines.  The set stores the
    // indices for those edges that share modified vertices.
    void UpdateLine (int i, const Vector2<Real>& average);

    // The number of lines is the number of vertices.
    Pointer1<NCLine> mLines;

    // The sharing edges processed by UpdateLine.
    std::set<int> mSharingEdges;
};

#include "Wm5ConvexPolygon2.inl"

typedef ConvexPolygon2<float> ConvexPolygon2f;
typedef ConvexPolygon2<double> ConvexPolygon2d;

}

#endif
