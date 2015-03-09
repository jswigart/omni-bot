// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5DistPoint3Circle3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
DistPoint3Circle3<Real>::DistPoint3Circle3 (const Vector3<Real>& point,
    const Circle3<Real>& circle)
    :
    mPoint(&point),
    mCircle(&circle)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& DistPoint3Circle3<Real>::GetPoint () const
{
    return *mPoint;
}
//----------------------------------------------------------------------------
template <typename Real>
const Circle3<Real>& DistPoint3Circle3<Real>::GetCircle () const
{
    return *mCircle;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Circle3<Real>::Get ()
{
    return Math<Real>::Sqrt(GetSquared());
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Circle3<Real>::GetSquared ()
{
    // Signed distance from point to plane of circle.
    Vector3<Real> diff0 = *mPoint - mCircle->Center;
    Real dist = diff0.Dot(mCircle->Normal);

    // Projection of P-C onto plane is Q-C = P-C - (fDist)*N.
    Vector3<Real> diff1 = diff0 - dist*mCircle->Normal;
    Real sqrLen = diff1.SquaredLength();
    Real sqrDistance;

    if (sqrLen >= Math<Real>::ZERO_TOLERANCE)
    {
        mClosestPoint1 = mCircle->Center + (mCircle->Radius/
            Math<Real>::Sqrt(sqrLen))*diff1;
        Vector3<Real> diff2 = *mPoint - mClosestPoint1;
        sqrDistance = diff2.SquaredLength();
    }
    else
    {
        mClosestPoint1 = Vector3<Real>(Math<Real>::MAX_REAL,
            Math<Real>::MAX_REAL,Math<Real>::MAX_REAL);
        sqrDistance = mCircle->Radius*mCircle->Radius + dist*dist;
    }

    mClosestPoint0 = *mPoint;

    return sqrDistance;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Circle3<Real>::Get (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedPoint = *mPoint + t*velocity0;
    Vector3<Real> movedCenter = mCircle->Center + t*velocity1;
    Circle3<Real> movedCircle(movedCenter, mCircle->Direction0,
        mCircle->Direction1, mCircle->Normal, mCircle->Radius);
    return DistPoint3Circle3<Real>(movedPoint, movedCircle).Get();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Circle3<Real>::GetSquared (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedPoint = *mPoint + t*velocity0;
    Vector3<Real> movedCenter = mCircle->Center + t*velocity1;
    Circle3<Real> movedCircle(movedCenter, mCircle->Direction0,
        mCircle->Direction1, mCircle->Normal, mCircle->Radius);
    return DistPoint3Circle3<Real>(movedPoint, movedCircle).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class DistPoint3Circle3<float>;

template WM5_MATHEMATICS_ITEM
class DistPoint3Circle3<double>;
//----------------------------------------------------------------------------
}
