// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrSegment3Plane3.h"
#include "Wm5IntrLine3Plane3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrSegment3Plane3<Real>::IntrSegment3Plane3 (const Segment3<Real>& rkSegment,
    const Plane3<Real>& rkPlane)
    :
    mSegment(&rkSegment),
    mPlane(&rkPlane)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Segment3<Real>& IntrSegment3Plane3<Real>::GetSegment () const
{
    return *mSegment;
}
//----------------------------------------------------------------------------
template <typename Real>
const Plane3<Real>& IntrSegment3Plane3<Real>::GetPlane () const
{
    return *mPlane;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrSegment3Plane3<Real>::Test ()
{
    Vector3<Real> P0 = mSegment->P0;
    Real sdistance0 = mPlane->DistanceTo(P0);
    if (Math<Real>::FAbs(sdistance0) <= Math<Real>::ZERO_TOLERANCE)
    {
        sdistance0 = (Real)0;
    }

    Vector3<Real> P1 = mSegment->P1;
    Real sdistance1 = mPlane->DistanceTo(P1);
    if (Math<Real>::FAbs(sdistance1) <= Math<Real>::ZERO_TOLERANCE)
    {
        sdistance1 = (Real)0;
    }

    Real prod = sdistance0*sdistance1;
    if (prod < (Real)0)
    {
        // The segment passes through the plane.
        mIntersectionType = IT_POINT;
        return true;
    }

    if (prod > (Real)0)
    {
        // The segment is on one side of the plane.
        mIntersectionType = IT_EMPTY;
        return false;
    }

    if (sdistance0 != (Real)0 || sdistance1 != (Real)0)
    {
        // A segment end point touches the plane.
        mIntersectionType = IT_POINT;
        return true;
    }

    // The segment is coincident with the plane.
    mIntersectionType = IT_SEGMENT;
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrSegment3Plane3<Real>::Find ()
{
    Line3<Real> line(mSegment->Center, mSegment->Direction);
    IntrLine3Plane3<Real> intr(line, *mPlane);
    if (intr.Find())
    {
        // The line intersects the plane, but possibly at a point that is
        // not on the segment.
        mIntersectionType = intr.GetIntersectionType();
        mSegmentParameter = intr.GetLineParameter();
        return Math<Real>::FAbs(mSegmentParameter) <= mSegment->Extent;
    }

    mIntersectionType = IT_EMPTY;
    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntrSegment3Plane3<Real>::GetSegmentParameter () const
{
    return mSegmentParameter;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrSegment3Plane3<float>;

template WM5_MATHEMATICS_ITEM
class IntrSegment3Plane3<double>;
//----------------------------------------------------------------------------
}
