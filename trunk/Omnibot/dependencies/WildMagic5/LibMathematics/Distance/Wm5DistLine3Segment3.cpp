// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5DistLine3Segment3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
DistLine3Segment3<Real>::DistLine3Segment3 (const Line3<Real>& line,
    const Segment3<Real>& segment)
    :
    mLine(&line),
    mSegment(&segment)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Line3<Real>& DistLine3Segment3<Real>::GetLine () const
{
    return *mLine;
}
//----------------------------------------------------------------------------
template <typename Real>
const Segment3<Real>& DistLine3Segment3<Real>::GetSegment () const
{
    return *mSegment;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Segment3<Real>::Get ()
{
    return Math<Real>::Sqrt(GetSquared());
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Segment3<Real>::GetSquared ()
{
    Vector3<Real> diff = mLine->Origin - mSegment->Center;
    Real a01 = -mLine->Direction.Dot(mSegment->Direction);
    Real b0 = diff.Dot(mLine->Direction);
    Real c = diff.SquaredLength();
    Real det = Math<Real>::FAbs((Real)1 - a01*a01);
    Real b1, s0, s1, sqrDist, extDet;

    if (det >= Math<Real>::ZERO_TOLERANCE)
    {
        // The line and segment are not parallel.
        b1 = -diff.Dot(mSegment->Direction);
        s1 = a01*b0 - b1;
        extDet = mSegment->Extent*det;

        if (s1 >= -extDet)
        {
            if (s1 <= extDet)
            {
                // Two interior points are closest, one on the line and one
                // on the segment.
                Real invDet = ((Real)1)/det;
                s0 = (a01*b1 - b0)*invDet;
                s1 *= invDet;
                sqrDist = s0*(s0 + a01*s1 + ((Real)2)*b0) +
                    s1*(a01*s0 + s1 + ((Real)2)*b1) + c;
            }
            else
            {
                // The endpoint e1 of the segment and an interior point of
                // the line are closest.
                s1 = mSegment->Extent;
                s0 = -(a01*s1 + b0);
                sqrDist = -s0*s0 + s1*(s1 + ((Real)2)*b1) + c;
            }
        }
        else
        {
            // The end point e0 of the segment and an interior point of the
            // line are closest.
            s1 = -mSegment->Extent;
            s0 = -(a01*s1 + b0);
            sqrDist = -s0*s0 + s1*(s1 + ((Real)2)*b1) + c;
        }
    }
    else
    {
        // The line and segment are parallel.  Choose the closest pair so that
        // one point is at segment center.
        s1 = (Real)0;
        s0 = -b0;
        sqrDist = b0*s0 + c;
    }

    mClosestPoint0 = mLine->Origin + s0*mLine->Direction;
    mClosestPoint1 = mSegment->Center + s1*mSegment->Direction;
    mLineParameter = s0;
    mSegmentParameter = s1;

    // Account for numerical round-off errors.
    if (sqrDist < (Real)0)
    {
        sqrDist = (Real)0;
    }
    return sqrDist;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Segment3<Real>::Get (Real t, const Vector3<Real>& velocity0,
    const Vector3<Real>& velocity1)
{
    Vector3<Real> movedOrigin = mLine->Origin + t*velocity0;
    Vector3<Real> movedCenter = mSegment->Center + t*velocity1;
    Line3<Real> movedLine(movedOrigin, mLine->Direction);
    Segment3<Real> movedSegment(movedCenter, mSegment->Direction,
        mSegment->Extent);
    return DistLine3Segment3<Real>(movedLine, movedSegment).Get();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Segment3<Real>::GetSquared (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedOrigin = mLine->Origin + t*velocity0;
    Vector3<Real> movedCenter = mSegment->Center + t*velocity1;
    Line3<Real> movedLine(movedOrigin, mLine->Direction);
    Segment3<Real> movedSegment(movedCenter, mSegment->Direction,
        mSegment->Extent);
    return DistLine3Segment3<Real>(movedLine, movedSegment).GetSquared();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Segment3<Real>::GetLineParameter () const
{
    return mLineParameter;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Segment3<Real>::GetSegmentParameter () const
{
    return mSegmentParameter;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class DistLine3Segment3<float>;

template WM5_MATHEMATICS_ITEM
class DistLine3Segment3<double>;
//----------------------------------------------------------------------------
}
