// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5DistLine3Ray3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
DistLine3Ray3<Real>::DistLine3Ray3 (const Line3<Real>& rkLine,
    const Ray3<Real>& rkRay)
    :
    mLine(&rkLine),
    mRay(&rkRay)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Line3<Real>& DistLine3Ray3<Real>::GetLine () const
{
    return *mLine;
}
//----------------------------------------------------------------------------
template <typename Real>
const Ray3<Real>& DistLine3Ray3<Real>::GetRay () const
{
    return *mRay;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Ray3<Real>::Get ()
{
    return Math<Real>::Sqrt(GetSquared());
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Ray3<Real>::GetSquared ()
{
    Vector3<Real> kDiff = mLine->Origin - mRay->Origin;
    Real a01 = -mLine->Direction.Dot(mRay->Direction);
    Real b0 = kDiff.Dot(mLine->Direction);
    Real c = kDiff.SquaredLength();
    Real det = Math<Real>::FAbs((Real)1 - a01*a01);
    Real b1, s0, s1, sqrDist;

    if (det >= Math<Real>::ZERO_TOLERANCE)
    {
        b1 = -kDiff.Dot(mRay->Direction);
        s1 = a01*b0 - b1;

        if (s1 >= (Real)0)
        {
            // Two interior points are closest, one on line and one on ray.
            Real invDet = ((Real)1)/det;
            s0 = (a01*b1 - b0)*invDet;
            s1 *= invDet;
            sqrDist = s0*(s0 + a01*s1 + ((Real)2)*b0) +
                s1*(a01*s0 + s1 + ((Real)2)*b1) + c;
        }
        else
        {
            // Origin of ray and interior point of line are closest.
            s0 = -b0;
            s1 = (Real)0;
            sqrDist = b0*s0 + c;
        }
    }
    else
    {
        // Lines are parallel, closest pair with one point at ray origin.
        s0 = -b0;
        s1 = (Real)0;
        sqrDist = b0*s0 + c;
    }

    mClosestPoint0 = mLine->Origin + s0*mLine->Direction;
    mClosestPoint1 = mRay->Origin + s1*mRay->Direction;
    mLineParameter = s0;
    mRayParameter = s1;

    // Account for numerical round-off errors.
    if (sqrDist < (Real)0)
    {
        sqrDist = (Real)0;
    }
    return sqrDist;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Ray3<Real>::Get (Real t, const Vector3<Real>& velocity0,
    const Vector3<Real>& velocity1)
{
    Vector3<Real> movedOrigin0 = mLine->Origin + t*velocity0;
    Vector3<Real> movedOrigin1 = mRay->Origin + t*velocity1;
    Line3<Real> movedLine(movedOrigin0, mLine->Direction);
    Ray3<Real> movedRay(movedOrigin1, mRay->Direction);
    return DistLine3Ray3<Real>(movedLine, movedRay).Get();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Ray3<Real>::GetSquared (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedOrigin0 = mLine->Origin + t*velocity0;
    Vector3<Real> movedOrigin1 = mRay->Origin + t*velocity1;
    Line3<Real> movedLine(movedOrigin0, mLine->Direction);
    Ray3<Real> movedRay(movedOrigin1, mRay->Direction);
    return DistLine3Ray3<Real>(movedLine, movedRay).GetSquared();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Ray3<Real>::GetLineParameter () const
{
    return mLineParameter;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Ray3<Real>::GetRayParameter () const
{
    return mRayParameter;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class DistLine3Ray3<float>;

template WM5_MATHEMATICS_ITEM
class DistLine3Ray3<double>;
//----------------------------------------------------------------------------
}
