// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrSegment3Sphere3.h"
#include "Wm5IntrSegment3Capsule3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrSegment3Sphere3<Real>::IntrSegment3Sphere3 (
    const Segment3<Real>& segment, const Sphere3<Real>& sphere)
    :
    mSegment(&segment),
    mSphere(&sphere)
{
    mQuantity = 0;
    ZeroThreshold = Math<Real>::ZERO_TOLERANCE;
}
//----------------------------------------------------------------------------
template <typename Real>
const Segment3<Real>& IntrSegment3Sphere3<Real>::GetSegment () const
{
    return *mSegment;
}
//----------------------------------------------------------------------------
template <typename Real>
const Sphere3<Real>& IntrSegment3Sphere3<Real>::GetSphere () const
{
    return *mSphere;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrSegment3Sphere3<Real>::Test ()
{
    Vector3<Real> diff = mSegment->Center - mSphere->Center;
    Real a0 = diff.Dot(diff) - mSphere->Radius*mSphere->Radius;
    Real a1 = mSegment->Direction.Dot(diff);
    Real discr = a1*a1 - a0;
    if (discr < (Real)0)
    {
        return false;
    }

    Real tmp0 = mSegment->Extent*mSegment->Extent + a0;
    Real tmp1 = ((Real)2)*a1*mSegment->Extent;
    Real qm = tmp0 - tmp1;
    Real qp = tmp0 + tmp1;
    if (qm*qp <= (Real)0)
    {
        return true;
    }

    return qm > (Real)0 && Math<Real>::FAbs(a1) < mSegment->Extent;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrSegment3Sphere3<Real>::Find ()
{
    Vector3<Real> diff = mSegment->Center - mSphere->Center;
    Real a0 = diff.Dot(diff) - mSphere->Radius*mSphere->Radius;
    Real a1 = mSegment->Direction.Dot(diff);
    Real discr = a1*a1 - a0;
    if (discr < (Real)0)
    {
        mQuantity = 0;
        return false;
    }

    Real tmp0 = mSegment->Extent*mSegment->Extent + a0;
    Real tmp1 = ((Real)2)*a1*mSegment->Extent;
    Real qm = tmp0 - tmp1;
    Real qp = tmp0 + tmp1;
    Real root;
    if (qm*qp <= (Real)0)
    {
        root = Math<Real>::Sqrt(discr);
        mSegmentParameter[0] = (qm > (Real)0 ? -a1 - root : -a1 + root);
        mPoint[0] = mSegment->Center + mSegmentParameter[0] *
            mSegment->Direction;
        mQuantity = 1;
        mIntersectionType = IT_POINT;
        return true;
    }

    if (qm > (Real)0 && Math<Real>::FAbs(a1) < mSegment->Extent)
    {
        if (discr >= ZeroThreshold)
        {
            root = Math<Real>::Sqrt(discr);
            mSegmentParameter[0] = -a1 - root;
            mSegmentParameter[1] = -a1 + root;
            mPoint[0] = mSegment->Center + mSegmentParameter[0] *
                mSegment->Direction;
            mPoint[1] = mSegment->Center + mSegmentParameter[1] *
                mSegment->Direction;
            mQuantity = 2;
            mIntersectionType = IT_SEGMENT;
        }
        else
        {
            mSegmentParameter[0] = -a1;
            mPoint[0] = mSegment->Center + mSegmentParameter[0] *
                mSegment->Direction;
            mQuantity = 1;
            mIntersectionType = IT_POINT;
        }
    }
    else
    {
        mQuantity = 0;
        mIntersectionType = IT_EMPTY;
    }

    return mQuantity > 0;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrSegment3Sphere3<Real>::Test (Real tmax,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    // Check if initially intersecting.
    if (Test())
    {
        return true;
    }

    // Substract the segment velocity from the sphere velocity so that
    // the calculations are based in the coordinate system of the segment.
    // In this system, the line is of course stationary.  The sphere spans
    // a capsule, but instead we will "grow" the segment by the sphere radius
    // and shrink the sphere to its center.  The problem is now to detect
    // the first time the moving center intersects the capsule formed by
    // the line segment and sphere radius.

    Capsule3<Real> capsule;
    capsule.Segment = *mSegment;
    capsule.Radius = mSphere->Radius;

    Vector3<Real> relVelocity = velocity1 - velocity0;
    Real relSpeed = relVelocity.Normalize();
    Segment3<Real> path;
    path.Extent = ((Real)0.5)*tmax*relSpeed;
    path.Direction = relVelocity;  // unit-length vector
    path.Center = mSphere->Center + path.Extent*path.Direction;

    return IntrSegment3Capsule3<Real>(path, capsule).Test();
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrSegment3Sphere3<Real>::Find (Real tmax,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    // Check if initially intersecting.
    if (Find())
    {
        mContactTime = (Real)0;
        mIntersectionType = IT_OTHER;
        return true;
    }

    // Substract the segment velocity from the sphere velocity so that
    // the calculations are based in the coordinate system of the segment.
    // In this system, the line is of course stationary.  The sphere spans
    // a capsule, but instead we will "grow" the segment by the sphere radius
    // and shrink the sphere to its center.  The problem is now to detect
    // the first time the moving center intersects the capsule formed by
    // the line segment and sphere radius.

    Capsule3<Real> capsule;
    capsule.Segment = *mSegment;
    capsule.Radius = mSphere->Radius;

    Vector3<Real> relVelocity = velocity1 - velocity0;
    Real relSpeed = relVelocity.Normalize();
    Segment3<Real> path;
    path.Extent = ((Real)0.5)*tmax*relSpeed;
    path.Direction = relVelocity;  // unit-length vector
    path.Center = mSphere->Center + path.Extent*path.Direction;

    IntrSegment3Capsule3<Real> intr(path, capsule);
    if (!intr.Find())
    {
        mIntersectionType = IT_EMPTY;
        return false;
    }

    // We now know the sphere will intersect the segment.  This can happen
    // either at a segment end point or at a segment interior point.  We
    // need to determine which.
    mContactTime = (intr.GetParameter(0) + path.Extent)/relSpeed;
    mQuantity = 1;
    mIntersectionType = IT_POINT;

    Vector3<Real> MCenter = mSphere->Center + mContactTime*velocity1;
    Vector3<Real> MOrigin = mSegment->Center + mContactTime*velocity0;
    Real origin = mSegment->Direction.Dot(MOrigin);
    Real negEnd = origin - mSegment->Extent;
    Real posEnd = origin + mSegment->Extent;
    Real center = mSegment->Direction.Dot(MCenter);

    if (center < negEnd)
    {
        // Intersection at segment end point P-e*D.
        mPoint[0] = MOrigin - mSegment->Extent*mSegment->Direction;
    }
    else if (center > posEnd)
    {
        // Intersection at segment end point P+e*D.
        mPoint[0] = MOrigin + mSegment->Extent*mSegment->Direction;
    }
    else
    {
        // Intersection with interior point on edge.  Use the projection
        // along direction axis to find which point that is.
        mPoint[0] = MOrigin + (center - origin)*mSegment->Direction;
    }

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrSegment3Sphere3<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& IntrSegment3Sphere3<Real>::GetPoint (int i) const
{
    return mPoint[i];
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntrSegment3Sphere3<Real>::GetSegmentParameter (int i) const
{
    return mSegmentParameter[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrSegment3Sphere3<float>;

template WM5_MATHEMATICS_ITEM
class IntrSegment3Sphere3<double>;
//----------------------------------------------------------------------------
}
