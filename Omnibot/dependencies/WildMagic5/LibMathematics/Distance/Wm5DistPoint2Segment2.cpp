// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5DistPoint2Segment2.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
DistPoint2Segment2<Real>::DistPoint2Segment2 (const Vector2<Real>& point,
    const Segment2<Real>& segment)
    :
    mPoint(&point),
    mSegment(&segment)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector2<Real>& DistPoint2Segment2<Real>::GetPoint () const
{
    return *mPoint;
}
//----------------------------------------------------------------------------
template <typename Real>
const Segment2<Real>& DistPoint2Segment2<Real>::GetSegment () const
{
    return *mSegment;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint2Segment2<Real>::Get ()
{
    return Math<Real>::Sqrt(GetSquared());
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint2Segment2<Real>::GetSquared ()
{
    Vector2<Real> diff = *mPoint - mSegment->Center;
    Real param = mSegment->Direction.Dot(diff);

    if (-mSegment->Extent < param)
    {
        if (param < mSegment->Extent)
        {
            mClosestPoint1 = mSegment->Center + param*mSegment->Direction;
        }
        else
        {
            mClosestPoint1 = mSegment->P1;
        }
    }
    else
    {
        mClosestPoint1 = mSegment->P0;
    }

    mClosestPoint0 = *mPoint;
    diff = mClosestPoint1 - mClosestPoint0;
    return diff.SquaredLength();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint2Segment2<Real>::Get (Real t,
    const Vector2<Real>& velocity0, const Vector2<Real>& velocity1)
{
    Vector2<Real> movedPoint = *mPoint + t*velocity0;
    Vector2<Real> movedCenter = mSegment->Center + t*velocity1;
    Segment2<Real> movedSegment(movedCenter, mSegment->Direction,
        mSegment->Extent);
    return DistPoint2Segment2<Real>(movedPoint, movedSegment).Get();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint2Segment2<Real>::GetSquared (Real t,
    const Vector2<Real>& velocity0, const Vector2<Real>& velocity1)
{
    Vector2<Real> movedPoint = *mPoint + t*velocity0;
    Vector2<Real> movedCenter = mSegment->Center + t*velocity1;
    Segment2<Real> movedSegment(movedCenter, mSegment->Direction,
        mSegment->Extent);
    return DistPoint2Segment2<Real>(movedPoint, movedSegment).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class DistPoint2Segment2<float>;

template WM5_MATHEMATICS_ITEM
class DistPoint2Segment2<double>;
//----------------------------------------------------------------------------
}
