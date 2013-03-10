// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ContMinBox2.h"
#include "Wm5ConvexHull2.h"
#include "Wm5Memory.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
MinBox2<Real>::MinBox2 (int numPoints, const Vector2<Real>* points,
    Real epsilon, Query::Type queryType, bool isConvexPolygon)
{
    // Get the convex hull of the points.
    Vector2<Real>* hullPoints = 0;
    if (isConvexPolygon)
    {
        hullPoints = (Vector2<Real>*)points;
    }
    else
    {
        ConvexHull2<Real> hull(numPoints, (Vector2<Real>*)points, epsilon,
            false, queryType);
        int hullDim = hull.GetDimension();
        int hullNumSimplices = hull.GetNumSimplices();
        const int* hullIndices = hull.GetIndices();

        if (hullDim == 0)
        {
            mMinBox.Center = points[0];
            mMinBox.Axis[0] = Vector2<Real>::UNIT_X;
            mMinBox.Axis[1] = Vector2<Real>::UNIT_Y;
            mMinBox.Extent[0] = (Real)0;
            mMinBox.Extent[1] = (Real)0;
            return;
        }

        if (hullDim == 1)
        {
            ConvexHull1<Real>* hull1 = hull.GetConvexHull1();
            hullIndices = hull1->GetIndices();

            mMinBox.Center = ((Real)0.5)*(points[hullIndices[0]] +
                points[hullIndices[1]]);
            Vector2<Real> diff =
                points[hullIndices[1]] - points[hullIndices[0]];
            mMinBox.Extent[0] = ((Real)0.5)*diff.Normalize();
            mMinBox.Extent[1] = (Real)0.0;
            mMinBox.Axis[0] = diff;
            mMinBox.Axis[1] = -mMinBox.Axis[0].Perp();

            delete0(hull1);
            return;
        }

        numPoints = hullNumSimplices;
        hullPoints = new1<Vector2<Real> >(numPoints);
        for (int i = 0; i < numPoints; ++i)
        {
            hullPoints[i] = points[hullIndices[i]];
        }
    }
    
    // The input points are V[0] through V[N-1] and are assumed to be the
    // vertices of a convex polygon that are counterclockwise ordered.  The
    // input points must not contain three consecutive collinear points.

    // Unit-length edge directions of convex polygon.  These could be
    // precomputed and passed to this routine if the application requires it.
    int numPointsM1 = numPoints -1;
    Vector2<Real>* edges = new1<Vector2<Real> >(numPoints);
    bool* visited = new1<bool>(numPoints);
    int i;
    for (i = 0; i < numPointsM1; ++i)
    {
        edges[i] = hullPoints[i + 1] - hullPoints[i];
        edges[i].Normalize();
        visited[i] = false;
    }
    edges[numPointsM1] = hullPoints[0] - hullPoints[numPointsM1];
    edges[numPointsM1].Normalize();
    visited[numPointsM1] = false;

    // Find the smallest axis-aligned box containing the points.  Keep track
    // of the extremum indices, L (left), R (right), B (bottom), and T (top)
    // so that the following constraints are met:
    //   V[L].X() <= V[i].X() for all i and V[(L+1)%N].X() > V[L].X()
    //   V[R].X() >= V[i].X() for all i and V[(R+1)%N].X() < V[R].X()
    //   V[B].Y() <= V[i].Y() for all i and V[(B+1)%N].Y() > V[B].Y()
    //   V[T].Y() >= V[i].Y() for all i and V[(T+1)%N].Y() < V[T].Y()
    Real xmin = hullPoints[0].X(), xmax = xmin;
    Real ymin = hullPoints[0].Y(), ymax = ymin;
    int LIndex = 0, RIndex = 0, BIndex = 0, TIndex = 0;
    for (i = 1; i < numPoints; ++i)
    {
        if (hullPoints[i].X() <= xmin)
        {
            xmin = hullPoints[i].X();
            LIndex = i;
        }
        if (hullPoints[i].X() >= xmax)
        {
            xmax = hullPoints[i].X();
            RIndex = i;
        }

        if (hullPoints[i].Y() <= ymin)
        {
            ymin = hullPoints[i].Y();
            BIndex = i;
        }
        if (hullPoints[i].Y() >= ymax)
        {
            ymax = hullPoints[i].Y();
            TIndex = i;
        }
    }

    // Apply wrap-around tests to ensure the constraints mentioned above are
    // satisfied.
    if (LIndex == numPointsM1)
    {
        if (hullPoints[0].X() <= xmin)
        {
            xmin = hullPoints[0].X();
            LIndex = 0;
        }
    }

    if (RIndex == numPointsM1)
    {
        if (hullPoints[0].X() >= xmax)
        {
            xmax = hullPoints[0].X();
            RIndex = 0;
        }
    }

    if (BIndex == numPointsM1)
    {
        if (hullPoints[0].Y() <= ymin)
        {
            ymin = hullPoints[0].Y();
            BIndex = 0;
        }
    }

    if (TIndex == numPointsM1)
    {
        if (hullPoints[0].Y() >= ymax)
        {
            ymax = hullPoints[0].Y();
            TIndex = 0;
        }
    }

    // The dimensions of the axis-aligned box.  The extents store width and
    // height for now.
    mMinBox.Center.X() = ((Real)0.5)*(xmin + xmax);
    mMinBox.Center.Y() = ((Real)0.5)*(ymin + ymax);
    mMinBox.Axis[0] = Vector2<Real>::UNIT_X;
    mMinBox.Axis[1] = Vector2<Real>::UNIT_Y;
    mMinBox.Extent[0] = ((Real)0.5)*(xmax - xmin);
    mMinBox.Extent[1] = ((Real)0.5)*(ymax - ymin);
    Real minAreaDiv4 = mMinBox.Extent[0]*mMinBox.Extent[1];

    // The rotating calipers algorithm.
    Vector2<Real> U = Vector2<Real>::UNIT_X;
    Vector2<Real> V = Vector2<Real>::UNIT_Y;

    bool done = false;
    while (!done)
    {
        // Determine the edge that forms the smallest angle with the current
        // box edges.
        int flag = F_NONE;
        Real maxDot = (Real)0;

        Real dot = U.Dot(edges[BIndex]);
        if (dot > maxDot)
        {
            maxDot = dot;
            flag = F_BOTTOM;
        }

        dot = V.Dot(edges[RIndex]);
        if (dot > maxDot)
        {
            maxDot = dot;
            flag = F_RIGHT;
        }

        dot = -U.Dot(edges[TIndex]);
        if (dot > maxDot)
        {
            maxDot = dot;
            flag = F_TOP;
        }

        dot = -V.Dot(edges[LIndex]);
        if (dot > maxDot)
        {
            maxDot = dot;
            flag = F_LEFT;
        }

        switch (flag)
        {
        case F_BOTTOM:
            if (visited[BIndex])
            {
                done = true;
            }
            else
            {
                // Compute box axes with E[B] as an edge.
                U = edges[BIndex];
                V = -U.Perp();
                UpdateBox(hullPoints[LIndex], hullPoints[RIndex],
                    hullPoints[BIndex], hullPoints[TIndex], U, V,
                    minAreaDiv4);

                // Mark edge visited and rotate the calipers.
                visited[BIndex] = true;
                if (++BIndex == numPoints)
                {
                    BIndex = 0;
                }
            }
            break;
        case F_RIGHT:
            if (visited[RIndex])
            {
                done = true;
            }
            else
            {
                // Compute box axes with E[R] as an edge.
                V = edges[RIndex];
                U = V.Perp();
                UpdateBox(hullPoints[LIndex], hullPoints[RIndex],
                    hullPoints[BIndex], hullPoints[TIndex], U, V,
                    minAreaDiv4);

                // Mark edge visited and rotate the calipers.
                visited[RIndex] = true;
                if (++RIndex == numPoints)
                {
                    RIndex = 0;
                }
            }
            break;
        case F_TOP:
            if (visited[TIndex])
            {
                done = true;
            }
            else
            {
                // Compute box axes with E[T] as an edge.
                U = -edges[TIndex];
                V = -U.Perp();
                UpdateBox(hullPoints[LIndex], hullPoints[RIndex],
                    hullPoints[BIndex], hullPoints[TIndex], U, V,
                    minAreaDiv4);

                // Mark edge visited and rotate the calipers.
                visited[TIndex] = true;
                if (++TIndex == numPoints)
                {
                    TIndex = 0;
                }
            }
            break;
        case F_LEFT:
            if (visited[LIndex])
            {
                done = true;
            }
            else
            {
                // Compute box axes with E[L] as an edge.
                V = -edges[LIndex];
                U = V.Perp();
                UpdateBox(hullPoints[LIndex], hullPoints[RIndex],
                    hullPoints[BIndex], hullPoints[TIndex], U, V,
                    minAreaDiv4);

                // Mark edge visited and rotate the calipers.
                visited[LIndex] = true;
                if (++LIndex == numPoints)
                {
                    LIndex = 0;
                }
            }
            break;
        case F_NONE:
            // The polygon is a rectangle.
            done = true;
            break;
        }
    }

    delete1(visited);
    delete1(edges);
    if (!isConvexPolygon)
    {
        delete1(hullPoints);
    }
}
//----------------------------------------------------------------------------
template <typename Real>
MinBox2<Real>::operator Box2<Real> () const
{
    return mMinBox;
}
//----------------------------------------------------------------------------
template <typename Real>
void MinBox2<Real>::UpdateBox (const Vector2<Real>& LPoint,
    const Vector2<Real>& RPoint, const Vector2<Real>& BPoint,
    const Vector2<Real>& TPoint, const Vector2<Real>& U,
    const Vector2<Real>& V, Real& minAreaDiv4)
{
    Vector2<Real> RLDiff = RPoint - LPoint;
    Vector2<Real> TBDiff = TPoint - BPoint;
    Real extent0 = ((Real)0.5)*(U.Dot(RLDiff));
    Real extent1 = ((Real)0.5)*(V.Dot(TBDiff));
    Real areaDiv4 = extent0*extent1;
    if (areaDiv4 < minAreaDiv4)
    {
        minAreaDiv4 = areaDiv4;
        mMinBox.Axis[0] = U;
        mMinBox.Axis[1] = V;
        mMinBox.Extent[0] = extent0;
        mMinBox.Extent[1] = extent1;
        Vector2<Real> LBDiff = LPoint - BPoint;
        mMinBox.Center = LPoint + U*extent0 + V*(extent1 - V.Dot(LBDiff));
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class MinBox2<float>;

template WM5_MATHEMATICS_ITEM
class MinBox2<double>;
//----------------------------------------------------------------------------
}
