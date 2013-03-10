// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrRay2Circle2.h"
#include "Wm5IntrLine2Circle2.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrRay2Circle2<Real>::IntrRay2Circle2 (const Ray2<Real>& ray,
    const Circle2<Real>& circle)
    :
    mRay(&ray),
    mCircle(&circle)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Ray2<Real>& IntrRay2Circle2<Real>::GetRay () const
{
    return *mRay;
}
//----------------------------------------------------------------------------
template <typename Real>
const Circle2<Real>& IntrRay2Circle2<Real>::GetCircle () const
{
    return *mCircle;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrRay2Circle2<Real>::Find ()
{
    Real t[2];
    bool intersects = IntrLine2Circle2<Real>::Find(mRay->Origin,
        mRay->Direction, mCircle->Center, mCircle->Radius, mQuantity, t);

    if (intersects)
    {
        // Reduce root count if line-circle intersections are not on ray.
        if (mQuantity == 1)
        {
            if (t[0] < (Real)0)
            {
                mQuantity = 0;
            }
        }
        else
        {
            if (t[1] < (Real)0)
            {
                mQuantity = 0;
            }
            else if (t[0] < (Real)0)
            {
                mQuantity = 1;
                t[0] = t[1];
            }
        }

        for (int i = 0; i < mQuantity; ++i)
        {
            mPoint[i] = mRay->Origin + t[i]*mRay->Direction;
        }
    }

    mIntersectionType = (mQuantity > 0 ? IT_POINT : IT_EMPTY);
    return mIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrRay2Circle2<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector2<Real>& IntrRay2Circle2<Real>::GetPoint (int i) const
{
    return mPoint[i];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrRay2Circle2<float>;

template WM5_MATHEMATICS_ITEM
class IntrRay2Circle2<double>;
//----------------------------------------------------------------------------
}
