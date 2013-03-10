// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5DistPoint2Box2.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
DistPoint2Box2<Real>::DistPoint2Box2 (const Vector2<Real>& point,
    const Box2<Real>& box)
    :
    mPoint(&point),
    mBox(&box)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector2<Real>& DistPoint2Box2<Real>::GetPoint () const
{
    return *mPoint;
}
//----------------------------------------------------------------------------
template <typename Real>
const Box2<Real>& DistPoint2Box2<Real>::GetBox () const
{
    return *mBox;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint2Box2<Real>::Get ()
{
    return Math<Real>::Sqrt(GetSquared());
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint2Box2<Real>::GetSquared ()
{
    // Work in the box's coordinate system.
    Vector2<Real> diff = *mPoint - mBox->Center;

    // Compute squared distance and closest point on box.
    Real sqrDistance = (Real)0;
    Real delta;
    Vector2<Real> closest;
    int i;
    for (i = 0; i < 2; ++i)
    {
        closest[i] = diff.Dot(mBox->Axis[i]);
        if (closest[i] < -mBox->Extent[i])
        {
            delta = closest[i] + mBox->Extent[i];
            sqrDistance += delta*delta;
            closest[i] = -mBox->Extent[i];
        }
        else if (closest[i] > mBox->Extent[i])
        {
            delta = closest[i] - mBox->Extent[i];
            sqrDistance += delta*delta;
            closest[i] = mBox->Extent[i];
        }
    }

    mClosestPoint0 = *mPoint;
    mClosestPoint1 = mBox->Center;
    for (i = 0; i < 2; ++i)
    {
        mClosestPoint1 += closest[i]*mBox->Axis[i];
    }

    return sqrDistance;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint2Box2<Real>::Get (Real t, const Vector2<Real>& velocity0,
    const Vector2<Real>& velocity1)
{
    Vector2<Real> movedPoint = *mPoint + t*velocity0;
    Vector2<Real> movedCenter = mBox->Center + t*velocity1;
    Box2<Real> movedBox(movedCenter, mBox->Axis, mBox->Extent);
    return DistPoint2Box2<Real>(movedPoint, movedBox).Get();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint2Box2<Real>::GetSquared (Real t,
    const Vector2<Real>& velocity0, const Vector2<Real>& velocity1)
{
    Vector2<Real> movedPoint = *mPoint + t*velocity0;
    Vector2<Real> movedCenter = mBox->Center + t*velocity1;
    Box2<Real> movedBox(movedCenter, mBox->Axis, mBox->Extent);
    return DistPoint2Box2<Real>(movedPoint, movedBox).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class DistPoint2Box2<float>;

template WM5_MATHEMATICS_ITEM
class DistPoint2Box2<double>;
//----------------------------------------------------------------------------
}
