// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrHalfspace3Sphere3.h"
#include "Wm5IntrUtility3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrHalfspace3Sphere3<Real>::IntrHalfspace3Sphere3 (
    const Plane3<Real>& rkHalfspace, const Sphere3<Real>& rkSphere)
    :
    mHalfspace(&rkHalfspace),
    mSphere(&rkSphere)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Plane3<Real>& IntrHalfspace3Sphere3<Real>::GetHalfspace () const
{
    return *mHalfspace;
}
//----------------------------------------------------------------------------
template <typename Real>
const Sphere3<Real>& IntrHalfspace3Sphere3<Real>::GetSphere () const
{
    return *mSphere;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrHalfspace3Sphere3<Real>::Test ()
{
    Real distance = mHalfspace->Normal.Dot(mSphere->Center);
    return distance <= mHalfspace->Constant + mSphere->Radius;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrHalfspace3Sphere3<Real>::Test (Real tmax,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    mContactTime = (Real)0;
    Real tlast = Math<Real>::MAX_REAL;
    Vector3<Real> relVelocity = velocity1 - velocity0;
    Real distance = mHalfspace->Normal.Dot(mSphere->Center);

    return IntrAxis<Real>::Test(mHalfspace->Normal, relVelocity,
        -Math<Real>::MAX_REAL, mHalfspace->Constant,
        distance - mSphere->Radius, distance + mSphere->Radius, tmax,
        mContactTime, tlast);
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrHalfspace3Sphere3<Real>::Find (Real tmax,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    mContactTime = (Real)0;
    Real tlast = Math<Real>::MAX_REAL;
    Vector3<Real> relVelocity = velocity1 - velocity0;
    Real distance = mHalfspace->Normal.Dot(mSphere->Center);

    if (!IntrAxis<Real>::Test(mHalfspace->Normal, relVelocity,
        -Math<Real>::MAX_REAL, mHalfspace->Constant,
        distance - mSphere->Radius, distance + mSphere->Radius, tmax,
        mContactTime, tlast))
    {
        // Never intersecting.
        return false;
    }

    if (mContactTime == (Real)0)
    {
        // Intersecting now.
        return false;
    }

    mPoint = mSphere->Center + mContactTime*velocity1 -
        mSphere->Radius*mHalfspace->Normal;

    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& IntrHalfspace3Sphere3<Real>::GetPoint () const
{
    return mPoint;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrHalfspace3Sphere3<float>;

template WM5_MATHEMATICS_ITEM
class IntrHalfspace3Sphere3<double>;
//----------------------------------------------------------------------------
}
