// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5DistRay2Segment2.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
DistRay2Segment2<Real>::DistRay2Segment2 (const Ray2<Real>& ray,
    const Segment2<Real>& segment)
    :
    mRay(&ray),
    mSegment(&segment)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Ray2<Real>& DistRay2Segment2<Real>::GetRay () const
{
    return *mRay;
}
//----------------------------------------------------------------------------
template <typename Real>
const Segment2<Real>& DistRay2Segment2<Real>::GetSegment () const
{
    return *mSegment;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistRay2Segment2<Real>::Get ()
{
    return Math<Real>::Sqrt(GetSquared());
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistRay2Segment2<Real>::GetSquared ()
{
    Vector2<Real> diff = mRay->Origin- mSegment->Center;
    Real a01 = -mRay->Direction.Dot(mSegment->Direction);
    Real b0 = diff.Dot(mRay->Direction);
    Real b1 = -diff.Dot(mSegment->Direction);
    Real c = diff.SquaredLength();
    Real det = Math<Real>::FAbs((Real)1 - a01*a01);
    Real s0, s1, sqrDist, extDet;

    if (det >= Math<Real>::ZERO_TOLERANCE)
    {
        // The ray and segment are not parallel.
        s0 = a01*b1 - b0;
        s1 = a01*b0 - b1;
        extDet = mSegment->Extent*det;

        if (s0 >= (Real)0)
        {
            if (s1 >= -extDet)
            {
                if (s1 <= extDet)  // region 0
                {
                    // Minimum at interior points of ray and segment.
                    Real invDet = ((Real)1)/det;
                    s0 *= invDet;
                    s1 *= invDet;
                    sqrDist = (Real)0;
                }
                else  // region 1
                {
                    s1 = mSegment->Extent;
                    s0 = -(a01*s1 + b0);
                    if (s0 > (Real)0)
                    {
                        sqrDist = -s0*s0 + s1*(s1 + ((Real)2)*b1) + c;
                    }
                    else
                    {
                        s0 = (Real)0;
                        sqrDist = s1*(s1 + ((Real)2)*b1) + c;
                    }
                }
            }
            else  // region 5
            {
                s1 = -mSegment->Extent;
                s0 = -(a01*s1 + b0);
                if (s0 > (Real)0)
                {
                    sqrDist = -s0*s0 + s1*(s1 + ((Real)2)*b1) + c;
                }
                else
                {
                    s0 = (Real)0;
                    sqrDist = s1*(s1 + ((Real)2)*b1) + c;
                }
            }
        }
        else
        {
            if (s1 <= -extDet)  // region 4
            {
                s0 = -(-a01*mSegment->Extent + b0);
                if (s0 > (Real)0)
                {
                    s1 = -mSegment->Extent;
                    sqrDist = -s0*s0 + s1*(s1 + ((Real)2)*b1) + c;
                }
                else
                {
                    s0 = (Real)0;
                    s1 = -b1;
                    if (s1 < -mSegment->Extent)
                    {
                        s1 = -mSegment->Extent;
                    }
                    else if (s1 > mSegment->Extent)
                    {
                        s1 = mSegment->Extent;
                    }
                    sqrDist = s1*(s1 + ((Real)2)*b1) + c;
                }
            }
            else if (s1 <= extDet)  // region 3
            {
                s0 = (Real)0;
                s1 = -b1;
                if (s1 < -mSegment->Extent)
                {
                    s1 = -mSegment->Extent;
                }
                else if (s1 > mSegment->Extent)
                {
                    s1 = mSegment->Extent;
                }
                sqrDist = s1*(s1 + ((Real)2)*b1) + c;
            }
            else  // region 2
            {
                s0 = -(a01*mSegment->Extent + b0);
                if (s0 > (Real)0)
                {
                    s1 = mSegment->Extent;
                    sqrDist = -s0*s0 + s1*(s1 + ((Real)2)*b1) + c;
                }
                else
                {
                    s0 = (Real)0;
                    s1 = -b1;
                    if (s1 < -mSegment->Extent)
                    {
                        s1 = -mSegment->Extent;
                    }
                    else if (s1 > mSegment->Extent)
                    {
                        s1 = mSegment->Extent;
                    }
                    sqrDist = s1*(s1 + ((Real)2)*b1) + c;
                }
            }
        }
    }
    else
    {
        // Ray and segment are parallel.
        if (a01 > (Real)0)
        {
            // Opposite direction vectors.
            s1 = -mSegment->Extent;
        }
        else
        {
            // Same direction vectors.
            s1 = mSegment->Extent;
        }

        s0 = -(a01*s1 + b0);
        if (s0 > (Real)0)
        {
            sqrDist = -s0*s0 + s1*(s1 + ((Real)2)*b1) + c;
        }
        else
        {
            s0 = (Real)0;
            sqrDist = s1*(s1 + ((Real)2)*b1) + c;
        }
    }

    mClosestPoint0 = mRay->Origin + s0*mRay->Direction;
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
Real DistRay2Segment2<Real>::Get (Real t, const Vector2<Real>& velocity0,
    const Vector2<Real>& velocity1)
{
    Vector2<Real> movedOrigin = mRay->Origin + t*velocity0;
    Vector2<Real> movedCenter = mSegment->Center + t*velocity1;
    Ray2<Real> movedRay(movedOrigin, mRay->Direction);
    Segment2<Real> movedSegment(movedCenter, mSegment->Direction,
        mSegment->Extent);
    return DistRay2Segment2<Real>(movedRay, movedSegment).Get();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistRay2Segment2<Real>::GetSquared (Real t,
    const Vector2<Real>& velocity0, const Vector2<Real>& velocity1)
{
    Vector2<Real> movedOrigin = mRay->Origin + t*velocity0;
    Vector2<Real> movedCenter = mSegment->Center + t*velocity1;
    Ray2<Real> movedRay(movedOrigin, mRay->Direction);
    Segment2<Real> movedSegment(movedCenter, mSegment->Direction,
        mSegment->Extent);
    return DistRay2Segment2<Real>(movedRay, movedSegment).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class DistRay2Segment2<float>;

template WM5_MATHEMATICS_ITEM
class DistRay2Segment2<double>;
//----------------------------------------------------------------------------
}
