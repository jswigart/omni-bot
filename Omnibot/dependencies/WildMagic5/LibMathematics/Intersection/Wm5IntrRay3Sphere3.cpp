// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrRay3Sphere3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrRay3Sphere3<Real>::IntrRay3Sphere3 (const Ray3<Real>& ray,
    const Sphere3<Real>& sphere)
    :
    mRay(&ray),
    mSphere(&sphere)
{
    mQuantity = 0;
}
//----------------------------------------------------------------------------
template <typename Real>
const Ray3<Real>& IntrRay3Sphere3<Real>::GetRay () const
{
    return *mRay;
}
//----------------------------------------------------------------------------
template <typename Real>
const Sphere3<Real>& IntrRay3Sphere3<Real>::GetSphere () const
{
    return *mSphere;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrRay3Sphere3<Real>::Test ()
{
    Vector3<Real> diff = mRay->Origin - mSphere->Center;
    Real a0 = diff.Dot(diff) - mSphere->Radius*mSphere->Radius;
    if (a0 <= (Real)0)
    {
        // P is inside the sphere
        return true;
    }
    // else: P is outside the sphere

    Real a1 = mRay->Direction.Dot(diff);
    if (a1 >= (Real)0)
    {
        return false;
    }

    // Quadratic has a real root if discriminant is nonnegative.
    return a1*a1 >= a0;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrRay3Sphere3<Real>::Find ()
{
    Vector3<Real> diff = mRay->Origin - mSphere->Center;
    Real a0 = diff.Dot(diff) - mSphere->Radius*mSphere->Radius;
    Real a1, discr, root;
    if (a0 <= (Real)0)
    {
        // P is inside the sphere
        a1 = mRay->Direction.Dot(diff);
        discr = a1*a1 - a0;
        root = Math<Real>::Sqrt(discr);
        mRayParameter[0] = -a1 + root;
        mPoint[0] = mRay->Origin + mRayParameter[0]*mRay->Direction;
        mQuantity = 1;
        mIntersectionType = IT_POINT;
        return true;
    }
    // else: P is outside the sphere

    a1 = mRay->Direction.Dot(diff);
    if (a1 >= (Real)0.0)
    {
        mQuantity = 0;
        mIntersectionType = IT_EMPTY;
        return false;
    }

    discr = a1*a1 - a0;
    if (discr < (Real)0.0)
    {
        mQuantity = 0;
        mIntersectionType = IT_EMPTY;
    }
    else if (discr >= Math<Real>::ZERO_TOLERANCE)
    {
        root = Math<Real>::Sqrt(discr);
        mRayParameter[0] = -a1 - root;
        mRayParameter[1] = -a1 + root;
        mPoint[0] = mRay->Origin + mRayParameter[0]*mRay->Direction;
        mPoint[1] = mRay->Origin + mRayParameter[1]*mRay->Direction;
        mQuantity = 2;
        mIntersectionType = IT_SEGMENT;
    }
    else
    {
        mRayParameter[0] = -a1;
        mPoint[0] = mRay->Origin + mRayParameter[0]*mRay->Direction;
        mQuantity = 1;
        mIntersectionType = IT_POINT;
    }

    return mQuantity > 0;
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrRay3Sphere3<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& IntrRay3Sphere3<Real>::GetPoint (int i) const
{
    return mPoint[i];
}
//----------------------------------------------------------------------------
template <typename Real>
Real IntrRay3Sphere3<Real>::GetRayParameter (int i) const
{
    return mRayParameter[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrRay3Sphere3<float>;

template WM5_MATHEMATICS_ITEM
class IntrRay3Sphere3<double>;
//----------------------------------------------------------------------------
}
