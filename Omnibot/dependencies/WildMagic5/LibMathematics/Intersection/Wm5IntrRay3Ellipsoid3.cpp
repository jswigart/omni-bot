// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrRay3Ellipsoid3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrRay3Ellipsoid3<Real>::IntrRay3Ellipsoid3 (const Ray3<Real>& rkRay,
    const Ellipsoid3<Real>& rkEllipsoid)
    :
    mRay(&rkRay),
    mEllipsoid(&rkEllipsoid)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Ray3<Real>& IntrRay3Ellipsoid3<Real>::GetRay () const
{
    return *mRay;
}
//----------------------------------------------------------------------------
template <typename Real>
const Ellipsoid3<Real>& IntrRay3Ellipsoid3<Real>::GetEllipsoid () const
{
    return *mEllipsoid;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrRay3Ellipsoid3<Real>::Test ()
{
    // The ellipsoid is (X-K)^T*M*(X-K)-1 = 0 and the ray is X = P+t*D.
    // Substitute the ray equation into the ellipsoid equation to obtain
    // a quadratic equation
    //   Q(t) = a2*t^2 + 2*a1*t + a0 = 0
    // where a2 = D^T*M*D, a1 = D^T*M*(P-K), and a0 = (P-K)^T*M*(P-K)-1.

    Matrix3<Real> M;
    mEllipsoid->GetM(M);

    Vector3<Real> diff = mRay->Origin - mEllipsoid->Center;
    Vector3<Real> matDir = M*mRay->Direction;
    Vector3<Real> matDiff = M*diff;
    Real a2 = mRay->Direction.Dot(matDir);
    Real a1 = mRay->Direction.Dot(matDiff);
    Real a0 = diff.Dot(matDiff) - (Real)1;

    // No intersection if Q(t) has no real roots.
    Real discr = a1*a1 - a0*a2;
    if (discr < (Real)0)
    {
        return false;
    }

    // Test whether ray origin is inside ellipsoid.
    if (a0 <= (Real)0)
    {
        return true;
    }

    // At this point, Q(0) = a0 > 0 and Q(t) has real roots.  It is also
    // the case that a2 > 0, since M is positive definite, implying that
    // D^T*M*D > 0 for any nonzero vector D.  Thus, an intersection occurs
    // only when Q'(0) < 0.
    return a1 < (Real)0;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrRay3Ellipsoid3<Real>::Find ()
{
    // The ellipsoid is (X-K)^T*M*(X-K)-1 = 0 and the line is X = P+t*D.
    // Substitute the line equation into the ellipsoid equation to obtain
    // a quadratic equation
    //   Q(t) = a2*t^2 + 2*a1*t + a0 = 0
    // where a2 = D^T*M*D, a1 = D^T*M*(P-K), and a0 = (P-K)^T*M*(P-K)-1.

    Matrix3<Real> M;
    mEllipsoid->GetM(M);

    Vector3<Real> diff = mRay->Origin - mEllipsoid->Center;
    Vector3<Real> matDir = M*mRay->Direction;
    Vector3<Real> matDiff = M*diff;
    Real a2 = mRay->Direction.Dot(matDir);
    Real a1 = mRay->Direction.Dot(matDiff);
    Real a0 = diff.Dot(matDiff) - (Real)1;

    // Intersection occurs if Q(t) has real roots with t >= 0.
    Real discr = a1*a1 - a0*a2;
    Real t[2];
    if (discr < (Real)0)
    {
        mIntersectionType = IT_EMPTY;
        mQuantity = 0;
    }
    else if (discr > (Real)0)
    {
        Real root = Math<Real>::Sqrt(discr);
        Real inv = ((Real)1)/a2;
        t[0] = (-a1 - root)*inv;
        t[1] = (-a1 + root)*inv;

        if (t[0] >= (Real)0)
        {
            mIntersectionType = IT_SEGMENT;
            mQuantity = 2;
            mPoint[0] = mRay->Origin + t[0]*mRay->Direction;
            mPoint[1] = mRay->Origin + t[1]*mRay->Direction;
        }
        else if (t[1] >= (Real)0)
        {
            mIntersectionType = IT_POINT;
            mQuantity = 1;
            mPoint[0] = mRay->Origin + t[1]*mRay->Direction;
            t[0] = t[1];
        }
        else
        {
            mIntersectionType = IT_EMPTY;
            mQuantity = 0;
        }
    }
    else
    {
        t[0] = -a1/a2;
        if (t[0] >= (Real)0)
        {
            mIntersectionType = IT_POINT;
            mQuantity = 1;
            mPoint[0] = mRay->Origin + t[0]*mRay->Direction;
        }
        else
        {
            mIntersectionType = IT_EMPTY;
            mQuantity = 0;
        }
    }

    return mIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrRay3Ellipsoid3<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& IntrRay3Ellipsoid3<Real>::GetPoint (int i) const
{
    return mPoint[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrRay3Ellipsoid3<float>;

template WM5_MATHEMATICS_ITEM
class IntrRay3Ellipsoid3<double>;
//----------------------------------------------------------------------------
}
