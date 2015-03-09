// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrLine2Circle2.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrLine2Circle2<Real>::IntrLine2Circle2 (const Line2<Real>& line,
    const Circle2<Real>& circle)
    :
    mLine(&line),
    mCircle(&circle)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Line2<Real>& IntrLine2Circle2<Real>::GetLine () const
{
    return *mLine;
}
//----------------------------------------------------------------------------
template <typename Real>
const Circle2<Real>& IntrLine2Circle2<Real>::GetCircle () const
{
    return *mCircle;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrLine2Circle2<Real>::Find ()
{
    Real t[2];
    bool intersects = Find(mLine->Origin, mLine->Direction,
        mCircle->Center, mCircle->Radius, mQuantity, t);

    if (intersects)
    {
        for (int i = 0; i < mQuantity; ++i)
        {
            mPoint[i] = mLine->Origin + t[i]*mLine->Direction;
        }
    }

    mIntersectionType = (mQuantity > 0 ? IT_POINT : IT_EMPTY);
    return mIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrLine2Circle2<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector2<Real>& IntrLine2Circle2<Real>::GetPoint (int i) const
{
    return mPoint[i];
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrLine2Circle2<Real>::Find (const Vector2<Real>& origin,
    const Vector2<Real>& direction, const Vector2<Real>& center,
    Real radius, int& rootCount, Real t[2])
{
    // Intersection of a the line P+t*D and the circle |X-C| = R.  The line
    // direction is unit length. The t value is a root to the quadratic
    // equation:
    //   0 = |t*D+P-C|^2 - R^2
    //     = t^2 + 2*Dot(D,P-C)*t + |P-C|^2-R^2
    //     = t^2 + 2*a1*t + a0
    // If two roots are returned, the order is T[0] < T[1].

    Vector2<Real> diff = origin - center;
    Real a0 = diff.SquaredLength() - radius*radius;
    Real a1 = direction.Dot(diff);
    Real discr = a1*a1 - a0;
    if (discr > Math<Real>::ZERO_TOLERANCE)
    {
        rootCount = 2;
        discr = Math<Real>::Sqrt(discr);
        t[0] = -a1 - discr;
        t[1] = -a1 + discr;
    }
    else if (discr < -Math<Real>::ZERO_TOLERANCE)
    {
        rootCount = 0;
    }
    else  // discr == 0
    {
        rootCount = 1;
        t[0] = -a1;
    }

    return rootCount != 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrLine2Circle2<float>;

template WM5_MATHEMATICS_ITEM
class IntrLine2Circle2<double>;
//----------------------------------------------------------------------------
}
