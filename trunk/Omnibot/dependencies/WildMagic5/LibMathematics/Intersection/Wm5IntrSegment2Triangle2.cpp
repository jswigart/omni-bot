// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrSegment2Triangle2.h"
#include "Wm5Intersector1.h"
#include "Wm5IntrLine2Triangle2.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrSegment2Triangle2<Real>::IntrSegment2Triangle2 (
    const Segment2<Real>& segment, const Triangle2<Real>& triangle)
    :
    mSegment(&segment),
    mTriangle(&triangle)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Segment2<Real>& IntrSegment2Triangle2<Real>::GetSegment () const
{
    return *mSegment;
}
//----------------------------------------------------------------------------
template <typename Real>
const Triangle2<Real>& IntrSegment2Triangle2<Real>::GetTriangle () const
{
    return *mTriangle;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrSegment2Triangle2<Real>::Test ()
{
    Real dist[3];
    int sign[3], positive, negative, zero;
    IntrLine2Triangle2<Real>::TriangleLineRelations(mSegment->Center,
        mSegment->Direction, *mTriangle, dist, sign, positive, negative,
        zero);

    if (positive == 3 || negative == 3)
    {
        mIntersectionType = IT_EMPTY;
    }
    else
    {
        Real param[2];
        IntrLine2Triangle2<Real>::GetInterval(mSegment->Center,
            mSegment->Direction, *mTriangle, dist, sign, param);

        Intersector1<Real> intr(param[0], param[1],
            -mSegment->Extent, +mSegment->Extent);

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
bool IntrSegment2Triangle2<Real>::Find ()
{
    Real dist[3];
    int sign[3], positive, negative, zero;
    IntrLine2Triangle2<Real>::TriangleLineRelations(mSegment->Center,
        mSegment->Direction, *mTriangle, dist, sign, positive, negative,
        zero);

    if (positive == 3 || negative == 3)
    {
        // No intersections.
        mQuantity = 0;
        mIntersectionType = IT_EMPTY;
    }
    else
    {
        Real param[2];
        IntrLine2Triangle2<Real>::GetInterval(mSegment->Center,
            mSegment->Direction, *mTriangle, dist, sign, param);

        Intersector1<Real> intr(param[0], param[1],
            -mSegment->Extent, +mSegment->Extent);

        intr.Find();

        mQuantity = intr.GetNumIntersections();
        if (mQuantity == 2)
        {
            // Segment intersection.
            mIntersectionType = IT_SEGMENT;
            mPoint[0] = mSegment->Center +
                intr.GetIntersection(0)*mSegment->Direction;
            mPoint[1] = mSegment->Center +
                intr.GetIntersection(1)*mSegment->Direction;
        }
        else if (mQuantity == 1)
        {
            // Point intersection.
            mIntersectionType = IT_POINT;
            mPoint[0] = mSegment->Center +
                intr.GetIntersection(0)*mSegment->Direction;
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
int IntrSegment2Triangle2<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector2<Real>& IntrSegment2Triangle2<Real>::GetPoint (int i) const
{
    return mPoint[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrSegment2Triangle2<float>;

template WM5_MATHEMATICS_ITEM
class IntrSegment2Triangle2<double>;
//----------------------------------------------------------------------------
}
