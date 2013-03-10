// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5ContPointInPolygon2.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
PointInPolygon2<Real>::PointInPolygon2 (int numPoints,
    const Vector2<Real>* points)
    :
    mNumPoints(numPoints),
    mPoints(points)
{
}
//----------------------------------------------------------------------------
template <typename Real>
bool PointInPolygon2<Real>::Contains (const Vector2<Real>& p) const
{
    bool inside = false;
    for (int i = 0, j = mNumPoints-1; i < mNumPoints; j = i++)
    {
        const Vector2<Real>& U0 = mPoints[i];
        const Vector2<Real>& U1 = mPoints[j];
        Real rhs, lhs;

        if (p.Y() < U1.Y())  // U1 above ray
        {
            if (U0.Y() <= p.Y())  // U0 on or below ray
            {
                lhs = (p.Y() - U0.Y())*(U1.X() - U0.X());
                rhs = (p.X() - U0.X())*(U1.Y() - U0.Y());
                if (lhs > rhs)
                {
                    inside = !inside;
                }
            }
        }
        else if (p.Y() < U0.Y())  // U1 on or below ray, U0 above ray
        {
            lhs = (p.Y() - U0.Y())*(U1.X() - U0.X());
            rhs = (p.X() - U0.X())*(U1.Y() - U0.Y());
            if (lhs < rhs)
            {
                inside = !inside;
            }
        }
    }
    return inside;
}
//----------------------------------------------------------------------------
template <typename Real>
bool PointInPolygon2<Real>::ContainsConvexOrderN (const Vector2<Real>& p)
    const
{
    for (int i1 = 0, i0 = mNumPoints-1; i1 < mNumPoints; i0 = i1++)
    {
        Real nx = mPoints[i1].Y() - mPoints[i0].Y();
        Real ny = mPoints[i0].X() - mPoints[i1].X();
        Real dx = p.X() - mPoints[i0].X();
        Real dy = p.Y() - mPoints[i0].Y();
        if (nx*dx + ny*dy > (Real)0)
        {
            return false;
        }
    }
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool PointInPolygon2<Real>::ContainsConvexOrderLogN (const Vector2<Real>& p)
    const
{
    return SubContainsPoint(p, 0, 0);
}
//----------------------------------------------------------------------------
template <typename Real>
bool PointInPolygon2<Real>::ContainsQuadrilateral (const Vector2<Real>& p)
    const
{
    if (mNumPoints != 4)
    {
        return false;
    }

    Real nx = mPoints[2].Y() - mPoints[0].Y();
    Real ny = mPoints[0].X() - mPoints[2].X();
    Real dx = p.X() - mPoints[0].X();
    Real dy = p.Y() - mPoints[0].Y();

    if (nx*dx + ny*dy > (Real)0)
    {
        // P potentially in <V0,V1,V2>
        nx = mPoints[1].Y() - mPoints[0].Y();
        ny = mPoints[0].X() - mPoints[1].X();
        if (nx*dx + ny*dy > (Real)0.0)
        {
            return false;
        }

        nx = mPoints[2].Y() - mPoints[1].Y();
        ny = mPoints[1].X() - mPoints[2].X();
        dx = p.X() - mPoints[1].X();
        dy = p.Y() - mPoints[1].Y();
        if (nx*dx + ny*dy > (Real)0)
        {
            return false;
        }
    }
    else
    {
        // P potentially in <V0,V2,V3>
        nx = mPoints[0].Y() - mPoints[3].Y();
        ny = mPoints[3].X() - mPoints[0].X();
        if (nx*dx + ny*dy > (Real)0)
        {
            return false;
        }

        nx = mPoints[3].Y() - mPoints[2].Y();
        ny = mPoints[2].X() - mPoints[3].X();
        dx = p.X() - mPoints[3].X();
        dy = p.Y() - mPoints[3].Y();
        if (nx*dx + ny*dy > (Real)0)
        {
            return false;
        }
    }
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool PointInPolygon2<Real>::SubContainsPoint (const Vector2<Real>& p, int i0,
    int i1) const
{
    Real nx, ny, dx, dy;

    int diff = i1 - i0;
    if (diff == 1 || (diff < 0 && diff + mNumPoints == 1))
    {
        nx = mPoints[i1].Y() - mPoints[i0].Y();
        ny = mPoints[i0].X() - mPoints[i1].X();
        dx = p.X() - mPoints[i0].X();
        dy = p.Y() - mPoints[i0].Y();
        return nx*dx + ny*dy <= (Real)0;
    }

    // Bisect the index range.
    int mid;
    if (i0 < i1)
    {
        mid = (i0 + i1) >> 1;
    }
    else
    {
        mid = ((i0 + i1 + mNumPoints) >> 1);
        if (mid >= mNumPoints)
        {
            mid -= mNumPoints;
        }
    }

    // Determine which side of the splitting line contains the point.
    nx = mPoints[mid].Y() - mPoints[i0].Y();
    ny = mPoints[i0].X() - mPoints[mid].X();
    dx = p.X() - mPoints[i0].X();
    dy = p.Y() - mPoints[i0].Y();
    if (nx*dx + ny*dy > (Real)0)
    {
        // P potentially in <V(i0),V(i0+1),...,V(mid-1),V(mid)>
        return SubContainsPoint(p, i0, mid);
    }
    else
    {
        // P potentially in <V(mid),V(mid+1),...,V(i1-1),V(i1)>
        return SubContainsPoint(p, mid, i1);
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class PointInPolygon2<float>;

template WM5_MATHEMATICS_ITEM
class PointInPolygon2<double>;
//----------------------------------------------------------------------------
}
