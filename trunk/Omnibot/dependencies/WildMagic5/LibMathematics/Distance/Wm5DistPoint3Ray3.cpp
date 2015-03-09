// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5DistPoint3Ray3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
DistPoint3Ray3<Real>::DistPoint3Ray3 (const Vector3<Real>& point,
    const Ray3<Real>& ray)
    :
    mPoint(&point),
    mRay(&ray)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& DistPoint3Ray3<Real>::GetPoint () const
{
    return *mPoint;
}
//----------------------------------------------------------------------------
template <typename Real>
const Ray3<Real>& DistPoint3Ray3<Real>::GetRay () const
{
    return *mRay;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Ray3<Real>::Get ()
{
    return Math<Real>::Sqrt(GetSquared());
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Ray3<Real>::GetSquared ()
{
    Vector3<Real> diff = *mPoint - mRay->Origin;
    mRayParameter = mRay->Direction.Dot(diff);
    if (mRayParameter > (Real)0)
    {
        mClosestPoint1 = mRay->Origin + mRayParameter*mRay->Direction;
    }
    else
    {
        mClosestPoint1 = mRay->Origin;
        mRayParameter = (Real)0;
    }

    mClosestPoint0 = *mPoint;
    diff = mClosestPoint1 - mClosestPoint0;
    return diff.SquaredLength();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Ray3<Real>::Get (Real t, const Vector3<Real>& velocity0,
    const Vector3<Real>& velocity1)
{
    Vector3<Real> movedPoint = *mPoint + t*velocity0;
    Vector3<Real> movedOrigin = mRay->Origin + t*velocity1;
    Ray3<Real> movedRay(movedOrigin, mRay->Direction);
    return DistPoint3Ray3<Real>(movedPoint, movedRay).Get();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Ray3<Real>::GetSquared (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedPoint = *mPoint + t*velocity0;
    Vector3<Real> movedOrigin = mRay->Origin + t*velocity1;
    Ray3<Real> movedRay(movedOrigin, mRay->Direction);
    return DistPoint3Ray3<Real>(movedPoint, movedRay).GetSquared();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Ray3<Real>::GetRayParameter () const
{
    return mRayParameter;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class DistPoint3Ray3<float>;

template WM5_MATHEMATICS_ITEM
class DistPoint3Ray3<double>;
//----------------------------------------------------------------------------
}
