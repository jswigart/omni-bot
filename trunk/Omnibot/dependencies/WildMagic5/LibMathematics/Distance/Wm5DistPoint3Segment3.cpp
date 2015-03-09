// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5DistPoint3Segment3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
DistPoint3Segment3<Real>::DistPoint3Segment3 (const Vector3<Real>& point,
    const Segment3<Real>& segment)
    :
    mPoint(&point),
    mSegment(&segment)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& DistPoint3Segment3<Real>::GetPoint () const
{
    return *mPoint;
}
//----------------------------------------------------------------------------
template <typename Real>
const Segment3<Real>& DistPoint3Segment3<Real>::GetSegment () const
{
    return *mSegment;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Segment3<Real>::Get ()
{
    return Math<Real>::Sqrt(GetSquared());
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Segment3<Real>::GetSquared ()
{
    Vector3<Real> diff = *mPoint - mSegment->Center;
    mSegmentParameter = mSegment->Direction.Dot(diff);

    if (-mSegment->Extent < mSegmentParameter)
    {
        if (mSegmentParameter < mSegment->Extent)
        {
            mClosestPoint1 = mSegment->Center +
                mSegmentParameter*mSegment->Direction;
        }
        else
        {
            mClosestPoint1 = mSegment->P1;
            mSegmentParameter = mSegment->Extent;
        }
    }
    else
    {
        mClosestPoint1 = mSegment->P0;
        mSegmentParameter = -mSegment->Extent;
    }

    mClosestPoint0 = *mPoint;
    diff = mClosestPoint1 - mClosestPoint0;
    return diff.SquaredLength();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Segment3<Real>::Get (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedPoint = *mPoint + t*velocity0;
    Vector3<Real> movedCenter = mSegment->Center + t*velocity1;
    Segment3<Real> movedSegment(movedCenter, mSegment->Direction,
        mSegment->Extent);
    return DistPoint3Segment3<Real>(movedPoint, movedSegment).Get();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Segment3<Real>::GetSquared (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedPoint = *mPoint + t*velocity0;
    Vector3<Real> movedCenter = mSegment->Center + t*velocity1;
    Segment3<Real> movedSegment(movedCenter, mSegment->Direction,
        mSegment->Extent);
    return DistPoint3Segment3<Real>(movedPoint, movedSegment).GetSquared();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Segment3<Real>::GetSegmentParameter () const
{
    return mSegmentParameter;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class DistPoint3Segment3<float>;

template WM5_MATHEMATICS_ITEM
class DistPoint3Segment3<double>;
//----------------------------------------------------------------------------
}
