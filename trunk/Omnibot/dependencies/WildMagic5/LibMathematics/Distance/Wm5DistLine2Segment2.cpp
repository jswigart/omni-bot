// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5DistLine2Segment2.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
DistLine2Segment2<Real>::DistLine2Segment2 (const Line2<Real>& line,
    const Segment2<Real>& segment)
    :
    mLine(&line),
    mSegment(&segment)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Line2<Real>& DistLine2Segment2<Real>::GetLine () const
{
    return *mLine;
}
//----------------------------------------------------------------------------
template <typename Real>
const Segment2<Real>& DistLine2Segment2<Real>::GetSegment () const
{
    return *mSegment;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine2Segment2<Real>::Get ()
{
    return Math<Real>::Sqrt(GetSquared());
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine2Segment2<Real>::GetSquared ()
{
    Vector2<Real> diff = mLine->Origin - mSegment->Center;
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
                sqrDist = (Real)0;
            }
            else
            {
                // The endpoint e1 of the segment and an interior point of
                // the line are closest.
                s1 = mSegment->Extent;
                s0 = -(a01*s1 + b0);
                sqrDist = -s0*s0 +s1*(s1 + ((Real)2)*b1) + c;
            }
        }
        else
        {
            // The endpoint e0 of the segment and an interior point of the
            // line are closest.
            s1 = -mSegment->Extent;
            s0 = -(a01*s1 + b0);
            sqrDist = -s0*s0 +s1*(s1 + ((Real)2)*b1) + c;
        }
    }
    else
    {
        // The line and segment are parallel.  Choose the closest pair so that
        // one point is at segment origin.
        s1 = (Real)0;
        s0 = -b0;
        sqrDist = b0*s0 + c;
    }

    mClosestPoint0 = mLine->Origin + s0*mLine->Direction;
    mClosestPoint1 = mSegment->Center + s1*mSegment->Direction;

    // Account for numerical round-off errors.
    if (sqrDist < (Real)0)
    {
        sqrDist = (Real)0;
    }
    return sqrDist;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine2Segment2<Real>::Get (Real t, const Vector2<Real>& velocity0,
    const Vector2<Real>& velocity1)
{
    Vector2<Real> movedOrigin = mLine->Origin + t*velocity0;
    Vector2<Real> movedCenter = mSegment->Center + t*velocity1;
    Line2<Real> movedLine(movedOrigin, mLine->Direction);
    Segment2<Real> movedSegment(movedCenter, mSegment->Direction,
        mSegment->Extent);
    return DistLine2Segment2<Real>(movedLine, movedSegment).Get();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine2Segment2<Real>::GetSquared (Real t,
    const Vector2<Real>& velocity0, const Vector2<Real>& velocity1)
{
    Vector2<Real> movedOrigin = mLine->Origin + t*velocity0;
    Vector2<Real> movedCenter = mSegment->Center + t*velocity1;
    Line2<Real> movedLine(movedOrigin, mLine->Direction);
    Segment2<Real> movedSegment(movedCenter, mSegment->Direction,
        mSegment->Extent);
    return DistLine2Segment2<Real>(movedLine, movedSegment).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class DistLine2Segment2<float>;

template WM5_MATHEMATICS_ITEM
class DistLine2Segment2<double>;
//----------------------------------------------------------------------------
}
