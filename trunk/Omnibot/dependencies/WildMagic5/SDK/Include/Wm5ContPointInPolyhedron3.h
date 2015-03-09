// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#ifndef WM5CONTPOINTINPOLYHEDRON3_H
#define WM5CONTPOINTINPOLYHEDRON3_H

#include "Wm5MathematicsLIB.h"
#include "Wm5Plane3.h"
#include "Wm5Ray3.h"
#include "Wm5Vector2.h"

// This class contains various implementations for point-in-polyhedron
// queries.  The planes stored with the faces are used in all cases to
// reject ray-face intersection tests, a quick culling operation.
//
// The algorithm is to cast a ray from the input point P and test for
// intersection against each face of the polyhedron.  If the ray only
// intersects faces at interior points (not vertices, not edge points),
// then the point is inside when the number of intersections is odd and
// the point is outside when the number of intersections is even.  If the
// ray intersects an edge or a vertex, then the counting must be handled
// differently.  The details are tedious.  As an alternative, the approach
// here is to allow you to specify 2*N+1 rays, where N >= 0.  You should
// choose these rays randomly.  Each ray reports "inside" or "outside".
// Whichever result occurs N+1 or more times is the "winner".  The input
// rayQuantity is 2*N+1.  The input array Direction must have rayQuantity
// elements.  If you are feeling lucky, choose rayQuantity to be 1.
//
// TO DO.  Add a Contains function that uses exact arithmetic and that
// casts one ray, keeping track of the parity correctly when the ray
// intersects a vertex or an edge.  If the faces are always triangles,
// the use of barycentric coordinates gives us a "normalized" measurement
// of how close to a vertex or an edge the intersection point is.  This,
// in turn, might make it easy to use filtered predicates, which will be
// faster than always using exact arithmetic.  If the faces are not
// triangles, any triangulation introduces edges that are not face
// boundary edges.  These should be ignored by the special-case handling
// of boundary edges.  In the most general case of simple polygon faces
// without triangulation, it is not immediately clear how to compute a
// "normalized" measurement that will allow us to use filtered predicates
// in an easy manner.

namespace Wm5
{

template <typename Real>
class WM5_MATHEMATICS_ITEM PointInPolyhedron3
{
public:
    // For simple polyhedra with triangle faces.
    class WM5_MATHEMATICS_ITEM TriangleFace
    {
    public:
        // When you view the face from outside, the vertices are
        // counterclockwise ordered.  The Indices array stores the indices
        // into the vertex array.
        int Indices[3];

        // The normal vector is unit length and points to the outside of the
        // polyhedron.
        Plane3<Real> Plane;
    };

    // The Contains query will use ray-triangle intersection queries.
    PointInPolyhedron3 (int numPoints, const Vector3<Real>* points,
        int numFaces, const TriangleFace* faces, int numRays,
        const Vector3<Real>* directions);

    // For simple polyhedra with convex polygon faces.
    class WM5_MATHEMATICS_ITEM ConvexFace
    {
    public:
        // When you view the face from outside, the vertices are
        // counterclockwise ordered.  The Indices array stores the indices
        // into the vertex array.
        std::vector<int> Indices;

        // The normal vector is unit length and points to the outside of the
        // polyhedron.
        Plane3<Real> Plane;
    };

    // The Contains query will use ray-convexpolygon intersection queries.  A
    // ray-convexpolygon intersection query can be implemented in many ways.
    // In this context, uiMethod is one of three value:
    //   0 : Use a triangle fan and perform a ray-triangle intersection query
    //       for each triangle.
    //   1 : Find the point of intersection of ray and plane of polygon.  Test
    //       whether that point is inside the convex polygon using an O(N)
    //       test.
    //   2 : Find the point of intersection of ray and plane of polygon.  Test
    //       whether that point is inside the convex polygon using an O(log N)
    //       test.
    PointInPolyhedron3 (int numPoints, const Vector3<Real>* points,
        int numFaces, const ConvexFace* faces, int numRays,
        const Vector3<Real>* directions, unsigned int method);

    // For simple polyhedra with simple polygon faces that are generally
    // not all convex.
    class WM5_MATHEMATICS_ITEM SimpleFace
    {
    public:
        // When you view the face from outside, the vertices are
        // counterclockwise ordered.  The Indices array stores the indices
        // into the vertex array.
        std::vector<int> Indices;

        // The normal vector is unit length and points to the outside of the
        // polyhedron.
        Plane3<Real> Plane;

        // Each simple face may be triangulated.  The indices are relative to
        // the vertex array.  Each triple of indices represents a triangle in
        // the triangulation.
        std::vector<int> Triangles;
    };

    // The Contains query will use ray-simplepolygon intersection queries.  A
    // ray-simplepolygon intersection query can be implemented in a couple of
    // ways.  In this context, uiMethod is one of two value:
    //   0 : Iterate over the triangles of each face and perform a
    //       ray-triangle intersection query for each triangle.  This requires
    //       that the SimpleFace::Triangles array be initialized for each
    //       face.
    //   1 : Find the point of intersection of ray and plane of polygon.  Test
    //       whether that point is inside the polygon using an O(N) test.  The
    //       SimpleFace::Triangles array is not used for this method, so it
    //       does not have to be initialized for each face.
    PointInPolyhedron3 (int numPoints, const Vector3<Real>* points,
        int numFaces, const SimpleFace* faces, int numRays,
        const Vector3<Real>* directions, unsigned intmethod);

    // This function will select the actual algorithm based on which
    // constructor you used for this class.
    bool Contains (const Vector3<Real>& p) const;

private:
    // For all types of faces.  The ray origin is the test point.  The ray
    // direction is one of those passed to the constructors.  The plane origin
    // is a point on the plane of the face.  The plane normal is a unit-length
    // normal to the face and that points outside the polyhedron.
    static bool FastNoIntersect (const Ray3<Real>& ray,
        const Plane3<Real>& plane);

    // For triangle faces.
    bool ContainsT0 (const Vector3<Real>& p) const;

    // For convex faces.
    bool ContainsC0 (const Vector3<Real>& p) const;
    bool ContainsC1C2 (const Vector3<Real>& p, unsigned int method) const;

    // For simple faces.
    bool ContainsS0 (const Vector3<Real>& p) const;
    bool ContainsS1 (const Vector3<Real>& p) const;

    int mNumPoints;
    const Vector3<Real>* mPoints;

    int mNumFaces;
    const TriangleFace* mTFaces;
    const ConvexFace* mCFaces;
    const SimpleFace* mSFaces;

    unsigned int mMethod;
    int mNumRays;
    const Vector3<Real>* mDirections;

    // Temporary storage for those methods that reduce the problem to 2D
    // point-in-polygon queries.  The array stores the projections of
    // face vertices onto the plane of the face.  It is resized as needed.
    mutable std::vector<Vector2<Real> > mProjVertices;
};

typedef PointInPolyhedron3<float> PointInPolyhedron3f;
typedef PointInPolyhedron3<double> PointInPolyhedron3d;

}

#endif
