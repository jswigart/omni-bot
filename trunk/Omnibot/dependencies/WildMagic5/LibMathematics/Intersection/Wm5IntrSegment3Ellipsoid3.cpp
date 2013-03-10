// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrSegment3Ellipsoid3.h"
#include "Wm5Intersector1.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrSegment3Ellipsoid3<Real>::IntrSegment3Ellipsoid3 (
    const Segment3<Real>& segment, const Ellipsoid3<Real>& ellipsoid)
    :
    mSegment(&segment),
    mEllipsoid(&ellipsoid)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Segment3<Real>& IntrSegment3Ellipsoid3<Real>::GetSegment () const
{
    return *mSegment;
}
//----------------------------------------------------------------------------
template <typename Real>
const Ellipsoid3<Real>& IntrSegment3Ellipsoid3<Real>::GetEllipsoid () const
{
    return *mEllipsoid;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrSegment3Ellipsoid3<Real>::Test ()
{
    // The ellipsoid is (X-K)^T*M*(X-K)-1 = 0 and the segment is X = P+t*D.
    // Substitute the segment equation into the ellipsoid equation to obtain
    // a quadratic equation
    //   Q(t) = a2*t^2 + 2*a1*t + a0 = 0
    // where a2 = D^T*M*D, a1 = D^T*M*(P-K), and a0 = (P-K)^T*M*(P-K)-1.

    Matrix3<Real> M;
    mEllipsoid->GetM(M);

    Vector3<Real> diff = mSegment->Center - mEllipsoid->Center;
    Vector3<Real> matDir = M*mSegment->Direction;
    Vector3<Real> matDiff = M*diff;
    Real a2 = mSegment->Direction.Dot(matDir);
    Real a1 = mSegment->Direction.Dot(matDiff);
    Real a0 = diff.Dot(matDiff) - (Real)1;

    // No intersection if Q(t) has no real roots.
    Real discr = a1*a1 - a0*a2;
    if (discr < (Real)0)
    {
        return false;
    }

    // Test whether segment origin is inside ellipsoid.
    if (a0 <= (Real)0)
    {
        return true;
    }

    // At this point, Q(0) = a0 > 0 and Q(t) has real roots.  It is also
    // the case that a2 > 0, since M is positive definite, implying that
    // D^T*M*D > 0 for any nonzero vector D.

    Real q, qder, e = mSegment->Extent;

    if (a1 >= (Real)0)
    {
        // Roots are possible only on [-e,0], e is the segment extent.  At
        // least one root occurs if Q(-e) <= 0 or if Q(-e) > 0 and Q'(-e) < 0.
        q = a0 + e*(((Real)-2)*a1 + a2*e);
        if (q <= (Real)0)
        {
            return true;
        }

        qder = a1 - a2*e;
        if (qder < (Real)0)
        {
            return true;
        }
    }
    else
    {
        // Roots are only possible on [0,e], e is the segment extent.  At
        // least one root occurs if Q(e) <= 0 or if Q(e) > 0 and Q'(e) > 0.
        q = a0 + e*(((Real)2)*a1 + a2*e);
        if (q <= (Real)0.0)
        {
            return true;
        }

        qder = a1 + a2*e;
        if (qder < (Real)0)
        {
            return true;
        }
    }

    return false;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrSegment3Ellipsoid3<Real>::Find ()
{
    // The ellipsoid is (X-K)^T*M*(X-K)-1 = 0 and the line is X = P+t*D.
    // Substitute the line equation into the ellipsoid equation to obtain
    // a quadratic equation
    //   Q(t) = a2*t^2 + 2*a1*t + a0 = 0
    // where a2 = D^T*M*D, a1 = D^T*M*(P-K), and a0 = (P-K)^T*M*(P-K)-1.

    Matrix3<Real> M;
    mEllipsoid->GetM(M);

    Vector3<Real> diff = mSegment->Center - mEllipsoid->Center;
    Vector3<Real> matDir = M*mSegment->Direction;
    Vector3<Real> matDiff = M*diff;
    Real a2 = mSegment->Direction.Dot(matDir);
    Real a1 = mSegment->Direction.Dot(matDiff);
    Real a0 = diff.Dot(matDiff) - (Real)1;

    // Intersection occurs if Q(t) has real roots with t >= 0.
    Real discr = a1*a1 - a0*a2;
    Real t[2];
    if (discr < (Real)0)
    {
        mIntersectionType = IT_EMPTY;
        mQuantity = 0;
    }
    else if (discr > Math<Real>::ZERO_TOLERANCE)
    {
        Real root = Math<Real>::Sqrt(discr);
        Real inv = ((Real)1)/a2;
        t[0] = (-a1 - root)*inv;
        t[1] = (-a1 + root)*inv;

        // assert: t0 < t1 since a2 > 0
        Intersector1<Real> intr(t[0], t[1], -mSegment->Extent,
            mSegment->Extent);
        intr.Find();

        mQuantity = intr.GetNumIntersections();
        if (mQuantity == 2)
        {
            mIntersectionType = IT_SEGMENT;
            mPoint[0] = mSegment->Center + intr.GetIntersection(0) *
                mSegment->Direction;
            mPoint[1] = mSegment->Center + intr.GetIntersection(1) *
                mSegment->Direction;
        }
        else if (mQuantity == 1)
        {
            mIntersectionType = IT_POINT;
            mPoint[0] = mSegment->Center + intr.GetIntersection(0) *
                mSegment->Direction;
        }
        else
        {
            mIntersectionType = IT_EMPTY;
        }
    }
    else
    {
        t[0] = -a1/a2;
        if (Math<Real>::FAbs(t[0]) <= mSegment->Extent)
        {
            mIntersectionType = IT_POINT;
            mQuantity = 1;
            mPoint[0] = mSegment->Center + t[0]*mSegment->Direction;
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
int IntrSegment3Ellipsoid3<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& IntrSegment3Ellipsoid3<Real>::GetPoint (int i) const
{
    return mPoint[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrSegment3Ellipsoid3<float>;

template WM5_MATHEMATICS_ITEM
class IntrSegment3Ellipsoid3<double>;
//----------------------------------------------------------------------------
}
