// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5DistPoint2Ray2.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
DistPoint2Ray2<Real>::DistPoint2Ray2 (const Vector2<Real>& point,
    const Ray2<Real>& ray)
    :
    mPoint(&point),
    mRay(&ray)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector2<Real>& DistPoint2Ray2<Real>::GetPoint () const
{
    return *mPoint;
}
//----------------------------------------------------------------------------
template <typename Real>
const Ray2<Real>& DistPoint2Ray2<Real>::GetRay () const
{
    return *mRay;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint2Ray2<Real>::Get ()
{
    return Math<Real>::Sqrt(GetSquared());
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint2Ray2<Real>::GetSquared ()
{
    Vector2<Real> diff = *mPoint - mRay->Origin;
    Real param = mRay->Direction.Dot(diff);
    if (param > (Real)0)
    {
        mClosestPoint1 = mRay->Origin + param*mRay->Direction;
    }
    else
    {
        mClosestPoint1 = mRay->Origin;
    }

    mClosestPoint0 = *mPoint;
    diff = mClosestPoint1 - mClosestPoint0;
    return diff.SquaredLength();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint2Ray2<Real>::Get (Real t, const Vector2<Real>& velocity0,
    const Vector2<Real>& velocity1)
{
    Vector2<Real> movedPoint = *mPoint + t*velocity0;
    Vector2<Real> movedOrigin = mRay->Origin + t*velocity1;
    Ray2<Real> movedRay(movedOrigin, mRay->Direction);
    return DistPoint2Ray2<Real>(movedPoint, movedRay).Get();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint2Ray2<Real>::GetSquared (Real t,
    const Vector2<Real>& velocity0, const Vector2<Real>& velocity1)
{
    Vector2<Real> movedPoint = *mPoint + t*velocity0;
    Vector2<Real> movedOrigin = mRay->Origin + t*velocity1;
    Ray2<Real> movedRay(movedOrigin, mRay->Direction);
    return DistPoint2Ray2<Real>(movedPoint, movedRay).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class DistPoint2Ray2<float>;

template WM5_MATHEMATICS_ITEM
class DistPoint2Ray2<double>;
//----------------------------------------------------------------------------
}
