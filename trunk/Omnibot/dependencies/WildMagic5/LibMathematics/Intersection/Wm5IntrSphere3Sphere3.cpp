// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.3 (2011/10/26)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrSphere3Sphere3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrSphere3Sphere3<Real>::IntrSphere3Sphere3 (const Sphere3<Real>& rkSphere0,
    const Sphere3<Real>& rkSphere1)
    :
    mSphere0(&rkSphere0),
    mSphere1(&rkSphere1)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Sphere3<Real>& IntrSphere3Sphere3<Real>::GetSphere0 () const
{
    return *mSphere0;
}
//----------------------------------------------------------------------------
template <typename Real>
const Sphere3<Real>& IntrSphere3Sphere3<Real>::GetSphere1 () const
{
    return *mSphere1;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrSphere3Sphere3<Real>::Test ()
{
    Vector3<Real> diff = mSphere1->Center - mSphere0->Center;
    Real rSum = mSphere0->Radius + mSphere1->Radius;
    return diff.SquaredLength() <= rSum*rSum;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrSphere3Sphere3<Real>::Find ()
{
    // Plane of intersection must have N as its normal.
    Vector3<Real> C1mC0 = mSphere1->Center - mSphere0->Center;
    Real sqrLen = C1mC0.SquaredLength();
    Real r0 = mSphere0->Radius, r1 = mSphere1->Radius;

    Real rSum = r0 + r1;
    Real rSumSqr = rSum*rSum;
    if (sqrLen > rSumSqr)
    {
        // Spheres are disjoint/separated.
        mIntersectionType = IT_EMPTY;
        return false;
    }
    if (sqrLen == rSumSqr)
    {
        // Spheres are just touching.  The caller must call
        // GetIntersectionType() to determine what type of intersection has
        // occurred.  In this case, GetContactPoint() should be called, not
        // GetCircle().  The circle parameters are set just in case the caller
        // does not test for intersection type.
        C1mC0.Normalize();
        mContactPoint = mSphere0->Center + r0*C1mC0;
        mCircle.Normal = C1mC0;
        mCircle.Center = mContactPoint;
        mCircle.Radius = (Real)0;
        mIntersectionType = IT_POINT;
        return true;
    }

    Real rDif = r0 - r1;
    Real rDifSqr = rDif*rDif;
    if (sqrLen < rDifSqr)
    {
        // One sphere is strictly contained in the other.  The caller must
        // call GetIntersectionType() to determine what type of intersection
        // has occurred.  In this case, neither GetCircle() nor
        // GetContactPoint() should be called.  The circle and contact
        // parameters are set just in case the caller does not test for
        // intersection type, but the choices are arbitrary.
        C1mC0.Normalize();
        mContactPoint = ((Real)0.5)*(mSphere0->Center + mSphere1->Center);
        mCircle.Normal = C1mC0;
        mCircle.Center = mContactPoint;
        mCircle.Radius = (Real)0;
        mIntersectionType = (rDif <= (Real)0 ? IT_SPHERE0 : IT_SPHERE1);
        return true;
    }
    if (sqrLen == rDifSqr)
    {
        // One sphere is contained in the other sphere but with a single point
        // of contact.  The caller must call GetIntersectionType() to
        // determine what type of intersection has occurred.  In this case,
        // GetContactPoint() should be called.  The circle parameters are set
        // just in case the caller does not test for intersection type.
        C1mC0.Normalize();
        mCircle.Radius = (Real)0;
        mCircle.Normal = C1mC0;
        if (rDif <= (Real)0)
        {
            mContactPoint = mSphere1->Center + r1*C1mC0;
            mIntersectionType = IT_SPHERE0_POINT;
        }
        else
        {
            mContactPoint = mSphere0->Center + r0*C1mC0;
            mIntersectionType = IT_SPHERE1_POINT;
        }
        mCircle.Center = mContactPoint;
        return true;
    }

    // Compute t for which the circle of intersection has center
    // K = C0 + t*(C1 - C0).
    Real t = ((Real)0.5)*((Real)1 + rDif*rSum/sqrLen);

    // Center and radius of circle of intersection.
    mCircle.Center = mSphere0->Center + t*C1mC0;
    mCircle.Radius = Math<Real>::Sqrt(Math<Real>::FAbs(r0*r0 - t*t*sqrLen));

    // Compute N, U, and V for plane of circle.
    C1mC0.Normalize();
    mCircle.Normal = C1mC0;
    Vector3<Real>::GenerateComplementBasis(mCircle.Direction0,
        mCircle.Direction1, mCircle.Normal);

    // The intersection is a circle.
    mIntersectionType = IT_CIRCLE;
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrSphere3Sphere3<Real>::Test (Real tmax,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> relVelocity = velocity1 - velocity0;
    Real a = relVelocity.SquaredLength();
    Vector3<Real> CDiff = mSphere1->Center - mSphere0->Center;
    Real c = CDiff.SquaredLength();
    Real rSum = mSphere0->Radius + mSphere1->Radius;
    Real rSumSqr = rSum*rSum;

    if (a > (Real)0)
    {
        Real b = CDiff.Dot(relVelocity);
        if (b <= (Real)0)
        {
            if (-tmax*a <= b)
            {
                return a*c - b*b <= a*rSumSqr;
            }
            else
            {
                return tmax*(tmax*a + ((Real)2)*b) + c <= rSumSqr;
            }
        }
    }

    return c <= rSumSqr;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrSphere3Sphere3<Real>::Find (Real tmax,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> relVelocity = velocity1 - velocity0;
    Real a = relVelocity.SquaredLength();
    Vector3<Real> CDiff = mSphere1->Center - mSphere0->Center;
    Real c = CDiff.SquaredLength();
    Real rSum = mSphere0->Radius + mSphere1->Radius;
    Real rSumSqr = rSum*rSum;

    if (a > (Real)0)
    {
        Real b = CDiff.Dot(relVelocity);
        if (b <= (Real)0)
        {
            if (-tmax*a <= b || tmax*(tmax*a + ((Real)2.0)*b) + c <= rSumSqr)
            {
                Real cdiff = c - rSumSqr;
                Real discr = b*b - a*cdiff;
                if (discr >= (Real)0)
                {
                    if (cdiff <= (Real)0)
                    {
                        // The spheres are initially intersecting.  Estimate a
                        // point of contact by using the midpoint of the line
                        // segment connecting the sphere centers.
                        mContactTime = (Real)0;
                        mContactPoint = ((Real)0.5)*(mSphere0->Center +
                            mSphere1->Center);
                    }
                    else
                    {
                        // The first time of contact is in [0,tmax].
                        mContactTime = -(b + Math<Real>::Sqrt(discr))/a;
                        if (mContactTime < (Real)0)
                        {
                            mContactTime = (Real)0;
                        }
                        else if (mContactTime > tmax)
                        {
                            mContactTime = tmax;
                        }

                        Vector3<Real> newCDiff = CDiff +
                            mContactTime*relVelocity;

                        mContactPoint = mSphere0->Center +
                            mContactTime*velocity0 +
                            (mSphere0->Radius/rSum)*newCDiff;
                    }
                    return true;
                }
            }
            return false;
        }
    }

    if (c <= rSumSqr)
    {
        // The spheres are initially intersecting.  Estimate a point of
        // contact by using the midpoint of the line segment connecting the
        // sphere centers.
        mContactTime = (Real)0;
        mContactPoint = ((Real)0.5)*(mSphere0->Center + mSphere1->Center);
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
const Circle3<Real>& IntrSphere3Sphere3<Real>::GetCircle () const
{
    return mCircle;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& IntrSphere3Sphere3<Real>::GetContactPoint () const
{
    return mContactPoint;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrSphere3Sphere3<float>;

template WM5_MATHEMATICS_ITEM
class IntrSphere3Sphere3<double>;
//----------------------------------------------------------------------------
}
