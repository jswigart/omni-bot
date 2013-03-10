// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5DistPoint3Rectangle3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
DistPoint3Rectangle3<Real>::DistPoint3Rectangle3 (const Vector3<Real>& point,
    const Rectangle3<Real>& rectangle)
    :
    mPoint(&point),
    mRectangle(&rectangle)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& DistPoint3Rectangle3<Real>::GetPoint () const
{
    return *mPoint;
}
//----------------------------------------------------------------------------
template <typename Real>
const Rectangle3<Real>& DistPoint3Rectangle3<Real>::GetRectangle () const
{
    return *mRectangle;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Rectangle3<Real>::Get ()
{
    return Math<Real>::Sqrt(GetSquared());
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Rectangle3<Real>::GetSquared ()
{
    Vector3<Real> diff = mRectangle->Center - *mPoint;
    Real b0 = diff.Dot(mRectangle->Axis[0]);
    Real b1 = diff.Dot(mRectangle->Axis[1]);
    Real s0 = -b0, s1 = -b1;
    Real sqrDistance = diff.SquaredLength();

    if (s0 < -mRectangle->Extent[0])
    {
        s0 = -mRectangle->Extent[0];
    }
    else if (s0 > mRectangle->Extent[0])
    {
        s0 = mRectangle->Extent[0];
    }
    sqrDistance += s0*(s0 + ((Real)2)*b0);

    if (s1 < -mRectangle->Extent[1])
    {
        s1 = -mRectangle->Extent[1];
    }
    else if (s1 > mRectangle->Extent[1])
    {
        s1 = mRectangle->Extent[1];
    }
    sqrDistance += s1*(s1 + ((Real)2)*b1);

    // Account for numerical round-off error.
    if (sqrDistance < (Real)0)
    {
        sqrDistance = (Real)0;
    }

    mClosestPoint0 = *mPoint;
    mClosestPoint1 = mRectangle->Center + s0*mRectangle->Axis[0] +
        s1*mRectangle->Axis[1];
    mRectCoord[0] = s0;
    mRectCoord[1] = s1;

    return sqrDistance;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Rectangle3<Real>::Get (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedPoint = *mPoint + t*velocity0;
    Vector3<Real> movedCenter = mRectangle->Center + t*velocity1;
    Rectangle3<Real> movedRectangle(movedCenter, mRectangle->Axis[0],
        mRectangle->Axis[1], mRectangle->Extent[0], mRectangle->Extent[1]);
    return DistPoint3Rectangle3<Real>(movedPoint,movedRectangle).Get();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Rectangle3<Real>::GetSquared (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedPoint = *mPoint + t*velocity0;
    Vector3<Real> movedCenter = mRectangle->Center + t*velocity1;
    Rectangle3<Real> movedRectangle(movedCenter, mRectangle->Axis[0],
        mRectangle->Axis[1], mRectangle->Extent[0], mRectangle->Extent[1]);
    return DistPoint3Rectangle3<Real>(movedPoint,movedRectangle).GetSquared();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Rectangle3<Real>::GetRectangleCoordinate (int i) const
{
    return mRectCoord[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class DistPoint3Rectangle3<float>;

template WM5_MATHEMATICS_ITEM
class DistPoint3Rectangle3<double>;
//----------------------------------------------------------------------------
}
