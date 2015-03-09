// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrRay2Ray2.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrRay2Ray2<Real>::IntrRay2Ray2 (const Ray2<Real>& ray0,
    const Ray2<Real>& ray1)
    :
    mRay0(&ray0),
    mRay1(&ray1)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Ray2<Real>& IntrRay2Ray2<Real>::GetRay0 () const
{
    return *mRay0;
}
//----------------------------------------------------------------------------
template <typename Real>
const Ray2<Real>& IntrRay2Ray2<Real>::GetRay1 () const
{
    return *mRay1;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrRay2Ray2<Real>::Test ()
{
    Vector2<Real> diff;
    Real parameter[2];
    mIntersectionType = Classify(parameter, &diff, 0);

    if (mIntersectionType == IT_POINT)
    {
        // Test whether the line-line intersection is on the rays.
        if (parameter[0] >= (Real)0 && parameter[1] >= (Real)0)
        {
            mQuantity = 1;
        }
        else
        {
            mQuantity = 0;
            mIntersectionType = IT_EMPTY;
        }
    }
    else if (mIntersectionType == IT_RAY)
    {
        mQuantity = INT_MAX;
    }
    else
    {
        mQuantity = 0;
    }

    return mIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrRay2Ray2<Real>::Find ()
{
    Vector2<Real> diff;
    Real parameter[2];
    mIntersectionType = Classify(parameter, &diff, 0);

    if (mIntersectionType == IT_POINT)
    {
        // Test whether the line-line intersection is on the rays.
        if (parameter[0] >= (Real)0 && parameter[1] >= (Real)0)
        {
            mQuantity = 1;
            mPoint = mRay0->Origin + parameter[0]*mRay0->Direction;
        }
        else
        {
            mQuantity = 0;
            mIntersectionType = IT_EMPTY;
        }
    }
    else if (mIntersectionType == IT_RAY)
    {
        mQuantity = INT_MAX;
    }
    else
    {
        mQuantity = 0;
    }

    return mIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrRay2Ray2<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector2<Real>& IntrRay2Ray2<Real>::GetPoint () const
{
    return mPoint;
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrRay2Ray2<Real>::Classify (Real* s, Vector2<Real>* diff,
    Vector2<Real>* diffN)
{
    // The intersection of two lines is a solution to P0+s0*D0 = P1+s1*D1.
    // Rewrite this as s0*D0 - s1*D1 = P1 - P0 = Q.  If D0.Dot(Perp(D1)) = 0,
    // the lines are parallel.  Additionally, if Q.Dot(Perp(D1)) = 0, the
    // lines are the same.  If D0.Dot(Perp(D1)) is not zero, then
    //   s0 = Q.Dot(Perp(D1))/D0.Dot(Perp(D1))
    // produces the point of intersection.  Also,
    //   s1 = Q.Dot(Perp(D0))/D0.Dot(Perp(D1))

    Vector2<Real> originDiff = mRay1->Origin - mRay0->Origin;
    if (diff)
    {
        *diff = originDiff;
    }

    Real D0DotPerpD1 = mRay0->Direction.DotPerp(mRay1->Direction);
    if (Math<Real>::FAbs(D0DotPerpD1) > Math<Real>::ZERO_TOLERANCE)
    {
        // Lines intersect in a single point.
        if (s)
        {
            Real invD0DotPerpD1 = ((Real)1)/D0DotPerpD1;
            Real diffDotPerpD0 = originDiff.DotPerp(mRay0->Direction);
            Real diffDotPerpD1 = originDiff.DotPerp(mRay1->Direction);
            s[0] = diffDotPerpD1*invD0DotPerpD1;
            s[1] = diffDotPerpD0*invD0DotPerpD1;
        }
        return IT_POINT;
    }

    // Lines are parallel.
    originDiff.Normalize();
    if (diffN)
    {
        *diffN = originDiff;
    }

    Real diffNDotPerpD1 = originDiff.DotPerp(mRay1->Direction);
    if (Math<Real>::FAbs(diffNDotPerpD1) <= Math<Real>::ZERO_TOLERANCE)
    {
        // Lines are colinear.
        return IT_RAY;
    }

    // Lines are parallel, but distinct.
    return IT_EMPTY;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrRay2Ray2<float>;

template WM5_MATHEMATICS_ITEM
class IntrRay2Ray2<double>;
//----------------------------------------------------------------------------
}
