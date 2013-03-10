// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrSegment2Circle2.h"
#include "Wm5IntrLine2Circle2.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrSegment2Circle2<Real>::IntrSegment2Circle2 (const Segment2<Real>& segment,
    const Circle2<Real>& circle)
    :
    mSegment(&segment),
    mCircle(&circle)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Segment2<Real>& IntrSegment2Circle2<Real>::GetSegment () const
{
    return *mSegment;
}
//----------------------------------------------------------------------------
template <typename Real>
const Circle2<Real>& IntrSegment2Circle2<Real>::GetCircle () const
{
    return *mCircle;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrSegment2Circle2<Real>::Find ()
{
    Real t[2];
    bool intersects = IntrLine2Circle2<Real>::Find(mSegment->Center,
        mSegment->Direction, mCircle->Center, mCircle->Radius, mQuantity, t);

    if (intersects)
    {
        // Reduce root count if line-circle intersections are not on segment.
        if (mQuantity == 1)
        {
            if (Math<Real>::FAbs(t[0]) > mSegment->Extent)
            {
                mQuantity = 0;
            }
        }
        else
        {
            if (t[1] < -mSegment->Extent || t[0] > mSegment->Extent)
            {
                mQuantity = 0;
            }
            else
            {
                if (t[1] <= mSegment->Extent)
                {
                    if (t[0] < -mSegment->Extent)
                    {
                        mQuantity = 1;
                        t[0] = t[1];
                    }
                }
                else
                {
                    mQuantity = (t[0] >= -mSegment->Extent ? 1 : 0);
                }
            }
        }

        for (int i = 0; i < mQuantity; ++i)
        {
            mPoint[i] = mSegment->Center + t[i]*mSegment->Direction;
        }
    }

    mIntersectionType = (mQuantity > 0 ? IT_POINT : IT_EMPTY);
    return mIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrSegment2Circle2<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector2<Real>& IntrSegment2Circle2<Real>::GetPoint (int i) const
{
    return mPoint[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrSegment2Circle2<float>;

template WM5_MATHEMATICS_ITEM
class IntrSegment2Circle2<double>;
//----------------------------------------------------------------------------
}
