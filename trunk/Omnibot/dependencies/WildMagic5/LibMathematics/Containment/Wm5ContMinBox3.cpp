// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ContMinBox3.h"
#include "Wm5ContMinBox2.h"
#include "Wm5ConvexHull3.h"
#include "Wm5EdgeKey.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
MinBox3<Real>::MinBox3 (int numPoints, const Vector3<Real>* points,
    Real epsilon, Query::Type queryType)
{
    // Get the convex hull of the points.
    ConvexHull3<Real> kHull(numPoints,(Vector3<Real>*)points, epsilon, false,
        queryType);
    int hullDim = kHull.GetDimension();

    if (hullDim == 0)
    {
        mMinBox.Center = points[0];
        mMinBox.Axis[0] = Vector3<Real>::UNIT_X;
        mMinBox.Axis[1] = Vector3<Real>::UNIT_Y;
        mMinBox.Axis[2] = Vector3<Real>::UNIT_Z;
        mMinBox.Extent[0] = (Real)0;
        mMinBox.Extent[1] = (Real)0;
        mMinBox.Extent[2] = (Real)0;
        return;
    }

    if (hullDim == 1)
    {
        ConvexHull1<Real>* pkHull1 = kHull.GetConvexHull1();
        const int* hullIndices = pkHull1->GetIndices();

        mMinBox.Center =
            ((Real)0.5)*(points[hullIndices[0]] + points[hullIndices[1]]);
        Vector3<Real> diff =
            points[hullIndices[1]] - points[hullIndices[0]];
        mMinBox.Extent[0] = ((Real)0.5)*diff.Normalize();
        mMinBox.Extent[1] = (Real)0;
        mMinBox.Extent[2] = (Real)0;
        mMinBox.Axis[0] = diff;
        Vector3<Real>::GenerateComplementBasis(mMinBox.Axis[1],
            mMinBox.Axis[2], mMinBox.Axis[0]);

        delete0(pkHull1);
        return;
    }

    int i, j;
    Vector3<Real> origin, diff, U, V, W;
    Vector2<Real>* points2;
    Box2<Real> box2;

    if (hullDim == 2)
    {
        // When ConvexHull3 reports that the point set is 2-dimensional, the
        // caller is responsible for projecting the points onto a plane and
        // calling ConvexHull2.  ConvexHull3 does provide information about
        // the plane of the points.  In this application, we need only
        // project the input points onto that plane and call ContMinBox in
        // two dimensions.

        // Get a coordinate system relative to the plane of the points.
        origin = kHull.GetPlaneOrigin();
        W = kHull.GetPlaneDirection(0).Cross(kHull.GetPlaneDirection(1));
        Vector3<Real>::GenerateComplementBasis(U, V, W);

        // Project the input points onto the plane.
        points2 = new1<Vector2<Real> >(numPoints);
        for (i = 0; i < numPoints; ++i)
        {
            diff = points[i] - origin;
            points2[i].X() = U.Dot(diff);
            points2[i].Y() = V.Dot(diff);
        }

        // Compute the minimum area box in 2D.
        box2 = MinBox2<Real>(numPoints, points2, epsilon, queryType, false);
        delete1(points2);

        // Lift the values into 3D.
        mMinBox.Center = origin + box2.Center.X()*U + box2.Center.Y()*V;
        mMinBox.Axis[0] = box2.Axis[0].X()*U + box2.Axis[0].Y()*V;
        mMinBox.Axis[1] = box2.Axis[1].X()*U + box2.Axis[1].Y()*V;
        mMinBox.Axis[2] = W;
        mMinBox.Extent[0] = box2.Extent[0];
        mMinBox.Extent[1] = box2.Extent[1];
        mMinBox.Extent[2] = (Real)0;
        return;
    }

    int hullQuantity = kHull.GetNumSimplices();
    const int* hullIndices = kHull.GetIndices();
    Real volume, minVolume = Math<Real>::MAX_REAL;

    // Create the unique set of hull vertices to minimize the time spent
    // projecting vertices onto planes of the hull faces.
    std::set<int> uniqueIndices;
    for (i = 0; i < 3*hullQuantity; ++i)
    {
        uniqueIndices.insert(hullIndices[i]);
    }

    // Use the rotating calipers method on the projection of the hull onto
    // the plane of each face.  Also project the hull onto the normal line
    // of each face.  The minimum area box in the plane and the height on
    // the line produce a containing box.  If its volume is smaller than the
    // current volume, this box is the new candidate for the minimum volume
    // box.  The unique edges are accumulated into a set for use by a later
    // step in the algorithm.
    const int* currentHullIndex = hullIndices;
    Real height, minHeight, maxHeight;
    std::set<EdgeKey> edges;
    points2 = new1<Vector2<Real> >(uniqueIndices.size());
    for (i = 0; i < hullQuantity; ++i)
    {
        // Get the triangle.
        int v0 = *currentHullIndex++;
        int v1 = *currentHullIndex++;
        int v2 = *currentHullIndex++;

        // Save the edges for later use.
        edges.insert(EdgeKey(v0, v1));
        edges.insert(EdgeKey(v1, v2));
        edges.insert(EdgeKey(v2, v0));

        // Get 3D coordinate system relative to plane of triangle.
        origin = (points[v0] + points[v1] + points[v2])/(Real)3.0;
        Vector3<Real> edge1 = points[v1] - points[v0];
        Vector3<Real> edge2 = points[v2] - points[v0];
        W = edge2.UnitCross(edge1);  // inner-pointing normal
        if (W == Vector3<Real>::ZERO)
        {
            // The triangle is needle-like, so skip it.
            continue;
        }
        Vector3<Real>::GenerateComplementBasis(U, V, W);

        // Project points onto plane of triangle, onto normal line of plane.
        // TO DO.  In theory, minHeight should be zero since W points to the
        // interior of the hull.  However, the snap rounding used in the 3D
        // convex hull finder involves loss of precision, which in turn can
        // cause a hull facet to have the wrong ordering (clockwise instead
        // of counterclockwise when viewed from outside the hull).  The
        // height calculations here trap that problem (the incorrectly ordered
        // face will not affect the minimum volume box calculations).
        minHeight = (Real)0;
        maxHeight = (Real)0;
        j = 0;
        std::set<int>::const_iterator iter = uniqueIndices.begin();
        while (iter != uniqueIndices.end())
        {
            int index = *iter++;
            diff = points[index] - origin;
            points2[j].X() = U.Dot(diff);
            points2[j].Y() = V.Dot(diff);
            height = W.Dot(diff);
            if (height > maxHeight)
            {
                maxHeight = height;
            }
            else if (height < minHeight)
            {
                minHeight = height;
            }

            j++;
        }
        if (-minHeight > maxHeight)
        {
            maxHeight = -minHeight;
        }

        // Compute minimum area box in 2D.
        box2 = MinBox2<Real>((int)uniqueIndices.size(), points2, epsilon,
            queryType, false);

        // Update current minimum-volume box (if necessary).
        volume = maxHeight*box2.Extent[0]*box2.Extent[1];
        if (volume < minVolume)
        {
            minVolume = volume;

            // Lift the values into 3D.
            mMinBox.Extent[0] = box2.Extent[0];
            mMinBox.Extent[1] = box2.Extent[1];
            mMinBox.Extent[2] = ((Real)0.5)*maxHeight;
            mMinBox.Axis[0] = box2.Axis[0].X()*U + box2.Axis[0].Y()*V;
            mMinBox.Axis[1] = box2.Axis[1].X()*U + box2.Axis[1].Y()*V;
            mMinBox.Axis[2] = W;
            mMinBox.Center = origin + box2.Center.X()*U + box2.Center.Y()*V
                + mMinBox.Extent[2]*W;
        }
    }

    // The minimum-volume box can also be supported by three mutually
    // orthogonal edges of the convex hull.  For each triple of orthogonal
    // edges, compute the minimum-volume box for that coordinate frame by
    // projecting the points onto the axes of the frame.
    std::set<EdgeKey>::const_iterator e2iter;
    for (e2iter = edges.begin(); e2iter != edges.end(); e2iter++)
    {
        W = points[e2iter->V[1]] - points[e2iter->V[0]];
        W.Normalize();

        std::set<EdgeKey>::const_iterator e1iter = e2iter;
        for (++e1iter; e1iter != edges.end(); e1iter++)
        {
            V = points[e1iter->V[1]] - points[e1iter->V[0]];
            V.Normalize();
            Real dot = V.Dot(W);
            if (Math<Real>::FAbs(dot) > Math<Real>::ZERO_TOLERANCE)
            {
                continue;
            }

            std::set<EdgeKey>::const_iterator e0iter = e1iter;
            for (++e0iter; e0iter != edges.end(); e0iter++)
            {
                U = points[e0iter->V[1]] - points[e0iter->V[0]];
                U.Normalize();
                dot = U.Dot(V);
                if (Math<Real>::FAbs(dot) > Math<Real>::ZERO_TOLERANCE)
                {
                    continue;
                }
                dot = U.Dot(W);
                if (Math<Real>::FAbs(dot) > Math<Real>::ZERO_TOLERANCE)
                {
                    continue;
                }
    
                // The three edges are mutually orthogonal.  Project the
                // hull points onto the lines containing the edges.  Use
                // hull point zero as the origin.
                Real umin = (Real)0, umax = (Real)0;
                Real vmin = (Real)0, vmax = (Real)0;
                Real wmin = (Real)0, wmax = (Real)0;
                origin = points[hullIndices[0]];

                std::set<int>::const_iterator iter = uniqueIndices.begin();
                while (iter != uniqueIndices.end())
                {
                    int index = *iter++;
                    diff = points[index] - origin;

                    Real fU = U.Dot(diff);
                    if (fU < umin)
                    {
                        umin = fU;
                    }
                    else if (fU > umax)
                    {
                        umax = fU;
                    }

                    Real fV = V.Dot(diff);
                    if (fV < vmin)
                    {
                        vmin = fV;
                    }
                    else if (fV > vmax)
                    {
                        vmax = fV;
                    }

                    Real fW = W.Dot(diff);
                    if (fW < wmin)
                    {
                        wmin = fW;
                    }
                    else if (fW > wmax)
                    {
                        wmax = fW;
                    }
                }

                Real uExtent = ((Real)0.5)*(umax - umin);
                Real vExtent = ((Real)0.5)*(vmax - vmin);
                Real wExtent = ((Real)0.5)*(wmax - wmin);

                // Update current minimum-volume box (if necessary).
                volume = uExtent*vExtent*wExtent;
                if (volume < minVolume)
                {
                    minVolume = volume;

                    mMinBox.Extent[0] = uExtent;
                    mMinBox.Extent[1] = vExtent;
                    mMinBox.Extent[2] = wExtent;
                    mMinBox.Axis[0] = U;
                    mMinBox.Axis[1] = V;
                    mMinBox.Axis[2] = W;
                    mMinBox.Center = origin +
                        ((Real)0.5)*(umin+umax)*U +
                        ((Real)0.5)*(vmin+vmax)*V +
                        ((Real)0.5)*(wmin+wmax)*W;
                }
            }
        }
    }

    delete1(points2);
}
//----------------------------------------------------------------------------
template <typename Real>
MinBox3<Real>::operator Box3<Real> () const
{
    return mMinBox;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class MinBox3<float>;

template WM5_MATHEMATICS_ITEM
class MinBox3<double>;
//----------------------------------------------------------------------------
}
