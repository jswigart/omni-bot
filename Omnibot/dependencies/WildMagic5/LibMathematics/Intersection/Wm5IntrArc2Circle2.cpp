// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrArc2Circle2.h"
#include "Wm5IntrCircle2Circle2.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrArc2Circle2<Real>::IntrArc2Circle2 (const Arc2<Real>& arc,
    const Circle2<Real>& circle)
    :
    mArc(&arc),
    mCircle(&circle)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Arc2<Real>& IntrArc2Circle2<Real>::GetArc () const
{
    return *mArc;
}
//----------------------------------------------------------------------------
template <typename Real>
const Circle2<Real>& IntrArc2Circle2<Real>::GetCircle () const
{
    return *mCircle;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrArc2Circle2<Real>::Find ()
{
    mQuantity = 0;

    Circle2<Real> circleOfArc(mArc->Center, mArc->Radius);
    IntrCircle2Circle2<Real> intr(*mCircle, circleOfArc);
    if (!intr.Find())
    {
        // Arc and circle do not intersect.
        mIntersectionType = IT_EMPTY;
        return false;
    }

    if (intr.GetIntersectionType() == IT_OTHER)
    {
        // Arc is on the circle.
        mIntersectionType = IT_OTHER;
        return true;
    }

    // Test whether circle-circle intersection points are on the arc.
    for (int i = 0; i < intr.GetQuantity(); ++i)
    {
        if (mArc->Contains(intr.GetPoint(i)))
        {
            mPoint[mQuantity++] = intr.GetPoint(i);
        }
    }

    mIntersectionType = (mQuantity > 0 ? IT_POINT : IT_EMPTY);
    return mIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrArc2Circle2<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector2<Real>& IntrArc2Circle2<Real>::GetPoint (int i) const
{
    return mPoint[i];
}
//----------------------------------------------------------------------------
template <typename Real>
const Arc2<Real>& IntrArc2Circle2<Real>::GetIntersectionArc () const
{
    return *mArc;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrArc2Circle2<float>;

template WM5_MATHEMATICS_ITEM
class IntrArc2Circle2<double>;
//----------------------------------------------------------------------------
}
