// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5DistRay2Ray2.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
DistRay2Ray2<Real>::DistRay2Ray2 (const Ray2<Real>& ray0,
    const Ray2<Real>& ray1)
    :
    mRay0(&ray0),
    mRay1(&ray1)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Ray2<Real>& DistRay2Ray2<Real>::GetRay0 () const
{
    return *mRay0;
}
//----------------------------------------------------------------------------
template <typename Real>
const Ray2<Real>& DistRay2Ray2<Real>::GetRay1 () const
{
    return *mRay1;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistRay2Ray2<Real>::Get ()
{
    return Math<Real>::Sqrt(GetSquared());
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistRay2Ray2<Real>::GetSquared ()
{
    Vector2<Real> diff = mRay0->Origin - mRay1->Origin;
    Real a01 = -mRay0->Direction.Dot(mRay1->Direction);
    Real b0 = diff.Dot(mRay0->Direction);
    Real c = diff.SquaredLength();
    Real det = Math<Real>::FAbs((Real)1 - a01*a01);
    Real b1, s0, s1, sqrDist;

    if (det >= Math<Real>::ZERO_TOLERANCE)
    {
        // Rays are not parallel.
        b1 = -diff.Dot(mRay1->Direction);
        s0 = a01*b1 - b0;
        s1 = a01*b0 - b1;

        if (s0 >= (Real)0)
        {
            if (s1 >= (Real)0)  // region 0 (interior)
            {
                // Minimum at two interior points of rays.
                Real invDet = ((Real)1)/det;
                s0 *= invDet;
                s1 *= invDet;
                sqrDist = (Real)0;
            }
            else  // region 3 (side)
            {
                s1 = (Real)0;
                if (b0 >= (Real)0)
                {
                    s0 = (Real)0;
                    sqrDist = c;
                }
                else
                {
                    s0 = -b0;
                    sqrDist = b0*s0 + c;
                }
            }
        }
        else
        {
            if (s1 >= (Real)0)  // region 1 (side)
            {
                s0 = (Real)0;
                if (b1 >= (Real)0)
                {
                    s1 = (Real)0;
                    sqrDist = c;
                }
                else
                {
                    s1 = -b1;
                    sqrDist = b1*s1 + c;
                }
            }
            else  // region 2 (corner)
            {
                if (b0 < (Real)0)
                {
                    s0 = -b0;
                    s1 = (Real)0;
                    sqrDist = b0*s0 + c;
                }
                else
                {
                    s0 = (Real)0;
                    if (b1 >= (Real)0)
                    {
                        s1 = (Real)0;
                        sqrDist = c;
                    }
                    else
                    {
                        s1 = -b1;
                        sqrDist = b1*s1 + c;
                    }
                }
            }
        }
    }
    else
    {
        // Rays are parallel.
        if (a01 > (Real)0.0)
        {
            // Opposite direction vectors.
            s1 = (Real)0;
            if (b0 >= (Real)0)
            {
                s0 = (Real)0;
                sqrDist = c;
            }
            else
            {
                s0 = -b0;
                sqrDist = b0*s0 + c;
            }
        }
        else
        {
            // Same direction vectors.
            if (b0 >= (Real)0)
            {
                b1 = -diff.Dot(mRay1->Direction);
                s0 = (Real)0;
                s1 = -b1;
                sqrDist = b1*s1 + c;
            }
            else
            {
                s0 = -b0;
                s1 = (Real)0;
                sqrDist = b0*s0 + c;
            }
        }
    }

    mClosestPoint0 = mRay0->Origin + s0*mRay0->Direction;
    mClosestPoint1 = mRay1->Origin + s1*mRay1->Direction;

    // Account for numerical round-off errors.
    if (sqrDist < (Real)0)
    {
        sqrDist = (Real)0;
    }
    return sqrDist;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistRay2Ray2<Real>::Get (Real t, const Vector2<Real>& velocity0,
    const Vector2<Real>& velocity1)
{
    Vector2<Real> movedOrigin0 = mRay0->Origin + t*velocity0;
    Vector2<Real> movedOrigin1 = mRay1->Origin + t*velocity1;
    Ray2<Real> movedRay0(movedOrigin0, mRay0->Direction);
    Ray2<Real> movedRay1(movedOrigin1, mRay1->Direction);
    return DistRay2Ray2<Real>(movedRay0, movedRay1).Get();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistRay2Ray2<Real>::GetSquared (Real t,
    const Vector2<Real>& velocity0, const Vector2<Real>& velocity1)
{
    Vector2<Real> movedOrigin0 = mRay0->Origin + t*velocity0;
    Vector2<Real> movedOrigin1 = mRay1->Origin + t*velocity1;
    Ray2<Real> movedRay0(movedOrigin0, mRay0->Direction);
    Ray2<Real> movedRay1(movedOrigin1, mRay1->Direction);
    return DistRay2Ray2<Real>(movedRay0, movedRay1).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class DistRay2Ray2<float>;

template WM5_MATHEMATICS_ITEM
class DistRay2Ray2<double>;
//----------------------------------------------------------------------------
}
