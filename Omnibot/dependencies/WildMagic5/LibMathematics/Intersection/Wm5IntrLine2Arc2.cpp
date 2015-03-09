// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrLine2Arc2.h"
#include "Wm5IntrLine2Circle2.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrLine2Arc2<Real>::IntrLine2Arc2 (const Line2<Real>& rkLine,
    const Arc2<Real>& rkArc)
    :
    mLine(&rkLine),
    mArc(&rkArc)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Line2<Real>& IntrLine2Arc2<Real>::GetLine () const
{
    return *mLine;
}
//----------------------------------------------------------------------------
template <typename Real>
const Arc2<Real>& IntrLine2Arc2<Real>::GetArc () const
{
    return *mArc;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrLine2Arc2<Real>::Find ()
{
    Real t[2];
    int quantity;
    bool intersects = IntrLine2Circle2<Real>::Find(mLine->Origin,
        mLine->Direction, mArc->Center, mArc->Radius, quantity, t);

    mQuantity = 0;
    if (intersects)
    {
        for (int i = 0; i < quantity; ++i)
        {
            Vector2<Real> point = mLine->Origin + mLine->Direction*t[i];
            if (mArc->Contains(point))
            {
                mPoint[mQuantity++] = point;
            }
        }
    }

    mIntersectionType = (mQuantity > 0 ? IT_POINT : IT_EMPTY);
    return mIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrLine2Arc2<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector2<Real>& IntrLine2Arc2<Real>::GetPoint (int i) const
{
    return mPoint[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrLine2Arc2<float>;

template WM5_MATHEMATICS_ITEM
class IntrLine2Arc2<double>;
//----------------------------------------------------------------------------
}
