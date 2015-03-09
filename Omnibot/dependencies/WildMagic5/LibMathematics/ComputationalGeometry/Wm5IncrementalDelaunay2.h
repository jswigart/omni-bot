// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5INCREMENTALDELAUNAY2_H
#define WM5INCREMENTALDELAUNAY2_H

#include "Wm5MathematicsLIB.h"
#include "Wm5RVector2.h"
#include "Wm5MinHeap.h"
#include "Wm5Vector2.h"
#include "Wm5VEManifoldMesh.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM IncrementalDelaunay2
{
public:
    // Construction and destruction.  The bounding rectangle for the data
    // points must be specified.  Each (x,y) must satisfy xmin <= x <= xmax
    // and ymin <= y <= ymax.  If 'uncertainty' is set to 0, then the
    // geometric computations involve only floating-point arithmetic.  If
    // 'uncertainty' is in (0,1), then filtered predicates are used to
    // compute the signs of quantities of interest.  If 'uncertainty' is
    // set to 1, then exact rational arithmetic is used.
    IncrementalDelaunay2 (Real xmin, Real ymin, Real xmax, Real ymax,
        Real uncertainty = (Real)0);

    ~IncrementalDelaunay2 ();

    // Insert a point into the triangulation.  The return value is the index
    // associated with the vertex in the vertex map.  The supertriangle
    // vertices are at indices 0, 1, and 2.  If the input point already
    // exists, its vertex-map index is simply returned.  If the position
    // is outside the domain specified in the constructor, the return value
    // is -1.
    int Insert (const Vector2<Real>& position);

    // Remove a point from the triangulation.  The return value is the index
    // associated with the vertex in the vertex map when that vertex exists.
    // If the vertex does not exist, the return value is -1.
    int Remove (const Vector2<Real>& position);

    // Support for debugging.  Return an index array of all the triangles,
    // including those that connect to vertices of the supertriangle.  The
    // caller is responsible for deleting the raiIndices output.
    void GetAllTriangles (int& numTriangles, int*& indices);

    //========================================================================
    // Generate a compactified representation of the triangulation.  After
    // this call, the functions following this one are valid to call.  Also,
    // if you call Insert and/or Remove after calling GenerateRepresentation,
    // the functions following this one are invalid to call, and if you are
    // hanging onto pointers and/or references produced by these functions,
    // they are now invalid.  You may, however, call GenerateRepresentation
    // again at which time the functions following this one are once again
    // valid to call.
    void GenerateRepresentation ();

    // N = GetNumTriangles() is the number of triangles in the mesh.  The
    // array returned by I = GetIndices() contains N tuples, each tuple
    // having 3 elements and representing a triangle.  An index I[*] is
    // relative to the vertex array V.  The array returned by
    // A = GetAdjacencies() contains N tuples, each tuple having 3 elements
    // and representing those triangles adjacent to the 3 edges of a triangle.
    // An index A[*] is relative to the index array I.
    int GetNumTriangles () const;
    const int* GetIndices () const;
    const int* GetAdjacencies () const;

    // The input vertex array.  The array includes all unique points passed
    // to Insert, even if Remove was called later for any inserted points.
    // The points at indices 0, 1, and 2 are always the vertices of the
    // supertriangle.
    const std::vector<Vector2<Real> >& GetVertices () const;

    // The unique vertices processed.  These are the actual vertices in the
    // triangulation.  The 'int' value is the index associated with the
    // vertex.
    const std::map<Vector2<Real>,int>& GetUniqueVertices () const;

    // Locate those triangle edges that do not share other triangles.  The
    // returned quantity is the number of edges in the hull.  The returned
    // array has 2*quantity indices, each pair representing an edge.  The
    // edges are not ordered, but the pair of vertices for an edge is ordered
    // so that they conform to a counterclockwise traversal of the hull.  The
    // return value is 'true' iff the dimension is 2.
    bool GetHull (int& numEdges, int*& indices);

    // Support for searching the triangulation for a triangle that contains
    // a point.  If there is a containing triangle, the returned value is a
    // triangle index i with 0 <= i < GetNumTriangles().  If there is not a
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
    //========================================================================

private:
    // Convenient type definitions.
    typedef std::map<Vector2<Real>,int> VertexMap;
    typedef Rational<4*sizeof(Real)> QRational;
    typedef RVector2<4*sizeof(Real)> QRVector;

    class Triangle
    {
    public:
        Triangle (int v0, int v1, int v2);

        bool IsInsertionComponent (int posIndex, const Vector2<Real>& test,
            Triangle* adj, const IncrementalDelaunay2* delaunay);

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

    // Support for the removal polygon and its triangulation.
    class RPVertex
    {
    public:
        RPVertex (int index = -1, Triangle* tri = 0, Triangle* adj = 0);

        // The index into the vertex pool of the position.
        int Index;

        // The triangle sharing edge <Index,NextIndex> and inside the
        // removal polygon.
        Triangle* Tri;

        // The triangle sharing edge <PrevIndex,Index> and inside the
        // removal polygon.
        Triangle* Adj;

        // A vertex is either convex or reflex.  Its condition is stored by
        // the following member.
        bool IsConvex;

        // A convex vertex is either an ear tip or it is not.  Its condition
        // is stored by the following member.
        bool IsEarTip;

        // The removal polygon will contain supervertices when the removal
        // point is on the boundary of the convex hull of the triangulation.
        bool IsSuperVertex;

        // Let V0 be the position of 'this' vertex.  If V0 is a supervertex or
        // is not an ear, its weight is +INFINITY.  Otherwise, let Vp be its
        // predecessor and let Vn be its successor when traversing the polygon
        // counterclockwise.  Let P be the removal point.  The weight is the
        // ratio
        //   Weight = H(Vp,V0,Vn,P)/D(Vp,V0,Vn)
        // where
        //           +              -+
        //   D = det | Vp.X()  Vp.Y()  1 |
        //           | V0.X()  V0.Y()  1 |
        //           | Vn.X()  Vn.Y()  1 |
        //           +-             -+
        // and
        //           +-                            -+
        //   H = det | Vp.X()  Vp.Y()  Vp.X()^2+Vp.Y()^2  1 |
        //           | V0.X()  V0.Y()  V0.X()^2+V0.Y()^2  1 |
        //           | Vn.X()  Vn.Y()  Vn.X()^2+Vn.Y()^2  1 |
        //           | P.X()   P.Y()   P.X()^2+P.Y()^2    1 |
        //           +-                            -+
        Real Weight;

        // Vertex links for polygon.
        int VPrev, VNext;

        // Convex/reflex vertex links (disjoint lists).
        int SPrev, SNext;

        // Ear tip record.
        const MinHeapRecord<int,Real>* EarRecord;
    };

    class Triangulate
    {
    public:
        Triangulate (std::vector<RPVertex>& polygon, int removal,
            IncrementalDelaunay2* delaunay);

    private:
        // Prevent MSVC warning C4512 (assignment operator could not be
        // generated).  No assignment is needed by this class.
        Triangulate& operator= (const Triangulate&) { return *this; }

        RPVertex& V (int i);
        bool IsConvex (int i);
        bool IsEarTip (int i);
        void InsertAfterC (int i);   // insert convex vertex
        void InsertAfterR (int i);   // insert reflex vertesx
        void RemoveV (int i);        // remove vertex
        void RemoveR (int i);        // remove reflex vertex
        Real ComputeWeight (int v0, int p);

        std::vector<RPVertex>& mPolygon;
        int mNumVertices;
        IncrementalDelaunay2* mDelaunay;
        int mCFirst, mCLast;  // linear list of convex vertices
        int mRFirst, mRLast;  // linear list of reflex vertices
        MinHeap<int,Real> mEHeap;  // priority queue of ear tips
    };

    // The directed line is <V0,V1>.  The return value is +1 when 'test' is
    // to the right of the line, -1 when 'test' is to the left of the line,
    // or 0 when 'test' is exactly on the line.
    int ToLine (const Vector2<Real>& test, int v0, int v1) const;

    // The triangle is <V0,V1,V2>.  The return value is +1 when 'test' is
    // outside the triangle, -1 when 'test' is inside the triangle, or 0 when
    // 'test' is exactly on the triangle.
    int ToTriangle (const Vector2<Real>& test, int v0, int v1, int v2) const;

    // The triangle of the circumcircle is <V0,V1,V2>.  The return value is
    // +1 when 'test' is outside the circle, -1 when 'test' is inside the
    // circle, or 0 when 'test' is exactly on the circle.
    int ToCircumcircle (const Vector2<Real>& test, int v0, int v1,
        int v2) const;

    // Use a linear walk to find the triangle containing the point.
    Triangle* GetContainingTriangleInternal (const Vector2<Real>& position)
        const;

    // Return 'true' iff the specified triangle contains a supervertex.  This
    // function is used by GenerateRepresentation.
    bool ContainsSupervertex (Triangle* tri) const;

    // Swap the shared edge with the other diagonal of the quadrilateral
    // union of the two triangles.
    void SwapEdge (Triangle* tri0, Triangle* tri1);

private:
    // The rectangular domain in which all input points live.
    Real mXMin, mXMax, mYMin, mYMax;

    // The vertices of the triangulation.  The vertex pool stores the unique
    // positions that were passed to the Insert function.  This allows for a
    // fast look-up of vertices by the GetContainingTriangle function.
    VertexMap mVMap;
    std::vector<Vector2<Real> > mVertexPool;

    // This member is used to decide whether or not to accept the results of
    // ToLine when computed using floating-point arithmetic.  The test
    // involves a determinant sign.  When the determinant is sufficiently
    // small, the result is uncertain and the determinant is recomputed using
    // exact rational arithmetic.
    Real mUncertainty;

    // When the uncertainty is positive, filtered predicate queries are used
    // and storage is needed for rational vectors to minimize the computation
    // of such vectors.
    mutable std::vector<QRVector>* mRatVertexPool;
    mutable std::vector<bool>* mRatVertexEvaluated;

    // The current triangulation.
    std::set<Triangle*> mTriangle;

    // Compacted informatoin about the triangulation.
    //   N = number of triangles
    //   I = Array of 3-tuples of indices into V that represent the
    //       triangles (3*N total elements).
    //   A = Array of 3-tuples of indices into I that represent the
    //       adjacent triangles (3*N total elements).
    // The i-th triangle has vertices
    //   vertex[0] = V[I[3*i+0]]
    //   vertex[1] = V[I[3*i+1]]
    //   vertex[2] = V[I[3*i+2]]
    // and edge index pairs
    //   edge[0] = <I[3*i+0],I[3*i+1]>
    //   edge[1] = <I[3*i+1],I[3*i+2]>
    //   edge[2] = <I[3*i+2],I[3*i+0]>
    // The triangles adjacent to these edges have indices
    //   adjacent[0] = A[3*i+0] is the triangle sharing edge[0]
    //   adjacent[1] = A[3*i+1] is the triangle sharing edge[1]
    //   adjacent[2] = A[3*i+2] is the triangle sharing edge[2]
    // If there is no adjacent triangle, the A[*] value is set to -1.  The
    // triangle adjacent to edge[j] has vertices
    //   adjvertex[0] = V[I[3*adjacent[j]+0]]
    //   adjvertex[1] = V[I[3*adjacent[j]+1]]
    //   adjvertex[2] = V[I[3*adjacent[j]+2]]
    int mNumTriangles;
    int* mIndices;
    int* mAdjacencies;

    // Store the path of triangles visited in a GetContainingTriangle
    // function call.
    mutable int mPathLast;
    mutable int* mPath;

    // If a query point is not in the convex hull of the input points, the
    // point is outside an edge of the last triangle in the search path.
    // These are the vertex indices for that edge.
    mutable int mLastEdgeV0, mLastEdgeV1;
    mutable int mLastEdgeOpposite, mLastEdgeOppositeIndex;
};

typedef IncrementalDelaunay2<float> IncrementalDelaunay2f;
typedef IncrementalDelaunay2<double> IncrementalDelaunay2d;

}

#endif
