// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5DistRay3Ray3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
DistRay3Ray3<Real>::DistRay3Ray3 (const Ray3<Real>& ray0,
    const Ray3<Real>& ray1)
    :
    mRay0(&ray0),
    mRay1(&ray1)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Ray3<Real>& DistRay3Ray3<Real>::GetRay0 () const
{
    return *mRay0;
}
//----------------------------------------------------------------------------
template <typename Real>
const Ray3<Real>& DistRay3Ray3<Real>::GetRay1 () const
{
    return *mRay1;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistRay3Ray3<Real>::Get ()
{
    return Math<Real>::Sqrt(GetSquared());
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistRay3Ray3<Real>::GetSquared ()
{
    Vector3<Real> diff = mRay0->Origin - mRay1->Origin;
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
                sqrDist = s0*(s0 + a01*s1 + ((Real)2)*b0) +
                    s1*(a01*s0 + s1 + ((Real)2)*b1) + c;
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
        if (a01 > (Real)0)
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
    mRay0Parameter = s0;
    mRay1Parameter = s1;

    // Account for numerical round-off errors.
    if (sqrDist < (Real)0)
    {
        sqrDist = (Real)0;
    }
    return sqrDist;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistRay3Ray3<Real>::Get (Real t, const Vector3<Real>& velocity0,
    const Vector3<Real>& velocity1)
{
    Vector3<Real> movedOrigin0 = mRay0->Origin + t*velocity0;
    Vector3<Real> movedOrigin1 = mRay1->Origin + t*velocity1;
    Ray3<Real> movedRay0(movedOrigin0, mRay0->Direction);
    Ray3<Real> movedRay1(movedOrigin1, mRay1->Direction);
    return DistRay3Ray3<Real>(movedRay0, movedRay1).Get();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistRay3Ray3<Real>::GetSquared (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedOrigin0 = mRay0->Origin + t*velocity0;
    Vector3<Real> movedOrigin1 = mRay1->Origin + t*velocity1;
    Ray3<Real> movedRay0(movedOrigin0, mRay0->Direction);
    Ray3<Real> movedRay1(movedOrigin1, mRay1->Direction);
    return DistRay3Ray3<Real>(movedRay0, movedRay1).GetSquared();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistRay3Ray3<Real>::GetRay0Parameter () const
{
    return mRay0Parameter;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistRay3Ray3<Real>::GetRay1Parameter () const
{
    return mRay1Parameter;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class DistRay3Ray3<float>;

template WM5_MATHEMATICS_ITEM
class DistRay3Ray3<double>;
//----------------------------------------------------------------------------
}
