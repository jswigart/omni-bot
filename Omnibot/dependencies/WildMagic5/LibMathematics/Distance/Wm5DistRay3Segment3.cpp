// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5DistRay3Segment3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
DistRay3Segment3<Real>::DistRay3Segment3 (const Ray3<Real>& ray,
    const Segment3<Real>& segment)
    :
    mRay(&ray),
    mSegment(&segment)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Ray3<Real>& DistRay3Segment3<Real>::GetRay () const
{
    return *mRay;
}
//----------------------------------------------------------------------------
template <typename Real>
const Segment3<Real>& DistRay3Segment3<Real>::GetSegment () const
{
    return *mSegment;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistRay3Segment3<Real>::Get ()
{
    return Math<Real>::Sqrt(GetSquared());
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistRay3Segment3<Real>::GetSquared ()
{
    Vector3<Real> diff = mRay->Origin- mSegment->Center;
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
                    sqrDist = s0*(s0 + a01*s1 + ((Real)2)*b0) +
                        s1*(a01*s0 + s1 + ((Real)2)*b1) + c;
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
    mRayParameter = s0;
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
Real DistRay3Segment3<Real>::Get (Real t, const Vector3<Real>& velocity0,
    const Vector3<Real>& velocity1)
{
    Vector3<Real> movedOrigin = mRay->Origin + t*velocity0;
    Vector3<Real> movedCenter = mSegment->Center + t*velocity1;
    Ray3<Real> movedRay(movedOrigin, mRay->Direction);
    Segment3<Real> movedSegment(movedCenter, mSegment->Direction,
        mSegment->Extent);
    return DistRay3Segment3<Real>(movedRay, movedSegment).Get();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistRay3Segment3<Real>::GetSquared (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedOrigin = mRay->Origin + t*velocity0;
    Vector3<Real> movedCenter = mSegment->Center + t*velocity1;
    Ray3<Real> movedRay(movedOrigin, mRay->Direction);
    Segment3<Real> movedSegment(movedCenter, mSegment->Direction,
        mSegment->Extent);
    return DistRay3Segment3<Real>(movedRay, movedSegment).GetSquared();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistRay3Segment3<Real>::GetRayParameter () const
{
    return mRayParameter;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistRay3Segment3<Real>::GetSegmentParameter () const
{
    return mSegmentParameter;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class DistRay3Segment3<float>;

template WM5_MATHEMATICS_ITEM
class DistRay3Segment3<double>;
//----------------------------------------------------------------------------
}
