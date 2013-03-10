// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrSegment2Box2.h"
#include "Wm5IntrLine2Box2.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrSegment2Box2<Real>::IntrSegment2Box2 (const Segment2<Real>& segment,
    const Box2<Real>& box, bool solid)
    :
    mSegment(&segment),
    mBox(&box)
{
    mSolid = solid;
}
//----------------------------------------------------------------------------
template <typename Real>
const Segment2<Real>& IntrSegment2Box2<Real>::GetSegment () const
{
    return *mSegment;
}
//----------------------------------------------------------------------------
template <typename Real>
const Box2<Real>& IntrSegment2Box2<Real>::GetBox () const
{
    return *mBox;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrSegment2Box2<Real>::Test ()
{
    Vector2<Real> diff = mSegment->Center - mBox->Center;

    Real AWdU[2], ADdU[2], RHS;
    AWdU[0] = Math<Real>::FAbs(mSegment->Direction.Dot(mBox->Axis[0]));
    ADdU[0] = Math<Real>::FAbs(diff.Dot(mBox->Axis[0]));
    RHS = mBox->Extent[0] + mSegment->Extent*AWdU[0];
    if (ADdU[0] > RHS)
    {
        return false;
    }

    AWdU[1] = Math<Real>::FAbs(mSegment->Direction.Dot(mBox->Axis[1]));
    ADdU[1] = Math<Real>::FAbs(diff.Dot(mBox->Axis[1]));
    RHS = mBox->Extent[1] + mSegment->Extent*AWdU[1];
    if (ADdU[1] > RHS)
    {
        return false;
    }

    Vector2<Real> perp = mSegment->Direction.Perp();
    Real LHS = Math<Real>::FAbs(perp.Dot(diff));
    Real part0 = Math<Real>::FAbs(perp.Dot(mBox->Axis[0]));
    Real part1 = Math<Real>::FAbs(perp.Dot(mBox->Axis[1]));
    RHS = mBox->Extent[0]*part0 + mBox->Extent[1]*part1;
    return LHS <= RHS;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrSegment2Box2<Real>::Find ()
{
    Real t0 = -mSegment->Extent, t1 = mSegment->Extent;
    return IntrLine2Box2<Real>::DoClipping(t0, t1, mSegment->Center,
        mSegment->Direction, *mBox, mSolid, mQuantity, mPoint,
        mIntersectionType);
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrSegment2Box2<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector2<Real>& IntrSegment2Box2<Real>::GetPoint (int i) const
{
    return mPoint[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrSegment2Box2<float>;

template WM5_MATHEMATICS_ITEM
class IntrSegment2Box2<double>;
//----------------------------------------------------------------------------
}
