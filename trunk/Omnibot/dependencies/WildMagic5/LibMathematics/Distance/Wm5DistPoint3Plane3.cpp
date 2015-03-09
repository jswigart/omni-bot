// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5DistPoint3Plane3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
DistPoint3Plane3<Real>::DistPoint3Plane3 (const Vector3<Real>& point,
    const Plane3<Real>& plane)
    :
    mPoint(&point),
    mPlane(&plane)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& DistPoint3Plane3<Real>::GetPoint () const
{
    return *mPoint;
}
//----------------------------------------------------------------------------
template <typename Real>
const Plane3<Real>& DistPoint3Plane3<Real>::GetPlane () const
{
    return *mPlane;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Plane3<Real>::Get ()
{
    Real signedDistance = mPlane->Normal.Dot(*mPoint) - mPlane->Constant;
    mClosestPoint0 = *mPoint;
    mClosestPoint1 = *mPoint - signedDistance*mPlane->Normal;
    return Math<Real>::FAbs(signedDistance);
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Plane3<Real>::GetSquared ()
{
    Real signedDistance = mPlane->Normal.Dot(*mPoint) - mPlane->Constant;
    mClosestPoint0 = *mPoint;
    mClosestPoint1 = *mPoint - signedDistance*mPlane->Normal;
    return signedDistance*signedDistance;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Plane3<Real>::Get (Real t, const Vector3<Real>& velocity0,
    const Vector3<Real>& velocity1)
{
    Vector3<Real> movedPoint = *mPoint + t*velocity0;
    Real movedConstant = mPlane->Constant + t*mPlane->Normal.Dot(velocity1);
    Plane3<Real> movedPlane(mPlane->Normal, movedConstant);
    return DistPoint3Plane3<Real>(movedPoint, movedPlane).Get();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Plane3<Real>::GetSquared (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedPoint = *mPoint + t*velocity0;
    Real movedConstant = mPlane->Constant + t*mPlane->Normal.Dot(velocity1);
    Plane3<Real> movedPlane(mPlane->Normal, movedConstant);
    return DistPoint3Plane3<Real>(movedPoint, movedPlane).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class DistPoint3Plane3<float>;

template WM5_MATHEMATICS_ITEM
class DistPoint3Plane3<double>;
//----------------------------------------------------------------------------
}
