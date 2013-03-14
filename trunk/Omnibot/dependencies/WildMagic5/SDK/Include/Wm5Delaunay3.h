// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5DELAUNAY3_H
#define WM5DELAUNAY3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Delaunay1.h"
#include "Wm5Delaunay2.h"
#include "Wm5ETManifoldMesh.h"
#include "Wm5Query3.h"

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM Delaunay3 : public Delaunay<Real>
{
public:
    // The input to the constructor is the array of vertices whose Delaunay
    // tetrahedralization is required.  If you want Delaunay3 to delete the
    // vertices during destruction, set bOwner to 'true'.  Otherwise, you
    // own the vertices and must delete them yourself.  Before using this
    // class, you should "clean" your input points by removing duplicates.
    //
    // You have a choice of speed versus accuracy.  The fastest choice is
    // Query::QT_INT64, but it gives up a lot of precision, scaling the points
    // to [0,2^{10}]^3.  The choice Query::QT_INTEGER gives up less precision,
    // scaling the points to [0,2^{20}]^3.  The choice Query::QT_RATIONAL uses
    // exact arithmetic, but is the slowest choice.  The choice Query::QT_REAL
    // uses floating-point arithmetic, but is not robust in all cases.

    Delaunay3 (int numVertices, Vector3<Real>* vertices, Real epsilon,
        bool owner, Query::Type queryType);
    virtual ~Delaunay3 ();

    // The input vertex array.
    const Vector3<Real>* GetVertices () const;

    // The number of unique vertices processed.
    int GetNumUniqueVertices () const;

    // If GetDimension() returns 1, then the points lie on a line.  You must
    // create a Delaunay1 object using the function provided.
    const Vector3<Real>& GetLineOrigin () const;
    const Vector3<Real>& GetLineDirection () const;
    Delaunay1<Real>* GetDelaunay1 () const;

    // If GetDimension() returns 2, then the points lie on a plane.  The plane
    // has two direction vectors (inputs 0 or 1).  You must create a Delaunay2
    // object using the function provided.
    const Vector3<Real>& GetPlaneOrigin () const;
    const Vector3<Real>& GetPlaneDirection (int i) const;
    Delaunay2<Real>* GetDelaunay2 () const;

    // Locate those tetrahedra faces that do not share other tetrahedra.
    // The returned quantity is the number of triangles in the hull.  The
    // returned array has 3*quantity indices, each triple representing a
    // triangle.  The triangles are counterclockwise ordered when viewed
    // from outside the hull.  The return value is 'true' iff the dimension
    // is 3.
    bool GetHull (int& numTriangles, int*& indices) const;

    // Support for searching the tetrahedralization for a tetrahedron that
    // contains a point.  If there is a containing tetrahedron, the returned
    // value is a tetrahedron index i with 0 <= i < numTriangles.  If there is
    // not a containing tetrahedron, -1 is returned.
    int GetContainingTetrahedron (const Vector3<Real>& p) const;

    // If GetContainingTetrahedron returns a nonnegative value, the path of
    // tetrahedra searched for the containing tetrahedra is stored in an
    // array.  The last index of the array is returned by GetPathLast; it is
    // one less than the number of array elements.  The array itself is
    // returned by GetPath.
    int GetPathLast () const;
    const int* GetPath () const;

    // If GetContainingTetrahedron returns -1, the path of tetrahedra
    // searched may be obtained by GetPathLast and GetPath.  The input point
    // is outside a face of the last tetrahedron in the path.  This function
    // returns the vertex indices <v0,v1,v2> of the face, listed in
    // counterclockwise order relative to the convex hull of the data points
    // as viewed by an outside observer.  The final output is the index of the
    // vertex v3 opposite the face.  The return value of the function is the
    // index of the quadruple of vertex indices; the value is 0, 1, 2, or 3.
    int GetLastFace (int& v0, int& v1, int& v2, int& v3) const;

    // Get the vertices for tetrahedron i.  The function returns 'true' if i
    // is a valid tetrahedron index, in which case the vertices are valid.
    // Otherwise, the function returns 'false' and the vertices are invalid.
    bool GetVertexSet (int i, Vector3<Real> vertices[4]) const;

    // Get the vertex indices for tetrahedron i.  The function returns 'true'
    // if i is a valid tetrahedron index, in which case the vertices are
    // valid.  Otherwise, the function returns 'false' and the vertices are
    // invalid.
    bool GetIndexSet (int i, int indices[4]) const;

    // Get the indices for tetrahedra adjacent to tetrahedron i.  The function
    // returns 'true' if i is a valid tetrahedron index, in which case the
    // adjacencies are valid.  Otherwise, the function returns 'false' and
    // the adjacencies are invalid.
    bool GetAdjacentSet (int i, int adjacencies[4]) const;

    // Compute the barycentric coordinates of P with respect to tetrahedron i.
    // The function returns 'true' if i is a valid tetrahedron index, in which
    // case the coordinates are valid.  Otherwise, the function returns
    // 'false' and the coordinate array is invalid.
    bool GetBarycentricSet (int i, const Vector3<Real>& p, Real bary[4])
        const;

    // Support for streaming to/from disk.
    Delaunay3 (const char* filename, int mode = FileIO::FM_DEFAULT_READ);
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

    class Tetrahedron
    {
    public:
        Tetrahedron (int v0, int v1, int v2, int v3);

        bool IsInsertionComponent (int i, Tetrahedron* adj,
            const Query3<Real>* query, const int* supervertices);

        int DetachFrom (int adjIndex, Tetrahedron* adj);

        int V[4];
        Tetrahedron* Adj[4];
        int Time;
        bool IsComponent;
        bool OnStack;
    };

    class Face : public ETManifoldMesh::Triangle
    {
    public:
        Face (int v0 = -1, int v1 = -1, int v2 = -1, int nullIndex = -1,
            Tetrahedron* tri = 0);

        static ETManifoldMesh::TPtr TCreator (int v0, int v1, int v2);

        int NullIndex;
        Tetrahedron* Tetra;
    };

    void Update (int i);
    Tetrahedron* GetContainingTetrahedron (int i) const;
    void RemoveTetrahedra ();
    bool IsSupervertex (int i) const;
    bool SharesFace (int i, Tetrahedron* face, Tetrahedron* adj);

    // The input vertices.
    Vector3<Real>* mVertices;

    // The number of unique vertices processed.
    int mNumUniqueVertices;

    // The scaled input vertices with additional storage for the four
    // supertetrahedron vertices.  This array and supporting data structures
    // are for robust calculations.
    Vector3<Real>* mSVertices;
    Query3<Real>* mQuery;
    Vector3<Real> mMin;
    Real mScale;

    // The indices for the three supertetrahedron vertices.
    int mSupervertices[4];

    // The current tetrahedralization.
    std::set<Tetrahedron*> mTetrahedra;

    // The line of containment if the dimension is 1.
    Vector3<Real> mLineOrigin, mLineDirection;

    // The plane of containment if the dimension is 2.
    Vector3<Real> mPlaneOrigin, mPlaneDirection[2];

    // Store the path of tetrahedra visited in a GetContainingTetrahedron
    // function call.
    mutable int mPathLast;
    mutable int* mPath;

    // If a query point is not in the convex hull of the input points, the
    // point is outside a face of the last tetrahedron in the search path.
    // These are the vertex indices for that face.
    mutable int mLastFaceV0, mLastFaceV1, mLastFaceV2;
    mutable int mLastFaceOpposite, mLastFaceOppositeIndex;
};

typedef Delaunay3<float> Delaunay3f;
typedef Delaunay3<double> Delaunay3d;

}

#endif
