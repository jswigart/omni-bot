// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrArc2Arc2.h"
#include "Wm5IntrCircle2Circle2.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrArc2Arc2<Real>::IntrArc2Arc2 (const Arc2<Real>& arc0,
    const Arc2<Real>& arc1)
    :
    mArc0(&arc0),
    mArc1(&arc1)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Arc2<Real>& IntrArc2Arc2<Real>::GetArc0 () const
{
    return *mArc0;
}
//----------------------------------------------------------------------------
template <typename Real>
const Arc2<Real>& IntrArc2Arc2<Real>::GetArc1 () const
{
    return *mArc1;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrArc2Arc2<Real>::Find ()
{
    mQuantity = 0;

    Circle2<Real> circle0(mArc0->Center, mArc0->Radius);
    Circle2<Real> circle1(mArc1->Center, mArc1->Radius);
    IntrCircle2Circle2<Real> intr(circle0, circle1);
    if (!intr.Find())
    {
        // arcs do not intersect
        mIntersectionType = IT_EMPTY;
        return false;
    }

    if (intr.GetIntersectionType() == IT_OTHER)
    {
        // Arcs are cocircular.  Determine if they overlap.  Let arc0 be
        // <A0,A1> and arc1 be <B0,B1>, the points ordered counterclockwise
        // around the circle of the arc.
        if (mArc1->Contains(mArc0->End0))
        {
            if (mArc1->Contains(mArc0->End1))
            {
                // Arc0 inside arc 1, <B0,A0,A1,B1>.
                mIntersectionType = IT_OTHER;
                mIntersectionArc = *mArc0;
            }
            else
            {
                if (mArc0->End0 != mArc1->End1)
                {
                    // Arc0 and arc1 overlap, <B0,A0,B1,A1>.
                    mIntersectionType = IT_OTHER;
                    mIntersectionArc.Center = mArc0->Center;
                    mIntersectionArc.Radius = mArc0->Radius;
                    mIntersectionArc.End0 = mArc0->End0;
                    mIntersectionArc.End1 = mArc1->End1;
                }
                else
                {
                    // Arc0 and arc1 share end point, <B0,A0,B1,A1>, A0 = B1.
                    mIntersectionType = IT_POINT;
                    mQuantity = 1;
                    mPoint[0] = mArc0->End0;
                }
            }
            return true;
        }

        if (mArc1->Contains(mArc0->End1))
        {
            if (mArc0->End1 != mArc1->End0)
            {
                // Arc0 and arc1 overlap, <A0,B0,A1,B1>.
                mIntersectionType = IT_OTHER;
                mIntersectionArc.Center = mArc0->Center;
                mIntersectionArc.Radius = mArc0->Radius;
                mIntersectionArc.End0 = mArc1->End0;
                mIntersectionArc.End1 = mArc0->End1;
            }
            else
            {
                // Arc0 and arc1 share end point, <A0,B0,A1,B1>, B0 = A1.
                mIntersectionType = IT_POINT;
                mQuantity = 1;
                mPoint[0] = mArc1->End0;
            }
            return true;
        }

        if (mArc0->Contains(mArc1->End0))
        {
            // Arc1 inside arc0, <A0,B0,B1,A1>.
            mIntersectionType = IT_OTHER;
            mIntersectionArc = *mArc1;
            return true;
        }
        else
        {
            // Arcs do not overlap, <A0,A1,B0,B1>.
            mIntersectionType = IT_EMPTY;
            return false;
        }
    }

    // Test whether circle-circle intersection points are on the arcs.
    for (int i = 0; i < intr.GetQuantity(); ++i)
    {
        if (mArc0->Contains(intr.GetPoint(i))
        &&  mArc1->Contains(intr.GetPoint(i)))
        {
            mPoint[mQuantity++] = intr.GetPoint(i);
        }
    }

    mIntersectionType = (mQuantity > 0 ? IT_POINT : IT_EMPTY);
    return mIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrArc2Arc2<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector2<Real>& IntrArc2Arc2<Real>::GetPoint (int i) const
{
    return mPoint[i];
}
//----------------------------------------------------------------------------
template <typename Real>
const Arc2<Real>& IntrArc2Arc2<Real>::GetIntersectionArc () const
{
    return mIntersectionArc;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrArc2Arc2<float>;

template WM5_MATHEMATICS_ITEM
class IntrArc2Arc2<double>;
//----------------------------------------------------------------------------
}
