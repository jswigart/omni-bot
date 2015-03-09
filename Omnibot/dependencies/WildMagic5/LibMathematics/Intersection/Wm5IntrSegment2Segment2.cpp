// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrSegment2Segment2.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrSegment2Segment2<Real>::IntrSegment2Segment2 (
    const Segment2<Real>& segment0, const Segment2<Real>& segment1)
    :
    mSegment0(&segment0),
    mSegment1(&segment1)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Segment2<Real>& IntrSegment2Segment2<Real>::GetSegment0 () const
{
    return *mSegment0;
}
//----------------------------------------------------------------------------
template <typename Real>
const Segment2<Real>& IntrSegment2Segment2<Real>::GetSegment1 () const
{
    return *mSegment1;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrSegment2Segment2<Real>::Test ()
{
    Vector2<Real> diff;
    Real parameter[2];
    mIntersectionType = Classify(parameter, &diff ,0);

    if (mIntersectionType == IT_POINT)
    {
        // Test whether the line-line intersection is on the segments.
        if (Math<Real>::FAbs(parameter[0]) <= mSegment0->Extent
        &&  Math<Real>::FAbs(parameter[1]) <= mSegment1->Extent)
        {
            mQuantity = 1;
        }
        else
        {
            mQuantity = 0;
            mIntersectionType = IT_EMPTY;
        }
    }
    else if (mIntersectionType == IT_SEGMENT)
    {
        mQuantity = INT_MAX;
    }
    else
    {
        mQuantity = 0;
    }

    return mIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrSegment2Segment2<Real>::Find ()
{
    Vector2<Real> diff;
    Real parameter[2];
    mIntersectionType = Classify(parameter, &diff, 0);

    if (mIntersectionType == IT_POINT)
    {
        // Test whether the line-line intersection is on the segments.
        if (Math<Real>::FAbs(parameter[0]) <= mSegment0->Extent
        &&  Math<Real>::FAbs(parameter[1]) <= mSegment1->Extent)
        {
            mQuantity = 1;
            mPoint = mSegment0->Center + parameter[0]*mSegment0->Direction;
        }
        else
        {
            mQuantity = 0;
            mIntersectionType = IT_EMPTY;
        }
    }
    else if (mIntersectionType == IT_SEGMENT)
    {
        mQuantity = INT_MAX;
    }
    else
    {
        mQuantity = 0;
    }

    return mIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrSegment2Segment2<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector2<Real>& IntrSegment2Segment2<Real>::GetPoint () const
{
    return mPoint;
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrSegment2Segment2<Real>::Classify (Real* s, Vector2<Real>* diff,
    Vector2<Real>* diffN)
{
    // The intersection of two lines is a solution to P0+s0*D0 = P1+s1*D1.
    // Rewrite this as s0*D0 - s1*D1 = P1 - P0 = Q.  If D0.Dot(Perp(D1)) = 0,
    // the lines are parallel.  Additionally, if Q.Dot(Perp(D1)) = 0, the
    // lines are the same.  If D0.Dot(Perp(D1)) is not zero, then
    //   s0 = Q.Dot(Perp(D1))/D0.Dot(Perp(D1))
    // produces the point of intersection.  Also,
    //   s1 = Q.Dot(Perp(D0))/D0.Dot(Perp(D1))

    Vector2<Real> originDiff = mSegment1->Center - mSegment0->Center;
    if (diff)
    {
        *diff = originDiff;
    }

    Real D0DotPerpD1 = mSegment0->Direction.DotPerp(mSegment1->Direction);
    if (Math<Real>::FAbs(D0DotPerpD1) > Math<Real>::ZERO_TOLERANCE)
    {
        // Lines intersect in a single point.
        if (s)
        {
            Real invD0DotPerpD1 = ((Real)1)/D0DotPerpD1;
            Real diffDotPerpD0 = originDiff.DotPerp(mSegment0->Direction);
            Real diffDotPerpD1 = originDiff.DotPerp(mSegment1->Direction);
            s[0] = diffDotPerpD1*invD0DotPerpD1;
            s[1] = diffDotPerpD0*invD0DotPerpD1;
        }
        return IT_POINT;
    }

    // Lines are parallel.
    originDiff.Normalize();
    if (diffN)
    {
        *diffN = originDiff;
    }

    Real diffNDotPerpD1 = originDiff.DotPerp(mSegment1->Direction);
    if (Math<Real>::FAbs(diffNDotPerpD1) <= Math<Real>::ZERO_TOLERANCE)
    {
        // Lines are colinear.
        return IT_SEGMENT;
    }

    // Lines are parallel, but distinct.
    return IT_EMPTY;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrSegment2Segment2<float>;

template WM5_MATHEMATICS_ITEM
class IntrSegment2Segment2<double>;
//----------------------------------------------------------------------------
}
