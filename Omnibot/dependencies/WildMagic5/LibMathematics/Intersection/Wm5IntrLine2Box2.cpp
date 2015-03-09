// Geometric Tools, LLC
// Copyright (c) 1998-2012
// Distributed under the Boost Software License, Version 1.0.
// http://www.boost.org/LICENSE_1_0.txt
// http://www.geometrictools.com/License/Boost/LICENSE_1_0.txt
//
// File Version: 5.0.1 (2010/10/01)

#include "Wm5MathematicsPCH.h"
#include "Wm5IntrLine2Box2.h"

namespace Wm5
{
//----------------------------------------------------------------------------
template <typename Real>
IntrLine2Box2<Real>::IntrLine2Box2 (const Line2<Real>& line,
    const Box2<Real>& box)
    :
    mLine(&line),
    mBox(&box)
{
}
//----------------------------------------------------------------------------
template <typename Real>
const Line2<Real>& IntrLine2Box2<Real>::GetLine () const
{
    return *mLine;
}
//----------------------------------------------------------------------------
template <typename Real>
const Box2<Real>& IntrLine2Box2<Real>::GetBox () const
{
    return *mBox;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrLine2Box2<Real>::Test ()
{
    Vector2<Real> diff = mLine->Origin - mBox->Center;
    Vector2<Real> perp = mLine->Direction.Perp();
    Real LHS = Math<Real>::FAbs(perp.Dot(diff));
    Real part0 = Math<Real>::FAbs(perp.Dot(mBox->Axis[0]));
    Real part1 = Math<Real>::FAbs(perp.Dot(mBox->Axis[1]));
    Real RHS = mBox->Extent[0]*part0 + mBox->Extent[1]*part1;
    return LHS <= RHS;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrLine2Box2<Real>::Find ()
{
    Real t0 = -Math<Real>::MAX_REAL;
    Real t1 = Math<Real>::MAX_REAL;
    return DoClipping(t0, t1, mLine->Origin, mLine->Direction, *mBox,
        true, mQuantity, mPoint, mIntersectionType);
}
//----------------------------------------------------------------------------
template <typename Real>
int IntrLine2Box2<Real>::GetQuantity () const
{
    return mQuantity;
}
//----------------------------------------------------------------------------
template <typename Real>
const Vector2<Real>& IntrLine2Box2<Real>::GetPoint (int i) const
{
    return mPoint[i];
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrLine2Box2<Real>::DoClipping (Real t0, Real t1,
    const Vector2<Real>& origin, const Vector2<Real>& direction,
    const Box2<Real>& box, bool solid, int& quantity, Vector2<Real> point[2],
    int& intrType)
{
    // Convert linear component to box coordinates.
    Vector2<Real> diff = origin - box.Center;
    Vector2<Real> BOrigin(
        diff.Dot(box.Axis[0]),
        diff.Dot(box.Axis[1])
    );
    Vector2<Real> BDirection(
        direction.Dot(box.Axis[0]),
        direction.Dot(box.Axis[1])
    );

    Real saveT0 = t0, saveT1 = t1;
    bool notAllClipped =
        Clip(+BDirection.X(), -BOrigin.X()-box.Extent[0], t0, t1) &&
        Clip(-BDirection.X(), +BOrigin.X()-box.Extent[0], t0, t1) &&
        Clip(+BDirection.Y(), -BOrigin.Y()-box.Extent[1], t0, t1) &&
        Clip(-BDirection.Y(), +BOrigin.Y()-box.Extent[1], t0, t1);

    if (notAllClipped && (solid || t0 != saveT0 || t1 != saveT1))
    {
        if (t1 > t0)
        {
            intrType = IT_SEGMENT;
            quantity = 2;
            point[0] = origin + t0*direction;
            point[1] = origin + t1*direction;
        }
        else
        {
            intrType = IT_POINT;
            quantity = 1;
            point[0] = origin + t0*direction;
        }
    }
    else
    {
        intrType = IT_EMPTY;
        quantity = 0;
    }

    return intrType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <typename Real>
bool IntrLine2Box2<Real>::Clip (Real denom, Real numer, Real& t0, Real& t1)
{
    // Return value is 'true' if line segment intersects the current test
    // plane.  Otherwise 'false' is returned in which case the line segment
    // is entirely clipped.

    if (denom > (Real)0)
    {
        if (numer > denom*t1)
        {
            return false;
        }
        if (numer > denom*t0)
        {
            t0 = numer/denom;
        }
        return true;
    }
    else if (denom < (Real)0)
    {
        if (numer > denom*t0)
        {
            return false;
        }
        if (numer > denom*t1)
        {
            t1 = numer/denom;
        }
        return true;
    }
    else
    {
        return numer <= (Real)0;
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Explicit instantiation.
//----------------------------------------------------------------------------
template WM5_MATHEMATICS_ITEM
class IntrLine2Box2<float>;

template WM5_MATHEMATICS_ITEM
class IntrLine2Box2<double>;
//----------------------------------------------------------------------------
}
