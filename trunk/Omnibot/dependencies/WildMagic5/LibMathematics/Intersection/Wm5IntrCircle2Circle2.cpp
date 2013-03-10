// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrCircle2Circle2.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrCircle2Circle2<Real>::IntrCircle2Circle2 (const Circle2<Real>& circle0,
    const Circle2<Real>& circle1)
    :
    mCircle0(&circle0),
    mCircle1(&circle1)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Circle2<Real>& IntrCircle2Circle2<Real>::GetCircle0 () const
{
    return *mCircle0;
}
//----------------------------------------------------------------------------
template <typename Real>
const Circle2<Real>& IntrCircle2Circle2<Real>::GetCircle1 () const
{
    return *mCircle1;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrCircle2Circle2<Real>::Find ()
{
    // The two circles are |X-C0| = R0 and |X-C1| = R1.  Define U = C1 - C0
    // and V = Perp(U) where Perp(x,y) = (y,-x).  Note that Dot(U,V) = 0 and
    // |V|^2 = |U|^2.  The intersection points X can be written in the form
    // X = C0+s*U+t*V and X = C1+(s-1)*U+t*V.  Squaring the circle equations
    // and substituting these formulas into them yields
    //   R0^2 = (s^2 + t^2)*|U|^2
    //   R1^2 = ((s-1)^2 + t^2)*|U|^2.
    // Subtracting and solving for s yields
    //   s = ((R0^2-R1^2)/|U|^2 + 1)/2
    // Then replace in the first equation and solve for t^2
    //   t^2 = (R0^2/|U|^2) - s^2.
    // In order for there to be solutions, the right-hand side must be
    // nonnegative.  Some algebra leads to the condition for existence of
    // solutions,
    //   (|U|^2 - (R0+R1)^2)*(|U|^2 - (R0-R1)^2) <= 0.
    // This reduces to
    //   |R0-R1| <= |U| <= |R0+R1|.
    // If |U| = |R0-R1|, then the circles are side-by-side and just tangent.
    // If |U| = |R0+R1|, then the circles are nested and just tangent.
    // If |R0-R1| < |U| < |R0+R1|, then the two circles to intersect in two
    // points.

    Vector2<Real> U = mCircle1->Center - mCircle0->Center;
    Real USqrLen = U.SquaredLength();
    Real R0 = mCircle0->Radius, R1 = mCircle1->Radius;
    Real R0mR1 = R0 - R1;
    if (USqrLen < Math<Real>::ZERO_TOLERANCE
    &&  Math<Real>::FAbs(R0mR1) < Math<Real>::ZERO_TOLERANCE)
    {
        // Circles are essentially the same.
        mIntersectionType = IT_OTHER;
        mQuantity = 0;
        return true;
    }

    Real R0mR1Sqr = R0mR1*R0mR1;
    if (USqrLen < R0mR1Sqr)
    {
        mIntersectionType = IT_EMPTY;
        mQuantity = 0;
        return false;
    }

    Real R0pR1 = R0 + R1;
    Real R0pR1Sqr = R0pR1*R0pR1;
    if (USqrLen > R0pR1Sqr)
    {
        mIntersectionType = IT_EMPTY;
        mQuantity = 0;
        return false;
    }

    if (USqrLen < R0pR1Sqr)
    {
        if (R0mR1Sqr < USqrLen)
        {
            Real invUSqrLen = ((Real)1)/USqrLen;
            Real s = ((Real)0.5)*((R0*R0-R1*R1)*invUSqrLen+(Real)1);
            Vector2<Real> tmp = mCircle0->Center + s*U;

            // In theory, discr is nonnegative.  However, numerical round-off
            // errors can make it slightly negative.  Clamp it to zero.
            Real discr = R0*R0*invUSqrLen - s*s;
            if (discr < (Real)0)
            {
                discr = (Real)0;
            }
            Real t = Math<Real>::Sqrt(discr);
            Vector2<Real> V(U.Y(), -U.X());
            mQuantity = 2;
            mPoint[0] = tmp - t*V;
            mPoint[1] = tmp + t*V;
        }
        else
        {
            // |U| = |R0-R1|, circles are tangent.
            mQuantity = 1;
            mPoint[0] = mCircle0->Center + (R0/R0mR1)*U;
        }
    }
    else
    {
        // |U| = |R0+R1|, circles are tangent.
        mQuantity = 1;
        mPoint[0] = mCircle0->Center + (R0/R0pR1)*U;
    }

    mIntersectionType = IT_POINT;
    return true;
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrCircle2Circle2<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector2<Real>& IntrCircle2Circle2<Real>::GetPoint (int i) const
{
    return mPoint[i];
}
//----------------------------------------------------------------------------
template <typename Real>
const Circle2<Real>& IntrCircle2Circle2<Real>::GetIntersectionCircle () const
{
    return *mCircle0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrCircle2Circle2<float>;

template WM5_MATHEMATICS_ITEM
class IntrCircle2Circle2<double>;
//----------------------------------------------------------------------------
}
