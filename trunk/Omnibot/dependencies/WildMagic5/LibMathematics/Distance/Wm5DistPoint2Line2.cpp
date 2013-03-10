// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5DistPoint2Line2.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
DistPoint2Line2<Real>::DistPoint2Line2 (const Vector2<Real>& point,
    const Line2<Real>& line)
    :
    mPoint(&point),
    mLine(&line)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector2<Real>& DistPoint2Line2<Real>::GetPoint () const
{
    return *mPoint;
}
//----------------------------------------------------------------------------
template <typename Real>
const Line2<Real>& DistPoint2Line2<Real>::GetLine () const
{
    return *mLine;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint2Line2<Real>::Get ()
{
    return Math<Real>::Sqrt(GetSquared());
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint2Line2<Real>::GetSquared ()
{
    Vector2<Real> diff = *mPoint - mLine->Origin;
    Real param = mLine->Direction.Dot(diff);
    mClosestPoint0 = *mPoint;
    mClosestPoint1 = mLine->Origin + param*mLine->Direction;
    diff = mClosestPoint1 - mClosestPoint0;
    return diff.SquaredLength();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint2Line2<Real>::Get (Real t, const Vector2<Real>& velocity0,
    const Vector2<Real>& velocity1)
{
    Vector2<Real> movedPoint = *mPoint + t*velocity0;
    Vector2<Real> movedOrigin = mLine->Origin + t*velocity1;
    Line2<Real> movedLine(movedOrigin, mLine->Direction);
    return DistPoint2Line2<Real>(movedPoint, movedLine).Get();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint2Line2<Real>::GetSquared (Real t,
    const Vector2<Real>& velocity0, const Vector2<Real>& velocity1)
{
    Vector2<Real> movedPoint = *mPoint + t*velocity0;
    Vector2<Real> movedOrigin = mLine->Origin + t*velocity1;
    Line2<Real> movedLine(movedOrigin, mLine->Direction);
    return DistPoint2Line2<Real>(movedPoint, movedLine).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class DistPoint2Line2<float>;

template WM5_MATHEMATICS_ITEM
class DistPoint2Line2<double>;
//----------------------------------------------------------------------------
}
