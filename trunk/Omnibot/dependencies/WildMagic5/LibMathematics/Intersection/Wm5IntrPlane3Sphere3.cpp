// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrPlane3Sphere3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrPlane3Sphere3<Real>::IntrPlane3Sphere3 (const Plane3<Real>& plane,
    const Sphere3<Real>& sphere)
    :
    mPlane(&plane),
    mSphere(&sphere)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Plane3<Real>& IntrPlane3Sphere3<Real>::GetPlane () const
{
    return *mPlane;
}
//----------------------------------------------------------------------------
template <typename Real>
const Sphere3<Real>& IntrPlane3Sphere3<Real>::GetSphere () const
{
    return *mSphere;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrPlane3Sphere3<Real>::Test ()
{
    Real signedDistance = mPlane->DistanceTo(mSphere->Center);
    return Math<Real>::FAbs(signedDistance) <= mSphere->Radius;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrPlane3Sphere3<Real>::Find ()
{
    Real signedDistance = mPlane->DistanceTo(mSphere->Center);
    Real distance = Math<Real>::FAbs(signedDistance);
    mCircle.Center = mSphere->Center - signedDistance*mPlane->Normal;
    mCircle.Normal = mPlane->Normal;
    if (distance <= mSphere->Radius)
    {
        // The sphere intersects the plane in a circle.  The circle is
        // degenerate when distance is equal to mSphere->Radius, in which
        // case the circle radius is zero.
        mCircle.Radius = Math<Real>::Sqrt(Math<Real>::FAbs(
            mSphere->Radius*mSphere->Radius - distance*distance));
        return true;
    }

    // Additional indication that the circle is invalid.
    mCircle.Radius = (Real)-1;
    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrPlane3Sphere3<Real>::SphereIsCulled () const
{
    Real signedDistance = mPlane->DistanceTo(mSphere->Center);
    return signedDistance <= -mSphere->Radius;
}
//----------------------------------------------------------------------------
template <typename Real>
const Circle3<Real>& IntrPlane3Sphere3<Real>::GetCircle () const
{
    return mCircle;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrPlane3Sphere3<float>;

template WM5_MATHEMATICS_ITEM
class IntrPlane3Sphere3<double>;
//----------------------------------------------------------------------------
}
