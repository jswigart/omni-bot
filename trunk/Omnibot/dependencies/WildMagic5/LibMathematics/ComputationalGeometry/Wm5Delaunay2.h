// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5DELAUNAY2_H
#define WM5DELAUNAY2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Delaunay1.h"
#include "Wm5Query2.h"
#include "Wm5VEManifoldMesh.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM Delaunay2 : public Delaunay<Real>
{
public:
    // The input to the constructor is the array of vertices whose Delaunay
    // triangulation is required.  If you want Delaunay2 to delete the
    // vertices during destruction, set bOwner to 'true'.  Otherwise, you
    // own the vertices and must delete them yourself.
    //
    // You have a choice of speed versus accuracy.  The fastest choice is
    // Query::QT_INT64, but it gives up a lot of precision, scaling the points
    // to [0,2^{16}]^3.  The choice Query::QT_INTEGER gives up less precision,
    // scaling the points to [0,2^{20}]^3.  The choice Query::QT_RATIONAL uses
    // exact arithmetic, but is the slowest choice.  The choice Query::QT_REAL
    // uses floating-point arithmetic, but is not robust in all cases.
    Delaunay2 (int numVertices, Vector2<Real>* vertices, Real epsilon,
        bool owner, Query::Type queryType);
    virtual ~Delaunay2 ();

    // The input vertex array.
    const Vector2<Real>* GetVertices () const;

    // The number of unique vertices processed.
    int GetNumUniqueVertices () const;

    // If GetDimension() returns 1, then the points lie on a line.  You must
    // create a Delaunay1 object using the function provided.
    const Vector2<Real>& GetLineOrigin () const;
    const Vector2<Real>& GetLineDirection () const;
    Delaunay1<Real>* GetDelaunay1 () const;

    // Locate those triangle edges that do not share other triangles.  The
    // returned quantity is the number of edges in the hull.  The returned
    // array has 2*numEdges indices, each pair representing an edge.  The
    // edges are not ordered, but the pair of vertices for an edge is ordered
    // so that they conform to a counterclockwise traversal of the hull.  The
    // return value is 'true' iff the dimension is 2.
    bool GetHull (int& numEdges, int*& indices);

    // Support for searching the triangulation for a triangle that contains
    // a point.  If there is a containing triangle, the returned value is a
    // triangle index i with 0 <= i < riTQuantity.  If there is not a
    // containing triangle, -1 is returned.
    int GetContainingTriangle (const Vector2<Real>& p) const;

    // If GetContainingTriangle returns a nonnegative value, the path of
    // triangles searched for the containing triangles is stored in an array.
    // The last index of the array is returned by GetPathLast; it is one
    // less than the number of array elements.  The array itself is returned
    // by GetPath.
    int GetPathLast () const;
    const int* GetPath () const;

    // If GetContainingTriangle returns -1, the path of triangles searched
    // may be obtained by GetPathLast and GetPath.  The input point is outside
    // an edge of the last triangle in the path.  This function returns the
    // vertex indices <v0,v1> of the edge, listed in counterclockwise order
    // relative to the convex hull of the data points.  The final output is
    // the index of the vertex v2 opposite the edge.  The return value of
    // the function is the index of the triple of vertex indices; the value
    // is 0, 1, or 2.
    int GetLastEdge (int& v0, int& v1, int& v2) const;

    // Get the vertices for triangle i.  The function returns 'true' if i is
    // a valid triangle index, in which case the vertices are valid.
    // Otherwise, the function returns 'false' and the vertices are invalid.
    bool GetVertexSet (int i, Vector2<Real> vertices[3]) const;

    // Get the vertex indices for triangle i.  The function returns 'true' if
    // i is a valid triangle index, in which case the vertices are valid.
    // Otherwise, the function returns 'false' and the vertices are invalid.
    bool GetIndexSet (int i, int indices[3]) const;

    // Get the indices for triangles adjacent to triangle i.  The function
    // returns 'true' if i is a valid triangle index, in which case the
    // adjacencies are valid.  Otherwise, the function returns 'false' and
    // the adjacencies are invalid.
    bool GetAdjacentSet (int i, int adjacencies[3]) const;

    // Compute the barycentric coordinates of P with respect to triangle i.
    // The function returns 'true' if i is a valid triangle index, in which
    // case the coordinates are valid.  Otherwise, the function returns
    // 'false' and the coordinate array is invalid.
    bool GetBarycentricSet (int i, const Vector2<Real>& p, Real bary[3])
        const;

    // Support for streaming to/from disk.
    Delaunay2 (const char* filename, int mode = FileIO::FM_DEFAULT_READ);
    bool Load (const char* filename, int mode = FileIO::FM_DEFAULT_READ);
    bool Save (const char* filename, int mode = FileIO::FM_DEFAULT_WRITE)
        const;

private:
    using Delaunay<Real>::mQueryType;
    using Delaunay<Real>::mNumVertices;
    using Delaunay<Real>::mDimension;
    using Delaunay<Real>::mNumSimplices;
    using Delaunay<Real>::mIndices;
    using Delaunay<Real>::mAdjacencies;
    using Delaunay<Real>::mEpsilon;
    using Delaunay<Real>::mOwner;

    class Triangle
    {
    public:
        Triangle (int v0, int v1, int v2);

        bool IsInsertionComponent (int i, Triangle* adj,
            const Query2<Real>* query, const int* supervertices);

        int DetachFrom (int adjIndex, Triangle* adj);

        int V[3];
        Triangle* Adj[3];
        int Time;
        bool IsComponent;
        bool OnStack;
    };

    class Edge : public VEManifoldMesh::Edge
    {
    public:
        Edge (int v0 = -1, int v1 = -1, int nullIndex = -1,
            Triangle* tri = 0);

        static VEManifoldMesh::EPtr ECreator (int v0, int v1);

        int NullIndex;
        Triangle* Tri;
    };

    void Update (int i);
    Triangle* GetContainingTriangle (int i) const;
    void RemoveTriangles ();
    bool IsSupervertex (int i) const;

    // The input vertices.
    Vector2<Real>* mVertices;

    // The number of unique vertices processed.
    int mNumUniqueVertices;

    // The scaled input vertices with additional storage for the three
    // supertriangle vertices.  This array and supporting data structures
    // are for robust calculations.
    Vector2<Real>* mSVertices;
    Query2<Real>* mQuery;
    Vector2<Real> mMin;
    Real mScale;

    // The indices for the three supertriangle vertices.
    int mSupervertices[3];

    // The current triangulation.
    std::set<Triangle*> mTriangles;

    // The line of containment if the dimension is 1.
    Vector2<Real> mLineOrigin, mLineDirection;

    // Store the path of tetrahedra visited in a GetContainingTetrahedron
    // function call.
    mutable int mPathLast;
    mutable int* mPath;

    // If a query point is not in the convex hull of the input points, the
    // point is outside an edge of the last triangle in the search path.
    // These are the vertex indices for that edge.
    mutable int mLastEdgeV0, mLastEdgeV1;
    mutable int mLastEdgeOpposite, mLastEdgeOppositeIndex;
};

typedef Delaunay2<float> Delaunay2f;
typedef Delaunay2<double> Delaunay2d;

}

#endif
