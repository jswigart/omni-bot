// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5DistLine2Line2.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
DistLine2Line2<Real>::DistLine2Line2 (const Line2<Real>& rkLine0,
    const Line2<Real>& rkLine1)
    :
    mLine0(&rkLine0),
    mLine1(&rkLine1)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Line2<Real>& DistLine2Line2<Real>::GetLine0 () const
{
    return *mLine0;
}
//----------------------------------------------------------------------------
template <typename Real>
const Line2<Real>& DistLine2Line2<Real>::GetLine1 () const
{
    return *mLine1;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine2Line2<Real>::Get ()
{
    Real sqrDist = GetSquared();
    return Math<Real>::Sqrt(sqrDist);
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine2Line2<Real>::GetSquared ()
{
    Vector2<Real> diff = mLine0->Origin - mLine1->Origin;
    Real a01 = -mLine0->Direction.Dot(mLine1->Direction);
    Real b0 = diff.Dot(mLine0->Direction);
    Real c = diff.SquaredLength();
    Real det = Math<Real>::FAbs((Real)1 - a01*a01);
    Real b1, s0, s1, sqrDist;

    if (det >= Math<Real>::ZERO_TOLERANCE)
    {
        // Lines are not parallel.
        b1 = -diff.Dot(mLine1->Direction);
        Real invDet = ((Real)1)/det;
        s0 = (a01*b1 - b0)*invDet;
        s1 = (a01*b0 - b1)*invDet;
        sqrDist = (Real)0;
    }
    else
    {
        // Lines are parallel, select any closest pair of points.
        s0 = -b0;
        s1 = (Real)0;
        sqrDist = b0*s0 + c;

        // Account for numerical round-off errors.
        if (sqrDist < (Real)0)
        {
            sqrDist = (Real)0;
        }
    }

    mClosestPoint0 = mLine0->Origin + s0*mLine0->Direction;
    mClosestPoint1 = mLine1->Origin + s1*mLine1->Direction;
    return sqrDist;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine2Line2<Real>::Get (Real t, const Vector2<Real>& velocity0,
    const Vector2<Real>& velocity1)
{
    Vector2<Real> movedOrigin0 = mLine0->Origin + t*velocity0;
    Vector2<Real> movedOrigin1 = mLine1->Origin + t*velocity1;
    Line2<Real> movedLine0(movedOrigin0, mLine0->Direction);
    Line2<Real> movedLine1(movedOrigin1, mLine1->Direction);
    return DistLine2Line2<Real>(movedLine0, movedLine1).Get();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine2Line2<Real>::GetSquared (Real t,
    const Vector2<Real>& velocity0, const Vector2<Real>& velocity1)
{
    Vector2<Real> movedOrigin0 = mLine0->Origin + t*velocity0;
    Vector2<Real> movedOrigin1 = mLine1->Origin + t*velocity1;
    Line2<Real> movedLine0(movedOrigin0, mLine0->Direction);
    Line2<Real> movedLine1(movedOrigin1, mLine1->Direction);
    return DistLine2Line2<Real>(movedLine0, movedLine1).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class DistLine2Line2<float>;

template WM5_MATHEMATICS_ITEM
class DistLine2Line2<double>;
//----------------------------------------------------------------------------
}
