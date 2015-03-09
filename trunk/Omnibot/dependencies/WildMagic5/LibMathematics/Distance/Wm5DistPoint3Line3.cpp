// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5DistPoint3Line3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
DistPoint3Line3<Real>::DistPoint3Line3 (const Vector3<Real>& point,
    const Line3<Real>& line)
    :
    mPoint(&point),
    mLine(&line)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& DistPoint3Line3<Real>::GetPoint () const
{
    return *mPoint;
}
//----------------------------------------------------------------------------
template <typename Real>
const Line3<Real>& DistPoint3Line3<Real>::GetLine () const
{
    return *mLine;
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Line3<Real>::Get ()
{
    return Math<Real>::Sqrt(GetSquared());
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Line3<Real>::GetSquared ()
{
    Vector3<Real> diff = *mPoint - mLine->Origin;
    mLineParameter = mLine->Direction.Dot(diff);
    mClosestPoint0 = *mPoint;
    mClosestPoint1 = mLine->Origin + mLineParameter*mLine->Direction;
    diff = mClosestPoint1 - mClosestPoint0;
    return diff.SquaredLength();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Line3<Real>::Get (Real t, const Vector3<Real>& velocity0,
    const Vector3<Real>& velocity1)
{
    Vector3<Real> movedPoint = *mPoint + t*velocity0;
    Vector3<Real> movedOrigin = mLine->Origin + t*velocity1;
    Line3<Real> movedLine(movedOrigin, mLine->Direction);
    return DistPoint3Line3<Real>(movedPoint, movedLine).Get();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Line3<Real>::GetSquared (Real t,
    const Vector3<Real>& velocity0, const Vector3<Real>& velocity1)
{
    Vector3<Real> movedPoint = *mPoint + t*velocity0;
    Vector3<Real> movedOrigin = mLine->Origin + t*velocity1;
    Line3<Real> movedLine(movedOrigin, mLine->Direction);
    return DistPoint3Line3<Real>(movedPoint, movedLine).GetSquared();
}
//----------------------------------------------------------------------------
template <typename Real>
Real DistPoint3Line3<Real>::GetLineParameter () const
{
    return mLineParameter;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class DistPoint3Line3<float>;

template WM5_MATHEMATICS_ITEM
class DistPoint3Line3<double>;
//----------------------------------------------------------------------------
}
