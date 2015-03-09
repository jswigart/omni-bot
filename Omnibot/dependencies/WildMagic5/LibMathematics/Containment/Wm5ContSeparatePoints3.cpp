// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ContSeparatePoints3.h"
#include "Wm5ConvexHull3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
SeparatePoints3<Real>::SeparatePoints3 (int numPoints0,
    const Vector3<Real>* points0, int numPoints1,
    const Vector3<Real>* points1, Plane3<Real>& separatingPlane)
{
    // Construct convex hull of point set 0.
    ConvexHull3<Real> hull0(numPoints0, (Vector3<Real>*)points0, 0.001f,
        false, Query::QT_INT64);

    // Code does not currently handle point/segment/polygon hull.
    assertion(hull0.GetDimension() == 3,
        "Code currently supports only noncoplanar points\n");
    if (hull0.GetDimension() < 3)
    {
        return;
    }

    int numTriangles0 = hull0.GetNumSimplices();
    const int* indices0 = hull0.GetIndices();

    // Construct convex hull of point set 1.
    ConvexHull3<Real> hull1(numPoints1, (Vector3<Real>*)points1, 0.001f,
        false, Query::QT_INT64);

    // Code does not currently handle point/segment/polygon hull.
    assertion(hull1.GetDimension() == 3,
        "Code currently supports only noncoplanar points\n");
    if (hull1.GetDimension() < 3)
    {
        return;
    }

    int numTriangles1 = hull1.GetNumSimplices();
    const int* indices1 = hull1.GetIndices();

    // Test faces of hull 0 for possible separation of points.
    int i, i0, i1, i2, side0, side1;
    Vector3<Real> diff0, diff1;
    for (i = 0; i < numTriangles0; ++i)
    {
        // Look up face (assert: i0 != i1 && i0 != i2 && i1 != i2).
        i0 = indices0[3*i  ];
        i1 = indices0[3*i+1];
        i2 = indices0[3*i+2];

        // Compute potential separating plane (assert: normal != (0,0,0)).
        separatingPlane = Plane3<Real>(points0[i0], points0[i1], points0[i2]);

        // Determine if hull 1 is on same side of plane.
        side1 = OnSameSide(separatingPlane, numTriangles1, indices1, points1);

        if (side1)
        {
            // Determine which side of plane hull 0 lies.
            side0 = WhichSide(separatingPlane, numTriangles0, indices0,
                points0);
            if (side0*side1 <= 0)  // Plane separates hulls.
            {
                mSeparated = true;
                return;
            }
        }
    }

    // Test faces of hull 1 for possible separation of points.
    for (i = 0; i < numTriangles1; ++i)
    {
        // Look up edge (assert: i0 != i1 && i0 != i2 && i1 != i2).
        i0 = indices1[3*i  ];
        i1 = indices1[3*i+1];
        i2 = indices1[3*i+2];

        // Compute perpendicular to face (assert: normal != (0,0,0)).
        separatingPlane = Plane3<Real>(points1[i0], points1[i1], points1[i2]);

        // Determine if hull 0 is on same side of plane.
        side0 = OnSameSide(separatingPlane, numTriangles0, indices0, points0);
        if (side0)
        {
            // Determine which side of plane hull 1 lies.
            side1 = WhichSide(separatingPlane, numTriangles1, indices1,
                points1);
            if (side0*side1 <= 0)  // Plane separates hulls.
            {
                mSeparated = true;
                return;
            }
        }
    }

    // Build edge set for hull 0.
    std::set<std::pair<int,int> > edgeSet0;
    for (i = 0; i < numTriangles0; ++i)
    {
        // Look up face (assert: i0 != i1 && i0 != i2 && i1 != i2).
        i0 = indices0[3*i  ];
        i1 = indices0[3*i+1];
        i2 = indices0[3*i+2];
        edgeSet0.insert(std::make_pair(i0, i1));
        edgeSet0.insert(std::make_pair(i0, i2));
        edgeSet0.insert(std::make_pair(i1, i2));
    }

    // Build edge list for hull 1.
    std::set<std::pair<int,int> > edgeSet1;
    for (i = 0; i < numTriangles1; ++i)
    {
        // Look up face (assert: i0 != i1 && i0 != i2 && i1 != i2).
        i0 = indices1[3*i  ];
        i1 = indices1[3*i+1];
        i2 = indices1[3*i+2];
        edgeSet1.insert(std::make_pair(i0, i1));
        edgeSet1.insert(std::make_pair(i0, i2));
        edgeSet1.insert(std::make_pair(i1, i2));
    }

    // Test planes whose normals are cross products of two edges, one from
    // each hull.
    std::set<std::pair<int,int> >::iterator e0iter = edgeSet0.begin();
    std::set<std::pair<int,int> >::iterator e0end = edgeSet0.end();
    for (/**/; e0iter != e0end; ++e0iter)
    {
        // Get edge.
        diff0 = points0[e0iter->second] - points0[e0iter->first];

        std::set<std::pair<int,int> >::iterator e1iter = edgeSet0.begin();
        std::set<std::pair<int,int> >::iterator e1end = edgeSet0.end();
        for (/**/; e1iter != e1end; ++e1iter)
        {
            diff1 = points1[e1iter->second] - points1[e1iter->first];

            // Compute potential separating plane.
            separatingPlane.Normal = diff0.UnitCross(diff1);
            separatingPlane.Constant = separatingPlane.Normal.Dot(
                points0[e0iter->first]);

            // Determine if hull 0 is on same side of plane.
            side0 = OnSameSide(separatingPlane, numTriangles0, indices0,
                points0);
            side1 = OnSameSide(separatingPlane, numTriangles1, indices1,
                points1);

            if (side0*side1 < 0)  // Plane separates hulls.
            {
                mSeparated = true;
                return;
            }
        }
    }

    mSeparated = false;
}
//----------------------------------------------------------------------------
template <typename Real>
SeparatePoints3<Real>::operator bool ()
{
    return mSeparated;
}
//----------------------------------------------------------------------------
template <typename Real>
int SeparatePoints3<Real>::OnSameSide (const Plane3<Real>& plane,
    int numTriangles, const int* indices, const Vector3<Real>* points)
{
    // test if all points on same side of plane (nx,ny,nz)*(x,y,z) = c
    int posSide = 0, negSide = 0;

    for (int t = 0; t < numTriangles; ++t)
    {
        for (int i = 0; i < 3; ++i)
        {
            int v = indices[3*t + i];
            Real c0 = plane.Normal.Dot(points[v]);
            if (c0 > plane.Constant + Math<Real>::ZERO_TOLERANCE)
            {
                ++posSide;
            }
            else if (c0 < plane.Constant - Math<Real>::ZERO_TOLERANCE)
            {
                ++negSide;
            }
            
            if (posSide && negSide)
            {
                // Plane splits point set.
                return 0;
            }
        }
    }

    return (posSide ? +1 : -1);
}
//----------------------------------------------------------------------------
template <typename Real>
int SeparatePoints3<Real>::WhichSide (const Plane3<Real>& plane,
    int numTriangles, const int* indices, const Vector3<Real>* points)
{
    // Establish which side of plane hull is on.
    for (int t = 0; t < numTriangles; ++t)
    {
        for (int i = 0; i < 3; ++i)
        {
            int v = indices[3*t + i];
            Real c0 = plane.Normal.Dot(points[v]);
            if (c0 > plane.Constant + Math<Real>::ZERO_TOLERANCE)
            {
                // Positive side.
                return +1;
            }
            if (c0 < plane.Constant - Math<Real>::ZERO_TOLERANCE)
            {
                // Negative side.
                return -1;
            }
        }
    }

    // Hull is effectively collinear.
    return 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class SeparatePoints3<float>;

template WM5_MATHEMATICS_ITEM
class SeparatePoints3<double>;
//----------------------------------------------------------------------------
}
