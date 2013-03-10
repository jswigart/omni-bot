// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5TRIANGULATEEC_H
#define WM5TRIANGULATEEC_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Query2.h"
#include "Wm5Vector2.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM TriangulateEC
{
public:
    // This class implements triangulation of polygons using ear clipping.
    // The method is O(n^2) for n input points.  There are five constructors.
    // The query type and epsilon parameters are discussed later.  In all
    // cases, the output is
    //
    //    triangles:
    //        An array of 3*T indices representing T triangles.  Each triple
    //        (i0,i1,i2) corresponds to the triangle (P[i0],P[i1],P[i2]),
    //        where P is the 'positions' input.  These triangles are all
    //        counterclockwise ordered.
    //
    // TriangulateEC(positions,queryType,epsilon,triangles)
    //    positions:
    //        An array of n vertex positions for a simple polygon.  The
    //        polygon is (P[0],P[1],...,P[n-1]).
    //               
    // TriangulateEC(positions,queryType,epsilon,polygon,triangles)
    //    positions:
    //        An array of vertex positions, not necessarily the exact set of
    //        positions for the polygon vertices.
    //    polygon:
    //        An array of n indices into 'positions'.  If the array is
    //        (I[0],I[1],...,I[n-1]), the polygon vertices are
    //        (P[I[0]],P[I[1]],...,P[I[n-1]]).
    //               
    // TriangulateEC(positions,queryType,epsilon,outerPolygon,innerPolygon,
    //               triangles)
    //    positions:
    //        An array of vertex positions, not necessarily the exact set of
    //        positions for the polygon vertices.
    //    outerPolygon:
    //        An array of n indices into 'positions' for the outer polygon.
    //        If the array is (I[0],I[1],...,I[n-1]), the outer polygon
    //        vertices are (P[I[0]],P[I[1]],...,P[I[n-1]]).
    //    innerPolygon:
    //        An array of m indices into 'positions' for the inner polygon.
    //        The inner polygon must be strictly inside the outer polygon.
    //        If the array is (J[0],J[1],...,J[m-1]), the inner polygon
    //        vertices are (P[J[0]],P[J[1]],...,P[J[m-1]]).
    //               
    // TriangulateEC(positions,queryType,epsilon,outerPolygon,innerPolygons,
    //               triangles)
    //    positions:
    //        An array of vertex positions, not necessarily the exact set of
    //        positions for the polygon vertices.
    //    outerPolygon:
    //        An array of n indices into 'positions' for the outer polygon.
    //        If the array is (I[0],I[1],...,I[n-1]), the outer polygon
    //        vertices are (P[I[0]],P[I[1]],...,P[I[n-1]]).
    //    innerPolygons:
    //        An array of arrays of indices, each index array representing
    //        an inner polygon.  The inner polygons must be nonoverlapping and
    //        strictly contained in the outer polygon.  If innerPolygons[i]
    //        is the array (J[0],J[1],...,J[m-1]), the inner polygon
    //        vertices are (P[J[0]],P[J[1]],...,P[J[m-1]]).
    //               
    // TriangulateEC(positions,queryType,epsilon,tree,triangles)
    //    positions:
    //        An array of vertex positions, not necessarily the exact set of
    //        positions for the polygon vertices.
    //    tree:
    //        A hierarchy of nested polygons.  The root node corresponds to
    //        the outermost outer polygon.  The child nodes of the root
    //        correspond to inner polygons contained by the outer polygon.
    //        Each inner polygon may itself contain outer polygons, and
    //        those outer polygons may themselves contain inner polygons.
    //
    // You have a choice of speed versus accuracy.  The fastest choice is
    // Query::QT_INT64, but it gives up a lot of precision, scaling the points
    // to [0,2^{20}]^3.  The choice Query::QT_INTEGER gives up less precision,
    // scaling the points to [0,2^{24}]^3.  The choice Query::QT_RATIONAL uses
    // exact arithmetic, but is the slowest choice.  The choice Query::QT_REAL
    // uses floating-point arithmetic, but is not robust in all cases.  The
    // choice Query::QT_FILTERED uses floating-point arithmetic to compute
    // determinants whose signs are of interest.  If the floating-point value
    // is nearly zero, the determinant is recomputed using exact rational
    // arithmetic in order to correctly classify the sign.  The hope is to
    // have an exact result computed faster than with all rational arithmetic.
    // The value fEpsilon is used only for the Query::QT_FILTERED case and
    // should be in [0,1].  If 0, the method defaults to all exact rational
    // arithmetic.  If 1, the method defaults to all floating-point
    // arithmetic.  Generally, if M is the maximum absolute value of a
    // determinant and if d is the current determinant value computed as a
    // floating-point quantity, the recalculation with rational arithmetic
    // occurs when |d| < epsilon*M.

    // Convenient typedefs.
    typedef std::vector<Vector2<Real> > Positions;
    typedef std::vector<int> Indices;
    typedef std::vector<Indices*> IndicesArray;
    typedef std::map<int,int> IndexMap;

    // The input 'positions' represents an array of vertices for a simple
    // polygon. The vertices are positions[0] through positions[n-1], where
    // the polygon has n vertices that are listed in counterclockwise order.
    TriangulateEC (const Positions& positions, Query::Type queryType,
        Real epsilon, Indices& triangles);

    // The input 'positions' represents an array of vertices that contains the
    // vertices of a simple polygon.  The input 'polygon' represents a simple
    // polygon whose vertices are positions[polygon[0]] through
    // positions[polygon[m-1]], where the polygon has m vertices that are
    // listed in counterclockwise order.
    TriangulateEC (const Positions& positions, Query::Type queryType,
        Real epsilon, const Indices& polygon, Indices& triangles);

    // The input 'positions' is a shared array of vertices that contains the
    // vertices for two simple polygons, an outer polygon and an inner
    // polygon.  The inner polygon must be strictly inside the outer polygon.
    // The input 'outer' represents the outer polygon whose vertices are
    // positions[outer[0]] through positions[outer[n-1]], where the outer
    // polygon has n vertices that are listed in counterclockwise order.  The
    // input 'inner' represents the inner polygon whose vertices are
    // positions[inner[0]] through positions[inner[m-1]], where the inner
    // polygon has m vertices that are listed in clockwise order.
    TriangulateEC (const Positions& positions, Query::Type queryType,
        Real epsilon, const Indices& outer, const Indices& inner,
        Indices& triangles);

    // The input 'positions' is a shared array of vertices that contains the
    // vertices for multiple simple polygons, an outer polygon and one or more
    // inner polygons.  The inner polygons must be nonoverlapping and strictly
    // inside the outer polygon.  The input 'outer' represents the outer
    // polygon whose vertices are positions[outer[0]] through
    // positions[outer[n-1]], where the outer polygon has n vertices that are
    // listed in counterclockwise order.  The input element 'inners[i]'
    // represents the i-th inner polygon whose vertices are
    // positions[inners[i][0]] through positions[inners[i][m-1]], where this
    // polygon has m vertices that are listed in clockwise order.
    TriangulateEC (const Positions& positions, Query::Type queryType,
        Real epsilon, const Indices& outer, const IndicesArray& inners,
        Indices& triangles);

    // A tree of nested polygons.  The root node corresponds to an outer
    // polygon.  The children of the root correspond to inner polygons, which
    // are nonoverlapping polygons strictly contained in the outer polygon.
    // Each inner polygon may itself contain an outer polygon, thus leading
    // to a hierarchy of polygons.  The outer polygons have vertices listed
    // in counterclockwise order.  The inner polygons have vertices listed in
    // clockwise order.
    class WM5_MATHEMATICS_ITEM Tree
    {
    public:
        Indices Polygon;
        std::vector<Tree*> Child;
    };

    // The input 'positions' is a shared array of vertices that contains the
    // vertices for multiple simple polygons in a tree of polygons.
    TriangulateEC (const Positions& positions, Query::Type queryType,
        Real epsilon, const Tree* tree, Indices& triangles);

    ~TriangulateEC ();

    // Helper function to delete Tree objects.  Call this only if all tree
    // nodes were dynamically allocated.
    static void Delete (Tree*& root);

private:
    // Create the query object and scaled positions to be used during
    // triangulation.  Extra elements are required when triangulating polygons
    // with holes.  These are preallocated to avoid dynamic resizing during
    // the triangulation.
    void InitializePositions (const Positions& positions,
        Query::Type queryType, Real epsilon, int extraElements);

    // Create the vertex objects that store the various lists required by the
    // ear-clipping algorithm.
    void InitializeVertices (int numVertices, const int* indices);

    // Apply ear clipping to the input polygon.  Polygons with holes are
    // preprocessed to obtain an index array that is nearly a simple polygon.
    // This outer polygon has a pair of coincident edges per inner polygon.
    void DoEarClipping (int numVertices, const int* indices,
        Indices& triangles);

    // This function is used to help determine a pair of visible vertices
    // for the purpose of triangulating polygons with holes.  The query is
    // point-in-triangle, but is encapsulated here to use the same type of
    // query object that the user specified in the constructors.
    int TriangleQuery (const Vector2<Real>& position, Query::Type queryType,
        Real epsilon, const Vector2<Real> triangle[3]) const;

    // Given an outer polygon that contains an inner polygon, this function
    // determines a pair of visible vertices and inserts two coincident edges
    // to generate a nearly simple polygon.
    void CombinePolygons (Query::Type queryType, Real epsilon,
        int nextElement, const Indices& outer, const Indices& inner,
        IndexMap& indexMap, Indices& combined);

    // Two extra elements are needed in the position array per outer-inners
    // polygon.  This function computes the total number of extra elements
    // needed for the input tree.  This number is passed to the function
    // InitializePositions.
    static int GetExtraElements (const Tree* tree);

    // Given an outer polygon that contains a set of nonoverlapping inner
    // polygons, this function determines pairs of visible vertices and
    // inserts coincident edges to generate a nearly simple polygon.  It
    // repeatedly calls CombinePolygons for each inner polygon of the outer
    // polygon.
    void ProcessOuterAndInners (Query::Type queryType, Real epsilon,
        const Indices& outer, const IndicesArray& inners,
        int& nextElement, IndexMap& indexMap, Indices& combined);

    // The insertion of coincident edges to obtain a nearly simple polygon
    // requires duplication of vertices in order that the ear-clipping
    // algorithm work correctly.  After the triangulation, the indices of
    // the duplicated vertices are converted to the original indices.
    void RemapIndices (const IndexMap& indexMap, Indices& triangles) const;

    // Doubly linked lists for storing specially tagged vertices.
    class Vertex
    {
    public:
        Vertex ();

        int Index;  // index of vertex in position array
        bool IsConvex, IsEar;
        int VPrev, VNext; // vertex links for polygon
        int SPrev, SNext; // convex/reflex vertex links (disjoint lists)
        int EPrev, ENext; // ear links
    };

    Vertex& V (int i);
    bool IsConvex (int i);
    bool IsEar (int i);
    void InsertAfterC (int i);   // insert convex vertex
    void InsertAfterR (int i);   // insert reflex vertesx
    void InsertEndE (int i);     // insert ear at end of list
    void InsertAfterE (int i);   // insert ear after efirst
    void InsertBeforeE (int i);  // insert ear before efirst
    void RemoveV (int i);        // remove vertex
    int  RemoveE (int i);        // remove ear at i
    void RemoveR (int i);        // remove reflex vertex

    // The doubly linked list.
    std::vector<Vertex> mVertices;
    int mCFirst, mCLast;  // linear list of convex vertices
    int mRFirst, mRLast;  // linear list of reflex vertices
    int mEFirst, mELast;  // cyclical list of ears

    // For robust determinant calculation.
    Query2<Real>* mQuery;
    std::vector<Vector2<Real> >mSPositions;
};

}

#endif
