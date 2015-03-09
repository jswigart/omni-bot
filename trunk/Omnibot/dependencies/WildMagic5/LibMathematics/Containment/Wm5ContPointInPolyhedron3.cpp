// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ContPointInPolyhedron3.h"
#include "Wm5ContPointInPolygon2.h"
#include "Wm5IntrRay3Plane3.h"
#include "Wm5IntrRay3Triangle3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
PointInPolyhedron3<Real>::PointInPolyhedron3 (int numPoints,
    const Vector3<Real>* points, int numFaces, const TriangleFace* faces,
    int numRays, const Vector3<Real>* directions)
    :
    mNumPoints(numPoints),
    mPoints(points),
    mNumFaces(numFaces),
    mTFaces(faces),
    mCFaces(0),
    mSFaces(0),
    mMethod(0),
    mNumRays(numRays),
    mDirections(directions)
{
}
//----------------------------------------------------------------------------
template <typename Real>
PointInPolyhedron3<Real>::PointInPolyhedron3 (int numPoints,
    const Vector3<Real>* points, int numFaces, const ConvexFace* faces,
    int numRays, const Vector3<Real>* directions, unsigned int method)
    :
    mNumPoints(numPoints),
    mPoints(points),
    mNumFaces(numFaces),
    mTFaces(0),
    mCFaces(faces),
    mSFaces(0),
    mMethod(method),
    mNumRays(numRays),
    mDirections(directions)
{
}
//----------------------------------------------------------------------------
template <typename Real>
PointInPolyhedron3<Real>::PointInPolyhedron3 (int numPoints,
    const Vector3<Real>* points, int numFaces, const SimpleFace* faces,
    int numRays, const Vector3<Real>* directions, unsigned int method)
    :
    mNumPoints(numPoints),
    mPoints(points),
    mNumFaces(numFaces),
    mTFaces(0),
    mCFaces(0),
    mSFaces(faces),
    mMethod(method),
    mNumRays(numRays),
    mDirections(directions)
{
}
//----------------------------------------------------------------------------
template <typename Real>
bool PointInPolyhedron3<Real>::Contains (const Vector3<Real>& p) const
{
    if (mTFaces)
    {
        return ContainsT0(p);
    }

    if (mCFaces)
    {
        if (mMethod == 0)
        {
            return ContainsC0(p);
        }

        return ContainsC1C2(p, mMethod);
    }

    if (mSFaces)
    {
        if (mMethod == 0)
        {
            return ContainsS0(p);
        }

        if (mMethod == 1)
        {
            return ContainsS1(p);
        }
    }

    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool PointInPolyhedron3<Real>::FastNoIntersect (const Ray3<Real>& ray,
    const Plane3<Real>& plane)
{
    Real planeDistance = plane.Normal.Dot(ray.Origin) - plane.Constant;
    Real planeAngle = plane.Normal.Dot(ray.Direction);

    if (planeDistance < (Real)0)
    {
        // The ray origin is on the negative side of the plane.
        if (planeAngle <= (Real)0)
        {
            // The ray points away from the plane.
            return true;
        }
    }

    if (planeDistance > (Real)0)
    {
        // The ray origin is on the positive side of the plane.
        if (planeAngle >= (Real)0)
        {
            // The ray points away from the plane.
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool PointInPolyhedron3<Real>::ContainsT0 (const Vector3<Real>& p) const
{
    int insideCount = 0;

    Triangle3<Real> triangle;
    Ray3<Real> ray;
    ray.Origin = p;

    for (int j = 0; j < mNumRays; ++j)
    {
        ray.Direction = mDirections[j];

        // Zero intersections to start with.
        bool odd = false;

        const TriangleFace* face = mTFaces;
        for (int i = 0; i < mNumFaces; ++i, ++face)
        {
            // Attempt to quickly cull the triangle.
            if (FastNoIntersect(ray, face->Plane))
            {
                continue;
            }

            // Get the triangle vertices.
            for (int k = 0; k < 3; ++k)
            {
                triangle.V[k] = mPoints[face->Indices[k]];
            }

            // Test for intersection.
            if (IntrRay3Triangle3<Real>(ray, triangle).Test())
            {
                // The ray intersects the triangle.
                odd = !odd;
            }
        }

        if (odd)
        {
            insideCount++;
        }
    }

    return insideCount > mNumRays/2;
}
//----------------------------------------------------------------------------
template <typename Real>
bool PointInPolyhedron3<Real>::ContainsC0 (const Vector3<Real>& p) const
{
    int insideCount = 0;

    Triangle3<Real> triangle;
    Ray3<Real> ray;
    ray.Origin = p;

    for (int j = 0; j < mNumRays; ++j)
    {
        ray.Direction = mDirections[j];

        // Zero intersections to start with.
        bool odd = false;

        const ConvexFace* face = mCFaces;
        for (int i = 0; i < mNumFaces; ++i, ++face)
        {
            // Attempt to quickly cull the triangle.
            if (FastNoIntersect(ray, face->Plane))
            {
                continue;
            }

            // Process the triangles in a trifan of the face.
            const int numVerticesM1 = (int)face->Indices.size() - 1;
            triangle.V[0] = mPoints[face->Indices[0]];
            for (int k = 1; k < numVerticesM1; ++k)
            {
                triangle.V[1] = mPoints[face->Indices[k]];
                triangle.V[2] = mPoints[face->Indices[k + 1]];

                if (IntrRay3Triangle3<Real>(ray, triangle).Test())
                {
                    // The ray intersects the triangle.
                    odd = !odd;
                }
            }
        }

        if (odd)
        {
            insideCount++;
        }
    }

    return insideCount > mNumRays/2;
}
//----------------------------------------------------------------------------
template <typename Real>
bool PointInPolyhedron3<Real>::ContainsS0 (const Vector3<Real>& p) const
{
    int insideCount = 0;

    Triangle3<Real> triangle;
    Ray3<Real> ray;
    ray.Origin = p;

    for (int j = 0; j < mNumRays; ++j)
    {
        ray.Direction = mDirections[j];

        // Zero intersections to start with.
        bool odd = false;

        const SimpleFace* face = mSFaces;
        for (int i = 0; i < mNumFaces; ++i, ++face)
        {
            // Attempt to quickly cull the triangle.
            if (FastNoIntersect(ray, face->Plane))
            {
                continue;
            }

            // The triangulation must exist to use it.
            const int numTriangles = (int)face->Triangles.size()/3;
            assertion(numTriangles > 0, "Triangulation must exist\n");

            // Process the triangles in a triangulation of the face.
            const int* currIndex = &face->Triangles[0];
            for (int t = 0; t < numTriangles; ++t)
            {
                // Get the triangle vertices.
                for (int k = 0; k < 3; ++k)
                {
                    triangle.V[k] = mPoints[*currIndex++];
                }

                // Test for intersection.
                if (IntrRay3Triangle3<Real>(ray, triangle).Test())
                {
                    // The ray intersects the triangle.
                    odd = !odd;
                }
            }
        }

        if (odd)
        {
            insideCount++;
        }
    }

    return insideCount > mNumRays/2;
}
//----------------------------------------------------------------------------
template <typename Real>
bool PointInPolyhedron3<Real>::ContainsC1C2 (const Vector3<Real>& p,
    unsigned int method) const
{
    int insideCount = 0;

    Ray3<Real> ray;
    ray.Origin = p;

    for (int j = 0; j < mNumRays; ++j)
    {
        ray.Direction = mDirections[j];

        // Zero intersections to start with.
        bool odd = false;

        const ConvexFace* face = mCFaces;
        for (int i = 0; i < mNumFaces; ++i, ++face)
        {
            // Attempt to quickly cull the triangle.
            if (FastNoIntersect(ray, face->Plane))
            {
                continue;
            }

            // Compute the ray-plane intersection.
            IntrRay3Plane3<Real> calc(ray, face->Plane);
            bool intersects = calc.Find();

            // If you trigger this assertion, numerical round-off errors have
            // led to a discrepancy between FastNoIntersect and the Find()
            // result.
            assertion(intersects, "Unexpected condition\n");
            WM5_UNUSED(intersects);

            Vector3<Real> intr = ray.Origin +
                calc.GetRayParameter()*ray.Direction;

            // Get a coordinate system for the plane.  Use vertex 0 as the
            // origin.
            const Vector3<Real>& V0 = mPoints[face->Indices[0]];
            Vector3<Real> U0, U1;
            Vector3<Real>::GenerateComplementBasis(U0, U1,
                face->Plane.Normal);

            // Project the intersection onto the plane.
            Vector3<Real> diff = intr - V0;
            Vector2<Real> projIntersect(U0.Dot(diff), U1.Dot(diff));

            // Project the face vertices onto the plane of the face.
            if (face->Indices.size() > mProjVertices.size())
            {
                mProjVertices.resize(face->Indices.size());
            }

            // Project the remaining vertices.  Vertex 0 is always the origin.
            const int numIndices = (int)face->Indices.size();
            mProjVertices[0] = Vector2<Real>::ZERO;
            for (int k = 1; k < numIndices; ++k)
            {
                diff = mPoints[face->Indices[k]] - V0;
                mProjVertices[k][0] = U0.Dot(diff);
                mProjVertices[k][1] = U1.Dot(diff);
            }

            // Test whether the intersection point is in the convex polygon.
            PointInPolygon2<Real> PIP((int)mProjVertices.size(),
                &mProjVertices[0]);

            if (method == 1)
            {
                if (PIP.ContainsConvexOrderN(projIntersect))
                {
                    // The ray intersects the triangle.
                    odd = !odd;
                }
            }
            else
            {
                if (PIP.ContainsConvexOrderLogN(projIntersect))
                {
                    // The ray intersects the triangle.
                    odd = !odd;
                }
            }
        }

        if (odd)
        {
            insideCount++;
        }
    }

    return insideCount > mNumRays/2;
}
//----------------------------------------------------------------------------
template <typename Real>
bool PointInPolyhedron3<Real>::ContainsS1 (const Vector3<Real>& p) const
{
    int insideCount = 0;

    Ray3<Real> ray;
    ray.Origin = p;

    for (int j = 0; j < mNumRays; ++j)
    {
        ray.Direction = mDirections[j];

        // Zero intersections to start with.
        bool odd = false;

        const SimpleFace* face = mSFaces;
        for (int i = 0; i < mNumFaces; ++i, ++face)
        {
            // Attempt to quickly cull the triangle.
            if (FastNoIntersect(ray, face->Plane))
            {
                continue;
            }

            // Compute the ray-plane intersection.
            IntrRay3Plane3<Real> calc(ray, face->Plane);
            bool intersects = calc.Find();

            // If you trigger this assertion, numerical round-off errors have
            // led to a discrepancy between FastNoIntersect and the Find()
            // result.
            assertion(intersects, "Unexpected condition\n");
            WM5_UNUSED(intersects);

            Vector3<Real> intr = ray.Origin +
                calc.GetRayParameter()*ray.Direction;

            // Get a coordinate system for the plane.  Use vertex 0 as the
            // origin.
            const Vector3<Real>& V0 = mPoints[face->Indices[0]];
            Vector3<Real> U0, U1;
            Vector3<Real>::GenerateComplementBasis(U0, U1,
                face->Plane.Normal);

            // Project the intersection onto the plane.
            Vector3<Real> diff = intr - V0;
            Vector2<Real> projIntersect(U0.Dot(diff), U1.Dot(diff));

            // Project the face vertices onto the plane of the face.
            if (face->Indices.size() > mProjVertices.size())
            {
                mProjVertices.resize(face->Indices.size());
            }

            // Project the remaining vertices.  Vertex 0 is always the origin.
            const int numIndices = (int)face->Indices.size();
            mProjVertices[0] = Vector2<Real>::ZERO;
            for (int k = 1; k < numIndices; ++k)
            {
                diff = mPoints[face->Indices[k]] - V0;
                mProjVertices[k][0] = U0.Dot(diff);
                mProjVertices[k][1] = U1.Dot(diff);
            }

            // Test whether the intersection point is in the convex polygon.
            PointInPolygon2<Real> PIP((int)mProjVertices.size(),
                &mProjVertices[0]);

            if (PIP.Contains(projIntersect))
            {
                // The ray intersects the triangle.
                odd = !odd;
            }
        }

        if (odd)
        {
            insideCount++;
        }
    }

    return insideCount > mNumRays/2;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class PointInPolyhedron3<float>;

template WM5_MATHEMATICS_ITEM
class PointInPolyhedron3<double>;
//----------------------------------------------------------------------------
}
