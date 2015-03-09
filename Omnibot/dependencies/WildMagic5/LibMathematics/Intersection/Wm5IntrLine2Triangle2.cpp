// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrLine2Triangle2.h"
#include "Wm5Intersector1.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrLine2Triangle2<Real>::IntrLine2Triangle2 (const Line2<Real>& line,
    const Triangle2<Real>& triangle)
    :
    mLine(&line),
    mTriangle(&triangle)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Line2<Real>& IntrLine2Triangle2<Real>::GetLine () const
{
    return *mLine;
}
//----------------------------------------------------------------------------
template <typename Real>
const Triangle2<Real>& IntrLine2Triangle2<Real>::GetTriangle () const
{
    return *mTriangle;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrLine2Triangle2<Real>::Test ()
{
    Real dist[3];
    int sign[3], positive, negative, zero;
    TriangleLineRelations(mLine->Origin, mLine->Direction, *mTriangle,
        dist, sign, positive, negative, zero);

    if (positive == 3 || negative == 3)
    {
        mIntersectionType = IT_EMPTY;
    }
    else
    {
        Real param[2];
        GetInterval(mLine->Origin, mLine->Direction, *mTriangle, dist,
            sign, param);

        Intersector1<Real> intr(param[0], param[1],
            -Math<Real>::MAX_REAL, +Math<Real>::MAX_REAL);

        intr.Find();

        mQuantity = intr.GetNumIntersections();
        if (mQuantity == 2)
        {
            mIntersectionType = IT_SEGMENT;
        }
        else if (mQuantity == 1)
        {
            mIntersectionType = IT_POINT;
        }
        else
        {
            mIntersectionType = IT_EMPTY;
        }
    }

    return mIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrLine2Triangle2<Real>::Find ()
{
    Real dist[3];
    int sign[3], positive, negative, zero;
    TriangleLineRelations(mLine->Origin, mLine->Direction, *mTriangle,
        dist, sign, positive, negative, zero);

    if (positive == 3 || negative == 3)
    {
        // No intersections.
        mQuantity = 0;
        mIntersectionType = IT_EMPTY;
    }
    else
    {
        Real param[2];
        GetInterval(mLine->Origin, mLine->Direction, *mTriangle, dist,
            sign, param);

        Intersector1<Real> intr(param[0], param[1],
            -Math<Real>::MAX_REAL, +Math<Real>::MAX_REAL);

        intr.Find();

        mQuantity = intr.GetNumIntersections();
        if (mQuantity == 2)
        {
            // Segment intersection.
            mIntersectionType = IT_SEGMENT;
            mPoint[0] = mLine->Origin +
                intr.GetIntersection(0)*mLine->Direction;
            mPoint[1] = mLine->Origin +
                intr.GetIntersection(1)*mLine->Direction;
        }
        else if (mQuantity == 1)
        {
            // Point intersection.
            mIntersectionType = IT_POINT;
            mPoint[0] = mLine->Origin +
                intr.GetIntersection(0)*mLine->Direction;
        }
        else
        {
            // No intersections.
            mIntersectionType = IT_EMPTY;
        }
    }

    return mIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrLine2Triangle2<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector2<Real>& IntrLine2Triangle2<Real>::GetPoint (int i) const
{
    return mPoint[i];
}
//----------------------------------------------------------------------------
template <typename Real>
void IntrLine2Triangle2<Real>::TriangleLineRelations (
    const Vector2<Real>& origin, const Vector2<Real>& direction,
    const Triangle2<Real>& triangle, Real dist[3], int sign[3],
    int& positive, int& negative, int& zero)
{
    positive = 0;
    negative = 0;
    zero = 0;
    for (int i = 0; i < 3; ++i)
    {
        Vector2<Real> diff = triangle.V[i] - origin;
        dist[i] = diff.DotPerp(direction);
        if (dist[i] > Math<Real>::ZERO_TOLERANCE)
        {
            sign[i] = 1;
            ++positive;
        }
        else if (dist[i] < -Math<Real>::ZERO_TOLERANCE)
        {
            sign[i] = -1;
            ++negative;
        }
        else
        {
            dist[i] = (Real)0;
            sign[i] = 0;
            ++zero;
        }
    }
}
//----------------------------------------------------------------------------
template <typename Real>
void IntrLine2Triangle2<Real>::GetInterval (const Vector2<Real>& origin,
    const Vector2<Real>& direction, const Triangle2<Real>& triangle,
    const Real dist[3], const int sign[3], Real param[2])
{
    // Project triangle onto line.
    Real proj[3];
    int i;
    for (i = 0; i < 3; ++i)
    {
        Vector2<Real> diff = triangle.V[i] - origin;
        proj[i] = direction.Dot(diff);
    }

    // Compute transverse intersections of triangle edges with line.
    Real numer, denom;
    int i0, i1, i2;
    int quantity = 0;
    for (i0 = 2, i1 = 0; i1 < 3; i0 = i1++)
    {
        if (sign[i0]*sign[i1] < 0)
        {
            assertion(quantity < 2, "Too many intersections\n");
            numer = dist[i0]*proj[i1] - dist[i1]*proj[i0];
            denom = dist[i0] - dist[i1];
            param[quantity++] = numer/denom;
        }
    }

    // Check for grazing contact.
    if (quantity < 2)
    {
        for (i0 = 1, i1 = 2, i2 = 0; i2 < 3; i0 = i1, i1 = i2++)
        {
            if (sign[i2] == 0)
            {
                assertion(quantity < 2, "Too many intersections\n");
                param[quantity++] = proj[i2];
            }
        }
    }

    // Sort.
    assertion(quantity >= 1, "Need at least one intersection\n");
    if (quantity == 2)
    {
        if (param[0] > param[1])
        {
            Real save = param[0];
            param[0] = param[1];
            param[1] = save;
        }
    }
    else
    {
        param[1] = param[0];
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrLine2Triangle2<float>;

template WM5_MATHEMATICS_ITEM
class IntrLine2Triangle2<double>;
//----------------------------------------------------------------------------
}
