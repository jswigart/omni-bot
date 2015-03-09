// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5DistLine2Ray2.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
DistLine2Ray2<Real>::DistLine2Ray2 (const Line2<Real>& line,
    const Ray2<Real>& ray)
    :
    mLine(&line),
    mRay(&ray)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Line2<Real>& DistLine2Ray2<Real>::GetLine () const
{
    return *mLine;
}
//----------------------------------------------------------------------------
template <typename Real>
const Ray2<Real>& DistLine2Ray2<Real>::GetRay () const
{
    return *mRay;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine2Ray2<Real>::Get ()
{
    return Math<Real>::Sqrt(GetSquared());
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine2Ray2<Real>::GetSquared ()
{
    Vector2<Real> diff = mLine->Origin - mRay->Origin;
    Real a01 = -mLine->Direction.Dot(mRay->Direction);
    Real b0 = diff.Dot(mLine->Direction);
    Real c = diff.SquaredLength();
    Real det = Math<Real>::FAbs((Real)1 - a01*a01);
    Real b1, s0, s1, sqrDist;

    if (det >= Math<Real>::ZERO_TOLERANCE)
    {
        b1 = -diff.Dot(mRay->Direction);
        s1 = a01*b0 - b1;

        if (s1 >= (Real)0)
        {
            // Two interior points are closest, one on line and one on ray.
            Real invDet = ((Real)1)/det;
            s0 = (a01*b1 - b0)*invDet;
            s1 *= invDet;
            sqrDist = (Real)0;
        }
        else
        {
            // Origin of ray and interior point of line are closest.
            s0 = -b0;
            s1 = (Real)0;
            sqrDist = b0*s0 + c;

            // Account for numerical round-off errors.
            if (sqrDist < (Real)0)
            {
                sqrDist = (Real)0;
            }
        }
    }
    else
    {
        // Lines are parallel, closest pair with one point at ray origin.
        s0 = -b0;
        s1 = (Real)0;
        sqrDist = b0*s0 + c;

        // Account for numerical round-off errors.
        if (sqrDist < (Real)0)
        {
            sqrDist = (Real)0;
        }
    }

    mClosestPoint0 = mLine->Origin + s0*mLine->Direction;
    mClosestPoint1 = mRay->Origin + s1*mRay->Direction;
    return sqrDist;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine2Ray2<Real>::Get (Real t, const Vector2<Real>& velocity0,
    const Vector2<Real>& velocity1)
{
    Vector2<Real> movedOrigin0 = mLine->Origin + t*velocity0;
    Vector2<Real> movedOrigin1 = mRay->Origin + t*velocity1;
    Line2<Real> movedLine(movedOrigin0, mLine->Direction);
    Ray2<Real> movedRay(movedOrigin1, mRay->Direction);
    return DistLine2Ray2<Real>(movedLine, movedRay).Get();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine2Ray2<Real>::GetSquared (Real t,
    const Vector2<Real>& velocity0, const Vector2<Real>& velocity1)
{
    Vector2<Real> movedOrigin0 = mLine->Origin + t*velocity0;
    Vector2<Real> movedOrigin1 = mRay->Origin + t*velocity1;
    Line2<Real> movedLine(movedOrigin0, mLine->Direction);
    Ray2<Real> movedRay(movedOrigin1, mRay->Direction);
    return DistLine2Ray2<Real>(movedLine, movedRay).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class DistLine2Ray2<float>;

template WM5_MATHEMATICS_ITEM
class DistLine2Ray2<double>;
//----------------------------------------------------------------------------
}
