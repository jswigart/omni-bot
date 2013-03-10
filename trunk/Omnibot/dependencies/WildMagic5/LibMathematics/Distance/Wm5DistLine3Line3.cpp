// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5DistLine3Line3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
DistLine3Line3<Real>::DistLine3Line3 (const Line3<Real>& line0,
    const Line3<Real>& line1)
    :
    mLine0(&line0),
    mLine1(&line1)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Line3<Real>& DistLine3Line3<Real>::GetLine0 () const
{
    return *mLine0;
}
//----------------------------------------------------------------------------
template <typename Real>
const Line3<Real>& DistLine3Line3<Real>::GetLine1 () const
{
    return *mLine1;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Line3<Real>::Get ()
{
    return Math<Real>::Sqrt(GetSquared());
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Line3<Real>::GetSquared ()
{
    Vector3<Real> diff = mLine0->Origin - mLine1->Origin;
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
        sqrDist = s0*(s0 + a01*s1 + ((Real)2)*b0) +
            s1*(a01*s0 + s1 + ((Real)2)*b1) + c;
    }
    else
    {
        // Lines are parallel, select any closest pair of points.
        s0 = -b0;
        s1 = (Real)0;
        sqrDist = b0*s0 + c;
    }

    mClosestPoint0 = mLine0->Origin + s0*mLine0->Direction;
    mClosestPoint1 = mLine1->Origin + s1*mLine1->Direction;
    mLine0Parameter = s0;
    mLine1Parameter = s1;

    // Account for numerical round-off errors.
    if (sqrDist < (Real)0)
    {
        sqrDist = (Real)0;
    }
    return sqrDist;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Line3<Real>::Get (Real t, const Vector3<Real>& velocity0,
    const Vector3<Real>& velocity1)
{
    Vector3<Real> movedOrigin0 = mLine0->Origin + t*velocity0;
    Vector3<Real> movedOrigin1 = mLine1->Origin + t*velocity1;
    Line3<Real> movedLine0(movedOrigin0, mLine0->Direction);
    Line3<Real> movedLine1(movedOrigin1, mLine1->Direction);
    return DistLine3Line3<Real>(movedLine0, movedLine1).Get();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Line3<Real>::GetSquared (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedOrigin0 = mLine0->Origin + t*velocity0;
    Vector3<Real> movedOrigin1 = mLine1->Origin + t*velocity1;
    Line3<Real> movedLine0(movedOrigin0, mLine0->Direction);
    Line3<Real> movedLine1(movedOrigin1, mLine1->Direction);
    return DistLine3Line3<Real>(movedLine0, movedLine1).GetSquared();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Line3<Real>::GetLine0Parameter () const
{
    return mLine0Parameter;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistLine3Line3<Real>::GetLine1Parameter () const
{
    return mLine1Parameter;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class DistLine3Line3<float>;

template WM5_MATHEMATICS_ITEM
class DistLine3Line3<double>;
//----------------------------------------------------------------------------
}
