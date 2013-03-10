// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrRay2Box2.h"
#include "Wm5IntrLine2Box2.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrRay2Box2<Real>::IntrRay2Box2 (const Ray2<Real>& ray, const Box2<Real>& box)
    :
    mRay(&ray),
    mBox(&box)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Ray2<Real>& IntrRay2Box2<Real>::GetRay () const
{
    return *mRay;
}
//----------------------------------------------------------------------------
template <typename Real>
const Box2<Real>& IntrRay2Box2<Real>::GetBox () const
{
    return *mBox;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrRay2Box2<Real>::Test ()
{
    Real WdU[2], AWdU[2], DdU[2], ADdU[2];

    Vector2<Real> diff = mRay->Origin - mBox->Center;

    WdU[0] = mRay->Direction.Dot(mBox->Axis[0]);
    AWdU[0] = Math<Real>::FAbs(WdU[0]);
    DdU[0] = diff.Dot(mBox->Axis[0]);
    ADdU[0] = Math<Real>::FAbs(DdU[0]);
    if (ADdU[0] > mBox->Extent[0] && DdU[0]*WdU[0] >= (Real)0)
    {
        return false;
    }

    WdU[1] = mRay->Direction.Dot(mBox->Axis[1]);
    AWdU[1] = Math<Real>::FAbs(WdU[1]);
    DdU[1] = diff.Dot(mBox->Axis[1]);
    ADdU[1] = Math<Real>::FAbs(DdU[1]);
    if (ADdU[1] > mBox->Extent[1] && DdU[1]*WdU[1] >= (Real)0)
    {
        return false;
    }

    Vector2<Real> perp = mRay->Direction.Perp();
    Real LHS = Math<Real>::FAbs(perp.Dot(diff));
    Real part0 = Math<Real>::FAbs(perp.Dot(mBox->Axis[0]));
    Real part1 = Math<Real>::FAbs(perp.Dot(mBox->Axis[1]));
    Real RHS = mBox->Extent[0]*part0 + mBox->Extent[1]*part1;
    return LHS <= RHS;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrRay2Box2<Real>::Find ()
{
    Real t0 = (Real)0, t1 = Math<Real>::MAX_REAL;
    return IntrLine2Box2<Real>::DoClipping(t0, t1, mRay->Origin,
        mRay->Direction, *mBox, true, mQuantity, mPoint, mIntersectionType);
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrRay2Box2<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector2<Real>& IntrRay2Box2<Real>::GetPoint (int i) const
{
    return mPoint[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrRay2Box2<float>;

template WM5_MATHEMATICS_ITEM
class IntrRay2Box2<double>;
//----------------------------------------------------------------------------
}
