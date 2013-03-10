// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrSegment3Cylinder3.h"
#include "Wm5IntrLine3Cylinder3.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrSegment3Cylinder3<Real>::IntrSegment3Cylinder3 (
    const Segment3<Real>& segment, const Cylinder3<Real>& cylinder)
    :
    mSegment(&segment),
    mCylinder(&cylinder)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Segment3<Real>& IntrSegment3Cylinder3<Real>::GetSegment () const
{
    return *mSegment;
}
//----------------------------------------------------------------------------
template <typename Real>
const Cylinder3<Real>& IntrSegment3Cylinder3<Real>::GetCylinder () const
{
    return *mCylinder;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrSegment3Cylinder3<Real>::Find ()
{
    Real t[2];
    int quantity = IntrLine3Cylinder3<Real>::Find(mSegment->Center,
        mSegment->Direction, *mCylinder, t);

    mQuantity = 0;
    for (int i = 0; i < quantity; ++i)
    {
        if (Math<Real>::FAbs(t[i]) <= mSegment->Extent)
        {
            mPoint[mQuantity++] = mSegment->Center +
                t[i]*mSegment->Direction;
        }
    }

    if (mQuantity == 2)
    {
        mIntersectionType = IT_SEGMENT;
    }
    else if (mQuantity == 1)
    {
        mIntersectionType = IT_POINT;
    }
    else
    {
        mIntersectionType = IT_EMPTY;
    }

    return mIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrSegment3Cylinder3<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector3<Real>& IntrSegment3Cylinder3<Real>::GetPoint (int i) const
{
    return mPoint[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrSegment3Cylinder3<float>;

template WM5_MATHEMATICS_ITEM
class IntrSegment3Cylinder3<double>;
//----------------------------------------------------------------------------
}
